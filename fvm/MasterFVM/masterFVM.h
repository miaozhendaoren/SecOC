//#include "i2c.h"
// #include "type.h"
#include "MasterFVM_Cfg.h"

void MasterFVM_Init(void);

void MasterFVM_getTripValue(const PduInfoType* PduInfoPtr ); //   获取trip及hash结果

void MasterFVM_getResetValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr); //   获取reset及hash结果

void MasterFVM_changestate(PduIdType TxPduId);

void MasterFVM_MainTx(void);




