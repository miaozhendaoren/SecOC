#include "SecOC.h"
#include "SecOC_Lcfg.h"
#include "SecOC_PBcfg.h"
#include "PduR_SecOC.h"
#include "Csm.h"   //optional
#include "Det.h"   // optional
//#include "NvM.h"
//#include "SecOC_Cbk.h"
//#include "Rte_SecOC.h"
//#include "Rte_Csm_Type.h"
#include "Rte_SecOC_Type.h"

#include "SecOC_helper.h"


static Std_VersionInfoType _SecOC_VersionInfo =  //具体数值暂时不确定
{
	.vendorID = (uint16)1,
	.moduleID = (uint16) 1,
	.instanceID = (uint8)1,
	.sw_major_version = (uint8)1,
	.sw_minor_version = (uint8)0,
	.sw_patch_version = (uint8)0,

};

const SecOCGeneral_type tmpSecOCGeneral;
const SecOCRxPduProcessing_type* tmpSecOCRxPduProcessing;
const SecOCTxPduProcessing_type* tmpSecOCTxPduProcessing;

SecOC_StateType _secOCState=SECOC_UNINIT;

#if (SECOC_ERROR_DETECT == STD_ON)
#define VALIDATE_STATE_INIT()\
	if(SECOC_INIT!=_secOCState){\
		Det_ReportError(); \
		return; \
	}

#else
#define VALIDATE_STATE_INIT(_api)
#endif

extern SecOCintermidate_type SecOCintermidate[];
extern SecOCintermidateRx_type SecOCintermidateRx[];

uint8 tmpinter[64];

uint8 tmppduinfo[64];

PduInfoType* secoctmp={
	.SduDataPtr = tmppduinfo

};

const ComConfig_type * ComConfig;
uint8 com_pdur[] = {vcom};

/* Com_Asu_Config declaration*/
extern Com_Asu_Config_type ComAsuConfiguration;
static Com_Asu_Config_type * Com_Asu_Config = &ComAsuConfiguration;


/*Initializes the the SecOC module. Successful initialization leads to state Sec
OC_INIT.
[SWS_SecOC_00054]⌈
Within SecOC_Init, the module shall initialize all internal global variables and the
buffers of the SecOC I-PDUs.
*/
// ******************FUNCTION***********************
// void SecOC_Init(const SecOC_ConfigType *config)
FUNC(void, SECOC_CODE)
SecOC_Init(P2CONST(SecOC_ConfigType, AUTOMATIC, SECOC_APPL_DATA) config) {
	tmpSecOCGeneral = config->general;
	tmpSecOCTxPduProcessing = config->secOCTxPduProcessings;
	tmpSecOCRxPduProcessing = config->secOCRxPduProcessings;
	//	initial(variables);
	//	if(!success){
	//		SecOCState = SecOC_E_UNINIT;
	//	}
	_secOCState = SECOC_INIT;
}

/*
This service stops the secure onboard communication. All buffered I-PDU are
removed and have to be obtained again, if needed, after SecOC_Init has been
called. By a call to SecOC_DeInit the AUTOSAR SecOC module is put into a not
initialized state (SecOC_UNINIT).*/
// ******************FUNCTION***********************
// void SecOC_DeInit (void)
FUNC(void, SECOC_CODE)
SecOC_DeInit(void) {
	_secOCState = SECOC_UNINIT;
}

/*Returns the version information of this module.
 *
 */
// ******************FUNCTION***********************
// void SecOC_GetVersionInfo (Std_VersionInfoType* versioninfo) //0x02 同步 可重入
FUNC(void, SECOC_CODE)
SecOC_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC, SECOC_APPL_DATA) versioninfo) //0x02 同步 可重入
{
	VALIDATE_STATE_INIT();
	memcpy(versionInfo, &_SecOC_VersionInfo, sizeof(Std_VersionInfoType));
	return;
}

