#include "Com.h"
#include "PduR_Com.h"
#include "Std_Types.h"


unsigned char re_auth=0;

void Com_TxConfirmation(PduIdType TxPduId, Std_ReturnType result){
	//pass   确认上层知道结果
	if(success){
		result = E_OK;
		
	}else{
		result = E_NOT_OK;
	}
}

void Com_TpTxConfirmation(PduIdType TxPduId, Std_ReturnType result){
	//pass   确认上层知道结果  upper layer tp 
//	UNLOCKBUFFER(&AsuIPdu->PduBufferState);

}

BufReq_ReturnType Com_CopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr){
	ComIPdu_type *IPdu = GET_IPdu(id);
	Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(id);

	if( (IPdu->ComIPduDirection == SEND) &&
			(Asu_IPdu->PduBufferState.CurrentPosition + info->SduLength <= IPdu->ComIPduSize) )
	{
		void const * source = (uint8*)IPdu->ComIPduDataPtr + Asu_IPdu->PduBufferState.CurrentPosition;
//		LOCKBUFFER(&Asu_IPdu->PduBufferState);
		info->SduDataPtr = source;
		//memcpy( (void*) info->SduDataPtr, source, info->SduDataPtr);
		Asu_IPdu->PduBufferState.CurrentPosition += info->SduLength;
		*availableDataPtr = IPdu->ComIPduSize - Asu_IPdu->PduBufferState.CurrentPosition;
		return BUFREQ_OK;
	}
	else
	{
		return BUFREQ_E_NOT_OK;
	}//BUFREQ_E_BUSY / BUFREQ_E_OVFL
}

void Com_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr){
	if(direct || tp){  // direct || tp
		copyData();
	}else if(re_auth==1){ // re-auth
		copyBuffer();
	}
}

BufReq_ReturnType Com_StartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
	empty();
	//通知上层
	if(success){
		return BUFREQ_OK;
	}else{
		return BUFREQ_E_NOT_OK;
}

BufReq_ReturnType Com_CopyRxData ( PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr)
{
	copyData();
	if(success){
		return BUFREQ_OK;
	}else{
		return BUFREQ_E_NOT_OK;
	}//BUFREQ_E_BUSY / BUFREQ_E_OVFL
}

void Com_TpRxIndication ( PduIdType id, Std_ReturnType result)
{
	empty();
	if(success){
		result = E_OK;
	}else{
		result = E_NOT_OK;
	}
}

int main(){
	
	Std_ReturnType result;
	PduIdType id;
	const PduInfoType* PduInfoPtr;
	
	
	//Authentication during direct transmission
	result = PduR_ComTransmit( id, PduInfoPtr);
	
	
	
	//Authentication during triggered transmission
	result = PduR_ComTransmit(id, PduInfoPtr);

	
	//Authentication during transport protocol transmission
	result =PduR_ComTransmit(id, PduInfoPtr);

	
	//Authentication with upper layer transport protocol
	result = PduR_ComTransmit(id, PduInfoPtr);

	
	while(1);
	
}
