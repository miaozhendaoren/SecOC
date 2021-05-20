#include "slaveFVM_Cfg.h"

uint8 preTrip[3 * NUM_MSG];

uint8 resetData[3 * NUM_RESET];
uint8 preresetData[3 * NUM_MSG];
uint8 msgData[6 * NUM_MSG];
uint8 preMsgData[6 * NUM_MSG];


ConfirmECU_Type confirmECU[] = {  //可配置的
        {
                .state=0,
                .ackv = 0x2be
        },
        {
                .state=0,
                .ackv = 0x2bf
        }
};

ResetCntS_Type resetCnt[] = {
        {
                .resetdata = resetData,
                .preresetdata = preresetData,
                .ResetCntLength = 15,

        },
        {
                .resetdata = resetData,
                .preresetdata = preresetData,
                .ResetCntLength = 17,
                .resetcanid = 0x66
        }
};

uint16 resetcanid[] = {0x65, 0x66};

ResetState_Type resetState[] = {
        {
                .resetflag=0,
                .resetloss=0,
                .resetTag=0,
                .resetTime=1000
        },
        {
                .resetflag=0,
                .resetloss=0,
                .resetTag=0,
                .resetTime=2500
        }
};

/*发送和接收的preTX  preRx共用 preMsgData preresetData preTrip 通过索引进行区分*/
MsgCntS_Type msgCnt[] = {
        {
                .msgdata=msgData,
                .premsgdata=preMsgData,
                .MsgCntLength=24
        },
        {
                .msgdata=msgData,
                .premsgdata=preMsgData,
                .MsgCntLength=26
        }
};

uint8 ResetFlagLength = 2;  //最大2bit