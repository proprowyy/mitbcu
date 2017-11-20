/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_dev_config_info.c
 * 文件标识：
 * 摘    要： 设备配置信息主函数
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-03-26
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#include "zhw_dev_config_info.h"
#ifdef BCU_DEV
int update_start = 0 ;
pthread_mutex_t mutex_update_ip_array;
#endif

//struct dev_type_and_no local_dev;
global_array_t *global_array_ptr = NULL;
global_array_t *global_array_ptr_head = NULL;
pthread_mutex_t zhw_buffer_register_lock;


int row;
int column;

int eamp_number = 0;
int bcu_number = 0;
int ccu_number = 0;
int pcu_number = 0;
int cmu_number = 0;

int db_err_status = 0 ;
static long network_malloc_size = 0;///wilson add
char network_temp_ip_info_buff[100];///wilson add


#if 0
/*
int ZhwUpdateBCUIpArray(ip_mac_table_row_t *pip_mac_table_row)
{
	char *save_point_start = NULL ;
	char *save_point_middle = NULL ;
	char *save_point_end = NULL ;
	char a = 0;
	char b = 0;
	int length = 0 ;
	int i ;

#ifdef BCU_DEV
	pthread_mutex_lock(&mutex_update_ip_array);
#endif
	global_array_ptr = global_array_ptr_head;

	while(global_array_ptr != NULL)
	{
		if((global_array_ptr->global_config_info.dev_type == pip_mac_table_row->current_row_info.device_type)
				&&(global_array_ptr->global_config_info.dev_number == pip_mac_table_row->current_row_info.device_no))
		{
			global_array_ptr->global_config_info.is_server = pip_mac_table_row->current_row_info.server_whether;
			strcpy((char *)global_array_ptr->global_config_info.dev_name,(char *)pip_mac_table_row->current_row_info.device_name);
			strcpy((char *)global_array_ptr->global_config_info.dev_ip,(char *)pip_mac_table_row->current_row_info.ip_address);
//			strcpy((char *)global_array_ptr->global_config_info.dev_mac,(char *)pip_mac_table_row->current_row_info.mac_address);

			{
#if 1 ///wilson add
			    save_point_start = &network_temp_ip_info_buff[0];
#else
				save_point_start = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.mac_address));				bzero(save_point_start,sizeof(pip_mac_table_row->current_row_info.mac_address));
#endif
				memcpy(save_point_start,pip_mac_table_row->current_row_info.mac_address,sizeof(pip_mac_table_row->current_row_info.mac_address));
				save_point_middle = save_point_start;
				save_point_end = save_point_start;
				while(save_point_end != NULL)
				{
					save_point_end = strstr(save_point_middle,",");

					length = (int)(strlen(save_point_middle) - strlen(save_point_end));

					if( i < 6 )
					{
						if(length  == 2)
						{
							a = *(save_point_middle);
							save_point_middle++;
							b = *(save_point_middle);
							global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
						}
						else if ( (length > 0) && (length < 2) )
						{
							memcpy(&b,save_point_middle,sizeof(b));
							global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
						}

						i++ ;
					}
					save_point_middle = (save_point_end + 1) ;
				}
				#if 1/// wilson add
				save_point_start = NULL ;
				#else
//				diag_printf("zhw-11111111-info:%s\n",save_point_start);
				free(save_point_start); // add by zhw-2014-12-29
				save_point_start = NULL ;
//				diag_printf("zhw-33333333\n");
				#endif
			}

			global_array_ptr = global_array_ptr_head;
#ifdef BCU_DEV
			pthread_mutex_unlock(&mutex_update_ip_array);
#endif
			return 1;
		}
		else
		{
			if(global_array_ptr->next != NULL)
			{
				global_array_ptr = global_array_ptr->next;
			}
			else
			{
				break;
			}
		}
	}
	row += 1;
	global_array_ptr->next = (global_array_t *)malloc(sizeof(global_array_t));
	global_array_ptr = global_array_ptr->next;
	if(global_array_ptr != NULL)
	{
		bzero(global_array_ptr,sizeof(global_array_t));
		global_array_ptr->global_config_info.dev_number = pip_mac_table_row->current_row_info.device_no;
		global_array_ptr->global_config_info.dev_type = pip_mac_table_row->current_row_info.device_type;
		global_array_ptr->global_config_info.is_server = pip_mac_table_row->current_row_info.server_whether;

		global_array_ptr->global_config_info.dev_name = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.device_name));
		network_malloc_size += sizeof(pip_mac_table_row->current_row_info.device_name);
		diag_printf("3:network_malloc_size = %ld\n",network_malloc_size);
		bzero(global_array_ptr->global_config_info.dev_name,sizeof(pip_mac_table_row->current_row_info.device_name));
		strcpy((char *)global_array_ptr->global_config_info.dev_name,(char *)pip_mac_table_row->current_row_info.device_name);

		global_array_ptr->global_config_info.dev_ip = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.ip_address));
		network_malloc_size += sizeof(pip_mac_table_row->current_row_info.ip_address);
		diag_printf("4:network_malloc_size = %ld\n",network_malloc_size);
		bzero(global_array_ptr->global_config_info.dev_ip,sizeof(pip_mac_table_row->current_row_info.ip_address));
		strcpy((char *)global_array_ptr->global_config_info.dev_ip,(char *)pip_mac_table_row->current_row_info.ip_address);

//		strcpy((char *)global_array_ptr->global_config_info.dev_mac,(char *)pip_mac_table_row->current_row_info.mac_address);
		{
#if 1///wilson add
			save_point_start = &network_temp_ip_info_buff[0];
#else
			save_point_start = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.mac_address));
#endif
			bzero(save_point_start,sizeof(pip_mac_table_row->current_row_info.mac_address));
			memcpy(save_point_start,pip_mac_table_row->current_row_info.mac_address,sizeof(pip_mac_table_row->current_row_info.mac_address));
			save_point_end = save_point_start;
			save_point_middle = save_point_start;
			while(save_point_end != NULL)
			{
				save_point_end = strstr(save_point_middle,",");

				length = (int)(strlen(save_point_middle) - strlen(save_point_end));

				if( i < 6 )
				{
					if(length  == 2)
					{
						a = *(save_point_middle);
						save_point_middle++;
						b = *(save_point_middle);
						global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
					}
					else if ( (length > 0) && (length < 2) )
					{
						memcpy(&b,save_point_middle,sizeof(b));
						global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
					}

					i++ ;
				}
				save_point_middle = (save_point_end + 1) ;
			}
#if 0///wilson add
//			diag_printf("zhw-11111111-info:%s\n",save_point_start);
			free(save_point_start); // add by zhw-2014-12-29
#endif
			save_point_start = NULL ;
//			diag_printf("zhw-33333333\n");
		}

		global_array_ptr->next = NULL;
		global_array_ptr = global_array_ptr_head;
#ifdef BCU_DEV
		pthread_mutex_unlock(&mutex_update_ip_array);
#endif
		return 2;
	}
	else
	{
		net_debug_printf(("malloc filed for the new device\n"));
	}
#ifdef BCU_DEV
	pthread_mutex_unlock(&mutex_update_ip_array);
#endif
	return 0;
}
*/
#endif