// ******************FUNCTION***********************
//0x49 同步 可重入不同的PduId。不可重入相同的PduId。 参考7.4
//E_OK: 传输请求已被接受。
//E_NOT_OK: 传输请求未被接受
// Std_ReturnType SecOC_IfTransmit (PduIdType TxPduId, const PduInfoType* PduInfoPtr)
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_IfTransmit(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
	//	if(direct || triggered transmission){
	//
	//		prepare();
	//		copyBuffer();  //时序图有时为copy()
	//	}else if(tp trans){
	//		prepare();
	//		copy();
	//	}
	//
	//	if(succcess){  //prepare 和 copy正确
	//		return E_OK;
	//	}else{
	//		return E_NOT_OK;
	//	}
	VALIDATE_STATE_INIT();
	if (TxPduId >= SECOC_NUM_OF_TX_IPDU)
	{
		return E_NOT_OK; //非secoc内txpduid
	}
	SecOCintermidate[TxPduId].adatachar = PduInfoPtr->SduDataPtr;
	SecOCintermidate[TxPduId].len = PduInfoPtr->SduLength;
	return E_OK;

	//	uint8 idx=0;
	//	for(idx; idx<SECOC_NUM_OF_TX_IPDU; idx++){
	//		SecOCTxPduProcessing_type cur = tmpSecOCTxPduProcessing[num];
	//		if(cur.SecOCTxAuthenticPduLayer.SecOCTxAuthenticLayerPduId == TxPduId){
	//			cur.SecOCTxAuthenticPduLayer.SecOCTxAuthenticLayerPduRef->SduDataPtr=PduInfoPtr->SduDataPtr;
	//			cur.SecOCTxAuthenticPduLayer.SecOCTxAuthenticLayerPduRef->MetaDataPtr=PduInfoPtr->MetaDataPtr;
	//			cur.SecOCTxAuthenticPduLayer.SecOCTxAuthenticLayerPduRef->SduLength=PduInfoPtr->SduLength;
	//			return E_OK;
	//		}//覆盖
	//	}
	//	return E_NOT_OK;
}

// ******************FUNCTION***********************
// Std_ReturnType SecOC_TpTransmit ( PduIdType TxPduId, const PduInfoType* PduInfoPtr )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_TpTransmit(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST) PduInfoPtr)
{
	VALIDATE_STATE_INIT();
	const PduInfoType* info = secoctmp;
	secoctmp->SduLength = PduInfoPtr->SduLength;
	BufReq_ReturnType rst= BUFREQ_E_NOT_OK;
	RetryInfoType* retry;
	PduLengthType* availableDataPtr;
	if(8 < PduInfoPtr->SduLength) return E_NOT_OK;
	while(rst!=BUFREQ_OK){
		rst = PduR_SecOCTpCopyTxData(TxPduId, info, retry, availableDataPtr);
	}
	SecOCintermidate[TxPduId].adatachar = info->SduDataPtr;
	SecOCintermidate[TxPduId].len = info->SduLength;
	return E_OK;
}

// ******************FUNCTION***********************
//Std_ReturnType SecOC_IfCancelTransmit (PduIdType TxPduId )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_IfCancelTransmit(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId)
{
//0x4a 同步 可重入不同的PduId。不可重入相同的PduId。
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
//请求取消较低层通信模块中正在进行的PDU传输。
}

// ******************FUNCTION***********************
//Std_ReturnType SecOC_TpCancelTransmit ( PduIdType TxPduId )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_TpCancelTransmit(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId)
{
//0x4a 同步 可重入不同的PduId。不可重入相同的PduId。
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
}

// ******************FUNCTION***********************
Std_ReturnType SecOC_TpCancelReceive ( PduIdType RxPduId )
{
//0x4c 同步 不可重入
//E_OK:目标模块成功执行取消。
//E_NOT_OK: 目标模块拒绝取消。
//请求取消较低层传输协议模块中正在进行的PDU接收。
}

//Optional Interface
// ******************FUNCTION***********************
Std_ReturnType SecOC_VerifyStatusOverride ( 
	uint16 ValueID,
	SecOC_OverrideStatusType overrideStatus,
	uint8 numberOfMessagesToOverride 
){
//0x0b 同步 可重入不同的FreshnessValueID。不可重入相同的FreshnessValueID。
//E_OK:请求成功。
//E_NOT_OK: 请求失败。
//8.3.9
//	accepts SecOCDataId as parameter

}

// ******************FUNCTION***********************
Std_ReturnType SecOC_SendDefaultAuthenticationInformation ( 
	uint16 FreshnessValueID, 
	boolean sendDefaultAuthenticationInformation 
){
//0x04 同步 可重入不同的FreshnessValueID。不可重入相同的FreshnessValueID。
//E_OK:请求成功。
//E_NOT_OK: 请求失败。
//8.3.9
}


