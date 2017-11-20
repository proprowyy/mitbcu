/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： set_cmd_package.cxx
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
#include "set_cmd_package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

send_infomation_t send_ts_cmd_to_control =
{
		BCUTSCmdInit,
		BCUSetTSCmd,
};

//控制信息（发送结构体）初始化
void BCUTSCmdInit(void)
{
	send_ts_cmd_to_control.src_devices_no = 0x00;
	strcpy((char *)send_ts_cmd_to_control.src_devices_name,"");
	memset((void *)&send_ts_cmd_to_control.event_info_ann,0,sizeof(send_ts_cmd_to_control.event_info_ann));
	memset((void *)&send_ts_cmd_to_control.event_info_intercom,0,sizeof(send_ts_cmd_to_control.event_info_intercom));
	send_ts_cmd_to_control.sequence = 0;

}

///<根据触摸屏通信格式设置相应命令包
void BCUSetTSCmd(unsigned char event_type,
					char *src_devices_name,	unsigned char src_devices_no,
					unsigned char state_request,unsigned char request_or_over,
					unsigned char line_number,unsigned char region_number,
					unsigned int broadcast_sequence,unsigned char broadcast_times,
					unsigned char arrived_station,unsigned char current_station_code,unsigned char station_effective,unsigned char whether_pre_read,
					unsigned char dst_device_number)
{
	int current_event = (int)event_type;
	strcpy((char *)send_ts_cmd_to_control.src_devices_name ,src_devices_name);
	send_ts_cmd_to_control.src_devices_no = src_devices_no;

	send_ts_cmd_to_control.event_ptt_state = 0;//wilson 2012-12-29


	if(current_event == OCC_EVENT || current_event == LIVE_ANN_EVENT || current_event == EMERG_ANN_EVENT ||
	   current_event == EMERG_ANN_EVENT || current_event == MANUAL_ANN_EVENT || current_event == SKIP_STATION||
	   current_event == LINE_NUMBER_CHANGE_EVENT)
	{
		send_ts_cmd_to_control.event_type_ann = event_type;

		send_ts_cmd_to_control.event_type_intercom = INTERCOM_IDLE;
	}
	else if(current_event == D2P_INTERCOMM_EVENT || current_event == D2D_INTERCOMM_EVENT || current_event == D2D_HANGUP_D2P_EVENT)
	{
		send_ts_cmd_to_control.event_type_intercom = event_type;
		send_ts_cmd_to_control.event_type_ann = ANN_IDLE;
	}
	else
	{
		send_ts_cmd_to_control.event_type_ann = ANN_IDLE;
		send_ts_cmd_to_control.event_type_intercom = INTERCOM_IDLE;
	}
	switch(current_event)
	{

		case LIVE_ANN_EVENT:
			send_ts_cmd_to_control.event_info_ann.live_announce.live_active = state_request;
			send_ts_cmd_to_control.event_info_ann.live_announce.live_begin_or_over = request_or_over;
			send_ts_cmd_to_control.event_info_ann.live_announce.live_response = 0;

			break;


		case D2D_INTERCOMM_EVENT:
			send_ts_cmd_to_control.event_info_intercom.d2d_intercomm.d2d_intercomm_active = state_request;
			send_ts_cmd_to_control.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = request_or_over;
			send_ts_cmd_to_control.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = dst_device_number;
			break;
		case D2P_INTERCOMM_EVENT:
			send_ts_cmd_to_control.event_info_intercom.d2p_intercomm.d2p_intercomm_active = state_request;
			send_ts_cmd_to_control.event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;
			send_ts_cmd_to_control.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = request_or_over;
			send_ts_cmd_to_control.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = dst_device_number;
			send_ts_cmd_to_control.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
			break;

		default:
			break;
	}
}

//触摸屏填充控制信息
void BcuScreenSetCmd(unsigned char event_type,
						char *src_devices_name,	unsigned char src_devices_no,
						unsigned char state_request,unsigned char request_or_over,
						unsigned char line_number,unsigned char region_number,
						unsigned int broadcast_sequence,unsigned char broadcast_times,
						unsigned char arrived_station,unsigned char current_station_code,
						unsigned char station_effective,unsigned char whether_pre_read,
						unsigned char dst_device_number)
{
//wilson 2013-06-20
	send_ts_cmd_to_control.setinformation(event_type,//事件类型
													src_devices_name,//源设备名称
													src_devices_no,//源设备号
													state_request,//状态请求结束
													request_or_over,//事件请求结束,0:开始 1:结束 2：预读
													line_number,//线路号
													region_number,
													broadcast_sequence,//文件序列号
													broadcast_times,//播报次数
													arrived_station,//是否到站
													current_station_code,//站代码
													station_effective,
													whether_pre_read,
													dst_device_number//对讲状态下的目标设备号
													);
}
int TouchScreenSetCmd(void *param_table)

