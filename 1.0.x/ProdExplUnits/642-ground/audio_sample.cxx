/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： audio_sample.c
 * 文件标识：
 * 摘    要：The source file of sample audio
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "audio_sample.h"
#include "alarm_audio_data.h"

cyg_io_handle_t bcu_audio_handle;
unsigned char whether_open_sample_timer = 0;//标记是否打开音频播放与采集定时器

/*This timer is used to sample the audio data*/
cyg_handle_t  counter_handle_sample_audio;
cyg_handle_t alarm_handle_sample_audio;
cyg_alarm alarm_object_sample_audio;

/*确保声卡不死机*/
extern pthread_t thread_of_sample_and_play;
cyg_handle_t  counter_handle_snd_enable;
cyg_handle_t alarm_handle_snd_enable;
cyg_alarm alarm_object_snd_enable;


unsigned char whe_d2d_souder_have_been_worked = 0;//标记声卡是否已经开始工作
unsigned char whe_d2d_souder_stable = 0;//标记声卡是否已经工作稳定
int play_audio = 0;//标记是否可以播放
int begin_to_broadcast_audio_data = 0;//标记是否允许播放

/*audio sample timer function*/
void alarm_func_handle_audio_sample(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	sem_post(&sem_wakeup_bcu_mic_sample);
}

/*Create the audio sample timer*/
int CreateAudioSampleTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_sample_audio);
	cyg_alarm_create(counter_handle_sample_audio,alarm_func_handle_audio_sample,0,&alarm_handle_sample_audio,&alarm_object_sample_audio);
	return 0;
}

/*Delete the audio sample timer*/
int DeleteAudioSampleTimer()
{
	cyg_alarm_delete(alarm_handle_sample_audio);
	return 0;
}

/*Set the audio sample timer time interval*/
int SetAudioSampleTimeInterval(int time_interval)
{

	if(1)
	{
		cyg_alarm_initialize(alarm_handle_sample_audio,cyg_current_time()+time_interval,time_interval);
	}
	else
	{
		cyg_alarm_initialize(alarm_handle_sample_audio,cyg_current_time()+(100),time_interval);
	}

	return 0;
}

/*Open the audio sample timer*/
int OpenAudioSampleTimer()
{
	cyg_alarm_enable(alarm_handle_sample_audio);
	return 0;
}
int CloseAudioSampleTimerForPcuAlarm(void)
{
	cyg_alarm_disable(alarm_handle_sample_audio);	
	return 0;
}

/*Close the audio sample timer*/
int CloseAudioSampleTimer()
{
	diag_printf("CloseAudioSampleTimer\n");
	cyg_alarm_disable(alarm_handle_sample_audio);
	whether_open_sample_timer = 0;
	return 0;
}

/*Start the audio sample timer*/
void StartAudioSampleTimer()
{
	diag_printf("StartAudioSampleTimer\n");
	///<设置音频采集定时器采样周期
	if(bcu_state.bcu_active_intercom_state->state_id == D2P_INTERCOMM_EVENT)
	{
		SetAudioSampleTimeInterval(8);
	}
	else if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT)
	{
		SetAudioSampleTimeInterval(10);
	}
	else if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
	{
		SetAudioSampleTimeInterval(10);
	}
	else
	{
		SetAudioSampleTimeInterval(SAMPLE_AUDIO_FREQUENCE);
	}

	OpenAudioSampleTimer();
	whether_open_sample_timer = 1;
}

void ResetSoundTimer()
{///<重置音频采集定时器
	cyg_alarm_initialize(alarm_handle_sample_audio,cyg_current_time()+150,22);
	OpenAudioSampleTimer();
}

