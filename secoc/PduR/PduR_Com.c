#include "PduR_Com.h"
#include "ComStack_Types.h"
#include "SecOC_Types.h"


unsigned char mode =0; //鍒ゅ埆璋冪敤鐨凷ecoc鎺ュ彛

extern const SecOCTxPduProcessing_type* tmpSecOCTxPduProcessing;


Std_ReturnType PduR_ComTransmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr){
	uint8 num=0;
	for(num=0; num<SECOC_NUM_OF_TX_IPDU; num++){
		SecOCTxPduProcessing_type cur = *(tmpSecOCTxPduProcessing+num);
		if(cur.SecOCTxAuthenticPduLayer.SecOCTxAuthenticLayerPduId == TxPduId){
			if(cur->SecOCTxAuthenticPduLayer->SecOCPduType == SECOC_IFPDU){
				return SecOC_IfTransmit(TxPduId, PduInfoPtr );
			}
			return SecOC_TpTransmit(TxPduId, PduInfoPtr );
		}
	}
	return E_NOT_OK;
	
}




