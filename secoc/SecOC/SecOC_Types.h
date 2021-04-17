


#ifndef _SECOC_TYPES_H_
#define _SECOC_TYPES_H_


#include "ComStack_Types.h"

typedef enum {
	CFUNC,
	RTE
}SecOCQueryFreshnessValue_type;


typedef enum {
	SECOC_IFPDU,
  SECOC_TPPDU
}SecOCPduType_type;





typedef struct{
	boolean EcucDefaultBswPartition;
	//0..1
	
	boolean PartitionCanBeRestarted;
	//1
	
//	EcucEcuPartitionRef; //Foreign reference to ECU-PARTITION
//	//0..1
	
//	EcucPartitionBswModuleDistinguishedPartition; //Foreign reference to BSW-DISTINGUISHED-PARTITION
//	0..*
	
//	EcucPartitionSoftwareComponentInstanceRef;
//Instance reference to SW-COMPONENT-PROTOTYPE context: ROO
//T-SW-COMPOSITION-PROTOTYPE
	//0..*

}EcucPartition_type;  //应在ecu_cfg.c中



typedef struct{
	const uint8 SecOCIPduSize;
	const uint8 SecOCIPduSt;
	uint8 SecOCIPducurpos;
	uint8 lock;
	void *const SecOCIpduDataPtr;
	uint8 SecOCIPdulen;
}SecOCPdu_type;


typedef struct{
	uint8 abc;
	uint8 avac;
}SecOCPduCnt_type;

typedef struct{
	uint32 SecOCBufferLength;   //0-4294967295
	//1
}SecOCSameBufferPduCollection_type; //0..*




typedef struct{
	uint32 CsmJobId;
	//1
	
//	CsmJobInterfaceUsePort_type CsmJobInterfaceUsePort;
//	//1    [CRYPTO_USE_FNC  / CRYPTO_USE_PORT / CRYPTO_USE_PORT_OPTIMIZED]
//	
//	boolean CsmJobPrimitiveCallbackUpdateNotification;
//	//0..1
//	
//	uint32 CsmJobPriority;
//	//1
//	
//	CsmProcessingMode_type CsmProcessingMode;
//	//1   [CRYPTO_PROCESSING_ASYNC/ CRYPTO_PROCESSING_SYNC]
//	
//	const CsmInOutRedirections_type * const CsmInOutRedirectionRef;
//	//0..1
//	
//	const CsmKey_type *const CsmJobKeyRef;
//	//1
//	
//	const CsmCallback_type *const CsmJobPrimitiveCallbackRef;
//	//0..1 
//	
//	const CsmPrimitives_type *const CsmJobPrimitiveRef;
//	//1
//	
//	const CsmQueue_type *const CsmJobQueueRef;
//	//1
	
}CsmJob_type; //此结构体可能在csm模块中


//typedef struct{
//	boolean DynamicLength;
//	//0..1
//	boolean J1939Requestable;
//	//0..1
	
//	uint32 PduLength;
	//1
	
//	const EcucPartition *const EcucPduDefaultPartitionRef;
//	//0..1;
//	
//	const MetaDataType_type *const MetaDataTypeRef;
//	//0..1
//	
//	SysTPduToFrameTriggeringRef;
//	//0..1
//	//Foreign reference to FRAME-TRIGGERING
//	
//	SysTPduToPduTriggeringRef;
//	//0..1 Foreign reference to PDU-TRIGGERING
//	
//	const EcucPduDedicatedPartition_type *const *EcucPduDedicatedPartition;
//	//0..*
	
	
}Pdu_type;   //此结构可能存在pdu模块中

typedef struct{
	SecOCPduType_type SecOCPduType;
	//1
	
	uint16 SecOCTxAuthenticLayerPduId;
	//1
	
	const SecOCPdu_type  * const SecOCTxAuthenticLayerPduRef;
	//1
	
}SecOCTxAuthenticPduLayer_type;

typedef struct{
	uint8 SecOCAuthPduHeaderLength; //[0 .. 4]
	//0..1
	
	uint16 SecOCTxSecuredLayerPduId;
	//1
	
	const SecOCPdu_type  * const SecOCTxSecuredLayerPduRef;
	//1
	
}SecOCTxSecuredPdu_type;


typedef struct{
	uint8 SecOCAuthPduHeaderLength;  //[0..4]
	//0..1
	
	uint16 SecOCTxAuthenticPduId;
	//1
	
	const SecOCPdu_type  * const SecOCTxAuthenticPduRef;
	//1
	
}SecOCTxAuthenticPdu_type;

