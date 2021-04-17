#include"Std_Types.h"
#include"ComStack_Types.h"

//�������
BufReq_ReturnType PduR_CanTpCopyTxData(
 PduIdType id,
 const PduInfoType* info,
 const RetryInfoType* retry,
 PduLengthType* availableDataPtr
);

//�������
void PduR_CanTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);

 //�������
BufReq_ReturnType PduR_CanTpStartOfReception(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType TpSduLength,
 PduLengthType* bufferSizePtr
);


//�������
BufReq_ReturnType PduR_CanTpCopyRxData(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType* bufferSizePtr
);  
 
//�������
PduR_CanTpRxIndication(PduIdType, Std_ReturnType);

 






 


