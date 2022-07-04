#include <linux/sysinfo.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

#include "../inc/Bald_Get_Sys_Info.h"
#include "../inc/Bald_cJSON.h"

#define MAX_BUFF_SIZE 4096
#define MAX_DISK_NUM 40

const char * const GET_SYS_STR_SHOW_ALL = "all";
const char * const GET_SYS_STR_SHOW = "s";
const char * const GET_SYS_STR_EXCLUDE = "e";
char *g_paDiskPath[MAX_DISK_NUM];
uint8_t g_u8DiskNum = 0;

time_t g_u32TimeInterval;
CPU_JIFFIES_ST g_stCpu;
NET_INFO_ST *g_pstNet;


/******************************字符串处理函数******************************/

/******************************************************
 * ****** Function		:	GET_SYS_Del_Str_Useless 
 * ****** brief			:	删除字符串前后的空格 和 换行
 * ****** param			:	@pcData:需要修改的字符串
 * ****** return		:   NULL
 * *******************************************************/
static void GET_SYS_Del_Str_Useless(char *pcData)
{
	uint32_t u32Len = strlen(pcData);
	char aTempData[u32Len];
	memcpy(aTempData, pcData, u32Len);

	uint32_t i = 0,j = u32Len-1;
	while( i < u32Len && (pcData[i] == ' '||pcData[i] == '\n') )i++;
	while( j >= 0	  && (pcData[j] == ' '||pcData[j] == '\n') )j--;

	memset(pcData,0,u32Len);
	memcpy(pcData,aTempData+i, j-i+1);
}

	

/******************************只获取一次******************************/

/******************************************************
 * ****** Function		:   GET_SYS_CpuNum
 * ****** brief			:	使用命令的方式获取 cpu的数量	
 * ****** param			:	@pcData:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int8_t GET_SYS_CpuNum(char *pcData)
{
	FILE *fp = popen("grep -c \"model name\" /proc/cpuinfo","r");	
	fread(pcData, MAX_BUFF_SIZE, 1, fp);
	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_HostName
 * ****** brief			:	使用命令的方式获取 主机名
 * ****** param			:	@pcData:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int8_t GET_SYS_HostName(char *pcData)
{
	int fd = open("/etc/hostname",O_RDONLY);	
	read(fd,pcData,MAX_BUFF_SIZE);
	GET_SYS_Del_Str_Useless(pcData);
	close(fd);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_UserName
 * ****** brief			:	使用命令的方式获取用户的名称 
 * ****** param			:	@pcData:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int8_t GET_SYS_UserName(char *pcData)
{
	FILE *fp = popen("whoami","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_Version
 * ****** brief			:	使用命令的方式获取系统版本
 * ****** param			:	@pcData:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int8_t GET_SYS_Version(char *pcData)
{
	FILE *fp = popen("lsb_release -a|sed -n 4p |awk '{print$2}' 2>/dev/null","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_Distributor
 * ****** brief			:	使用命令的方式获取系统发行系统
 * ****** param			:	@pcData:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int8_t GET_SYS_Distributor(char *pcData)
{
	FILE *fp = popen("lsb_release -a|sed -n 2p|awk '{print$3}' 2>/dev/null","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_GetOnce
 * ****** brief			:	解析传入的 筛选设置json，根据设置 获取对应的数值并写入json
 * ****** param			:   @cjOpt:筛选设置
 * ****** return		:   无论成功与否都返回json obj
 * *******************************************************/
