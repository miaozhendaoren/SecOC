//Ԥ������
void SecOC_MainFunctionRx ( void );
//0x06
//�˺���ִ��SecOCģ���Rx·���������֤����֤������̡�
//SecOC_MainFunctionRx������ʱ�����ݲ���SecOCMainFunctionPeriodRx��������
//���������SecOC_MainFunctionRx��SecOC����Ҫ����Ƿ����µ�Secured I-PDU��Ҫ��֤��
//����У�SecOCģ�齫��ͬһ�������������ж�ÿ����ʶ��Ϊ�µ�IPDU������֤
//����ÿ������֤�ɹ���Secured I-PDU, SecOCģ��Ӧ����ͨ������PduR_SecOC[If|Tp]RxIndication����Authentic I-PDU��
//�����䴫�ݸ��ϲ�ͨ��ģ��

void SecOC_MainFunctionTx ( void );
//0x03
//�˺���ִ��SecOCģ���Tx·���������֤����֤������̡�
//SecOC_MainFunctionTx������ʱ�����ݲ���SecOCMainFunctionPeriodTx��������
//����Ѿ�������SecOC_MainFunctionTx��SecOC����Ҫ����Ƿ����µ�Authentic I-PDU��Ҫ��֤��
//����У�SecOCģ���򽫽���ÿ��IPDU�������֤��
//�����ЩIPDU����ͬ��������������ʶ��Ϊ�µ�
//����ÿ������֤��Authentic I-PDU, SecOCģ��Ӧ����ͨ������PduR�����ײ�ģ���Secured I-PDU����


