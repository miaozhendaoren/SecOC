

#ifndef MACGENERATE_H_
#define MACGENERATE_H_
//#include "Platform_Types.h"

typedef unsigned char         	uint8;          /*           0 .. 255             */
typedef unsigned short        	uint16;         /*           0 .. 65535           */
typedef unsigned long         	uint32;

void Preprocess(uint8* datavalue, uint8 datalen);

void gethash(uint8 *hash);

void Mac_Generate(uint8* datavalue, uint8 datalen, uint8* hash);

#endif




