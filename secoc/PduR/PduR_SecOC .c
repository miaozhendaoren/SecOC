#include "PduR_SecOC.h"
#include "Com.h"

unsigned char mode=0; //暂时设定为控制认证的模式

unsigned char re_auth1=1;

extern const SecOCTxPduProcessing_type* tmpSecOCTxPduProcessing;


typedef enum
{
    CanIF,
    canTP,
	FrIF,
    Com,
    CanTp_COM,
    CanTp_TXCopy,
    drirect
}type_t;


int Com_type[] =   {CanIF, CanIF};

Std_ReturnType PduR_SecOCTransmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr){
	//依据TxPduId进行分配路径

	if(Com_type[TxPduId]==CanIF){   //direct / re_auth
		 return CanIf_Transmit(TxPduId, PduInfoPtr);

	}else if(Com_type[TxPduId]==FrIF){ //triggered trans
		return FrIf_Transmit(TxPduId, PduInfoPtr);
	}else if(Com_type[TxPduId]==canTP){  //tp
		return CanTp_Transmit(TxPduId, PduInfoPtr);
	}
//	else if(mode==3){  //upper layer transport protocol
//		return CanTp_Transmit(TxPduId, PduInfoPtr);
//	}
}


BufReq_ReturnType PduR_SecOCTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
{ //  需要执行循环   
	return Com_CopyTxData( id, info, retry, availableDataPtr);
}

void PduR_SecOCIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result){
	Com_TxConfirmation(TxPduId, result);
}

void PduR_SecOCIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr){
	Com_RxIndication(RxPduId, PduInfoPtr);
	if(re_auth){ //re-auth
		result = SecOC_TpTransmit(  TxPduId,  PduInfoPtr );

	}
}

BufReq_ReturnType PduR_SecOCTpStartOfReception( PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
	return Com_StartOfReception( id,  info,  TpSduLength,  bufferSizePtr);

}

BufReq_ReturnType PduR_SecOCTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr)
{
	return Com_CopyRxData(id, info, bufferSizePtr);
}

void PduR_SecOCTpRxIndication(PduIdType id, Std_ReturnType result){
	Com_TpRxIndication(id, result);
}

void PduR_SecOCTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result){
	Com_TpTxConfirmation(TxPduId, result);
}



