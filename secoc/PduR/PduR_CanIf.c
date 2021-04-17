



#include "PduR_CanIf.h"
#include "SecOC.h"

unsigned char mode=0; //暂时设定为控制认证的模式

void PduR_CanIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result){
//	if(mode == 0){
//		SecOC_TxConfirmation( PduIdType TxPduId, Std_ReturnType result );
//	}
//	else if(mode == 1){		// I-PDU from the COM module to the CanIf module
//		Com_TxConfirmation(PduIdType, Std_ReturnType);
//	}
//	else if(mode == 2){		// an I-PDU is gatewayed between two CAN networks (CAN1 and CAN2) using CanIf.	/		 an I-PDU is gatewayed from CAN1 to CAN2 and also received locally by the COM module.
//		// do none
//	}
	SecOC_TxConfirmation( TxPduId,  result );

}


void PduR_CanIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr){
//	if(mode == 0){ //direct || re-authentcation
//		SecOC_RxIndication (RxPduId, PduInfoPtr );
//	}
//	else if(mode == 1){		// I-PDU from the CanIf module to the COM module
//		Com_RxIndication(PduIdType, const PduInfoType*);
//	}
//	else if(mode == 2){		// an I-PDU is gatewayed between two CAN networks (CAN1 and CAN2) using CanIf.
//		CanIf_Transmit(Std_ReturnType, PduIdType, const PduInfoType*);
//	}
//	else if(mode == 3){		//an I-PDU is gatewayed between CAN and FlexRay, using trigger transmit (with buffering and without buffering).
//		copy buffer();
//	}
//	else if(mode == 4){		//an I-PDU is gatewayed from CAN to LIN, using trigger transmit (LIN sporadic frame and all other LIN frame types).
//		buffer IPDU();
//	}
//	else if(mode == 5){		//an I-PDU is gatewayed from CAN1 to CAN2 and also received locally by the COM module.
//		Com_RxIndication(PduIdType,const PduInfoType*);
//	}

	SecOC_RxIndication (RxPduId, PduInfoPtr );
}







