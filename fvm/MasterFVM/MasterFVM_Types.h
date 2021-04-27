#ifndef _MASTERFVM_TYPES_H_
#define _MASTERFVM_TYPES_H_

#include "ComStack_Types.h"

typedef struct {
    uint8 state;
	uint16 ackv;
}ConfirmECU_Type;

typedef struct {
    uint8* resetdata;
	uint8 ResetCntLength;
    uint16 resetcanid;
    uint32 resetSyntag;
	uint32 resetSynTime;
}ResetCnt_Type;


