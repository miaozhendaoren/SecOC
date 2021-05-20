#include "i2c.h"
#include "slaveFVM.h"
#include "MacGenerate.h"
#include "usart.h"
#include "fvm_conf.h"
#include "Csm.h"
#include "masterFVM.h"

// #define ADDR_24LCxx_Write 0xA0
// #define ADDR_24LCxx_Read 0xA1


uint16 ackvid = 0x322; // ecu返回的ack确认报文  可配置
uint8 verifystate = 0; // verifystate确认状态 0表示未确认 1表示确认
uint16 notifyid = 0x386; // ecu返回的通知报文  可配置
uint8 secocenabled = 0;     //secoc可工作  0表示不行 1表示可以
uint8 errorid = 0x64; // 出错

/**
 * 根据id匹配不同报文
 * 1.ack确认报文表示 master收到自己的ack报文，修改标记在后续收到trip同步报文不返回ack报文
 * 2.通知报文送达则secoc功能可执行。。
 * 3.错误报文表面trip同步失败
*/
FUNC(void, SLAVE_CODE)
FVM_changestate(VAR(PduInfoType, COMSTACK_TYPES_VAR) RxPduId) {
    if (RxPduId == ackvid) // ack确认报文
    {
        verifystate = 1; // 状态验证
    } else if (RxPduId == notifyid) // 通知报文
    {
        secocenabled = 1; // 不可工作
    } else if (RxPduId == errorid) // 错误
    {
        // pass
    }
}

/**
 * 	周期性调用查看reset同步消息有没有丢失达到上限
	若达到上限发送报错报文，内容为报错的resetid
*/
FUNC(void, SLAVE_CODE)
FVM_Syn_check(void) {
    // 判断secocenabled == 1 可用
    if (secocenabled != 1) return;
    for (i = 0; i < NUM_RESET; i++) {  //占2字节
        resetState[i].resetTag++;
        //uart_send_byte(synstag[i].syntag);
        if (resetState[i].resetTag == resetState[i].resetTime) {
            if (resetState[i].resetflag == 0)    //未收到同步消息,则丢失消息次数+1
            {
                resetState[i].resetloss += 1;
            }
            if (resetState[i].resetloss == SYN_ERROR_SIZE) {
                errorvalue[0] = 0x01;
                errorvalue[1] = resetcanid[i] / 255;
                errorvalue[2] = resetcanid[i] % 255;

                memcpy(info->SduDataPtr, errorvalue, 3);
                info->SduLength = 8;
                CanIf_Transmiterrorcanid, info);
            }
            resetState[i].resetTag = 0;
            resetState[i].resetflag = 0;
        }
    }
}


uint16 tripcanid = 0x2bd; //可配置
uint8 trip[3]; // trip报文
uint8 TripCntLength = 16; //可配置
uint16 ackid = 0x2be; //返回的ack报文  可配置

