#ifndef _CSM_H
#define _CSM_H

#include "Csm_Types.h"

Std_ReturnType Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);


Std_ReturnType Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,Crypto_VerifyResultType* verifyPtr);


#endif
