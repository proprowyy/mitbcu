/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：bcu_function.c
 * 文件标识：
 * 摘    要：bcu处理的函数的库函数
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：2012-10-24
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */

#include "bcu_function.h"
unsigned char network_priority = 10;
unsigned char ts_priority = 10;
unsigned char control_priority = 10;

unsigned char whether_updata_db = 0;
unsigned char sim_auto_flag = 0;//by pan --add 2015-12-25

/*----------------begin of bcu beta 1.1----------------------*/
/*PPT state information*/
common_package_t network_send_package_ptt_release_to_bcu;
common_package_t network_send_package_ptt_press_to_bcu;
common_package_t network_send_package_ptt_release_to_pcu;
common_package_t network_send_package_ptt_press_to_pcu;

//发送选车命令与监听命令
common_big_package_t *iph_select,*iph_select_intercom;
send_infomation_t g_MonPcuCmdPakage ,g_iph_pcu,bcu_send_infomation;
//××××××××××××××××××××××××××××××//




unsigned char deamon_timer_flag = 0;
unsigned char key_timer_flag = 0;
unsigned char sync_timer_flag = 0;
unsigned char network_count = 0;


///<BCU发送给其他设备的命令包
common_package_t dev_vol_to_eamp,dev_vol_to_pcu,dev_vol_to_bcu,recv_dev_col_bcu,dev_vol_to_ccu;

int package_sequence = 0;
int ntwork_thread_prio = 0;//网络线程优先级
char *blank_audio_data = NULL;//空白数据首地址
int whether_could_be_finished_d2d = 0;//是否结束司机对讲
int whether_eant_to_delay_finished_d2d = 0;//判断是否延时结束司机对讲
int whether_have_begin_broadcast_alarm_audio_data = 0;//是否已经开始播放PCU请求提示音
unsigned char whether_get_out3d5_button_state=0;//是否获取外接3D5按钮状态
unsigned char last_out3d5_button_state=0;//上一次3D5状态
unsigned char whether_ask_key_info = 0;//是否发送钥匙信息查询包
unsigned char whether_send_to_pcu = 0;//定时发送信息包给PCU，解决长久PCU没有网络通信，首次PCU请求无法接通问题

/*司机对讲延迟定时器，目前尚未使用*/
cyg_handle_t  counter_handle_delay_d2d;
cyg_handle_t alarm_handle_delay_d2d;
cyg_alarm alarm_object_delay_d2d;

/*钥匙信号查询定时器*/
cyg_handle_t  counter_handle_key_info;
cyg_handle_t alarm_handle_key_info;
cyg_alarm alarm_object_key_info;

/*外接3D5检测定时器，目前尚未使用*/
cyg_handle_t  counter_handle_get_out3d5_button_state;
cyg_handle_t alarm_handle_get_out3d5_button_state;
cyg_alarm alarm_object_get_out3d5_button_state;
bcu_state_t bcu_state =
{
	BCURuleInit,
	BCUAddRule,
	BCURemoveRule,
	BCUDisableRule,
	BCUEnableRule,
	{0},
	{0},
	{{{0}}},
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};


/*Save the audio data*/
char src_device_info[10] = "\0";
char this_device_info[10] = "\0";
char audio_data_log_file_name[60] = "\0";
FILE *fp_audio_data_log_file = NULL;
unsigned char whether_save_live_audio_data = 0;
unsigned char whether_save_d2d_audio_data = 0;
unsigned char whether_save_d2p_audio_data = 0;

void InitBcuAndState()
{
	  bcu_state.system_language = 0;
	  bcu_state.live_button_state = 0;
	  bcu_state.d2d_button_state = 0;
	  bcu_state.d2p_button_state = 0;
	  bcu_state.ts_current_state = 1;


	  bcu_state.iph_monitor_cur_page =0;
	  bcu_state.select_monitor_or_ann_page = 0;
	  bcu_state.d2p_intercom_page = 0;
	  bcu_state.d2d_intercom_page = 0;
	  bcu_state.elsect_car_page=0;
	  bcu_state.static_page=1;
}

int BcuGetNetTxCmdBuffId(void)
{
     return bcu_state.cmd_send_buffer_id;
}

int InitBuffer(void)
{
	int network_send_audio_buffer;
	int network_receive_audio_buffer;
	int network_receive_control_buffer;//网络线程接受控制线程的buffer
	int network_send_control_buffer;//网络线程发送控制线程的buffer
	int control_receive_cmd_from_screen;//主控线程与触摸屏线程之间的buffer
	int send_info_to_pis_buffer; //发送控制信息给PIS
	int recv_info_to_pis_buffer; //接受控制信息给PIS
	int local_audio_data_buffer;
	int alarm_audio_data_buffer;
	int ptt_state_send_buffer_id;
	int ptt_state_recv_buffer_id;
	int network_udp_common_send_buffer;
	int network_udp_common_recv_buffer;
	int control_recv_ts_buffer;
	int dev_vol_info_buffer;



	control_recv_ts_buffer = BlockBufferCreate(2, sizeof(ts_dev_volume_info_t), "bcu-control-recv-ts-info");
	if (0 == control_recv_ts_buffer )
	{
		return -1;
	}


	network_send_audio_buffer = CharBufferCreate( (DEFAULT_AUDIO_BUFFER_SIZE),"bcu-network-audio-tx");//用于音频发送
	if (0 == network_send_audio_buffer )
	{
		return -1;
	}
	CharBufferCreate((3 * 1024 + 64),"bcu-network-audio-pendig");

	network_receive_audio_buffer = CharBufferCreate((DEFAULT_AUDIO_BUFFER_SIZE + 3 * 1024),"bcu-network-audio-rx"); //用于音频接收(对讲)
	network_receive_audio_buffer = CharBufferCreate((DEFAULT_AUDIO_BUFFER_SIZE + 3 * 1024),"bcu-local-audio-rx"); //用于音频接收(对讲)
	if ( 0 == network_receive_audio_buffer )
	{
		 CharBufferDelete(network_send_audio_buffer);
		 return -1;
	}

	network_receive_control_buffer = BlockBufferCreate(DEFAULT_BUFFER_TOTAL_BLOCKS, sizeof(network_send_package_t), "bcu-network-cmd-rx"); //用于网络接收响应,块缓冲
	if ( 0 == network_receive_control_buffer )
	{
	   CharBufferDelete(network_send_audio_buffer);
	   CharBufferDelete(network_receive_audio_buffer);
	   return -1;
	}

	network_send_control_buffer = BlockBufferCreate(10, sizeof(network_send_package_t),"bcu-network-cmd-tx"); //用于主控线程接收响应,块缓冲
	if ( 0 == network_send_control_buffer )
	{
	   CharBufferDelete(network_send_audio_buffer);
	   CharBufferDelete(network_receive_audio_buffer);
	   BlockBufferDelete(network_receive_control_buffer);
	   return -1;
	}

	control_receive_cmd_from_screen = BlockBufferCreate(5, sizeof(send_infomation_t),"bcu-screen-cmd-tx"); //用于接收响应,块缓冲
	if ( 0 == control_receive_cmd_from_screen )
	{
	   CharBufferDelete(network_send_audio_buffer);
	   CharBufferDelete(network_receive_audio_buffer);
	   BlockBufferDelete(network_receive_control_buffer);
	   BlockBufferDelete(network_send_control_buffer);
	   return -1;
	}

	alarm_audio_data_buffer = CharBufferCreate((300 + (1024 * 11) ),"bcu-alarm-audio-data");

	if ( 0 == alarm_audio_data_buffer )
	{
		 CharBufferDelete(network_send_audio_buffer);
		 CharBufferDelete(network_receive_audio_buffer);
		 BlockBufferDelete(network_receive_control_buffer);
		 BlockBufferDelete(network_send_control_buffer);
		 BlockBufferDelete(control_receive_cmd_from_screen);
		 BlockBufferDelete(send_info_to_pis_buffer);
		 BlockBufferDelete(recv_info_to_pis_buffer);
		 BlockBufferDelete(local_audio_data_buffer);
		 return -1;
	}



	ptt_state_recv_buffer_id = BlockBufferCreate(3, sizeof(common_package_t),"bcu-ptt-state-rx");
	if ( 0 == ptt_state_recv_buffer_id )
	{
		 CharBufferDelete(network_send_audio_buffer);
		 CharBufferDelete(network_receive_audio_buffer);
		 BlockBufferDelete(network_receive_control_buffer);
		 BlockBufferDelete(network_send_control_buffer);
		 BlockBufferDelete(control_receive_cmd_from_screen);
		 BlockBufferDelete(send_info_to_pis_buffer);
		 BlockBufferDelete(recv_info_to_pis_buffer);
		 BlockBufferDelete(local_audio_data_buffer);
		 CharBufferDelete(alarm_audio_data_buffer);
		 BlockBufferDelete(ptt_state_send_buffer_id);
		 return -1;
	}//bcu-ptt-state-tx

	network_udp_common_send_buffer = BlockBufferCreate(5, sizeof(common_package_t), "network-udp-common-send");
	if (0 == network_udp_common_send_buffer )
	{
		CharBufferDelete(network_send_audio_buffer);
		CharBufferDelete(network_receive_audio_buffer);
		BlockBufferDelete(network_receive_control_buffer);
		BlockBufferDelete(network_send_control_buffer);
		BlockBufferDelete(control_receive_cmd_from_screen);
		BlockBufferDelete(send_info_to_pis_buffer);
		BlockBufferDelete(recv_info_to_pis_buffer);
		BlockBufferDelete(local_audio_data_buffer);
		CharBufferDelete(alarm_audio_data_buffer);
		BlockBufferDelete(ptt_state_send_buffer_id);
		BlockBufferDelete(ptt_state_recv_buffer_id);
	   return -1;
	}


	dev_vol_info_buffer = BlockBufferCreate(2, sizeof(common_package_t), "network-udp-dev_vol-recv");
	if(dev_vol_info_buffer == -1)
	{
		return -1;
	}

	network_udp_common_recv_buffer = BlockBufferCreate(3, sizeof(common_package_t), "network-udp-common-recv"); //用于网络接收响应,块缓冲
   if ( 0 == network_udp_common_recv_buffer )
	{
		CharBufferDelete(network_send_audio_buffer);
		CharBufferDelete(network_receive_audio_buffer);
		BlockBufferDelete(network_receive_control_buffer);
		BlockBufferDelete(network_send_control_buffer);
		BlockBufferDelete(control_receive_cmd_from_screen);
		BlockBufferDelete(send_info_to_pis_buffer);
		BlockBufferDelete(recv_info_to_pis_buffer);
		BlockBufferDelete(local_audio_data_buffer);
		CharBufferDelete(alarm_audio_data_buffer);
		BlockBufferDelete(ptt_state_send_buffer_id);
		BlockBufferDelete(ptt_state_recv_buffer_id);
		BlockBufferDelete(network_udp_common_send_buffer);
	   return -1;
   }

	int ret = 0;
	ret= BlockBufferCreate(3, sizeof(network_heart_package_t), "network-udp-heart-recv");
	if ( 0 == ret )
	{
		return -1;
	}
	ret= BlockBufferCreate(3, sizeof(network_heart_package_t), "network-udp-heart-send");
	if ( 0 == ret )
	{
		return -1;
	}

	ret= BlockBufferCreate(2, sizeof(common_big_package_t), "bcu-recv-all-dev-state");
	if ( 0 == ret )
	{
		return -1;
	}

	ret = BlockBufferCreate(3, sizeof(common_big_package_t), "network-udp-common-big-recv-version-check");
	if(ret == 0)
	{
		return -25;
	}


		ret = BlockBufferCreate(2, sizeof(network_shell_package_t),"network-udp-shell-recv");
		if ( 0 == ret )
		{
			return -30;
		}
		ret = BlockBufferCreate(2, sizeof(network_shell_package_t),"network-udp-shell-send");
		if ( 0 == ret )
		{
			return -31;
		}
		BlockBufferCreate(3, sizeof(common_big_package_t), "network-udp-server-send");
		BlockBufferCreate(2, sizeof(common_big_package_t), "network-udp-server-recv");

	return 0;
}

/*This function is used to clear all audio data buffer*/
void ClearAllAudioDataBuffer()
{
	/*Clear aduio data send buffer*/
	CharBufferClear(bcu_state.audio_data_send_buffer_id);

	/*Clear aduio data reveive buffer*/
	CharBufferClear(bcu_state.audio_data_recv_buffer_id);

	return ;
}


/*Set PPT state information*/
void SetPTTStateInfoPackage()
{
	/*send to opposite bcu*/
	network_send_package_ptt_release_to_bcu.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
	strcpy(network_send_package_ptt_release_to_bcu.src_dev_name,bcu_state.bcu_info.devices_name);
	network_send_package_ptt_release_to_bcu.src_dev_number = bcu_state.bcu_info.devices_no;

	strcpy(network_send_package_ptt_release_to_bcu.dst_dev_name,bcu_state.bcu_info.devices_name);
	network_send_package_ptt_release_to_bcu.dst_dev_number = bcu_state.opposite_bcu_no;

	network_send_package_ptt_release_to_bcu.common_data_u.comm_ptt_state.src_ptt = 0;
	network_send_package_ptt_release_to_bcu.common_data_u.comm_ptt_state.dst_ptt = 0;


	network_send_package_ptt_press_to_bcu.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
	strcpy(network_send_package_ptt_press_to_bcu.src_dev_name,bcu_state.bcu_info.devices_name);
	network_send_package_ptt_press_to_bcu.src_dev_number = bcu_state.bcu_info.devices_no;

	strcpy(network_send_package_ptt_press_to_bcu.dst_dev_name,bcu_state.bcu_info.devices_name);
	network_send_package_ptt_press_to_bcu.dst_dev_number = bcu_state.opposite_bcu_no;

	network_send_package_ptt_press_to_bcu.common_data_u.comm_ptt_state.src_ptt = 1;
	network_send_package_ptt_press_to_bcu.common_data_u.comm_ptt_state.dst_ptt = 0;


	/*send to opposite pcu*/
	network_send_package_ptt_release_to_pcu.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
	strcpy(network_send_package_ptt_release_to_pcu.src_dev_name,"BCU");
	network_send_package_ptt_release_to_pcu.src_dev_number = bcu_state.bcu_info.devices_no;

	strcpy(network_send_package_ptt_release_to_pcu.dst_dev_name,"PCU");
	network_send_package_ptt_release_to_pcu.dst_dev_number = 0;

	network_send_package_ptt_release_to_pcu.common_data_u.comm_ptt_state.src_ptt = 0;
	network_send_package_ptt_release_to_pcu.common_data_u.comm_ptt_state.dst_ptt = 0;


	network_send_package_ptt_press_to_pcu.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
	strcpy(network_send_package_ptt_press_to_pcu.src_dev_name,"BCU");
	network_send_package_ptt_press_to_pcu.src_dev_number = bcu_state.bcu_info.devices_no;

	strcpy(network_send_package_ptt_press_to_pcu.dst_dev_name,"PCU");
	network_send_package_ptt_press_to_pcu.dst_dev_number = 0;

	network_send_package_ptt_press_to_pcu.common_data_u.comm_ptt_state.src_ptt = 1;
	network_send_package_ptt_press_to_pcu.common_data_u.comm_ptt_state.dst_ptt = 0;

	return ;
}

/*Init BCU rule*/
void BCURuleInit()
{
	int i = 0;
	for(i = 0;i < RECORD_OF_RULES;i ++)
	{
		memset((void *)&bcu_state.bcu_state_rule_table[i].xrule,0,sizeof(bcu_state.bcu_state_rule_table[i].xrule));
		bcu_state.bcu_state_rule_table[i].xrule.is_empty = 1;
		bcu_state.bcu_state_rule_table[i].rule_enable = FALSE;
	}
}

/*Add bcu rule*/
void BCUAddRule(state_machine_rule_t xRule)
{
	int i = 0;
	for(i = 0;i < RECORD_OF_RULES;i ++)
	{
		if(bcu_state.bcu_state_rule_table[i].xrule.is_empty == 1)
		{
			memcpy((void *)&bcu_state.bcu_state_rule_table[i].xrule ,(void *)&xRule,sizeof(xRule));
			bcu_state.bcu_state_rule_table[i].xrule.is_empty = 0;
			bcu_state.bcu_state_rule_table[i].rule_enable = TRUE;
			break;
		}
	}
}

/*remove bcu rule*/
void BCURemoveRule(state_machine_rule_t xRule)
{
	int i = 0;
	for(i = 0;i < RECORD_OF_RULES;i ++)
	{
		if(bcu_state.bcu_state_rule_table[i].xrule.is_empty == 0)
		{
			if(bcu_state.bcu_state_rule_table[i].xrule.psrc_state->state_id == xRule.psrc_state->state_id)
			{
				if(bcu_state.bcu_state_rule_table[i].xrule.dest_state->state_id == xRule.dest_state->state_id)
				{
					if(bcu_state.bcu_state_rule_table[i].xrule.trigger_event == xRule.trigger_event)
					{
						memset((void *)&bcu_state.bcu_state_rule_table[i].xrule,'0',sizeof(bcu_state.bcu_state_rule_table[i].xrule));
						bcu_state.bcu_state_rule_table[i].xrule.is_empty = 0;
						bcu_state.bcu_state_rule_table[i].rule_enable = FALSE;
						break;
					}
				}
			}
		}
	}
}

/*disable bcu rule*/
void BCUDisableRule(state_machine_rule_t xRule)
{
	int i = 0;
	for(i = 0;i < RECORD_OF_RULES;i ++)
	{
		if(bcu_state.bcu_state_rule_table[i].xrule.is_empty == 0)
		{
			if(bcu_state.bcu_state_rule_table[i].xrule.psrc_state->state_id == xRule.psrc_state->state_id)
			{
				if(bcu_state.bcu_state_rule_table[i].xrule.dest_state->state_id == xRule.dest_state->state_id)
				{
					if(bcu_state.bcu_state_rule_table[i].xrule.trigger_event == xRule.trigger_event)
					{
						bcu_state.bcu_state_rule_table[i].rule_enable = FALSE;
						break;
					}
				}
			}
		}
	}
}

/*enable bcu rule*/
void BCUEnableRule(state_machine_rule_t xRule)
{
	int i = 0;
	for(i = 0;i < RECORD_OF_RULES;i ++)
	{
		if(bcu_state.bcu_state_rule_table[i].xrule.is_empty == 0)
		{
			if(bcu_state.bcu_state_rule_table[i].xrule.psrc_state->state_id == xRule.psrc_state->state_id)
			{
				if(bcu_state.bcu_state_rule_table[i].xrule.dest_state->state_id == xRule.dest_state->state_id)
				{
					if(bcu_state.bcu_state_rule_table[i].xrule.trigger_event == xRule.trigger_event)
					{
						bcu_state.bcu_state_rule_table[i].rule_enable = TRUE;
						break;
					}
				}
			}
		}
	}
}

/*Set rule*/
state_machine_rule_t RuleSet(state_process_t *src_state,state_process_t *dst_state,int trigger_event)
{
	state_machine_rule_t xRule;
	xRule.psrc_state = src_state;
	xRule.dest_state = dst_state;
	xRule.trigger_event = trigger_event;
	return xRule;
}