/**
 * 更新trip报文
 * 1.若收到过ack确认报文证明自己的ack被Master接收，直接结束
 * 
 * 2.解析收到的trip报文，重新构造验证mac所需的原始数据dataptr
 * PduInfoPtr.data = trip(TripCntLength) + reset(1bit)+ mac(64bit-TripCntLength-1);
 * char *dataptr 组成为  tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLength)+reset(1bit)：值为1 + padding0;
 * 在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
 * 例如  TripCntLength=12  [ 0x02 0xbd 0x00 0x14]  [0000 0100 1011 1101 0000 0000 0001 0100]
 * 
 * 3.调用Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,uint8* verifyPtr);
 * 若验证通过，使用verifyPtr(trip(TripCntLength)+1(1bit))更新trip值和reset值，
 * info->SduDataPtr = NULL;
 * info->SduLength=8;
 * 并调用CanIf_Transmit(ackid, info); 
 * 
 * 4.验证失败不更新
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    // 确认接收
    if (verifystate == 1) return E_OK;

    uint8 data = *PduInfoPtr->SduDataPtr;

    // trip
    for (int i = 1; i <= TripCntLength; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    // reset
    if (is_k(data, TripCntLength + 1))
        trip[2] = set_k(trip[2], 1);

    // mac
    uint8 *mac;
    uint32 macLength = 64 - TripCntLength - 1;
    for (int i = TripCntLength + 1; i < 64; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    char dataptr[4];
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid, i))
            data[0] = set_k(data[0], i);
    }
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid >> 8, i))
            data[1] = set_k(data[1], i);
    }

    // Csm验证
    if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK) return E_NOT_OK;

    verifyPtr(TripCntLength + 1);
    PduInfoPtr->SduDataPtr = NULL;
    PduInfoPtr->SduLength = 8;
    CanIf_Transmit(ackid, PduInfoPtr);

    return E_OK;
}


uint16 error_id = 0x100; //报错报文id   可配置

/**
 * 更新reset报文
 * 1.先判断TxPduId 若>=NUM_RESET， 则直接退出;
 * 
 * 2.解析收到的reset报文，重新构造验证mac所需的原始数据dataptr
 * PduInfoPtr.data = reset(ResetCntLength)+ mac(64bit-ResetCntLength);
 * char *dataptr 组成为
 * resetcanid[2](reset拆分成两个char, 由TxPduId作为索引找到)+trip(TripCntLength)+reset(ResetCntLength) + padding0;
 * 在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
 * 
 * 3.调用Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,uint8* verifyPtr);
 * 若验证通过，使用verifyPtr（trip(TripCntLength)+1(1bit)）更新trip值和reset值
 * 
 * 4.验证失败不更新 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    if (NUM_RESET <= TxPduId) return E_NOT_OK;

    uint8 data = *PduInfoPtr->SduDataPtr;

    // trip
    for (int i = 1; i <= TripCntLength; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    // reset
    if (is_k(data, TripCntLength + 1))
        trip[2] = set_k(trip[2], 1);

    // mac
    uint8 *mac;
    uint32 macLength = 64 - TripCntLength - 1;
    for (int i = TripCntLength + 1; i < 64; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    char dataptr[4];
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid, i))
            data[0] = set_k(data[0], i);
    }
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid >> 8, i))
            data[1] = set_k(data[1], i);
    }

    // Csm验证
    if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK) return E_NOT_OK;

    verifyPtr(TripCntLength + 1);
    PduInfoPtr->SduDataPtr = NULL;
    PduInfoPtr->SduLength = 8;

    return E_OK;
}

//下面两个函数是sender slave的部分

/**
 * 获取新鲜值及裁剪新鲜值
 * 1.根据SecOCFreshnessValueID 作为索引找到需要对应的 resetCnt[id], preTrip, msgCnt[]
 * 
 * 2.通过比较各计数器上一次发送值和当前值，构造新鲜值
 *  参考图
 * 
 * 3.将新鲜值按照 trip reset msg 进行构造 SecOCFreshnessValue
 * 
 * 4.根据 SecOCTruncatedFreshnessValueLength 长度，截取msg中后 (SecOCTruncatedFreshnessValueLength-2) 比特位长度 + 2 比特位reset flag（reset后两位）构造SecOCTruncatedFreshnessValue
 * 参考图
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
) {
    ResetCnt_Type current_reset = resetCnt[SecOCFreshnessValueID];
    ResetState_Type current_resetState = resetState[SecOCFreshnessValueID];
    MsgCntS_Type current_msg = msgCnt[SecOCFreshnessValueID];
    // 连接值并判断
    // 相同，则把 message counter +1 ，reserflag 使用当前 flag，
    uint8 can_data[8] = 0;
    uint64 *can_data64 = can_data
    uint16 data_reset_num;
    uint16 data_trip;

    data_trip = trip[0];
    data_trip <<= 8;
    data_trip += trip[1];
    data_trip <<= (16 - tripCntLength);
    // 这里的拼接有问题，得手动拼一下
    if((trip | current_reset.resetdata ) == ( pretrip | current_reset.preresetdata)){
        can_data64 |= (uint64)current_reset.prereserData << (64 - 16 - 1 );
        can_data64 |= (uint64)current_msg.premsgdata << (64 - 16 - current_reset.ResetCntLength - 1);
        can_data64 |= current_resetState.resetflag
    } else {
        can_data64 |= (uint64)current_reset.reserData << (64-16-1);
        can_data64 |= (uint64)(initValue+1) <<(64 - 16 - current_reset.ResetCntLength - 1);
        can_data64 |= current_resetState.resetflag
    }
    SecOCFreshnessValue = can_data
    return E_OK;
}

//根据 SecOCTruncatedFreshnessValueLength 长度，截取msg中后 (SecOCTruncatedFreshnessValueLength-2) 比特位长度 + 2 比特位reset flag（reset后两位）构造SecOCTruncatedFreshnessValue
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FvM_GetTxFreshnessTruncData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength
) {
    ResetCnt_Type current_reset = resetCnt[SecOCFreshnessValueID];
    ResetState_Type current_resetState = resetState[SecOCFreshnessValueID];
    MsgCntS_Type current_msg = msgCnt[SecOCFreshnessValueID];

    uint64 result;
    //截取msg中后 (SecOCTruncatedFreshnessValueLength-2) 比特位长度
    (current_msg.msgdata << (current_msg.MsgCntLength - (SecOCTruncatedFreshnessValueLength - 2)))>>(current_msg.MsgCntLength - (SecOCTruncatedFreshnessValueLength - 2))
    //移位给resetflag
    current_msg.msgdata << 2;
    current_msg.msgdata |= current_resetState.resetflag;
    SecOCTruncatedFreshnessValue = current_msg.msgdata;
    return E_OK;
}


//下面两个函数是receiver slave的部分

/**
 * 1.若 SecOCTruncatedFreshnessValueLength == SecOCFreshnessValueLength 将 SecOCTruncatedFreshnessValue 复制到 SecOCFreshnessValue
 * 2.根据SecOCAuthVerifyAttempts依次比较表格中不同的format：
	依次比较resetflag（根据SecOCAuthVerifyAttempts值）: 
	    若不满足条件,SecOCAuthVerifyAttempts+1, 重新进行2.1, 若SecOCAuthVerifyAttempts=5了退出
		若满足，比较reset（resetData 和preresetData）：
			若构造失败， SecOCAuthVerifyAttempts+1，重新进行2.1， 若 SecOCAuthVerifyAttempts=5 了退出
			比较msg（其中lower低位为 （SecOCTruncatedFreshnessValueLength-2）  高位为  MsgCntLength-（SecOCTruncatedFreshnessValueLength-2））
			构造出SecOCFreshnessValue，SecOCAuthVerifyAttempts+1 并返回;
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshness(
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCTruncatedFreshnessValue,
        VAR(uint32, FRESH_VAR) SecOCTruncatedFreshnessValueLength,
        VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
) {
    if(*SecOCTruncatedFreshnessValueLength==*SecOCFreshnessValueLength){
        memcopy(SecOCFreshnessValueLength,SecOCTruncatedFreshnessValue,SecOCTruncatedFreshnessValueLength);
    }
    uint8 latest_reset_flag,revevied_reset_flag,result_reset_flag;
    uint8 latest_trip[3],pre_rx_trip[3];
    uint8 lasest_msg,rx_msg;
    for(;SecOCAuthVerifyAttempts<=5;SecOCAuthVerifyAttempts++){

    }
    {
        if ((resetArray[i].reset[1] & 0x03) == (rxfv & 0x03)) {		//第1种情况 Latest value=Received value
            if (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && resetArray[i].reset[1] == preRxArr[j].pre_rx_reset[1]) {
                constructedfv[0] = preRxArr[j].pre_rx_trip[0];
                constructedfv[1] = preRxArr[j].pre_rx_trip[1];
                constructedfv[2] = preRxArr[j].pre_rx_reset[0];
                constructedfv[3] = preRxArr[j].pre_rx_reset[1];
                constructedfv[6] = rxfv & 0x03;
                if (preRxArr[j].pre_rx_msgL < (rxfv >> 2)) {
                    constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                    constructedfv[5] = (preRxArr[j].pre_rx_msgH[1] & 0xc0) + (rxfv >> 2);		//保存高位的最后两位，与低位RX value
                }
                else {
                    if (preRxArr[j].pre_rx_msgH[0] == 255 && (preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//可能修改，当高位msg满时，高低位全赋最大值
                        constructedfv[4] = 0xff;
                        constructedfv[5] = 0xff;
                    }
                    else {
                        if ((preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//0xc0 二进制是11000000，高10位中的后两位进位
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0] + 1;
                            constructedfv[5] = rxfv >> 2;		//0x3f 二进制是00111111，进位后前两位变0，前一字节+1，后面存进位后的低位
                        }
                        else {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                            constructedfv[5] = (((preRxArr[j].pre_rx_msgH[1] >> 6) + 1) << 6) + (rxfv >> 2);		//高位+1，低位赋 Rx value
                        }
                    }
                }
            }
            if (resetArray[i].reset[0] > preRxArr[j].pre_rx_reset[0]			//1:前高位大于后高位	2:高位相等，前低位大于后低位
                || (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && resetArray[i].reset[1] > preRxArr[j].pre_rx_reset[1])) {
                constructedfv[0] = trip[0];
                constructedfv[1] = trip[1];
                constructedfv[2] = resetArray[i].reset[0];
                constructedfv[3] = resetArray[i].reset[1];
                constructedfv[4] = 0;
                constructedfv[5] = rxfv >> 2;
                constructedfv[6] = rxfv & 0x03;
            }
            return constructedfv;
        }


        if (((resetArray[i].reset[1] & 0x03) - 1) == (rxfv & 0x03)		//(0,3) (1,0) (2,1) (3,2)
            ||((resetArray[i].reset[1] & 0x03)==0&&((rxfv & 0x03)==3))) {		//第2种情况 Latest value-1=Received value	1:不发生退位	2：发生退位
            if ((resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && (resetArray[i].reset[1] > preRxArr[j].pre_rx_reset[1]) && (resetArray[i].reset[1] - 1) == preRxArr[j].pre_rx_reset[1])		//两种情况，1：高位相等，低位-1后相等；（避免高位相等，但低位0-1=255，导致错误匹配）2：高位比pre高一位，但减一后降为，即低位为0与255时
                || (resetArray[i].reset[0] == (preRxArr[j].pre_rx_reset[0] + 1) && resetArray[i].reset[1] == 0 && preRxArr[j].pre_rx_reset[1] == 255)) {		//Latest value-1==pre Rx value
                constructedfv[0] = preRxArr[j].pre_rx_trip[0];
                constructedfv[1] = preRxArr[j].pre_rx_trip[1];
                constructedfv[2] = preRxArr[j].pre_rx_reset[0];
                constructedfv[3] = preRxArr[j].pre_rx_reset[1];
                constructedfv[6] = rxfv & 0x03;
                if (preRxArr[j].pre_rx_msgL < (rxfv >> 2)) {		//Pre rx value<Rx value(no carry)
                    constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                    constructedfv[5] = (preRxArr[j].pre_rx_msgH[1] & 0xc0) + (rxfv >> 2);
                }
                else {		//Pre rx value>=Rx value(with carry)
                    if (preRxArr[j].pre_rx_msgH[0] == 255 && (preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//可能修改，当高位msg满时，高低位全赋最大值
                        constructedfv[4] = 0xff;
                        constructedfv[5] = 0xff;
                    }
                    else {
                        if ((preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0] + 1;
                            constructedfv[5] = rxfv >> 2;
                        }
                        else {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                            constructedfv[5] = (((preRxArr[j].pre_rx_msgH[1] >> 6) + 1) << 6) + (rxfv >> 2);		//高位+1，低位赋 Rx value
                        }
                    }
                }
            }
            if ((resetArray[i].reset[0] > (preRxArr[j].pre_rx_reset[0] + 1))		//1：前高位比后高位高大于1；	2：前高位与后高位差1，但防止进位的情况；	3：高位相等时，进行比较，但防止出现进位情况
                || ((resetArray[i].reset[0] == (preRxArr[j].pre_rx_reset[0] + 1)) && !(resetArray[i].reset[1] == 0 && preRxArr[j].pre_rx_reset[1] == 255))
                || (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && preRxArr[j].pre_rx_reset[1] != 255 && resetArray[i].reset[1] > (preRxArr[j].pre_rx_reset[1]) + 1)) {		//Latest value-1>pre Rx value
                constructedfv[0] = trip[0];
                constructedfv[1] = trip[1];
                /*if (resetArray[i].reset[0] == 0 && resetArray[i].reset[1] == 0) {		//理论上不会发生，reset不会为0
                    constructedfv[2] = 0;
                    constructedfv[3] = 1;
                }*/
                /*if (resetArray[i].reset[0] == 0 && resetArray[i].reset[1] == 1) {		//可能修改，这里是当reset已经为1时（即到达边界值，赋值1），无法再减，使用原边界值
                    constructedfv[2] = resetArray[i].reset[0];
                    constructedfv[3] = resetArray[i].reset[1];
                }*/
                if (resetArray[i].reset[1] == 0) {			//reset高位不是0，低位是0
                    constructedfv[2] = resetArray[i].reset[0] - 1;
                    constructedfv[3] = 255;
                }
                else {
                    constructedfv[2] = resetArray[i].reset[0];
                    constructedfv[3] = resetArray[i].reset[1] - 1;
                }

                constructedfv[4] = 0;
                constructedfv[5] = rxfv >> 2;
                constructedfv[6] = rxfv & 0x03;
            }
            return constructedfv;
        }


        if (((resetArray[i].reset[1] & 0x03) + 1) == (rxfv & 0x03)		//(0,1) (1,2) (2,3) (3,0)
            ||((resetArray[i].reset[1] & 0x03)==3&&(rxfv & 0x03)==0)) {		//第3种情况  Latest value+1=Received value	1：不发生进位	2：发生进位
            if (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && resetArray[i].reset[1] != 255 && (resetArray[i].reset[1] + 1) == preRxArr[j].pre_rx_reset[1]		//1:高位相等，低位相加可相等，但防止进位；	2：高位差1，进位可相等
                || (resetArray[i].reset[0] + 1) == preRxArr[j].pre_rx_reset[0] && resetArray[i].reset[1] == 255 && preRxArr[j].pre_rx_reset[1] == 0) {		//Latest value+1==pre Rx value
                constructedfv[0] = preRxArr[j].pre_rx_trip[0];
                constructedfv[1] = preRxArr[j].pre_rx_trip[1];
                constructedfv[2] = preRxArr[j].pre_rx_reset[0];
                constructedfv[3] = preRxArr[j].pre_rx_reset[1];
                constructedfv[6] = rxfv & 0x03;
                if (preRxArr[j].pre_rx_msgL < (rxfv >> 2)) {		//Pre rx value<Rx value(no carry)
                    constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                    constructedfv[5] = (preRxArr[j].pre_rx_msgH[1] & 0xc0) + (rxfv >> 2);
                }
                else {		//Pre rx value>=Rx value(with carry)
                    if (preRxArr[j].pre_rx_msgH[0] == 255 && (preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//可能修改，当高位msg满时，高低位全赋最大值
                        constructedfv[4] = 0xff;
                        constructedfv[5] = 0xff;
                    }
                    else {
                        if ((preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0] + 1;
                            constructedfv[5] = rxfv >> 2;
                        }
                        else {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                            constructedfv[5] = (((preRxArr[j].pre_rx_msgH[1] >> 6) + 1) << 6) + (rxfv >> 2);		//前两个二进制（高位）+1，低位赋 Rx value
                        }
                    }
                }
            }
            if ((resetArray[i].reset[0] > preRxArr[j].pre_rx_reset[0])	//1:高位大于高位；	2：高位相等，低位大于等于后低位，防止255进位出错
                ||(resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && (resetArray[i].reset[1] >= preRxArr[j].pre_rx_reset[1]))) {
                constructedfv[0] = trip[0];
                constructedfv[1] = trip[1];
                if (resetArray[i].reset[0] == 255 && resetArray[i].reset[1] == 255) {		//可能修改，当reset满值时，使用满值
                    constructedfv[2] = 255;
                    constructedfv[3] = 255;
                }
                else {
                    if (resetArray[i].reset[1] == 255) {
                        constructedfv[2] = resetArray[i].reset[0] + 1;
                        constructedfv[3] = 0;
                    }
                    else {
                        constructedfv[2] = resetArray[i].reset[0];
                        constructedfv[3] = resetArray[i].reset[1] + 1;
                    }
                }
                constructedfv[4] = 0;
                constructedfv[5] = rxfv >> 2;
                constructedfv[6] = rxfv & 0x03;
            }
            return constructedfv;
        }


        if ((((resetArray[i].reset[1] & 0x03) - 2) == (rxfv & 0x03)	//(0,2) (1,3) (2,0) (3,1)
             ||((resetArray[i].reset[1] & 0x03)==0&&(rxfv & 0x03)==2)
             ||((resetArray[i].reset[1] & 0x03)==1&&(rxfv & 0x03)==3))
            &&R_fv_flag==0) {		//第4种情况  Latest value-2=Received value并且R_fv_flag=0(表示未进入过第4中情况)	1:不发生退位	2,3：发生退位

            R_fv_flag=1;	//已经进入过该情况，将标志置为1

            if ((resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && (resetArray[i].reset[1] > 1) && (resetArray[i].reset[1]-2) == preRxArr[j].pre_rx_reset[1])		//1:高位相等，低位差2(不进位)	2:发生进位
                || ((resetArray[i].reset[0] - 1) == preRxArr[j].pre_rx_reset[0] && ((resetArray[i].reset[1] == 0 && preRxArr[j].pre_rx_reset[1] == 254) || (resetArray[i].reset[1] == 1 && preRxArr[j].pre_rx_reset[1] == 255)))) {
                constructedfv[0] = preRxArr[j].pre_rx_trip[0];
                constructedfv[1] = preRxArr[j].pre_rx_trip[1];
                constructedfv[2] = preRxArr[j].pre_rx_reset[0];
                constructedfv[3] = preRxArr[j].pre_rx_reset[1];
                constructedfv[6] = rxfv & 0x03;
                if (preRxArr[j].pre_rx_msgL < (rxfv >> 2)) {		//Pre rx value<Rx value(no carry)
                    constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                    constructedfv[5] = (preRxArr[j].pre_rx_msgH[1] & 0xc0) + (rxfv >> 2);
                }
                else {		//Pre rx value>=Rx value(with carry)
                    if (preRxArr[j].pre_rx_msgH[0] == 255 && (preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//可能修改，当高位msg满时，高低位全赋最大值
                        constructedfv[4] = 0xff;
                        constructedfv[5] = 0xff;
                    }
                    else {
                        if ((preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0] + 1;
                            constructedfv[5] = rxfv >> 2;
                        }
                        else {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                            constructedfv[5] = (((preRxArr[j].pre_rx_msgH[1] >> 6) + 1) << 6) + (rxfv >> 2);		//前两个二进制（高位）+1，低位赋 Rx value
                        }
                    }
                }
            }
            if ((resetArray[i].reset[0] > (preRxArr[j].pre_rx_reset[0] + 1))		//1:高位大于高位+1	2:高位等于高位+1，去掉特殊值	3:高位相等，减2后大于后面
                || (resetArray[i].reset[0] == (preRxArr[j].pre_rx_reset[0] + 1) && !((resetArray[i].reset[1] == 0 && preRxArr[j].pre_rx_reset[1] == 254) || (resetArray[i].reset[0] == 1 && preRxArr[j].pre_rx_reset[1] == 255)))
                || (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && preRxArr[j].pre_rx_reset[1]<254 && resetArray[i].reset[1]>(preRxArr[j].pre_rx_reset[1] + 2))) {
                constructedfv[0] = trip[0];
                constructedfv[1] = trip[1];
                /*if (resetArray[i].reset[0] == 0 && resetArray[i].reset[1] == 0) {		//理论上不会发生，reset不会为0
                    constructedfv[2] = 0;
                    constructedfv[3] = 1;
                }*/
                /*if ((resetArray[i].reset[0] == 0 && resetArray[i].reset[1] == 1)) {		//可能修改，这里是当reset已经为1时（到达边界值），无法再减，使用原值
                    constructedfv[2] = resetArray[i].reset[0];
                    constructedfv[3] = resetArray[i].reset[1];
                }*/
                if (resetArray[i].reset[1] == 0) {
                    constructedfv[2] = resetArray[i].reset[0] - 1;
                    constructedfv[3] = 254;
                }
                else if (resetArray[i].reset[1] == 1) {
                    constructedfv[2] = resetArray[i].reset[0] - 1;
                    constructedfv[3] = 255;
                }
                else {
                    constructedfv[2] = resetArray[i].reset[0];
                    constructedfv[3] = resetArray[i].reset[1] - 2;
                }

                constructedfv[4] = 0;
                constructedfv[5] = rxfv >> 2;
                constructedfv[6] = rxfv & 0x03;
            }
            return constructedfv;
        }

        if ((((resetArray[i].reset[1] & 0x03) + 2) == (rxfv & 0x03)		//(0,2) (1,3) (2,0) (3,1)
             ||((resetArray[i].reset[1] & 0x03)==2&&(rxfv & 0x03)==0)
             ||((resetArray[i].reset[1] & 0x03)==3&&(rxfv & 0x03)==1))
            &&R_fv_flag==1) {		//第5种情况  Latest value+2=Received value并且R_fv_flag=1(表示已经进入过情况4)	1:不发生进位	2,3：发生进位

            R_fv_flag=0;	//已经进入过情况5，将R_fv_flag置回0

            if ((resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && (resetArray[i].reset[1] < 254) && (resetArray[i].reset[1] + 2) == preRxArr[j].pre_rx_reset[1])
                || ((resetArray[i].reset[0] + 1) == preRxArr[j].pre_rx_reset[0] && ((resetArray[i].reset[1] == 254 && preRxArr[j].pre_rx_reset[1] == 0) || (resetArray[i].reset[1] == 255 && preRxArr[j].pre_rx_reset[1] == 1)))) {		//1:不发生进位时，高位相等，低位差2		2: 高位差1，发生进位
                constructedfv[0] = preRxArr[j].pre_rx_trip[0];
                constructedfv[1] = preRxArr[j].pre_rx_trip[1];
                constructedfv[2] = preRxArr[j].pre_rx_reset[0];
                constructedfv[3] = preRxArr[j].pre_rx_reset[1];
                constructedfv[6] = rxfv & 0x03;
                if (preRxArr[j].pre_rx_msgL < (rxfv >> 2)) {
                    constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                    constructedfv[5] = (preRxArr[j].pre_rx_msgH[1] & 0xc0) + (rxfv >> 2);
                }
                else {
                    if (preRxArr[j].pre_rx_msgH[0] == 255 && (preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {		//可能修改，当高位msg满时，高低位全赋最大值
                        constructedfv[4] = 0xff;
                        constructedfv[5] = 0xff;
                    }
                    else {
                        if ((preRxArr[j].pre_rx_msgH[1] & 0xc0) == 0xc0) {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0] + 1;
                            constructedfv[5] = rxfv >>2 ;
                        }
                        else {
                            constructedfv[4] = preRxArr[j].pre_rx_msgH[0];
                            constructedfv[5] = (((preRxArr[j].pre_rx_msgH[1] >> 6) + 1) << 6) + (rxfv >> 2);		//前两个二进制（高位）+1，低位赋 Rx value
                        }
                    }
                }
            }
            if ((resetArray[i].reset[0] > preRxArr[j].pre_rx_reset[0])		//1:高位大于高位	2:高位相等，相差超过2，防止出现进位错误 (只要保证前面的低位>=254,就可以)
                || (resetArray[i].reset[0] == preRxArr[j].pre_rx_reset[0] && ((resetArray[i].reset[1] + 2) > preRxArr[j].pre_rx_reset[1] || (resetArray[i].reset[1] >= 254)))) {
                constructedfv[0] = trip[0];
                constructedfv[1] = trip[1];
                if (resetArray[i].reset[0] == 255 && resetArray[i].reset[1] >= 254) {		//可能修改，reset接近最大值，无法再+2，赋值边界值255
                    constructedfv[2] = 255;
                    constructedfv[3] = 255;
                }
                else {
                    if (resetArray[i].reset[1] == 254) {
                        constructedfv[2] = resetArray[i].reset[0] + 1;
                        constructedfv[3] = 0;
                    }
                    else if (resetArray[i].reset[1] == 255) {
                        constructedfv[2] = resetArray[i].reset[0] + 1;
                        constructedfv[3] = 1;
                    }
                    else {
                        constructedfv[2] = resetArray[i].reset[0];
                        constructedfv[3] = resetArray[i].reset[1] + 2;
                    }
                }
                constructedfv[4] = 0;
                constructedfv[5] = rxfv >> 2;
                constructedfv[6] = rxfv & 0x03;
            }
            return constructedfv;
        }

        return NULL;
    }
}

FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshnessAuthData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCAuthDataFreshnessValue,
        VAR(uint16, FRESH_VAR) SecOCAuthDataFreshnessValueLength,
        VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
) {

}

