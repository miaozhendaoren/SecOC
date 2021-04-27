[toc]

# SecOC

## 代码重构

### Slave模块

`Std_ReturnType` :
    `FUNC(VAR(Std_ReturnType,STD_TYPES_VAR),SLAVE_CODE)`

`PduIdType RxPduId` :
    `VAR(PduInfoType,COMSTACK_TYPES_VAR) RxPduId`

`const PduInfoType* PduInfoPtr` :
    `P2CONST(PduInfoType,SLAVE_CODE,SLAVE_APPL_CONST) PduInfoPtr`

`uint16 SecOCFreshnessValueID,`:
    `VAR(uint16,FRESH_VAR) SecOCFreshnessValueID,`

`uint8* SecOCFreshnessValue,` :
    `P2VAR(uint8,SLAVE_CODE,SLAVE_APPL_DATA) SecOCFreshnessValue,`

`uint32* SecOCFreshnessValueLength,`:
    `P2VAR(uint32,SLAVE_CODE,SLAVE_APPL_DATA) SecOCFreshnessValueLength`

`uint8* SecOCTruncatedFreshnessValue,` :
    `P2VAR(uint8,SLAVE_CODE,SLAVE_APPL_DATA) SecOCTruncatedFreshnessValue,`

`uint32* SecOCTruncatedFreshnessValueLength` :
    `P2VAR(uint32,SLAVE_CODE,SLAVE_APPL_DATA) SecOCTruncatedFreshnessValueLength`   
     
`const uint8* SecOCTruncatedFreshnessValue,` :
    ``

### SecOC 模块

`Std_ReturnType`: `VAR(Std_ReturnType, STD_TYPES_VAR)`

`PduIdType`: `VAR(PduIdType, COMSTACK_TYPES_VAR)`

`PduLengthType`: `VAR(PduLengthType, COMSTACK_TYPES_VAR)`

`SecOC_OverrideStatusType`: `VAR(SecOC_OverrideStatusType, RTE_SECOC_TYPES_VAR)`

`BufReq_ReturnType`: `VAR(BufReq_ReturnType, COMSTACK_TYPES_VAR)`

`SecOCintermidate_type`: `VAR(SecOCintermidate_type, SECOC_TYPES_VAR)`

`SecOC_VerificationStatusType`: `VAR(SecOC_VerificationStatusType, RTE_SECOC_TYPES_VAR)`

`SecOCRxPduProcessing_type`: `VAR(SecOCRxPduProcessing_type, SECOC_TYPES_VAR)`

</br>

`const PduInfoType*`: `P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_CONST)`

`const SecOC_ConfigType*`: `P2CONST(SecOC_ConfigType, AUTOMATIC, SECOC_APPL_CONST)`

`const RetryInfoType*`: `P2CONST(RetryInfoType, AUTOMATIC, SECOC_APPL_CONST)`

`const SecOC_ConfigType *`: `P2CONST(SecOC_ConfigType, AUTOMATIC, SECOC_APPL_CONST)`

</br>

`Std_VersionInfoType*`: `P2VAR(Std_VersionInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`PduInfoType*`: `P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`PduLengthType*`: `P2VAR(PduLengthType, AUTOMATIC, SECOC_APPL_DATA)`



## 接口实现

### 说明

- 进行某接口的编码前，应将其功能记录在此文档中
- 使用`CUint`完成单元测试

### DDL

- 5.10日前完成FVM部分的接口实现
- 5.11日前完成FVM部分的接口测试

### MasterFVM

#### step1：接口的实现


##### MasterFVM_Init

- 完成人：赵宸阳

- 功能：
    - 此函数是在Master启动时调用，将trip计数器加一

##### MasterFVM_getTripValue

- 完成人：赵宸阳

- 功能：
    - 此函数接受一个`PduInfoType`指针，向其中填入trip消息的内容

##### MasterFVM_getResetValue

- 完成人：赵宸阳

- 功能：
    - 此函数接受一个`PduIdType`类型的id值和一个`PduInfoType`指针
    - id值为reset消息的canId，`PduInfoType`指针中需要填入reset消息的内容

##### MasterFVM_changestate

- 完成人：赵宸阳

- 功能：
    - 此函数在trip同步消息的Ack报文到达master调用
    - 此函数发送一个ack确认报文回slave并修改master的状态

##### MasterFVM_MainTx

- 完成人：赵宸阳

- 功能：
    - 此函数为周期被调用的函数
    - 在trip同步完成之前，周期发送trip同步消息
    - 在trip同步完成之后，周期发送reset同步消息

#### step2：单元测试

### SlaveFVM

#### step1：接口的实现

##### FVM_changestate

- 完成人：李兴诚

##### FVM_Syn_check

- 完成人：李兴诚

##### FVM_updateTrip

- 完成人：张文涛

##### FVM_updateReset

- 完成人：张文涛

##### FVM_GetTxFreshness

- 完成人：李兴诚

##### FvM_GetTxFreshnessTruncData

- 完成人：李兴诚

##### FVM_GetRxFreshness

- 完成人：张文涛

##### FVM_GetRxFreshnessAuthData

- 完成人：张文涛

##### FVM_updatePreValue

- 完成人：李兴诚

#### step2：单元测试

### Secoc