/*Initial bcu rule table*/
void BCURuleTableInit()
{
	bcu_state.RuleInit();

	bcu_state.state_process_ann.auto_announcement = &auto_annoucement_state;
	bcu_state.state_process_ann.idle =  &idle_ann_state;
	bcu_state.state_process_ann.live_announcement =  &live_annoucement_state;
	bcu_state.state_process_ann.occ =  &occ_state;
	bcu_state.state_process_ann.emergency_announcement =  &emergency_annoucement_state;
	bcu_state.state_process_ann.manual_announcement =  &manual_annoucement_state;

	bcu_state.state_process_intercom.idle = &idle_intercom_state;
	bcu_state.state_process_intercom.d2d_intercom = &d2d_intercomm_state;
	bcu_state.state_process_intercom.d2p_intercom = &d2p_intercomm_state;
	bcu_state.state_process_intercom.d2d_hang_up_d2p = &intercomm_hang_up_state;
	bcu_state.state_process_intercom.d2p_moniort_state=&d2p_moniort_state;
	//idle intercom-2
	bcu_state.AddRule(RuleSet(&idle_intercom_state,&d2d_intercomm_state,EVENT_PTT_OR_DRIVER_CALL));
	bcu_state.AddRule(RuleSet(&idle_intercom_state,&d2p_intercomm_state,EVENT_CONFIRM_PCU_CALL));
	bcu_state.AddRule(RuleSet(&idle_intercom_state,&d2p_moniort_state,D2P_MONITOR_EVENT_CALL));
	bcu_state.AddRule(RuleSet(&d2p_moniort_state,&idle_intercom_state,D2P_MONITOR_EXIT_CALL));
	//D2D-1
	bcu_state.AddRule(RuleSet(&d2d_intercomm_state,&idle_intercom_state,EVENT_PTT_RELEASE_AND_DRIVER_RELEASE));

	//D2P-2
	bcu_state.AddRule(RuleSet(&d2p_intercomm_state,&idle_intercom_state,EVENT_CONFIRM_RELEASE_PCU));
	bcu_state.AddRule(RuleSet(&d2p_intercomm_state,&intercomm_hang_up_state,EVENT_TRANSFER_TO_D2D_OR_DRIVER_CALL));

	//D2D hang up D2P-2
	bcu_state.AddRule(RuleSet(&intercomm_hang_up_state,&d2d_intercomm_state,EVENT_TIME_OUT));
	bcu_state.AddRule(RuleSet(&intercomm_hang_up_state,&d2p_intercomm_state,EVENT_CONFIRM_TRANSFER_TO_D2P));
}


void RegisterCommonBufferRecvBuffer()
{
	ZhwBufferRegister(1,COMMON_PACKAGE_TYPE_PTT_STATE,1, bcu_state.ppt_state_recv_buffer_id);
	///<注册设备音量、监听扬声器音量、所有设备状态接收缓存区
	ZhwBufferRegister(1,COMMON_PACKAGE_TYPE_DEVICE_INFO,1, bcu_state.udp_common_recv_dev_colueme);
	ZhwBufferRegister(1,COMMON_PACKAGE_TYPE_MONITOR_SPEAKER_CONTROL,1, bcu_state.udp_common_recv_dev_colueme);
	ZhwBufferRegister(2,COMMON_PACKAGE_TYPE_ALL_DEV_STATE,1, bcu_state.all_dev_state_buffer_id);
	ZhwBufferRegister(2,COMMON_PACKAGE_TYPE_SOFTWARE_VERSION,1,bcu_state.software_version_buffer_id);

}
/*Initialize the bcu control thread data buffer*/
void BCUControlMainBufferInit()
{
	int network_send_audio_buffer; // 发送音频数据缓冲区，每次从这里取出数据发送
	int network_receive_audio_buffer; // 接收音频数据缓冲区，每次收到音频数据，存放在此
	int network_receive_cmd_buffer; // 接受响应缓冲区(块)，每次收到响应信息，存放在此
	int control_receive_cmd_from_screen;//主控线程与触摸屏线程之间的buffer
	int control_send_cmd_to_network;//主控线程发送网络线程之间buffer
	int local_audio_data_buffer;
	int bcu_alarm_audio_data;
	int ppt_state_recv_buffer_id;

	bcu_state.comm_server_send_big_buffer_id = BlockBufferOpen("network-udp-server-send");
	bcu_state.comm_server_recv_big_buffer_id = BlockBufferOpen("network-udp-server-recv");


	bcu_state.comm_cmd_send_buffer_id = BlockBufferOpen("network-udp-common-send");
	bcu_state.comm_cmd_recv_buffer_id = BlockBufferOpen("network-udp-common-recv");

	bcu_state.udp_common_recv_dev_colueme = BlockBufferOpen("network-udp-dev_vol-recv");
	bcu_state.software_version_buffer_id = BlockBufferOpen("network-udp-common-big-recv-version-check");

	network_send_audio_buffer = CharBufferOpen("bcu-network-audio-tx");
	network_receive_audio_buffer = CharBufferOpen("bcu-network-audio-rx");

	bcu_state.local_audio_data_recv_buffer_id = CharBufferOpen("bcu-local-audio-rx");


	network_receive_cmd_buffer = BlockBufferOpen("bcu-network-cmd-rx");
	control_send_cmd_to_network = BlockBufferOpen("bcu-network-cmd-tx");
	control_receive_cmd_from_screen = BlockBufferOpen("bcu-screen-cmd-tx");



	local_audio_data_buffer = CharBufferOpen("bcu-local-audio-data");
	bcu_alarm_audio_data = CharBufferOpen("bcu-alarm-audio-data");

	ppt_state_recv_buffer_id = BlockBufferOpen("bcu-ptt-state-rx");

	bcu_state.dev_vol_info_buffer_id = BlockBufferOpen("bcu-control-recv-ts-info");

	bcu_state.cmd_send_buffer_id = control_send_cmd_to_network;
	bcu_state.cmd_recv_buffer_id = network_receive_cmd_buffer;
	bcu_state.audio_data_send_buffer_id = network_send_audio_buffer;
	bcu_state.audio_data_recv_buffer_id = network_receive_audio_buffer;
	bcu_state.recv_cmd_from_touch = control_receive_cmd_from_screen;
	bcu_state.local_audio_data_buffer_id = local_audio_data_buffer;
	bcu_state.alarm_audio_data_buffer_id = bcu_alarm_audio_data;

	bcu_state.ppt_state_recv_buffer_id = ppt_state_recv_buffer_id;

	bcu_state.pending_buffer_id = CharBufferOpen("bcu-network-audio-pendig");
	bcu_state.all_dev_state_buffer_id = BlockBufferOpen("bcu-recv-all-dev-state");
	
	bcu_state.shell_recv_buffer_id = BlockBufferOpen("network-udp-shell-recv");
    bcu_state.shell_send_buffer_id = BlockBufferOpen("network-udp-shell-send");
	
}

/*initialize the original state of bcu*/
void BCUStateInit()
{///<初始化BCU 状态
#if 1
	bcu_state.bcu_info.current_state_ann = ANN_IDLE;
	bcu_state.bcu_info.current_state_intercom = INTERCOM_IDLE;
	bcu_state.bcu_active_ann_state =  &idle_ann_state;
	bcu_state.bcu_active_intercom_state = &idle_intercom_state;
	bcu_state.mic_owner = NONE;
	bcu_state.other_bcu_ptt_state = 0;

#endif
}

/*set d2p over cmd package*/
void SetD2POverCMD(send_infomation_t *param_cmd_info)
{
#if 0
	param_cmd_info->event_type_ann = ANN_IDLE;
	param_cmd_info->event_type_intercom = D2P_INTERCOM_PENDING;
	memset((void *)&param_cmd_info->event_info_ann,0,sizeof(param_cmd_info->event_info_ann));
	param_cmd_info->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 0;
#endif
	return ;
}

/*D2D hangup D2P timeout handle*/
void D2DHangUpD2PTimeOutHandle()
{
	if(whether_d2d_hangup_d2p_timeout == 1)
	{///<目前没有使用，司机对讲打断乘客对讲之后，如果司机对讲退出，自动切换到与之前的乘客继续进行对讲
		whether_d2d_hangup_d2p_timeout = 0;

		send_infomation_t d2p_over_cmd;

		SetD2POverCMD(&d2p_over_cmd);

		UpdataGlobalDeviceInfo(d2p_over_cmd);/*Update global device information*/
		StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_TIME_OUT,&d2p_over_cmd);
	}
}

/*Change bcu state*/
void StateMachineExchange(state_process_t **state_process,int trigger_signal,send_infomation_t *param_send_info)
{
#if 1
	int i = 0;
	debug_print(("(*state_process)->state_id = %d,trigger_signal = %d\n",(*state_process)->state_id,trigger_signal));
	for(i = 0;i < RECORD_OF_RULES;i++)
	{
		if(bcu_state.bcu_state_rule_table[i].rule_enable == TRUE &&
		   (*state_process)->state_id == bcu_state.bcu_state_rule_table[i].xrule.psrc_state->state_id &&
		   trigger_signal == bcu_state.bcu_state_rule_table[i].xrule.trigger_event)
		{
			(*state_process)->exit();
			*state_process = bcu_state.bcu_state_rule_table[i].xrule.dest_state;
			cyg_thread_delay(10);
			(*state_process)->enter(param_send_info);
			break;
		}
	}
	if(i == RECORD_OF_RULES )
	{
		intercomm_debug_print(("aaaaa,error request!\n"));
	}
#endif
	return ;
}
void MoniortStateMachineExchange(state_process_t **state_process,int trigger_signal,send_infomation_t *param_send_info){

	int i = 0;
		debug_print(("(*state_process)->state_id = %d,trigger_signal = %d\n",(*state_process)->state_id,trigger_signal));
		for(i = 0;i < RECORD_OF_RULES;i++)
		{
			if(bcu_state.bcu_state_rule_table[i].rule_enable == TRUE &&
			   (*state_process)->state_id == bcu_state.bcu_state_rule_table[i].xrule.psrc_state->state_id &&
			   trigger_signal == bcu_state.bcu_state_rule_table[i].xrule.trigger_event)
			{
				(*state_process)->exit();
				*state_process = bcu_state.bcu_state_rule_table[i].xrule.dest_state;
				cyg_thread_delay(10);
				(*state_process)->enter(param_send_info);
				break;
			}
		}

		if(i == RECORD_OF_RULES )
		{
			intercomm_debug_print(("aaaaa,error request!\n"));
		}



}
/*Chaneg thread's priority*/
int PthreadPriorityChangeForSchedRr(pthread_t  target_thread_id, int  new_prior )
{
	 int a,b;

	  if(target_thread_id == thread_of_control )
	  {
		  control_priority = new_prior;
	  }
	  else if(target_thread_id == thread_of_screen )
	  {
		  ts_priority = new_prior;
	  }
	  else if(target_thread_id == thread_of_network )
	  {
		  network_priority = new_prior;
	  }
	  struct sched_param param_temp;
	  if(target_thread_id == thread_of_network )
	  {
		  pthread_getschedparam(target_thread_id, &a, &param_temp);
		  b = param_temp.sched_priority;
	  }
	  param_temp.sched_priority = new_prior;
	  pthread_setschedparam(target_thread_id, SCHED_RR, &param_temp);
	  pthread_getschedparam(target_thread_id, &a, &param_temp);
	  sched_yield();
	  if(param_temp.sched_priority != new_prior && target_thread_id == thread_of_network)
	  {
		  debug_print(("thread priority change error-%d-%d-%d\n",param_temp.sched_priority,new_prior,b));
	  }
	  return 0;
}

