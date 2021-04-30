// #include "i2c.h"
#include <string.h>
#include "masterFVM.h"
#include "MacGenerate.h"
#include "Csm_Types.h"
#include "../../Compiler_Cfg.h"
#include "MasterFVM_Cfg.h"
// #include "usart.h"
// #include "fvm_conf.h"

#define ADDR_24LCxx_Write 0xA0
#define ADDR_24LCxx_Read 0xA1

uint8 trip[3];			  //初始化时从非易失性存储器中获得并+1后再存回非易失性存储器， 低位先占满8字节 高位再占
						  //例如  TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][]
uint8 TripCntLength = 16; //可配置

// 返回一个uint8的实际位数
uint8 length(uint8 num)
{
	int temp = 0;
	while (num)
	{
		temp += 1;
		num >>= 1;
	}
	return temp;
}

// k=0..8,设置num的第k位为1
uint8 set_k(uint8 num, uint8 k)
{
	num += 1 << k;
	return num;
}

//第k位是否为1
uint8 is_k(uint8 num, uint8 k)
{
	if (num & (1 << k))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

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

	// HAL_I2C_Mem_Read(&hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, trip, 2, 0xff); //读取2字节的trip

	// 表面Read
	trip[1] = 153;
	trip[0] = 222;

	if (trip[1] == 255)
	{ //低位满，需进位
		if (trip[0] == 255)
		{ //trip值达到最大值
			trip[0] = 0;
			trip[1] = 1;
			TripCntLength = 1;
		}
		else
		{ //trip未达最大值， 高位进位，低位到0
			trip[0] += 1;
			trip[1] = 0;
			TripCntLength = (uint8)8 + length(trip[0]);
		}
	}
	else
	{
		trip[1] += 1;
		TripCntLength = (uint8)8 + length(trip[0]);
	}

	// for (i = 0; i < 2; i++)
	// {
	// 	HAL_I2C_Mem_Write(&hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &trip[i], 1, 0xff); //使用I2C块读，出错。因此采用此种方式，逐个单字节写入
	// 	HAL_Delay(5);																			  //此处延时必加，与AT24C02写时序有关
	// }
}

uint16 trip_can_id = 0x2bd;		   //可配置
uint32 jobId = 2333;			   // 自定义
Crypto_OperationModeType mode = 3; // 自定义
ResetCnt_Type current_reset;