//回调通知
// ******************FUNCTION***********************
//提供空间存储要接收到的要处理的报文数据，将报文数据保存到指定空间中
// void SecOC_RxIndication ( PduIdType RxPduId, const PduInfoType* PduInfoPtr ){
FUNC(void, SECOC_CODE)
SecOC_RxIndication(
	VAR(PduIdType, COMSTACK_TYPES_VAR) RxPduId,
	P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST) PduInfoPtr)
{
	//	if(verify direct){
	//		prepare();
	//		copy();
	//	}else if(re-authenti){
	//		prepare();
	//		copyBuffer();
	//	}
	VALIDATE_STATE_INIT();
	uint8 idx,AuthStartPosition,AuthLen;
	AuthStartPosition=tmpSecOCRxPduProcessing->SecOCAuthDataFreshnessStartPosition;
	AuthLen=tmpSecOCRxPduProcessing->SecOCAuthDataFreshnessLen;
	//暂定replace，覆盖方式	保存了s-pdu和a-pdu，以及它们的长度
	memcpy((uint8*)SecOCintermidateRx[RxPduId].sdatachar,PduInfoPtr->SduDataPtr,PduInfoPtr->SduLength);
	memcpy((uint8*)SecOCintermidateRx[RxPduId].adatachar,PduInfoPtr->SduDataPtr+AuthStartPosition,AuthLen);
	SecOCintermidateRx[RxPduId].slen=PduInfoPtr->SduLength;
	SecOCintermidateRx[RxPduId].len=AuthLen;
}

// ******************FUNCTION***********************
// void SecOC_TpRxIndication ( PduIdType id, Std_ReturnType result ){
FUNC(void, SECOC_CODE)
SecOC_TpRxIndication(
	VAR(PduIdType, COMSTACK_TYPES_VAR) id,
	VAR(Std_ReturnType, STD_TYPES_VAR) result)
{
	VALIDATE_STATE_INIT();
	empty();
	if(success){
		result = E_OK;
	}else{
		result = E_NOT_OK;
	}
}

// ******************FUNCTION***********************
// void SecOC_TxConfirmation( PduIdType TxPduId, Std_ReturnType result ){
FUNC(void, SECOC_CODE)
SecOC_TxConfirmation(
	VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
	VAR(Std_ReturnType, STD_TYPES_VAR) result)
{
	VALIDATE_STATE_INIT();
	if(result == E_OK){
		SecOCintermidate[TxPduId].sdatachar=null;
		SecOCintermidate[TxPduId].slen=0;
		//若发送成功，则释放s-pdu缓存
	}

	PduR_SecOCIfTxConfirmation(TxPduId, result);
}

// ******************FUNCTION***********************
// void SecOC_TpTxConfirmation ( PduIdType TxPduId, Std_ReturnType result ){
FUNC(void,SECOC_CODE)
SecOC_TpTxConfirmation(
	PduIdType TxPduId, 
	Std_ReturnType result)
{
	VALIDATE_STATE_INIT();

	if(TxPduId>=SECOC_NUM_OF_TX_IPDU){
		return ;  //非secoc内txpduid
	}
	if(result == E_OK){  //传输成功, free(spdu)
		SecOCintermidate[TxPduId].sdatachar=null;
		SecOCintermidate[TxPduId].slen =0;
	}
	if(tmpSecOCTxPduProcessing[TxPduId].SecOCTxAuthenticPduLayer.SecOCPduType == SECOC_IFPDU){
		PduR_SecOCIfTxConfirmation(TxPduId, result );
	}
	if(tmpSecOCTxPduProcessing[TxPduId].SecOCTxAuthenticPduLayer.SecOCPduType == SECOC_TPPDU){
		PduR_SecOCTpTxConfirmation(TxPduId, result);

	}
}

// ******************FUNCTION***********************
// Std_ReturnType SecOC_TriggerTransmit ( PduIdType TxPduId, PduInfoType* PduInfoPtr ){
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_TriggerTransmit(
	VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
	P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
	VALIDATE_STATE_INIT();

	if(TxPduId>=SECOC_NUM_OF_TX_IPDU){
		return E_NOT_OK;  //非secoc内txpduid
	}
	if(SecOCintermidate[TxPduId].slen > PduInfoPtr->SduLength){
		return E_NOT_OK;
	}
	PduInfoPtr->SduDataPtr = SecOCintermidate[TxPduId].sdatachar;
	PduInfoPtr->SduLength = SecOCintermidate[TxPduId].slen;


	return E_OK;
}

