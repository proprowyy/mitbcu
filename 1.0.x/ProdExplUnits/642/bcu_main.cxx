/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： bcu_main.c
 * 文件标识：
 * 摘    要： 广播控制盒主函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2012-08-22
 *
 * 取代版本： V0.0.0
 * 原作者  ： wilson
 * 完成日期：
 */

#include "bcu_main.h"
#include <cyg/hal/plf_io.h>

network_shell_package_t recv_shell_from_network;

static void Init_ocs_select_car_struct(void)
{
	    iph_select = (common_big_package_t *)malloc( sizeof(common_big_package_t));
	    iph_select_intercom=(common_big_package_t *)malloc( sizeof(common_big_package_t));
		memset(iph_select,0,sizeof(common_big_package_t));
		memset(iph_select_intercom,0,sizeof(common_big_package_t));
		memset(&g_MonPcuCmdPakage,0,sizeof(g_MonPcuCmdPakage));
		memset(&g_iph_pcu,0,sizeof(g_MonPcuCmdPakage));
		memset(&bcu_send_infomation,0,sizeof(bcu_send_infomation));

}

extern char pcu_alarm_audio[];
Node *PCURequsthead = NULL;
Node *BCURequsthead = NULL;
common_big_package_t recv_temp_big;
int main(int argc, char **argv)
{
	
	int return_value_of_thread_create = 0;/*the return of thread create*/
	void *return_value_of_join_thread[8];/*the return of thread wait*/
	int return_init_buffer = 0;/*the value of init buffer*/

	/*the declaration of thread attributions*/
	pthread_attr_t attr_of_screen;/*the attribution of touch sreen thread*/
	pthread_attr_t attr_of_network;/*the attribution of network thread*/
	pthread_attr_t attr_of_control;/*the attribution of control thread*/
	pthread_attr_t attr_of_sample_and_play;/*the attribution of mic sample and audio play thread*/
	pthread_attr_t attr_of_failure_statics;/*the attribution of failure statics thread*/
	pthread_attr_t attr_of_gd_sync;/*the attribution of global device sync thread*/
	pthread_attr_t attr_of_demao_thread;
	//****************************//
	/*设备软件版本号登记*/
	SetSoftwareVersion("642-PA-BCU-app-1-20171109","2017-11-09");
	//*********************//
	//初始化选车结构全局变量
	Init_ocs_select_car_struct();
	//*********************//
	//初始化IP所有内容以及其他信息
    InitGlobalArray();
    //*********************//
   /*Initialize the bcu_state*/
    InitBcuAndState();
    //*********************//
	/*Acquire this device number*/
	InitAllDeviceInfo();
	/*Update current device information*/
	UpdateCurrentDeviceInformation();
	/*Acquire this device number*/
	GetOtherDeviceNo();
    /*创建pcu请求链表*/
    PCURequsthead=creat_linked_list();
    BCURequsthead=creat_linked_list();
   /*Initial the mutex*/
	InitMutex();
	/*Initialize the Buffer*/
	return_init_buffer = InitBuffer();
	if(return_init_buffer == -1)
	{
		debug_print(("Create buffer error!\n"));
		pthread_exit(NULL);
	}

	/*Initialize the semaphore*/
	InitSemaphore(&sem_wakeup_bcu_mic_sample);
	InitSemaphore(&sem_demao);

	/*initialize the share variable,which is used to identify the audio data's destination*/
	InitAudioDataDestination();
	bcu_audio_in_ctrl(0);

	/*设置系统默认音量*/
	bcu_state.device_volume.d2d_volume = DEFAULT_INTERCOMM_VOLUME;
	bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);

	/*Lookup the sound card,which is used on bcu*/
	if( LookupSndCard( &bcu_audio_handle ) < 0 )
	{
		diag_printf("LookupSndCard faill !\n");
	}
	/*Open the sound card,the open mode is CYG_SND_PLAYBACK_MODE*/
	OpenSndCard(&bcu_audio_handle,"codec",&current_sounder_mode);
	/*声卡打开之后才能调节声卡的线性音量*/
	AdjustVolumeAfterCODEC();
	/*Initial all device information*/
	InitAllDeviceInfo();
	/*Set the package of PTT state*/
	SetPTTStateInfoPackage();
	Enable6d5();

	/*空白数据，口播按钮松开之后补发，从而及时将采集的口播数据发送给EAMP并且播放出来*/
	blank_audio_data = (char *)malloc((1024 * 1) * sizeof(char));
	memset((void *)blank_audio_data,0xff,(1024 *1));



	ThreadAttributionConfigure(&attr_of_screen,(BCU_PRIORIT),///<触摸屏线程属性值初始化
								thread_stack_of_screen+(1024 * 20),(1024 * 25));

	ThreadAttributionConfigure(&attr_of_network,BCU_PRIORIT,///<线程属性值初始化
						  (thread_stack_of_network + (1024 * 16)),(1024 * 25));
	pthread_attr_setschedpolicy(&attr_of_network,SCHED_FIFO);

	ThreadAttributionConfigure(&attr_of_control,BCU_PRIORIT,///<控制线程属性值初始化
									 (thread_stack_of_control + (1024 * 24)),(1024 * 24));

	diag_printf("%x-%x\n",thread_stack_of_control, (thread_stack_of_control + (1024 * 24)));
	ThreadAttributionConfigure(&attr_of_sample_and_play,BCU_HIGH_MIC_SAMPLE_PRIORITY,///<音频采集与播放线程属性值初始化
									 (thread_stack_of_sample_amd_play + (1024 * 20)),(1024 * 20));

	ThreadAttributionConfigure(&attr_of_failure_statics,BCU_PRIORIT,///<故障统计线程属性值初始化
									 (thread_stack_of_failure_statics + (1024 * 3)),(1024 * 3));

	ThreadAttributionConfigure(&attr_of_gd_sync,BCU_PRIORIT,///<同步线程属性值初始化
									 (thread_stack_of_gd_sync + (1024 * 3)),(1024 * 3));

	ThreadAttributionConfigure(&attr_of_demao_thread,BCU_HIGH_MIC_SAMPLE_PRIORITY+2,///<
									 (thread_stack_of_dameo + (1024 * 3)),(1024 * 3));

	return_value_of_thread_create = pthread_create(&thread_of_demao,
														&attr_of_demao_thread,
														DemaoEntry,
														NULL
														);

	/*Create the thread of touch screen*/
	return_value_of_thread_create = pthread_create(&thread_of_screen,
														&attr_of_screen,
														TouchScreenEntry,
														NULL
														);
	 CYG_TEST_CHECK(return_value_of_thread_create == 0, "touch screen thread returned error");
	 debug_print(("I am the touch screen thread!\n"));

