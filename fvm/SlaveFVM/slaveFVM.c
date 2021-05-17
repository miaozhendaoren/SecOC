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
uint8 TripCntLengthgth = 16; //可配置
uint16 ackid = 0x2be; //返回的ack报文  可配置

/**
 * 更新trip报文
 * 1.若收到过ack确认报文证明自己的ack被Master接收，直接结束
 * 
 * 2.解析收到的trip报文，重新构造验证mac所需的原始数据dataptr
 * PduInfoPtr.data = trip(TripCntLengthgth) + reset(1bit)+ mac(64bit-TripCntLengthgth-1);
 * char *dataptr 组成为  tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLengthgth)+reset(1bit)：值为1 + padding0;
 * 在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
 * 例如  TripCntLengthgth=12  [ 0x02 0xbd 0x00 0x14]  [0000 0100 1011 1101 0000 0000 0001 0100]
 * 
 * 3.调用Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,uint8* verifyPtr);
 * 若验证通过，使用verifyPtr(trip(TripCntLengthgth)+1(1bit))更新trip值和reset值，
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

    // 获取trip报文
    uint8 data = *PduInfoPtr->SduDataPtr;

    // trip
    for (int i = 1; i <= TripCntLengthgth; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    // reset
    if (is_k(data, TripCntLengthgth + 1))
        trip[2] = set_k(trip[2], 1);

    // mac
    uint8 *mac;
    uint32 macLength = 64 - TripCntLengthgth - 1;
    for (int i = TripCntLengthgth + 1; i < 64; i++) {
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

    verifyPtr(TripCntLengthgth + 1);
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
 * PduInfoPtr.data = reset(ResetCntLengthgth)+ mac(64bit-ResetCntLengthgth);
 * char *dataptr 组成为
 * resetcanid[2](reset拆分成两个char, 由TxPduId作为索引找到)+trip(TripCntLengthgth)+reset(ResetCntLengthgth) + padding0;
 * 在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
 * 
 * 3.调用Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,uint8* verifyPtr);
 * 若验证通过，使用verifyPtr（trip(TripCntLengthgth)+1(1bit)）更新trip值和reset值
 * 
 * 4.验证失败不更新 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    if (NUM_RESET <= TxPduId) return E_NOT_OK;

    // 获取trip报文
    uint8 data = *PduInfoPtr->SduDataPtr;

    // trip
    for (int i = 1; i <= TripCntLengthgth; i++) {
        int index = (i + 8 - 1) / 8 - 1;
        if (is_k(data, i))
            trip[index] = set_k(trip[index], i % 8);
    }

    // reset
    if (is_k(data, TripCntLengthgth + 1))
        trip[2] = set_k(trip[2], 1);

    // mac
    uint8 *mac;
    uint32 macLength = 64 - TripCntLengthgth - 1;
    for (int i = TripCntLengthgth + 1; i < 64; i++) {
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
    if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK)
        return E_NOT_OK;

    verifyPtr(TripCntLengthgth + 1);
    PduInfoPtr->SduDataPtr = NULL;
    PduInfoPtr->SduLength = 8;

    return E_OK;
}

//下面两个函数是sender slave的部分

/**
 * 获取新鲜值及 裁剪新鲜值
 * 	1.根据SecOCFreshnessValueID 作为索引找到需要对应的 resetCnt[id], preTrip, msgCnt[]
 * 
 * 2.通过比较各计数器上一次发送值和当前值，构造新鲜值
 * 参考图
 * 
 * 3.将新鲜值按照 trip reset msg进行构造 SecOCFreshnessValue
 * 
 * 4.根据SecOCTruncatedFreshnessValueLength长度，截取msg中后(SecOCTruncatedFreshnessValueLength-2)比特位长度+2比特位reset flag（reset后两位）构造SecOCTruncatedFreshnessValue
 * 参考图
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
) {

}


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FvM_GetTxFreshnessTruncData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength
) {

}


//下面两个函数是receiver slave的部分

/**
 * 1.若SecOCTruncatedFreshnessValueLength ==SecOCFreshnessValueLength   将SecOCTruncatedFreshnessValue 复制到 SecOCFreshnessValue
 * 2.根据SecOCAuthVerifyAttempts依次比较表格中不同的format：
	依次比较resetflag（根据SecOCAuthVerifyAttempts值）: 
	    若不满足条件,SecOCAuthVerifyAttempts+1, 重新进行2.1, 若SecOCAuthVerifyAttempts=5了退出
		若满足，比较reset（resetData 和preresetData）：
			若构造失败， SecOCAuthVerifyAttempts+1，重新进行2.1， 若SecOCAuthVerifyAttempts=5了退出
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
 * SecOCFreshnessValue包括trip(TripCntLengthgth) reset(ResetCntLengthgth) msg(MsgCntLengthgth) resetflag(ResetFlagLength)
 * 由于长度是比特长度因此需要将各计数器的值分离出来，从而更新对应值：
 * preTrip[3*SecOCFreshnessValueID] =  trip(TripCntLengthgth)
 * resetCnt[SecOCFreshnessValueID]->preresetData[3*SecOCFreshnessValueID] = reset(ResetCntLengthgth)
 * msgCnt[SecOCFreshnessValueID]->premsgdata[3*SecOCFreshnessValueID] =msg(MsgCntLengthgth)
*/
//需保证报文没被取消
FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                   P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {


}