/*Judge whether have finish d2d*/
void JudgeWhetherD2DHaveFinished()
{///<判断D2D是否已经完成
	if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT ||
	   bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)
	{
		if(GetD2DExternButtonState() == 1 && bcu_state.d2d_button_state == 1)
		{
			send_infomation_t d2d_over_package;

			d2d_over_package.src_devices_no = bcu_state.bcu_info.devices_no;
			strcpy(d2d_over_package.src_devices_name,bcu_state.bcu_info.devices_name);

			d2d_over_package.event_type_ann = ANN_IDLE;
			d2d_over_package.event_type_intercom = D2D_INTERCOMM_EVENT;

			d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
			d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
			d2d_over_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 0;
			d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;

			UpdataGlobalDeviceInfo(d2d_over_package);/*Update global device information*/
			if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)//pending
			{
				send_infomation_t rt_d2d_bk_d2p,*p_d2d_bk_d2p;
				p_d2d_bk_d2p = &rt_d2d_bk_d2p;
				memset((void *)p_d2d_bk_d2p,0x00,sizeof(rt_d2d_bk_d2p));
				p_d2d_bk_d2p->event_type_ann = ANN_IDLE;
				p_d2d_bk_d2p->event_type_intercom = D2P_INTERCOMM_EVENT;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 1;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pending = 0;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = bcu_state.pcu_request_info.recept_pcu_no;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = 1;
				p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;
				p_d2d_bk_d2p->src_devices_no = bcu_state.bcu_info.devices_no;
				strcpy((char *)p_d2d_bk_d2p->src_devices_name,(char *)bcu_state.bcu_info.devices_name);
				StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_CONFIRM_TRANSFER_TO_D2P,&rt_d2d_bk_d2p);

				current_is_being_d2d = 0;

				AlarmTSToChangeScreen(31);
			}
			else if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)//idle
			{
				debug_print(("We will finish the D2D\n"));
				StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_PTT_RELEASE_AND_DRIVER_RELEASE,&d2d_over_package);
			}
		}
	}
}


void judgeWheteherExitD2DEnterD2P()
{///<判断是否退出D2D再进入D2P
	if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)
	{
		send_infomation_t d2d_over_package;

		d2d_over_package.src_devices_no = bcu_state.bcu_info.devices_no;
		strcpy(d2d_over_package.src_devices_name,bcu_state.bcu_info.devices_name);

		d2d_over_package.event_type_ann = ANN_IDLE;
		d2d_over_package.event_type_intercom = D2D_INTERCOMM_EVENT;

		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;
		UpdataGlobalDeviceInfo(d2d_over_package);/*Update global device information*/
		if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)//pending
		{
			send_infomation_t rt_d2d_bk_d2p,*p_d2d_bk_d2p;
			p_d2d_bk_d2p = &rt_d2d_bk_d2p;
			memset((void *)p_d2d_bk_d2p,0x00,sizeof(rt_d2d_bk_d2p));
			p_d2d_bk_d2p->event_type_ann = ANN_IDLE;
			p_d2d_bk_d2p->event_type_intercom = D2P_INTERCOMM_EVENT;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pending = 0;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = bcu_state.pcu_request_info.recept_pcu_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;
			p_d2d_bk_d2p->src_devices_no = bcu_state.bcu_info.devices_no;
			strcpy((char *)p_d2d_bk_d2p->src_devices_name,(char *)bcu_state.bcu_info.devices_name);

			StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_CONFIRM_TRANSFER_TO_D2P,&rt_d2d_bk_d2p);
			current_is_being_d2d = 0;
#if 0
			PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT+1);
			ChangeIntercommLabelState(current_is_being_d2d);
			Fl::wait();
			wd_touch_screen->show();
			cyg_thread_delay(10);
			PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT);
#else
			AlarmTSToChangeScreen(31);
#endif
		}
	}
}

/*Judge whether have finish d2d*/
void NetFinishD2D()
{///<网络结束D2D

	if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT ||
	   bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)
	{
		if(bcu_state.d2d_button_state != 1)
		{
			return ;
		}
		send_infomation_t d2d_over_package;

		d2d_over_package.src_devices_no = bcu_state.bcu_info.devices_no;
		strcpy(d2d_over_package.src_devices_name,bcu_state.bcu_info.devices_name);

		d2d_over_package.event_type_ann = ANN_IDLE;
		d2d_over_package.event_type_intercom = D2D_INTERCOMM_EVENT;

		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;
		UpdataGlobalDeviceInfo(d2d_over_package);/*Update global device information*/
		if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)//pending
		{
			send_infomation_t rt_d2d_bk_d2p,*p_d2d_bk_d2p;
			p_d2d_bk_d2p = &rt_d2d_bk_d2p;
			memset((void *)p_d2d_bk_d2p,0x00,sizeof(rt_d2d_bk_d2p));
			p_d2d_bk_d2p->event_type_ann = ANN_IDLE;
			p_d2d_bk_d2p->event_type_intercom = D2P_INTERCOMM_EVENT;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pending = 0;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = bcu_state.pcu_request_info.recept_pcu_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;
			p_d2d_bk_d2p->src_devices_no = bcu_state.bcu_info.devices_no;
			strcpy((char *)p_d2d_bk_d2p->src_devices_name,(char *)bcu_state.bcu_info.devices_name);
			StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_CONFIRM_TRANSFER_TO_D2P,&rt_d2d_bk_d2p);
			current_is_being_d2d = 0;
#if 0
			ChangeIntercommLabelState(current_is_being_d2d);
#else
			AlarmTSToChangeScreen(31);
#endif
		}
		else if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)//idle
		{
			debug_print(("We will finish the D2D\n"));
			StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_PTT_RELEASE_AND_DRIVER_RELEASE,&d2d_over_package);
		}
	}
}



/*Judge whether want to enter D2D state*/
void JudegWhetherRequestD2D()
{
	if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE &&
			GetD2DExternButtonState() == 1 && bcu_state.mic_owner == NONE && bcu_state.d2d_button_state == 0)
	{
		debug_print(("I want to enter D2D state\n"));

		send_infomation_t D2D_request_cmd_package;

		strcpy(D2D_request_cmd_package.src_devices_name,"BCU");
		D2D_request_cmd_package.src_devices_no = bcu_state.bcu_info.devices_no;

		D2D_request_cmd_package.event_type_ann = ANN_IDLE;
		D2D_request_cmd_package.event_type_intercom = D2D_INTERCOMM_EVENT;

		D2D_request_cmd_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 1;
		D2D_request_cmd_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
		D2D_request_cmd_package.event_info_intercom.d2d_intercomm.d2d_intercomm_response = 0;
		D2D_request_cmd_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;
		D2D_request_cmd_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 1;
		UpdataGlobalDeviceInfo(D2D_request_cmd_package);/*Update global device information*/
		bcu_state.this_bcu_request = 1;
		StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_PTT_OR_DRIVER_CALL,&D2D_request_cmd_package);

	}

}



/*Update Line number*/


/*Update skip station information*/



/*Handle announcement event*/
void ANNStateHangle(send_infomation_t *param_recv_cmd_info_of_ann,send_infomation_t *param_cmd_info)/*BCU announcement handle*/
{

	if(param_cmd_info->event_type_ann != ANN_IDLE)
	{

		if(WhetherWantToExchangeState(*param_cmd_info))
		{
			debug_print(("I want to exchange state\n"));

			if(param_cmd_info->event_type_ann == LIVE_ANN_EVENT && bcu_state.mic_owner != NONE &&
					param_cmd_info->event_info_ann.live_announce.live_active == 1)
			{
				debug_print(("we want to change to live,but the mic have been occupied\n"));
				param_cmd_info->src_devices_no = 0;
				strcpy((char *)param_cmd_info->src_devices_name,"");
			}
			else
			{
				debug_print(("ANN State change\n"));

				BcuDynamicStateExchange(&bcu_state.bcu_active_ann_state,param_cmd_info);

			}
		}
		else
		{
			memcpy((void *)param_recv_cmd_info_of_ann,(void *)param_cmd_info,sizeof(*param_cmd_info));
		}
	}

}


/*Handle intercom event*/
void IntercomStateHangle(send_infomation_t *param_recv_cmd_info_of_intercom,send_infomation_t *param_cmd_info)/*BCU announcement handle*/
{
	/*refuse the request of PCU*/
	if(param_cmd_info->event_type_intercom != INTERCOM_IDLE)
	{
		if(param_cmd_info->event_type_intercom == D2P_INTERCOMM_EVENT &&
		   param_cmd_info->event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0)
		{
		//	BCUUpdataPCUDeviceInfo(*param_cmd_info);
//			CharBufferBrush(bcu_state.alarm_audio_data_buffer_id);
		}

		if(param_cmd_info->event_type_intercom == D2P_INTERCOMM_EVENT &&
		   param_cmd_info->event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1 &&
		   param_cmd_info->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over == 0)
		{
			bcu_state.pcu_request_info.recept_pcu_no =
					param_cmd_info->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no;

			debug_print(("active_pcu_no = %d\n",bcu_state.pcu_request_info.recept_pcu_no));
		}

			if(WhetherWantToExchangeState(*param_cmd_info))
			{
				intercomm_debug_print(("Intercomm State change\n"));
				StateMachineExchange(&bcu_state.bcu_active_intercom_state,GetEventType(bcu_state.bcu_active_intercom_state,*param_cmd_info),param_cmd_info);
			}
			else
			{
				memcpy((void *)param_recv_cmd_info_of_intercom,(void *)param_cmd_info,sizeof(*param_cmd_info));
			}

			param_recv_cmd_info_of_intercom->src_devices_no = 0;
			strcpy(param_recv_cmd_info_of_intercom->src_devices_name,"");

	}
}

