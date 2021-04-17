#include "SecOC.h"
//#include






uint8 TX_APDU_Buffer[SECOC_NUM_OF_TX_IPDU*64];
uint8 TX_SPDU_Buffer[TX_SINGLE_IPDU*64];
uint8 TX_ALPDU_Buffer[TX_COLLECTION_IPDU*64];
uint8 TX_CPDU_Buffer[TX_COLLECTION_IPDU*64];


SecOCPduCnt_type txcnt[SECOC_NUM_OF_TX_IPDU]=   //发送apud的计数器
{
	{
		.abc=0
	},
	{
		.abc=0
	}

};


const SecOCPdu_type txApdu[SECOC_NUM_OF_TX_IPDU]=
{
	{
		.SecOCIPduSize=64,
		.SecOCIPduSt=0,
		.SecOCIPducurpos=0,
		.SecOCIpduDataPtr = TX_APDU_Buffer,
		.SecOCIPdulen=0
	},
	{
		.SecOCIPduSize=64,
		.SecOCIPduSt=64,
		.SecOCIPducurpos=0,
		.SecOCIpduDataPtr = TX_APDU_Buffer,
		.SecOCIPdulen=0
	}
};

const SecOCPdu_type txSpdu[TX_SINGLE_IPDU]=
{
	{
		.SecOCIPduSize=64,
		.SecOCIPduSt=0,
		.SecOCIPducurpos=0,
		.SecOCIpduDataPtr = TX_SPDU_Buffer,
		.SecOCIPdulen=0
	}
};

const SecOCPdu_type txALpdu[TX_COLLECTION_IPDU]=
{
	{
		.SecOCIPduSize=64,
		.SecOCIPduSt=0,
		.SecOCIPducurpos=0,
		.SecOCIpduDataPtr = TX_ALPDU_Buffer,
		.SecOCIPdulen=0
	}
};

const SecOCPdu_type txCpdu[TX_COLLECTION_IPDU]=
{
	{
		.SecOCIPduSize=64,
		.SecOCIPduSt=0,
		.SecOCIPducurpos=0,
		.SecOCIpduDataPtr = TX_CPDU_Buffer,
		.SecOCIPdulen=0
	}
};




const EcucPartition_type EcucPartitionRefs1=
{
	.PartitionCanBeRestarted=FALSE
};


const SecOCSameBufferPduCollection_type  SecOCSameBufferPduCollection=
{
	.SecOCBufferLength=0
	//1 定义 被pdu使用的buffer
};//1

const CsmJob_type CsmJob=
{
	.CsmJobId=1
};





const SecOCGeneral_type SecOCGeneral =
{
	.SecOCDefaultAuthenticationInformationPattern = 0,  //[0..255]
	//当abc达到阈值，或返回不可恢复错误用这个值替代fv和mac每个字节，否则取消传输请求，移除A IPDU
	//0..1
	.SecOCDevErrorDetect= SECOC_ERROR_DETECT,
	//1 开关 error detect和通知notification
	.SecOCEnableForcedPassOverride=FALSE,
	/*When this configuration option is set to TRUE then the functionality inside
	the function SecOC_VerifyStatusOverride to send I-PDUs to upper layer
	independent of the verification result is enabled.*/

	.SecOCIgnoreVerificationResult=FALSE,
	//True 则验证失败也 调用PduR_SecOC[If|Tp]RxIndication
	.SecOCMainFunctionPeriodRx=0,  //[0..inf]
	.SecOCMainFunctionPeriodTx=0, //[0..inf]
	
	.SecOCMaxAlignScalarType=NULL,  //String ?
	/*The scalar type which has the maximum alignment restrictions on the
given platform. This type can be e.g. uint8, uint16 or uint32.*/

	.SecOCOverrideStatusWithDataId=FALSE,//?
	//0..1
	/*true: Function SecOC_VerifyStatusOverride accepts SecOCDataId
as parameter.
 false: Function SecOC_VerifyStatusOverride accepts
SecOCFreshnessValueId as parameter.
	 *
	 */
	.SecOCQueryFreshnessValue=CFUNC,   //[CFUNC/RTE]
	//使用哪种方式获得新鲜值
	.SecOCVerificationStatusCallout=NULL,
	//0..*
	/*Entry address of the customer specific call out routine which shall be
invoked in case of a verification attempt.
*/
	.SecOCVersionInfoApi=FALSE,
	//If true the SecOC_GetVersionInfo API is available.
	.SecOCEcucPartitionRef=&EcucPartitionRefs1
	//0..1
	//Reference to EcucPartition, where SecOC module is assigned to.
//	Tags:
//	atp.Status=draft
};

