#include "../inc/Bald_Server.h"
#include <stdint.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define MAX_BUFF_SIZE 1024
#define MAX_CONNECT_SIZE 10
#define PORT 5555

CLINET_INFO_ST g_stClientMap[MAX_CONNECT_SIZE];

void SERVER_Print_Client_Map(void)
{
	int16_t s16ClientNum = 0;
	printf("			+-----------------------------------+\n");
	printf("			|--num--|--fd--|---------ip---------|\n");
	for(int i = 0 ; i <MAX_CONNECT_SIZE ; i++)
	{
		if(g_stClientMap[i].fd != 0)
		{
			s16ClientNum++;
		printf("			|-----------------------------------|\n");
		printf("			|  %3d  |  %2d  |   %15s  |\n",s16ClientNum,g_stClientMap[i].fd,g_stClientMap[i].aAddr);
		}
	}
	printf("			+-----------------------------------+\n");
}


/******************************************************
 * ****** Function		:   SERVER_Find_Seat_By_Fd
 * ****** brief			:	通过fd查找位置
 * ****** param			:   NULL
 * ****** return		:   -1:未找到对应的fd 其他为找到的位置
 * *******************************************************/
int16_t SERVER_Find_Seat_By_Fd(int fd)
{
	int16_t i = 0;
	for ( i = 0 ; i < MAX_CONNECT_SIZE ; i++ )
	{
		if(fd == g_stClientMap[i].fd)
		{
			return i;
		}
	}
	return -1;
}

/******************************************************
 * ****** Function		:	SERVER_Find_An_Enpty_Seat 
 * ****** brief			:	查找客户表中的空位
 * ****** param			:   NULL
 * ****** return		:   -1:未找到空位 其他为找到的空位
 * *******************************************************/
int16_t SERVER_Find_An_Enpty_Seat(void)
{
	uint16_t i = 0;
	for( i = 0 ; i < MAX_CONNECT_SIZE ; i++ )
	{
		if(g_stClientMap[i].fd == 0)
		{
			return i;
		}
	}

	return -1;
}


/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int main(void)
{
	int ret;
	time_t u32Time;
	struct sockaddr_in stServerAddr,stClientAddr;
	char *buff = (char *)malloc(MAX_BUFF_SIZE);

	socklen_t u32ClientSize;

	bzero(buff,MAX_BUFF_SIZE);

	int listenFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	int s16Opt = 1;
	setsockopt(listenFd,SOL_SOCKET,SO_REUSEADDR,&s16Opt,sizeof(int));
    stServerAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    stServerAddr.sin_family=AF_INET;
	stServerAddr.sin_port=htons(PORT);

	if(bind(listenFd, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr))!=0)
	{
		perror("bind error");
		exit(-1);
    }     

	listen(listenFd,MAX_CONNECT_SIZE);

	//epoll结构体初始化
	int epfd = epoll_create(1);
	struct epoll_event stEvent,stEvents[MAX_CONNECT_SIZE];
	bzero(&stEvent,sizeof(stEvent));
	bzero(&stEvents,sizeof(stEvents));

	//epoll设置
	stEvent.events = EPOLLIN;
	stEvent.data.fd = listenFd;

	//写入监听的fd
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &stEvent);
	if (ret == -1)
	{
		perror("epoll ctl error");
		exit(-1);
	}

	uint16_t u16ClientNum = 0;

	SERVER_Print_Client_Map();

	while(1)
	{

		int16_t s16EventsNum = epoll_wait(epfd,stEvents,MAX_CONNECT_SIZE,-1);
		if(s16EventsNum == -1)
		{
			perror("epoll wait error");
			exit(-1);
		}

		for(int16_t i = 0 ; i < s16EventsNum ; i++)
		{
			//输入事件
			if(stEvents[i].events == EPOLLIN)
			{
				//连接事件
				if(stEvents[i].data.fd == listenFd)
				{
					int clientFd = accept(listenFd, (struct sockaddr *)&stClientAddr,&u32ClientSize);
					if (clientFd == -1)
					{
						perror("accept error");
						continue;
					}
				
					//未超出最大连接数
					if(u16ClientNum < MAX_CONNECT_SIZE)
					{
						u16ClientNum++;
						//写入客户表
						int16_t s16Seat = SERVER_Find_An_Enpty_Seat();	
						if(s16Seat >= 0 && s16Seat < MAX_CONNECT_SIZE)
						{
							g_stClientMap[s16Seat].fd = clientFd;
							strcpy(g_stClientMap[s16Seat].aAddr,inet_ntoa(stClientAddr.sin_addr));
							SERVER_Print_Client_Map();
						}

						//写入epoll的读事件中
						stEvent.events = EPOLLIN|EPOLLET;
						stEvent.data.fd = clientFd;
						ret = epoll_ctl(epfd,EPOLL_CTL_ADD,clientFd,&stEvent);
						if(ret == -1)
						{
							perror("新建客户失败");
							continue;
						}
					}
					//超出最大连接数
					else if(u16ClientNum == MAX_CONNECT_SIZE)
					{
						send(clientFd,"is full please wait\n",strlen("is full please wait\n"),0);
						close(clientFd);
					}
				}
				//可读事件
				else
				{
					int clientFd = stEvents[i].data.fd;
					ret = recv(clientFd,buff,MAX_BUFF_SIZE,0);
					//读取错误
					if( ret == -1 )
					{
						perror("recv error");
					}
					//断开连接
					else if(ret == 0)
					{
						u16ClientNum--;
						close(clientFd);
						//删除用户表数据
						int16_t s16Seat = SERVER_Find_Seat_By_Fd(clientFd);
						if(s16Seat >= 0 &&s16Seat < MAX_CONNECT_SIZE)
						{
							memset(g_stClientMap[s16Seat].aAddr,0,IP_SIZE);
							g_stClientMap[s16Seat].fd = 0;
							SERVER_Print_Client_Map();
						}
						continue;
					}

					//正确读取
					time(&u32Time);
					printf("\n========================================\n");
					printf("%s",ctime(&u32Time));
					printf("recv :%s\n",buff);
					printf("========================================\n");
					memset(buff,0,MAX_BUFF_SIZE);
				}
			}
			//可写事件
			else
			{

			}

		}



	}


	return 0;
}
