#include "i2c.h"
#include "masterFVM.h"
#include "MacGenerate.h"
#include "usart.h"
#include "fvm_conf.h"

#define ADDR_24LCxx_Write 0xA0
#define ADDR_24LCxx_Read 0xA1

uint8 trip[3];			  //初始化时从非易失性存储器中获得并+1后再存回非易失性存储器， 低位先占满8字节 高位再占
						  //例如  TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][]
uint8 TripCntLength = 16; //可配置

FUNC(void, MASTER_CODE)
MasterFVM_Init(void)
{
	/*
	1.获取非易失性存储器中的值，存入全局变量trip[3]对应索引内，trip数组使用情况依据TripCntLength决定。优先使用低索引；
	2.对trip[3]+1, 达到char最大值则，发生进位;  
		若发生进位导致trip到达TripCntLength规定最大情况，则 trip[3] =1,即高位为0，最低位为1；
		if(trip[-1]==255){   trip[-2]+=1;  依次观察是否进位
	3.将新的trip值更新到非易失性存储器中；

	*/

	//需先判断TripCntLength长度，确定多少数组元素需要管理
	uint8 i;
	HAL_I2C_Mem_Read(&hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, trip, 2, 0xff); //读取2字节的trip

	if (trip[1] == 255)
	{ //低位满，需进位
		if (trip[0] == 255)
		{ //trip值达到最大值
			trip[0] = 0;
			trip[1] = 1;
		}
		else
		{ //trip未达最大值， 高位进位，低位到0
			trip[0] += 1;
			trip[1] = 0;
		}
	}
	else
	{
		trip[1] += 1;
	}

	for (i = 0; i < 2; i++)
	{
		HAL_I2C_Mem_Write(&hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &trip[i], 1, 0xff); //使用I2C块读，出错。因此采用此种方式，逐个单字节写入
		HAL_Delay(5);																			  //此处延时必加，与AT24C02写时序有关
	}
}

uint16 tripcanid = 0x2bd; //可配置

FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
	/*
	默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
		trip(TripCntLength)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
		例如 TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
		由于后接1bit reset且值为1 则dataptr = [0x02] [0xbd] [1001 1111][1111 0000] 最后4位为补0
	1.构造生成mac的原始数据 char *dataptr 组成为  tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLength)+reset(1bit)：值为1 + padding0;
		在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
		例如  TripCntLength=12  [ 0x02 0xbd 0x00 0x14]  此时
	2.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
		获得mac值
	3. 数据排版 PduInfoPtr.SduDataPtr =trip(TripCntLength)+1(1bit)+ mac(64bit-TripCntLength-1);  
		重排位，在数据生成时不再填充0
	*/
}

FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
	/*
	1.先判断TxPduId 若>=NUM_RESET， 则直接退出;
	
	默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
		trip(TripCntLength)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
		例如 TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
		由于后接ResetCntLength reset   如  ResetCntLength=11  且reset=0x04 0xff 左移5位并连接到前面trip后
		 则dataptr = [0x00] [0x65] [1001 1111][1111 0011] [1111 1100 ]最后2位为补0
	2.构造生成mac的原始数据 char *dataptr 组成为  
		resetcanid[2](reset拆分成两个char, 由TxPduId作为索引找到)+trip(TripCntLength)+reset(ResetCntLength) + padding0;
		在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
		reset(ResetCntLength)值在获取后需要将resetData[3*TxPduId]对应值加1 若发生进位需要改变
		若reset达到ResetCntLength规定的最大值，则reset不加值。

	3.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
		获得mac值
	4. 数据排版 PduInfoPtr.SduDataPtr =reset(ResetCntLength)+ mac(64bit-ResetCntLength);  
		重排位，在数据生成时不再填充0
	
	*/
	if(TxPduId < NUM_RESET){
		
	}
}

FUNC(void, MASTER_CODE)
MasterFVM_changestate(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId)
{
	/* 收到id后检查confirmECU 对应索引值， 修改state状态为1， 发送ackv的报文*/
	ConfirmECU_Type tmp *;
	tmp = &confirmECU[TxPduId];
	tmp.state = 1;
	PduInfoPtr info;
	CanIf_Transmit(tmp.ackv, info);
}

