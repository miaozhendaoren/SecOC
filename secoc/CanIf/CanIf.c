#include "CanIf.h"


Std_ReturnType CanIf_Transmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr){
	uint8 success=1;
	copyBuffer();//补充具体地过程
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

void canif_rx_main(){  //canif收到报文后将数据传输给pdur  未来可能通过中断实现调用此函数
	PduR_CanIfRxIndication(RxPduId, PduInfoPtr);
	
	

}
