#include "../inc/Bald_Client.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../inc/Bald_cJSON.h"
#include "../inc/Bald_Get_Sys_Info.h"

int g_fd = 0;

int8_t GET_SYS_UserName(char *pcData);

int main(int argc , char *argv[])
{
	uint16_t u16SleepTime = 0;
	uint16_t u16ExecTime = 1;
	struct sockaddr_in stClientAddr;
	socklen_t u32ClientSize;
	g_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    stClientAddr.sin_family=AF_INET;
	stClientAddr.sin_port=htons(8001);
	inet_pton(AF_INET,"169.254.141.47", &stClientAddr.sin_addr.s_addr);
	GET_SYS_CpuInit();

	/** int ret = connect(g_fd,(struct sockaddr*) &stClientAddr,sizeof(stClientAddr)); */
	if(argc == 3)
	{
		u16SleepTime = atoi(argv[1]);
		u16ExecTime = atoi(argv[2]);
	}
	else
	{
		printf("\033[31merror!\033[0m\n");
		printf("Usage:\n");
		printf(" \033[32m%s sleep_times exec_num\033[0m\n\n",argv[0]);

		printf(" sleep_times	:The time between each exec\n");
		printf(" exec_num	:The number of executions ,0 is infinite\n");
		exit(0);
	}


	uint16_t u16Time = 0;

	do
	{
		cJSON *cjRoot = cJSON_CreateObject();

		//once
		cJSON *cjOnce = GET_SYS_GetOnce();
		if(cjOnce != NULL)
		{
			cJSON_AddItemToObject(cjRoot, "once",cjOnce);
		}

		//time
		cJSON *cjTime = GET_SYS_GetTime();
		if(cjTime != NULL)
		{
			cJSON_AddItemToObject(cjRoot, "time",cjTime);
		}

		//mem
		cJSON *cjMem = GET_SYS_GetMem();
		if(cjMem != NULL)
		{
			cJSON_AddItemToObject(cjRoot, "mem",cjMem);
		}

		//cpu
		cJSON *cjCpu = GET_SYS_GetCpu();
		if(cjCpu != NULL)
		{
			cJSON_AddItemToObject(cjRoot, "cpu",cjCpu);
		}

		//net
		cJSON *cjNet = GET_SYS_GetNet();
		if(cjCpu != NULL)
		{
			cJSON_AddItemToObject(cjRoot, "net",cjNet);
		}

		char *pcJson = cJSON_PrintUnformatted(cjRoot);
		/** char *pcJson = cJSON_Print(cjRoot); */

		printf("%s\n",pcJson);
		/** send(g_fd,pcJson,strlen(pcJson),0); */

		cJSON_Delete(cjRoot);
		u16Time++;
		if( u16ExecTime != 0 && u16Time >= u16ExecTime)break;
		sleep(u16SleepTime);

	} while(1);

	return 0;
}
