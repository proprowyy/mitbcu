/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_read_from_db_test.c
*文件标识：读取配置文件
*摘  要：设备上电之后从I/O口获得设备的相关参数，根据参数读取配置表中的相关设置，获取IP地址。
*接受参数：无
*返回数据：将获得的IP地址返回给用户。
*
*当前版本：00.00.01
*作  者：周魏
*完成日期：
*
*取代版本：原版本号
*原作者 ：
*完成时间：
*/


#include <stdio.h>
#include <string.h>
#include "bcu_get_data_from_ccu_type.h"
#include "zhw_read_from_db_test.h"
//整车设备的设备名、设备号、及IP地址表
ip_mac_table_item_t  device_info[]=
{
#if 0
		{"EAMP",7,1,"192.168.10.120",{ 0x0,0x90,0x27,0x8c,0x57,0x20},0},
		{"EAMP",7,2,"192.168.10.121",{ 0x0,0x90,0x27,0x8c,0x57,0x21},0},
		{"EAMP",7,3,"192.168.10.122",{ 0x0,0x90,0x27,0x8c,0x57,0x22},0},
		{"EAMP",7,4,"192.168.10.123",{ 0x0,0x90,0x27,0x8c,0x57,0x23},0},
		{"EAMP",7,5,"192.168.10.124",{ 0x0,0x90,0x27,0x8c,0x57,0x24},0},
		{"EAMP",7,6,"192.168.10.125",{ 0x0,0x90,0x27,0x8c,0x57,0x25},0},
		{"EAMP",7,200,"192.168.10.207","",0},
		{"PCU",1,1,"192.168.10.140",{ 0x0,0x90,0x27,0x8c,0x57,0x40},0},
		{"PCU",1,2,"192.168.10.141",{ 0x0,0x90,0x27,0x8c,0x57,0x41},0},
		{"PCU",1,3,"192.168.10.142",{ 0x0,0x90,0x27,0x8c,0x57,0x42},0},
		{"PCU",1,4,"192.168.10.143",{ 0x0,0x90,0x27,0x8c,0x57,0x43},0},
		{"PCU",1,5,"192.168.10.144",{ 0x0,0x90,0x27,0x8c,0x57,0x44},0},
		{"PCU",1,6,"192.168.10.145",{ 0x0,0x90,0x27,0x8c,0x57,0x45},0},
		{"PCU",1,7,"192.168.10.146",{ 0x0,0x90,0x27,0x8c,0x57,0x46},0},
		{"PCU",1,8,"192.168.10.147",{ 0x0,0x90,0x27,0x8c,0x57,0x47},0},
		{"PCU",1,9,"192.168.10.148",{ 0x0,0x90,0x27,0x8c,0x57,0x48},0},
		{"PCU",1,10,"192.168.10.149",{ 0x0,0x90,0x27,0x8c,0x57,0x49},0},
		{"PCU",1,11,"192.168.10.150",{ 0x0,0x90,0x27,0x8c,0x57,0x50},0},
		{"PCU",1,12,"192.168.10.151",{ 0x0,0x90,0x27,0x8c,0x57,0x51},0},
		{"PCU",1,200,"192.168.10.201","",0},
#endif
#if 1
		{"BCU",3,1,"11.15.100.110",{ 0x0,0x90,0x27,0x8c,0x57,0x10},0},
		{"BCU",3,2,"11.15.100.111",{ 0x0,0x90,0x27,0x8c,0x57,0x11},0},
		{"BCU",3,3,"11.15.100.112",{ 0x0,0x90,0x27,0x8c,0x57,0x12},0},
		{"BCU",3,4,"11.15.100.113",{ 0x0,0x90,0x27,0x8c,0x57,0x13},0},
		{"BCU",3,5,"11.15.100.114",{ 0x0,0x90,0x27,0x8c,0x57,0x14},0},
#else

		{"BCU",3,1,"192.168.10.110",{ 0x0,0x90,0x27,0x8c,0x57,0x10},0},
		{"BCU",3,2,"192.168.10.111",{ 0x0,0x90,0x27,0x8c,0x57,0x11},0},

#endif
		{"COMMON",8,230,"230.10.10.86","",0},
		{"BCU",3,230,"230.10.10.55","",0},
		{"OCS",7,230,"11.15.100.200","",0},
		{"OCS",7,231,"11.15.100.200","",0},

};
unsigned int device_info_items = sizeof(device_info) / sizeof(device_info[0]);

/*
 *   	@brief	本函数的功能是根据所给的设备名和设备号责解析出设备的IP地址；
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @return   device_ip  相应设备的IP地址
 */
int ZhwReadFromTestDB(int *row, int *column,void **point)
{

	int test_column = TEST_COLUMN;
	int test_row = 0;
	int *column_mark;
	int *row_mark;
    test_row = device_info_items;
	column_mark = &test_column;
	row_mark = &test_row;
	memcpy(column,column_mark,sizeof(column_mark));
	memcpy(row,row_mark,sizeof(row_mark));
	*point = device_info;
	return 1;
}
