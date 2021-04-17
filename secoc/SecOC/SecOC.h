
#ifndef _SECOC_H_
#define _SECOC_H_

#include "SecOC_Cfg.h"









 

//Development Error Types
#define SECOC_E_PARAM_POINTER  			0x01
#define SECOC_E_UNINIT 					0x02
#define SECOC_E_INVALID_PDU_SDU_ID		0x03
#define SECOC_E_INIT_FAILED				0x07
#define SECOC_E_CRYPTO_FAILURE			0x04
//Runtime Error Types
#define SECOC_E_FRESHNESS_FAILURE		0x08






typedef struct { //�ض�ʵ�����ݽṹ      ����SecOCģ�����ݽṹ
	const SecOCGeneral_type general;
	const SecOCTxPduProcessing_type* secOCTxPduProcessings;
	const SecOCRxPduProcessing_type* secOCRxPduProcessings;
}SecOC_ConfigType;


typedef enum {   //SecOC״̬
	SECOC_UNINIT,   //SecOCδ��ʼ��
	SECOC_INIT		//SecOC��ʼ��
}SecOC_StateType;


SecOCMainFunctionPeriodRx; //��������
SecOCMainFunctionPeriodTx;  //��������




void SecOC_Init (const SecOC_ConfigType* config);
/*
0x01 ͬ�� ������
��ʼ��SECOCģ�顣�ɹ���ʼ���ᵼ��״̬SEC OC_INIT��
*/

void SecOC_DeInit (void);   //0x05 ͬ�� ��������
//�÷���ֹͣ��ȫ����ͨ�š����л����I-PDU����ɾ���������Ҫ��
//�����ڵ���SecOC_Init֮���ٴλ�á�ͨ������SecOC_DeInit��
//��AUTOSAR SecOCģ������δ��ʼ��״̬(SecOC_UNINIT)��
//���ô˺������������internal global variables �� i-pdu����


void SecOC_GetVersionInfo (Std_VersionInfoType* versioninfo);//0x02 ͬ�� ������


Std_ReturnType SecOC_IfTransmit ( PduIdType TxPduId, const PduInfoType* PduInfoPtr );
//0x49 ͬ�� �����벻ͬ��PduId������������ͬ��PduId�� �ο�7.4
//E_OK: ���������ѱ����ܡ�
//E_NOT_OK: ��������δ������


Std_ReturnType SecOC_TpTransmit ( PduIdType TxPduId, const PduInfoType* PduInfoPtr );
//0x49 ͬ�� �����벻ͬ��PduId������������ͬ��PduId�� �ο�7.4
//E_OK: ���������ѱ����ܡ�
//E_NOT_OK: ��������δ������

Std_ReturnType SecOC_IfCancelTransmit (PduIdType TxPduId );
//0x4a ͬ�� �����벻ͬ��PduId������������ͬ��PduId��
//E_OK:Ŀ��ģ��ɹ�ִ��ȡ����
//E_NOT_OK: Ŀ��ģ��ܾ�ȡ����
//����ȡ���ϵͲ�ͨ��ģ�������ڽ��е�PDU���䡣

Std_ReturnType SecOC_TpCancelTransmit ( PduIdType TxPduId );
//0x4a ͬ�� �����벻ͬ��PduId������������ͬ��PduId��
//E_OK:Ŀ��ģ��ɹ�ִ��ȡ����
//E_NOT_OK: Ŀ��ģ��ܾ�ȡ����
//����ȡ���ϵͲ�ͨ��ģ�������ڽ��е�PDU���䡣

Std_ReturnType SecOC_TpCancelReceive ( PduIdType RxPduId );
//0x4c ͬ�� ��������
//E_OK:Ŀ��ģ��ɹ�ִ��ȡ����
//E_NOT_OK: Ŀ��ģ��ܾ�ȡ����
//����ȡ���ϵͲ㴫��Э��ģ�������ڽ��е�PDU���ա�


//Optional Interface
Std_ReturnType SecOC_VerifyStatusOverride ( 
	uint16 ValueID,
	SecOC_OverrideStatusType overrideStatus,
	uint8 numberOfMessagesToOverride 
);
//0x0b ͬ�� �����벻ͬ��FreshnessValueID������������ͬ��FreshnessValueID��
//E_OK:����ɹ���
//E_NOT_OK: ����ʧ�ܡ�
//8.3.9

Std_ReturnType SecOC_SendDefaultAuthenticationInformation ( 
	uint16 FreshnessValueID, 
	boolean sendDefaultAuthenticationInformation 
);
//0x04 ͬ�� �����벻ͬ��FreshnessValueID������������ͬ��FreshnessValueID��
//E_OK:����ɹ���
//E_NOT_OK: ����ʧ�ܡ�
//8.3.9



