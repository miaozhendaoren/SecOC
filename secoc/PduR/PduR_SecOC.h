#include"Std_Types.h"
#include"ComStack_Types.h"

Std_ReturnType PduR_SecOCTransmit(PduIdType TxPduId , const PduInfoType* PduInfoPtr);
//PDU的传输请求。

BufReq_ReturnType PduR_SecOCTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);
//调用这个函数来获取I-PDU段(N-PDU)的传输数据。每个对这个函数的调用都会提供I-PDU数据的下一部分，
//除非retry->TpDataState是TP_DATARETRY。
//在本例中，函数将重新启动以从retry->TxTpDataCnt所指示的当前位置的偏移量开始复制数据。
//剩余数据的大小被写入到availableDataPtr所指示的位置。

void PduR_SecOCIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result);
//下层通信接口模块确认PDU的传输或传输PDU失败。

void PduR_SecOCIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr);
//从底层通信接口模块接收到的PDU的指示。

BufReq_ReturnType PduR_SecOCTpStartOfReception(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType TpSduLength,
 PduLengthType* bufferSizePtr
);
//这个函数在接收N-SDU开始时被调用。N-SDU可能被分割成多个N-PDU(带有一个或多个后续CFs的FF)，
//或者可能由单个N-PDU (SF)组成。当TpSduLength为0时，该服务将提供当前可用的最大缓冲区大小。

BufReq_ReturnType PduR_SecOCTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);
//调用这个函数是为了将I-PDU段(N-PDU)接收到的数据提供给上层。对这个函数的每次调用都会提供I-PDU数据的下一部分。
//剩余缓冲区的大小被写入bufferSizePtr所指示的位置。

void PduR_SecOCTpRxIndication(
 PduIdType id,
 Std_ReturnType result
);

//在通过TP API接收到I-PDU后调用，其结果表明传输是否成功。

void PduR_SecOCTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);
//I-PDU在其网络上传输之后调用这个函数，其结果表明传输是否成功。
 
 
 
 //可选

PduR_SecOCCancelTransmit();
//请求取消较低层通信模块中正在进行的PDU传输。

PduR_SecOCCancelReceive();
//请求取消较低层传输协议模块中正在进行的PDU接收进程。








