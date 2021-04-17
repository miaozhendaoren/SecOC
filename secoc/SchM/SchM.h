//预定功能
void SecOC_MainFunctionRx ( void );
//0x06
//此函数执行SecOC模块对Rx路径的身份验证和验证处理过程。
//SecOC_MainFunctionRx的周期时间依据参数SecOCMainFunctionPeriodRx进行配置
//如果安排了SecOC_MainFunctionRx，SecOC首先要检查是否有新的Secured I-PDU需要验证。
//如果有，SecOC模块将在同一个主函数调用中对每个被识别为新的IPDU进行验证
//对于每个新验证成功的Secured I-PDU, SecOC模块应立即通过调用PduR_SecOC[If|Tp]RxIndication生成Authentic I-PDU，
//并将其传递给上层通信模块

void SecOC_MainFunctionTx ( void );
//0x03
//此函数执行SecOC模块对Tx路径的身份验证和验证处理过程。
//SecOC_MainFunctionTx的周期时间依据参数SecOCMainFunctionPeriodTx进行配置
//如果已经安排了SecOC_MainFunctionTx，SecOC首先要检查是否有新的Authentic I-PDU需要认证。
//如果有，SecOC模块则将进行每个IPDU的身份验证，
//随后将这些IPDU在相同的主函数调用中识别为新的
//对于每个新认证的Authentic I-PDU, SecOC模块应立即通过调用PduR触发底层模块的Secured I-PDU传输