cJSON *GET_SYS_GetOnce(cJSON *cjOpt)
{
	uint8_t au8Show[5] = {0};

	cJSON *cjRoot = cJSON_CreateObject();

	char aTempBuff[MAX_BUFF_SIZE];	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//设置json为展示全部
	if(cjOpt->type == cJSON_String && cjOpt->valuestring != NULL && strcmp(GET_SYS_STR_SHOW_ALL,cjOpt->valuestring) == 0)
	{
		for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t); i ++ )
		{
			au8Show[i] = true;
		}
	}
	//设置json为指定展示
	else if(cjOpt->type == cJSON_Object)
	{
		cJSON *cjArray;
		cJSON *cjTempItem;
		bool bShowOrNot = false;
		//指定展示
		if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_SHOW)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = true;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = false;
			}
		}
		//指定不展示
		else if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_EXCLUDE)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = false;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = true;
			}
		}
		//都没有
		else
		{
			printf("json do not have \"e\" or \"s\"\n");
			return cjRoot;
		}

		uint8_t u8Len = cJSON_GetArraySize(cjArray);
		for ( int i = 0 ; i < u8Len ; i++ )
		{
			cjTempItem = cJSON_GetArrayItem(cjArray, i);
			if( cjTempItem != NULL && strcmp(cjTempItem->valuestring,"version") == 0)
			{
				au8Show[0] = bShowOrNot;
			}
			else if( cjTempItem != NULL && strcmp(cjTempItem->valuestring,"distributor") == 0)
			{
				au8Show[1] = bShowOrNot;
			}
			else if( cjTempItem != NULL && strcmp(cjTempItem->valuestring,"cpu_num") == 0)
			{
				au8Show[2] = bShowOrNot;
			}
			else if( cjTempItem != NULL && strcmp(cjTempItem->valuestring,"host_name") == 0)
			{
				au8Show[3] = bShowOrNot;
			}
			else if( cjTempItem != NULL && strcmp(cjTempItem->valuestring,"user_name") == 0)
			{
				au8Show[4] = bShowOrNot;
			}
		}

	}

	//写入
	//version
	if( ( au8Show[0] ) && ( GET_SYS_Version(aTempBuff) == 0))
	{
		cJSON_AddStringToObject(cjRoot, "version",(aTempBuff));	
	}

	//distributor
	if( ( au8Show[1] ) && ( GET_SYS_Distributor(aTempBuff) == 0))
	{
		cJSON_AddStringToObject(cjRoot, "distributor",(aTempBuff));	
	}

	//cpu数
	if( ( au8Show[2] ) && ( GET_SYS_CpuNum(aTempBuff) == 0))
	{
		cJSON_AddNumberToObject(cjRoot, "cpu_num", atoi(aTempBuff));	
	}

	bzero(aTempBuff, MAX_BUFF_SIZE);

	//hostname
	if( ( au8Show[3] ) && ( GET_SYS_HostName(aTempBuff) == 0))
	{
		cJSON_AddStringToObject(cjRoot, "host_name",(aTempBuff));	
	}
	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//username
	if( ( au8Show[4] ) && ( GET_SYS_UserName(aTempBuff) == 0))
	{
		cJSON_AddStringToObject(cjRoot, "user_name",(aTempBuff));	
	}

	return cjRoot;
}

/******************************开机时间******************************/

/******************************************************
 * ****** Function		:   GET_SYS_UpTime
 * ****** brief			:	获取开机时间 并计算执行的间隔 用于计算网络上下行速度
 * ****** param			:   @u32Time:写入结果的地址
 * ****** return		:	成功返回0 错误返回-1
 * *******************************************************/
static int16_t GET_SYS_UpTime(time_t *u32Time)
{
	struct sysinfo stSysInfo;
	if (sysinfo(&stSysInfo)) {
		return -1;
    }
	*u32Time = stSysInfo.uptime;
	//计算时间差
	g_u32TimeInterval = (g_u32TimeInterval > *u32Time || g_u32TimeInterval == 0)? *u32Time : *u32Time - g_u32TimeInterval ;

	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_GetTime
 * ****** brief			:	根据传入的筛选设置，将指定的参数写入json
 * ****** param			:	@cjOpt:筛选参数
 * ****** return		:	成功与否都返回json obj
 * *******************************************************/
cJSON *GET_SYS_GetTime(cJSON *cjOpt)
{
	cJSON *cjRoot = cJSON_CreateObject();
	uint8_t au8Show[5] = {0};

	char aTempBuff[MAX_BUFF_SIZE];	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//全部输出
	if(cjOpt->type == cJSON_String && strcmp(GET_SYS_STR_SHOW_ALL, cjOpt->valuestring) == 0)
	{

		for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
		{
			au8Show[i] = true;
		}
	}
	//筛选输出
	else if(cjOpt->type == cJSON_Object)
	{
		cJSON *cjArray;
		cJSON *cjTempItem;
		bool bShowOrNot = false;
		//指定展示
		if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_SHOW)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = true;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = false;
			}
		}
		//指定不展示
		else if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_EXCLUDE)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = false;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = true;
			}
		}
		//都没有
		else
		{
			printf("json do not have \"e\" or \"s\"\n");
			return cjRoot;
		}

		uint32_t u32Len = cJSON_GetArraySize(cjArray);

		for ( int i = 0 ; i < u32Len ; i ++ )
		{
			cjTempItem = cJSON_GetArrayItem(cjArray, i);
			if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"allsec") == 0)
			{
				au8Show[0] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"day") == 0)
			{
				au8Show[1] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"hour") == 0)
			{
				au8Show[2] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"min") == 0)
			{
				au8Show[3] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"sec") == 0)
			{
				au8Show[4] = bShowOrNot;
			}
		}
	}

	//获取时间
	time_t u32UpTime;	
	if( GET_SYS_UpTime(&u32UpTime) == -1 )
	{
		return cjRoot;
	}

	if(au8Show[0])
	{
		cJSON_AddNumberToObject(cjRoot, "allsec", u32UpTime);
	}
	if(au8Show[1])
	{
		cJSON_AddNumberToObject(cjRoot, "day", (uint32_t)(u32UpTime/86400));
	}
	if(au8Show[2])
	{
		cJSON_AddNumberToObject(cjRoot, "hour", (uint32_t)(u32UpTime%86400/3600));
	}
	if(au8Show[3])
	{
		cJSON_AddNumberToObject(cjRoot, "min", (uint32_t)(u32UpTime%86400%3600/60));
	}
	if(au8Show[4])
	{
		cJSON_AddNumberToObject(cjRoot, "sec", (uint32_t)(u32UpTime%86400%3600%60));
	}

	return cjRoot;
}