//	 /*Create the thread of network*/
	return_value_of_thread_create = pthread_create(&thread_of_network,
														&attr_of_network,
														NetworkHandleEntry,
														NULL
														);
	 CYG_TEST_CHECK(return_value_of_thread_create == 0,"network thread returned error");
	 debug_print(("I am the network thread\n"));

	 /*Create the thread of control*/
	 return_value_of_thread_create = pthread_create(&thread_of_control,
														 &attr_of_control,
														 SystemControl,
														 NULL
														 );
	 CYG_TEST_CHECK(return_value_of_thread_create == 0,"control thread returned error");
	 debug_print(("I am the control thread!\n"));

//	 /*Create the thread of sample and play*/
	 return_value_of_thread_create = pthread_create(&thread_of_sample_and_play,
														 &attr_of_sample_and_play,
														 BcuMicSampleAndPlayEntry,
														 NULL
														 );
	 CYG_TEST_CHECK(return_value_of_thread_create == 0,"mic get thread returned error");
	 debug_print(("I am the sample and play thread!\n"));

	 /*Create the thread of failure statics*/
	 return_value_of_thread_create = pthread_create(&thread_of_failure_statics,
														 &attr_of_failure_statics,
														 BcuFailureStaticsEntry,
														 NULL
														 );
	 CYG_TEST_CHECK(return_value_of_thread_create == 0,"failure statics thread returned error");
	 debug_print(("I am failure statics thread!\n"));

	 pthread_join(thread_of_network, &return_value_of_join_thread[1] );
	 pthread_join(thread_of_control, &return_value_of_join_thread[2] );
	 pthread_join(thread_of_sample_and_play, &return_value_of_join_thread[3] );
	 pthread_join(thread_of_failure_statics, &return_value_of_join_thread[4] );
	 pthread_join(thread_of_screen, &return_value_of_join_thread[0] );
	 pthread_join(thread_of_demao, &return_value_of_join_thread[7] );


	 debug_print(("I am main function, but I have finished my work now.\n"));

	return 0;
}


