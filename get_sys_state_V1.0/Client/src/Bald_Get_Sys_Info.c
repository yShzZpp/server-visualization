#include <linux/sysinfo.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

#include "../inc/Bald_Get_Sys_Info.h"
#include "../inc/Bald_cJSON.h"

time_t g_u32Time;
CPU_JIFFIES_ST g_stCpu;

#define MAX_BUFF_SIZE 1024

/******************************字符串处理函数******************************/
void GET_SYS_Del_Str_Useless(char *pcData)
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

int8_t GET_SYS_CpuNum(char *pcData)
{
	FILE *fp = popen("grep -c \"model name\" /proc/cpuinfo","r");	
	fread(pcData, MAX_BUFF_SIZE, 1, fp);
	fclose(fp);
	return 0;
}

int8_t GET_SYS_HostName(char *pcData)
{
	int fd = open("/etc/hostname",O_RDONLY);	
	read(fd,pcData,MAX_BUFF_SIZE);
	GET_SYS_Del_Str_Useless(pcData);
	close(fd);
	return 0;
}

int8_t GET_SYS_UserName(char *pcData)
{
	FILE *fp = popen("whoami","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

int8_t GET_SYS_Version(char *pcData)
{
	FILE *fp = popen("lsb_release -a|sed -n 3p |awk '{print$2}' 2>/dev/null","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

int8_t GET_SYS_Distributor(char *pcData)
{
	FILE *fp = popen("lsb_release -a|sed -n 1p|awk '{print$3}' 2>/dev/null","r");
	fread(pcData,MAX_BUFF_SIZE,1,fp);
	GET_SYS_Del_Str_Useless(pcData);
	fclose(fp);
	return 0;
}

cJSON *GET_SYS_GetOnce()
{
	cJSON *cjRoot = cJSON_CreateObject();
	char aTempBuff[MAX_BUFF_SIZE];	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//version
	if( GET_SYS_Version(aTempBuff) == 0)
	{
		cJSON_AddStringToObject(cjRoot, "version",(aTempBuff));	
	}

	//distributor
	if( GET_SYS_Distributor(aTempBuff) == 0)
	{
		cJSON_AddStringToObject(cjRoot, "distributor",(aTempBuff));	
	}

	//cpu数
	if( GET_SYS_CpuNum(aTempBuff) == 0)
	{
		cJSON_AddNumberToObject(cjRoot, "cpu_num", atoi(aTempBuff));	
	}

	bzero(aTempBuff, MAX_BUFF_SIZE);

	//hostname
	if( GET_SYS_HostName(aTempBuff) == 0)
	{
		cJSON_AddStringToObject(cjRoot, "host_name",(aTempBuff));	
	}
	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//username
	if( GET_SYS_UserName(aTempBuff) == 0)
	{
		cJSON_AddStringToObject(cjRoot, "user_name",(aTempBuff));	
	}

	return cjRoot;
}

/******************************开机时间******************************/
int16_t GET_SYS_UpTime(time_t *u32Time)
{
	struct sysinfo stSysInfo;
	if (sysinfo(&stSysInfo)) {
		return -1;
    }
	*u32Time = stSysInfo.uptime;
	g_u32Time = stSysInfo.uptime;

	return 0;
}


cJSON *GET_SYS_GetTime(void)
{
	cJSON *cjRoot = cJSON_CreateObject();

	char aTempBuff[MAX_BUFF_SIZE];	
	bzero(aTempBuff, MAX_BUFF_SIZE);

	//uptime
	time_t u32UpTime;	
	if( GET_SYS_UpTime(&u32UpTime) == 0)
	{
		cJSON_AddNumberToObject(cjRoot, "allsec", u32UpTime);
		cJSON_AddNumberToObject(cjRoot, "day", (uint32_t)u32UpTime/86400);
		cJSON_AddNumberToObject(cjRoot, "hour", (uint32_t)u32UpTime%86400/3600);
		cJSON_AddNumberToObject(cjRoot, "min", (uint32_t)u32UpTime%86400%3600/60);
		cJSON_AddNumberToObject(cjRoot, "sec", (uint32_t)u32UpTime%86400%3600%60);
	}

	return cjRoot;
}


/******************************内存******************************/

#define MEM_NUM 15
int16_t GET_SYS_Mem(uint32_t au32[MEM_NUM])
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
		sscanf(&aTempBuff[i], "%d[^ ]",&au32[j]);
		while(i < u32Len && aTempBuff[i] != ' ')i++;
		i++;
	}


	fclose(fp);
	return 0;
}

cJSON *GET_SYS_GetMem(void)
{
	cJSON *cjRoot = cJSON_CreateObject();
	uint32_t au32Mem[MEM_NUM];
	bzero(au32Mem, MEM_NUM);
	GET_SYS_Mem(au32Mem);

	cJSON_AddNumberToObject(cjRoot, "total_mem",au32Mem[0]);
	cJSON_AddNumberToObject(cjRoot, "used_mem",au32Mem[1]);
	cJSON_AddNumberToObject(cjRoot, "free_mem",au32Mem[2]);
	cJSON_AddNumberToObject(cjRoot, "shared_mem",au32Mem[3]);
	cJSON_AddNumberToObject(cjRoot, "buff/cache_mem",au32Mem[4]);
	cJSON_AddNumberToObject(cjRoot, "available_mem",au32Mem[5]);
	cJSON_AddNumberToObject(cjRoot, "total_low",au32Mem[6]);
	cJSON_AddNumberToObject(cjRoot, "used_low",au32Mem[7]);
	cJSON_AddNumberToObject(cjRoot, "free_low",au32Mem[8]);
	cJSON_AddNumberToObject(cjRoot, "total_high",au32Mem[9]);
	cJSON_AddNumberToObject(cjRoot, "used_high",au32Mem[10]);
	cJSON_AddNumberToObject(cjRoot, "free_high",au32Mem[11]);
	cJSON_AddNumberToObject(cjRoot, "total_sawp",au32Mem[12]);
	cJSON_AddNumberToObject(cjRoot, "used_sawp",au32Mem[13]);
	cJSON_AddNumberToObject(cjRoot, "free_swap",au32Mem[14]);

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
	char aTempBuff[2048];
	int fd = open("/proc/stat",O_RDONLY);
	if(fd < 0)
	{
		perror("open /proc/stat error");
		return -1;
	}

	//读取
	uint32_t u32Len = read(fd,aTempBuff,2048);
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



/******************************cpu使用率******************************/






















