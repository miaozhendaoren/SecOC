//可配置接口

//SecOCVerificationStatusCallout  调用一个callout函数来通知其他模块最新接收到的Secured I-PDU的验证状态
void SecOC_VerificationStatusCallout ( 
SecOC_VerificationStatusType verificationStatus 
);
// 0x50  同步 不可重入相同的FreshnessValueID。可重入不同的FreshnessValueID。
//该服务将每次的验证尝试状态从SecOC模块传播到其他模块。该服务配置如下：
//Only：“False”验证状态传播到模块中
//Both：“真”和“假”验证状态被传播到模块中
//None：没有验证状态传播
//注：参数freshnessValueID允许明确识别验证尝试的Secured I-PDU。由于每个Secured I-PDU至少有一个，但可能有两个相关的新鲜度值ID（即，一个Secured I-PDU可能有一个二次新鲜度值ID），因此SecOCVerificationStatusCallout能够表示已针对哪个新鲜度值执行了验证尝试。
//注：配置为通过SecOC_VerificationStatusCallout通知的任何模块都必须符合上述签名的目标功能。上面列出的目标函数的名称不是固定的。名称可以通过参数SecOCVerificationStatusCallout来配置