typedef struct{
  uint16 SecOCTxCryptographicPduId;
	//1
	
	const SecOCPdu_type  * const SecOCTxCryptographicPduRef;
	//1
	
}SecOCTxCryptographicPdu_type;

typedef struct{
	uint16 SecOCMessageLinkLen;
	/* Length of the Message Linker inside the Authentic I-PDU in bits.
	1
	*/
	
	uint16 SecOCMessageLinkPos;
	/* The position of the Message Linker inside the Authentic I-PDU in bits.
	1
	*/
	
}SecOCUseMessageLink_type;

typedef struct{
	const SecOCTxAuthenticPdu_type SecOCTxAuthenticPdu;
	//1
	
	const SecOCTxCryptographicPdu_type SecOCTxCryptographicPdu;
	//1
	
	const SecOCUseMessageLink_type SecOCUseMessageLink;
	//0..1
}SecOCTxSecuredPduCollection_type;

typedef struct{
	const SecOCTxSecuredPdu_type SecOCTxSecuredPdu; //[0..4]
	//0..1
	
	const SecOCTxSecuredPduCollection_type SecOCTxSecuredPduCollection;
	//0..1
}SecOCTxSecuredPduLayer_type;

typedef struct{
	uint32 SecOCSecuredTxPduLength;
	/*This parameter defines the length (in bytes) of the area within the Pdu
which shall be secured
	1
	*/
	
	uint32 SecOCSecuredTxPduOffset;
	/*This parameter defines the start position (offset in bytes) of the area within
the Pdu which shall be secured
	1
	*/
}SecOCTxPduSecuredArea_type;

typedef struct{
	uint8 SecOCDefaultAuthenticationInformationPattern;  //[0..255]
	//0..1

	boolean SecOCDevErrorDetect; 
	//1

	boolean SecOCEnableForcedPassOverride; 
	//1
	
	boolean SecOCIgnoreVerificationResult;
	//1	
	
	uint64 SecOCMainFunctionPeriodRx;  //[0..inf]
	//1
	
	uint64 SecOCMainFunctionPeriodTx; //[0..inf]
	//1
	
	uint8 *SecOCMaxAlignScalarType;  //String  如何表示
	//1
	
	boolean SecOCOverrideStatusWithDataId;
	//0..1
	
	SecOCQueryFreshnessValue_type SecOCQueryFreshnessValue;  //[CFUNC/RTE]
	//1
	
	void (*SecOCVerificationStatusCallout) (void);
	//0..*
	
	boolean SecOCVersionInfoApi;
	//1
	
	const EcucPartition_type * const SecOCEcucPartitionRef;
	//0..1

}SecOCGeneral_type;  
//1



typedef struct{
	uint16 SecOCAuthenticationBuildAttempts;
	//0..1
	
	uint16 SecOCAuthInfoTruncLength;
	//1
	
	uint16 SecOCDataId;
	//1
	
	uint16 SecOCFreshnessValueId; 
	//1
	
	uint8 SecOCFreshnessValueLength;
	//1
	
	uint8 SecOCFreshnessValueTruncLength; //0 .. 64
	//1
	
	boolean SecOCProvideTxTruncatedFreshnessValue;
	//1
	
	boolean SecOCUseTxConfirmation;
	//0..1
	
	const SecOCSameBufferPduCollection_type * const SecOCSameBufferPduRef;
	//0..1
	
	const CsmJob_type * const SecOCTxAuthServiceConfigRef;  //?
	//1
	
	const SecOCTxAuthenticPduLayer_type SecOCTxAuthenticPduLayer;
	//1
	
	const SecOCTxSecuredPduLayer_type SecOCTxSecuredPduLayer;
	//1
	
	const SecOCTxPduSecuredArea_type SecOCTxPduSecuredArea;
	//0..1
	
}SecOCTxPduProcessing_type;  //0..*









typedef struct{
	uint8 SecOCAuthPduHeaderLength;		//[0..4]
	//0..1

	uint16 SecOCRxSecuredLayerPduId;		//[0..65535]
	//1

	boolean SecOCSecuredRxPduVerification;
	//1

	const SecOCPdu_type * const SecOCRxSecuredLayerPduRef;
	//1
}SecOCRxSecuredPdu_type;

