#include "MasterFVM_Cfg.h"


uint8 resetData[3*NUM_RESET];


ConfirmECU_Type confirmECU[]={  //可配置的
    {
        .state=0,
        .ackv = 0x2be
    },
    {
        .state=0,
        .ackv = 0x2bf
    }
};

ResetCnt_Type resetCnt[]={
    {
        .resetdata = resetData,
	    .ResetCntLength = 15,
        .resetcanid = 0x65,
        .resettag = 0,
	    .resetSynTime = 1000
    },
    {
        .resetdata = resetData,
	    .ResetCntLength = 17,
        .resetcanid = 0x66,
        .resettag = 0,
	    .resetSynTime = 2500
    }
};


