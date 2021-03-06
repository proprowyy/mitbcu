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
	printf("line:%dfunction:%s",__LINE__,__FUNCTION__);
	///<开始或者结束PCU请求提示音
	StartOrBrokeBroadcastPcuRequestAlarmAudioData();
	StartOrBrokeBroadcastBcuRequestAlarmAudioData();
	diag_printf("I am idle intercom enter\n");
	if(send_information_intercomm_idle->event_type_intercom == D2D_INTERCOMM_EVENT)
	{
		diag_printf("send refuse bcu cmd\n");
		SendCmd(&send_information_intercomm_idle,"OCS",230);
	}
	if(send_information_intercomm_idle->event_type_intercom == D2P_INTERCOMM_EVENT)
	{

		SendCmd(&send_information_intercomm_idle,"PCU",bcu_state.pcu_request_info.recept_pcu_no);

	}
	if(send_information_intercomm_idle->event_type_intercom==D2P_MONITOR_EVENT)
	{
		SendCmd(&send_information_intercomm_idle,"PCU",send_information_intercomm_idle->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no);
	}
	ClearAllAudioDataBuffer();

}

/*The exit of intercom idle*/
void IdleIntercomExit()
{
	printf("line:%dfunction:%s",__LINE__,__FUNCTION__);
	SendPTTStateToBCU_Once();
	ClearAllAudioDataBuffer();
	whether_have_begin_broadcast_alarm_audio_data = 0;
}

/*The process of intercom idle*/
void IdleIntercomProcess(send_infomation_t *send_information_intercomm_idle)
{
	if( bcu_state.pcu_request_info.pcu_alarm_playing_again == 1
		&& bcu_state.pcu_request_info.pcu_alarm_playing == 1 )
	{
         bcu_state.pcu_request_info.pcu_alarm_playing_again = 0;
         OpenAudioSampleTimer();
	}

	if( bcu_state.bcu_request_info.bcu_alarm_playing_again == 1&& bcu_state.bcu_request_info.bcu_alarm_playing == 1 )
	{
	    bcu_state.bcu_request_info.bcu_alarm_playing_again = 0;
	    OpenAudioSampleTimer();
	}

	if(whether_have_begin_broadcast_alarm_audio_data == 1 &&
			bcu_state.pcu_request_info.request_number == 0 &&
			bcu_state.bcu_active_ann_state->state_id != LIVE_ANN_EVENT)
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

}

/*The enter of D2D intercom*/
void D2DIntercomEnter(send_infomation_t *send_information_intercomm_d2d)
{
	BcuResetPlayAlarmAudioWhenD2dReq();
	bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);
	bcu_state.d2d_button_state = 1;
	whether_eant_to_delay_finished_d2d = 0;
	ForceBUfferData_wilson();
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
	SendCmd(&send_information_intercomm_d2d,"OCS",230);
	BCU_LED_BUTTON3_ON ;///<点亮CC按钮的灯
	AdjustVolumeAfterCODEC();///<调节BCU的音量
			common_package_t temp_common_package;
			memset(&temp_common_package,0,sizeof (common_package_t));
			strcpy(temp_common_package.src_dev_name,"BCU");
			temp_common_package.src_dev_number=bcu_state.bcu_info.devices_no;
			bcu_call_state[bcu_state.bcu_info.devices_no-1].device_number=bcu_state.bcu_info.devices_no;
			bcu_call_state[bcu_state.bcu_info.devices_no-1].device_state = 1;
			memcpy(&temp_common_package.common_data_u.state,&bcu_call_state[bcu_state.bcu_info.devices_no-1],sizeof(bcu_call_state_t));
			temp_common_package.pkg_type=7;
			int rc= BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&temp_common_package,sizeof(common_package_t));
			if(rc<0)
			{
				printf("send common_package fail\n");
			}
}

