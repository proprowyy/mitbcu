/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_network_main.c
 * 文件标识：
 * 摘    要： 网络主函数
 *
 * 当前版本： V1.0.1
 * 作    者： zhouwei
 * 完成日期：
 *
 * 取代版本：v1.0.0
 * 原作者  ：zhouwei
 * 完成日期：2012-10-08
 *
 */
#include "zhw_network_main.h"
#include"zhw_shell_function.h"
#include"data_struct.h"
#define OCS_IP "11.15.100.200"
//缓冲指针
 buffer_pointer_t *p_buffer_pointer;
//×××××××××××××××××//


//自动升级接口结构体初始化//
shell_package_t shellcmd[SHELL_CMD_NUMBER] = {
		{ProtocolTestStart,ZhwProTS},
		{CommQualityTest,ZhwCqcModule},
		{SystemReboot,NetShellReboot},
		{DevFirmwareUpdate,DFUProc},
		{ZhwFunc4,ZhwBakFunc4},
		{ZhwFunc5,ZhwBakFunc5},
		{ZhwFunc6,ZhwBakFunc6},
		{ZhwFunc7,ZhwBakFunc7},
		{ZhwFunc8,ZhwBakFunc8},
		{ZhwFunc9,ZhwBakFunc9}
};
//××××××××××××××××××××××××//