void *DemaoEntry(void *arg)
{
	diag_printf("####\n");
	for(;;)
	{
		sem_wait(&sem_demao);
		PthreadPriorityChangeForSchedRr(thread_of_sample_and_play, BCU_PRIORIT-1);
	}
	return NULL;
}

/*the handle function of touch screen thread*/
void *TouchScreenEntry(void *arg)
{
	debug_print(("I am success entry touch screen handle thread!\n"));

	int screen_send_cmd_to_control,ts_dev_vol_info_buffer_id;
	/*触摸屏发送给控制线程的事件命令包*/
	screen_send_cmd_to_control = BlockBufferOpen("bcu-screen-cmd-tx");

	/*触摸屏发送给控制线程的设备音量信息包*/
	ts_dev_vol_info_buffer_id = BlockBufferOpen("bcu-control-recv-ts-info");

	TouchScreenMain(screen_send_cmd_to_control,ts_dev_vol_info_buffer_id);
	return NULL;
}


//网络线程处理函数
void *NetworkHandleEntry(void *arg)
{
	 debug_print(("I am success entry network handle thread!\n"));

	 network_buffer_t network_buffer_info;
	 /*neteork send cmd information buffer*/
	 network_buffer_info.udp_cmd_socket_buffer.udp_cmd_socket_send_buffer = BlockBufferOpen("bcu-network-cmd-tx");
	 /*neteork receive cmd information buffer*/
	 network_buffer_info.udp_cmd_socket_buffer.udp_cmd_socket_recv_buffer = BlockBufferOpen("bcu-network-cmd-rx");

	 /*neteork send audio data buffer*/
	 network_buffer_info.udp_data_socket_buffer.udp_data_socket_send_buffer = CharBufferOpen("bcu-network-audio-tx");
	 /*neteork receive audio data buffer*/
	 network_buffer_info.udp_data_socket_buffer.udp_data_socket_recv_buffer = CharBufferOpen("bcu-network-audio-rx");

	 /*neteork send common cmd information buffer*/
	 network_buffer_info.udp_common_socket_buffer.udp_common_socket_send_buffer = BlockBufferOpen("network-udp-common-send");
	 /*neteork receive common cmd information buffer*/
	 network_buffer_info.udp_common_socket_buffer.udp_common_socket_recv_buffer = BlockBufferOpen("network-udp-common-recv");

	 network_buffer_info.udp_heart_socket_buffer.udp_heart_socket_recv_buffer = BlockBufferOpen("network-udp-heart-recv");
	 network_buffer_info.udp_heart_socket_buffer.udp_heart_socket_send_buffer = BlockBufferOpen("network-udp-heart-send");


	 network_buffer_info.udp_shell_socket_buffer.udp_shell_socket_recv_buffer=BlockBufferOpen("network-udp-shell-recv");
	 network_buffer_info.udp_shell_socket_buffer.udp_shell_socket_send_buffer=BlockBufferOpen("network-udp-shell-send");

	 network_buffer_info.server_udp_socket_buffer.udp_server_socket_recv_buffer=BlockBufferOpen("network-udp-server-recv");
	 network_buffer_info.server_udp_socket_buffer.udp_server_socket_send_buffer=BlockBufferOpen("network-udp-server-send");

	 /*网络线程处理函数*/
	 NetWorkMain(network_buffer_info);
	 return 0;
}
void *SystemControl(void *arg)
{
    int ret = 0;


	debug_print(("I am success entry control handle thread!\n"));

	BCURuleTableInit();/*Initialize the bcu rule tables*/

	BCUControlMainBufferInit();/*initial bcu control thread data buffer*/

	RegisterCommonBufferRecvBuffer();///<注册接受相应命令包缓存区

	CreateD2DHangUpD2PTimeOutTimer();/*create D2D hangup D2P timeout timer*/

	CreateAudioSampleTimer();/*create  audio sample timer*/

	BCUStateInit();/*initialize the original state of bcu*/

	send_infomation_t recv_send_info_from_touch_screen;///<从触摸屏线程接收的命令
	ts_dev_volume_info_t ctrl_recv_from_ts;///<触摸屏音量设置信息命令
	network_send_package_t recv_network_info_from_network;///<从网络上接收的命令

	///<相关命令包的初始化
	ControlCommomPackageInit(&dev_vol_to_eamp,&dev_vol_to_pcu,&dev_vol_to_bcu,&dev_vol_to_ccu);

	int whether_ts_is_running = 0;
	int temp_current_pcu_request_number = 0;

	bcu_state.other_bcu_intercomm_state = INTERCOM_IDLE;

	///<创建同步定时器
	CreateSYNCTimer();

	for(;;)
	{

		///获取所有外部按钮当前状态
		GetPAAllOuterButtonState();
		//发送PA设备同步信号
		DevSyncHandle();

		///<获取对端PPT状态
		ReadOtherBCUPttState();

		/*D2D hangup D2P timeout handle*/
		D2DHangUpD2PTimeOutHandle();

		//司机对讲请求与挂断处理
		///D2DReqAndResponseHandle();
       

			bcu_state.this_bcu_is_active=1;

			if(bcu_state.this_bcu_is_active != 0)
			{
				if(GetLiveExternButtonState() == 1)
				{
					AlarmTSToChangeScreen(16);
				}
			}
			if(GetD2PExternButtonState() == 1)///<D2P外部按钮按下
			{

				AlarmTSToChangeScreen(17);
			}


		///<PCU请求时，改变相关按钮的颜色
		ChangeIntercommButtonColor(&whether_ts_is_running);

		if(BlockBufferRead(bcu_state.comm_server_recv_big_buffer_id,&recv_temp_big,sizeof(common_big_package_t)) > 0){
			diag_printf("recv a big common pakge !\n");

			ProbeBigCommPackage(&recv_temp_big);

			if(bcu_state.bcu_request_number !=0)
			{
				AlarmTSToChangeScreen(9);
				btn_emerg_ann->activate();
			}
			else{

				btn_emerg_ann->deactivate();
			}
			if(bcu_state.pcu_request_info.request_number != 0)
			{
				AlarmTSToChangeScreen(4);
				StartOrBrokeBroadcastPcuRequestAlarmAudioData();
			}
		}
		/*if there have cmd information comes from touch screen thread,we should response it as soon as possible*/
		if(BlockBufferRead(bcu_state.recv_cmd_from_touch,&recv_send_info_from_touch_screen,sizeof(recv_send_info_from_touch_screen)) > 0)
		{///<接收触摸屏线程发送过来的控制命令
							 DisplaySendCmd(recv_send_info_from_touch_screen);
							 ANNStateHangle(&recv_cmd_info_of_ann,&recv_send_info_from_touch_screen);/*Handle announcement event*/
							 PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY-2);
							 PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT+2);
							 PthreadPriorityChangeForSchedRr(thread_of_screen, BCU_PRIORIT+1);
							 IntercomStateHangle(&recv_cmd_info_of_intercom,&recv_send_info_from_touch_screen);/*Handle intercom event*/
							 cyg_thread_delay(20);
							 PthreadPriorityChangeForSchedRr(thread_of_screen, BCU_PRIORIT);
							 PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT);

		}