/**
 * 根据完整SecOCFreshnessValue 更新
 * SecOCFreshnessValue包括trip(TripCntLength) reset(ResetCntLength) msg(MsgCntLength) resetflag(ResetFlagLength)
 * 由于长度是比特长度因此需要将各计数器的值分离出来，从而更新对应值：
 * preTrip[3*SecOCFreshnessValueID] = trip(TripCntLength)
 * resetCnt[SecOCFreshnessValueID] -> preresetData[3*SecOCFreshnessValueID] = reset(ResetCntLength)
 * msgCnt[SecOCFreshnessValueID] -> premsgdata[3*SecOCFreshnessValueID] =msg(MsgCntLength)
*/
//需保证报文没被取消
FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                   P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    current_reset = resetCnt[TxPduId];

    uint8 data = *PduInfoPtr->SduDataPtr;

    // trip
    for (int i = 1; i <= TripCntLength; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    // reset
    if (is_k(data, TripCntLength + 1))
        trip[2] = set_k(trip[2], 1);

    // mac
    uint8 *mac;
    uint32 macLength = 64 - TripCntLength - 1;
    for (int i = TripCntLength + 1; i < 64; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    char dataptr[4];
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid, i))
            data[0] = set_k(data[0], i);
    }
    for (int i = 1; i <= 8; i++) {
        if (is_k(tripcanid >> 8, i))
            data[1] = set_k(data[1], i);
    }




    return E_OK;
}







