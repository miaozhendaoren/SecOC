#ifndef _COMSTACK_TYPES_H_
#define _COMSTACK_TYPES_H_

#include "Std_Types.h"



typedef uint8 PduIdType;      
//#define PduIdType uint16   //根据maximum num of pdus确定

typedef uint8 PduLengthType;
//#define PduLengthType uint16
//#define PduLengthType uint32    //根据maximum length of PDUs确定

//数据信息
typedef struct {
  uint8* SduDataPtr; // 数据指针
  uint8* MetaDataPtr; // meta数据指针
  PduLengthType SduLength; // pdu长度
} PduInfoType;

typedef uint8 PNCHandleType;   //Used to store the identifier of a partial network cluster.


enum TPParameterType {
	TP_STMIN=0x00,// Separation Time
	TP_BS, //Block Size
	TP_BC, //The Band width control parameter used in FlexRay transport protocol module.
};

typedef enum {
	BUFREQ_OK=0x00, // Buffer request accomplished successful. This status shall have the value 0.
	BUFREQ_E_NOT_OK, //Buffer request not successful. Buffer cannot be accessed. This status shall have the value 1.
	BUFREQ_E_BUSY, //Temporarily no buffer available. It's up the requester to retry request for a certain time. This status shall have the value 2.
	BUFREQ_E_OVFL, //No Buffer of the required length can be provided. This status shall have the value 3.
} BufReq_ReturnType;

typedef enum {
	TP_DATACONF= 0x00, 
	/*TP_DATACONF indicates that all data, that have been
	copied so far, are confirmed and can be removed from the
	TP buffer. Data copied by this API call are excluded and
	will be confirmed later.*/
	TP_DATARETRY,
	/*TP_DATARETRY indicates that this API call shall
	copyalready copied data in order to recover from an error.
	In this case TxTpDataCnt specifies the offset of the first
	byte to be copied by the API call.*/
	TP_CONFPENDING, //TP_CONFPENDING indicates that the previously copied data must remain in the TP.
} TpDataStateType;

typedef struct {
	TpDataStateType TpDataState; //The enum type to be used to store the state of Tp buffer.
	//
	PduLengthType TxTpDataCnt;  //Offset from the current position which identifies the number of bytes to be retransmitted.
} RetryInfoType;



//typedef struct{
//	boolean DynamicLength;   //0..1   usage of this attribute is restricted by [constr_3448].
//	boolean J1939Requestable; //0..1
//	uint32 PduLength;  //1
//	EcucPartition_type EcucPduDefaultPartitionRef; //0..1
//}Pdu_Type;


#define NetworkHandleType uint8





#endif /* COMSTACK_TYPES_H_ */


