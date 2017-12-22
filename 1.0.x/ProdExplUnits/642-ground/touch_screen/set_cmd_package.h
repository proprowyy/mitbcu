/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： set_cmd_package.h
 * 文件标识：
 * 摘    要： 触摸屏设置cmd命令包处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef TSSetCmd
#define TSSetCmd

#include "ts_head_file.h"

//控制信息（发送结构体）初始化
void BCUTSCmdInit(void);

//控制信息（发送结构体）填充
//入口参数
//[0]事件类型
//[1]源设备名称   [2]源设备号
//[3]状态请求结束   [4]开始或结束
//[5]线路号  [6]区间号
//[7]文件序列号   [8]播报次数
//[9]是否到站    [10]站代码      [11]站有效位 [12]是否预读
//[13]对讲状态下的目标设备号
void BCUSetTSCmd(unsigned char event_type,
					char *src_devices_name,	unsigned char src_devices_no,
					unsigned char state_request,unsigned char request_or_over,
					unsigned char line_number,unsigned char region_number,
					unsigned int broadcast_sequence,unsigned char broadcast_times,
					unsigned char arrived_station,unsigned char current_station_code,unsigned char station_effective,unsigned char whether_pre_read,
					unsigned char dst_device_number);

//触摸屏填充控制信息
//入口参数
//[0]事件类型
//[1]源设备名称   [2]源设备号
//[3]状态请求结束   [4]开始或结束
//[5]线路号  [6]区间号
//[7]文件序列号   [8]播报次数
//[9]是否到站    [10]站代码      [11]站有效位 [12]是否预读
//[13]对讲状态下的目标设备号
void BcuScreenSetCmd(unsigned char event_type,
						char *src_devices_name,	unsigned char src_devices_no,
						unsigned char state_request,unsigned char request_or_over,
						unsigned char line_number,unsigned char region_number,
						unsigned int broadcast_sequence,unsigned char broadcast_times,
						unsigned char arrived_station,unsigned char current_station_code,
						unsigned char station_effective,unsigned char whether_pre_read,
						unsigned char dst_device_number,int car_no);
//将触摸屏使用的将结构转换成网络命令
int TouchScreenSetCmd(void *param_table );

#endif