// void updatePreRxValue(uint16 SecOCFreshnessValueID,  uint8* SecOCFreshnessValue);









































uint8 trip[3];  //初始化时从非易失性存储器中获得并+1后再存回非易失性存储器， 低位先占满8字节 高位再占
//例如  TripCntLengthgth=11  且trip=0x04 0xff       [0000 0100][1111 1111][]
uint8 TripCntLengthgth = 16;    //可配置


void MasterFVM_Init(void) {
/*
	1.获取非易失性存储器中的值，存入全局变量trip[3]对应索引内，trip数组使用情况依据TripCntLengthgth决定。优先使用低索引；
	2.对trip[3]+1, 达到char最大值则，发生进位;  
		若发生进位导致trip到达TripCntLengthgth规定最大情况，则 trip[3] =1,即高位为0，最低位为1；
		if(trip[-1]==255){   trip[-2]+=1;  依次观察是否进位
	3.将新的trip值更新到非易失性存储器中；

	*/

    //需先判断TripCntLengthgth长度，确定多少数组元素需要管理
    uint8 i;
    HAL_I2C_Mem_Read(&hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, trip, 2, 0xff);//读取2字节的trip



    if (trip[1] == 255) {  //低位满，需进位
        if (trip[0] == 255) {  //trip值达到最大值
            trip[0] = 0;
            trip[1] = 1;
        } else {            //trip未达最大值， 高位进位，低位到0
            trip[0] += 1;
            trip[1] = 0;
        }
    } else {
        trip[1] += 1;
    }

    for (i = 0; i < 2; i++) {
        HAL_I2C_Mem_Write(&hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &trip[i], 1,
                          0xff);//使用I2C块读，出错。因此采用此种方式，逐个单字节写入
        HAL_Delay(5);//此处延时必加，与AT24C02写时序有关
    }

}

uint16 tripcanid = 0x2bd;  //可配置

void getTripValue(const PduInfoType *PduInfoPtr) {
    /*
    默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
        trip(TripCntLengthgth)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
        例如 TripCntLengthgth=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
        由于后接1bit reset且值为1 则dataptr = [0x02] [0xbd] [1001 1111][1111 0000] 最后4位为补0
    1.构造生成mac的原始数据 char *dataptr 组成为  tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLengthgth)+reset(1bit)：值为1 + padding0;
        在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
        例如  TripCntLengthgth=12  [ 0x02 0xbd 0x00 0x14]  此时
    2.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
        获得mac值
    3. 数据排版 PduInfoPtr.SduDataPtr =trip(TripCntLengthgth)+1(1bit)+ mac(64bit-TripCntLengthgth-1);
        重排位，在数据生成时不再填充0

    */
}

void getResetValue(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
    /*
    1.先判断TxPduId 若>=NUM_RESET， 则直接退出;

    默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
        trip(TripCntLengthgth)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
        例如 TripCntLengthgth=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
        由于后接ResetCntLengthgth reset   如  ResetCntLengthgth=11  且reset=0x04 0xff 左移5位并连接到前面trip后
         则dataptr = [0x00] [0x65] [1001 1111][1111 0011] [1111 1100 ]最后2位为补0
    2.构造生成mac的原始数据 char *dataptr 组成为
        resetcanid[2](reset拆分成两个char, 由TxPduId作为索引找到)+trip(TripCntLengthgth)+reset(ResetCntLengthgth) + padding0;
        在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
        reset(ResetCntLengthgth)值在获取后需要将resetData[3*TxPduId]对应值加1 若发生进位需要改变
        若reset达到ResetCntLengthgth规定的最大值，则reset不加值。

    3.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
        获得mac值
    4. 数据排版 PduInfoPtr.SduDataPtr =reset(ResetCntLengthgth)+ mac(64bit-ResetCntLengthgth);
        重排位，在数据生成时不再填充0

    */
}


