#ifndef __BALD_GET_SYS_INFO_H_
#define __BALD_GET_SYS_INFO_H_

#include <stdio.h>
#include <stdint.h>
#include "../inc/Bald_cJSON.h"

extern const char * const GET_SYS_STR_SHOW_ALL;
extern const char * const GET_SYS_STR_SHOW;
extern const char * const GET_SYS_STR_EXCLUDE ;


typedef struct net_tx_rx_st
{
	uint32_t u32Bytes;				//字节
	uint32_t u32Packets;			//包数
	uint32_t u32Errs;				//出错包
	uint32_t u32Drop;				//
	uint32_t u32Fifo;				//fifo
	uint32_t u32Frame;				//frame
	uint32_t u32Compressed;			//compressed
	uint32_t u32Multicast;			//multicast
}NET_TX_RX_ST;


typedef struct net_interface_st
{
	char cName[20];					//网口名
	NET_TX_RX_ST stRx;				//接收 下行
	NET_TX_RX_ST stTx;				//接收 上行
}NET_INTERFACE_ST;

typedef struct net_info_st
{
	uint32_t u32NetNum;
	NET_INTERFACE_ST stNetInter[0];
}NET_INFO_ST;


typedef struct cpu_jiffies_st
{
	char cName[20];					//cpu名称
	uint32_t u32User;				//用户态的cpu时间
	uint32_t u32Nice;				//nice值为负的进程占用的时间
	uint32_t u32Sys;				//内核态的cpu时间
	uint32_t u32Idle;				//出硬盘io等待时间以外的其他等待时间
	uint32_t u32Iowait;				//硬盘io等待时间
	uint32_t u32Irq;				//硬中断时间
	uint32_t u32Softirq;			//软中断时间
}CPU_JIFFIES_ST;

typedef struct cpu_info_st
{
	uint32_t u32CpuNum;
	CPU_JIFFIES_ST stCpu[0];
}CPU_INFO_ST;








void GET_SYS_CpuInit(void);
void GET_SYS_NetInit(void);
void GET_SYS_DiskInit(void);
void GET_SYS_DiskFree(void);

cJSON *GET_SYS_GetNet();
cJSON *GET_SYS_GetOnce(cJSON *cjOpt);
cJSON *GET_SYS_GetTime(cJSON *cjOpt);
cJSON *GET_SYS_GetMem(cJSON *cjOpt);
cJSON *GET_SYS_GetCpu(cJSON *cjOpt);
cJSON *GET_SYS_GetDisk(void);






#endif