/*judge whether want to exchange state*/
int WhetherWantToExchangeState(send_infomation_t send_information)
{
	int ret = 0;
	int current_event = 0;
	if(send_information.event_type_ann != ANN_IDLE)
	{
		current_event = send_information.event_type_ann;
	}
	else if(send_information.event_type_intercom != INTERCOM_IDLE)
	{
		current_event = send_information.event_type_intercom;
	}
	debug_print(("current_event = %d\n",current_event));
	switch(current_event)
	{

		case LIVE_ANN_EVENT:
			if(bcu_state.bcu_active_ann_state->state_id != LIVE_ANN_EVENT)
			{
				if(	send_information.event_info_ann.live_announce.live_active == 0)
				{
					ret = 0;
				}
				else
				{
					ret = 1;
				}
			}
			else if(send_information.event_info_ann.live_announce.live_active == 0)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;

		case D2D_INTERCOMM_EVENT:
			if(bcu_state.bcu_active_intercom_state->state_id != D2D_INTERCOMM_EVENT &&
				bcu_state.bcu_active_intercom_state->state_id != D2D_HANGUP_D2P_EVENT)
			{
				ret = 1;
			}
			else if(send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 0)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;
		case D2P_INTERCOMM_EVENT:
			if(bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT)
			{
				ret = 1;
			}
			else if(send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			if(bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT &&
			   send_information.event_type_intercom == D2P_INTERCOMM_EVENT &&
			   send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0)
			{
				ret = 0;
			}

			break;
		default:
			ret = 0;
			break;
		}
	return ret;
}

/*Judge whether have D2D request from network*/
void JudgeWhetherHaveD2DRequest(send_infomation_t *param_cmd_info,network_send_package_t *param_network_cmd_info)
{
	send_infomation_t param_send_infomation;
	if(param_network_cmd_info->send_information.event_type_intercom == D2D_INTERCOMM_EVENT &&
	   param_network_cmd_info->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active ==1 &&
	   param_network_cmd_info->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over == 0 &&
	   param_network_cmd_info->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_response == 0)
	{
		if((bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT ) &&bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT)//提示仲裁界面
		{

			if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT)
			{

				ForceExitLiveStateBeforeIntercomm();
			}
			if(WhetherWantToExchangeState(param_network_cmd_info->send_information))
			{

				memcpy((void *)&param_send_infomation,(void *)&param_network_cmd_info->send_information,sizeof(param_network_cmd_info->send_information));
				param_send_infomation.event_info_intercom.d2d_intercomm.d2d_intercomm_response = 1;
				param_send_infomation.init = NULL;/*need to modify*/
				param_send_infomation.setinformation = NULL;
				StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_PTT_OR_DRIVER_CALL,&param_send_infomation);
			}
			else
			{
				strcpy((char *)param_cmd_info,(char *)&param_network_cmd_info->send_information);
			}
		}
	}
	else if(param_network_cmd_info->send_information.event_type_intercom == D2D_INTERCOMM_EVENT &&
		   param_network_cmd_info->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active ==0)
		{
			if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
			{
				bcu_state.other_bcu_ptt_state =  0;
				NetFinishD2D();

			}
			//else if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)
			//{
			//	bcu_state.other_bcu_ptt_state =  0;

			//	judgeWheteherExitD2DEnterD2P();

			//}
		}
}
void SendPTTStateToPCU()
{///<发送当前BCU的PTT状态给PCU，只有在当前PTT状态与上次检测不同的情况下才会发送
	int current_ptt_state = GetPTTState();
	if(bcu_state.this_bcu_ptt_state != current_ptt_state)
	{
		bcu_state.this_bcu_ptt_state = current_ptt_state;
		common_package_t common_package_bcu_ppt_state;
		common_package_bcu_ppt_state.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
		strcpy(common_package_bcu_ppt_state.src_dev_name,bcu_state.bcu_info.devices_name);
		common_package_bcu_ppt_state.src_dev_number = bcu_state.bcu_info.devices_no;

		strcpy(common_package_bcu_ppt_state.dst_dev_name,"PCU");
		common_package_bcu_ppt_state.dst_dev_number = bcu_state.pcu_request_info.recept_pcu_no;

		common_package_bcu_ppt_state.common_data_u.comm_ptt_state.dst_ptt = bcu_state.this_bcu_ptt_state;
		common_package_bcu_ppt_state.common_data_u.comm_ptt_state.src_ptt = bcu_state.this_bcu_ptt_state;
		diag_printf("!-%d\n",bcu_state.this_bcu_ptt_state);
		int ret = 0,write_counts = 0;
		do
		{
			ret = BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&common_package_bcu_ppt_state,sizeof(common_package_bcu_ppt_state));
			write_counts ++;
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0 && write_counts < 2);
	}
}

void SendCurrentBCUPTTStateToPCU()
{///<发送当前BCU的PTT状态给PCU
	int current_ptt_state = GetPTTState();
	bcu_state.this_bcu_ptt_state = current_ptt_state;
	common_package_t common_package_bcu_ppt_state;
	common_package_bcu_ppt_state.pkg_type = COMMON_PACKAGE_TYPE_PTT_STATE;
	strcpy(common_package_bcu_ppt_state.src_dev_name,bcu_state.bcu_info.devices_name);
	common_package_bcu_ppt_state.src_dev_number = bcu_state.bcu_info.devices_no;

	common_package_bcu_ppt_state.common_data_u.comm_ptt_state.dst_ptt = bcu_state.this_bcu_ptt_state;
	common_package_bcu_ppt_state.common_data_u.comm_ptt_state.src_ptt = bcu_state.this_bcu_ptt_state;
	diag_printf("!-%d\n",bcu_state.this_bcu_ptt_state);
	int ret = 0,write_counts = 0;
	do
	{
		ret = BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&common_package_bcu_ppt_state,sizeof(common_package_bcu_ppt_state));
		write_counts ++;
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0 && write_counts < 2);
}

/*send ptt state to bcu*/
void SendPTTStateToBCU()
{///<发BCU的PTT状态
	int current_ptt_state = GetPTTState();
	if(bcu_state.this_bcu_ptt_state != current_ptt_state)
	{
		bcu_state.this_bcu_ptt_state = current_ptt_state;
		if(bcu_state.this_bcu_ptt_state == 0)//PTT松开
		{
			debug_print(("ppt release to bcu\n"));
			BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_release_to_bcu,sizeof(network_send_package_ptt_release_to_bcu));
		}
		else//PTT摁下
		{
			debug_print(("ppt press to bcu,%d\n",network_send_package_ptt_press_to_bcu.common_data_u.comm_ptt_state.src_ptt));
			BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_press_to_bcu,sizeof(network_send_package_ptt_press_to_bcu));
		}
		bcu_state.this_bcu_ptt_state = current_ptt_state;
	}
}
void SendPTTStateToBCU_Once()
{///<只发一次BCU的PTT状态
	int current_ptt_state = GetPTTState();
		bcu_state.this_bcu_ptt_state = current_ptt_state;
		if(bcu_state.this_bcu_ptt_state == 0)//PTT松开
		{
			debug_print(("ppt release to bcu\n"));
			BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_release_to_bcu,sizeof(network_send_package_ptt_release_to_bcu));
		}
		else//PTT摁下
		{
			debug_print(("ppt press to bcu\n"));
			BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_press_to_bcu,sizeof(network_send_package_ptt_press_to_bcu));
		}
}
/*Acquire other bcu ptt state*/
void ReadOtherBCUPttState()
{///<获取对端BCU的PTT状态
	common_package_t temp_ptt_state;
	if(BlockBufferRead(bcu_state.ppt_state_recv_buffer_id,&temp_ptt_state,sizeof(temp_ptt_state)) > 0)
	{
		debug_print(("!!!!!ReadOtherBCUPttState!!!!!!RRRRRR!!!!!-%d\n",temp_ptt_state.common_data_u.comm_ptt_state.src_ptt));
		bcu_state.other_bcu_ptt_state = temp_ptt_state.common_data_u.comm_ptt_state.src_ptt;
		debug_print(("ReadOtherBCUPttState:other_bcu_ptt_state = %d\n",bcu_state.other_bcu_ptt_state));
	}
	return ;
}


/*Acquire event type*/
int GetEventType(state_process_t *state_process,send_infomation_t screen_cmd)
{
#if 1
    int sig = 0;
	int event_type_ann = screen_cmd.event_type_ann;
	int event_type_intercom = screen_cmd.event_type_intercom;

	debug_print(("event_type_ann = %d,event_type_intercom = %d\n",event_type_ann,event_type_intercom));

	if (event_type_ann != ANN_IDLE )
	{
		switch( event_type_ann )
		{

			case LIVE_ANN_EVENT:
				if( screen_cmd.event_info_ann.live_announce.live_active == 1 )
				{
					sig = EVENT_LIVE_BUTTON_PRESS;
					return sig;
				}
				else
				{
					sig = EVENT_RETURN_BUTTON_PRESS;
					return sig;
				}
				break;
           default:
           	 sig = EVENT_MAX;
           	 break;
		}
	}
	if (event_type_intercom != INTERCOM_IDLE )
	{
		switch(event_type_intercom)
		{
			case D2D_INTERCOMM_EVENT:
				if(state_process->state_id == D2P_INTERCOMM_EVENT)
				{
					if( screen_cmd.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 1 )
					{
						sig = EVENT_TRANSFER_TO_D2D_OR_DRIVER_CALL;
						return sig;
					}
					else
					{
						sig = EVENT_CONFIRM_TRANSFER_TO_D2P;
					}
				}
				else
				{
					if( screen_cmd.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 1 )
					{
						sig = EVENT_PTT_OR_DRIVER_CALL;
					}
					else
					{
						sig = EVENT_CONFIRM_TRANSFER_TO_D2P;
					}
				}
				break;
			case D2P_INTERCOMM_EVENT:
				if( screen_cmd.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1 )
				{
					sig = EVENT_CONFIRM_PCU_CALL;
				}
				else
				{
					sig = EVENT_CONFIRM_RELEASE_PCU;
				}
				break;
			default:
				sig = EVENT_MAX;
				break;
		}
	}
	return sig;
#else
	return 0;
#endif
}