/*The exit of D2D intercom*/
void D2DIntercomExit()
{
	bcu_state.d2d_button_state = 0;
	whether_eant_to_delay_finished_d2d= 0;
	CloseAudioSampleTimer();
	begin_to_broadcast_d2d = 0;
	ClearAudioDataDestination();
	bcu_state.mic_owner = NONE;
	bcu_state.other_bcu_ptt_state = 0;
	ClearAllAudioDataBuffer();
	bcu_state.this_bcu_request = 0;
	BCU_LED_BUTTON3_DIS;

				common_package_t temp_common_package;
				memset(&temp_common_package,0,sizeof (common_package_t));
				strcpy(temp_common_package.src_dev_name,"BCU");
				temp_common_package.src_dev_number=bcu_state.bcu_info.devices_no;
				bcu_call_state[bcu_state.bcu_info.devices_no-1].device_number=bcu_state.bcu_info.devices_no;
				bcu_call_state[bcu_state.bcu_info.devices_no-1].device_state = 0;
				memcpy(&temp_common_package.common_data_u.state,&bcu_call_state[bcu_state.bcu_info.devices_no-1],sizeof(bcu_call_state_t));
				temp_common_package.pkg_type=7;
				int rc= BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&temp_common_package,sizeof(common_package_t));
				if(rc<0)
				{
					printf("send common_package fail\n");
				}

}

/*The process of D2D intercom*/
void D2DIntercomProcess(send_infomation_t *send_information)
{
#ifndef CONFIG_TEST_SND_IN_MULTI_THREAD
	SendCmd(&send_information,"BCU",bcu_state.opposite_bcu_no);
#endif
}


void D2PMoniortEnter(send_infomation_t *send_information_moniort_d2p)
{

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
	BCU_LED_BUTTON2_ON;
}

void D2PMoniortExit()
{
	begin_to_broadcast_audio_data = 0;
	ClearAudioDataDestination();
	bcu_state.mic_owner = NONE;
	begin_to_broadcast_d2d = 0;
	CloseAudioSampleTimer();
	ClearAllAudioDataBuffer();
	BCU_LED_BUTTON2_DIS;
}

void D2PMoniortProcess(send_infomation_t *send_information_moniort_d2p)
{
	SendCmd(&send_information_moniort_d2p,"PCU",1);
}
/*The enter of D2P intercom*/
void D2PIntercomEnter(send_infomation_t *send_information_intercomm_d2p)
{
    BcuResetPlayAlarmAudioWhenD2pReq(); ///< add, 0107
    bcu_6d5w_ctrl_wilson(bcu_state.device_volume.d2d_volume);
	bcu_6d5w_ctrl_wilson(4);
	play_audio = 0;
	bcu_state.this_bcu_intercomm_state=8;
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

	common_package_t temp_common_package;

	memset(&temp_common_package,0,sizeof (common_package_t));
	strcpy(temp_common_package.src_dev_name,"BCU");
	temp_common_package.src_dev_number=bcu_state.bcu_info.devices_no;
	bcu_call_state[bcu_state.bcu_info.devices_no-1].device_number=bcu_state.bcu_info.devices_no;
	bcu_call_state[bcu_state.bcu_info.devices_no-1].device_state = 1;
	memcpy(&temp_common_package.common_data_u.state,&bcu_call_state[bcu_state.bcu_info.devices_no-1],sizeof(bcu_call_state_t));
	temp_common_package.pkg_type=7;
	int rc= BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&temp_common_package,sizeof(common_package_t));
	if(rc<0)
	{
		printf("send common_package fail\n");
	}
	SendCmd(&send_information_intercomm_d2p,"PCU",bcu_state.pcu_request_info.recept_pcu_no);
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
	bcu_state.this_bcu_intercomm_state=0;
		common_package_t temp_common_package;
		memset(&temp_common_package,0,sizeof (common_package_t));
		strcpy(temp_common_package.src_dev_name,"BCU");
		temp_common_package.src_dev_number=bcu_state.bcu_info.devices_no;
		bcu_call_state[bcu_state.bcu_info.devices_no-1].device_number=bcu_state.bcu_info.devices_no;
		bcu_call_state[bcu_state.bcu_info.devices_no-1].device_state = 0;
		memcpy(&temp_common_package.common_data_u.state,&bcu_call_state[bcu_state.bcu_info.devices_no-1],sizeof(bcu_call_state_t));
		temp_common_package.pkg_type=7;
		int rc= BlockBufferWrite(bcu_state.comm_cmd_send_buffer_id,&temp_common_package,sizeof(common_package_t));
		if(rc<0)
		{
			printf("send common_package fail\n");
		}
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
{
         if( (bcu_state.pcu_request_info.open_snd_playback==0 && 2==GetSndCurrentMode()) || (bcu_state.pcu_request_info.pcu_alarm_playing==0 && 1==GetSndCurrentMode()) )
         {
        	 //diag_printf("Init, D2pReq, ready: %d, %d, %d \n", GetSndCurrentMode(), bcu_state.pcu_request_info.open_snd_playback,bcu_state.pcu_request_info.pcu_alarm_playing);
                CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
                CharBufferClear(bcu_state.pending_buffer_id);
                SetAudioSampleTimeInterval(20+2); ///< timer = 200ms for Mp3_Thread
                OpenAudioSampleTimer();
                bcu_state.pcu_request_info.pcu_alarm_playing = 1;
                bcu_state.pcu_request_info.open_snd_playback = 1;
         }
	 else if(bcu_state.pcu_request_info.pcu_alarm_playing==1 && 1==GetSndCurrentMode())
	 {
		//diag_printf("Init, D2pReq, reset: %d \n", bcu_state.pcu_request_info.request_number);
		 if( bcu_state.pcu_request_info.request_number ==0 ) {
					 BcuResetPlayAlarmAudioWhenD2pReq();
                 }
	 }

         return 0;
}


