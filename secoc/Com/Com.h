#include "Std_Types.h"
#include "ComStack_Types.h"

void Com_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

void Com_TpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);


BufReq_ReturnType Com_CopyTxData (
 PduIdType id,
 const PduInfoType* info,
 const RetryInfoType* retry,
 PduLengthType* availableDataPtr
);

void Com_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

BufReq_ReturnType Com_StartOfReception (
 PduIdType id,
 const PduInfoType* info,
 PduLengthType TpSduLength,
 PduLengthType* bufferSizePtr
);


BufReq_ReturnType Com_CopyRxData (
 PduIdType id,
 const PduInfoType* info,
 PduLengthType* bufferSizePtr
);

 
void Com_TpRxIndication ( PduIdType id, Std_ReturnType result);

 

