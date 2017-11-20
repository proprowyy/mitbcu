/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_network_main.h
 * 文件标识：
 * 摘    要： 网络主程序头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2012-09-17
 *
 * 取代版本： v0.0.0
 * 原作者  ： 周魏
 * 完成日期：2012-08-22
 */
#ifndef ZHW_NETWORK_MAIN_H_
#define ZHW_NETWORK_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <unistd.h> // sleep()
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <cyg/infra/testcase.h>
#include <cyg/hal/hal_io.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/plf_io.h>

//#include "pcu_main.h"

#include "data_struct.h"
#include "udp_send_fun.h"
#include "udp_recv_fun.h"
#include "udp_type.h"

#include "zhw_use_ioctl_to_set_net.h"
#include "zhw_network_config.h"
#include "zhw_socket_init.h"
#include "zhw_set_into_mul.h"
#include "zhw_sockfd_server_init.h"
#include "zhw_dev_config_info.h"
#include "zhw_middle_forward.h"
#include "zhw_mount_romfs.h"
#include "zhw_shell_function.h"
#ifndef TEST_AUDIO_DATA
//#define TEST_AUDIO_DATA
#endif
//12.17 增加IP地址解析
#define EAMPNUMBER 6
#define BCUNUMBER 2
#define PCUNUMBER 12
#define CCUNUMBER 2

//test 6.3--zhw


#ifndef NETWORKTEST
#define NETWORKTEST

#if(BCU_DEV_NO==1)
#define UDP_CMD_PORT 30001
#define UDP_DATA_PORT 30011
#define UDP_COMMON_BIG_PORT 30021
#endif

#if(BCU_DEV_NO==2)
#define UDP_CMD_PORT 30002
#define UDP_DATA_PORT 30012
#define UDP_COMMON_BIG_PORT 30022
#endif

#if(BCU_DEV_NO==3)
#define UDP_CMD_PORT 30003
#define UDP_DATA_PORT 30013
#define UDP_COMMON_BIG_PORT 30023
#endif

#if(BCU_DEV_NO==4)
#define UDP_CMD_PORT 30004
#define UDP_DATA_PORT 30014
#define UDP_COMMON_BIG_PORT 30024
#endif

#if(BCU_DEV_NO==5)
#define UDP_CMD_PORT 30005
#define UDP_DATA_PORT 30015
#define UDP_COMMON_BIG_PORT 30025
#endif

#define UDP_BIG_PORT 30020
#define UDP_COMMON_PORT 30564
#define UDP_SHELL_PORT 30006
#define UDP_HEART_PORT 30567

#define MUL_IP_CCU_CMD "230.10.10.44"  ///< add, 0803
#define MUL_IP_BCU_CMD "230.10.10.55"
#define MUL_IP_EAMP_DATA "230.10.10.66"
#define MUL_IP_EAMP_CMD "230.10.10.77"
#define MUL_IP_COMMON "230.10.10.89"
#define MUL_IP_COMMON_PCU "230.10.10.88"
#define MUL_IP_SHELL "230.10.10.99"

#define MUL_IP_CC_DATA "230.10.10.56"
#define MUL_IP_PC_DATA "230.10.10.57"
#define MUL_IP_CMU_CMD "230.10.10.58"

///< add begin, sync group
#define UDP_SYNC_COMMON_BCU_PORT      30100
#define UDP_SYNC_COMMON_CCU_PORT      30101
#define UDP_SYNC_COMMON_EAMP_PORT   30102
#define UDP_SYNC_COMMON_PCU_PORT       30103
#define MUL_IP_SYNC_COMMON_BCU  "230.10.10.100"
#define MUL_IP_SYNC_COMMON_CCU  "230.10.10.101"
#define MUL_IP_SYNC_COMMON_EAMP  "230.10.10.102"
#define MUL_IP_SYNC_COMMON_PCU  "230.10.10.103"
///< add end, 2013
#if 1
#define LOCAL_DEV_NAME "eth0"
#define NETMASK_ADDRS "255.255.255.0"
#define BROADCAST_ADDRS "11.15.100.255"
#define GATEWAY_ADDRS "11.15.100.254"
#define SERVER_ADDRS "11.15.100.250"

#else
#define LOCAL_DEV_NAME "eth0"
#define NETMASK_ADDRS "255.255.255.0"
#define BROADCAST_ADDRS "192.168.10.255"
#define GATEWAY_ADDRS "192.168.10.254"
#define SERVER_ADDRS "192.168.10.250"

#endif
#define SHELL_CMD_NUMBER 10
#define SHELL_CMD_LONG 20

typedef struct
{
	int cmd;
	void (*cmd_function)(network_shell_package_t t);
}shell_package_t;

extern shell_package_t shellcmd[SHELL_CMD_NUMBER];

void NetWorkMain(network_buffer_t network_buffer);

void ForceBUfferData_wilson();
void ForceGetBufferData_wilson();
char * zhw_server_dev_info(struct global_info *temp1);
char *zhw_network_get_local_ip(void);
int zhw_dev_weather_server(int a);
struct global_info zhw_server_info(int a); ///< add, 0818
#endif

#endif /* ZHW_NETWORK_MAIN_H_ */
