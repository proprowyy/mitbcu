/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： bcu_intercom.c
 * 文件标识：
 * 摘    要：The source file of bcu intercom
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "bcu_intercom.h"

/*This timer is used to close the pcu,when driver forget to close the pcu request*/
cyg_handle_t  counter_handle_d2d_hangup_d2p_timeout;
cyg_handle_t alarm_handle_d2d_hangup_d2p_timeout;
cyg_alarm alarm_object_d2d_hangup_d2p_timeout;



int whether_d2d_hangup_d2p_timeout = 0;//司机对讲挂起紧急对讲超时，目前没有使用
int begin_to_broadcast_d2d = 0;//开始播放司机对讲音频数据
unsigned  char  current_is_broadcastting_pcu_request_alarm_audio_data = 0;//当前坐在播放pcu请求提示音
extern cyg_io_handle_t bcu_audio_handle;
extern pthread_t thread_of_control;/*the handle of control thread*/
extern pthread_t thread_of_screen;
state_process_t idle_intercom_state =
{
		IdleIntercomEnter,
		IdleIntercomExit,
		IdleIntercomProcess,
		INTERCOM_IDLE
};
state_process_t  d2d_intercomm_state =
{
		D2DIntercomEnter,
		D2DIntercomExit,
		D2DIntercomProcess,
		D2D_INTERCOMM_EVENT

};
state_process_t  d2p_intercomm_state =
{
		D2PIntercomEnter,
		D2PIntercomExit,
		D2PIntercomProcess,
		D2P_INTERCOMM_EVENT

};
state_process_t d2p_moniort_state=
{
		D2PMoniortEnter,
		D2PMoniortExit,
		D2PMoniortProcess,
		D2P_MONITOR_EVENT


};
state_process_t  intercomm_hang_up_state =
{
		IntercomHangUpEnter,
		IntercomHangUpExit,
		IntercomHangUpProcess,
		D2D_HANGUP_D2P_EVENT
};


/*d2d hangup d2p timeout timer function*/
void alarm_func_handle_d2d_hangup_d2p_timeout(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	whether_d2d_hangup_d2p_timeout = 1;
	return ;
}


/*Create the d2d hangup d2p timeout timer*/
int CreateD2DHangUpD2PTimeOutTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_d2d_hangup_d2p_timeout);
	cyg_alarm_create(counter_handle_d2d_hangup_d2p_timeout,alarm_func_handle_d2d_hangup_d2p_timeout,0,&alarm_handle_d2d_hangup_d2p_timeout,&alarm_object_d2d_hangup_d2p_timeout);
	return 0;
}

/*Delete the d2d hangup d2p timeout timer*/
int DeleteD2DHangUpD2PTimeOutTimer()
{
	cyg_alarm_delete(alarm_handle_d2d_hangup_d2p_timeout);
	return 0;
}


/*Set the d2d hangup d2p timeout timer time interval*/
int SetD2DHangUpD2PTimeOutTimeInterval(int time_interval)
{
	cyg_alarm_initialize(alarm_handle_d2d_hangup_d2p_timeout,cyg_current_time()+time_interval,time_interval);
	return 0;
}

/*Open the d2d hangup d2p timeout timer*/
int OpenD2DHangUpD2PTimeOutTimer()
{
	cyg_alarm_enable(alarm_handle_d2d_hangup_d2p_timeout);
	return 0;
}

/*Close the d2d hangup d2p timeout timer*/
int CloseD2DHangUpD2PTimeOutTimer()
{
	cyg_alarm_disable(alarm_handle_d2d_hangup_d2p_timeout);
	return 0;
}

/*Start the d2d hangup d2p timeout timer*/
void StartD2DHangUpD2PTimeOutTimer()
{
	SetD2DHangUpD2PTimeOutTimeInterval(ALARM_INTERVAL_D2P);
	OpenD2DHangUpD2PTimeOutTimer();
}

