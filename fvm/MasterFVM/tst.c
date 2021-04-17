#include<stdio.h>
#include "MacGenerate.h"


void change(int* a){
    *a=4;
}


int main(){
	uint8 a;
	uint8 dataPtr[]={0x12,0x34,0x56,0x78,0x90};
	uint8 macPtr[16];
	uint8 macLengthPtr=16;
	
	
	
	
	Mac_Generate(dataPtr, 5, macPtr);
	for(int i=0; i<16; i++){
		printf("%x ",macPtr[i]);
	}
	return 0;
} 
