#include "FrIf.h"
#include "PduR_FrIf.h"

Std_ReturnType FrIf_Transmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr){

	//���ĵ���û�з����ڴ�
	if(success){
		return E_OK;
	}else{
		return E_NOT_OK;
	}
		
}
	

void frIf_main(){
	
	Std_ReturnType result;
	
	result = PduR_FrIfTriggerTransmit( TxPduId, PduInfoPtr);
	PduR_FrIfTxConfirmation(TxPduId, result);

}