/*The enter of intercom idle*/
void IdleIntercomEnter(send_infomation_t *send_information_intercomm_idle)
{

	///<开始或者结束PCU请求提示音
	StartOrBrokeBroadcastPcuRequestAlarmAudioData();
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD
	debug_print(("I am idle intercom enter\n"));

	debug_print(("send_information_intercomm_idle->event_type_intercom = %d\n",send_information_intercomm_idle->event_type_intercom));
	debug_print(("D2D_INTERCOMM_EVENT = %d,D2P_INTERCOMM_EVENT = %d\n",D2D_INTERCOMM_EVENT,D2P_INTERCOMM_EVENT));
	debug_print(("bcu_state.active_pcu_no = %d\n",bcu_state.pcu_request_info.recept_pcu_no));

	if(send_information_intercomm_idle->event_type_intercom == D2P_INTERCOMM_EVENT)
	{
		send_infomation_t a,b,*p,*p1;
		p = &a;
		p1 = &b;
		memcpy((void *)p,(void *)send_information_intercomm_idle,sizeof(*send_information_intercomm_idle));
		memcpy((void *)p1,(void *)send_information_intercomm_idle,sizeof(*send_information_intercomm_idle));
		///<发送相关命令给PCU
		SendCmd(&send_information_intercomm_idle,"PCU",bcu_state.pcu_request_info.recept_pcu_no);

	}
	if(send_information_intercomm_idle->event_type_intercom==22)
	SendCmd(&send_information_intercomm_idle,"PCU",send_information_intercomm_idle->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no);

	ClearAllAudioDataBuffer();
#endif
}

/*The exit of intercom idle*/
void IdleIntercomExit()
{
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD
	///<发送当前PTT状态给BCU
	SendPTTStateToBCU_Once();
	debug_print(("I am idle intercom exit\n"));

	ClearAllAudioDataBuffer();

	whether_have_begin_broadcast_alarm_audio_data = 0;

#endif
}

/*The process of intercom idle*/
void IdleIntercomProcess(send_infomation_t *send_information_intercomm_idle)
{
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD

	if( bcu_state.pcu_request_info.pcu_alarm_playing_again == 1&& bcu_state.pcu_request_info.pcu_alarm_playing == 1 )
	{
                bcu_state.pcu_request_info.pcu_alarm_playing_again = 0;
                OpenAudioSampleTimer();
		debug_print(("0108-idle-pcu: timer \n"));				
	}

	if(whether_have_begin_broadcast_alarm_audio_data == 1 &&
			bcu_state.pcu_request_info.request_number == 0 &&
			bcu_state.bcu_active_ann_state->state_id != OCC_EVENT &&
			bcu_state.bcu_active_ann_state->state_id != LIVE_ANN_EVENT &&
			bcu_state.bcu_active_ann_state->state_id != MIC_3D5_OUTER_EVENT)
	{///<当前状态不需要使用声卡操作，因此关闭声卡相关操作函数
		whether_have_begin_broadcast_alarm_audio_data = 0;
		CloseAudioSampleTimer();///<2013-10-24
		CharBufferClear(bcu_state.pending_buffer_id);
		CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
		bcu_6d5w_ctrl_wilson(0);
	}

	if(bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT &&
	  send_information_intercomm_idle->event_type_intercom == D2P_INTERCOMM_EVENT &&
	   send_information_intercomm_idle->event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0)
	{
		SendCmd(&send_information_intercomm_idle,"PCU",bcu_state.pcu_request_info.refuse_pcu_no);

	}
#endif	
}

