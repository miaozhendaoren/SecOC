#ifndef _MASTERFVM_TYPES_H_
#define _MASTERFVM_TYPES_H_

#include "ComStack_Types.h"

typedef struct {
    uint8 state;
	uint16 ackv;
}ConfirmECU_Type;

typedef struct {
    uint8* resetdata;
    uint8* preresetdata;
	uint8 ResetCntLength;
    uint16 resetcanid;
}ResetCntS_Type;


typedef struct {
    uint8 resetflag;
    uint8 resetloss;
    uint32 resetTag;
    uint32 resetTime;
}ResetState_Type;


typedef struct {
    uint8* msgdata;
    uint8* premsgdata;
	uint8 MsgCntLength;
}MsgCntS_Type;

#endif