/*BCU send response cmd to BCU*/
void SendCmd(send_infomation_t **send_cmd_info,char *dst_device_name,int dst_device_no)
{
#if 1
	if(send_cmd_info ==  NULL){	diag_printf("SendCmd parmer NULL 1.\n");  return ;}
	if(*send_cmd_info==  NULL){	diag_printf("SendCmd parmer NULL 2.\n");  return ;}
	if((*send_cmd_info)->src_devices_no == 0 && strcmp((*send_cmd_info)->src_devices_name,"") == 0){return ;}
	else
	{

		network_send_package_t network_send_information;
		network_pcu_t temp_send_pcu_network_package;
		(*send_cmd_info)->src_devices_no = bcu_state.bcu_info.devices_no;
		strcpy((*send_cmd_info)->src_devices_name,bcu_state.bcu_info.devices_name);

	    diag_printf("dst:%s,%d\n",dst_device_name,dst_device_no);
		memcpy((void *)&network_send_information.send_information,(void *)&(**send_cmd_info),sizeof(*(*send_cmd_info)));
	    strcpy(network_send_information.dst_devices_name,dst_device_name);
	    network_send_information.dst_devices_no = dst_device_no;
	    int ret = -1,write_counts = 0;


	    if(strcmp(dst_device_name,"PCU")==0)
	   	    {
	   			TransformSendPackage(&temp_send_pcu_network_package,&network_send_information);
	   	do
	   	   {
	   		ret = BlockBufferWrite(bcu_state.cmd_send_buffer_id,&temp_send_pcu_network_package,sizeof(network_send_information));
	   		write_counts ++;
	   		if(ret <= 0)
	   		{
	   			cyg_thread_delay(10);
	   		}
	   		diag_printf("SendCmd:Dst devices name=%s,Dst devices no=%d\n",network_send_information.dst_devices_name,network_send_information.dst_devices_no);
	   	   }while(ret <= 0 && write_counts < 2);

	   	}
	    else{
		   do
		  	   {
		  		ret = BlockBufferWrite(bcu_state.cmd_send_buffer_id,&network_send_information,sizeof(network_send_information));
		  		write_counts ++;
		  		if(ret <= 0)
		  		{
		  			cyg_thread_delay(10);
		  		}
		  	   }while(ret <= 0 && write_counts < 2);
		 diag_printf("SendCmd:Dst devices name=%s,Dst devices no=%d\n",network_send_information.dst_devices_name,network_send_information.dst_devices_no);
		(*send_cmd_info)->src_devices_no = 0;
		strcpy((char *)&(*send_cmd_info)->src_devices_name,"");
		(*send_cmd_info)->event_type_ann = ANN_IDLE;
		(*send_cmd_info)->event_type_intercom = INTERCOM_IDLE;
		memset((void *)&(*send_cmd_info)->event_info_ann,0,sizeof((*send_cmd_info)->event_info_ann));
		memset((void *)&(*send_cmd_info)->event_info_intercom,0,sizeof((*send_cmd_info)->event_info_intercom));
	}
	}
#endif
	return ;
}


void SetThisDevInfo(char name[],unsigned char number)
{
	sprintf(this_device_info,"%s%d",name,number);
}




void ClearProcessPackage(send_infomation_t *p_send_info)
{///<清空状态处理包
#if 1
	if(p_send_info->src_devices_no == 0 && strcmp(p_send_info->src_devices_name,"") == 0)
	{
		return ;
	}
	p_send_info->src_devices_no = 0;
	strcpy((char *)p_send_info->src_devices_name,"");
	p_send_info->event_type_ann = ANN_IDLE;
	p_send_info->event_type_intercom = INTERCOM_IDLE;
	memset((void *)&(p_send_info->event_info_ann),0,sizeof(p_send_info->event_info_ann));
	memset((void *)&(p_send_info->event_info_intercom),0,sizeof(p_send_info->event_info_intercom));
#endif
}
void SendBCUPTTPressStateTOBCU()
{///<发送BCU，本端BCU的PTT 已经按下
#if 1
	debug_print(("This BCU PTT is pressed\n"));
	bcu_state.this_bcu_ptt_state = 1;
	BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_press_to_bcu,sizeof(network_send_package_ptt_press_to_bcu));
#endif
}

void SendBCUPTTReleaseStateTOBCU()
{///<告知BCU，本端BCU的PTT 已经松开
#if 1
	debug_print(("This BCU PTT is released\n"));
	bcu_state.this_bcu_ptt_state = 0;
	BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&network_send_package_ptt_release_to_bcu,sizeof(network_send_package_ptt_release_to_bcu));
#endif
}
void ControlCommomPackageInit(common_package_t *p_to_eamp,common_package_t *p_to_pcu,common_package_t *p_to_bcu,common_package_t *p_to_ccu)
{///<通用控制命令包初始化
#if 1
	p_to_eamp->dst_dev_number = MUL_SYNC_DST_NO;
	strcpy(p_to_eamp->dst_dev_name,"EAMP");
	p_to_eamp->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_eamp->src_dev_name,bcu_state.bcu_info.devices_name);
	p_to_eamp->src_dev_number = bcu_state.bcu_info.devices_no;

	p_to_pcu->dst_dev_number = MUL_SYNC_DST_NO;
	strcpy(p_to_pcu->dst_dev_name,"PCU");
	strcpy(p_to_pcu->common_data_u.ts_dev_volume_info.device_name,"PCU");
	p_to_pcu->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_pcu->src_dev_name,bcu_state.bcu_info.devices_name);
	p_to_pcu->src_dev_number = bcu_state.bcu_info.devices_no;
	p_to_pcu->common_data_u.ts_dev_volume_info.device_no_recept_timeout = 10;

	p_to_bcu->dst_dev_number = bcu_state.opposite_bcu_no;
	strcpy(p_to_bcu->dst_dev_name,"BCU");
	p_to_bcu->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_bcu->src_dev_name,bcu_state.bcu_info.devices_name);
	p_to_bcu->src_dev_number = bcu_state.bcu_info.devices_no;

	p_to_ccu->dst_dev_number = MUL_SYNC_DST_NO;
	strcpy(p_to_ccu->dst_dev_name,"CCU");
	p_to_ccu->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_ccu->src_dev_name,bcu_state.bcu_info.devices_name);
	p_to_ccu->src_dev_number = bcu_state.bcu_info.devices_no;
#endif
}

int GetPTTState()
{
	int ret = 0;
	if(ppt_state_flag >= 2)
	{
		ret =  1;
	}
	return ret;
}



void ShowCurrentTime(int param_no)
{
	struct timeval tp_log;
	gettimeofday(&tp_log,NULL);
	diag_printf("%d:%d.%d\n",param_no,tp_log.tv_sec,(tp_log.tv_usec));
}



void alarm_func_handle_delay_d2d(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	whether_could_be_finished_d2d = 1;
}
void alarm_func_handle_get_out3d5_button_state(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	whether_get_out3d5_button_state = 1;
}

void CreateDelayD2DTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_delay_d2d);
	cyg_alarm_create(counter_handle_delay_d2d,alarm_func_handle_delay_d2d,0,
			          &alarm_handle_delay_d2d,&alarm_object_delay_d2d);
	cyg_alarm_initialize(alarm_handle_delay_d2d,cyg_current_time()+200,200);
	cyg_alarm_disable(alarm_handle_delay_d2d);
}

void CreateGetOut3D5ButtonStateTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_get_out3d5_button_state);
	cyg_alarm_create(counter_handle_get_out3d5_button_state,alarm_func_handle_get_out3d5_button_state,0,
			          &alarm_handle_get_out3d5_button_state,&alarm_object_get_out3d5_button_state);
	cyg_alarm_initialize(alarm_handle_get_out3d5_button_state,cyg_current_time()+5,5);
	cyg_alarm_enable(alarm_handle_get_out3d5_button_state);
}
void StartDelayD2DTimer()
{
	cyg_alarm_initialize(alarm_handle_delay_d2d,cyg_current_time()+200,200);
	cyg_alarm_enable(alarm_handle_delay_d2d);
	whether_could_be_finished_d2d = 0;
}
void StopDelayD2DTimer()
{
	cyg_alarm_disable(alarm_handle_delay_d2d);
	whether_could_be_finished_d2d = 0;
}

