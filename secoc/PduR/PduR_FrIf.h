#include"Std_Types.h"
#include"ComStack_Types.h"


//ÄÚÈÝÌí¼Ó

Std_ReturnType PduR_FrIfTriggerTransmit(PduIdType TxPduId,  PduInfoType* PduInfoPtr);

void PduR_FrIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result);


void PduR_FrIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr);




