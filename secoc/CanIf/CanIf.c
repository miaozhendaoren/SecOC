#include "CanIf.h"


Std_ReturnType CanIf_Transmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr){
	uint8 success=1;
	copyBuffer();//�������ع���
	if(success){
		return E_OK;
	}else{
		return E_NOT_OK;
	}
}


void canif_tx_main(){
	result = send(TxPduId);
	PduR_CanIfTxConfirmation(TxPduId, result);
}

void canif_rx_main(){  //canif�յ����ĺ����ݴ����pdur  δ������ͨ���ж�ʵ�ֵ��ô˺���
	PduR_CanIfRxIndication(RxPduId, PduInfoPtr);
	
	

}