/*The enter of D2D intercom*/
void D2DIntercomEnter(send_infomation_t *send_information_intercomm_d2d)
{
	bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);

	debug_print(("I am d2d intercom enter\n"));
	//bcu_audio_in_ctrl(0);
	///<设置当前CC按键状态
	bcu_state.d2d_button_state = 1;

	whether_eant_to_delay_finished_d2d = 0;
	///<关闭监听扬声器


	///<强制清除缓存区数据
	ForceBUfferData_wilson();

	bcu_test_for_ts = 0;

	bcu_state.mic_owner = INTERCOM;

	ClearAllAudioDataBuffer();

	play_audio = 0;
	CharBufferClear(bcu_state.pending_buffer_id);
	CharBufferClear(bcu_state.audio_data_recv_buffer_id);


	MicAdPcmWavheader(bcu_state.audio_data_recv_buffer_id);


	begin_to_broadcast_d2d = 1;
	StartAudioSampleTimer();///<2013-10-24
	debug_print(("0108-d2d-enter: timer \n"));

	bcu_state.other_bcu_ptt_state = 0;
	SendCmd(&send_information_intercomm_d2d,"BCU",bcu_state.opposite_bcu_no);

	BCU_LED_BUTTON3_ON ;///<点亮CC按钮的灯
	AdjustVolumeAfterCODEC();///<调节BCU的音量
}

/*The exit of D2D intercom*/
void D2DIntercomExit()
{
	debug_print(("I am d2d intercom exit\n"));

	bcu_state.d2d_button_state = 0;

	whether_eant_to_delay_finished_d2d= 0;

	///<打开监听扬声器

	bcu_test_for_ts = 0;


	CloseAudioSampleTimer();
	begin_to_broadcast_d2d = 0;

	ClearAudioDataDestination();

	bcu_state.mic_owner = NONE;

	bcu_state.other_bcu_ptt_state = 0;

	ClearAllAudioDataBuffer();



	if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT ||
			bcu_state.bcu_active_ann_state->state_id == OCC_EVENT ||
			bcu_state.bcu_active_ann_state->state_id == MIC_3D5_OUTER_EVENT)
	{
		bcu_state.mic_owner = ANNOUNCE;
		SetAudioDataDestination("EAMP",LIVE_DST_NO);
		StartAudioSampleTimer();
		debug_print(("0108-d2d-exit: timer \n"));		
	}

	bcu_state.this_bcu_request = 0;



	BCU_LED_BUTTON3_DIS;
}

/*The process of D2D intercom*/
void D2DIntercomProcess(send_infomation_t *send_information)
{
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD
	SendCmd(&send_information,"BCU",bcu_state.opposite_bcu_no);
#endif
}


void D2PMoniortEnter(send_infomation_t *send_information_moniort_d2p){


		diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
	    bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);
		bcu_6d5w_ctrl_wilson(4);
		SendCmd(&send_information_moniort_d2p,"PCU",send_information_moniort_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no);
		OpenSndCard(&bcu_audio_handle,"codec",&current_sounder_mode);
		ClearAllAudioDataBuffer();//清空
		CharBufferClear(bcu_state.audio_data_recv_buffer_id);
		MicAdPcmWavheader(bcu_state.audio_data_recv_buffer_id);//写入头;
		play_audio = 0;//打开播放线程中，播放d2p的标志位，这里d2p和监听使用一个播放;
		StartAudioSampleTimer();//打开声卡采样定时器;
		begin_to_broadcast_audio_data = 1;
		begin_to_broadcast_d2d = 1;

}

void D2PMoniortExit()
{
	diag_printf("%d:%s\n",__LINE__,__FUNCTION__);

	begin_to_broadcast_audio_data = 0;

	ClearAudioDataDestination();

	bcu_state.mic_owner = NONE;
	begin_to_broadcast_d2d = 0;

	CloseAudioSampleTimer();

	ClearAllAudioDataBuffer();

}

void D2PMoniortProcess(send_infomation_t *send_information_moniort_d2p){

	//diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
	 SendCmd(&send_information_moniort_d2p,"PCU",1);
}
/*The enter of D2P intercom*/
void D2PIntercomEnter(send_infomation_t *send_information_intercomm_d2p)
{
	diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
    BcuResetPlayAlarmAudioWhenD2pReq(); ///< add, 0107
    bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);
	bcu_6d5w_ctrl_wilson(4);
	play_audio = 0;
	begin_to_broadcast_d2d = 1;
	bcu_state.mic_owner = INTERCOM;
	CharBufferClear(bcu_state.cmd_recv_buffer_id);
	ClearAllAudioDataBuffer();
	CharBufferClear(bcu_state.pending_buffer_id);
	MicAdPcmWavheader(bcu_state.audio_data_recv_buffer_id);
	OpenSndCard(&bcu_audio_handle,"codec",&current_sounder_mode);
	SendCmd(&send_information_intercomm_d2p,"PCU",bcu_state.pcu_request_info.recept_pcu_no);
	begin_to_broadcast_d2d = 1;
	begin_to_broadcast_audio_data = 1;	
	AdjustVolumeAfterCODEC();
	///<发送BCU当前PTT按键状态给PCU
	SendCurrentBCUPTTStateToPCU();
	StartAudioSampleTimer();
	BCU_LED_BUTTON2_ON;
}