void change_state(PduIdType TxPduId) {
    /* 收到id后检查confirmECU 对应索引值， 修改state状态为1， 发送ackv的报文*/
    ConfirmECU_Type tmp
    *;
    tmp = &confirmECU[TxPduId];
    tmp.state = 1;
    PduInfoPtr info;
    CanIf_Transmit(tmp.ackv, info);
}


uint8 firsttrip = 0;     //标记是否发送过trip
uint8 triperrorlabel = 1;   //trip发送失败    0表示正常 1表示多次失败且达到上限
uint8 syn_for_trip = 0;   //0表示trip同步  1表示reset同步
uint32 windowTimetag; // trip时间计数
uint32 windowTime4ack; // trip周期时间阈值 可配置

uint8 resendTriptag;  //需要重传trip

uint8 resentTripTime;  //trip重传次数
uint8 triptxlimit; //重传次数上限  可配置


uint16 errorcanid = 0x64;  //可配置
uint16 notifycanid = 0x386;  //可配置

uint8 multi = 1; //时间倍率  1表示一次调用MainTx 为1ms   可配置

void MasterFVM_MainTx(void) {
    /*
        本函数将被时间调度函数周期性调用，执行内容如下：
        1.若未曾发送过trip报文，则发送一次
        2.trip同步失败是否达到上限
          若达到上限则结束
          未达上限则检查当前同步（syn_for_trip）为trip同步（进入3）还是reset同步（进入4）
        3.trip同步：
            windowTimetag每毫秒计1，当达到阈值windowTime4ack，检查confirmECU中的state状况，若存在非1的state，设resendTriptag=1
            判断resendTriptag是否==1，
            若==1：若达到trip重发上限，发报错报文errorcanid
                若未达重发上限，重发trip报文：调用getTripValue（）
            若==0:不需要重发，trip过程完成，发通知报文notifycanid，修改syn_for_trip=1 为reset同步过程。
        4.reset同步：
            每轮各reset的tag+1， 若达到阈值则调用getResetValue() 发送reset同步消息
    */

    unsigned char i;
    const PduInfoType *info;
    if (firsttrip == 0) {
        getTripValue(info);
        CanIf_Transmit(tripcanid, info);
        firsttrip = 1;
    }

    if (triperrorlabel == 0) { //未发生多次trip失败达到上限
        if (syn_for_trip == 0) {  //当前是trip同步阶段
            windowTimetag += multi;
            if (windowTimetag == windowTime4ack) {//10s
                for (i = 0; i < NUM_ECU; i++) {
                    if (confirmECU[i].state != 1) {   //有没收到的ack的

                        resendTriptag = 1;
                        break;
                    }
                }
                if (resendTriptag == 1) {  //需要重发trip
                    resentTripTime++;
                    if (resentTripTime == triptxlimit) {  //达到重发上限
                        info->SduDataPtr = NULL;
                        CanIf_Transmit(errorcanid, info);
                        triperrorlabel = 1; //多次发送trip都失败 =1
                    } else {
                        getTripValue(info);
                        CanIf_Transmit(tripcanid, info);
                        resendTriptag = 0;
                        windowTimetag = 0;
                    }
                } else { // resendtag==0  不需要重发，发送通知报文，开始按周期发送同步消息
                    info->SduDataPtr = NULL;
                    info->SduLength = 8;
                    CanIf_Transmit(notifycanid, info);
                    syn_for_trip = 1;
                    windowTimetag = 0;
                }
            }

            // trip同步阶段 - end  syn_for_trip=0
        } else {  //syn_for_trip=1  reset同步阶段
            for (i = 0; i < NUM_RESET; i++) {  //占2字节
                resetCnt[i].resetSyntag += multi;
            }
            for (i = 0; i < NUM_RESET; i++) {  //占2字节
                if (resetCnt[i].resetSyntag >= resetCnt[i].resetSynTime) {

                    getResetValue(resetCnt[i].resetcanid, info);
                    CanIf_Transmit(notifycanid, info);
                    resetCnt[i].resetSyntag = 0; //重新计数
                    break;
                }
            }
        }
    }

}




