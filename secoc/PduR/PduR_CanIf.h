#include"Std_Types.h"
#include"ComStack_Types.h"


void PduR_CanIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result);

void PduR_CanIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

////ÐÂÌí¼Ó
//PduR_<Canif>RxIndication(PduIdType, const PduInfoType*);
//	
//PduR_<Canif>TxConfirmation(PduIdType, Std_ReturnType);