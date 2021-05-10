



#ifndef _STD_TYPES_H
#define _STD_TYPES_H

//typedef unsigned char uint8;
//typedef unsigned short uint16;
//typedef unsigned int uint32;
//typedef unsigned long uint64;
#include "Platform_Types.h"






#define	NULL 0

//uint8[n]



typedef enum
{
    E_OK,
    E_NOT_OK,
}Std_ReturnType;





#define Std_ReturnType uint8
//E_OK 0 
//E_NOT_OK 1 
//0x02-0x3F 2 
#define Std_TransformerClass uint8
//STD_TRANSFORMER_UNSPECIFIED 0x00
//STD_TRANSFORMER_SERIALIZER 0x01
//STD_TRANSFORMER_SAFETY 0x02
//STD_TRANSFORMER_SECURITY 0x03
//STD_TRANSFORMER_CUSTOM 0xFF

#define Std_TransformerForwardCode uint8
//E_OK 0x00 --
//E_SAFETY_INVALID_REP 0x01 --
//E_SAFETY_INVALID_SEQ 0x02 --
//E_SAFETY_INVALID_CRC 0x03

#define Std_MessageTypeType uint8
//STD_MESSAGETYPE_REQUEST 0x00 Message type for a requestmessage
//STD_MESSAGETYPE_RESPONSE 0x01 Message type for a responsemessage
//0x02-0x3F 0x02 reserverd for future message type

#define Std_MessageResultType uint8
/*
STD_MESSAGERESULT_OK 0x00 STD_MESSAGERESULT_OK
STD_MESSAGERESULT_ERROR 0x01 Messageresult for an ERROR response
0x02-0x3F 0x02 reserverd for future message results

This type is used to encode different types of results for response messages. -
Currently this encoding is limited to the distinction between OK and ERROR
responses.*/

//function pointer
Std_ReturnType (*Std_ExtractProtocolHeaderFieldsType) (
 const uint8* buffer,
 uint32 bufferLength,
 Std_MessageTypeType* messageType,
 Std_MessageResultType* messageResult
);
/*
 Type for the function pointer to extract the relevant protocol header fields of the
message and the type of the message result of a transformer. - At the time being,
this is limited to the types used for C/S communication (i.e., REQUEST and
RESPONSE and OK and ERROR).
*/

#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
#define E_OK 0x00u
typedef unsigned char StatusType; /* OSEK compliance */
#endif
#define E_NOT_OK 0x01u
#define STD_HIGH 0x01u /* Physical state 5V or 3.3V */
#define STD_LOW 0x00u /* Physical state 0V */
#define STD_ACTIVE 0x01u /* Logical state active */
#define STD_IDLE 0x00u /* Logical state idle */
#define STD_ON 0x01u
#define STD_OFF 0x00u



struct Std_VersionInfoType{
	uint16 vendorID;
	uint16 moduleID;
	uint8 sw_major_version;
	uint8 sw_minor_version;
	uint8 sw_patch_version;
};
//This type shall be used to request the version of a BSW module using the <Module
//name>_GetVersionInfo() function.


struct Std_TransformerError{
	uint8 errorCode;
	Std_TransformerClass transformerClass;
};

struct Std_TransformerForward{ //(draft)
	Std_TransformerForwardCode errorCode;
	Std_TransformerClass transformerClass;
};
//Tags:atp.Status=draft







#endif /* STD_TYPES_H */


