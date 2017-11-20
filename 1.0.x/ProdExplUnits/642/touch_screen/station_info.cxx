/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： station_info.cxx
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
#include  "station_info.h"
//站台信息

region_station_t *p_line3_region_info;//3号线区间指针
region_station_t *p_line4_region_info;//4号线区间指针

select_region_t  bcu_station_select_info = {0,0};//BCU选择的区间信息

