#include"Std_Types.h"
#include"ComStack_Types.h"

//添加内容
BufReq_ReturnType PduR_CanTpCopyTxData(
 PduIdType id,
 const PduInfoType* info,
 const RetryInfoType* retry,
 PduLengthType* availableDataPtr
);

//添加内容
void PduR_CanTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);

 //添加内容
BufReq_ReturnType PduR_CanTpStartOfReception(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType TpSduLength,
 PduLengthType* bufferSizePtr
);


//添加内容
BufReq_ReturnType PduR_CanTpCopyRxData(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType* bufferSizePtr
);  
 
//添加内容
PduR_CanTpRxIndication(PduIdType, Std_ReturnType);

 






 