void NetWorkMain(network_buffer_t network_buffer)
{
	net_debug_printf(("I am success entry network handle thread!\n"));

	/*记录从音频缓冲读取长度*/
	int read_data_long = 0;
	/********************/

	/*套接字*/
	int udp_cmd_socket;
	int udp_data_socket;
	int udp_common_socket;
	int udp_shell_socket;
	int udp_common_big_socket;
	int udp_heart_socket;
	/********************/

	//sockaddr_in 绑定本地地址结构//
	struct sockaddr_in udp_cmd_socket_addr;//用于提供创建udp命令sockfd的ip和端口
	struct sockaddr_in udp_data_socket_addr;//用于提供创建udp数据sockfd的ip和端口
	struct sockaddr_in udp_common_socket_addr;//用于提供创建udp普通命令sockfd的ip和端口
	struct sockaddr_in udp_shell_socket_addr;//用于提供创建udp shell命令sockfd的ip和端口
	struct sockaddr_in udp_common_big_socket_addr;
	struct sockaddr_in udp_heart_socket_addr;
	//×××××××××××××××××××××××//

	//客户端地址
	struct sockaddr_in udp_shell_client_addr;
	//×××××××××××××××××××××××//

	//表示套接字最大值//
	int max_socket = 0;
	//×××××××××××××××××××××××//

	// 客户端地址结构长度//
	socklen_t len = sizeof(udp_shell_client_addr);
	//×××××××××××××××××××××××//

	//数据据缓冲
	char data_buffer_send[DATA_SIZE];
	network_send_package_t network_send_cmd_buf;	//用于网络发送控制信息缓存
	common_package_t common_package_buffer;		//用于存放通用套接字的数据缓存
	common_big_package_t common_big_package_buffer;
	network_heart_package_t heart_package_buffer;
	char shell_recv_buffer[128];
	char shell_send_buffer[128];
	//×××××××××××××××××××××××//

	//初始化缓冲
	memset(&data_buffer_send,'\0',sizeof(data_buffer_send));
	memset(&network_send_cmd_buf,'\0',sizeof(network_send_cmd_buf));
	memset(&common_package_buffer,'\0',sizeof(common_package_buffer));
	memset(&common_big_package_buffer,'\0',sizeof(common_big_package_buffer));
	memset(&heart_package_buffer,'\0',sizeof(heart_package_buffer));
	memset(&shell_recv_buffer,'\0',sizeof(shell_recv_buffer));
	memset(&shell_send_buffer,'\0',sizeof(shell_send_buffer));
	//×××××××××××××××××××××××//

	//初始 p_buffer_pointer
	buffer_pointer_t CachePoint = CacheCreate();
	p_buffer_pointer = &CachePoint;
	//×××××××××××××××××××××××//

	//设置本地ip，mac地址
	struct global_info dev_info_local;
	char *set_ip=zhw_server_dev_info(&dev_info_local);
	//×××××××××××××××××××××××//

	//初始化struct sockaddr_in 结构
	memset(&udp_cmd_socket_addr, 0, sizeof(udp_cmd_socket_addr));
	udp_cmd_socket_addr = IpAndPortInit(set_ip,UDP_CMD_PORT);

	memset(&udp_data_socket_addr, 0, sizeof(udp_data_socket_addr));
	udp_data_socket_addr = IpAndPortInit(set_ip,UDP_DATA_PORT);

	memset(&udp_common_socket_addr, 0, sizeof(udp_common_socket_addr));
	udp_common_socket_addr = IpAndPortInit(set_ip,UDP_COMMON_PORT);

	memset(&udp_shell_socket_addr, 0, sizeof(udp_shell_socket_addr));
	udp_shell_socket_addr = IpAndPortInit(set_ip,UDP_SHELL_PORT);

	memset(&udp_common_big_socket_addr,0,sizeof(udp_common_big_socket_addr));
	udp_common_big_socket_addr = IpAndPortInit(set_ip,UDP_BIG_PORT);

	memset(&udp_heart_socket_addr,0,sizeof(udp_heart_socket_addr));
	udp_heart_socket_addr = IpAndPortInit(set_ip,UDP_HEART_PORT);
	//×××××××××××××××××××××××//

	//创建套接字
	udp_cmd_socket = SockfdServerInit(udp_cmd_socket_addr,"SOCK_DGRAM");
	if(udp_cmd_socket<0){
		net_debug_printf(("the udp socket of cmd init fail!!!\n"));
	}else{
		max_socket = MaxSocket(max_socket,udp_cmd_socket);
	}

	udp_data_socket = SockfdServerInit(udp_data_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_data_socket<0){
		net_debug_printf(("the udp socket of data init fail!!!\n"));
	}else{
		max_socket = MaxSocket(max_socket,udp_data_socket);
	}

	udp_common_socket = SockfdServerInit(udp_common_socket_addr, "SOCK_DGRAM");//创建与eamp的套接字
	if(udp_common_socket<0){
		net_debug_printf(("the udp socket of common init fail!!!\n"));
	}else{
		max_socket = MaxSocket(max_socket,udp_common_socket);
	}

	udp_common_big_socket = SockfdServerInit(udp_common_big_socket_addr,"SOCK_DGRAM");
	if(udp_common_big_socket < 0){
		net_debug_printf(("the udp socket of udp_common_big_socket init fail!!!\n"));
	}else{
		max_socket = MaxSocket(max_socket,udp_common_big_socket);
	}

	udp_heart_socket = SockfdServerInit(udp_heart_socket_addr,"SOCK_DGRAM");
	if(udp_heart_socket < 0){
		net_debug_printf(("the udp socket of udp_heart_socket init fail!!!\n"));
	}else{
		max_socket = MaxSocket(max_socket,udp_heart_socket);
	}


	udp_shell_socket = SockfdServerInit(udp_shell_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_shell_socket<0){
		net_debug_printf(("the udp shell sockfd init fail!!!\n"));
	}else{
		net_debug_printf(("the  udp shell socket is created !!!\n"));
		max_socket = MaxSocket(max_socket,udp_shell_socket);
	}
	//×××××××××××××××××××××××//

	//加入组播
	if(strcmp(dev_info_local.dev_name,"BCU")==0)
	{
		if(ZhwSetMulitIp(udp_cmd_socket,set_ip,MUL_IP_BCU_CMD,UDP_CMD_PORT) < 0){
			net_debug_printf(("udp_cmd_socket %d set mul_ip:%s,port :%d err!!!\n",
			udp_cmd_socket,MUL_IP_BCU_CMD,UDP_CMD_PORT));
		}
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON,UDP_HEART_PORT) < 0){
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",
			udp_heart_socket,MUL_IP_COMMON,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_common_big_socket, set_ip, MUL_IP_SYNC_COMMON_BCU, UDP_SYNC_COMMON_BCU_PORT) < 0){
			net_debug_printf(("BCU, sync, udp_common_big_socket=%d, mul_ip=%s,port=%d, error\n",
			udp_common_big_socket, MUL_IP_SYNC_COMMON_BCU, UDP_SYNC_COMMON_BCU_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket, set_ip, MUL_IP_CC_DATA, UDP_DATA_PORT) < 0) {
			net_debug_printf(("udp_data_socket of BCU=%d, mul_ip=%s_port=%d, error!\n",
			udp_data_socket, MUL_IP_CC_DATA, UDP_DATA_PORT));
		}
		if(ZhwSetMulitIp(udp_common_socket, set_ip, "230.10.10.86", UDP_COMMON_PORT) < 0){
			net_debug_printf(("BCU, sync, udp_common_socket=%d, mul_ip=%s,port=%d, error\n",
		    udp_common_socket, "230.10.10.86", UDP_COMMON_PORT));
		}

		net_debug_printf(("BCU Multi is set ok!!!\n"));
	}
	//×××××××××××××××××××××××//

	//测试当前有数据发送或接受标志
	fd_set frd;
	fd_set fwd;
	fd_set fad;
	FD_ZERO(&fad);
	FD_SET(udp_cmd_socket,&fad);
	FD_SET(udp_data_socket,&fad);
	FD_SET(udp_common_socket,&fad);
	FD_SET(udp_shell_socket,&fad);
	FD_SET(udp_common_big_socket,&fad);
	FD_SET(udp_heart_socket,&fad);
	//×××××××××××××××××××××××//

	while(1)
	{

		FD_ZERO(&frd);
		FD_ZERO(&fwd);
		frd=fad;//将可读标志位赋值
		fwd=fad;

		//监听所有套接口套接口，看是否有数据
		int select_ret = select(max_socket+1,&frd,&fwd,NULL,0);
		if(select_ret==-1)
		{
			net_debug_printf(("the select is error!\n"));
			exit(1);
		}
		else
		{
			//数据接收
			if(FD_ISSET(udp_cmd_socket,&frd))//检测udp的cmd套接字是否有数据可以接受
			{
				DEBUG("There have cmd to receive\n");

				UdpRecvFun(udp_cmd_socket,1,&CachePoint,network_buffer.udp_cmd_socket_buffer.udp_cmd_socket_recv_buffer);

			} //end of FD_ISSET
			if(FD_ISSET(udp_data_socket,&frd))//检测udp的data套接字是否有数据接收
			{
				//diag_printf("There have  audio data to receive\n");

				UdpRecvFun(udp_data_socket,2,&CachePoint,network_buffer.udp_data_socket_buffer.udp_data_socket_recv_buffer);
			}
			if(FD_ISSET(udp_common_socket,&frd))//检测udp的common套接字是否有数据接收
			{
				net_debug_printf(("There have common cmd to receive\n"));
				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(common_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_common_socket, (void *)&common_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
				if ( ret > 0)
				{
            	  ret = BlockBufferWrite(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer, (void *)&common_package_buffer, sizeof(common_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf(("Net : receive common cmd ok, but BlockBufferWrite()<0 \n"));
            	  }
              }

				if( ZhwNetworkRecvForward(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer,1) < 0)
				{
					net_debug_printf(("send package to apply is error!!!\n"));
				}
			}

			if(FD_ISSET(udp_common_big_socket,&frd))//检测udp的big common套接字是否有数据接收
			{
				net_debug_printf2(("There have big common cmd to receive\n"));
				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(common_big_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_common_big_socket, (void *)&common_big_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
				if ( ret > 0)
                {

            	  ret = BlockBufferWrite(network_buffer.server_udp_socket_buffer.udp_server_socket_recv_buffer, (void *)&common_big_package_buffer, sizeof(common_big_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf2(("Net : receive common cmd ok, but BlockBufferWrite()<0 \n"));
            	  }
              }
			}

			if(FD_ISSET(udp_heart_socket,&frd))//检测heart套接字是否有数据接收
			{
				net_debug_printf2(("There have heart package to receive\n"));


				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(heart_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_heart_socket, (void *)&heart_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
              if ( ret > 0)
              {

            	  ret = BlockBufferWrite(network_buffer.udp_heart_socket_buffer.udp_heart_socket_recv_buffer, (void *)&heart_package_buffer, sizeof(heart_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf2(("Net : receive heart package ok, but BlockBufferWrite()<0 \n"));
            	  }
              }
              else
              {
            	  net_debug_printf2(("Net, heart_socket:  recvfrom() < 0 \n"));
              }
			}


			if(FD_ISSET(udp_shell_socket,&frd))//妫€娴媢dp鐨剆hell濂楁帴瀛楁槸鍚︽湁鏁版嵁鎺ユ敹锛燂紵锛燂紵锛燂紵锛?
			{

				memset(shell_recv_buffer,0,sizeof(shell_recv_buffer));
				net_debug_printf(("There shell socket have data to receive\n"));
				if(recvfrom(udp_shell_socket, (void *)shell_recv_buffer,sizeof(shell_recv_buffer),0,(struct sockaddr *)&udp_shell_client_addr,&len) < 0)
				{
					net_debug_printf(("udp_shell_socket receive data is err!!!\n"));
				}
				else
				{
					if(strlen(shell_recv_buffer) > 20 )
					{
						net_debug_printf(("the mast shell common long is 20 ,now the common of into is :%d!!!\n",strlen(shell_recv_buffer)));
					}
					else
					{

						network_shell_package_t recv_shell_from_network;
						memcpy(&recv_shell_from_network,shell_recv_buffer,sizeof(network_shell_package_t));
						DEBUG(">>>>>cmd = %d\n",recv_shell_from_network.cmd);

						int ret = 0;
						ret=BlockBufferWrite(network_buffer.udp_shell_socket_buffer.udp_shell_socket_recv_buffer, (void *)&shell_recv_buffer, sizeof(shell_recv_buffer));
						if ( ret < 0 )
						{
							 net_debug_printf2(("Net : receive shell cmd ok, but BlockBufferWrite()<0 \n"));
						}
					}
				}
			}

			//××××××××××××××××××××××××××//

			//发送数据
			if(FD_ISSET(udp_cmd_socket,&fwd))
			{
				//发送控制信息
				if(BlockBufferRead(network_buffer.udp_cmd_socket_buffer.udp_cmd_socket_send_buffer,(void *)&network_send_cmd_buf,sizeof(network_send_cmd_buf))>0)
				{

					DEBUG("send cmd to ocs sizeof %d!\n",sizeof(network_send_cmd_buf));
#if 0
						unsigned char buf[320];
						memcpy(buf,&network_send_cmd_buf,320);
									int i;
									for(i=0;i<320;i++)
									{
									diag_printf("buf[%d]=0x%02x\n",i,buf[i]);
								   }
#else

							if(network_send_cmd_buf.send_information.event_type_intercom == 8)
							{

							}
#endif


						if(UdpSendFunCMD(udp_cmd_socket,&network_send_cmd_buf,sizeof(network_send_cmd_buf),OCS_IP,UDP_CMD_PORT)< 0)//发送控制数据
						{

							DEBUG("send cmd to ocs  fail\n");

						}
				}
			}

			if(FD_ISSET(udp_data_socket,&fwd))
			{
				//发送音频信息
				read_data_long = CharBufferRead(network_buffer.udp_data_socket_buffer.udp_data_socket_send_buffer,(void *)data_buffer_send,sizeof(data_buffer_send));
				if( read_data_long > 0)
				{
					//diag_printf("send audio data to ocs !\n");

					UdpSendFunDATA(udp_data_socket,data_buffer_send,read_data_long,OCS_IP,UDP_DATA_PORT);//发送音频数据给EAMP
					//UdpSendFunDATA(udp_data_socket,data_buffer_send,read_data_long,"230.10.10.56",UDP_DATA_PORT);//发送音频数据给EAMP

				}
			}

#if 0
			if(FD_ISSET(udp_common_socket, &fwd))
			{//发送common 数据
				if(BlockBufferRead(network_buffer.udp_common_socket_buffer.udp_common_socket_send_buffer,(void *)&select_car_package_buffer,sizeof(select_car_package_buffer))>0)
				{
					//UdpSendFunCMD(udp_common_socket,&select_car_package_buffer,sizeof(ocs_select_car_t),,UDP_COMMON_PORT);//发送控制数据
		        }
			}
#endif


			if(FD_ISSET(udp_shell_socket,&fwd))
			{
				memset(shell_send_buffer,0,sizeof(shell_send_buffer));
				if(BlockBufferRead(network_buffer.udp_shell_socket_buffer.udp_shell_socket_send_buffer,(void *)&shell_send_buffer,sizeof(shell_send_buffer))>0)
				{
					DEBUG("There shell socket have data to send-2016\n");
					int ret = sendto(udp_shell_socket, (void *)shell_send_buffer,
													sizeof(network_shell_package_t),0,
													(struct sockaddr *)&udp_shell_client_addr, len);
					if(ret < 0)
					{
						net_debug_printf(("udp_shell_socket send data is err!!!\n"));
					}
				}
			}

			if(FD_ISSET(udp_common_big_socket, &fwd))
			{
				//发送big common 数据
				if(BlockBufferRead(network_buffer.server_udp_socket_buffer.udp_server_socket_send_buffer,(void *)&common_big_package_buffer,sizeof(common_big_package_buffer))>0)
				{
					DEBUG("send bigcom to ocs--------- %d!\n",sizeof(common_big_package_buffer));;
					if(common_big_package_buffer.pkg_type==1)
					{
						diag_printf("select car big pkg\n");
#if 0
						int i=0;
						for(;i<11;i++)
						{
							DEBUG("i=%d,common_big_package_buffer=%d\n",i,common_big_package_buffer.common_big_data_u.car_select_flag[i]);
						}
#endif
					}


					UdpSendFunCMD(udp_common_big_socket,&common_big_package_buffer,sizeof(common_big_package_t),OCS_IP,UDP_COMMON_BIG_PORT);//发送控制数据

					if(common_big_package_buffer.pkg_type==9||common_big_package_buffer.pkg_type==10)
					{
						UdpSendFunCMD(udp_common_big_socket,&common_big_package_buffer,sizeof(common_big_package_t),MUL_IP_SYNC_COMMON_BCU,UDP_SYNC_COMMON_BCU_PORT);//发送控制数据
					}
				}
			}


#if 0
			if(FD_ISSET(udp_heart_socket, &fwd))
			{
				//发送heart 数据
				if(BlockBufferRead(network_buffer.udp_heart_socket_buffer.udp_heart_socket_send_buffer,(void *)&heart_package_buffer,sizeof(heart_package_buffer))>0)
				{
					UdpSendFunCMD(udp_heart_socket,&heart_package_buffer,sizeof(network_heart_package_t),,UDP_HEART_PORT);
				}
			}
#endif
			//××××××××××××××××××××××××××//

		}

        PthreadPriorityChangeForSchedRr(thread_of_network, 10);

        sched_yield();//释放此次线程控制权		
	}
	
	CacheFree(CachePoint);

}

char *zhw_network_get_local_ip(void)
{
	struct dev_type_and_no local_device_io;
	struct global_info local_dev_info_local;

	local_device_io = ReadFromIO();
	local_dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
	debug_print(("zhw_networ_get_local_ip: %d-%d-%s\n", local_device_io.dev_type,
		               local_device_io.dev_number, local_dev_info_local.dev_ip));

	return  local_dev_info_local.dev_ip;
}
int zhw_dev_weather_server(int a)
{
	struct dev_type_and_no local_device_io;
	struct global_info local_dev_info_local;
	
	local_device_io = ReadFromIO();
	local_dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
	debug_print(("zhw_dev_weather_server: %d-%d-%s\n", local_device_io.dev_type, local_device_io.dev_number, local_dev_info_local.dev_ip));

	return  local_dev_info_local.is_server;
}
#if 1
char * zhw_server_dev_info(struct global_info *temp1)
{		global_info local;
		char *ip;
		struct dev_type_and_no temp;
		temp = ReadFromIO();
		local  = ReadGlobalArray(temp.dev_type,NULL,temp.dev_number,1);
		memcpy(temp1,&local,sizeof(global_info));
		char local_mac_temp[6];
		int loop;
	//	net_debug_printf(("the dev type is :%d,the dev number is :%d\n",local_device_io.dev_type,local_device_io.dev_number));

		ip = local.dev_ip;
		for(loop = 0 ;loop < 6 ;loop++)
		{
			local_mac_temp[loop] = local.dev_mac[loop];
		}

		net_debug_printf(("system already define CYGHWR_NET_DRIVER_ETH0_MANUAL!!!\n"));
		net_info_t net_info;
		net_info.inf_name = LOCAL_DEV_NAME;
		net_info.addrs_ip = ip;
		net_info.addrs_netmask = NETMASK_ADDRS;
		net_info.addrs_broadcast = BROADCAST_ADDRS;
		net_info.addrs_gateway = GATEWAY_ADDRS;
		net_info.addrs_server = SERVER_ADDRS;

		if(ZhwSetAddToSys(net_info,local_mac_temp)<0)
		{
			net_debug_printf(("init network is failed for %s !!!\n",net_info.inf_name));
			exit(0);
		}
		return ip;

}
#endif
void ForceBUfferData_wilson()
{
	ForceClearDataCache(p_buffer_pointer->ArrayToHandle,&(p_buffer_pointer->pToData));
}

struct global_info zhw_server_info(int a)
{
	struct global_info local_dev_info_local = {0};
	if( (a==1) || (a==2) )
	{
		local_dev_info_local = ResolveUseServerNumber(a);
	}
	 
	return local_dev_info_local;
}