uint8 firsttrip = 0;	  //标记是否发送过trip
uint8 triperrorlabel = 1; //trip发送失败    0表示正常 1表示多次失败且达到上限
uint8 syn_for_trip = 0;	  //0表示trip同步  1表示reset同步
uint32 windowTimetag;	  // trip时间计数
uint32 windowTime4ack;	  // trip周期时间阈值 可配置

uint8 resendTriptag; //需要重传trip

uint8 resentTripTime; //trip重传次数
uint8 triptxlimit;	  //重传次数上限  可配置

uint16 errorcanid = 0x64;	//可配置
uint16 notifycanid = 0x386; //可配置

uint8 multi = 1; //时间倍率  1表示一次调用MainTx 为1ms   可配置

FUNC(void, MASTER_CODE)
MasterFVM_MainTx(void)
{
	/*
		本函数将被时间调度函数周期性调用，执行内容如下：
		1.若未曾发送过trip报文，则发送一次
		2.trip同步失败是否达到上限
		  若达到上限则结束
		  未达上限则检查当前同步（syn_for_trip）为trip同步（进入3）还是reset同步（进入4）
		3.trip同步：
			windowTimetag每毫秒计1，当达到阈值windowTime4ack，检查confirmECU中的state状况，若存在非1的state，设resendTriptag=1
			判断resendTriptag是否==1，
			若==1：若达到trip重发上限，发报错报文errorcanid
				若未达重发上限，重发trip报文：调用MasterFVM_getTripValue（）
			若==0:不需要重发，trip过程完成，发通知报文notifycanid，修改syn_for_trip=1 为reset同步过程。
		4.reset同步：
			每轮各reset的tag+1， 若达到阈值则调用getResetValue() 发送reset同步消息
	*/

	unsigned char i;
	const PduInfoType *info;
	if (firsttrip == 0)
	{
		MasterFVM_getTripValue(info);
		CanIf_Transmit(tripcanid, info);
		firsttrip = 1;
	}

	if (triperrorlabel == 0)
	{ //未发生多次trip失败达到上限
		if (syn_for_trip == 0)
		{ //当前是trip同步阶段
			windowTimetag += multi;
			if (windowTimetag == windowTime4ack)
			{ //10s
				for (i = 0; i < NUM_ECU; i++)
				{
					if (confirmECU[i].state != 1)
					{ //有没收到的ack的

						resendTriptag = 1;
						break;
					}
				}
				if (resendTriptag == 1)
				{ //需要重发trip
					resentTripTime++;
					if (resentTripTime == triptxlimit)
					{ //达到重发上限
						info->SduDataPtr = NULL;
						CanIf_Transmit(errorcanid, info);
						triperrorlabel = 1; //多次发送trip都失败 =1
					}
					else
					{
						MasterFVM_getTripValue(info);
						CanIf_Transmit(tripcanid, info);
						resendTriptag = 0;
						windowTimetag = 0;
					}
				}
				else
				{ // resendtag==0  不需要重发，发送通知报文，开始按周期发送同步消息
					info->SduDataPtr = NULL;
					info->SduLength = 8;
					CanIf_Transmit(notifycanid, info);
					syn_for_trip = 1;
					windowTimetag = 0;
				}
			}

			// trip同步阶段 - end  syn_for_trip=0
		}
		else
		{ //syn_for_trip=1  reset同步阶段
			for (i = 0; i < NUM_RESET; i++)
			{ //占2字节
				resetCnt[i].resetSyntag += multi;
			}
			for (i = 0; i < NUM_RESET; i++)
			{ //占2字节
				if (resetCnt[i].resetSyntag >= resetCnt[i].resetSynTime)
				{

					MasterFVM_getResetValue(resetCnt[i].resetcanid, info);
					CanIf_Transmit(notifycanid, info);
					resetCnt[i].resetSyntag = 0; //重新计数
					break;
				}
			}
		}
	}
}
