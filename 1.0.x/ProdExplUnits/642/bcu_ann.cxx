/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： bcu_ann.c
 * 文件标识：
 * 摘    要：The source file of bcu announcement
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "bcu_ann.h"

#define SET_PA_AUDIO 1
#define SET_TALK_AUDIO 0

extern pthread_t thread_of_control;/*the handle of control thread*/
send_infomation_t recv_cmd_info_of_ann,recv_cmd_info_of_intercom;

unsigned char wherther_send_sync_signal = 0;//是否需要发送设备同步信号
state_process_t idle_ann_state =
{
		IdleAnnEnter,
		IdleAnnExit,
		IdleAnnProcess,
		ANN_IDLE
};

state_process_t mic_outer_3d5_ann_state =
{
		Outer3D5AnnEnter,
		Outer3D5AnnExit,
		Outer3D5AnnProcess,
		MIC_3D5_OUTER_EVENT
};

state_process_t auto_annoucement_state =
{
		AutoAnnEnter,
		AutoAnnExit,
		AutoAnnProcess,
		TMS_AUTO_ANN_EVENT
};

state_process_t live_annoucement_state =
{
		LiveAnnEnter,
		LiveAnnExit,
		LiveAnnProcess,
		LIVE_ANN_EVENT
};

state_process_t occ_state =
{
		OCCEnter,
		OCCExit,
		OCCProcess,
		OCC_EVENT
};

state_process_t emergency_annoucement_state =
{
		EmergAnnEnter,
		EmergAnnExit,
		EmergAnnProcess,
		EMERG_ANN_EVENT
};

state_process_t manual_annoucement_state =
{
		ManualAnnEnter,
		ManualAnnExit,
		ManualAnnProcess,
		MANUAL_ANN_EVENT
};

cyg_handle_t  counter_handle_sync;
cyg_handle_t alarm_handle_sync;
cyg_alarm alarm_object_sync;

void IdleAnnEnter(send_infomation_t *send_information_idle)
{
	StartOrBrokeBroadcastPcuRequestAlarmAudioData();
	debug_print(("I am idle ann enter\n"));

	bcu_state.bcu_info.current_state_ann = ANN_IDLE;

	SendCmd(&send_information_idle,"OCS",MUL_DST_NO);
}

void IdleAnnExit()
{
	debug_print(("I am idle ann exit\n"));
;
}

void IdleAnnProcess(send_infomation_t *send_information_idle)
{
	SendCmd(&send_information_idle,"EAMP",MUL_DST_NO);
}

void AutoAnnEnter(send_infomation_t *send_information_tms_auto)
{
	debug_print(("I am auto ann enter\n"));
	RestartSYNCTimer();///<重置同步定时器


	SendCmd(&send_information_tms_auto,"EAMP",MUL_DST_NO);

	/*Start timer*/
//	StartAutoSimulateTimer();
}

void AutoAnnExit()
{
	debug_print(("I am auto ann exit\n"));

	/*Close timer*/
}

void AutoAnnProcess(send_infomation_t *send_information_tms_auto)
{
//	diag_printf("ann process\n");
//	RestartSYNCTimer();///<重置同步定时器
	SendCmd(&send_information_tms_auto,"EAMP",MUL_DST_NO);
}