#if 1
		/*if there have cmd information comes from network thread,we should response it as soon as possible*/
		if(BlockBufferRead(bcu_state.cmd_recv_buffer_id,(void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network))>0 && recv_network_info_from_network.send_information.src_devices_no <= 18)
		{
			/*show the detail of cmd information*/

			if(!TransformIntercomPackage(&recv_network_info_from_network)){diag_printf("error:%d:%s\n",__LINE__,__FUNCTION__);}

				DisplayNetworkCmd(recv_network_info_from_network);

			/*Update global device information*/
			if(strcmp(recv_network_info_from_network.send_information.src_devices_name,"PCU") == 0)
			{
				PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT+2);
				PthreadPriorityChangeForSchedRr(thread_of_screen, BCU_PRIORIT+1);
			}
			//UpdataGlobalDeviceInfo(recv_network_info_from_network.send_information);
			cyg_thread_delay(10);
			if(strcmp(recv_network_info_from_network.send_information.src_devices_name,"PCU") == 0)
			{
				PthreadPriorityChangeForSchedRr(thread_of_screen, BCU_PRIORIT);
				PthreadPriorityChangeForSchedRr(thread_of_control, BCU_PRIORIT);
			}

			if(recv_network_info_from_network.send_information.event_type_intercom == D2D_INTERCOMM_EVENT)
			{

			//	JudgeWhetherHaveD2DRequest(&recv_cmd_info_of_intercom,&recv_network_info_from_network);
			}

		}