/******************************内存******************************/

#define MEM_NUM 15
/******************************************************
 * ****** Function		:   GET_SYS_Mem
 * ****** brief			:	使用free命令获取 内存
 * ****** param			:   @au32Mem:存放信息的数组
 * ****** return		:   成功返回0 失败成功返回0 失败返回-1
 * *******************************************************/
int16_t GET_SYS_Mem(uint32_t au32Mem[MEM_NUM])
{
	char aTempBuff[MAX_BUFF_SIZE];
	//按kB为单位获取 内存信息 需要兼容
	FILE *fp = popen("free -lk |sed 1d |awk '{print$2,$3,$4,$5,$6}'|tr '\n' ' '","r");	
	fread(aTempBuff, MAX_BUFF_SIZE, 1, fp);
	uint32_t u32Len = strlen(aTempBuff);
	if(u32Len == 0)
	{
		fclose(fp);
		printf("read %d\n",u32Len);
		return -1;
	}

	uint32_t i = 0,j = 0;
	for ( j = 0 ; j < MEM_NUM; j++ )
	{
		sscanf(&aTempBuff[i], "%d[^ ]",&au32Mem[j]);
		while(i < u32Len && aTempBuff[i] != ' ')i++;
		i++;
	}


	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   GET_SYS_GetMem
 * ****** brief			:	根据传入的筛选设置，将指定的参数写入json
 * ****** param			:	@cjOpt:筛选设置json
 * ****** return		:	成功与否都返回json
 * *******************************************************/
cJSON *GET_SYS_GetMem(cJSON *cjOpt)
{
	cJSON *cjRoot = cJSON_CreateObject();

	uint32_t au32Mem[MEM_NUM];
	bzero(au32Mem, MEM_NUM);

	uint8_t au8Show[MEM_NUM];

	if(GET_SYS_Mem(au32Mem) == -1)
	{
		return cjRoot;
	}

	//全部输出
	if(cjOpt->type == cJSON_String && strcmp(GET_SYS_STR_SHOW_ALL, cjOpt->valuestring) == 0)
	{

		for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
		{
			au8Show[i] = true;
		}
	}
	//筛选输出
	else if(cjOpt->type == cJSON_Object)
	{
		cJSON *cjArray;
		cJSON *cjTempItem;
		bool bShowOrNot = false;
		//指定展示
		if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_SHOW)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = true;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = false;
			}
		}
		//指定不展示
		else if((cjArray = cJSON_GetObjectItem(cjOpt, GET_SYS_STR_EXCLUDE)) != NULL && cjArray->type == cJSON_Array)
		{
			bShowOrNot = false;
			for ( int i = 0 ; i < sizeof(au8Show)/sizeof(uint8_t) ; i ++ )
			{
				au8Show[i] = true;
			}
		}
		//都没有
		else
		{
			printf("json do not have \"e\" or \"s\"\n");
			return cjRoot;
		}

		uint32_t u32Len = cJSON_GetArraySize(cjArray);

		for ( int i = 0 ; i < u32Len ; i ++ )
		{
			cjTempItem = cJSON_GetArrayItem(cjArray, i);
			if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"total_mem") == 0)
			{
				au8Show[0] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"used_mem") == 0)
			{
				au8Show[1] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"free_mem") == 0)
			{
				au8Show[2] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"shared_mem") == 0)
			{
				au8Show[3] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"buff_mem") == 0)
			{
				au8Show[4] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"available_mem") == 0)
			{
				au8Show[5] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"total_low") == 0)
			{
				au8Show[6] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"used_low") == 0)
			{
				au8Show[7] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"free_low") == 0)
			{
				au8Show[8] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"total_high") == 0)
			{
				au8Show[9] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"used_high") == 0)
			{
				au8Show[10] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"free_high") == 0)
			{
				au8Show[11] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"total_swap") == 0)
			{
				au8Show[12] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"used_swap") == 0)
			{
				au8Show[13] = bShowOrNot;
			}
			else if(cjTempItem != NULL && strcmp(cjTempItem->valuestring,"free_swap") == 0)
			{
				au8Show[14] = bShowOrNot;
			}
		}
	}

	if(au8Show[0] > 0)
	{
		cJSON_AddNumberToObject(cjRoot, "total_mem",au32Mem[0]);
	}
	if(au8Show[1])
	{
		cJSON_AddNumberToObject(cjRoot, "used_mem",au32Mem[1]);
	}
	if(au8Show[2])
	{
		cJSON_AddNumberToObject(cjRoot, "free_mem",au32Mem[2]);
	}
	if(au8Show[3])
	{
		cJSON_AddNumberToObject(cjRoot, "shared_mem",au32Mem[3]);
	}
	if(au8Show[4])
	{
		cJSON_AddNumberToObject(cjRoot, "buff_mem",au32Mem[4]);
	}
	if(au8Show[5])
	{
		cJSON_AddNumberToObject(cjRoot, "available_mem",au32Mem[5]);
	}
	if(au8Show[6])
	{
		cJSON_AddNumberToObject(cjRoot, "total_low",au32Mem[6]);
	}
	if(au8Show[7])
	{
		cJSON_AddNumberToObject(cjRoot, "used_low",au32Mem[7]);
	}
	if(au8Show[8])
	{
		cJSON_AddNumberToObject(cjRoot, "free_low",au32Mem[8]);
	}
	if(au8Show[9])
	{
		cJSON_AddNumberToObject(cjRoot, "total_high",au32Mem[9]);
	}
	if(au8Show[10])
	{
		cJSON_AddNumberToObject(cjRoot, "used_high",au32Mem[10]);
	}
	if(au8Show[11])
	{
		cJSON_AddNumberToObject(cjRoot, "free_high",au32Mem[11]);
	}
	if(au8Show[12])
	{
		cJSON_AddNumberToObject(cjRoot, "total_swap",au32Mem[12]);
	}
	if(au8Show[13])
	{
		cJSON_AddNumberToObject(cjRoot, "used_swap",au32Mem[13]);
	}
	if(au8Show[14])
	{
		cJSON_AddNumberToObject(cjRoot, "free_swap",au32Mem[14]);
	}
	return cjRoot;
}


