#include "MacGenerate.h"

#include "sm4.h"
#include <string.h>
#include <stdio.h>


uint8  key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};		//密钥 

		
uint8  predata[16];
uint8  output[16];
uint32   sk[32];



void Preprocess(uint8* datavalue, uint8 datalen){		//Mac generation内部调用mac值生成第1步:将输入处理成128位（补零）
	
	uint16 i;
	for (i = 0; i < 16; i++) {		//将原始数据扩充至128位 
		if (i < datalen)
			predata[i] = datavalue[i];
		else
			predata[i] = 0x00;
	}
	// return predata;		//返回扩充后的mac 
}

void gethash(uint8 *hash){		//加密函数 

	sm4_setkey_enc(sk,key);		//设置加密密钥 
	sm4_crypt_ecb(sk,16,predata,hash);		//第二个参数1代表加密，predata代表需要加密的数据，output为加密后数据 

	// return output;
}

void Mac_Generate(uint8* datavalue, uint8 datalen, uint8* hash){		//SecOC调用mac generation模块生成128bitMac值

	Preprocess(datavalue,  datalen);		//扩充模块 
	gethash(hash);		//加密模块 
	// return encryption;
} 


//int main(){
//	uint8 a;
//	uint8 dataPtr[]={0x12,0x34,0x56,0x78,0x90};
//	uint8 macPtr[16];
//	uint8 macLengthPtr=16;
//	MacGenerate(dataPtr, 5, macPtr);
//	for(int i=0; i<16; i++){
//		printf("%x ",macPtr[i]);
//	}
//	return 0;
//} 











