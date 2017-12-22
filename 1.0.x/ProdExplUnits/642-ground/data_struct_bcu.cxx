/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：data_struct.c
 * 文件标识：
 * 摘    要：控制信息结构体
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "data_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bcu_function.h"


//网络发送数据包
volatile network_send_package_t network_send_package =
{
		"",
		0,
		0,
		{0}
};


int GetDeviceType(char *devices_name,unsigned char event_type)
{
	int ret = 0;
	if( strcmp(devices_name,"BCU") == 0)
	{
		ret = TYPE_OF_BCU;
	}

	if(strcmp(devices_name,"CCU") == 0)
	{
		ret = TYPE_OF_CCU;
	}

	if(strcmp(devices_name,"EMAP") == 0)
	{
		ret = TYPE_OF_EAMP;
	}

	if(strcmp(devices_name,"PCU") == 0&&event_type==8)
	{
		ret = TYPE_OF_PECU;
	}
	return ret;
}