/******************************cpu使用率******************************/

//使用sh命令的方法，但是top执行起来会很慢 而且分开三次的结果不是一个同样的结果
int16_t GET_SYS_UserCpu(char * pcData)
{
	FILE *fp = popen("top -b -n 1 | grep Cpu | awk '{print $2}' |tr -d '\n' ","r");	
	int ret = fread(pcData, MAX_BUFF_SIZE, 1, fp);
	/** printf("%s\n",pcData); */
	if(ret != 0 )
	{
		fclose(fp);
		return -1;
	}

	return 0;

}

int16_t GET_SYS_SysCpu(char * pcData)
{
	FILE *fp = popen("top -b -n 1 | grep Cpu | awk '{print $4}' |tr -d '\n' ","r");	
	int ret = fread(pcData, MAX_BUFF_SIZE, 1, fp);
	/** printf("%s\n",pcData); */
	if(ret != 0 )
	{
		fclose(fp);
		return -1;
	}
	return 0;
}

int16_t GET_SYS_NiceCpu(char * pcData)
{
	FILE *fp = popen("top -b -n 1 | grep Cpu | awk '{print $6}' |tr -d '\n' ","r");	
	int ret = fread(pcData, MAX_BUFF_SIZE, 1, fp);
	/** printf("%s\n",pcData); */
	if(ret != 0 )
	{
		fclose(fp);
		return -1;
	}
	return 0;
}


int16_t GET_SYS_Proc_Stat(CPU_INFO_ST **ppstCpu)
{
	char aTempBuff[4096];
	int fd = open("/proc/stat",O_RDONLY);
	if(fd < 0)
	{
		perror("open /proc/stat error");
		return -1;
	}

	//读取
	uint32_t u32Len = read(fd,aTempBuff,4096);
	if(u32Len == 0)
	{
		perror("read /proc/stat error");
		return -1;
	}

	//计算cpu数
	uint32_t i =0;
	while( i < u32Len && aTempBuff[i] != 'i' && aTempBuff[i+1] != 'n' && aTempBuff[i+2] != 't' )
	{
		if(aTempBuff[i] == 'c' && aTempBuff[i+1] == 'p' && aTempBuff[i+2] == 'u')
		{
			(*ppstCpu)->u32CpuNum++;
			/** printf("%d   ",ppstCpu->u32CpuNum); */
		}
		i++;
	}

	//未获取到cpu数量
	if((*ppstCpu)->u32CpuNum == 0)
	{
		printf("no cpu !\n%s\n",aTempBuff);
		return -1;
	}
	/** ppstCpu->u32CpuNum--; */
	//开辟空间
	*ppstCpu = realloc(*ppstCpu, sizeof(CPU_INFO_ST) + sizeof(CPU_JIFFIES_ST) * (*ppstCpu)->u32CpuNum);
	if(*ppstCpu == NULL)
	{
		perror("cpu realloc error");
		return -1;
	}
	memset((*ppstCpu)->stCpu,0,sizeof(CPU_JIFFIES_ST) * (*ppstCpu)->u32CpuNum);

	//开始读取cpu数值
	i=0;
	for(uint32_t j = 0 ; j < (*ppstCpu)->u32CpuNum ; j++)
	{
		/** if( aTempBuff[i+1] != 'p' && aTempBuff[i+2] != 'u' )break; */
		sscanf(&aTempBuff[i],"%s %d %d %d %d %d %d %d",
					(*ppstCpu)->stCpu[j].cName,&(*ppstCpu)->stCpu[j].u32User,
					&(*ppstCpu)->stCpu[j].u32Nice,&(*ppstCpu)->stCpu[j].u32Sys,
					&(*ppstCpu)->stCpu[j].u32Idle,&(*ppstCpu)->stCpu[j].u32Iowait,
					&(*ppstCpu)->stCpu[j].u32Irq,&(*ppstCpu)->stCpu[j].u32Softirq);
		/** printf("%d,%d,%d,%d,%d,%d,%d\n",pstCpu->stCpu[j].u32User, */
		/**             pstCpu->stCpu[j].u32Nice,pstCpu->stCpu[j].u32Sys, */
		/**             pstCpu->stCpu[j].u32Idle,pstCpu->stCpu[j].u32Iowait, */
					/** pstCpu->stCpu[j].u32Irq,pstCpu->stCpu[j].u32Softirq); */
		i++;
		while( i < u32Len && aTempBuff[i] != 'c' )i++;
	}
	
	close(fd);
	return 0;
}