#if 1
		if(BlockBufferRead(bcu_state.shell_recv_buffer_id,(void *)&recv_shell_from_network,sizeof(recv_shell_from_network))>0)
				{
					int shell_loop;
					for(shell_loop = 0 ; shell_loop < SHELL_CMD_NUMBER ; shell_loop++)
					{
						if(recv_shell_from_network.cmd == shellcmd[shell_loop].cmd)
						{
							shellcmd[shell_loop].cmd_function(recv_shell_from_network);
							break;
						}
					}
				}
#endif

		//到此已经进入到具体的状态机里面了
		bcu_state.bcu_active_ann_state->process(&recv_cmd_info_of_ann);
		ClearProcessPackage(&recv_cmd_info_of_ann);
		bcu_state.bcu_active_intercom_state->process(&recv_cmd_info_of_intercom);
		ClearProcessPackage(&recv_cmd_info_of_intercom);
#endif

		sched_yield();

	}
	return NULL;
}

int times_of_show_led = 0;
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD
void *BcuMicSampleAndPlayEntry(void *arg)
{//---265
	debug_print(("I am success entry sample and play handle thread!\n"));
	int whether_send_blank_audio_data = 0;
    struct timeval tv_start, tv_end;
    CreateSndEnableTimer();
	for(;;)
	{
		DisableSndEnableTimer();
		sem_wait(&sem_wakeup_bcu_mic_sample);
		EnableSndEnableTimer();
		GetPAAllOuterButtonState2();

		if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT &&
		   bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT &&
		   bcu_state.bcu_info.current_state_ann == LIVE_ANN_EVENT) //d2d??
		{
			//diag_printf("Sample ann audio data !--2\n");
			///<采集口播音频数据
			PutMicSampleDataintoBuffer_EAMP(bcu_audio_handle, bcu_state.audio_data_send_buffer_id );
			if(bcu_state.this_bcu_ptt_state != GetPTTState())
			{
				bcu_state.this_bcu_ptt_state = GetPTTState();
				if(bcu_state.this_bcu_ptt_state == 0)
				{
					whether_send_blank_audio_data = 5;
				}
			}
			///<将口播音频数据写入到模拟总线上
			///<改变网络优先级，确保音频文件能够及时发送出去
			PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY);
			if(whether_send_blank_audio_data > 0 )
			{///<决定是否发送空白数据
				whether_send_blank_audio_data = 0;
				CharBufferWrite(bcu_state.audio_data_send_buffer_id, blank_audio_data, (1 * 1024));
				CharBufferWrite(bcu_state.local_audio_data_recv_buffer_id, blank_audio_data, (1 * 1024));
			}
		}
		else if(bcu_state.bcu_active_intercom_state->state_id == D2P_INTERCOMM_EVENT)
		{


			PutMicSampleDataintoBuffer_D2P(bcu_audio_handle, bcu_state.audio_data_send_buffer_id );
			///<改变网络优先级，确保音频文件能够及时发送出去
			PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY);
			if(begin_to_broadcast_audio_data == 1)
			{
				if(0 == play_audio)
				{
					//diag_printf("play intercomm audio data !--2\n");
					unsigned int current_total_bytes = 0;
					unsigned int current_right_delta = 0;
					CharBufferCurrentReadPointer_temp(bcu_state.audio_data_recv_buffer_id, &current_total_bytes,&current_right_delta);

					if( current_total_bytes >= 1024*1)
					{
						play_audio = 1;
						whe_d2d_souder_have_been_worked = 0;
						whe_d2d_souder_stable = 0;
					}
				}
				if(1 == play_audio){///<播放D2P音频数据
				PlayAudioTwice_D2P(bcu_audio_handle, bcu_state.audio_data_recv_buffer_id, bcu_state.pending_buffer_id);
				}
			}
			///<发送PTT状态给PCU
			SendPTTStateToPCU();
		}

		else if(bcu_state.bcu_active_intercom_state->state_id==D2P_MONITOR_EVENT)
				{
					//diag_printf("play monitir audio data !-1\n");
					PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY);
					if(begin_to_broadcast_audio_data == 1)
					{
						if(0 == play_audio)
						{

							unsigned int current_total_bytes = 0;
							unsigned int current_right_delta = 0;
							CharBufferCurrentReadPointer_temp(bcu_state.audio_data_recv_buffer_id, &current_total_bytes,&current_right_delta);

							if( current_total_bytes >= 1024*1)
							{
							//	diag_printf("play monitir audio data !-2\n");
								play_audio = 1;
								whe_d2d_souder_have_been_worked = 0;
								whe_d2d_souder_stable = 0;
							}
						}
						if(1 == play_audio)
						{///<播放D2P音频数据

							PlayAudioTwice_D2P(bcu_audio_handle, bcu_state.audio_data_recv_buffer_id, bcu_state.pending_buffer_id);

						}
					}

				}

		else if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
		{
			{
				///<采集司机对讲音量数据
				diag_printf("d2d ------------------audio\n");
				PutMicSampleDataintoBuffer_BCU(bcu_audio_handle, bcu_state.audio_data_send_buffer_id );

			}


			///<改变网络优先级，确保音频数据能够及时的发出
			PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY);

			if(play_audio == 0)
			{
				unsigned int current_total_bytes = 0;
				unsigned int current_right_delta = 0;
				CharBufferCurrentReadPointer_temp(bcu_state.audio_data_recv_buffer_id, &current_total_bytes,&current_right_delta);
				if( current_total_bytes >= 1024*3)
				{
					play_audio = 1;
					whe_d2d_souder_have_been_worked = 0;
					whe_d2d_souder_stable = 0;
				}
			}