// ******************FUNCTION***********************
// BufReq_ReturnType SecOC_CopyRxData ( PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr)   //输出
FUNC(VAR(BufReq_ReturnType, COMSTACK_TYPES_VAR), SECOC_CODE)
SecOC_CopyRxData(
	VAR(PduIdType, COMSTACK_TYPES_VAR) id,
	P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST) info,
	P2VAR(PduLengthType, AUTOMATIC, SECOC_APPL_DATA) bufferSizePtr) //输出
{
	/*copyData();
	if(success){
		return BUFREQ_OK;
	}else{
		return BUFREQ_E_NOT_OK;
	}//  BUFREQ_E_BUSY/ BUFREQ_E_OVFL*/
	VALIDATE_STATE_INIT();
	BufReq_ReturnType r = BUFREQ_OK;
	uint8 AuthStartPosition,AuthLen;
	AuthStartPosition=tmpSecOCRxPduProcessing->SecOCAuthDataFreshnessStartPosition;
	AuthLen=tmpSecOCRxPduProcessing->SecOCAuthDataFreshnessLen;
	boolean sizeOk;		//判断目前是否有足够的内存可以存储传输过来的数据
	//boolean dirOk;
	//boolean lockOk;
	//目前覆盖原数据
	sizeOk=1;
	//sizeOk=currentSize>=info->SduLength;
	//dirOk = SecoCIPduDirection == RECEIVE;
	//lockOk = isPduBufferLocked(PduId);
	if(sizeOk){
		memcpy((uint8*)SecOCintermidateRx[id].sdatachar,info->SduDataPtr,info->SduLength);
		SecOCintermidateRx[id].slen=info->SduLength;
		memcpy((uint8*)SecOCintermidateRx[id].adatachar,info->SduDataPtr+AuthStartPositin,AuthLen);
		SecOCintermidateRx[id].len=AuthLen;
		//*bufferSizePtr=Size-currentSize;
	}
	else{
		r=BUFREQ_E_NOT_OK;
	}
	return r;

}

// ******************FUNCTION***********************
// BufReq_ReturnType SecOC_CopyTxData ( PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
FUNC(VAR(BufReq_ReturnType, COMSTACK_TYPES_VAR), SECOC_CODE)
SecOC_CopyTxData(
	VAR(PduIdType, COMSTACK_TYPES_VAR) id,
	P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST) info,
	P2CONST(RetryInfoType, AUTOMATIC, SECOC_APPL_CONST) retry,
	P2VAR(PduLengthType, AUTOMATIC, SECOC_APPL_DATA) availableDataPtr)
{


	VALIDATE_STATE_INIT();
	if(SecOCintermidate[id].slen != 0){
		if(info->SduLength>SecOCintermidate[id].slen){
			return BUFREQ_E_BUSY;
		}
		memcpy( (void*) info->SduDataPtr, SecOCintermidate[id].sdatachar, info->SduLength);
		return BUFREQ_OK;
	}else
	{
		return BUFREQ_E_NOT_OK;
	}
}






// ******************FUNCTION***********************
// BufReq_ReturnType SecOC_StartOfReception ( PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr )
FUNC(VAR(BufReq_ReturnType, COMSTACK_TYPES_VAR), SECOC_CODE)
SecOC_StartOfReception(
	VAR(PduIdType, COMSTACK_TYPES_VAR) id,
	P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST) info,
	VAR(PduLengthType, COMSTACK_TYPES_VAR) TpSduLength,
	P2VAR(PduLengthType, AUTOMATIC, SECOC_APPL_DATA) bufferSizePtr)
{
	VALIDATE_STATE_INIT();
	prepare();
	//SecOC_RxIndication(id,info);
	uint8 AuthHeadlen,Alen,Freshnesslen,Maclen,Slen;
	AuthHeadlen=tmpSecOCRxPduProcessing->SecOCRxSecuredPduLayer.SecOCRxSecuredPdu.SecOCAuthPduHeaderLength;
	Alen=tmpSecOCRxPduProcessing->SecOCAuthDataFreshnessLen;
	Freshnesslen=tmpSecOCRxPduProcessing->SecOCFreshnessValueTruncLength;
	Maclen=tmpSecOCTxPduProcessing->SecOCAuthInfoTruncLength;
	Slen=AuthHeadlen+Alen+Freshnesslen+Maclen;
	BufReq_ReturnType r=BUFREQ_OK;

	if(upper tp){  //upper layer tp
		r=PduR_SecOCTpStartOfReception();
	}
	else{
		if(TpSduLength==0){
			r=BUFREQ_E_NOT_OK;
		}
		else if(tmpSecOCRxPduProcessing->SecOCReceptionOverflowStrategy==REJECT){
			r=BUFREQ_E_NOT_OK;
		}
		else if(AuthHeadlen>0&&info->SduLengt<Slen){		//待修正，文档描述不清楚
			r=BUFREQ_E_NOT_OK;
		}//  BUFREQ_E_BUSY/ BUFREQ_E_OVFL
	}

	return r;

	/*if(success){
			return BUFREQ_OK;
		}else{
			return BUFREQ_E_NOT_OK;
		}//  BUFREQ_E_BUSY/ BUFREQ_E_OVFL*/
}

