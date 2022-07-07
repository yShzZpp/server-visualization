/************************************************************************
 * * @Author  :YaoShiHong
 * * @Company :Bald
 * * @Date    :2022/07/5 
 * * @file    :Bald_Parse_Opt.h
 * ************************************************************************/
#ifndef __BALD_PARSE_OPT_H_
#define __BALD_PARSE_OPT_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "Bald_cJSON.h"

#define BALD_OPT_CPU_1 6
#define BALD_OPT_CPU_2 10
#define BALD_OPT_CPU_3 8




int16_t BALD_PARSE_OPT_Cpu(cJSON *cjOpt,uint8_t *au8Show_1,char **pau8Show_2,uint32_t u32CpuNum,uint8_t *au8Show_3);



#endif