cJSON *GET_SYS_GetCpu(void)
{
	int ret;
	cJSON *cjRoot = cJSON_CreateObject();
	cJSON *cjaCpuArray = cJSON_CreateArray();	
	
	CPU_INFO_ST *pstCpu = malloc(sizeof(CPU_INFO_ST));
	bzero(pstCpu, sizeof(CPU_INFO_ST));
	pstCpu->u32CpuNum = 0;
	ret = GET_SYS_Proc_Stat(&pstCpu);
	if(ret == -1)
	{
		free(pstCpu); pstCpu = NULL;
		return NULL;
	}


	for(uint32_t i = 0 ; i < pstCpu->u32CpuNum ; i++)
	{
		cJSON *cjTempItem = cJSON_CreateObject();
		cJSON_AddStringToObject(cjTempItem, "name",pstCpu->stCpu[i].cName);
		cJSON_AddNumberToObject(cjTempItem, "user",pstCpu->stCpu[i].u32User);
		cJSON_AddNumberToObject(cjTempItem, "nice",pstCpu->stCpu[i].u32Nice);
		cJSON_AddNumberToObject(cjTempItem, "sys",pstCpu->stCpu[i].u32Sys);
		cJSON_AddNumberToObject(cjTempItem, "idle",pstCpu->stCpu[i].u32Idle);
		cJSON_AddNumberToObject(cjTempItem, "iowait",pstCpu->stCpu[i].u32Iowait);
		cJSON_AddNumberToObject(cjTempItem, "irq",pstCpu->stCpu[i].u32Irq);
		cJSON_AddNumberToObject(cjTempItem, "softirq",pstCpu->stCpu[i].u32Softirq);


		cJSON_AddItemToArray(cjaCpuArray, cjTempItem);
	}

	//判断是否是第一次
	if( g_stCpu.u32User == 0 )
	{
		memcpy(&g_stCpu,&pstCpu->stCpu[0],sizeof(CPU_INFO_ST)+sizeof(CPU_JIFFIES_ST) * pstCpu->u32CpuNum);
	}
	
	//计算使用率
	//当前的cpu总量
	uint32_t u32Total_2 = pstCpu->stCpu[0].u32User + 
			pstCpu->stCpu[0].u32Nice + pstCpu->stCpu[0].u32Sys + 
			pstCpu->stCpu[0].u32Idle + pstCpu->stCpu[0].u32Iowait + 
			pstCpu->stCpu[0].u32Irq + pstCpu->stCpu[0].u32Softirq ;

	//上次的cpu总量
	uint32_t u32Total_1 = g_stCpu.u32User + 
			g_stCpu.u32Nice + g_stCpu.u32Sys + 
			g_stCpu.u32Idle + g_stCpu.u32Iowait + 
			g_stCpu.u32Irq + g_stCpu.u32Softirq ;
	
	/** float fCpuUsed_1 = ((	g_stCpu.stCpu[0].u32User - pstCpu->stCpu[0].u32User) +  */
	/**                 ( g_stCpu.stCpu[0].u32Sys - pstCpu->stCpu[0].u32Sys)) *100.0 /  */
	/**                 ((	g_stCpu.stCpu[0].u32User - pstCpu->stCpu[0].u32User) +  */
	/**                 ( g_stCpu.stCpu[0].u32Sys - pstCpu->stCpu[0].u32Sys) + */
	/**                 ( g_stCpu.stCpu[0].u32Idle - pstCpu->stCpu[0].u32Idle)); */

	if(u32Total_1 - u32Total_2 > 0)
	{
		float fCpuUser = ( pstCpu->stCpu[0].u32User - g_stCpu.u32User ) * 100.0 /
						(u32Total_2 - u32Total_1);
		float fCpuSys = ( pstCpu->stCpu[0].u32Sys - g_stCpu.u32Sys ) * 100.0 /
						(u32Total_2 - u32Total_1);
		float fCpuNice = (pstCpu->stCpu[0].u32Nice - g_stCpu.u32Nice ) * 100.0 /
						(u32Total_2 - u32Total_1);
		float fCpuIdle = ( pstCpu->stCpu[0].u32Idle - g_stCpu.u32Idle ) * 100.0 /
						(u32Total_2 - u32Total_1);
		float fCpuIowait = ( pstCpu->stCpu[0].u32Iowait - g_stCpu.u32Iowait ) * 100.0 /
						(u32Total_2 - u32Total_1);
		cJSON_AddNumberToObject(cjRoot, "us", fCpuUser);
		cJSON_AddNumberToObject(cjRoot, "sy", fCpuSys);
		cJSON_AddNumberToObject(cjRoot, "ni", fCpuNice);
		cJSON_AddNumberToObject(cjRoot, "id", fCpuIdle);
		cJSON_AddNumberToObject(cjRoot, "wa", fCpuIowait);
	}


	memcpy(&g_stCpu,&pstCpu->stCpu[0],sizeof(CPU_INFO_ST)+sizeof(CPU_JIFFIES_ST) * pstCpu->u32CpuNum);

	free(pstCpu);
	pstCpu = NULL;

	cJSON_AddItemToObject(cjRoot, "jiffies", cjaCpuArray);


	return cjRoot;
}