// ******************FUNCTION***********************
//调出定义
// Std_ReturnType SecOC_GetRxFreshness ( 
// 	uint16 SecOCFreshnessValueID, 
// 	const uint8* SecOCTruncatedFreshnessValue, 
// 	uint32 SecOCTruncatedFreshnessValueLength, 
// 	uint16 SecOCAuthVerifyAttempts, 
// 	uint8* SecOCFreshnessValue,    //输出
// 	uint32* SecOCFreshnessValueLength    //入出
// )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_GetRxFreshness(
	uint16 SecOCFreshnessValueID,
	const uint8 *SecOCTruncatedFreshnessValue,
	uint32 SecOCTruncatedFreshnessValueLength,
	uint16 SecOCAuthVerifyAttempts,
	uint8 *SecOCFreshnessValue,		  //输出
	uint32 *SecOCFreshnessValueLength //入出
)
{
//0x4f 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用该接口来获取当前的新鲜度值。
}

// ******************FUNCTION***********************
// Std_ReturnType SecOC_GetRxFreshnessAuthData ( 
// 	uint16 SecOCFreshnessValueID, 
// 	const uint8* SecOCTruncatedFreshnessValue, 
// 	uint32 SecOCTruncatedFreshnessValueLength, 
// 	const uint8* SecOCAuthDataFreshnessValue,     //该参数保存了接收到的尚未经过身份验证的PDU的一部分。该参数是可选的(参见描述)。
// 	uint16 SecOCAuthDataFreshnessValueLength,    //以位为单位，保存authentic PDU的新鲜度的长度。该参数是可选的(参见描述)。
// 	uint16 SecOCAuthVerifyAttempts, 
// 	uint8* SecOCFreshnessValue,        //输出
// 	uint32* SecOCFreshnessValueLength    //入出
// )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_GetRxFreshnessAuthData(
	uint16 SecOCFreshnessValueID,
	const uint8 *SecOCTruncatedFreshnessValue,
	uint32 SecOCTruncatedFreshnessValueLength,
	const uint8 *SecOCAuthDataFreshnessValue, //该参数保存了接收到的尚未经过身份验证的PDU的一部分。该参数是可选的(参见描述)。
	uint16 SecOCAuthDataFreshnessValueLength, //以位为单位，保存authentic PDU的新鲜度的长度。该参数是可选的(参见描述)。
	uint16 SecOCAuthVerifyAttempts,
	uint8 *SecOCFreshnessValue,		  //输出
	uint32 *SecOCFreshnessValueLength //入出
)
{
//0x4e 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用该接口来获取当前的新鲜度值。
}

// ******************FUNCTION***********************
// Std_ReturnType SecOC_GetTxFreshness ( 
// 	uint16 SecOCFreshnessValueID, 
// 	uint8* SecOCFreshnessValue,    //输出
// 	uint32* SecOCFreshnessValueLength 
// )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_GetTxFreshness(
	uint16 SecOCFreshnessValueID, 
	uint8* SecOCFreshnessValue,    //输出
	uint32* SecOCFreshnessValueLength 
)
{
//0x52 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//这个API从数组(SecOCFreshnessValue)第一个字节的最有效位以大端字节格式返回新鲜度值。
}

// ******************FUNCTION***********************
// Std_ReturnType SecOC_GetTxFreshnessTruncData ( 
// 	uint16 SecOCFreshnessValueID, 
// 	uint8* SecOCFreshnessValue, 
// 	uint32* SecOCFreshnessValueLength, 
// 	uint8* SecOCTruncatedFreshnessValue,  
// 	uint32* SecOCTruncatedFreshnessValueLength 
// )
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SECOC_CODE)
SecOC_GetTxFreshnessTruncData(
	uint16 SecOCFreshnessValueID,
	uint8* SecOCFreshnessValue,
	uint32* SecOCFreshnessValueLength,
	uint8* SecOCTruncatedFreshnessValue,
	uint32* SecOCTruncatedFreshnessValueLength
)
{
//0x51 同步 可重入
//E_OK：请求成功
//E_NOT_OK：请求失败，由于新鲜度或这个FreshnessValueId的普遍问题，无法提供新鲜度值。
//E_BUSY：新鲜度信息暂时无法提供。
//SecOC使用这个接口来获取当前的新鲜度值。接口函数还提供了在Secured I-PDU中传输的截断新鲜度。
}

// ******************FUNCTION***********************
//void SecOC_SPduTxConfirmation ( uint16 SecOCFreshnessValueID )
FUNC(void)
SecOC_SPduTxConfirmation(uint16 SecOCFreshnessValueID)
{
//0x4d 同步 可重入
//SecOC利用该接口去表示Secured I-PDU已经开始进行传输到PDUR。
}