void GetOuterButtonState()
{///<获取外部口播按钮状态

			if(bcu_state.live_button_state == 0 &&bcu_state.car_select_count_for_monitor>0)///<enter live
			{
				if(bcu_state.select_monitor_or_ann_page==1&&bcu_state.elsect_car_page==0)

				{
					diag_printf(("enter live\n"));
				   ExchangeToLive();
				   ChangeBtnState(6);
				   ChangeBtnState(8);

				}
			}
			else if(bcu_state.live_button_state == 1)///<exit live
			{

				if(bcu_state.select_monitor_or_ann_page==1&&bcu_state.elsect_car_page==0)
				{
					diag_printf("exit live\n");
					CannelSelectCar();
					RecovoryBtnState(6);
					RecovoryBtnState(8);
					ExitLive();
			     bcu_state.iph_monitor_cur_page =0;
				 bcu_state.select_monitor_or_ann_page = 0;
				 bcu_state.d2p_intercom_page = 0;
				 bcu_state.d2d_intercom_page = 0;
				 bcu_state.elsect_car_page=1;
				 bcu_state.static_page=0;
				 gp_main_file_active_page=gp_select_car_ann_page;
				 wz_window_view->value(gp_main_file_active_page);
				 wz_select_window->activate();
				}

			}
}
void JudegD2PButton()
{
	{
		diag_printf("bcu_state.pcu_request_info.request_number=%d\n",bcu_state.pcu_request_info.request_number);
		diag_printf("bcu_state.d2p_button_state======%d\n",bcu_state.d2p_button_state);
		debug_print(("bcu_state.d2p_button_state = %d\n",bcu_state.d2p_button_state));
		if(bcu_state.d2p_button_state == 0 && bcu_state.pcu_request_info.request_number>0)///< show d2p page
		{

			intercomm_debug_print(("Show D2P page\n"));
			ResetSoundTimer();
			ShowD2Ppage();
			wd_touch_screen->show();
		}
		else if(bcu_state.d2p_button_state == 1 )///<receive d2p
		{
			diag_printf("Recv D2P page\n");
			ResetSoundTimer();
			RecvD2PRequest();
			wd_touch_screen->show();
		}
		else if(bcu_state.d2p_button_state == 2)///<refuse d2p
		{
			diag_printf("Refuse D2P page\n");
			if(bcu_state.bcu_active_intercom_state->state_id == D2P_INTERCOMM_EVENT && is_intercomming_with_pcu == 1)
			{
				CloseAudioSampleTimer();
			}

			RefuseD2PRequest();
			wd_touch_screen->show();

		}
	}
}

void SendCMDToEAMP(int param_current_event_state_id)
{///<发送控制命令给EAMP
	network_send_package_t network_send_package;
	strcpy(network_send_package.dst_devices_name,"EAMP");
	network_send_package.dst_devices_no = MUL_DST_NO;
	network_send_package.send_information.event_type_ann = param_current_event_state_id;
	network_send_package.send_information.event_type_intercom = INTERCOM_IDLE;
	switch(param_current_event_state_id)
	{
		case LIVE_ANN_EVENT:
			network_send_package.send_information.event_info_ann.live_announce.live_active = 0;
			break;
	}
	BlockBufferWrite(bcu_state.cmd_send_buffer_id,&network_send_package,sizeof(network_send_package));
}

void ForceToIdle(int param_current_event_state_id)
{///<强制切换到空闲状态
	switch(param_current_event_state_id)
	{
		case LIVE_ANN_EVENT:
			LiveAnnExit();
			break;
		case EMERG_ANN_EVENT:
			EmergAnnExit();
			break;
		case MANUAL_ANN_EVENT:
			ManualAnnExit();
			break;
		case MIC_3D5_OUTER_EVENT:
			Outer3D5AnnExit();
			break;
	}
}

void RefuseAllPCUReqest()
{///<拒绝所有PCU请求
	network_send_package_t network_send_package;
	strcpy(network_send_package.dst_devices_name,"PCU");
	network_send_package.dst_devices_no = MUL_DST_NO;
	network_send_package.send_information.event_type_ann = ANN_IDLE;
	network_send_package.send_information.event_type_intercom = D2P_INTERCOMM_EVENT;
	network_send_package.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active = 0;
	network_send_package.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
	network_send_package.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = MUL_DST_NO;

	BlockBufferWrite(bcu_state.cmd_send_buffer_id,&network_send_package,sizeof(network_send_package));
}

void ClearAllPCURequestInfo()
{///<清除所有PCU请求信息

	ClearGlobalDeviceInfo();
	bcu_state.pcu_request_info.recept_pcu_no = 0;
	bcu_state.pcu_request_info.refuse_pcu_no = 0;
	bcu_state.pcu_request_info.request_number = 0;
}


void SendToOverOtherBCUD2D()
{///<发送哦命令，是对方BCU 结束D2D状态
	network_send_package_t network_send_package;
	strcpy(network_send_package.dst_devices_name,"BCU");
	network_send_package.dst_devices_no = bcu_state.opposite_bcu_no;
	network_send_package.send_information.event_type_ann = ANN_IDLE;
	network_send_package.send_information.event_type_intercom = D2D_INTERCOMM_EVENT;
	network_send_package.send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
	BlockBufferWrite(bcu_state.cmd_send_buffer_id,&network_send_package,sizeof(network_send_package));
}

void ForceBreakD2DToD2P()
{///<强制打断D2D，返回D2P
	if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)
	{///<当前状态是司机对讲打断乘客对讲
		send_infomation_t d2d_over_package;

		d2d_over_package.src_devices_no = bcu_state.bcu_info.devices_no;
		strcpy(d2d_over_package.src_devices_name,bcu_state.bcu_info.devices_name);

		d2d_over_package.event_type_ann = ANN_IDLE;
		d2d_over_package.event_type_intercom = D2D_INTERCOMM_EVENT;

		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;
		///<结束司机对讲
		UpdataGlobalDeviceInfo(d2d_over_package);/*Update global device information*/

		if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT)//pending
		{///<切换回司机乘客对讲
			send_infomation_t rt_d2d_bk_d2p,*p_d2d_bk_d2p;
			p_d2d_bk_d2p = &rt_d2d_bk_d2p;
			memset((void *)p_d2d_bk_d2p,0x00,sizeof(rt_d2d_bk_d2p));
			p_d2d_bk_d2p->event_type_ann = ANN_IDLE;
			p_d2d_bk_d2p->event_type_intercom = D2P_INTERCOMM_EVENT;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pending = 0;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = bcu_state.pcu_request_info.recept_pcu_no;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = 1;
			p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;
			p_d2d_bk_d2p->src_devices_no = bcu_state.bcu_info.devices_no;
			strcpy((char *)p_d2d_bk_d2p->src_devices_name,(char *)bcu_state.bcu_info.devices_name);
			StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_CONFIRM_TRANSFER_TO_D2P,&rt_d2d_bk_d2p);

			current_is_being_d2d = 0;
			diag_printf("4444\n");
//			AlarmTSToChangeScreen(31);
			ChangeIntercommLabelState(current_is_being_d2d);
			diag_printf("555\n");
		}
	}
	else if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
	{///<当前是司机对讲
		whether_change_d2d_to_idle = 1;
		send_infomation_t d2d_over_package;

		d2d_over_package.src_devices_no = bcu_state.bcu_info.devices_no;
		strcpy(d2d_over_package.src_devices_name,bcu_state.bcu_info.devices_name);

		d2d_over_package.event_type_ann = ANN_IDLE;
		d2d_over_package.event_type_intercom = D2D_INTERCOMM_EVENT;

		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_ppt_state = 0;
		d2d_over_package.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no = bcu_state.opposite_bcu_no;
		///<结束司机对讲
		StateMachineExchange(&bcu_state.bcu_active_intercom_state,EVENT_PTT_RELEASE_AND_DRIVER_RELEASE,&d2d_over_package);
	}
}




void alarm_func_handle_key_info(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	
	whether_ask_key_info = 1;
	whether_send_to_pcu = whether_send_to_pcu + 1;
	key_timer_flag ++;
}

void CreateAskKeyInfoTimer()
{
	
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_key_info);
	
	cyg_alarm_create(counter_handle_key_info,alarm_func_handle_key_info,0,&alarm_handle_key_info,&alarm_object_key_info);
	cyg_alarm_initialize(alarm_handle_key_info,cyg_current_time()+1000,1000);
	cyg_alarm_disable(alarm_handle_key_info);
}
void EnableAskKeyInfoTimer()
{
	diag_printf("EnableAskKeyInfoTimer()\n");

	cyg_alarm_enable(alarm_handle_key_info);
}

int ReadNumWilson()
{///<获取设备号
	return bcu_state.bcu_info.devices_no;
}

//add for simulation TCMS test
/*******auto ann sim 2015-12-25*******/
cyg_handle_t  counter_handle_sim_auto;
cyg_handle_t alarm_handle_sim_auto;
cyg_alarm alarm_object_sim_auto;
send_infomation_t send_to_eamp_for_auto_test;//add by pan 2015-12-16