void GET_SYS_CpuInit(void)
{
	//最大32个cpu
	bzero(&g_stCpu, sizeof(CPU_JIFFIES_ST));
}



/******************************网络使用率******************************/

NET_INTERFACE_ST g_stNet;

int16_t GET_SYS_Read_Proc_Net_Dev(NET_INFO_ST **ppstNet)
{
	int fd;
	char aTempBuff[4096];
	memset(aTempBuff,0,4096);

	//打开文件
	fd = open ("/proc/net/dev",O_RDONLY);

	if(fd < 0)
	{
		perror("open /proc/net/dev error");
		return -1;
	}

	//读取文件
	uint32_t u32Len = read(fd,aTempBuff,4096);
	if(u32Len == 0)
	{
		perror("read /proc/net/dev error");
		return -1;
	}

	//判断有多少个网口
	uint32_t i = 0 , j = 0;
	while( i < u32Len )
	{
		if(aTempBuff[i] == '\n')
		{
			(*ppstNet)->u32NetNum++;
			if((*ppstNet)->u32NetNum == 2)j = i+1;
		}
		i++;
	}

	(*ppstNet)->u32NetNum -= 2;

	//重新开辟空间
	*ppstNet = realloc(*ppstNet, sizeof(NET_INFO_ST) + sizeof(NET_INTERFACE_ST) * (*ppstNet)->u32NetNum);
	memset((*ppstNet)->stNetInter,0, sizeof(NET_INTERFACE_ST) * (*ppstNet)->u32NetNum);

	for( i = 0 ; i < (*ppstNet)->u32NetNum ;i++ )
	{
		while(aTempBuff[j] == ' ' || aTempBuff[j] == '\n')j++;
		sscanf(&aTempBuff[j],"%[^:]%*[^0-9]%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			(*ppstNet)->stNetInter[i].cName,
			&(*ppstNet)->stNetInter[i].stRx.u32Bytes, &(*ppstNet)->stNetInter[i].stRx.u32Packets,
			&(*ppstNet)->stNetInter[i].stRx.u32Errs,&(*ppstNet)->stNetInter[i].stRx.u32Drop,
			&(*ppstNet)->stNetInter[i].stRx.u32Fifo, &(*ppstNet)->stNetInter[i].stRx.u32Frame,
			&(*ppstNet)->stNetInter[i].stRx.u32Compressed,&(*ppstNet)->stNetInter[i].stRx.u32Multicast,
			&(*ppstNet)->stNetInter[i].stTx.u32Bytes, &(*ppstNet)->stNetInter[i].stTx.u32Packets,
			&(*ppstNet)->stNetInter[i].stTx.u32Errs,&(*ppstNet)->stNetInter[i].stTx.u32Drop,
			&(*ppstNet)->stNetInter[i].stTx.u32Fifo, &(*ppstNet)->stNetInter[i].stTx.u32Frame,
			&(*ppstNet)->stNetInter[i].stTx.u32Compressed,&(*ppstNet)->stNetInter[i].stTx.u32Multicast);	
		j++;
		while(j <u32Len && aTempBuff[j] != '\n')j++;
	}


	close(fd);
	return 0;
}



