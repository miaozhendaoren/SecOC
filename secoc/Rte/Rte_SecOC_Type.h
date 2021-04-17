#define SECOC_MAX_FRESHNESS_SIZE Elements



uint8 SecOC_FreshnessArrayType[SECOC_MAX_FRESHNESS_SIZE]; 
//uint8 ����

enum SecOC_VerificationResultType{
	SECOC_VERIFICATIONSUCCESS	=0x00,//	��֤�ɹ�
	SECOC_VERIFICATIONFAILURE	=0x01,//	��֤δ�ɹ�
	SECOC_FRESHNESSFAILURE	=0x02,	//���ڴ�������ʶ�ֵ����֤δ�ɹ�
	SECOC_AUTHENTICATIONBUILDFAILURE	=0x03,//	���ڴ���Ĺ��������֤���룬��֤���ɹ�
	SECOC_NO_VERIFICATION	=0x04,//	��������֤�������ѡ���ԭ�����ṩ���ϲ㡣(ֻ����ʹ��SecOCVerifyStatusOverrideʱ�ſ���)
	SECOC_VERIFICATIONFAILURE_OVERWRITTEN=0x05,//	��֤ʧ�ܣ������������PDU�ĸ���״̬��I-PDU�����ݵ��ϲ㡣(ֻ����ʹ��SecOCVerifyStatus Overrideʱ�ſ���)

};

struct SecOC_VerificationStatusType{
	uint16 freshnessValueID;
	SecOC_VerificationResultType verificationStatus;
	uint16 secOCDataId;
};

enum SecOC_OverrideStatusType{
	SECOC_OVERRIDE_DROP_UNTIL_NOTICE =0x00,//	�ڽ�һ��֪֮ͨǰ����ִ����֤����֤(û��CSM����)��I-PDU����������֤�������ΪSECOC_NO_VERIFICATION��
	SECOC_OVERRIDE_DROP_UNTIL_LIMIT	=0x01,//	�ڴﵽNumberOfMessagesToOverride֮ǰ����ִ�������֤����֤����CSM���ã�IPDU����������֤�������ΪSECOC_NO_VERIFICATION.
	SECOC_OVERRIDE_CANCEL	=0x02,//	ȡ������VerifyStatus��
	SECOC_OVERRIDE_PASS_UNTIL_NOTICE	=0x40,//	�ڽ�һ��֪֮ͨǰ��ִ�������֤����֤��IPDU��������֤������͵��ϲ㣬�����֤ʧ�ܣ���֤�������ΪSECOC_VERIFICATIONFAILURE_OVERWRITTEN��
	SECOC_OVERRIDE_SKIP_UNTIL_LIMIT	=0x41,//	�ڴﵽNumberOfMessagesToOverride֮ǰ����ִ�������֤����֤��I-PDU���͵��ϲ㣬��֤�������ΪSECOC_NO_VERIFICATION�����������SecOCRxSecuredPduCollection, SecOcӦ����SecOCRxAuthenticPdu�����ȴ�SecOCRxCryptographicPdu��
	SECOC_OVERRIDE_PASS_UNTIL_LIMIT	=0x42,//	�ڴﵽNumberOfMessagesToOverride֮ǰ��ִ�������֤����֤����I-PDU��������֤������͵��ϲ㣬�����֤ʧ�ܣ�����֤�������ΪSECOC_VERIFICATIONFAILURE_OVERWEITTEN��
	SECOC_OVERRIDE_SKIP_UNTIL_NOTICE	=0x43//	�ڽ�һ��֪֮ͨ�䣬��ִ�������֤����֤��I-PDU���͵��ϲ㣬��֤�������ΪSECOC_NO_VERIFICATION�����������SecOCRxSecuredPduCollection, SecOcӦ����SecOCRxAuthenticPdu�����ȴ�SecOCRxCryptographicPdu��

};