int TransformIntercomPackage(network_send_package_t *p_recv_network_info)
{

	diag_printf("The %s !\n",__FUNCTION__);

	network_pcu_t *p_temp_pcu_network_package;
	network_send_package_t temp_network_recv_package;
	memcpy(&temp_network_recv_package,p_recv_network_info,sizeof(network_send_package_t));
	char *p = (char *)&temp_network_recv_package;
	p_temp_pcu_network_package = (network_pcu_t *)p;

	if(p_temp_pcu_network_package->event_type_intercom == D2P_INTERCOMM_EVENT && strcmp(p_temp_pcu_network_package->src_devices_name,"PCU") == 0)
	{
		//diag_printf("event_type_intercom=%d---src_devices_name=%s\n",p_temp_pcu_network_package->event_type_intercom,p_temp_pcu_network_package->src_devices_name);
		p_recv_network_info->send_information.src_devices_no = p_temp_pcu_network_package->src_devices_no;
		strncpy(p_recv_network_info->send_information.src_devices_name,p_temp_pcu_network_package->src_devices_name,30);
		p_recv_network_info->send_information.event_ptt_state = p_temp_pcu_network_package->event_ptt_state;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active = p_temp_pcu_network_package->d2p_intercomm_active;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = p_temp_pcu_network_package->d2p_intercomm_bcu_device_no;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = p_temp_pcu_network_package->d2p_intercomm_pcu_device_no;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pending = p_temp_pcu_network_package->d2p_intercomm_pending;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = p_temp_pcu_network_package->d2p_intercomm_request_or_over;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response = p_temp_pcu_network_package->d2p_intercomm_response;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting = p_temp_pcu_network_package->d2p_intercomm_whether_is_connecting;
		p_recv_network_info->send_information.event_type_ann = 254;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.monitor=p_temp_pcu_network_package->monitor;
		p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}

int TransformSendPackage(network_pcu_t *p_temp_pcu_network_package,network_send_package_t *p_recv_network_info)
{
	if(p_recv_network_info->send_information.event_type_intercom == D2P_INTERCOMM_EVENT){
		p_temp_pcu_network_package->event_type_intercom=D2P_INTERCOMM_EVENT;
		p_temp_pcu_network_package->event_type_ann=254;
	}
	if(p_recv_network_info->send_information.event_type_intercom == D2P_MONITOR_EVENT){
		p_temp_pcu_network_package->event_type_ann=254;
		p_temp_pcu_network_package->event_type_intercom=D2P_MONITOR_EVENT;
	}

	strcpy(p_temp_pcu_network_package->dst_devices_name,p_recv_network_info->dst_devices_name);
	p_temp_pcu_network_package->dst_devices_no = p_recv_network_info->dst_devices_no;
	strcpy(p_temp_pcu_network_package->src_devices_name,p_recv_network_info->send_information.src_devices_name);
	p_temp_pcu_network_package->monitor=p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.monitor=0;
	p_temp_pcu_network_package->src_devices_no = p_recv_network_info->send_information.src_devices_no;
	p_temp_pcu_network_package->event_ptt_state = p_recv_network_info->send_information.event_ptt_state;
	p_temp_pcu_network_package->d2p_intercomm_active = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active;
	p_temp_pcu_network_package->d2p_intercomm_bcu_device_no = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no;
	p_temp_pcu_network_package->d2p_intercomm_pcu_device_no = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no;
	p_temp_pcu_network_package->d2p_intercomm_pending = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pending;
	p_temp_pcu_network_package->d2p_intercomm_request_or_over = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response;
	p_temp_pcu_network_package->d2p_intercomm_whether_is_connecting = p_recv_network_info->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting;
	return 0;
}

static int IphRequestInsertLink(const common_big_package_t *p_BigConmInfo_temp )
{
		Node * tempnode = create_node();
		int ret;
		strcpy( tempnode->devices_name, "PCU");
		tempnode->vehicle_number = p_BigConmInfo_temp->common_big_data_u.car_no;
		tempnode->devices_id = p_BigConmInfo_temp->common_big_data_u.iph_requset_no;
		tempnode->current_state = 0;
		tempnode->next = NULL;
		PCURequsthead = insert_list( PCURequsthead, tempnode);
		ret = dispalys( PCURequsthead);//显示请求，返回请求数
		return ret;
}

static int IphUpdateLink(const common_big_package_t *p_BigConmInfo_temp)
{
	int ret=0;
	Node *temp;
	int vn=p_BigConmInfo_temp->common_big_data_u.car_no;
	int iph=p_BigConmInfo_temp->common_big_data_u.iph_receive_no;
	temp=update_list(PCURequsthead,vn,iph,1);
	if( temp == NULL )
	{
		return ret=0 ;
	}

	if(p_BigConmInfo_temp->common_big_data_u.seat_id!=bcu_state.bcu_info.devices_no)
	{
		PCURequsthead = deletes_list( PCURequsthead, p_BigConmInfo_temp->common_big_data_u.iph_refuse_no, p_BigConmInfo_temp->common_big_data_u.car_no);

	  ret= dispalys(PCURequsthead);//显示请求，返回请求数
	}
	 ret= dispalys(PCURequsthead);//显示请求，返回请求

	 return ret;

}


static int IphDeleteLink(const common_big_package_t *p_BigConmInfo_temp)
{
	int ret;
	diag_printf("Over the d2p intercom .\n");

	PCURequsthead = deletes_list( PCURequsthead, p_BigConmInfo_temp->common_big_data_u.iph_refuse_no, p_BigConmInfo_temp->common_big_data_u.car_no);

	ret= dispalys(PCURequsthead);//显示请求，返回请求数

	if(	ret == 0)
	{
		BcuResetPlayAlarmAudioWhenD2pReq();
	}
	return ret;

}

static int BcuRequestInsertLink(const common_big_package_t *p_BigConmInfo_temp )
{
		Node * tempnode = create_node();
		int ret;
		strcpy( tempnode->devices_name, "BCU");
		tempnode->vehicle_number = p_BigConmInfo_temp->common_big_data_u.car_no;
		tempnode->devices_id = p_BigConmInfo_temp->common_big_data_u.bcu_requset_no;
		tempnode->current_state = 0;
		tempnode->next = NULL;
		BCURequsthead = insert_list( BCURequsthead, tempnode);
		ret = dispalys( BCURequsthead);//显示请求，返回请求数
		diag_printf("BCU Request \n");
		return ret;
}
static int BcuDeleteLink(const common_big_package_t *p_BigConmInfo_temp)
{
	int ret;
	diag_printf("Over the d2d intercom .\n");
	BCURequsthead = deletes_list( BCURequsthead, p_BigConmInfo_temp->common_big_data_u.bcu_refuse_no, p_BigConmInfo_temp->common_big_data_u.car_no);
	ret= dispalys(BCURequsthead);//显示请求，返回请求数
	if(	ret == 0)
	{
		bcu_state.bcu_request_number=ret;

		AlarmTSToChangeScreen(9);
		AlarmTSToChangeScreen(12);
	}
	if(ret >0)
	{
		AlarmTSToChangeScreen(9);
	}
	return ret;

}

static int BcuUpdateLink(const common_big_package_t *p_BigConmInfo_temp)
{
	int ret=0;
	Node *temp;
	int vn=p_BigConmInfo_temp->common_big_data_u.car_no;
	int iph=p_BigConmInfo_temp->common_big_data_u.bcu_receive_no;
	temp=update_list(BCURequsthead,vn,iph,1);
	if( temp == NULL )
	{
		return ret=-1 ;
	}
	if(p_BigConmInfo_temp->common_big_data_u.seat_id!=bcu_state.bcu_info.devices_no)
		{
			BCURequsthead = deletes_list( BCURequsthead, p_BigConmInfo_temp->common_big_data_u.iph_refuse_no, p_BigConmInfo_temp->common_big_data_u.car_no);

		  ret= dispalys(BCURequsthead);//显示请求，返回请求数
		}
	ret= dispalys(BCURequsthead);//显示请求，返回请求数
	AlarmTSToChangeScreen(9);
	return ret;

}


int ProbeBigCommPackage(const common_big_package_t *p_BigConmInfo)
{
	int ret=0;
	int i=0,j=0;
	int common_type_package=p_BigConmInfo->pkg_type;
	diag_printf("Probe big package iph_requset_no=%d\n",p_BigConmInfo->common_big_data_u.iph_requset_no);
	diag_printf("Probe big package iph_receive_no=%d\n",p_BigConmInfo->common_big_data_u.iph_receive_no);
	diag_printf("Probe big package iph_refuse_no=%d\n",p_BigConmInfo->common_big_data_u.iph_refuse_no);
	diag_printf("Probe big package type = %d\n",common_type_package);
	switch(common_type_package)
	{
	case 4:

			if(p_BigConmInfo->common_big_data_u.ann_event_flag ==1)
			{
				diag_printf("iscs enter control ann to car.\n");
				if(bcu_state.mic_owner == NONE || bcu_state.mic_owner == ANNOUNCE )
				{
					if(bcu_state.bcu_active_ann_state->state_id != LIVE_ANN_EVENT)
					{
						SetLiveCmd(0);
					}
				}
			}
			if(p_BigConmInfo->common_big_data_u.ann_event_flag==0)
			{
				diag_printf("iscs cannel control ann to car.\n");
			    SetLiveCmd(2);
			}
			ret=4;
			break;
	case 5:
		diag_printf("iscs enter control iph monitor to car.\n");
			for(i=0; i<8; i++)
			{
		diag_printf("enable=%d:no=%d :iph=02x%02x\n", p_BigConmInfo->common_big_data_u.monitor_event_flag,i,p_BigConmInfo->common_big_data_u.iph_select_flag[i]);
				if(p_BigConmInfo->common_big_data_u.iph_select_flag[i]==1&&
				   p_BigConmInfo->common_big_data_u.monitor_event_flag==1)
				{
					G_SetAndClearPakage(1,i+1,&g_iph_pcu);
					MoniortStateMachineExchange(&bcu_state.bcu_active_intercom_state,D2P_MONITOR_EVENT_CALL,&g_iph_pcu);
					break;
				}
				if(p_BigConmInfo->common_big_data_u.iph_select_flag[i]==1 &&
				   p_BigConmInfo->common_big_data_u.monitor_event_flag == 0)
				{
					G_SetAndClearPakage(0,i+1,&g_iph_pcu);
					MoniortStateMachineExchange(&bcu_state.bcu_active_intercom_state,D2P_MONITOR_EXIT_CALL,&g_iph_pcu);
					break;
				}

			}
			ret =5;
			break;
	case 7:
			diag_printf("recv select sync comm big form ocs .\n");
			for(j = 0 ; j < 11 ; ++j)
			{
				if(p_BigConmInfo->common_big_data_u.car_occupy_state[j] == 1)
				{
					bcu_state.car_occupy_state[j] = 1;
				}
			}
			AlarmTSToChangeScreen(33);
		break;
	case 8:
			diag_printf("recv iph intercom request.\n");
			bcu_state.pcu_request_info.request_number=IphRequestInsertLink(p_BigConmInfo);
		break;
	case 9:
				diag_printf("recv iph intercom connecting update.\n");
				bcu_state.pcu_request_info.request_number=IphUpdateLink(p_BigConmInfo);
		break;
	case 10:
		if(bcu_state.bcu_active_intercom_state->state_id == D2P_INTERCOMM_EVENT)
		{
			diag_printf("recv iph intercom refuse.\n");
			bcu_state.pcu_request_info.request_number=IphDeleteLink(p_BigConmInfo);
		}
		break;
	case 11:
		if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE&&
				   bcu_state.bcu_active_ann_state->state_id == ANN_IDLE)
		{
			diag_printf("recv car bcu intercom request.\n");
			bcu_state.bcu_request_number=BcuRequestInsertLink(p_BigConmInfo);
		}
		break;
	case 12:
		if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE||
				bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
		{
			diag_printf("recv car bcu intercom connecting update.\n");
			BcuUpdateLink(p_BigConmInfo);
		}
		break;
	case 13:
		if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE||
		bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
		{
			diag_printf("recv car bcu intercom refuse.\n");
			bcu_state.bcu_request_number=BcuDeleteLink(p_BigConmInfo);
		}
	default:
		diag_printf("no package type\n");
		break;
	}
	return ret;
}
//end of add
