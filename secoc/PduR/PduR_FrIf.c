#include "PduR_FrIf.h"

unsigned char mode=0; //暂时设定为控制认证的模式


Std_ReturnType PduR_FrIfTriggerTransmit(PduIdType TxPduId,  PduInfoType* PduInfoPtr){
//	if(mode==0){
//		return SecOC_TriggerTransmit ( TxPduId,  PduInfoPtr );
//	}else if(mode =1){	//I-PDU from the COM module to the FrIf module using trigger transmit.
//		return Com_TriggerTransmit(TxPduId, PduInfoPtr);
//	}


	return SecOC_TriggerTransmit ( TxPduId,  PduInfoPtr );
}


void PduR_FrIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result){	
//	if(mode == 0){ // triggered trans
//		SecOC_TxConfirmation(TxPduId, result);
//	}
//	else if(mode == 1){		//I-PDU from the COM module to the FrIf module using trigger transmit.
//		Com_TxConfirmation(PduIdType, Std_ReturnType);
//	}
//	else if(mode == 2){		//an I-PDU is gatewayed between CAN and FlexRay, using trigger transmit (with buffering and without buffering).
//		//do none;
//	}
	SecOC_TxConfirmation(TxPduId, result);
}


//非secoc模块相关api

void PduR_FrIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr){			// I-PDU from the FrIf module to the COM module.
	Com_RxIndication(RxPduId, PduInfoPtr);
}