//�ص�֪ͨ
void SecOC_RxIndication ( 
	PduIdType RxPduId, 
	const PduInfoType* PduInfoPtr 
);
//0x42 ͬ�� �����벻ͬ��PduId������������ͬ��PduId��
//�ӵײ�ͨ�Žӿ�ģ���յ���PDUָʾ

void SecOC_TpRxIndication ( 
	PduIdType id, 
	Std_ReturnType result 
);
//0x45 ͬ�� ������
//ͨ��TP API���յ�I-PDU֮����ø÷���������ʾ�����Ƿ�ɹ���

void SecOC_TxConfirmation (PduIdType TxPduId, Std_ReturnType result);
//0x40 ͬ�� �����벻ͬ��PduId������������ͬ��PduId��
//�²�ͨ�Žӿ�ģ��ȷ��PDU�Ĵ������PDUʧ�ܡ�

void SecOC_TpTxConfirmation ( 
	PduIdType id, 
	Std_ReturnType result 
);
//0x48 ͬ�� ������
//I-PDU���������ϴ���֮����øú������������������Ƿ�ɹ���

Std_ReturnType SecOC_TriggerTransmit ( 
	PduIdType TxPduId, 
	PduInfoType* PduInfoPtr 
);
//0x41 ͬ�� �����벻ͬPduId������������ͬ��PduId��
//�����API�У��ϲ�ģ��(������ģ��)Ӧ�ü����������Ƿ����PduInfoPtr->SduLength�ϱ��Ļ�������С��
//������ϣ��������ݸ��Ƶ�PduInfoPtr->SduDataPtr�ṩ�Ļ������У�����PduInfoPtr->SduLength�и���ʵ�ʸ��Ƶ����ݳ��ȡ�
//��������ϣ�������E_NOT_OK�����ı�PduInfoPtr��

BufReq_ReturnType SecOC_CopyRxData ( 
	PduIdType id, 
	const PduInfoType* info, 
	PduLengthType* bufferSizePtr   //���
);
//0x44 ͬ�� ������
//BUFREQ_OK�����ݸ��Ƴɹ���
//BUFREQ_E_NOT_OK�����ڷ�����������δ�����ơ�

BufReq_ReturnType SecOC_CopyTxData ( 
	PduIdType id, 
	const PduInfoType* info, 
	const RetryInfoType* retry, 
	PduLengthType* availableDataPtr  //���
);
//0x43 ͬ�� ������
//BUFREQ_OK����������ȫ��Ҫ���Ƶ����仺������
//BUFREQ_E_BUSY�������ܱ����㣬��Ϊ�����Tx�����������á��²�ģ������Ժ����Դ˵��á�û�и����κ����ݡ�
//BUFREQ_E_NOT_OK������δ�����ơ�����ʧ�ܡ�
//���ñ���������ȡI-PDU��(N-PDU)�Ĵ������ݡ�����retry->Tp DataStateΪTp_DATARETRY��
//����ÿ�ε��ñ����������ṩI-PDU���ݵ���һ���֡��ڱ����У������������������������ݣ�
//���retry->TxTpDataCnt��ָʾ�ĵ�ǰλ�õ�ƫ������ʼ���ơ�
//ʣ�����ݵĴ�С��д�뵽availableDataPtr��ָʾ��λ����ȥ��

BufReq_ReturnType SecOC_StartOfReception ( 
	PduIdType id, 
	const PduInfoType* info, 
	PduLengthType TpSduLength, 
	PduLengthType* bufferSizePtr 
);
//0x46 ͬ�� ������
//BUFREQ_OK�������ѱ����ܡ�bufferSizePtr��ʾ���õĽ��ջ����������ռ��������û�п��õ������С�Ļ���������bufferSizePtr����ʾ���ջ�������СΪ0��
//BUFREQ_E_NOT_OK�����ӱ��ܾ������ս�����ֹ��bufferSizePtr���ֲ��䡣
//BUFREQ_E_OVFL���޷��ṩ���賤�ȵĻ����������ܽ�����ֹ��bufferSizePtr���ֲ��䡣
//�������ڽ���N-SDU��ʼʱ�����á�N-SDU���ܱ��ָ�ɶ��N-PDU(����һ����������CF��FF)��
//���߿����ɵ���N-PDU (SF)��ɡ���TpSduLengthΪ0ʱ���÷����ṩ��ǰ���õ���󻺳�����С��
//�������SecOC_StartOfReceptionʱTpSduLengthequalΪ0����SecOCģ�齫����BUFREQ_E_NOT_OK��������ȡ��һ�����ж���


//��������
//��������ECU�����ڼ������ӵ�SecOC�Ĵ���Ƭ�Ρ��������������������д�Ĵ���
Std_ReturnType SecOC_GetRxFreshness ( 
	uint16 SecOCFreshnessValueID, 
	const uint8* SecOCTruncatedFreshnessValue, 
	uint32 SecOCTruncatedFreshnessValueLength, 
	uint16 SecOCAuthVerifyAttempts, 
	uint8* SecOCFreshnessValue,    //���
	uint32* SecOCFreshnessValueLength    //���
);
//0x4f ͬ�� ������
//E_OK������ɹ�
//E_NOT_OK������ʧ�ܣ��������ʶȻ����FreshnessValueId���ձ����⣬�޷��ṩ���ʶ�ֵ��
//E_BUSY�����ʶ���Ϣ��ʱ�޷��ṩ��
//SecOCʹ�øýӿ�����ȡ��ǰ�����ʶ�ֵ��

Std_ReturnType SecOC_GetRxFreshnessAuthData ( 
	uint16 SecOCFreshnessValueID, 
	const uint8* SecOCTruncatedFreshnessValue, 
	uint32 SecOCTruncatedFreshnessValueLength, 
	const uint8* SecOCAuthDataFreshnessValue,     //�ò��������˽��յ�����δ���������֤��PDU��һ���֡��ò����ǿ�ѡ��(�μ�����)��
	uint16 SecOCAuthDataFreshnessValueLength,    //��λΪ��λ������authentic PDU�����ʶȵĳ��ȡ��ò����ǿ�ѡ��(�μ�����)��
	uint16 SecOCAuthVerifyAttempts, 
	uint8* SecOCFreshnessValue,        //���
	uint32* SecOCFreshnessValueLength    //���
);
//0x4e ͬ�� ������
//E_OK������ɹ�
//E_NOT_OK������ʧ�ܣ��������ʶȻ����FreshnessValueId���ձ����⣬�޷��ṩ���ʶ�ֵ��
//E_BUSY�����ʶ���Ϣ��ʱ�޷��ṩ��
//SecOCʹ�øýӿ�����ȡ��ǰ�����ʶ�ֵ��

Std_ReturnType SecOC_GetTxFreshness ( 
	uint16 SecOCFreshnessValueID, 
	uint8* SecOCFreshnessValue,    //���
	uint32* SecOCFreshnessValueLength 
);
//0x52 ͬ�� ������
//E_OK������ɹ�
//E_NOT_OK������ʧ�ܣ��������ʶȻ����FreshnessValueId���ձ����⣬�޷��ṩ���ʶ�ֵ��
//E_BUSY�����ʶ���Ϣ��ʱ�޷��ṩ��
//���API������(SecOCFreshnessValue)��һ���ֽڵ�����Чλ�Դ���ֽڸ�ʽ�������ʶ�ֵ��

Std_ReturnType SecOC_GetTxFreshnessTruncData ( 
	uint16 SecOCFreshnessValueID, 
	uint8* SecOCFreshnessValue, 
	uint32* SecOCFreshnessValueLength, 
	uint8* SecOCTruncatedFreshnessValue,  
	uint32* SecOCTruncatedFreshnessValueLength 
);
//0x51 ͬ�� ������
//E_OK������ɹ�
//E_NOT_OK������ʧ�ܣ��������ʶȻ����FreshnessValueId���ձ����⣬�޷��ṩ���ʶ�ֵ��
//E_BUSY�����ʶ���Ϣ��ʱ�޷��ṩ��
//SecOCʹ������ӿ�����ȡ��ǰ�����ʶ�ֵ���ӿں������ṩ����Secured I-PDU�д���Ľض����ʶȡ�

void SecOC_SPduTxConfirmation ( 
	uint16 SecOCFreshnessValueID 
);
//0x4d ͬ�� ������
//SecOC���øýӿ�ȥ��ʾSecured I-PDU�Ѿ���ʼ���д��䡣







//
////8.8����ӿ�
///*nterfaces
//described here will be visible on the VFB and are used to generate the Rte between
//application software and the SecOC module.
//*/
//SecOC_VerificationStatusType VerificationStatus;
//
//VerifyStatusConfiguration(
//	VerifyStatusOverride[  //operation
//		uint16 ValueId,
//		SecOC_OverrideStatusType overrideStatus,
//		uint8 numberOfMessagesToOverride
//	]
//);
////E_OK �����ɹ�
////E_NOT_OK ����ʧ��
//
//FreshnessManagement(
//
//GetRxFreshness[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetRxFreshnessAuthData[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//SecOC_FreshnessArrayType authenticDataFreshnessValue,
//uint16 authenticDataFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetTxFreshness[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//GetTxFreshnessTruncData[
//uint16 freshnessValueId,
//SecOC_FreshnessArrayType truncatedFreshnessValue,
//uint32 truncatedFreshnessValueLength,
//uint16 authVerifyAttempts,
//SecOC_FreshnessArrayType freshnessValue,
//uint32 freshnessValueLength
//],
//
//SPduTxConfirmation[
//uint16 freshnessValueId
//]//0	E_OK
//
//
//);
////0	E_OK	�����ɹ�
////1	E_NOT_OK	����ʧ��
////2	E_BUSY	������ʱʧ�ܣ�����ֵ��ǰ�޷��ṩ��



#endif