#if 1
			if(1 == play_audio)
			{
				///<播放司机对讲音频数据
				PlayAudioTwice_D2D(bcu_audio_handle,bcu_state.audio_data_recv_buffer_id,bcu_state.pending_buffer_id);
			}
#endif
		}
		else if(bcu_state.bcu_active_intercom_state->state_id == D2D_HANGUP_D2P_EVENT )
		{
			if(  bcu_state.this_bcu_ptt_state == 1 && bcu_state.other_bcu_ptt_state == 0)
			{
				///<采集司机对讲音量数据
				PutMicSampleDataintoBuffer_BCU(bcu_audio_handle, bcu_state.audio_data_send_buffer_id );

			}


			///<改变网络优先级，确保音频数据能够及时的发出
			PthreadPriorityChangeForSchedRr(thread_of_network, BCU_HIGH_NETWORK_PRIORITY);

			if(play_audio == 0)
			{
				unsigned int current_total_bytes = 0;
				unsigned int current_right_delta = 0;
				CharBufferCurrentReadPointer_temp(bcu_state.audio_data_recv_buffer_id, &current_total_bytes,&current_right_delta);
				if( current_total_bytes >= 1024*3)
				{
					play_audio = 1;
					CharBufferClear(bcu_state.pending_buffer_id);
					MicAdPcmWavheader(bcu_state.pending_buffer_id);
					whe_d2d_souder_have_been_worked = 0;
					whe_d2d_souder_stable = 0;
				}
			}
#if 1
			if(1 == play_audio)
			{
				///<播放司机对讲音频数据
				PlayAudioTwice_D2D(bcu_audio_handle,bcu_state.audio_data_recv_buffer_id,bcu_state.pending_buffer_id);
			}
#endif
		}
		else if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE) ///< ach add, begin, 2014-01-08
		{
			if( bcu_state.pcu_request_info.request_number !=0 )
			{
				//diag_printf("play AlarmAudio audio data !-1\n");
			     if( 1==bcu_state.pcu_request_info.open_snd_playback )
			     {

				   bcu_state.pcu_request_info.open_snd_playback = 2;
				   if( 2==GetSndCurrentMode() )
				   {
						//OpenSndCard(&bcu_audio_handle, "playback");
				   }
				   MicAdPcmWavheader(bcu_state.alarm_audio_data_buffer_id);	 ///<

				   AlarmAudioDataIntoBuffer(bcu_state.alarm_audio_data_buffer_id);

			     }
				 
			     if( bcu_state.pcu_request_info.pcu_alarm_playing==1 )
			     {
					if( -5==PlayAudioTwice(bcu_audio_handle, bcu_state.alarm_audio_data_buffer_id, bcu_state.pending_buffer_id) )
					{

						char tmp_array[64];

						CharBufferBrush(bcu_state.alarm_audio_data_buffer_id);
						CharBufferRead(bcu_state.alarm_audio_data_buffer_id, tmp_array, 48);
						PlayAudioTwice(bcu_audio_handle, bcu_state.alarm_audio_data_buffer_id, bcu_state.pending_buffer_id);
						CharBufferClear(bcu_state.pending_buffer_id);
						bcu_state.pcu_request_info.pcu_alarm_playing_again = 1;
					}
				 }
			}
			else
			{
				if( bcu_state.pcu_request_info.pcu_alarm_playing==1 )
				{
					debug_print(("Bcu,mp3 thread:  close alarm play and reset \n"));
					BcuResetPlayAlarmAudioWhenD2pReq();
				}
			}
		}		

	}
	 return NULL;
}
#endif

void *BcuFailureStaticsEntry(void *arg)
{
	debug_print(("I am success entry failure statics handle thread!\n"));
	udp_heart_socket_buffer_t udp_heart_socket_buffer_info;
	udp_heart_socket_buffer_info.udp_heart_socket_recv_buffer = BlockBufferOpen("network-udp-heart-recv");
	udp_heart_socket_buffer_info.udp_heart_socket_send_buffer = BlockBufferOpen("network-udp-heart-send");

	return NULL;
}
/*---------------------------end of bcu beta 1.1-------------------------------*/
