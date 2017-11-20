/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： station_info.h
 * 文件标识：
 * 摘    要： 站台信息处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef STATIONINFO_H
#define STATIONINFO_H
#include "struct_type.h"
#include "ts_type.h"

typedef struct
{
	char skip_flag;//越站标志位
	char name[STATION_NAME_LENGTH];//站名
	unsigned char cursor;//该站在运营区间中的位置
	unsigned char open_door_side[2];///< 开么侧 1:left 2:right
}station_info_t;


typedef struct
{
	unsigned char station_select_begin_station;//区间起点站
	unsigned char station_select_terminal_station;//区间终点站
}select_region_t;

extern station_info_t station[2][MAX_STATION_NUMBER];

extern select_region_t bcu_station_select_info;

extern region_station_t *p_line3_region_info;
extern region_station_t *p_line4_region_info;
#endif
