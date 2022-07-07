#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/Bald_Parse_Opt.h"
#include "../inc/Bald_Get_Sys_Info.h"




/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int16_t BALD_PARSE_OPT_Cpu(cJSON *cjOpt,uint8_t *au8Show_1,char **pau8Show_2,uint32_t u32CpuNum,uint8_t *au8Show_3)
{
	uint8_t u8Opt_1 = 0 , u8Opt_2 = 0 , u8Opt_3 = 0;
	bool bShowOrNot_1 = false;
	//json出错
	if( cjOpt == NULL || (cjOpt->type != cJSON_Array && cjOpt->type != cJSON_String) )
	{
		return -1;
	}
	//打印全部  {"cpu":"all"}
	else if (cjOpt->type == cJSON_String && strcmp(cjOpt->valuestring,"all") == 0)
	{
		for(uint32_t i = 0 ; i < BALD_OPT_CPU_1 ;i ++)
		{
			au8Show_1[i] = true;
		}
		for(uint32_t i = 0 ; i < BALD_OPT_CPU_3 ;i ++)
		{
			au8Show_3[i] = true;
		}
		return 0;
	}

	//json 为数组 {"cpu":[{"s":["wa","id",{"jiffies":[]}]}]}
	//获取第一层cpu json数组的数组长度
	uint32_t u32SLen = cJSON_GetArraySize(cjOpt);
	for ( uint32_t i = 0 ;i < u32SLen ; i++)
	{
		//获取cpu json数组的每一个item
		cJSON *cjCpuArrayItem = cJSON_GetArrayItem(cjOpt, i);
		//printf("%d :",i);

		if(cjCpuArrayItem != NULL && cjCpuArrayItem->type == cJSON_Object )
		{
			cJSON *cjTempObj;	

							/**************************cpu 数组 指定展示******************/


			//指定展示
			if(( cjTempObj = cJSON_GetObjectItem(cjCpuArrayItem, "s") )!= NULL && cjTempObj->type == cJSON_Array)
			{
				//printf("指定 ");	

				//将全部都改为0
				for ( int i = 0 ; i < BALD_OPT_CPU_1 ; i ++ )
				{
					au8Show_1[i] = false;
				}

				//获取指定的数量
				uint32_t u32SLen = cJSON_GetArraySize(cjTempObj);

				for ( int i = 0 ; i < u32SLen ; i++)
				{
					cJSON *cjSItem = cJSON_GetArrayItem(cjTempObj, i);	
					//printf(" [%d]:",i);
					//占用率
					if(cjSItem->type == cJSON_String)
					{
						//user
						if(strcmp("us",cjSItem->valuestring) == 0)
						{
							au8Show_1[0] = true;
							//printf("us ");
						}
						//sys
						if(strcmp("sys",cjSItem->valuestring) == 0)
						{
							au8Show_1[1] = true;
							//printf("sys ");
						}
						//nice
						if(strcmp("ni",cjSItem->valuestring) == 0)
						{
							au8Show_1[2] = true;
							//printf("ni ");
						}
						//idle
						if(strcmp("id",cjSItem->valuestring) == 0)
						{
							au8Show_1[3] = true;
							//printf("id ");
						}
						//iowait
						if(strcmp("wa",cjSItem->valuestring) == 0)
						{
							au8Show_1[4] = true;
							//printf("wa ");
						}

					}

									/********************jiffies obj***********************/

					//{"cpu":[{"s":[{"jiffies":"all"}]}]}
					//{"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
					else if( cjSItem->type == cJSON_Object )
					{
						cJSON *cjJiffiesObj = cJSON_GetObjectItem( cjSItem, "jiffies");

						//全部置一全部输出 
						//{"cpu":[{"s":[{"jiffies":"all"}]}]}
						
						if(cjJiffiesObj != NULL && cjJiffiesObj->type == cJSON_String && strcmp(cjJiffiesObj->valuestring,"all") == 0)
						{
							//printf("jiffies all");
							au8Show_1[5] = true;
							for ( uint32_t i = 0 ; i < BALD_OPT_CPU_3;i++)
							{
								au8Show_3[i] = true;
							}
						}


						//jiffies 是json数组
						//1 {"cpu":[{"s":[{"jiffies":[{"name":"all"}]}]}]}
						//2 {"cpu":[{"s":[{"jiffies":[{"attr":"all"}]}]}]}
						//3 {"cpu":[{"s":[{"jiffies":[{"name":"all"},{"attr":"all"}]}]}]}
						//4 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}

						else if ( cjJiffiesObj != NULL && cjJiffiesObj->type == cJSON_Array && strcmp(cjJiffiesObj->string,"jiffies") == 0 )
						{
							au8Show_1[5] = true;
							//循环获取jiffies内部参数
							
									/********************jiffies 内部item***********************/

							uint32_t u32JiffiesLen = cJSON_GetArraySize(cjJiffiesObj);
							for ( int i = 0 ;i < u32JiffiesLen ; i ++ )
							{
								//获取jiffies数组的每个item
								cJSON *cjJiffiesItem = cJSON_GetArrayItem(cjJiffiesObj,i);
							

								//item 是json obj
								if(cjJiffiesItem != NULL && cjJiffiesItem->type == cJSON_Object)
								{	
									//获取 名称item 或 属性item
									cJSON *cjNameOrAttr;

										/********************jiffies 内部cpu名称***********************/

									//1 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
									//2 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
									//3 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
									//4 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
									
									if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "name")) != NULL)
									{
										cJSON *cjNameArray;

										//item 是json string
										//1 {"cpu":[{"s":[{"jiffies":[{"name":"all"}]}]}]}
										//2 {"cpu":[{"s":[{"jiffies":[{"attr":"all"}]}]}]}
										//3 {"cpu":[{"s":[{"jiffies":[{"name":"all"},{"attr":"all"}]}]}]}
										if(cjNameOrAttr->type == cJSON_String && strcmp(cjNameOrAttr->valuestring,"all") == 0)
										{
										}

										//1 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
										//2 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
										//指定名称输出
										if((cjNameArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL && cjNameArray->type == cJSON_Array)
										{

											//先将所有名称清空
											for( uint32_t i = 0 ; i < u32CpuNum ; i++ )
											{
												bzero(pau8Show_2[i],10);
											}

											//写入指定名称的偏移量
											uint32_t j = 0;

											//printf("指定名称:");
											//获取名称长度
											uint32_t u32NameLen = cJSON_GetArraySize(cjNameArray);
											//循环获取名称
											for ( uint32_t i = 0 ; i < u32NameLen ; i ++ )
											{
												//名称item
												cJSON *cjCpuName = cJSON_GetArrayItem(cjNameArray, i);
												//获取名称
												if(cjCpuName != NULL && cjCpuName->type == cJSON_String)
												{
													//printf(" %s",cjCpuName->valuestring);
													//将需要输出的名称放入指针数组中
													memcpy(pau8Show_2[j],cjCpuName->valuestring,10);
													j++;
												}

											}
											//printf(" ");
										}

										//3 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
										//4 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
										//除外名称输出
										else if ( (cjNameArray = cJSON_GetObjectItem(cjNameOrAttr, "e")) != NULL)
										{
											//printf("除外名称:");
											//获取名称长度
											uint32_t u32NameLen = cJSON_GetArraySize(cjNameArray);
											//循环获取名称
											for ( uint32_t i = 0 ; i < u32NameLen ; i ++ )
											{
												//名称item
												cJSON *cjCpuName = cJSON_GetArrayItem(cjNameArray, i);
												//获取名称
												if(cjCpuName != NULL && cjCpuName->type == cJSON_String)
												{
													//printf(" %s",cjCpuName->valuestring);
													for(uint32_t i = 0 ; i < u32CpuNum ; i++)
													{
														if(strcmp(cjCpuName->valuestring,pau8Show_2[i]) == 0)
														{
															bzero(pau8Show_2[i],10);
															break;
														}
													}
												}

											}
											//printf(" ");
										}

									}

										/********************jiffies 内部cpu属性***********************/

									else if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "attr")) != NULL)
									{
										cJSON *cjAttrArray;
										//item 是json string
										//1 {"cpu":[{"s":[{"jiffies":[{"name":"all"}]}]}]}
										//2 {"cpu":[{"s":[{"jiffies":[{"attr":"all"}]}]}]}
										//3 {"cpu":[{"s":[{"jiffies":[{"name":"all"},{"attr":"all"}]}]}]}
										if(cjNameOrAttr->type == cJSON_String && strcmp(cjNameOrAttr->valuestring,"all") == 0)
										{
											for(int i = 0 ; i < BALD_OPT_CPU_3 ; i ++)
											{
												au8Show_3[i] = true;
											}
										}
										//指定属性输出
										//1 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
										//3 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}]}]}
										else if((cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL && cjAttrArray->type == cJSON_Array)
										{
											//清零
											bzero(au8Show_3,sizeof(uint8_t)*BALD_OPT_CPU_3);

											//printf("指定属性:");
											//获取属性
											//获取属性长度
											uint32_t u32AttrLen = cJSON_GetArraySize(cjAttrArray);
											//循环获取属性
											for ( uint32_t i = 0 ; i < u32AttrLen ; i ++ )
											{
												//获取属性
												cJSON *cjCpuAttr = cJSON_GetArrayItem(cjAttrArray, i);
												if(cjCpuAttr != NULL && cjCpuAttr->type == cJSON_String)
												{
													//printf(" %s",cjCpuAttr->valuestring);
													if ( strcmp(cjCpuAttr->valuestring,"name") == 0 )
													{
														au8Show_3[0] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"user") == 0 )
													{
														au8Show_3[1] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"nice") == 0 )
													{
														au8Show_3[2] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"sys") == 0 )
													{
														au8Show_3[3] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"idle") == 0 )
													{
														au8Show_3[4] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"iowait") == 0 )
													{
														au8Show_3[5] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"irq") == 0 )
													{
														au8Show_3[6] = true;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"softirq") == 0 )
													{
														au8Show_3[7] = true;
													}
												}

											}
											//printf(" ");
											
										}

										//除外属性输出
										//4 {"cpu":[{"s":[{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
										//2 {"cpu":[{"s":[{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}]}]}
										
										else if ( (cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "e")) != NULL)
										{
											//printf("除外属性:");
											//全部置一
											for (int i = 0 ; i < BALD_OPT_CPU_3 ; i ++)
											{
												au8Show_3[i] = true;
											}
											//获取属性
											//获取属性长度
											uint32_t u32AttrLen = cJSON_GetArraySize(cjAttrArray);
											//循环获取属性
											for ( uint32_t i = 0 ; i < u32AttrLen ; i ++ )
											{
												//获取属性
												cJSON *cjCpuAttr = cJSON_GetArrayItem(cjAttrArray, i);
												if(cjCpuAttr != NULL && cjCpuAttr->type == cJSON_String)
												{
													//printf(" %s",cjCpuAttr->valuestring);
													if ( strcmp(cjCpuAttr->valuestring,"name") == 0 )
													{
														au8Show_3[0] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"user") == 0 )
													{
														au8Show_3[1] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"nice") == 0 )
													{
														au8Show_3[2] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"sys") == 0 )
													{
														au8Show_3[3] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"idle") == 0 )
													{
														au8Show_3[4] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"iowait") == 0 )
													{
														au8Show_3[5] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"irq") == 0 )
													{
														au8Show_3[6] = false;
													}
													else if ( strcmp(cjCpuAttr->valuestring,"softirq") == 0 )
													{
														au8Show_3[7] = false;
													}
												}

											}
											//printf(" ");
										}// if ( (cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL)
									}// if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "attr")) != NULL)
								}// if(cjJiffiesItem != NULL && cjJiffiesItem->type == cJSON_Object)
							}// for ( int i = 0 ;i < u32JiffiesLen ; i ++ )
						}// if ( cjJiffiesObj != NULL && cjJiffiesObj->type == cJSON_Array && strcmp(cjJiffiesObj->string,"jiffies") == 0 )
					}// if( cjSItem->type == cJSON_Object )
				}// for ( int i = 0 ; i < u32SLen ; i++)
			}// if(( cjTempObj = cJSON_GetObjectItem(cjCpuArrayItem, "s") )!= NULL && cjTempObj->type == cJSON_Array)
			


							/**************************cpu 数组 除外输出 cpu使用率******************/



			//{"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name","sys","user","nice"]}}]}]}
			else if(( cjTempObj = cJSON_GetObjectItem(cjCpuArrayItem,"e") )!= NULL && cjTempObj->type == cJSON_Array)
			{
				//除外展示
				//printf("除外");	
				//全部置一 
				for ( int i = 0 ; i < BALD_OPT_CPU_1 ; i ++ )
				{
					au8Show_1[i] = true;	
				}

				//获取除外的数组的长度

				uint32_t u32ELen = cJSON_GetArraySize(cjTempObj);
				for ( uint32_t i = 0 ; i < u32ELen; i++ )
				{
					//获取每一个item
					cJSON *cjEItem = cJSON_GetArrayItem(cjTempObj, i);
					if (cjEItem != NULL && cjEItem->type == cJSON_String)
					{
						//user
						if(strcmp("us",cjEItem->valuestring) == 0)
						{
							au8Show_1[0] = false;
							//printf("us ");
						}
						//sys
						if(strcmp("sys",cjEItem->valuestring) == 0)
						{
							au8Show_1[1] = false;
							//printf("sys ");
						}
						//nice
						if(strcmp("ni",cjEItem->valuestring) == 0)
						{
							au8Show_1[2] = false;
							//printf("ni ");
						}
						//idle
						if(strcmp("id",cjEItem->valuestring) == 0)
						{
							au8Show_1[3] = false;
							//printf("id ");
						}
						//iowait
						if(strcmp("wa",cjEItem->valuestring) == 0)
						{
							au8Show_1[4] = false;
							//printf("wa ");
						}
						//iowait
						if(strcmp("jiffies",cjEItem->valuestring) == 0)
						{
							au8Show_1[5] = false;
						}

					}
	
				}
			}

			


							/**************************cpu 除外展示时 jiffies******************/



			//{"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name","sys","user","nice"]}}]}]}
			else if(( cjTempObj = cJSON_GetObjectItem(cjCpuArrayItem,"jiffies") )!= NULL)
			{
				//jiffies
				//printf("jiffies");	
				cJSON *cjJiffiesObj = cjTempObj;

				//{"cpu":[{"e":["wa","id"]},{"jiffies":"all"}]}
				if(cjJiffiesObj != NULL && cjJiffiesObj->type == cJSON_String && strcmp(cjJiffiesObj->valuestring,"all") == 0)
				{
					//printf("jiffies all");
					au8Show_1[5] = true;
					for ( uint32_t i = 0 ; i < BALD_OPT_CPU_3;i++)
					{
						au8Show_3[i] = true;
					}
				}


				//jiffies 是json数组
				//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"}]}]}
				//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"attr":"all"}]}]}
				//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"},{"attr":"all"}]}]}
				//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}}]}

				else if ( cjJiffiesObj != NULL && cjJiffiesObj->type == cJSON_Array && strcmp(cjJiffiesObj->string,"jiffies") == 0 )
				{
					au8Show_1[5] = true;
					//循环获取jiffies内部参数
					
							/********************cup除外jiffies时 jiffies obj 内部item***********************/

					uint32_t u32JiffiesLen = cJSON_GetArraySize(cjJiffiesObj);
					for ( int i = 0 ;i < u32JiffiesLen ; i ++ )
					{
						//获取jiffies数组的每个item
						cJSON *cjJiffiesItem = cJSON_GetArrayItem(cjJiffiesObj,i);
						//item 是json obj
						
						if(cjJiffiesItem != NULL && cjJiffiesItem->type == cJSON_Object)
						{	
							//获取 名称item 或 属性item
							cJSON *cjNameOrAttr;

								/********************cpu除外jiffies时 jiffies obj 内部cpu名称***********************/

							
							//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"}]}]}
							//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"attr":"all"}]}]}
							//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"},{"attr":"all"}]}]}
							//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}}]}

							if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "name")) != NULL)
							{
								cJSON *cjNameArray;

								//name 为 string 且为all
								//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"}]}]}
								//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"attr":"all"}]}]}
								//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":"all"},{"attr":"all"}]}]}
								if(cjNameOrAttr->type == cJSON_String && strcmp(cjNameOrAttr->valuestring,"all") == 0)
								{
									
								}


								//name 为数组 

								//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
								//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
								//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
								//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
							
								//指定名称输出
								else if((cjNameArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL && cjNameArray->type == cJSON_Array)
								{
									//先将所有名称清空
									for( uint32_t i = 0 ; i < u32CpuNum ; i++ )
									{
										bzero(pau8Show_2[i],10);
									}

									//写入指定名称的偏移量
									uint32_t j = 0;

									//printf("指定名称:");
									//获取名称长度
									uint32_t u32NameLen = cJSON_GetArraySize(cjNameArray);
									//循环获取名称
									for ( uint32_t i = 0 ; i < u32NameLen ; i ++ )
									{
										//名称item
										cJSON *cjCpuName = cJSON_GetArrayItem(cjNameArray, i);
										//获取名称
										if(cjCpuName != NULL && cjCpuName->type == cJSON_String)
										{
											//printf(" %s",cjCpuName->valuestring);
											//将需要输出的名称放入指针数组中
											memcpy(pau8Show_2[j],cjCpuName->valuestring,10);
											j++;
										}

									}
									//printf(" ");
								}

								//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
								//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
								//除外名称输出
								else if ( (cjNameArray = cJSON_GetObjectItem(cjNameOrAttr, "e")) != NULL)
								{
									//printf("除外名称:");
									//获取名称长度
									uint32_t u32NameLen = cJSON_GetArraySize(cjNameArray);
									//循环获取名称
									for ( uint32_t i = 0 ; i < u32NameLen ; i ++ )
									{
										//名称item
										cJSON *cjCpuName = cJSON_GetArrayItem(cjNameArray, i);
										//获取名称
										if(cjCpuName != NULL && cjCpuName->type == cJSON_String)
										{
											//printf(" %s",cjCpuName->valuestring);
											for(uint32_t i = 0 ; i < u32CpuNum ; i++)
											{
												if(strcmp(cjCpuName->valuestring,pau8Show_2[i]) == 0)
												{
													bzero(pau8Show_2[i],10);
													break;
												}
											}
										}

									}
									//printf(" ");
								}

							}

								/********************cpu除外jiffies时 jiffies obj 内部cpu属性***********************/

							//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
							//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
							//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
							//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
						
							else if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "attr")) != NULL)
							{
								cJSON *cjAttrArray;

								if(cjNameOrAttr->type == cJSON_String && strcmp(cjNameOrAttr->valuestring,"all") == 0)
								{
									for( int i = 0 ;i < BALD_OPT_CPU_3 ; i ++ )
									{
										au8Show_3[i] = true;
									}
								}

								//指定属性输出
								//1 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
								//3 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"s":["name"]}}]}}]}
								
								if((cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL && cjAttrArray->type == cJSON_Array)
								{
									//清零
									bzero(au8Show_3,sizeof(uint8_t)*BALD_OPT_CPU_3);

									//printf("指定属性:");
									//获取属性
									//获取属性长度
									uint32_t u32AttrLen = cJSON_GetArraySize(cjAttrArray);
									//循环获取属性
									for ( uint32_t i = 0 ; i < u32AttrLen ; i ++ )
									{
										//获取属性
										cJSON *cjCpuAttr = cJSON_GetArrayItem(cjAttrArray, i);
										if(cjCpuAttr != NULL && cjCpuAttr->type == cJSON_String)
										{
											//printf(" %s",cjCpuAttr->valuestring);
											if ( strcmp(cjCpuAttr->valuestring,"name") == 0 )
											{
												au8Show_3[0] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"user") == 0 )
											{
												au8Show_3[1] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"nice") == 0 )
											{
												au8Show_3[2] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"sys") == 0 )
											{
												au8Show_3[3] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"idle") == 0 )
											{
												au8Show_3[4] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"iowait") == 0 )
											{
												au8Show_3[5] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"irq") == 0 )
											{
												au8Show_3[6] = true;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"softirq") == 0 )
											{
												au8Show_3[7] = true;
											}
										}

									}
									//printf(" ");
									
								}

								//除外属性输出
								//2 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"s":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
								//4 {"cpu":[{"e":["wa","id"]},{"jiffies":[{"name":{"e":["cpu"]}},{"attr":{"e":["name"]}}]}}]}
								
								else if ( (cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "e")) != NULL)
								{
									//printf("除外属性:");
									//全部置一
									for (int i = 0 ; i < BALD_OPT_CPU_3 ; i ++)
									{
										au8Show_3[i] = true;
									}
									//获取属性
									//获取属性长度
									uint32_t u32AttrLen = cJSON_GetArraySize(cjAttrArray);
									//循环获取属性
									for ( uint32_t i = 0 ; i < u32AttrLen ; i ++ )
									{
										//获取属性
										cJSON *cjCpuAttr = cJSON_GetArrayItem(cjAttrArray, i);
										if(cjCpuAttr != NULL && cjCpuAttr->type == cJSON_String)
										{
											//printf(" %s",cjCpuAttr->valuestring);
											if ( strcmp(cjCpuAttr->valuestring,"name") == 0 )
											{
												au8Show_3[0] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"user") == 0 )
											{
												au8Show_3[1] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"nice") == 0 )
											{
												au8Show_3[2] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"sys") == 0 )
											{
												au8Show_3[3] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"idle") == 0 )
											{
												au8Show_3[4] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"iowait") == 0 )
											{
												au8Show_3[5] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"irq") == 0 )
											{
												au8Show_3[6] = false;
											}
											else if ( strcmp(cjCpuAttr->valuestring,"softirq") == 0 )
											{
												au8Show_3[7] = false;
											}
										}

									}
									//printf(" ");
								}// if ( (cjAttrArray = cJSON_GetObjectItem(cjNameOrAttr, "s")) != NULL)
							}// if((cjNameOrAttr = cJSON_GetObjectItem(cjJiffiesItem, "attr")) != NULL)
						}// if(cjJiffiesItem != NULL && cjJiffiesItem->type == cJSON_Object)
					}// for ( int i = 0 ;i < u32JiffiesLen ; i ++ )
				}
			}
		}	
		//printf("\n");
	}
	
	return 0;
}














