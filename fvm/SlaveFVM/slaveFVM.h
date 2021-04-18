#ifndef _SLAVEFVM_H
#define _SLAVEFVM_H
#include "slaveFVM_Cfg.h"
#include "../../Compiler.h"
#include "../../Compiler_Cfg.h"

// void FVM_changestate(PduIdType RxPduId);
FUNC(void,SLAVE_CODE)
FVM_changestate(VAR(PduInfoType,COMSTACK_TYPES_VAR));

// void FVM_Syn_check(void);
FUNC(void,SLAVE_CODE)
FVM_Syn_check(void);

// Std_ReturnType FVM_updateTrip(const PduInfoType* PduInfoPtr );  //更新trip值
FUNC(VAR(Std_ReturnType,STD_TYPES_VAR))
FVM_updateTrip(P2CONST(PduInfoType,SLAVE_CODE,SLAVE_APPL_DATA) PduInfoPtr);

// Std_ReturnType FVM_updateReset(PduIdType TxPduId, const PduInfoType* PduInfoPtr);  //更新reset值


Std_ReturnType FVM_GetTxFreshness (
	uint16 SecOCFreshnessValueID,
	uint8* SecOCFreshnessValue,
	uint32* SecOCFreshnessValueLength
);


Std_ReturnType FvM_GetTxFreshnessTruncData (
	uint16 SecOCFreshnessValueID,
	uint8* SecOCFreshnessValue,
	uint32* SecOCFreshnessValueLength,
	uint8* SecOCTruncatedFreshnessValue,
	uint32* SecOCTruncatedFreshnessValueLength
);


Std_ReturnType FVM_GetRxFreshness (uint16 SecOCFreshnessValueID,
    const uint8* SecOCTruncatedFreshnessValue,
    uint32 SecOCTruncatedFreshnessValueLength,
    uint16 SecOCAuthVerifyAttempts,
    uint8* SecOCFreshnessValue,
    uint32* SecOCFreshnessValueLength
);

Std_ReturnType FVM_GetRxFreshnessAuthData (
	uint16 SecOCFreshnessValueID,
	const uint8* SecOCTruncatedFreshnessValue,
	uint32 SecOCTruncatedFreshnessValueLength,
	const uint8* SecOCAuthDataFreshnessValue,
	uint16 SecOCAuthDataFreshnessValueLength,
	uint16 SecOCAuthVerifyAttempts,
	uint8* SecOCFreshnessValue,
	uint32* SecOCFreshnessValueLength
);


void FVM_updatePreValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr);


// void updatePreRxValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr);

#endif

