#include"Std_Types.h"
#include"ComStack_Types.h"

Std_ReturnType PduR_SecOCTransmit(PduIdType TxPduId , const PduInfoType* PduInfoPtr);
//PDU�Ĵ�������

BufReq_ReturnType PduR_SecOCTpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);
//���������������ȡI-PDU��(N-PDU)�Ĵ������ݡ�ÿ������������ĵ��ö����ṩI-PDU���ݵ���һ���֣�
//����retry->TpDataState��TP_DATARETRY��
//�ڱ����У����������������Դ�retry->TxTpDataCnt��ָʾ�ĵ�ǰλ�õ�ƫ������ʼ�������ݡ�
//ʣ�����ݵĴ�С��д�뵽availableDataPtr��ָʾ��λ�á�

void PduR_SecOCIfTxConfirmation(PduIdType TxPduId, Std_ReturnType result);
//�²�ͨ�Žӿ�ģ��ȷ��PDU�Ĵ������PDUʧ�ܡ�

void PduR_SecOCIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr);
//�ӵײ�ͨ�Žӿ�ģ����յ���PDU��ָʾ��

BufReq_ReturnType PduR_SecOCTpStartOfReception(
 PduIdType id,
 const PduInfoType* info,
 PduLengthType TpSduLength,
 PduLengthType* bufferSizePtr
);
//��������ڽ���N-SDU��ʼʱ�����á�N-SDU���ܱ��ָ�ɶ��N-PDU(����һ����������CFs��FF)��
//���߿����ɵ���N-PDU (SF)��ɡ���TpSduLengthΪ0ʱ���÷����ṩ��ǰ���õ���󻺳�����С��

BufReq_ReturnType PduR_SecOCTpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);
//�������������Ϊ�˽�I-PDU��(N-PDU)���յ��������ṩ���ϲ㡣�����������ÿ�ε��ö����ṩI-PDU���ݵ���һ���֡�
//ʣ�໺�����Ĵ�С��д��bufferSizePtr��ָʾ��λ�á�

void PduR_SecOCTpRxIndication(
 PduIdType id,
 Std_ReturnType result
);

//��ͨ��TP API���յ�I-PDU����ã��������������Ƿ�ɹ���

void PduR_SecOCTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);
//I-PDU���������ϴ���֮���������������������������Ƿ�ɹ���
 
 
 
 //��ѡ

PduR_SecOCCancelTransmit();
//����ȡ���ϵͲ�ͨ��ģ�������ڽ��е�PDU���䡣

PduR_SecOCCancelReceive();
//����ȡ���ϵͲ㴫��Э��ģ�������ڽ��е�PDU���ս��̡�








