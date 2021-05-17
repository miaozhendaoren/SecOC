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

    // 获取trip报文
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

    // 获取trip报文
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
 * 3.将新鲜值按照 trip reset msg进行构造 SecOCFreshnessValue
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
    resetCnt[SecOCFreshnessValueID]
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
 * SecOCFreshnessValue包括trip(TripCntLength) reset(ResetCntLength) msg(MsgCntLengthgth) resetflag(ResetFlagLength)
 * 由于长度是比特长度因此需要将各计数器的值分离出来，从而更新对应值：
 * preTrip[3*SecOCFreshnessValueID] =  trip(TripCntLength)
 * resetCnt[SecOCFreshnessValueID] -> preresetData[3*SecOCFreshnessValueID] = reset(ResetCntLength)
 * msgCnt[SecOCFreshnessValueID] -> premsgdata[3*SecOCFreshnessValueID] =msg(MsgCntLengthgth)
*/
//需保证报文没被取消
FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                   P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {

}