cJSON *GET_SYS_GetNet(void)
{
	cJSON *cjaRoot = cJSON_CreateArray();
	NET_INFO_ST *pstNet = malloc(sizeof(NET_INFO_ST));
	memset(pstNet,0,sizeof(NET_INFO_ST));

	int ret = GET_SYS_Read_Proc_Net_Dev(&pstNet);
	if(ret == -1)
	{
		free(pstNet);
		pstNet = NULL;
	}

	for(uint32_t i = 0 ; i < pstNet->u32NetNum ; i ++)
	{

		cJSON *cjTempItem = cJSON_CreateObject();
		cJSON_AddStringToObject(cjTempItem, "name", pstNet->stNetInter[i].cName);

		cJSON *cjRx = cJSON_CreateObject();
		cJSON_AddNumberToObject(cjRx,"bytes", pstNet->stNetInter[i].stRx.u32Bytes);
		cJSON_AddNumberToObject(cjRx,"packets", pstNet->stNetInter[i].stRx.u32Packets);
		cJSON_AddNumberToObject(cjRx,"errs", pstNet->stNetInter[i].stRx.u32Errs);
		cJSON_AddNumberToObject(cjRx,"drop", pstNet->stNetInter[i].stRx.u32Drop);
		cJSON_AddNumberToObject(cjRx,"fifo", pstNet->stNetInter[i].stRx.u32Fifo);
		cJSON_AddNumberToObject(cjRx,"frame", pstNet->stNetInter[i].stRx.u32Frame);
		cJSON_AddNumberToObject(cjRx,"compressed", pstNet->stNetInter[i].stRx.u32Compressed);
		cJSON_AddNumberToObject(cjRx,"multicast", pstNet->stNetInter[i].stRx.u32Multicast);


		cJSON *cjTx = cJSON_CreateObject();
		cJSON_AddNumberToObject(cjTx,"bytes", pstNet->stNetInter[i].stTx.u32Bytes);
		cJSON_AddNumberToObject(cjTx,"packets", pstNet->stNetInter[i].stTx.u32Packets);
		cJSON_AddNumberToObject(cjTx,"errs", pstNet->stNetInter[i].stTx.u32Errs);
		cJSON_AddNumberToObject(cjTx,"drop", pstNet->stNetInter[i].stTx.u32Drop);
		cJSON_AddNumberToObject(cjTx,"fifo", pstNet->stNetInter[i].stTx.u32Fifo);
		cJSON_AddNumberToObject(cjTx,"frame", pstNet->stNetInter[i].stTx.u32Frame);
		cJSON_AddNumberToObject(cjTx,"compressed", pstNet->stNetInter[i].stTx.u32Compressed);
		cJSON_AddNumberToObject(cjTx,"multicast", pstNet->stNetInter[i].stTx.u32Multicast);

		//计算速度
		if(g_pstNet->u32NetNum > i && strcmp(g_pstNet->stNetInter[i].cName,pstNet->stNetInter[i].cName)  == 0 && g_u32TimeInterval > 0 && g_u32TimeInterval < 100)
		{
			//rx 下行速度	
			float fRx = (pstNet->stNetInter[i].stRx.u32Bytes * 1.0 - 
						g_pstNet->stNetInter[i].stRx.u32Bytes) / g_u32TimeInterval;
			cJSON_AddNumberToObject(cjRx, "speeds", fRx);

			//tx 下行速度
			float fTx = (pstNet->stNetInter[i].stTx.u32Bytes * 1.0 - 
						g_pstNet->stNetInter[i].stTx.u32Bytes) / g_u32TimeInterval;
			cJSON_AddNumberToObject(cjTx, "speeds", fTx);
		}

		cJSON_AddItemToObject(cjTempItem,"rx",cjRx);
		cJSON_AddItemToObject(cjTempItem,"tx",cjTx);

		cJSON_AddItemToArray(cjaRoot, cjTempItem);
	}

	//给全局的变量分配空间
	g_pstNet = realloc(g_pstNet,sizeof(NET_INFO_ST) + sizeof(NET_INTERFACE_ST) * pstNet->u32NetNum);
	memset(g_pstNet,0,sizeof(NET_INFO_ST) + sizeof(NET_INTERFACE_ST) * pstNet->u32NetNum);
	memcpy(g_pstNet,pstNet,sizeof(NET_INFO_ST) + sizeof(NET_INTERFACE_ST) * pstNet->u32NetNum);

	return cjaRoot;
}


void GET_SYS_NetInit()
{
	g_pstNet = malloc(sizeof(NET_INFO_ST));
	memset(g_pstNet,0,sizeof(NET_INFO_ST));
}