/*The exit of D2P intercom*/
void D2PIntercomExit()
{

	diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
	begin_to_broadcast_audio_data = 0;



	ClearAudioDataDestination();

	bcu_state.mic_owner = NONE;

	CloseAudioSampleTimer();

	ClearAllAudioDataBuffer();

	BcuInitPlayAlarmAudioWhenD2pReq();

	BCU_LED_BUTTON2_DIS;


}

/*The process of D2P intercom*/
void D2PIntercomProcess(send_infomation_t *send_information_intercomm_d2p)
{

	SendCmd(&send_information_intercomm_d2p,"PCU",bcu_state.pcu_request_info.recept_pcu_no);
}

/*The enter of D2D hangup D2P intercom*/
void IntercomHangUpEnter(send_infomation_t *send_information_intercomm_hangup)
{
	CharBufferClear(bcu_state.audio_data_send_buffer_id);
	/*need to send a package to current connect pcu*/
	if(bcu_state.pcu_request_info.recept_pcu_no != 0)
	{
		bcu_state.pcu_request_info.hang_up_pcu_no = bcu_state.pcu_request_info.recept_pcu_no;
		send_infomation_t d2d_bk_d2p,*p_d2d_bk_d2p;
		p_d2d_bk_d2p = &d2d_bk_d2p;
		memcpy((void *)p_d2d_bk_d2p,(void *)send_information_intercomm_hangup,sizeof(d2d_bk_d2p));
		p_d2d_bk_d2p->event_type_intercom = D2D_HANGUP_D2P_EVENT;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_active = 1;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pending = 1;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no = bcu_state.bcu_info.devices_no;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no = bcu_state.pcu_request_info.recept_pcu_no;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over = 1;
		p_d2d_bk_d2p->event_info_intercom.d2p_intercomm.d2p_intercomm_response = 0;

		SendCmd(&p_d2d_bk_d2p,"PCU",bcu_state.pcu_request_info.recept_pcu_no);
	}

	play_audio = 0;
	bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);
	debug_print(("I am intercom hang up enter\n"));

//	CloseMonitorSpeaker();
	ClearAllAudioDataBuffer();

	bcu_state.d2d_button_state = 1;
	bcu_state.other_bcu_ptt_state = 0;
	ChangeD2DBreakUpD2PBtnState();

	///<强制清除缓存区数据
	ForceBUfferData_wilson();

	bcu_state.mic_owner = INTERCOM;

	//SetAudioDataDestination("BCU",bcu_state.opposite_bcu_no);
	SetAudioDataDestination("CC",230);

	ClearAllAudioDataBuffer();

	play_audio = 0;
	CharBufferClear(bcu_state.pending_buffer_id);
	CharBufferClear(bcu_state.audio_data_recv_buffer_id);
	CharBufferClear(bcu_state.audio_data_send_buffer_id);


	begin_to_broadcast_d2d = 1;
	CharBufferClear(bcu_state.audio_data_recv_buffer_id);

	StartAudioSampleTimer();

	BCU_LED_BUTTON3_ON ;
}

/*The exit of D2D hangup D2P intercom*/
void IntercomHangUpExit()
{
	debug_print(("I am intercom hang up exit\n"));

	SendToOverOtherBCUD2D();
	begin_to_broadcast_d2d = 0;

	bcu_state.d2d_button_state = 0;

	CloseAudioSampleTimer();

	ClearAudioDataDestination();


	bcu_state.mic_owner = NONE;
	BCU_LED_BUTTON3_DIS;
	bcu_6d5w_ctrl_wilson(0);
}

