#define SECOC_MAX_FRESHNESS_SIZE Elements



uint8 SecOC_FreshnessArrayType[SECOC_MAX_FRESHNESS_SIZE]; 
//uint8 数组

enum SecOC_VerificationResultType{
	SECOC_VERIFICATIONSUCCESS	=0x00,//	验证成功
	SECOC_VERIFICATIONFAILURE	=0x01,//	验证未成功
	SECOC_FRESHNESSFAILURE	=0x02,	//由于错误的新鲜度值，验证未成功
	SECOC_AUTHENTICATIONBUILDFAILURE	=0x03,//	由于错误的构建身份验证代码，验证不成功
	SECOC_NO_VERIFICATION	=0x04,//	已跳过验证，数据已“按原样”提供给上层。(只有在使用SecOCVerifyStatusOverride时才可能)
	SECOC_VERIFICATIONFAILURE_OVERWRITTEN=0x05,//	验证失败，但是由于这个PDU的覆盖状态，I-PDU被传递到上层。(只有在使用SecOCVerifyStatus Override时才可能)

};

struct SecOC_VerificationStatusType{
	uint16 freshnessValueID;
	SecOC_VerificationResultType verificationStatus;
	uint16 secOCDataId;
};

enum SecOC_OverrideStatusType{
	SECOC_OVERRIDE_DROP_UNTIL_NOTICE =0x00,//	在进一步通知之前，不执行验证器验证(没有CSM调用)，I-PDU被丢弃，验证结果设置为SECOC_NO_VERIFICATION。
	SECOC_OVERRIDE_DROP_UNTIL_LIMIT	=0x01,//	在达到NumberOfMessagesToOverride之前，不执行身份验证器验证（无CSM调用）IPDU被丢弃，验证结果设置为SECOC_NO_VERIFICATION.
	SECOC_OVERRIDE_CANCEL	=0x02,//	取消覆盖VerifyStatus。
	SECOC_OVERRIDE_PASS_UNTIL_NOTICE	=0x40,//	在进一步通知之前，执行身份验证器验证，IPDU独立于验证结果发送到上层，如果验证失败，验证结果设置为SECOC_VERIFICATIONFAILURE_OVERWRITTEN。
	SECOC_OVERRIDE_SKIP_UNTIL_LIMIT	=0x41,//	在达到NumberOfMessagesToOverride之前，不执行身份验证者验证，I-PDU发送到上层，验证结果设置为SECOC_NO_VERIFICATION。如果配置了SecOCRxSecuredPduCollection, SecOc应处理SecOCRxAuthenticPdu而不等待SecOCRxCryptographicPdu。
	SECOC_OVERRIDE_PASS_UNTIL_LIMIT	=0x42,//	在达到NumberOfMessagesToOverride之前，执行身份验证者验证，将I-PDU独立于验证结果发送到上层，如果验证失败，将验证结果设置为SECOC_VERIFICATIONFAILURE_OVERWEITTEN。
	SECOC_OVERRIDE_SKIP_UNTIL_NOTICE	=0x43//	在进一步通知之间，不执行身份验证器验证，I-PDU发送到上层，验证结果设置为SECOC_NO_VERIFICATION。如果配置了SecOCRxSecuredPduCollection, SecOc应处理SecOCRxAuthenticPdu而不等待SecOCRxCryptographicPdu。

};