int ReadType(void)
{
	int type = 3; ///< 1:PCU; 5:CCU; 3:BCU; 7:EAMP
	return type;
}

int ReadNum(void)
{
	int number = BCU_DEV_NO;
	return number;
}

int CountDevNumber(char *dev_name,int dev_number)
{
	if((strcmp(dev_name,"EAMP") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		eamp_number++;
		return 1;
	}
	if((strcmp(dev_name,"BCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		bcu_number++;
		return 1;
	}
	if((strcmp(dev_name,"CCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		ccu_number++;
		return 1;
	}
	if((strcmp(dev_name,"PCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		pcu_number++;
		return 1;
	}
	return 0;
}

int ZhwGetDevNumber(char *dev_name)
{
	int inqu_i;
	int dev_number = 0;
	global_array_t *inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<row;inqu_i++)
	{
		if( (strcmp(dev_name,inqu_ptr->global_config_info.dev_name) == 0) && (0 < inqu_ptr->global_config_info.dev_number) && ( inqu_ptr->global_config_info.dev_number < 200) )
		{
			(dev_number) = (dev_number + 1) ;
		}
		inqu_ptr =inqu_ptr->next;
	}
	if(dev_number > 0)
	{
		return dev_number;
	}

//	net_debug_printf(("the %s number in ZhwGetDevNumber() function inqu is %d\n",dev_name,dev_number));
	return -1;
}

/*
 *   	@brief	本函数的功能是通过调用底层I/O口驱动函数获得设备类型
 *   			和设备号；
 *     @param void 无传入参数
 *     @return   dev_type_and_no  本接口对应的设备类型和设备号
 */
struct dev_type_and_no ReadFromIO(void)
{
	struct dev_type_and_no local_dev;
	local_dev.dev_type = ReadType();	//获取设备类型
	local_dev.dev_number = ReadNum();	//获取设备号

	return local_dev;
}

/*
 *   	@brief	本函数的功能是读取数据库中设备信息并对全局数组进行初始化；
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @return   device_ip  相应设备的IP地址
 */
void InitGlobalArray(void)
{
	int row_mark;
	int read_db_ret = 1;
	global_array_ptr = (global_array_t *)malloc(sizeof(global_array_t));
	network_malloc_size = network_malloc_size + sizeof(global_array_t);
	diag_printf("6-network_malloc_size = %ld\n",network_malloc_size);
	bzero(global_array_ptr,sizeof(global_array_t));
	global_array_ptr_head = global_array_ptr;

	char *save_point_start = NULL ;
	char *save_point_middle = NULL ;
	char *save_point_end = NULL ;
	char a = 0;
	char b = 0;
	int length = 0 ;

	ip_mac_table_item_t *point = NULL;
	int i ;

	pthread_mutex_init(&zhw_buffer_register_lock, NULL); ///< add, 2013
#ifndef BCU_DEV
#else
		//add for update ip array zhw-2014-11-11
		pthread_mutex_init(&mutex_update_ip_array,NULL);
#endif
	{
		net_debug_printf(("not have DB,get message from local file !!!\n"));
		if(ZhwReadFromTestDB(&row, &column,(void **)&point) < 0)
		{
			net_debug_printf(("get message from local file is err!!!\n"));
		}
	}


	for(row_mark = 0;row_mark<row;row_mark++,point++)
	{
		i = 0 ;
		global_array_ptr->next = (global_array_t *)malloc(sizeof(global_array_t));
		global_array_ptr = global_array_ptr->next;
		bzero(global_array_ptr,sizeof(global_array_t));

		global_array_ptr->global_config_info.dev_type = point->device_type;

		global_array_ptr->global_config_info.dev_name = (char *)malloc(sizeof(point->device_name));
		network_malloc_size = network_malloc_size + sizeof(point->device_name);
		diag_printf("7-network_malloc_size = %ld\n",network_malloc_size);
		bzero(global_array_ptr->global_config_info.dev_name,sizeof(point->device_name));
		memcpy(global_array_ptr->global_config_info.dev_name,point->device_name,sizeof(point->device_name));
		net_debug_printf(("device name  = %s--",point->device_name));

		global_array_ptr->global_config_info.dev_number = point->device_no ;
		net_debug_printf(("device number  = %d--",point->device_no));

		global_array_ptr->global_config_info.dev_ip = (char *)malloc(sizeof(point->ip_address));
		network_malloc_size = network_malloc_size + sizeof(point->ip_address);
		diag_printf("8-network_malloc_size = %ld\n",network_malloc_size);
		bzero(global_array_ptr->global_config_info.dev_ip,sizeof(point->ip_address));
		memcpy(global_array_ptr->global_config_info.dev_ip,point->ip_address,sizeof(point->ip_address));


		if( (0 == read_db_ret) && (0 == db_err_status) )
		{
#if 1 ///wilson add
			 save_point_start = &network_temp_ip_info_buff[0];
#else
			save_point_start = (char *)malloc(sizeof(point->mac_address));
#endif
			bzero(save_point_start,sizeof(point->mac_address));
			memcpy(save_point_start,point->mac_address,sizeof(point->mac_address));
			save_point_middle = save_point_start;
			save_point_end = save_point_start;
			while(save_point_end != NULL)
			{
				save_point_end = strstr(save_point_middle,",");

				length = (int)(strlen(save_point_middle) - strlen(save_point_end));

				if( i < 6 )
				{
					if(length  == 2)
					{
						a = *(save_point_middle);
						save_point_middle++;
						b = *(save_point_middle);
						global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
					}
					else if ( (length > 0) && (length < 2) )
					{
						memcpy(&b,save_point_middle,sizeof(b));
						global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
					}

					i++ ;
				}
				save_point_middle = (save_point_end + 1) ;
			}
#if 0 ///wilson add
			free(save_point_start); // add by zhw-2014-12-29
#endif
			save_point_start = NULL ;
		}
		else
		{
			for(i = 0 ;i < 6 ;i++)
			{
				global_array_ptr->global_config_info.dev_mac[i] = point->mac_address[i];
			}
		}

		net_debug_printf(( "Mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
	    		global_array_ptr->global_config_info.dev_mac[0],
	    		global_array_ptr->global_config_info.dev_mac[1],
	    		global_array_ptr->global_config_info.dev_mac[2],
	    		global_array_ptr->global_config_info.dev_mac[3],
	    		global_array_ptr->global_config_info.dev_mac[4],
	    		global_array_ptr->global_config_info.dev_mac[5] ));

		global_array_ptr->global_config_info.is_server = point->server_whether ;

	}

	global_array_ptr->next = NULL;
	point = NULL;
}

/*
 *   	@brief	本函数的功能是根据所给的设备类型或者设备名（只需给出其中的一个即可，另一个可用0或NULL表示）和设备号解析出设备的IP地址
 *   			用于向相应设备发送数据和解析本地设备；
 *  	@param device_type 从IO接口读取出的本地设备类型，从驱动中直接读取过来的是整数；
 *  			111（整数7）:表示EAMP；
 *  			011（整数3）:表示BCU；
 *  			001（整数1）:表示PCU；
 *  			101 (整数5)：表示CCU;
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @param resolve_type 表示给出的用于解析的是设备名还是设备类型；
 *     				1：表示使用设备类型来查询（表示本地设备）；
 *     				2：表示使用设备名来查询（表示全局设备）；
 *     				3 :表示使用ip地址来查询.
 *
 *     @return   device_info  相应设备整体信息
 */
struct global_info ReadGlobalArray(int device_type,char *device_name_or_ip,int device_number,int resolve_type)
{
	struct global_info ret_dev_info;
#ifdef BCU_DEV
	if(1 == update_start)
	{
		pthread_mutex_lock(&mutex_update_ip_array);
	}
#endif
	switch(resolve_type)
	{
	case 1:
		if(device_type < 0) //changed by zhw-2014-11-17 for CMU
		{
			net_debug_printf(("the number of device_type is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseType(device_type,device_number);
		break;
	case 2:
		if(device_name_or_ip == NULL)
		{
			net_debug_printf(("the number of device_name is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseName(device_name_or_ip,device_number);
		break;
	case 3:
		if(device_name_or_ip == NULL)
		{
			net_debug_printf(("the number of device_name is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseIp(device_name_or_ip);
		break;
	default :
		net_debug_printf(("the number of resolve_type is err!!!\n"));
		break;
	}
#ifdef BCU_DEV
	if(1 == update_start)
	{
		pthread_mutex_unlock(&mutex_update_ip_array);
	}
#endif
	return ret_dev_info;
}

/*
 *   	@brief	本函数的功能是根据所给的设备类型和设备号责解析出本地设备的IP地址；
 *     @param device_type 需要解析的设备的接口类型
 *     @param device_number 	需要解析的设备号
 *     @return  local_device_ip  本地设备的IP地址
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseType(int device_type,int device_number)
{
	int inqu_i;
	int i;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<row;inqu_i++)
	{
		if((device_type == inqu_ptr->global_config_info.dev_type)&&(device_number == inqu_ptr->global_config_info.dev_number))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0;i < 6; i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}
	if(ret_dev_info.dev_ip == NULL)
	{
		net_debug_printf(("local inqu is err,something is wrong!!!\n"));
	}
	return ret_dev_info;
}

/*
 *   	@brief	本函数的功能是根据所给的设备名和设备号责解析出全局设备的IP地址；
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @return  global_device_ip  所需查询的全局设备的IP地址
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseName(char *device_name,int device_number)
{
	int inqu_i;
	int i;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
		if((strcmp(device_name,inqu_ptr->global_config_info.dev_name) == 0)&&(device_number == inqu_ptr->global_config_info.dev_number))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0;i < 6;i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}
	if(ret_dev_info.dev_ip == NULL)
	{
		net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}

	return ret_dev_info;
}


/*
 *   	@brief	本函数的功能是根据所给的设备IP解析出设备信息
 *     @param device_ip 需要解析的设备ip
 *     @return  ret_dev_info  所需查询的设备的信息。
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseIp(char *device_ip)
{
	int inqu_i;
	int i;
	int strcmp_ret = 1;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
		if(((strcmp_ret = strcmp(device_ip,inqu_ptr->global_config_info.dev_ip)) == 0))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0; i < 6; i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}

	if(strcmp_ret != 0)
	{
		net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}

	return ret_dev_info;
}

//用于查询服务器的信息
struct global_info ResolveUseServerNumber(int a)
{
	int inqu_i;
	int i;
	int strcmp_ret = 1;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	
	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
	if(inqu_ptr->global_config_info.is_server == a )
	{
	ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
	ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
	ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
	for(i = 0; i < 6; i++)
	{
	ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
	}
	ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
	ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
	strcmp_ret = 0 ;
	break;
	}
	else
	{
	inqu_ptr =inqu_ptr->next;
	}
	}
	 
	if(strcmp_ret != 0)
	{
	net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}
	 
	return ret_dev_info;
}