void LiveAnnEnter(send_infomation_t *send_information_live)
{
    diag_printf("I am live ann enter\n");

	SendCmd(&send_information_live,"OCS",MUL_DST_NO);///<2013-12-3

	bcu_state.live_button_state = 1;

	CloseAudioSampleTimer();

	SetAudioDataDestination("OCS",LIVE_DST_NO);

	bcu_state.mic_owner = ANNOUNCE;

	StartAudioSampleTimer();///<2013-10-24

	bcu_state.bcu_info.current_state_ann = LIVE_ANN_EVENT;

	CharBufferClear(bcu_state.audio_data_send_buffer_id);
	CharBufferClear(bcu_state.pending_buffer_id);
	CharBufferClear(bcu_state.audio_data_recv_buffer_id);
	CharBufferClear(bcu_state.local_audio_data_recv_buffer_id);
	MicAdPcmWavheader(bcu_state.local_audio_data_recv_buffer_id);


#if 1
   	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(F, 24, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(F, 24, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_set( CYGHWR_HAL_KINETIS_PIN(F, 24, 1, KINETIS_PIN_PULLUP) );

   	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(F, 25, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(F, 25, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_set( CYGHWR_HAL_KINETIS_PIN(F, 25, 1, KINETIS_PIN_PULLUP) );


	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_set( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) );

	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_set( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) ); ///< PTB20 = audio_ctrl0
#endif	

	ShowCurrentTime(3);
	BCU_LED_BUTTON1_ON;


}

void LiveAnnExit()
{
	bcu_state.live_button_state = 0;
	diag_printf("I am live ann exit\n");

	bcu_state.mic_owner = NONE;



	CloseAudioSampleTimer();

	ClearAllAudioDataBuffer();//清空发送音频buf

	ClearAudioDataDestination();

	///<BCU 音频输出到扬声器
	bcu_audio_talk_pa_choose(SET_TALK_AUDIO);


	whether_have_begin_broadcast_alarm_audio_data = 0;



	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_clear( CYGHWR_HAL_KINETIS_PIN(A, 11, 1, KINETIS_PIN_PULLUP) ); ///< for d2p
	hal_set_pin_function( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_ddr_out( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) );
	hal_gpio_pin_clear( CYGHWR_HAL_KINETIS_PIN(B, 20, 1, KINETIS_PIN_PULLUP) ); ///< PTB20 = audio_ctrl0

	BCU_LED_BUTTON1_DIS;
}

void LiveAnnProcess(send_infomation_t *send_information_live)
{

	SendCmd(&send_information_live,"EAMP",MUL_DST_NO);

}
void OCCEnter(send_infomation_t *send_information_occ)
{

}
void OCCExit()
{

}
void OCCProcess(send_infomation_t *send_information)
{

}
void EmergAnnEnter(send_infomation_t *send_information_emerg)
{

}

void EmergAnnExit()
{

}

void EmergAnnProcess(send_infomation_t *send_information_emerg)
{

}
void ManualAnnEnter(send_infomation_t *send_information_manual)
{

}
void ManualAnnExit()
{

}

void ManualAnnProcess(send_infomation_t *send_information_manual)
{

}


void Outer3D5AnnEnter(send_infomation_t *send_information_live)
{

}

void Outer3D5AnnExit()
{

}

void Outer3D5AnnProcess(send_infomation_t *send_information_live)
{


}

void alarm_func_handle_sync(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	wherther_send_sync_signal = 1;
	sync_timer_flag++;
}

void CreateSYNCTimer()
{///<创建同步定时器
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_sync);
	cyg_alarm_create(counter_handle_sync,alarm_func_handle_sync,0,&alarm_handle_sync,&alarm_object_sync);
	cyg_alarm_initialize(alarm_handle_sync,cyg_current_time()+400,400);
	cyg_alarm_disable(alarm_handle_sync);
	diag_printf("CreateSYNCTimer\n");
}
void RestartSYNCTimer()
{///<重启同步定时器
	cyg_alarm_initialize(alarm_handle_sync,cyg_current_time()+400,400);
	cyg_alarm_enable(alarm_handle_sync);
	diag_printf("RestartSYNCTimer\n");
}
void CloseSYNCTimer()
{///<关闭同步定时器
	cyg_alarm_disable(alarm_handle_sync);
	diag_printf("CloseSYNCTimer\n");
}

void SendSYNCPackageToEamp()
{///<发送同步信息包给EAMP
	int ret = 0,write_counts = 0;
	network_send_package_t sync_info = {"\0",0};
	sync_info.dst_devices_no =  MUL_DST_NO;
	strcpy((char *)sync_info.dst_devices_name,"EAMP");
	sync_info.send_information.src_devices_no =  bcu_state.bcu_info.devices_no;
	strcpy((char *)sync_info.send_information.src_devices_name,bcu_state.bcu_info.devices_name);
	sync_info.send_information.event_type_ann = AUTO_EAMP_SYNC_SIGNAL;
	sync_info.send_information.event_type_intercom = INTERCOM_IDLE;
//	last_control_flag = control_flag;control_flag = 192;
	do
	{
		ret = BlockBufferWrite(bcu_state.cmd_send_buffer_id,&sync_info,sizeof(sync_info));
		write_counts ++;
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0 && write_counts < 2);

}

void DevSyncHandle()
{

	if(wherther_send_sync_signal == 1)
	{///<当前是否需要发送同步信号

		wherther_send_sync_signal = 0;
		SendSYNCPackageToEamp();
		CloseSYNCTimer();

	}

}

