
#ifndef _SECOC_H_
#define _SECOC_H_

#include "SecOC_Cfg.h"









 

//Development Error Types
#define SECOC_E_PARAM_POINTER  			0x01
#define SECOC_E_UNINIT 					0x02
#define SECOC_E_INVALID_PDU_SDU_ID		0x03
#define SECOC_E_INIT_FAILED				0x07
#define SECOC_E_CRYPTO_FAILURE			0x04
//Runtime Error Types
#define SECOC_E_FRESHNESS_FAILURE		0x08






typedef struct { //特定实现数据结构      配置SecOC模块数据结构
	const SecOCGeneral_type general;
	const SecOCTxPduProcessing_type* secOCTxPduProcessings;
	const SecOCRxPduProcessing_type* secOCRxPduProcessings;
}SecOC_ConfigType;


typedef enum {   //SecOC状态
	SECOC_UNINIT,   //SecOC未初始化
	SECOC_INIT		//SecOC初始化
}SecOC_StateType;


SecOCMainFunctionPeriodRx; //接收周期
SecOCMainFunctionPeriodTx;  //发送周期




void SecOC_Init (const SecOC_ConfigType* config);
/*
0x01 同步 非重入
初始化SECOC模块。成功初始化会导致状态SEC OC_INIT。
*/

void SecOC_DeInit (void);   //0x05 同步 不可重入
//该服务将停止安全车载通信。所有缓冲的I-PDU都被删除，如果需要，
//必须在调用SecOC_Init之后再次获得。通过调用SecOC_DeInit，
//将AUTOSAR SecOC模块置于未初始化状态(SecOC_UNINIT)。
//调用此函数后将清空所有internal global variables 和 i-pdu缓存


void SecOC_GetVersionInfo (Std_VersionInfoType* versioninfo);//0x02 同步 可重入


Std_ReturnType SecOC_IfTransmit ( PduIdType TxPduId, const PduInfoType* PduInfoPtr );
//0x49 同步 可重入不同的PduId。不可重入相同的PduId。 参考7.4
//E_OK: 传输请求已被接受。
//E_NOT_OK: 传输请求未被接受


Std_ReturnType SecOC_TpTransmit ( PduIdType TxPduId, const PduInfoType* PduInfoPtr );
//0x49 同步 可重入不同的PduId。不可重入相同的PduId。 参考7.4
//E_OK: 传输请求已被接受。
//E_NOT_OK: 传输请求未被接受

Std_ReturnType SecOC_IfCancelTransmit (PduIdType TxPduId );
//0x4a 同步 可重入不同的PduId。不可重入相同的PduId。
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
//请求取消较低层通信模块中正在进行的PDU传输。

Std_ReturnType SecOC_TpCancelTransmit ( PduIdType TxPduId );
//0x4a 同步 可重入不同的PduId。不可重入相同的PduId。
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
//请求取消较低层通信模块中正在进行的PDU传输。

Std_ReturnType SecOC_TpCancelReceive ( PduIdType RxPduId );
//0x4c 同步 不可重入
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
//请求取消较低层传输协议模块中正在进行的PDU接收。


//Optional Interface
Std_ReturnType SecOC_VerifyStatusOverride ( 
	uint16 ValueID,
	SecOC_OverrideStatusType overrideStatus,
	uint8 numberOfMessagesToOverride 
);
//0x0b 同步 可重入不同的FreshnessValueID。不可重入相同的FreshnessValueID。
//E_OK:请求成功。
//E_NOT_OK: 请求失败。
//8.3.9

Std_ReturnType SecOC_SendDefaultAuthenticationInformation ( 
	uint16 FreshnessValueID, 
	boolean sendDefaultAuthenticationInformation 
);
//0x04 同步 可重入不同的FreshnessValueID。不可重入相同的FreshnessValueID。
//E_OK:请求成功。
//E_NOT_OK: 请求失败。
//8.3.9



//回调通知
void SecOC_RxIndication ( 
	PduIdType RxPduId, 
	const PduInfoType* PduInfoPtr 
);
//0x42 同步 可重入不同的PduId。不可重入相同的PduId。
//从底层通信接口模块收到的PDU指示

void SecOC_TpRxIndication ( 
	PduIdType id, 
	Std_ReturnType result 
);
//0x45 同步 可重入
//通过TP API接收到I-PDU之后调用该服务，其结果表示传输是否成功。

void SecOC_TxConfirmation (PduIdType TxPduId, Std_ReturnType result);
//0x40 同步 可重入不同的PduId。不可重入相同的PduId。
//下层通信接口模块确认PDU的传输或传输PDU失败。

void SecOC_TpTxConfirmation ( 
	PduIdType id, 
	Std_ReturnType result 
);
//0x48 同步 可重入
//I-PDU在其网络上传输之后调用该函数，其结果表明传输是否成功。

Std_ReturnType SecOC_TriggerTransmit ( 
	PduIdType TxPduId, 
	PduInfoType* PduInfoPtr 
);
//0x41 同步 可重入不同PduId。不可重入相同的PduId。
//在这个API中，上层模块(被调用模块)应该检查可用数据是否符合PduInfoPtr->SduLength上报的缓冲区大小。
//如果符合，则将其数据复制到PduInfoPtr->SduDataPtr提供的缓冲区中，并在PduInfoPtr->SduLength中更新实际复制的数据长度。
//如果不符合，它返回E_NOT_OK，不改变PduInfoPtr。

BufReq_ReturnType SecOC_CopyRxData ( 
	PduIdType id, 
	const PduInfoType* info, 
	PduLengthType* bufferSizePtr   //输出
);
//0x44 同步 可重入
//BUFREQ_OK：数据复制成功。
//BUFREQ_E_NOT_OK：由于发生错误，数据未被复制。