{
	unsigned char state_request = 0;
	unsigned char request_or_over = 0;
	unsigned char line_number = 0;
	unsigned char region_number = 0;
	unsigned int broadcast_sequence = 0;
	unsigned char broadcast_times = 0;
	unsigned char stop_or_leave = 0;
	unsigned char current_station_code = 0;
	unsigned char current_station_effective = 0;
	unsigned char manual_pre_read = 0;
	unsigned char dst_device_number = 0;
	bcu_screen_param_t *param_table_ptr = (bcu_screen_param_t  *)param_table;

	if( NULL== param_table_ptr )
	{

		return -1;
	}
	   	
	switch( param_table_ptr->event_type )
	{
		case OCC_EVENT:
			state_request = param_table_ptr->state_active;
			request_or_over = param_table_ptr->begin_or_over;
			break;
		case LIVE_ANN_EVENT:
			state_request = param_table_ptr->state_active;
			request_or_over = param_table_ptr->begin_or_over;
			break;
		case EMERG_ANN_EVENT:
//			//printf("-----------------------------EMERG_ANN_EVENT-------------\n");
			state_request = param_table_ptr->state_active;
			request_or_over = param_table_ptr->begin_or_over;
			broadcast_sequence = param_table_ptr->broadcast_sequence;
			broadcast_times = param_table_ptr->broadcast_times;
			break;
		case MANUAL_ANN_EVENT:
//		    //printf("-----------------------------MANUAL_ANN_EVENT-------------\n");
			state_request = param_table_ptr->state_active;
			request_or_over = param_table_ptr->begin_or_over;
			line_number = param_table_ptr->line_number;
			current_station_code = param_table_ptr->current_station_code;
			stop_or_leave = param_table_ptr->stop_or_leave;
			manual_pre_read = param_table_ptr->manual_pre_read;
			break;
		case D2D_INTERCOMM_EVENT:
		case D2P_INTERCOMM_EVENT:					
			state_request = param_table_ptr->state_active;
			request_or_over = param_table_ptr->begin_or_over;	
			dst_device_number = param_table_ptr->dst_device_number;	
			break;
		case LINE_NUMBER_CHANGE_EVENT:
		   line_number = param_table_ptr->line_number;
		   region_number = param_table_ptr->region_number;
		   break;
		case SKIP_STATION:
			ts_debug_print(("skip station\n"));
			line_number = param_table_ptr->line_number;
			current_station_code = param_table_ptr->current_station_code;
			current_station_effective = param_table_ptr->station_effective;
			ts_debug_print(("line_number = %d,current_station_code = %d,current_station_effective = %d\n",line_number,current_station_code,current_station_effective));
			break;
		default:

			return -1;
	}		   
//	return 0;
	// 打包写入到缓存区中
	BcuScreenSetCmd( param_table_ptr->event_type,    // event type
					 bcu_state.bcu_info.devices_name,            // source device name
					 bcu_state.bcu_info.devices_no, // source device number
					 state_request,   // state_request -----> active
					 request_or_over, // request or over ----> begin
					 line_number,       // line_number
					 region_number,
					 broadcast_sequence,  // broadcast_sequence
					 broadcast_times,        // broadcast_times
					 stop_or_leave,         // arrived or predict
					 current_station_code,  // current_station_code
					 current_station_effective,
					 manual_pre_read,
					 dst_device_number     // dst_device_number for intercom


	);


	int ret = 0,write_counts = 0;
	send_ts_cmd_to_control.sequence = ts_package_sequence;
	ret = BlockBufferWrite(ts_buffer_id, &send_ts_cmd_to_control, sizeof(send_ts_cmd_to_control));
	write_counts ++;
	while(ret <= 0 && write_counts < 2)
	{
		cyg_thread_delay(10);
		ret = BlockBufferWrite(ts_buffer_id, &send_ts_cmd_to_control, sizeof(send_ts_cmd_to_control));
		write_counts ++;
		diag_printf("!!!!!@!!!-ret = %d,%d\n",ret,ts_package_sequence);
	}

	return 0;
}