const SecOCTxPduProcessing_type SecOCTxPduProcessing[]=
{
	{
		.SecOCAuthenticationBuildAttempts=0,  //0..65535
		//0..1
		.SecOCAuthInfoTruncLength=8,    //1..65535
		// MAC长度
		.SecOCDataId=0,    //Data Identifier of the Secured I-PDU
		.SecOCFreshnessValueId=0,  //0..65535
		.SecOCFreshnessValueLength=0, //完整bits fv =trip+ reset + msg + resetflag;
		.SecOCFreshnessValueTruncLength=0,  //[SWS_SecOC_00094]   若<实际新鲜值长度  则用此裁剪长度新鲜值（重要部分 后部分）
		//S IPDU新鲜值长度
		.SecOCProvideTxTruncatedFreshnessValue=FALSE,  //?
		/*This parameter specifies if the Tx query freshness function provides the
	truncated freshness info instead of generating this by SecOC In this case,
	SecOC shall add this data to the Authentic PDU instead of truncating the
	freshness value.*/
		.SecOCUseTxConfirmation=FALSE,
		/* 决定SecOC_SPduTxConfirmation是否被调用*/
		//0..1
		.SecOCSameBufferPduRef=&SecOCSameBufferPduCollection,  //?
//		This reference is used to collect Pdus that are using the same SecOC
//		buffer.
		//0..1
		
		.SecOCTxAuthServiceConfigRef=&CsmJob,
		//1
		/*此引用用于定义调用哪个加密服务函数进行身份验证。
		 * 如果使用具有动态长度的PDU(例如，CanTP或动态长度PDU)，
		 * 则必须选择不易受到长度扩展攻击(例如，CMAC/HMAC)的MAC算法*/

		.SecOCTxAuthenticPduLayer =
		{

			.SecOCPduType = SECOC_IFPDU,
			.SecOCTxAuthenticLayerPduId = 0,  //用于SecOC_[If|Tp]Transmit.

			.SecOCTxAuthenticLayerPduRef = txApdu
	
			// 参数
			//1
		},//1

		.SecOCTxSecuredPduLayer =
		{
			.SecOCTxSecuredPdu=
			{
				.SecOCAuthPduHeaderLength =0,   //[SWS_SecOC_00261] indicate the length of the Authentic I-PDU in bytes
				//0..1  此S IPDU的结构= Secured I-PDU Header | Authentic I-PDU | Freshness Value | Authenticator | Busspecific padding).
				//配置Authentic I-PDU长度
				.SecOCTxSecuredLayerPduId = 0,  //(SecOC_[If|Tp]TxConfirmation) and for TriggerTransmit
				.SecOCTxSecuredLayerPduRef = txSpdu
				//1
			}//0..1
//			.SecOCTxSecuredPduCollection=
//			{
//				.SecOCTxAuthenticPdu=
//				{
//					.SecOCAuthPduHeaderLength=0,
//					//0..1
//					.SecOCTxAuthenticPduId =0 ,//(SecOC_IfTxConfirmation) and for TriggerTransmit.
//					.SecOCTxAuthenticPduRef= &pdu
//					//1
//				},
//				.SecOCTxCryptographicPdu =
//				{
//					.SecOCTxCryptographicPduId=0,//((SecOC_IfTxConfirmation) ) and for TriggerTransmit.
//					.SecOCTxCryptographicPduRef=&pdu
//					//1
//				},
//				.SecOCUseMessageLink =
//				{
//					.SecOCMessageLinkLen =0,
//					.SecOCMessageLinkPos = 0
//				}//0..1
//
//			}//0..1
		},
		.SecOCTxPduSecuredArea =
		{
			.SecOCSecuredTxPduLength =0,
			.SecOCSecuredTxPduOffset =0
		}//0..1   参与生成mac的部分

	},
	{
			.SecOCAuthenticationBuildAttempts=0,  //0..65535
			//0..1
			.SecOCAuthInfoTruncLength=1,    //1..65535
			// MAC长度
			.SecOCDataId=0,    //Data Identifier of the Secured I-PDU
			.SecOCFreshnessValueId=0,  //0..65535
			.SecOCFreshnessValueLength=0, //完整bits fv
			.SecOCFreshnessValueTruncLength=0,  //[SWS_SecOC_00094]   若<实际新鲜值长度  则用此裁剪长度新鲜值（重要部分 后部分）
			//S IPDU新鲜值长度
			.SecOCProvideTxTruncatedFreshnessValue=FALSE,  //?
			/*This parameter specifies if the Tx query freshness function provides the
		truncated freshness info instead of generating this by SecOC In this case,
		SecOC shall add this data to the Authentic PDU instead of truncating the
		freshness value.*/
			.SecOCUseTxConfirmation=FALSE,
			/* 决定SecOC_SPduTxConfirmation是否被调用*/
			//0..1
			.SecOCSameBufferPduRef=&SecOCSameBufferPduCollection,  //?
	//		This reference is used to collect Pdus that are using the same SecOC
	//		buffer.
			//0..1

			.SecOCTxAuthServiceConfigRef=&CsmJob,
			//1
			/*此引用用于定义调用哪个加密服务函数进行身份验证。
			 * 如果使用具有动态长度的PDU(例如，CanTP或动态长度PDU)，
			 * 则必须选择不易受到长度扩展攻击(例如，CMAC/HMAC)的MAC算法*/

			.SecOCTxAuthenticPduLayer =
			{

				.SecOCPduType = SECOC_IFPDU,
				.SecOCTxAuthenticLayerPduId = 0,  //用于SecOC_[If|Tp]Transmit.
				.SecOCTxAuthenticLayerPduRef = txApdu+1

				// 参数
				//1
			},//1

			.SecOCTxSecuredPduLayer =
			{
//				.SecOCTxSecuredPdu=
//				{
//					.SecOCAuthPduHeaderLength =0,   //[SWS_SecOC_00261] indicate the length of the Authentic I-PDU in bytes
//					//0..1  此S IPDU的结构= Secured I-PDU Header | Authentic I-PDU | Freshness Value | Authenticator | Busspecific padding).
//					//配置Authentic I-PDU长度
//					.SecOCTxSecuredLayerPduId = 0,  //(SecOC_[If|Tp]TxConfirmation) and for TriggerTransmit
//					.SecOCTxSecuredLayerPduRef = &pdu
//					//1
//				},//0..1
				.SecOCTxSecuredPduCollection=
				{
					.SecOCTxAuthenticPdu=
					{
						.SecOCAuthPduHeaderLength=0,
						//0..1
						.SecOCTxAuthenticPduId =0 ,//(SecOC_IfTxConfirmation) and for TriggerTransmit.
						.SecOCTxAuthenticPduRef= txALpdu
						//1
					},
					.SecOCTxCryptographicPdu =
					{
						.SecOCTxCryptographicPduId=0,//((SecOC_IfTxConfirmation) ) and for TriggerTransmit.
						.SecOCTxCryptographicPduRef=txCpdu
						//1
					},
					.SecOCUseMessageLink =
					{
						.SecOCMessageLinkLen =0,
						.SecOCMessageLinkPos = 0
					}//0..1

				}//0..1
			},
			.SecOCTxPduSecuredArea =
			{
				.SecOCSecuredTxPduLength =0,
				.SecOCSecuredTxPduOffset =0
			}//0..1   参与生成mac的部分

		}
};





