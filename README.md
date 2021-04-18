# SecOC


## Slave模块
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

## SecOC 模块

`Std_ReturnType`: `VAR(Std_ReturnType, STD_TYPES_VAR)`

`PduIdType`: `VAR(PduIdType, COMSTACK_TYPES_VAR)`

`PduLengthType`: `VAR(PduLengthType, COMSTACK_TYPES_VAR)`

`SecOC_OverrideStatusType`: `VAR(SecOC_OverrideStatusType, RTE_SECOC_TYPES_VAR)`

`BufReq_ReturnType`: `VAR(BufReq_ReturnType, COMSTACK_TYPES_VAR)`

`SecOCintermidate_type`: `VAR(SecOCintermidate_type, SECOC_TYPES_VAR)`

`SecOC_VerificationStatusType`: `VAR(SecOC_VerificationStatusType, RTE_SECOC_TYPES_VAR)`

`SecOCRxPduProcessing_type`: `VAR(SecOCRxPduProcessing_type, SECOC_TYPES_VAR)`

</br>

`const PduInfoType*`: `P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`const SecOC_ConfigType*`: `P2CONST(SecOC_ConfigType, AUTOMATIC, SECOC_APPL_DATA)`

`const RetryInfoType*`: `P2CONST(RetryInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`const SecOC_ConfigType *`: `P2CONST(SecOC_ConfigType, AUTOMATIC, SECOC_APPL_DATA)`

</br>

`Std_VersionInfoType*`: `P2VAR(Std_VersionInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`PduInfoType*`: `P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)`

`PduLengthType*`: `P2VAR(PduLengthType, AUTOMATIC, SECOC_APPL_DATA)`