BufReq_ReturnType SecOC_CopyTxData ( 
	PduIdType id, 
	const PduInfoType* info, 
	const RetryInfoType* retry, 
	PduLengthType* availableDataPtr  //输出
);
//0x43 同步 可重入
//BUFREQ_OK：数据已完全按要求复制到传输缓冲区。
//BUFREQ_E_BUSY：请求不能被满足，因为所需的Tx数据量不可用。下层模块可能稍后重试此调用。没有复制任何数据。
//BUFREQ_E_NOT_OK：数据未被复制。请求失败。
//调用本函数来获取I-PDU段(N-PDU)的传输数据。除非retry->Tp DataState为Tp_DATARETRY，
//否则每次调用本函数都会提供I-PDU数据的下一部分。在本例中，本函数重新启动来复制数据，
//会从retry->TxTpDataCnt所指示的当前位置的偏移量开始复制。
//剩余数据的大小被写入到availableDataPtr所指示的位置中去。

BufReq_ReturnType SecOC_StartOfReception ( 
	PduIdType id, 
	const PduInfoType* info, 
	PduLengthType TpSduLength, 
	PduLengthType* bufferSizePtr 
);
//0x46 同步 可重入
//BUFREQ_OK：连接已被接受。bufferSizePtr表示可用的接收缓冲区；接收继续。如果没有可用的所需大小的缓冲区，则bufferSizePtr将表示接收缓冲区大小为0。
//BUFREQ_E_NOT_OK：连接被拒绝；接收将会中止。bufferSizePtr保持不变。
//BUFREQ_E_OVFL：无法提供所需长度的缓冲区；接受将会中止。bufferSizePtr保持不变。
//本函数在接收N-SDU开始时被调用。N-SDU可能被分割成多个N-PDU(带有一个或多个后续CF的FF)，
//或者可能由单个N-PDU (SF)组成。当TpSduLength为0时，该服务将提供当前可用的最大缓冲区大小。
//如果调用SecOC_StartOfReception时TpSduLengthequal为0，则SecOC模块将返回BUFREQ_E_NOT_OK，并不采取进一步的行动。


//调出定义
//调出是在ECU集成期间必须添加到SecOC的代码片段。大多数调出的内容是手写的代码
Std_ReturnType SecOC_GetRxFreshness ( 
	uint16 SecOCFreshnessValueID, 
	const uint8* SecOCTruncatedFreshnessValue, 
	uint32 SecOCTruncatedFreshnessValueLength, 
	uint16 SecOCAuthVerifyAttempts, 
	uint8* SecOCFreshnessValue,    //输出
	uint32* SecOCFreshnessValueLength    //入出
);
//0x4f 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用该接口来获取当前的新鲜度值。

Std_ReturnType SecOC_GetRxFreshnessAuthData ( 
	uint16 SecOCFreshnessValueID, 
	const uint8* SecOCTruncatedFreshnessValue, 
	uint32 SecOCTruncatedFreshnessValueLength, 
	const uint8* SecOCAuthDataFreshnessValue,     //该参数保存了接收到的尚未经过身份验证的PDU的一部分。该参数是可选的(参见描述)。
	uint16 SecOCAuthDataFreshnessValueLength,    //以位为单位，保存authentic PDU的新鲜度的长度。该参数是可选的(参见描述)。
	uint16 SecOCAuthVerifyAttempts, 
	uint8* SecOCFreshnessValue,        //输出
	uint32* SecOCFreshnessValueLength    //入出
);
//0x4e 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用该接口来获取当前的新鲜度值。

Std_ReturnType SecOC_GetTxFreshness ( 
	uint16 SecOCFreshnessValueID, 
	uint8* SecOCFreshnessValue,    //输出
	uint32* SecOCFreshnessValueLength 
);
//0x52 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//这个API从数组(SecOCFreshnessValue)第一个字节的最有效位以大端字节格式返回新鲜度值。

Std_ReturnType SecOC_GetTxFreshnessTruncData ( 
	uint16 SecOCFreshnessValueID, 
	uint8* SecOCFreshnessValue, 
	uint32* SecOCFreshnessValueLength, 
	uint8* SecOCTruncatedFreshnessValue,  
	uint32* SecOCTruncatedFreshnessValueLength 
);
//0x51 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用这个接口来获取当前的新鲜度值。接口函数还提供了在Secured I-PDU中传输的截断新鲜度。

void SecOC_SPduTxConfirmation ( 
	uint16 SecOCFreshnessValueID 
);
//0x4d 同步 可重入
//SecOC利用该接口去表示Secured I-PDU已经开始进行传输。







//
////8.8服务接口
///*nterfaces
//described here will be visible on the VFB and are used to generate the Rte between
//application software and the SecOC module.
//*/
//SecOC_VerificationStatusType VerificationStatus;
//
//VerifyStatusConfiguration(
//	VerifyStatusOverride[  //operation
//		uint16 ValueId,
//		SecOC_OverrideStatusType overrideStatus,
//		uint8 numberOfMessagesToOverride
//	]
//);
////E_OK 操作成功
////E_NOT_OK 操作失败
//
//FreshnessManagement(
//
//GetRxFreshness[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetRxFreshnessAuthData[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//SecOC_FreshnessArrayType authenticDataFreshnessValue,
//uint16 authenticDataFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetTxFreshness[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetTxFreshnessTruncData[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//SPduTxConfirmation[
//uint16 freshnessValueId
//]//0	E_OK
//
//
//);
////0	E_OK	操作成功
////1	E_NOT_OK	操作失败
////2	E_BUSY	操作暂时失败，新鲜值当前无法提供。



#endif