//Service Interfaces




//预定功能

// ******************FUNCTION***********************
/*SECOC 未初始化 调用此函数将直接返回，报错（可选）
 * 由SecOCMainFunctionPeriodTx 进行配置
 */
//void SecOC_MainFunctionTx ( void )
FUNC(void, SECOC_CODE)
SecOC_MainFunctionTx(void)
{
	VALIDATE_STATE_INIT();
	uint8 idx=0;
	Std_ReturnType result;
	PduInfoType *PduInfoPtr;
	if(_secOCState != SECOC_INIT) return;

	for(idx; idx<SECOC_NUM_OF_TX_IPDU; idx++){
		if(SecOCintermidate[idx].len > 0){
			authenticate(SecOCintermidate[idx],  &PduInfoPtr);
			if(PduInfoPtr->SduLength>0){ //authenticate 成功
				result = PduR_SecOCTransmit( SecOCintermidate.SecOCTxPduId,  PduInfoPtr);

			}
		}
	}
}

// ******************FUNCTION***********************
/*SECOC 未初始化 调用此函数将直接返回，报错（可选）
 * 由SecOCMainFunctionPeriodRx 进行配置
 */
// void SecOC_MainFunctionRx (void)
FUNC(void, SECOC_CODE)
SecOC_MainFunctionRx(void)
{
	VALIDATE_STATE_INIT();
	uint8 idx=0, idx2=0;
	Std_ReturnType result;
	PduInfoType *PduInfoPtr;
	VerificationResultType state;
	boolean opt=1;  //是否像上层传输
	SecOCRxPduProcessing_type curSecOCRxPduProcessing;

	if(_secOCState != SECOC_INIT) return;

	for(idx; idx<SECOC_NUM_OF_RX_IPDU; idx++) {
		if(SecOCintermidateRx[idx].slen > 0) {
			for(idx2=0; idx2<SECOC_NUM_OF_RX_IPDU; idx2++){
				if(tmpSecOCRxPduProcessing[idx2].SecOCDataId == SecOCintermidateRx[idx].SecOCRxPduId){
					curSecOCRxPduProcessing = tmpSecOCRxPduProcessing[idx2];
				}
			}
			if(curSecOCRxPduProcessing==NULL){
				break;
			}

			verify(SecOCintermidateRx[idx],state,curSecOCRxPduProcessing);
			if(state != SECOC_VERIFICATIONSUCCESS){
				if(tmpSecOCGeneral.SecOCIgnoreVerificationResult==TRUE){
					if(curSecOCRxPduProcessing.SecOCRxAuthenticPduLayer.SecOCPduType ==SECOC_IFPDU){
						PduR_SecOCIfRxIndication( RxPduId, PduInfoPtr);
					} else {
						while(loop) {
							result = PduR_SecOCTpCopyRxData(id, info, bufferSizePtr);
							if(opt2) {
								//可选
								PduR_SecOCTpRxIndication( id,  result);
							}

						}
					}
//					else if(re-auth){ //re-auth  这一段也可能并入上面 authentic过程还是放在tx中
//
//						PduR_SecOCIfRxIndication( RxPduId, PduInfoPtr);
//
//						authenticate();
//						result = PduR_SecOCTransmit(  TxPduId,  PduInfoPtr);
//
//					}
				}
			}
		}
	}
}

//
//void secoc_loop(){
//	while(1){
//		BufReq_ReturnType bresult;
//		bresult = PduR_SecOCTpCopyTxData(id, info,  retry, availableDataPtr);
//		// PduR_SecOCTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
//		if(bresult ==  BUFREQ_E_BUSY){
//			secoc_retry_in_next_main_func();
//		}else if(bresult ==  BUFREQ_E_NOT_OK){
//			PduR_SecOCTpTxConfirmation(TxPduId, E_NOT_OK); //终止transmission
//		}
//	}
//}

