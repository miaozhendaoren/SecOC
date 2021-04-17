#include "CanTp.h"



Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr){
	//文档没有标注分配内存
	if(success){

		return E_OK;
	}else{  //state!=idle
		return E_NOT_OK;
	}
}


void cantp_tx_main(){
	BufReq_ReturnType bufresult;

	RetryInfoType *retry;
	const PduInfoType* info;
	bufresult = PduR_CanTpCopyTxData(TxPduId,  info,  retry,  availableDataPtr);

	if(bufresult == BUFREQ_OK){
		result = send(); //发送报文
		PduR_CanTpTxConfirmation(TxPduId, result);
	}
}

void cantp_rx_main(){
	
	bufresult = PduR_CanTpStartOfReception(id, info, TpSduLength, bufferSizePtr);
	while(loop){
		bufresult = PduR_CanTpCopyRxData(id, info, bufferSizePtr);
		if(upper layer tp){
			if(opt){
				PduR_CanTpRxIndication(id, result);
			}
		}
	}
	if(verify tp){
		PduR_CanTpRxIndication(id, result);
	}
	
}
