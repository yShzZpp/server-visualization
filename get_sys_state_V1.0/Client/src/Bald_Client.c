#include "../inc/Bald_Client.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "../inc/Bald_cJSON.h"
#include "../inc/Bald_Get_Sys_Info.h"

int g_fd = 0;

int8_t GET_SYS_UserName(char *pcData);
int main(void)
{
	struct sockaddr_in stClientAddr;
	socklen_t u32ClientSize;
	g_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    stClientAddr.sin_family=AF_INET;
	stClientAddr.sin_port=htons(5555);
	inet_pton(AF_INET,"192.168.202.193", &stClientAddr.sin_addr.s_addr);
	GET_SYS_CpuInit();

	/** int ret = connect(g_fd,(struct sockaddr*) &stClientAddr,sizeof(stClientAddr)); */

	while(1)
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

		char *pcJson = cJSON_PrintUnformatted(cjRoot);
		/** char *pcJson = cJSON_Print(cjRoot); */

		printf("%s\n",pcJson);
		/** send(g_fd,pcJson,strlen(pcJson),0); */

		cJSON_Delete(cjRoot);
		sleep(1);
	}
	return 0;
}