/******************************磁盘******************************/

void GET_SYS_DiskInit()
{
	char aTempBuff[MAX_BUFF_SIZE];
	bzero(aTempBuff, MAX_BUFF_SIZE);
	
	//获取磁盘数量
	FILE *fp = popen("df -h|sed 1d |awk '{print $6}' |wc -l 2>/dev/null","r");
	fread(aTempBuff, MAX_BUFF_SIZE, 1, fp);
	g_u8DiskNum = atoi(aTempBuff)  > MAX_DISK_NUM ? MAX_DISK_NUM : atoi(aTempBuff);
	fclose(fp);
	

	bzero(aTempBuff, MAX_BUFF_SIZE);
	//获取磁盘名字
	fp = popen("df -h|sed 1d |awk '{print $6}' |tr '\n' ' '","r");
	fread(aTempBuff,MAX_BUFF_SIZE,1,fp);

	//计算buff长度
	uint32_t u32Len = strlen(aTempBuff);

	//i:用于g_paDiskPath指针数组的偏移 j:用于计算aTempBuff的偏移 k:用于计算每一个磁盘名称的长度
	uint32_t j = 0 ,k = 0;
	for ( int i = 0 ; i < g_u8DiskNum ; i ++ )
	{
		while(j < u32Len && aTempBuff[j] != ' ')
		{
			j++; 
		}
		//分配空间
		g_paDiskPath[i] = malloc(sizeof(char*) * (j - k) );
		memcpy(g_paDiskPath[i],&aTempBuff[k], j - k);
		/** printf("%s\n",g_paDiskPath[i]); */

		k = ++j;
	}
	fclose(fp);
}

int16_t GET_SYS_Disk(struct statfs *pstDisk,char *pcName)
{
	int ret;
	ret = statfs(pcName, pstDisk);
	if(ret != 0)
	{
		printf("%s",pcName);
		perror("statfs  error");
		return -1;
	}

	return 0;
}

cJSON *GET_SYS_GetDisk(void)
{
	cJSON * cjaRoot =cJSON_CreateArray();
	float fUsed;
	char aFsid[20] ;

	for ( int i = 0 ; i < g_u8DiskNum ; i++ )
	{
		cJSON *cjTempItem = cJSON_CreateObject();

		struct statfs stDisk;
		if(GET_SYS_Disk(&stDisk,g_paDiskPath[i]) == -1)
		/** if(GET_SYS_Disk(&stDisk,"/home/zzpp") == -1) */
		{
			continue;
		}

		cJSON_AddStringToObject(cjTempItem, "name", g_paDiskPath[i]);

		switch(stDisk.f_type)
		{
			case 0xEF51:
			case 0xEF53:
				cJSON_AddStringToObject(cjTempItem, "type", "EXT");
				break;

			case 0x3d33:
				cJSON_AddStringToObject(cjTempItem, "type", "FAT");
				break;

			case 0x5346544e:
				cJSON_AddStringToObject(cjTempItem, "type", "NIFS");
				break;

			default:
				cJSON_AddNumberToObject(cjTempItem, "type", stDisk.f_type);
				break;
		}

		cJSON_AddNumberToObject(cjTempItem, "bsize", stDisk.f_bsize);
		cJSON_AddNumberToObject(cjTempItem, "blocks", stDisk.f_blocks);
		cJSON_AddNumberToObject(cjTempItem, "bfree", stDisk.f_bfree);
		cJSON_AddNumberToObject(cjTempItem, "bavail", stDisk.f_bavail);
		cJSON_AddNumberToObject(cjTempItem, "files", stDisk.f_files);
		cJSON_AddNumberToObject(cjTempItem, "ffree", stDisk.f_ffree);
		sprintf(aFsid,"%x-%x",stDisk.f_fsid.__val[0],stDisk.f_fsid.__val[1]);
		cJSON_AddStringToObject(cjTempItem, "fsid", aFsid);

		//剩余
		fUsed = stDisk.f_bfree *100.0 / stDisk.f_blocks;
		cJSON_AddNumberToObject(cjTempItem, "free_perc", fUsed);

		//可用
		fUsed =  stDisk.f_bavail * 100.0 / stDisk.f_blocks;
		cJSON_AddNumberToObject(cjTempItem, "avail_perc", fUsed);

		//已用
		fUsed = (stDisk.f_blocks - stDisk.f_bavail) * 100.0 / stDisk.f_blocks;
		cJSON_AddNumberToObject(cjTempItem, "used_perc", fUsed);

		cJSON_AddItemToArray(cjaRoot,cjTempItem);
	}


	return cjaRoot;
}

void GET_SYS_DiskFree()
{
	for ( int i = 0 ; i < g_u8DiskNum ; i ++ )
	{
		free(g_paDiskPath[i]);
	}
}