void get_value(uint16 can_id, const PduInfoType *PduInfoPtr, uint8 tripCntLength, uint8 ResetCntLength)
{
	uint8 can_data[8];
	memset(can_data, 0, sizeof(can_data));

	// 声明两个存data的变量
	uint16 data_reset_num;
	uint16 data_trip;

	uint8 data_generate_mac[8];
	memset(data_generate_mac, 0, sizeof(data_generate_mac));
	// 向生成mac的原始数据加入canid

	data_generate_mac[0] = (uint8)(can_id >> 8);
	data_generate_mac[1] = (uint8)can_id;

	// 左移trip
	data_trip = trip[0];
	data_trip <<= 8;
	data_trip += trip[1];
	data_trip <<= (16 - tripCntLength);

	//向生成mac的原始数据加入trip
	data_generate_mac[2] = (uint8)(data_trip >> 8);
	data_generate_mac[3] = (uint8)(data_trip);

	// 向生成mac的原始数据中加入ResetCnt的值
	if (ResetCntLength == 0)
	{
		int flag = 0;
		for (int i = 2; i < 8 && flag == 0; ++i)
		{
			uint8 num = data_generate_mac[i];
			for (int j = 7; j >= 0; --j)
			{ // 遇到第一个为0的为置1
				if (!is_k(num, j))
				{
					data_generate_mac[i] = set_k(num, j);
					flag = 1;
					break;
				}
			}
		}
	}
	else
	{
		uint8* data_reset = current_reset.resetdata;
		data_reset_num = data_reset[0];
		data_reset_num <<= 8;
		data_reset_num += data_reset[1];
		data_reset_num <<= (16 - ResetCntLength);
		int reset_index = 0;
		int flag = 0;
		for (int i = 2; i < 8 && flag == 0; ++i)
		{
			for (int j = 7; j >= 0; --j)
			{
				if (!is_k(data_generate_mac[i], j))
				{
					if (data_reset_num & (1 << reset_index))
					{
						data_generate_mac[i] = set_k(data_generate_mac[i], j);
						reset_index++;
					}
					else
					{
						flag = 1;
						break;
					}
				}
			}
		}
	}
	// 生成mac值
	uint8 mac[8];
	uint8 mac_length[8];
	memset(mac, 0, sizeof(mac));
	memset(mac_length, 0, sizeof(mac_length));
//	Csm_MacGenerate(jobId, mode, data_generate_mac, 16 + tripCntLength + ResetCntLength, mac, mac_length);
	uint64 num_mac = 0;
	for (int i = 0; i < 8; ++i)
	{
		num_mac += mac[7 - i] << (8 * i);
	}
	num_mac >>= (16 + tripCntLength + ResetCntLength);

	uint8 mac_index = 0;
	// 数据重排
	if (ResetCntLength == 0)
	{ // trip + 1 + mac
		can_data[0] = data_trip >> 8;
		can_data[1] = (uint8)data_trip;
		int first = 0;
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 7; j >= 0; --j)
			{
				if (!is_k(can_data[i], j))
				{
					if (!first)
					{
						first = 1;
						can_data[i] = set_k(can_data[i], j);
					}
					else
					{
						if (num_mac & (1 << mac_index))
						{
							can_data[i] = set_k(can_data[i], j);
						}
						mac_index++;
					}
				}
			}
		}
	}
	else
	{ // reset + mac
		can_data[0] = data_reset_num >> 8;
		can_data[1] = (uint8)data_reset_num;
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 7; j >= 0; --j)
			{
				if (!is_k(can_data[i], j))
				{

					if (num_mac & (1 << mac_index))
					{
						can_data[i] = set_k(can_data[i], j);
					}
					mac_index++;
				}
			}
		}
	}

    PduInfoPtr = (const PduInfoType *) can_data;
}

FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
	/*
	默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
		trip(TripCntLength)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
		例如 TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
		由于后接1bit reset且值为1 则dataptr = [0x02] [0xbd] [1001 1111][1111 0000] 最后4位为补0
	1.构造生成mac的原始数据 char *dataptr 组成为  
	tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLength)+reset(1bit)：值为1 + padding0;
		在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
		例如  TripCntLength=12  [ 0x02 0xbd 0x00 0x14]  此时
	2.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
		获得mac值
	3. 数据排版 PduInfoPtr.SduDataPtr =trip(TripCntLength)+1(1bit)+ mac(64bit-TripCntLength-1);  
		重排位，在数据生成时不再填充0
	*/
	get_value(trip_can_id, PduInfoPtr, TripCntLength, 0);
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
	if (TxPduId < NUM_RESET)
	{
		current_reset = resetCnt[TxPduId];
		get_value(current_reset.resetcanid, PduInfoPtr, TripCntLength, current_reset.ResetCntLength);
	}
}

FUNC(void, MASTER_CODE)
MasterFVM_changestate(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId)
{
	/* 收到id后检查confirmECU 对应索引值， 修改state状态为1， 发送ackv的报文*/
	ConfirmECU_Type *tmp;
	tmp = &confirmECU[TxPduId];
	tmp->state = 1;
	PduInfoType *info;
//	CanIf_Transmit(tmp->ackv, info);
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
	PduInfoType *info = NULL;
	if (firsttrip == 0)
	{
		MasterFVM_getTripValue(info);
//		CanIf_Transmit(trip_can_id, info);
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
//						CanIf_Transmit(errorcanid, info);
						triperrorlabel = 1; //多次发送trip都失败 =1
					}
					else
					{
						MasterFVM_getTripValue(info);
//						CanIf_Transmit(trip_can_id, info);
						resendTriptag = 0;
						windowTimetag = 0;
					}
				}
				else
				{ // resendtag==0  不需要重发，发送通知报文，开始按周期发送同步消息
					info->SduDataPtr = NULL;
					info->SduLength = 8;
//					CanIf_Transmit(notifycanid, info);
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

					MasterFVM_getResetValue(i, info);
//					CanIf_Transmit(notifycanid, info);
					resetCnt[i].resetSyntag = 0; //重新计数
					break;
				}
			}
		}
	}
}