const SecOCRxPduProcessing_type SecOCRxPduProcessing[]=
{
	{
		.SecOCAuthDataFreshnessLen = 0,	//[0..65535](default value --)  【SWS_SecOC_00219】 决定了AIPDU作为fv的长度
		//0..1 a-pdu长度
		.SecOCAuthDataFreshnessStartPosition = 0,	//[0..65535](default value --) 【SWS_SecOC_00219】 决定了AIPDU作为fv的起始点
		//0..1 a-pdu起始点
		.SecOCAuthenticationBuildAttempts = 0,	//[0..65535](default value --)
		//0..1
		.SecOCAuthenticationVerifyAttempts = 0,		//[0..65535](default value 0)
		//0..1
		//=0 则 ava执行一次
		.SecOCAuthInfoTruncLength = 1,		//[1..65535](default value --)
		// payload of s-pdu
		.SecOCDataId = 0,		//[0..65535](default value --)
		.SecOCFreshnessValueId = 0,		//[0..65535](default value --)
		.SecOCFreshnessValueLength = 0,		//[0..64](default value --)
		//新鲜值的完整长度
		.SecOCFreshnessValueTruncLength = 0,	//[0..64](default value --)
		//secoc中的新鲜值长度
		.SecOCReceptionOverflowStrategy = QUEUE,		//[QUEUE/REJECT/REPLACE]
		//	replace  secoc释放所有相关缓存
		// reject 若secoc忙,secoc 忽略SecOC_RxIndication， 对任何调用secoc_StartOfReception 返回BUFREQ_E_NOT_OK
		//  queue 针对相同SIPDU 则排队
		// 若设置QUEUE 则不可使用SecOCRxSecuredPduCollection模式
		.SecOCReceptionQueueSize = RECEPTION_QUEUE_SIZE,		//[1..65535](default value --)
		//0..1  决定排队数量
		.SecOCUseAuthDataFreshness = false,		//(default value false)  【SWS_SecOC_00219】 true 则使用部分A ipdu作为新鲜值
		//使用部分 a PDU作为新鲜值
		.SecOCVerificationStatusPropagationMode = BOTH,		//[BOTH/FAILURE_ONLY/NONE]
		// 结果发送给上层的原则 both： true/false 都发 ， failure_only：只false发   none： 都不发
		//.SecOCRxAuthServiceConfigRef = CsmJob,		//Symbolic name reference to [ CsmJob ]
		/*This reference is used to define which crypto service function is called for
		authentication.
		If PDUs with a dynamic length are used (e.g. CanTP or Dynamic Length
		PDUs) a MAC algorithm has to be chosen, that is not vulnerable to length
		extension attack (e.g. CMAC/HMAC).*/

		//.SecOCSameBufferPduRef = SecOCSameBufferPduCollection,		//Reference to [ SecOCSameBufferPduCollection ]
		//0..1
		.SecOCRxAuthenticPduLayer=
		{
			.SecOCPduType=SECOC_IFPDU,		//[SECOC_IFPDU/SECOC_TPPDU]
			//.SecOCRxAuthenticLayerPduRef=pdu
		},//1

		.SecOCRxPduSecuredArea=
		{
			.SecOCSecuredRxPduLength=0,			//[0..4294967295](default value --)
			.SecOCSecuredRxPduOffset=0			//[0..4294967295](default value 0)
		},//0..1   参与mac生成的a pdu部分

		.SecOCRxSecuredPduLayer=
		{
				.SecOCRxSecuredPdu=
			{
				.SecOCAuthPduHeaderLength=0,		//[0..4](default value 0)
				//0..1  头字节 SecOC_[If|Tp]RxIndication
				.SecOCRxSecuredLayerPduId=0,		//[0..65535](default value --)
				//决定secoc_[if/tp]RxIndication
				.SecOCSecuredRxPduVerification=false,		//(default value false)
				//若=false  直接提取authentic ipdu 而不验证


				//.SecOCRxSecuredLayerPduRef=pdu,
			},//0..1
			.SecOCRxSecuredPduCollection=
			{   //若SecOCReceptionOverflowStrategy 则不能用这个模式
				.SecOCSecuredRxPduVerification=false,		//(default value false)
				.SecOCRxAuthenticPdu=
				{
					.SecOCAuthPduHeaderLength=0,		//[0..4](default value 0)
					//0..1
					.SecOCRxAuthenticPduId=0,		//[0..65535](default value --)
					// SecOC_IfRxIndication.
					//.SecOCRxAuthenticPduRef=pdu
				},
				.SecOCRxCryptographicPdu=
				{
					.SecOCRxCryptographicPduId=0,		//[0..65535](default value --)
					//SecOC_IfRxIndication.
					//.SecOCRxCryptographicPduRef=pdu
				},
				.SecOCUseMessageLink=
				{
					.SecOCMessageLinkLen =0,
					.SecOCMessageLinkPos = 0
				}//0..1
			}//0..1
		}//1 接收到的报文数据
	}
};


const SecOC_ConfigType SecOCConfiguration=
{
	.general=SecOCGeneral,
	.secOCTxPduProcessings=SecOCTxPduProcessing,
	.secOCRxPduProcessings=SecOCRxPduProcessing
};


//临时存储secoc中间结果
//SecOCintermidate_type SecOCintermidate[SECOC_NUM_OF_TX_IPDU]={
//	{
//		.SecOCTxPduId=0,
//		.abc=0
//	}
//};
//
//SecOCintermidateRx_type SecOCintermidateRx[SECOC_NUM_OF_RX_IPDU]={
//	{
//		.SecOCTxPduId=0,
//		.abc=0,
//		.avac=0
//	}
//};