/*play audio data*/
int BcuPlayAudio(cyg_io_handle_t audio_handle, int audio_source_buffer_id)
{
	unsigned int current_total_bytes = 0;
	unsigned int current_right_bytes = 0;	
//	unsigned char *start_address = NULL;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0;

	CharBufferCurrentReadPointer( audio_source_buffer_id, &current_total_bytes, &current_right_bytes);

	if(begin_to_broadcast_d2d == 1 && current_total_bytes < 2 * 1024)
	{
		return 0;
	}
	begin_to_broadcast_d2d = 0;
	if(current_total_bytes < 2 * 1024)
	{
		return 0;
	}
	char temp_buffer[2 * 1024] = "\0";
	current_total_bytes = CharBufferRead(audio_source_buffer_id,temp_buffer,(2 * 1024));
	len = current_total_bytes;
	debug_print(("before: current_total_bytes=%d, len=%d\n", current_total_bytes, len));
	len_backup = len;
	snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM ;
	ret_bwrite = cyg_io_bwrite( audio_handle, (void *)temp_buffer, (cyg_uint32  *)&len, snd_set_local);
	debug_print(("after: bw=%d, len=%d,%d\n", ret_bwrite, len, len_backup));
	return 0;
}
int PlayAudioTwice(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{///<播放音频文件,D2P提示音
	unsigned int current_total_bytes = 0;
	unsigned int main_current_total_bytes = 0;
	unsigned int pending_current_total_bytes = 0;
	unsigned int right_bytes = 0;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0, left_len = 0;
	int ret = 0;
	char tmp_buff[3*1024];
	int tmp_read_len = 0;
	int index = 0;
	int total_read_len = 0;
	unsigned char  buffer_clear_flag  = CharBufferCheckClearFlag(audio_source_buffer_id);
	unsigned char if_read_main_buffer = 0;
	int border_of_audia_data_size = 2234;
	CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
	CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
	//diag_printf("$$$$$ info func %s line %d pend_bytes = %d\n",__FUNCTION__,__LINE__,pending_current_total_bytes);
    current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
    debug_print(("main_current_total_bytes = %d\n",main_current_total_bytes));
    debug_print(("pending_current_total_bytes = %d\n",pending_current_total_bytes));
	if( current_total_bytes <=  border_of_audia_data_size )
	{

		return -5;
	}

	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		 debug_print(("PlayAudio: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
		 return -1;
	}
	if( current_total_bytes > border_of_audia_data_size )
	{
	    ret = 0;
		ret = CharBufferRead(pending_audio_buffer_id, tmp_buff, sizeof(tmp_buff) );
		//diag_printf("$$$$$ info func %s line %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		if( ret >0 )
		{
			tmp_read_len = sizeof(tmp_buff) - ret;
			index = ret;
			total_read_len += ret;
		   if( total_read_len < sizeof(tmp_buff) )
		   {
				 if_read_main_buffer = 1;
		   }
		}
		else if( -2 == ret ) ///< pending is empty
		{
			tmp_read_len = sizeof(tmp_buff);
			index = 0;
			total_read_len = 0;
			if_read_main_buffer = 1;
		}
		else
		{
		   debug_print(("PlayAudioTwice:  read pending=%d, This is unlikely\n", ret));
		   return -6; ///< error
		}
		if( if_read_main_buffer==1)
		{
			ret = 0;
			ret = CharBufferRead(audio_source_buffer_id, &tmp_buff[index], tmp_read_len );
		//	diag_printf("$$$$$ info func %s line %d ret = %d\n",__FUNCTION__,__LINE__,ret);
			if( ret > 0 )
			{
				total_read_len += ret;
			}
		}
		if( total_read_len < border_of_audia_data_size)
		{
			  debug_print(("PlayAudioTwice:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
			  return -6;
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			 debug_print(("PlayAudio: before bwrite, buffer cleared-02\n"));
			 return -1;
		}

		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;
		debug_print(("0108:  Before cyg_io_bwrite,%d \n",len));
		ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		debug_print(("0108: after cyg_io_bwrite \n"));
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			debug_print(("PlayAudioTwice: after bwrite, buffer cleared,so return -1\n"));
			 return -1;
		}
		debug_print(("PlayAudioTwice-0108: bw=%d, len=%d, total=%d, dec=%d, left=%d \n", ret_bwrite, len, current_total_bytes, len_backup - len, current_total_bytes-(len_backup - len) ));

		if( len > len_backup )
		{
			left_len = 0;
		}
		else if ( len >= 0 )
		{
			left_len = len;
		}
		if( left_len > 0 ) ///< write into pending buffer
		{
#if 0
			if(begin_to_broadcast_audio_data == 1)
			{
				CharBufferWrite(pending_audio_buffer_id, &tmp_buff[total_read_len-left_len], left_len );
			}
			else
			{
				CharBufferClear(pending_audio_buffer_id);
			}
			  #else
		   CharBufferWrite(pending_audio_buffer_id, &tmp_buff[total_read_len-left_len], left_len );
			  #endif

		}
		ret = 0; ///< write 2KiB,ok
	}
	else
	{
		ret = -2;
	}
	return ret;
}
int PlayAudioTwice_analog(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{///<播放音频文件,D2P提示音
	unsigned int current_total_bytes = 0;
	unsigned int main_current_total_bytes = 0;
	unsigned int pending_current_total_bytes = 0;
	unsigned int right_bytes = 0;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0, left_len = 0;
	int ret = 0;
	char tmp_buff[3*1024];
	int tmp_read_len = 0;
	int index = 0;
	int total_read_len = 0;
	unsigned char  buffer_clear_flag  = CharBufferCheckClearFlag(audio_source_buffer_id);
	unsigned char if_read_main_buffer = 0;
	int border_of_audia_data_size = 64;
	memset((void *)tmp_buff,0xff,sizeof(tmp_buff));
	CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
	CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
    current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
    debug_print(("main_current_total_bytes = %d\n",main_current_total_bytes));
    debug_print(("pending_current_total_bytes = %d\n",pending_current_total_bytes));
	if( current_total_bytes <=  border_of_audia_data_size )
	{
		return -5;
	}
	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		 debug_print(("PlayAudio: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
		 return -1;
	}

	if( current_total_bytes > border_of_audia_data_size )
	{
	    ret = 0;
		ret = CharBufferRead(pending_audio_buffer_id, tmp_buff, sizeof(tmp_buff) );
		if( ret >0 )
		{
			tmp_read_len = sizeof(tmp_buff) - ret;
			index = ret;
			total_read_len += ret;
		   if( total_read_len < sizeof(tmp_buff) )
		   {
				 if_read_main_buffer = 1;
		   }
		}
		else if( -2 == ret ) ///< pending is empty
		{
			tmp_read_len = sizeof(tmp_buff);
			index = 0;
			total_read_len = 0;
			if_read_main_buffer = 1;
		}
		else
		{
		   debug_print(("PlayAudioTwice:  read pending=%d, This is unlikely\n", ret));
		   return -6; ///< error
		}
		if( if_read_main_buffer==1)
		{
			ret = 0;
			ret = CharBufferRead(audio_source_buffer_id, &tmp_buff[index], tmp_read_len );
			if( ret > 0 )
			{
				total_read_len += ret;
			}
		}
		if( total_read_len < border_of_audia_data_size)
		{
			  debug_print(("PlayAudioTwice:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
			  return -6;
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			 debug_print(("PlayAudio: before bwrite, buffer cleared-02\n"));
			 return -1;
		}

		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;
		debug_print(("0108:  Before cyg_io_bwrite,%d \n",len));
		ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		debug_print(("0108: after cyg_io_bwrite \n"));

		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			debug_print(("PlayAudioTwice: after bwrite, buffer cleared,so return -1\n"));
			 return -1;
		}
		debug_print(("PlayAudioTwice-0108: bw=%d, len=%d, total=%d, dec=%d, left=%d \n", ret_bwrite, len, current_total_bytes, len_backup - len, current_total_bytes-(len_backup - len) ));

		if( len > len_backup )
		{
			left_len = 0;
		}
		else if ( len >= 0 )
		{
			left_len = len;
		}
		if( left_len > 0 ) ///< write into pending buffer
		{
		   CharBufferWrite(pending_audio_buffer_id, &tmp_buff[total_read_len-left_len], left_len );
		}
		ret = 0; ///< write 2KiB,ok
	}
	else
	{
		ret = -2;
	}
	return ret;
}
int PlayAudioTwice_D2P(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{///<D2P播放音频文件
	unsigned int current_total_bytes = 0;
	unsigned int main_current_total_bytes = 0;
	unsigned int pending_current_total_bytes = 0;
	unsigned int right_bytes = 0;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0;
	int left_len = 0;
	int ret = 0;
	char tmp_buff[3*1024];
	int tmp_read_len = 0;
	int index = 0;
	int total_read_len = 0;
	unsigned char  buffer_clear_flag  = CharBufferCheckClearFlag(audio_source_buffer_id);
	unsigned char if_read_main_buffer = 0;
	int border_of_audia_data_size = 64;
	CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
	CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
    current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
    sd_debug_print(("@1current_total_bytes = %d,pending_current_total_bytes = %d\n",current_total_bytes,pending_current_total_bytes));
	if(whe_d2d_souder_have_been_worked >= 10 || whe_d2d_souder_stable == 1)
	{
		whe_d2d_souder_have_been_worked = 0;
		whe_d2d_souder_stable = 1;
		int border_of_audio_data_size = 3000;
		if(current_total_bytes > 3000)
		{
			sd_debug_print(("@2current_total_bytes = %d,pending_current_total_bytes = %d\n",current_total_bytes,pending_current_total_bytes));
			if(main_current_total_bytes >= border_of_audio_data_size)
			{
				CharBufferClear(pending_audio_buffer_id);
			}
			if(main_current_total_bytes - border_of_audio_data_size > 0 &&
					main_current_total_bytes - border_of_audio_data_size < 3024)
			{
				CharBufferRead(audio_source_buffer_id,tmp_buff,main_current_total_bytes - border_of_audio_data_size);
			}
			CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
			CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
			current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
			sd_debug_print(("@3current_total_bytes = %d,pending_current_total_bytes = %d\n",current_total_bytes,pending_current_total_bytes));
		    if( current_total_bytes <=  2232 )
			{
		    	sd_debug_print(("BBBBBBB:%d\n",current_total_bytes));
				return -5;
			}
		}
	}
	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		sd_debug_print(("PlayAudioTwice_D2P: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
		 return -1;
	}

	if( current_total_bytes > border_of_audia_data_size )
	{
		sd_debug_print(("$$$$ func %s line %d ok\n",__FUNCTION__,__LINE__));
	    ret = 0;
		ret = CharBufferRead(pending_audio_buffer_id, tmp_buff, sizeof(tmp_buff) );
		if( ret >0 )
		{
			tmp_read_len = sizeof(tmp_buff) - ret;
			index = ret;
			total_read_len += ret;
		   if( total_read_len < sizeof(tmp_buff) )
		   {
				 if_read_main_buffer = 1;
		   }
		}
		else if( -2 == ret ) ///< pending is empty
		{
			tmp_read_len = sizeof(tmp_buff);
			index = 0;
			total_read_len = 0;
			if_read_main_buffer = 1;
		}
		else
		{
			sd_debug_print(("PlayAudioTwice_D2P:  read pending=%d, This is unlikely\n", ret));
		   return -6; ///< error
		}
		if( if_read_main_buffer==1)
		{
			ret = 0;
			ret = CharBufferRead(audio_source_buffer_id, &tmp_buff[index], tmp_read_len );
			if( ret > 0 )
			{
				total_read_len += ret;
			}
		}

		if( total_read_len < border_of_audia_data_size)
		{
			sd_debug_print(("PlayAudioTwice_D2P:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			sd_debug_print(("PlayAudioTwice_D2P: before bwrite, buffer cleared-02\n"));
			 return -1;
		}
		if(total_read_len < 0)
		{
			total_read_len = 0;
		}
		if(total_read_len < 3000)
		{
			memset((void *)&tmp_buff[total_read_len],0x7f,3000-total_read_len);
		}
		total_read_len = 3000;
		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;
		sd_debug_print(("Before cyg_io_bwrite-PlayAudioTwice_D2P,%d\n",len));
		whe_d2d_souder_have_been_worked ++;
		if(GetPTTState())
		{
			memset(tmp_buff,0x00,sizeof(tmp_buff));
			cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
			return 0;
		}
		ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		sd_debug_print(("Before cyg_io_bwrite\n"));
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			sd_debug_print(("PlayAudioTwice_D2P: after bwrite, buffer cleared,so return -1\n"));
			 return -1;
		}
		int a = current_total_bytes-(len_backup - len) ;
		sd_debug_print(("PlayAudioTwice_D2P: bw=%d, left=%d, total=%d, dec=%d,%d \n", ret_bwrite, len, current_total_bytes, current_total_bytes-(len_backup - len),a));
		if(a < 0)
		{
			//diag_printf("sSSSSSSSSSSSSS\n");
			return 0;
		}
		if ( len >= 0 )
		{
			left_len = current_total_bytes - total_read_len  +  len;
		}
		if( left_len > 0 ) ///< write into pending buffer
		{

		}
		ret = 0; ///< write 2KiB,ok
	}
	else
	{
		ret = -2;
		sd_debug_print(("$$$$ func %s line %d fail\n",__FUNCTION__,__LINE__));
	}

	return ret;
}
int PlayAudioTwice_D2D(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{///<D2D播放音频文件
	unsigned int current_total_bytes = 0;
	unsigned int main_current_total_bytes = 0;
	unsigned int pending_current_total_bytes = 0;
	unsigned int right_bytes = 0;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0, left_len = 0;
	int ret = 0;
	char tmp_buff[3500];
	memset((void *)&tmp_buff[0],0x7f,3500);
	int tmp_read_len = 0;
	int index = 0;
	int total_read_len = 0;
	unsigned char  buffer_clear_flag  = CharBufferCheckClearFlag(audio_source_buffer_id);
	unsigned char if_read_main_buffer = 0;
	int border_of_audia_data_size = 64;
	CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
	CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
    current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
    sd_debug_print(("current_total_bytes = %d,pending_current_total_bytes = %d\n",current_total_bytes,pending_current_total_bytes));
	if(current_total_bytes == 0)
	{
		memset((void *)&tmp_buff[0],0x7f,3500);
		total_read_len = 3500;
		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;
		cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		return -1;
	}
	else if(whe_d2d_souder_have_been_worked >= 10 || whe_d2d_souder_stable == 1)
	{
		whe_d2d_souder_have_been_worked = 0;
		whe_d2d_souder_stable = 1;
		int border_of_audio_data_size = 3000;
		if(current_total_bytes > 3000)
		{
			sd_debug_print(("current_total_bytes = %d,pending_current_total_bytes = %d\n",current_total_bytes,pending_current_total_bytes));
			if(main_current_total_bytes >= border_of_audio_data_size)
			{
				CharBufferClear(pending_audio_buffer_id);
			}
			if(main_current_total_bytes - border_of_audio_data_size > 0 &&
					main_current_total_bytes - border_of_audio_data_size < 3024)
			{
				CharBufferRead(audio_source_buffer_id,tmp_buff,main_current_total_bytes - border_of_audio_data_size);
			}
			CharBufferCurrentReadPointer_temp(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
			CharBufferCurrentReadPointer_temp(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
			current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
		    if( current_total_bytes <=  2232 )
			{
		    	sd_debug_print(("BBBBBBB:%d\n",current_total_bytes));
				return -5;
			}
		}
	}
	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		sd_debug_print(("PlayAudioTwice_D2P: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
		 return -1;
	}
	if( current_total_bytes > border_of_audia_data_size )
	{
	    ret = 0;
		ret = CharBufferRead(pending_audio_buffer_id, tmp_buff, sizeof(tmp_buff) );
		if( ret >0 )
		{
			tmp_read_len = sizeof(tmp_buff) - ret;
			index = ret;
			total_read_len += ret;
		   if( total_read_len < sizeof(tmp_buff) )
		   {
				 if_read_main_buffer = 1;
		   }
		}
		else if( -2 == ret ) ///< pending is empty
		{
			tmp_read_len = sizeof(tmp_buff);
			index = 0;
			total_read_len = 0;
			if_read_main_buffer = 1;
		}
		else
		{
			sd_debug_print(("PlayAudioTwice_D2D:  read pending=%d, This is unlikely\n", ret));
		   return -6; ///< error
		}
		if( if_read_main_buffer==1)
		{
			ret = 0;
			ret = CharBufferRead(audio_source_buffer_id, &tmp_buff[index], tmp_read_len );
			if( ret > 0 )
			{
				total_read_len += ret;
			}
		}
		if( total_read_len < border_of_audia_data_size)
		{
			sd_debug_print(("PlayAudioTwice_D2D:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
			  //return -6;
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			sd_debug_print(("PlayAudioTwice_D2D: before bwrite, buffer cleared-02\n"));
			 return -1;
		}
		 sd_debug_print(("total_read_len = %d\n",total_read_len));
		if(total_read_len < 0)
		{
			total_read_len = 0;
		}
		if(total_read_len < 3072)
		{
			memset((void *)&tmp_buff[total_read_len],0x7f,3072 - total_read_len);
		}
		total_read_len = 3500;
		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;

		sd_debug_print(("Before cyg_io_bwrite-PlayAudioTwice_D2D,%d\n",len));
		whe_d2d_souder_have_been_worked ++;
		ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		sd_debug_print(("Before cyg_io_bwrite\n"));

		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			sd_debug_print(("PlayAudioTwice_D2D: after bwrite, buffer cleared,so return -1\n"));
			 return -1;
		}
		sd_debug_print(("PlayAudioTwice_D2D: bw=%d, left=%d, total=%d, dec=%d \n", ret_bwrite, len, current_total_bytes, current_total_bytes-(len_backup - len)));

		if( len > len_backup )
		{
			left_len = 0;
		}
		else if ( len >= 0 )
		{
			left_len = len;
		}
		if( left_len > 0 ) ///< write into pending buffer
		{
			//   CharBufferWrite(pending_audio_buffer_id, &tmp_buff[total_read_len-left_len], left_len );

		}
		ret = 0; ///< write 2KiB,ok
	}
	else
	{
		ret = -2;
	}
	return ret;
}

int BcuPlayAudio_analog(cyg_io_handle_t audio_handle, int audio_source_buffer_id)
{///<写音频数据到模拟总线上
	unsigned int current_total_bytes = 0;
	unsigned int current_right_bytes = 0;	
//	unsigned char *start_address = NULL;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0;

	CharBufferCurrentReadPointer( audio_source_buffer_id, &current_total_bytes, &current_right_bytes);

	if(begin_to_broadcast_d2d == 1 && current_total_bytes < 1 * 1024)
	{
		return 0;
	}
	begin_to_broadcast_d2d = 0;
	if(current_total_bytes < 1 * 1024)
	{
		return 0;
	}
	char temp_buffer[1 * 1024] = "\0";
	current_total_bytes = CharBufferRead(audio_source_buffer_id,temp_buffer,(1 * 1024));

	len = current_total_bytes;
	debug_print(("before: current_total_bytes=%d, len=%d\n", current_total_bytes, len));


	len_backup = len;
	snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM ;
	ret_bwrite = cyg_io_bwrite( audio_handle, (void *)temp_buffer, (cyg_uint32  *)&len, snd_set_local);

	debug_print(("after: bw=%d, len=%d,%d\n", ret_bwrite, len, len_backup));

	return 0;
}

void PutMicSampleDataintoBuffer_EAMP(cyg_io_handle_t audio_handle, int buffer_id)
{///<口播采集音频数据
	int wr_ret = 0;
	int button_state = 0;
	button_state = GetPTTState();

	cyg_uint8  audio_rx[1024 * 8] = { 0 } ;
	cyg_uint8  *audio_tx = audio_rx;
	cyg_uint32 len = 100,len1  = 100;
	cyg_uint32 len_origin = 0;
	cyg_uint32 snd_set ;
	Cyg_ErrNo stat_rw;

	len = sizeof (snd_set) ;

	snd_set = (1<<8) | CYG_SND_AUDIO_STREAM;
	stat_rw = cyg_io_bread(bcu_audio_handle, (void *)audio_tx, &len, snd_set);
	len1 = len;
	if(button_state == 1)///<PTT按下
	{
		if (stat_rw == -EIO ||stat_rw<0  )
		{
			len = 0 ;
		}

		audio_tx = audio_tx + len;
		audio_tx = audio_rx ;
		len_origin = len;

		if(len > 0)
		{


				wr_ret = CharBufferWrite(buffer_id, audio_tx, len);
			//	wr_ret = CharBufferWrite(bcu_state.local_audio_data_recv_buffer_id, audio_tx, len);

		}
		debug_print(("len = %d,wr_ret = %d\n",len,wr_ret));
		diag_printf("len = %d,wr_ret = %d\n",len,wr_ret);
	}
	else
	{
		len = 0 ;
		audio_tx = audio_rx;
	}
	return ;
}

void PutMicSampleDataintoBuffer_EAMP_Outer(cyg_io_handle_t audio_handle, int buffer_id)
{///<外接3D5音频采集
	int wr_ret = 0;
	cyg_uint8  audio_rx[1024 * 8] = { 0 } ;
	cyg_uint8  *audio_tx = audio_rx;
	cyg_uint32 len =100,len1  = 100;
	cyg_uint32 len_origin = 0;
	cyg_uint32 snd_set ;
	Cyg_ErrNo stat_rw;


	len = sizeof (snd_set) ;
	snd_set = (1<<8) | CYG_SND_AUDIO_STREAM;
	stat_rw = cyg_io_bread(bcu_audio_handle, (void *)audio_tx, &len, snd_set);
	len1 = len;

	if (stat_rw == -EIO ||stat_rw<0  )
	{
		len = 0 ;
	}

	audio_tx = audio_tx + len;
	audio_tx = audio_rx ;
	len_origin = len;
	if(len > 0)
	{
		wr_ret = CharBufferWrite(buffer_id, audio_tx, len);
		wr_ret = CharBufferWrite(bcu_state.local_audio_data_recv_buffer_id, audio_tx, len);
	}
	diag_printf("len = %d,wr_ret = %d\n",len,wr_ret);
	return ;
}

void PutMicSampleDataintoBuffer_D2P(cyg_io_handle_t audio_handle, int buffer_id)
{///<D2P音频采集
	//diag_printf("$$$$$ func %s line %d\n",__FUNCTION__,__LINE__);
	int wr_ret = 0;
	cyg_uint8  audio_rx[ 1024 * 8 ] = { 0 } ;
	cyg_uint8  *audio_tx = audio_rx;
	cyg_uint32 len = 100;
	cyg_uint32 snd_set ;
	Cyg_ErrNo stat_rw;
	audio_tx = audio_rx ;
	snd_set = (1<<8) | CYG_SND_AUDIO_STREAM;
	debug_print(("Read Snd,before:  %d, len=%d \n", stat_rw, len));
	stat_rw = cyg_io_bread(audio_handle ,(void *)audio_tx, &len, snd_set);
	debug_print(("Read Snd,after:  %d, len=%d \n", stat_rw, len));
	if (stat_rw == -EIO ||stat_rw < 0  )
	{
		len = 0 ;
	}
	
	audio_tx = audio_rx ;
	if(GetPTTState())
	wr_ret = CharBufferWrite(buffer_id, audio_tx, len);

	return ;
}
void PutMicSampleDataintoBuffer_BCU(cyg_io_handle_t audio_handle, int buffer_id)
{///<D2D音频采集
	int wr_ret = 0;
	cyg_uint8  audio_rx[ 1024 * 8 ] = { 0 } ;
	cyg_uint8  *audio_tx = audio_rx;
	cyg_uint32 len = 100;
	cyg_uint32 snd_set ;
	Cyg_ErrNo stat_rw;
	audio_tx = audio_rx ;

	snd_set = (1<<8) | CYG_SND_AUDIO_STREAM;
	stat_rw = cyg_io_bread(audio_handle ,(void *)audio_tx, &len, snd_set);
	if (stat_rw == -EIO ||stat_rw < 0  )
	{
		len = 0 ;
	}
	audio_tx = audio_rx ;
	if(GetPTTState())
	 wr_ret = CharBufferWrite(buffer_id, audio_tx, len);
	 sd_debug_print(("wr_ret = %d\n",wr_ret));

	return ;
}

void alarm_func_handle_snd_enable(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	sem_post(&sem_demao);
	deamon_timer_flag ++;
}
int CreateSndEnableTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_snd_enable);
	cyg_alarm_create(counter_handle_snd_enable,alarm_func_handle_snd_enable,0,&alarm_handle_snd_enable,&alarm_object_snd_enable);
	cyg_alarm_initialize(alarm_handle_snd_enable,cyg_current_time()+(500),500);
	cyg_alarm_disable(alarm_handle_snd_enable);
	return 0;
}

int EnableSndEnableTimer()
{
	cyg_alarm_initialize(alarm_handle_snd_enable,cyg_current_time()+(500),500);
	cyg_alarm_enable(alarm_handle_snd_enable);
	return 0;
}
int DisableSndEnableTimer()
{
	cyg_alarm_disable(alarm_handle_snd_enable);
	return 0;
}
