#ifndef __BALD_SERVER_H__
#define __BALD_SERVER_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define IP_SIZE 16

typedef struct client_info_st
{
	int fd;
	char aAddr[IP_SIZE];

}CLINET_INFO_ST;












#endif
