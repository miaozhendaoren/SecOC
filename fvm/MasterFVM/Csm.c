#include<stdio.h>
#include "Csm.h"
#include "MacGenerate.h"

uint8 tmpmac[16];


Std_ReturnType Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr){

    Mac_Generate(dataPtr, dataLength, macPtr);
    *macLengthPtr = 16;
    return E_OK;
}


Std_ReturnType Csm_MacVerify(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr,uint32 dataLength,uint8* macPtr,uint32 macLength,Crypto_VerifyResultType* verifyPtr){
   uint8 shang = macLength/8;
   uint8 yu = macLength%8;
   uint8 i;
   MacGenerate(dataPtr, dataLength, tmpmac);
   for(i=0; i<shang; i++){
       if(tmpmac[i]!=macPtr[i]){
           *verifyPtr=1;
           return E_OK;
       }
   }
   if(yu>0){
       if((tmpmac[i]>>(8-yu))!=(macPtr[i]>>(8-yu))){
           *verifyPtr=1;
           return E_OK;
       }
   }
   *verifyPtr=0;
   return E_OK;
   
}

// int main(){
// 	uint8 a;
// 	uint8 dataPtr[]={0x12,0x34,0x56,0x78,0x90};
// 	uint8 macPtr[16];
// 	uint8 macLengthPtr=16;
// 	a=Csm_MacGenerate(0, 0, dataPtr, 5, macPtr, &macLengthPtr);
// 	for(int i=0; i<16; i++){
// 		printf("%x ",macPtr[i]);
// 	}
// } 

