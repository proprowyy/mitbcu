/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： bcu_config.c
 * 文件标识：
 * 摘    要：The source file of current bcu configure
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "bcu_config.h"
#include "bcu_function.h"
#include "network/zhw_dev_config_info.h"
extern bcu_state_t  bcu_state;

/*get current device name id and car number */
int GetCurrentDeviceInfomation(device_id_info_t *this_device_id_info)
{


	strcpy((char *)this_device_id_info->device_name,"BCU");
	this_device_id_info->device_no = ReadNum();
	this_device_id_info->dev_type = ReadType();
	diag_printf("GetCurrentDeviceInfomation:%s,%d,%d\n",this_device_id_info->device_name,this_device_id_info->device_no,this_device_id_info->dev_type);

	return 0;
}

/*Acquire other device number*/
void GetOtherDeviceNo()
{
	if(bcu_state.bcu_info.devices_no == 1)
	{
		bcu_state.bcu_info.devices_no=1;
		bcu_state.opposite_bcu_no = 2;
	}
	else
	{
		bcu_state.bcu_info.devices_no=2;
		bcu_state.opposite_bcu_no = 1;
	}
}

/*Update current device information*/
void UpdateCurrentDeviceInformation()
{
	strcpy(bcu_state.bcu_info.devices_name,"BCU");
	bcu_state.dev_type=ReadType();
	bcu_state.bcu_info.devices_no =ReadNum();
}



