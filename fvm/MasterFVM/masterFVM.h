//#include "i2c.h"
// #include "type.h"
#include "MasterFVM_Cfg.h"
#include "../../Compiler.h"


uint8 length(uint8 num);
uint8 set_k(uint8 num, uint8 k);
uint8 is_k(uint8 num, uint8 k);
void get_value(uint16 can_id, const PduInfoType *PduInfoPtr, uint8 tripCntLength, uint8 ResetCntLength);

// void MasterFVM_Init(void);
FUNC(void, MASTER_CODE)
MasterFVM_Init(void);

// void MasterFVM_getTripValue(const PduInfoType* PduInfoPtr ); //   获取trip及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);

// void MasterFVM_getResetValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr); //   获取reset及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);

// void MasterFVM_changestate(PduIdType TxPduId);
FUNC(void, MASTER_CODE)
MasterFVM_changestate(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId);

// void MasterFVM_MainTx(void);
FUNC(void, MASTER_CODE)
MasterFVM_MainTx(void);