void  StartOrBrokeBroadcastPcuRequestAlarmAudioData()
{///<打开或关闭PCU请求提示音的播放
	if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE
	   && bcu_state.bcu_active_ann_state->state_id == ANN_IDLE
	   && bcu_state.bcu_request_number == 0){
			bcu_6d5w_ctrl_wilson(1);
			BcuInitPlayAlarmAudioWhenD2pReq(); ///< add, 0107
		}
}




int BcuResetPlayAlarmAudioWhenD2dReq(void)
{
	bcu_state.bcu_request_info.open_snd_playback = 0;
    bcu_state.bcu_request_info.bcu_alarm_playing_again = 0;
    CloseAudioSampleTimerForPcuAlarm();
    CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
    CharBufferClear(bcu_state.pending_buffer_id);
	if( bcu_state.bcu_request_info.bcu_alarm_playing == 1 )
	{
		bcu_state.bcu_request_info.bcu_alarm_playing = 0;
	}
	if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT)
	{
		OpenAudioSampleTimer();
    }
	return 0;
}

int BcuInitPlayAlarmAudioWhenD2dReq(void)
{

	if( (bcu_state.bcu_request_info.open_snd_playback==0&& 2==GetSndCurrentMode())
	  || (bcu_state.bcu_request_info.bcu_alarm_playing==0&& 1==GetSndCurrentMode()))
	{
		CharBufferClear(bcu_state.alarm_audio_data_buffer_id);
        CharBufferClear(bcu_state.pending_buffer_id);
        SetAudioSampleTimeInterval(20+2); ///< timer = 200ms for Mp3_Thread
        OpenAudioSampleTimer();
        bcu_state.bcu_request_info.bcu_alarm_playing = 1;
        bcu_state.bcu_request_info.open_snd_playback = 1;
    }
	else if(bcu_state.bcu_request_info.bcu_alarm_playing==1 && 1==GetSndCurrentMode())
	{
		if( bcu_state.bcu_request_number ==0 )
		{
			BcuResetPlayAlarmAudioWhenD2dReq();
		}
	}
   return 0;
}


void  StartOrBrokeBroadcastBcuRequestAlarmAudioData()
{///<打开或关闭PCU请求提示音的播放
	if(bcu_state.bcu_active_intercom_state->state_id == INTERCOM_IDLE
			&&   bcu_state.bcu_active_ann_state->state_id == ANN_IDLE )
	{
		bcu_6d5w_ctrl_wilson(1);
		BcuInitPlayAlarmAudioWhenD2dReq(); ///< add, 0107
	}
}




//------------------------------------------------------
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
	if(bcu_state.d2d_page_select_bcu_btn_state == 1)
	{
		JudgeWhetherD2DHaveFinished();/*Judge whether have finish d2d*/
		JudegWhetherRequestD2D();/*Judge whether want to enter D2D state*/
	}
}