/*The process of D2D hangup D2P intercom*/
void IntercomHangUpProcess(send_infomation_t *send_information_intercomm_hangup)
{
	SendCmd(&send_information_intercomm_hangup,"BCU",bcu_state.opposite_bcu_no);
}

int BcuResetPlayAlarmAudioWhenD2pReq(void)
{///<重置PCU请求时提示音的播放
        bcu_state.pcu_request_info.open_snd_playback = 0;
        bcu_state.pcu_request_info.pcu_alarm_playing_again = 0;
        CloseAudioSampleTimerForPcuAlarm();
        intercomm_debug_print(("0108-reset: close timer \n"));
        CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
        CharBufferClear(bcu_state.pending_buffer_id);
	if( bcu_state.pcu_request_info.pcu_alarm_playing == 1 )
	{
              bcu_state.pcu_request_info.pcu_alarm_playing = 0;
	}
	intercomm_debug_print(("Reset, D2pReq: %d \n", GetSndCurrentMode() ));
        if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT)
        {
        	OpenAudioSampleTimer();
        }
       return 0;
}
int BcuInitPlayAlarmAudioWhenD2pReq(void)
{///<PCU请求时，初始化播放信息


		intercomm_debug_print(("Init, D2pReq, enter: %d, %d, %d--%d \n", GetSndCurrentMode(), bcu_state.pcu_request_info.open_snd_playback,bcu_state.pcu_request_info.pcu_alarm_playing, bcu_state.pcu_request_info.request_number ));
         if( (bcu_state.pcu_request_info.open_snd_playback==0 && 2==GetSndCurrentMode()) || (bcu_state.pcu_request_info.pcu_alarm_playing==0 && 1==GetSndCurrentMode()) )
         {
        	 diag_printf("Init, D2pReq, ready: %d, %d, %d \n", GetSndCurrentMode(), bcu_state.pcu_request_info.open_snd_playback,bcu_state.pcu_request_info.pcu_alarm_playing);
                CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
                CharBufferClear(bcu_state.pending_buffer_id);
                SetAudioSampleTimeInterval(20+2); ///< timer = 200ms for Mp3_Thread
                OpenAudioSampleTimer();
                bcu_state.pcu_request_info.pcu_alarm_playing = 1;
                bcu_state.pcu_request_info.open_snd_playback = 1;
         }
	 else if(bcu_state.pcu_request_info.pcu_alarm_playing==1 && 1==GetSndCurrentMode())
	 {
		diag_printf("Init, D2pReq, reset: %d \n", bcu_state.pcu_request_info.request_number);
		 if( bcu_state.pcu_request_info.request_number ==0 ) {
					 BcuResetPlayAlarmAudioWhenD2pReq();
                 }
	 }

         return 0;
}


void  StartOrBrokeBroadcastPcuRequestAlarmAudioData()
{///<打开或关闭PCU请求提示音的播放
	if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE
	   && bcu_state.other_bcu_intercomm_state != D2P_INTERCOMM_EVENT
	   && bcu_state.bcu_active_ann_state->state_id == ANN_IDLE ){
			bcu_6d5w_ctrl_wilson(2);
			BcuInitPlayAlarmAudioWhenD2pReq(); ///< add, 0107
		}
}


void AdjustVolumeAfterCODEC()
{///<在CODEC下调节音量
	if(current_sounder_mode != BCU_SND_MODE_CODEC)
	{
		return ;
	}
	if(bcu_state.device_volume.d2d_volume == 4)
	{

		AdjustBCUSNDVolume(1);
	}
	else
	{
		AdjustBCUSNDVolume(1);
	}
}

void D2DReqAndResponseHandle()
{
			JudgeWhetherD2DHaveFinished();/*Judge whether have finish d2d*/
			//JudegWhetherRequestD2D();/*Judge whether want to enter D2D state*/
}


