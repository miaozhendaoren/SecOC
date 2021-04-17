#include "PduR_CanTp.h"

unsigned char mode=0; //暂时设定为控制认证的模式

//添加内容
BufReq_ReturnType PduR_CanTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
{
//	if(mode == 1){		// (1)I-PDU from the DCM module to the CanTp module using the transport protocol API.		/		(2)I-PDU from the DCM module to the CanTp, FrTp and LinTp (LinIf includes the transport protocol module) module using the transport protocol API.
//		Dcm_CopyTxData(BufReq_ReturnType, PduIdType,const PduInfoType*, RetryInfoType*, PduLengthType*);
//	}
//	else if(mode == 2){		//(1)an I-PDU (contained in a SF) received from CAN1 transport protocol and gatewayed to DCM (internal) and gatewayed to CAN2 transport protocol.		/		(2) how routing of a broadcast TP protocol (e.g. BAM of J1939Tp) is performed.
//		copy data();
//	}
//	else if(mode == 3){		//(1)an (multi N-PDU) I-PDU is gatewayed between two CAN networks, using transport protocol module.		/		(2)an (multi N-PDU) I-PDU is gatewayed from J1939Tp to two CAN-Networks, using transport protocol module.
//		//do none
//	}
//	else if(mode ==0){ //使用secoc模块  tp trans
//		return SecOC_CopyTxData ( id, info, retry, availableDataPtr);
//
//	}
	return SecOC_CopyTxData ( id, info, retry, availableDataPtr);

	
}	

//添加内容
void PduR_CanTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
//	if(mode == 0){ // tp trans
//		SecOC_TpTxConfirmation(TxPduId, result);
//	}
//	else if(mode == 1){		//I-PDU from the DCM module to the CanTp module using the transport protocol API.
//		Dcm_TpTxConfirmation(PduIdType, Std_ReturnType);
//	}
//	else if(mode == 2){		//(1)an I-PDU (contained in a SF) received from CAN1 transport protocol and gatewayed to DCM (internal) and gatewayed to CAN2 transport protocol.		/		(2)I-PDU from the DCM module to the CanTp, FrTp and LinTp (LinIf includes the transport protocol module) module using the transport protocol API.
//		//do none		(3) an (multi N-PDU) I-PDU is gatewayed between two CAN networks, using transport protocol module.		/		(4)an (multi N-PDU) I-PDU is gatewayed from J1939Tp to two CAN-Networks, using transport protocol module.			/			(5) how routing of a broadcast TP protocol (e.g. BAM of J1939Tp) is performed.
//	}
	SecOC_TpTxConfirmation(TxPduId, result);
}

//添加内容
BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength,PduLengthType* bufferSizePtr){
	if(mode == 0){  //verify tp || upper tp
		 return SecOC_StartOfReception(id,info, TpSduLength, bufferSizePtr);
	}
	else if(mode == 1){		// I-PDU from the CanTp module to the DCM module. The reception is made using the transport protocol APIs.
		Dcm_StartOfReception(BufReq_ReturnType, PduIdType,const PduInfoType*, PduLengthType, PduLengthType*);
	}
	else if(mode == 2){		//(1)an I-PDU (contained in a SF) received from CAN1 transport protocol and gatewayed to DCM (internal) and gatewayed to CAN2 transport protocol.		/		(2)an (multi N-PDU) I-PDU is gatewayed between two CAN networks, using transport protocol module.
		//do none			
	}
}

//添加内容
BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr){
	if(mode == 0){ //verify tp    up layer tp
		return SecOC_CopyRxData(id, info, bufferSizePtr);
	}
	else if(mode == 1){		// I-PDU from the CanTp module to the DCM module. The reception is made using the transport protocol APIs.
		Dcm_CopyRxData(BufReq_ReturnType, PduIdType,const PduInfoType*, PduLengthType*);
	}
	else if(mode == 2){		//(1)an I-PDU (contained in a SF) received from CAN1 transport protocol and gatewayed to DCM (internal) and gatewayed to CAN2 transport protocol.		/		(2)an (multi N-PDU) I-PDU is gatewayed between two CAN networks, using transport protocol module.
		copy_data();
	}
}

//添加内容
void PduR_CanTpRxIndication(PduIdType id, Std_ReturnType result){
	if(mode == 0){  //verify tp    upper layer tp
		SecOC_TpRxIndication(id, result);
	}
	else if(mode == 1){		// I-PDU from the CanTp module to the DCM module. The reception is made using the transport protocol APIs.
		Dcm_TpRxIndication(PduIdType, Std_ReturnType);
	}
	else if(mode == 2){		// (1)an I-PDU (contained in a SF) received from CAN1 transport protocol and gatewayed to DCM (internal) and gatewayed to CAN2 transport protocol.		/		(2)an (multi N-PDU) I-PDU is gatewayed between two CAN networks, using transport protocol module.
		//do none
	}
	
}