// ******************FUNCTION***********************
// void authenticate(SecOCintermidate_type SecOCintermidate,  PduInfoType* PduInfoPtr)
FUNC(void, SECOC_CODE)
authenticate(VAR(SecOCintermidate_type, SECOC_APPL_DATA) SecOCintermidate, P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{

	uint8 pduid = SecOCintermidate.SecOCTxPduId;
	boolean judgePattern = 0;
	Std_ReturnType resultfv, resultmac;
	uint8 *SecOCFreshnessValue;
	uint8 *SecOCTruncatedFreshnessValue;
	uint8 *Mac;
	uint8 *truncatedMac;
	uint8 *tmpinterpnt = tmpinter;
	uint8 tmpinterlen = 0;
	memcpy(tmpinterpnt, SecOCintermidate.adatachar, SecOCintermidate.len);
	tmpinterlen += SecOCintermidate.len;
	PduInfoPtr->SduLength = 0;
	if (tmpSecOCGeneral.SecOCQueryFreshnessValue == CFUNC)
	{
		if (tmpSecOCTxPduProcessing[pduid].SecOCProvideTxTruncatedFreshnessValue == TRUE)
		{
			resultfv = SecOC_GetTxFreshnessTruncData(tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueId,
													 SecOCFreshnessValue,
													 tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueLength,
													 SecOCTruncatedFreshnessValue,
													 tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueTruncLength);
			*(tmpinter + tmpinterlen) = SecOCTruncatedFreshnessValue;
			tmpinterlen += tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueTruncLength;
		}
		else
		{
			resultfv = SecOC_GetTxFreshness(tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueId,
											SecOCFreshnessValue,
											tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueLength);
			*(tmpinter + tmpinterlen) = SecOCFreshnessValue;
			tmpinterlen += tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueLength;
		}
	}
	else
	{ //rte
		if (tmpSecOCTxPduProcessing[pduid].SecOCProvideTxTruncatedFreshnessValue == TRUE)
		{
			resultfv = FreshnessManagement_GetTxFreshnessTruncData();
		}
		else
		{
			resultfv = FreshnessManagement_GetTxFreshness();
		}
	}

	if (resultfv == E_OK)
	{

		resultmac = Csm_MacGenerate(SecOCTxPduProcessing[pduid].SecOCTxAuthServiceConfigRef, 0, tmpinter, tmpinterlen, truncatedMac, tmpSecOCTxPduProcessing[pduid].SecOCAuthInfoTruncLength);
		if (resultmac == E_OK)
		{

			*(tmpinter + tmpinterlen) = truncatedMac;
			tmpinterlen += tmpSecOCTxPduProcessing[pduid].SecOCAuthInfoTruncLength;
			memcpy(SecOCintermidate.sdatachar, tmpinter, tmpinterlen);
			SecOCintermidate.slen = tmpinterlen;
			//			updatePreTxValue(tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueId, SecOCFreshnessValue);

			if (tmpSecOCGeneral.SecOCQueryFreshnessValue == CFUNC)
			{
				SecOC_SPduTxConfirmation(tmpSecOCTxPduProcessing[pduid].SecOCFreshnessValueID);
			}
			else
			{ //RTE
				FreshnessManagement_SPduTxConfirmation();
			}
			PduInfoPtr->SduDataPtr = SecOCintermidat.sdatachar;
			PduInfoPtr->SduLength = SecOCintermidat.slen;
		}
		else if (resultmac == E_NOT_OK || KEY_FAILURE)
		{ //不可恢复错误
			judgePattern = 1;
		}
		else
		{ //可恢复错误 abc+=1
			SecOCintermidate.abc += 1;
			if (SecOCintermidate.abc == tmpSecOCTxPduProcessing[pduid].SecOCAuthenticationBuildAttempts)
			{
				judgePattern = 1;
			}
		}
	}
	else if (resultfv == E_BUSY)
	{
		SecOCintermidate.abc += 1;
		if (SecOCintermidate.abc == tmpSecOCTxPduProcessing[pduid].SecOCAuthenticationBuildAttempts)
		{
			judgePattern = 1;
		}
	}
	else
	{ // 不可恢复错误
		judgePattern = 1;
	}

	if (judgePattern == 1)
	{
		if (tmpSecOCGeneral.SecOCDefaultAuthenticationInformationPattern > -1)
		{
			result = SecOC_SendDefaultAuthenticationInformation(curSecOCTxPduProcessing.SecOCFreshnessValueId, 1);
		}
		else
		{
			SecOCintermidate.len = 0;
		}
		report_DET(SECOC_E_CRYPTO_FAILURE);
	}
}

// ******************FUNCTION***********************
// void verify(SecOCintermidateRx_type SecOCintermidatRx, VerificationResultType verificationResult, SecOCRxPduProcessing_type curSecOCRxPduProcessing)
FUNC(void, SECOC_CODE)
verify(VAR(SecOCintermidateRx_type, SECOC_APPL_DATA) SecOCintermidatRx, VAR(VerificationResultType, SECOC_APPL_DATA) verificationResult, VAR(SecOCRxPduProcessing_type, SECOC_APPL_DATA) curSecOCRxPduProcessing) {
	uint8 idx = 0;

	uint8 *SecOCTruncatedFreshnessValue;
	uint16 SecOCAuthDataFreshnessValueLength;
	const uint8 *SecOCAuthDataFreshnessValue;
	uint8 *SecOCFreshnessValue;
	uint32 *SecOCFreshnessValueLength;

	Std_ReturnType resultfv, resultmac;

	SecOCTruncatedFreshnessValue = SecOCintermidatRx.sdatachar; //新鲜值

	if (tmpSecOCGeneral.SecOCQueryFreshnessValue == CFUNC)
	{
		if (curSecOCRxPduProcessing.SecOCUseAuthDataFreshness == TRUE)
		{
			resultfv = SecOC_GetRxFreshnessAuthData(
				curSecOCRxPduProcessing.SecOCFreshnessValueId,
				SecOCTruncatedFreshnessValue,
				curSecOCRxPduProcessing.SecOCFreshnessValueTruncLength,
				SecOCAuthDataFreshnessValue,	   //该参数保存了接收到的尚未经过身份验证的PDU的一部分。该参数是可选的(参见描述)。
				SecOCAuthDataFreshnessValueLength, //以位为单位，保存authentic PDU的新鲜度的长度。该参数是可选的(参见描述)。
				SecOCintermidatRx.avac,
				SecOCFreshnessValue, //输出
				SecOCFreshnessValueLength);
			//						curSecOCRxPduProcessing.SecOCAuthDataFreshnessStartPosition, curSecOCRxPduProcessing.SecOCAuthDataFreshnessLen);
		}
		else
		{
			resultfv = SecOC_GetRxFreshness(
				curSecOCRxPduProcessing.SecOCFreshnessValueId,
				SecOCTruncatedFreshnessValue,
				curSecOCRxPduProcessing.SecOCTruncatedFreshnessValueLength,
				curSecOCRxPduProcessing.avac,
				SecOCFreshnessValue, //输出
				SecOCFreshnessValueLength);
		}
	}
	else
	{ //rte
		if (curSecOCRxPduProcessing.SecOCUseAuthDataFreshness == TRUE)
		{
			resultfv = FreshnessManagement_GetRxFreshnessAuthData(SecOCAuthDataFreshnessStartPosition, SecOCAuthDataFreshnessLen);
		}
		else
		{
			resultfv = FreshnessManagement_GetRxFreshness();
		}
	}

	if (resultfv == E_OK)
	{
		resultmac = Csm_MacVerify();
		if (resultmac == E_OK)
		{
			if (pass)
			{
				VerificationResultType = SECOC_VERIFICATIONSUCCESS;
			}
			else
			{ //mac验证失败
				SecOCintermidatRx.abc = 0;
				SecOCintermidatRx.avac += 1;
				if (SecOCintermidatRx.avac == curSecOCRxPduProcessing.SecOCAuthenticationVerifyAttempts)
				{

					VerificationResultType = SECOC_VERIFICATIONFAILURE;
					drop(s - pdu);
					if (SecOC_VerifyStatusOverride)
					{
						doby(overrideStatus);
					}
				}
			}
		}
		else if (resultmac == E_NOT_OK || KEY_FAILURE)
		{ //不可恢复错误
			drop(s - pdu);
			VerificationResultType = SECOC_VERIFICATIONFAILURE;
			if (SecOC_VerifyStatusOverride)
			{
				doby(overrideStatus);
			}
		}
		else
		{ //E_BUSY, QUEUE_FULL
			SecOCintermidatRx.abc += 1;
			if (SecOCintermidatRx.abc == curSecOCRxPduProcessing.SecOCAuthenticationBuildAttempts)
			{
				drop(s - pdu);
				VerificationResultType = SECOC_AUTHENTICATIONBUILDFAILURE;
				if (SecOC_VerifyStatusOverride)
				{
					doby(overrideStatus);
				}
			}
		}
	}
	else if (resultfv == E_BUSY)
	{
		SecOCintermidatRx.abc += 1;
		if (SecOCintermidatRx.abc == curSecOCRxPduProcessing.SecOCAuthenticationBuildAttempts)
		{
			drop(s - pdu);
			VerificationResultType = SECOC_AUTHENTICATIONBUILDFAILURE;
			if (SecOC_VerifyStatusOverride)
			{
				doby(overrideStatus);
			}
		}
	}
	else
	{	// 不可恢复错误   E_NOT_OK
		//
		drop(a - pdu);
		VerificationResultType = SECOC_FRESHNESSFAILURE;
	}
}

// ******************FUNCTION***********************
// void SecOC_VerificationStatusCallout (SecOC_VerificationStatusType verificationStatus)
FUNC(void, SECOC_CODE)
SecOC_VerificationStatusCallout(VAR(SecOC_VerificationStatusType, SECOC_APPL_DATA) verificationStatus) {}