typedef struct{
	uint8 SecOCAuthPduHeaderLength;		//[0..4]
	//0..1

	uint16 SecOCRxAuthenticPduId;		//[0..65535]
	//1

	const SecOCPdu_type * const SecOCRxAuthenticPduRef;
	//1


}SecOCRxAuthenticPdu_type;


typedef struct{
	uint16 SecOCRxCryptographicPduId;		//[0..65535]
	//1

	const SecOCPdu_type * const SecOCRxCryptographicPduRef;
	//1

}SecOCRxCryptographicPdu_type;


typedef struct{
	boolean SecOCSecuredRxPduVerification;
	//1

	const SecOCRxAuthenticPdu_type SecOCRxAuthenticPdu;
	//1

	const SecOCRxCryptographicPdu_type SecOCRxCryptographicPdu;
	//1

	const SecOCUseMessageLink_type SecOCUseMessageLink;
	//0..1


}SecOCRxSecuredPduCollection_type;

typedef struct{
	const SecOCRxSecuredPdu_type SecOCRxSecuredPdu;
	//0..1

	const SecOCRxSecuredPduCollection_type SecOCRxSecuredPduCollection;
	//0..1


}SecOCRxSecuredPduLayer_type;






typedef struct{
	SecOCQueryFreshnessValue_type SecOCPduType;		//[SECOC_IFPDU/SECOC_TPPDU] 枚举类型
	//1

	const SecOCPdu_type * const SecOCRxAuthenticLayerPduRef;
	//1


}SecOCRxAuthenticPduLayer_type;






typedef struct{
	uint32 SecOCSecuredRxPduLength;		//[0..4294967295]
	//1 This parameter defines the length (in bytes) of the area within the Pdu which is secured


	uint32 SecOCSecuredRxPduOffset;		//[0..4294967295]
	//1 This parameter defines the start position (offset in bytes) of the area within the Pdu which is secured



}SecOCRxPduSecuredArea_type;  //决定哪段参与计算mac  若不存在则全部authentic
//0..1



typedef struct{
	uint16 SecOCAuthDataFreshnessLen;		//[0..65535](default value --)
	//0..1

	uint16 SecOCAuthDataFreshnessStartPosition;		//[0..65535](default value --)
	//0..1

	uint16 SecOCAuthenticationBuildAttempts;		//[0..65535](default value --)
	//0..1

	uint16 SecOCAuthenticationVerifyAttempts;		//[0..65535](default value 0)
	//0..1

	uint16 SecOCAuthInfoTruncLength;		//[1..65535](default value --)
	//1

	uint16 SecOCDataId;		//[0..65535](default value --)
	//1

	uint16 SecOCFreshnessValueId;		//[0..65535](default value --)
	//1

	uint8 SecOCFreshnessValueLength;		//[0..64](default value --)
	//1

	uint8 SecOCFreshnessValueTruncLength;		//[0..64](default value --)
	//1

	SecOCQueryFreshnessValue_type SecOCReceptionOverflowStrategy;		//[QUEUE/REJECT/REPLACE] 数据类型
	//1

	uint16 SecOCReceptionQueueSize;		//[1..65535](default value --)
	//0..1

	boolean SecOCUseAuthDataFreshness;		//(default value false)
	//1

	SecOCQueryFreshnessValue_type SecOCVerificationStatusPropagationMode;		//[BOTH/FAILURE_ONLY/NONE] 枚举类型
	//1

	const CsmJob_type * const SecOCRxAuthServiceConfigRef;		//Symbolic name reference to [ CsmJob ]
	//1

	const SecOCSameBufferPduCollection_type * const SecOCSameBufferPduRef;		//Reference to [ SecOCSameBufferPduCollection ]
	//0..1

	const SecOCRxAuthenticPduLayer_type SecOCRxAuthenticPduLayer;
	//1

	const SecOCRxPduSecuredArea_type SecOCRxPduSecuredArea;
	//0..1

	const SecOCRxSecuredPduLayer_type SecOCRxSecuredPduLayer;
	//1

}SecOCRxPduProcessing_type;


#define SPACE_SIZE 4

typedef struct{
	uint16 SecOCTxPduId;
	uint8 adatachar[8];
	uint8 sdatachar[8];
	uint8 len;
	uint8 slen;
	uint8 abc; //
}SecOCintermidate_type;

typedef struct{
	uint16 SecOCRxPduId;
	uint8 adatachar[8];
	uint8 sdatachar[8];
	uint8 len;
	uint8 slen;
	uint8 abc;
	uint8 avac;
}SecOCintermidateRx_type;

#endif 
