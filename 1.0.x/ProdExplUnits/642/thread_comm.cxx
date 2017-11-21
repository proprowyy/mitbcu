/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：thread_comm.c
 * 文件标识：
 * 摘    要：控制信息结构体
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：2012-10-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "thread_comm.h"
#include "link.h"
//#include "touch_screen/touch_screen_intercomm.h"

/*The declaration of mutex*/
pthread_mutex_t mutex_of_control_to_network_comm;/*the mutex between control thread and netwoek thread*/
pthread_mutex_t mutex_of_global_device_info;/*the mutex of global info struct*/

/*The declaration of semaphore*/
sem_t  sem_wakeup_bcu_mic_sample;
sem_t sem_demao;

#ifdef CONFIG_TEST_SND_IN_MULTI_THREAD
sem_t  sem_snd_card_self_test;
#endif

//控制线程与网络线程之间的通信结构体:主控线程-->网络线程
volatile threads_communication_type_t control_to_network_communication;

/*All device information*/
volatile global_device_info_state_t global_device_info_state =
{
		InitGlobalInfo,
		UpdateGlobalInfo,
		{{"0"}},
		{{"0"}},

};

/*Initial the mutex*/
void InitMutex()
{
	pthread_mutex_init(&mutex_of_control_to_network_comm,NULL);
	pthread_mutex_init(&mutex_of_global_device_info,NULL);
}

/*Initialize the semaphore*/
void InitSemaphore(sem_t *param_sem)
{
	sem_init(param_sem, 0, 0);
}

/*Initialize the destination of audio data*/
void InitAudioDataDestination(void)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	strcpy((char *)control_to_network_communication.device_name,"");
	control_to_network_communication.device_no = 0;
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Clear the destination of audio data*/
void ClearAudioDataDestination()
{
	InitAudioDataDestination();
}

/*Set the destination of audio data*/
void SetAudioDataDestination(char *device_name, unsigned char device_no)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	strcpy((char *)control_to_network_communication.device_name,device_name);
	control_to_network_communication.device_no = device_no;
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Initial all device information*/
void InitAllDeviceInfo()
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.init();
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Update  device information*/
void UpdataDeviceInformation(send_infomation_t recv_package)
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.setglobalinfo(recv_package);
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Initial the global device information struct*/
void InitGlobalInfo()///<modify by wilson 2014_01_03
{
	int i = 0;
	for(i = 1;i < CONFIG_BCU_NUM + 1;i ++)//bcu
	{
		global_device_info_state.device_bcu[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_bcu[i].devices_name,"BCU");
		global_device_info_state.device_bcu[i].current_state_ann = ANN_IDLE;
		global_device_info_state.device_bcu[i].current_state_intercom = INTERCOM_IDLE;
		memset((void *)&(global_device_info_state.device_bcu[i].event_infomation_ann),0x00,sizeof(global_device_info_state.device_bcu[i].event_infomation_ann));
		memset((void *)&(global_device_info_state.device_bcu[i].event_infomation_intercom),0x00,sizeof(global_device_info_state.device_bcu[i].event_infomation_intercom));
	}

	for(i = 1;i < CONFIG_PECU_NUM + 1;i ++)//pcu
	{
		global_device_info_state.device_pcu[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_pcu[i].devices_name,"PCU");
		global_device_info_state.device_pcu[i].current_state_intercom = INTERCOM_IDLE;
		memset((void *)&(global_device_info_state.device_pcu[i].event_infomation_intercom),0x00,sizeof(global_device_info_state.device_pcu[i].event_infomation_intercom));
	}
	ts_debug_print(("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"));
}

/*Update  global device information struct*/
void UpdateGlobalInfo(send_infomation_t receive_package)
{
	int source_device_type = GetDeviceType(receive_package.src_devices_name,receive_package.event_type_intercom);//The source device
	int source_device_number = receive_package.src_devices_no ;

	switch(source_device_type)
	{
		case TYPE_OF_BCU:
			if(receive_package.event_type_intercom == D2D_INTERCOMM_EVENT && source_device_number <= 2)
			{
				memcpy((void *)&global_device_info_state.device_bcu[source_device_number].event_infomation_intercom,
						(void *)&receive_package.event_info_intercom,sizeof(receive_package.event_info_intercom));

				if(receive_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 1)
				{
					global_device_info_state.device_bcu[source_device_number].current_state_intercom = D2D_INTERCOMM_EVENT;
				}
				else
				{
					global_device_info_state.device_bcu[source_device_number].current_state_intercom = INTERCOM_IDLE;
				}

				if(bcu_state.bcu_active_intercom_state->state_id == D2P_INTERCOMM_EVENT)
				{//Next state shoule be D2D hagnup D2P
					global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = D2D_HANGUP_D2P_EVENT;
					global_device_info_state.device_pcu[bcu_state.pcu_request_info.recept_pcu_no].current_state_intercom = D2P_INTERCOM_PENDING;
				}
				else
				{
					global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = D2D_INTERCOMM_EVENT;
				}
			}
			break;

		case TYPE_OF_PECU:
#if 0
			if(source_device_number<=8){
				if(	receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1
				   &&receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over == 1
				   &&receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_response == 0
				   &&receive_package.event_info_intercom.d2p_intercomm.monitor==0
				   &&receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting ==0){
					      diag_printf("Request the intercom .\n");

										Node * tempnode = create_node();

										strcpy( tempnode->devices_name, receive_package.src_devices_name);

										tempnode->devices_id = receive_package.src_devices_no;

										tempnode->current_state = 0;

										tempnode->next = NULL;

										PCURequsthead = insert_list( PCURequsthead, tempnode);

										bcu_state.pcu_request_info.request_number = dispalys( PCURequsthead);//显示请求，返回请求数

				}

					else if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0){

										diag_printf("Over the intercom .\n");

										PCURequsthead = deletes_list( PCURequsthead, receive_package.src_devices_no, 0);

										//bcu_state.pcu_request_info.request_number = dispalys(PCURequsthead);//显示请求，返回请求数

										if(	bcu_state.pcu_request_info.request_number == 0)
										{
											AlarmTSToChangeScreen(12);
										}

				}

				if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting ==1
				  &&receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no == bcu_state.bcu_info.devices_no){
						if(update_list(PCURequsthead,0,receive_package.src_devices_no,1)==NULL){return ;}
						bcu_state.this_bcu_intercomm_state=D2P_INTERCOMM_EVENT;
						AlarmTSToChangeScreen(6);
						AlarmTSToChangeScreen(5);

				}


			}

#endif

			break;
		default:
			break;
	}
}


/*Set the destination device.which is offer to network thread*/
void SetDSTDeviceNameAndNumber(char *device_name,int device_number)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	control_to_network_communication.device_no = device_number;
	strcpy((char *)&control_to_network_communication.device_name,(char *)device_name);
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Get the destination device's name and number*/
void DSTDeviceNameAndNumber(char *dst_device_name,int *dst_device_number)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	*dst_device_number = control_to_network_communication.device_no;
	strcpy((char *)dst_device_name,(char *)control_to_network_communication.device_name);
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Update global device information*/
void UpdataGlobalDeviceInfo(send_infomation_t received_information_backup)
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.setglobalinfo(received_information_backup);
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

void ClearGlobalDeviceInfo()
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.init();
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Show pcu request*/
void ShowPCURequest()
{
	int i = 1;
	for(i = 1; i < 6; i ++)
	{
		debug_print(("PCU%d = %d,request_sequence = %d\n",i,global_device_info_state.device_pcu[i].event_infomation_intercom.d2p_intercomm.d2p_intercomm_active,
														   global_device_info_state.device_pcu[i].request_sequence));
		ts_debug_print(("PCU%d = %d,request_sequence = %d\n",i,global_device_info_state.device_pcu[i].event_infomation_intercom.d2p_intercomm.d2p_intercomm_active,
																   global_device_info_state.device_pcu[i].request_sequence));
	}
}

/*Copy global struct*/
void ScreenCopyGlobalDeviceInfo(global_device_info_state_t *global_device_info_state_backup)
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	memcpy((void *)global_device_info_state_backup,(void *)&global_device_info_state,sizeof(global_device_info_state));
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Try Copy global struct*/
int TryScreenCopyGlobalDeviceInfo(global_device_info_state_t *global_device_info_state_backup)
{
	if ( pthread_mutex_trylock(&mutex_of_global_device_info) != 0 )
	{
	       return -1;
	}

	if( NULL != global_device_info_state_backup )
	{
		  memcpy((void *)global_device_info_state_backup, (void *)&global_device_info_state, sizeof(global_device_info_state));
	}
	pthread_mutex_unlock(&mutex_of_global_device_info);

	return 0;
}

/*BCU update PCU state*/
void BCUUpdataPCUDeviceInfo(send_infomation_t received_information_backup)
{
	int pcu_no;
	pthread_mutex_lock(&mutex_of_global_device_info);
	pcu_no = received_information_backup.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no;
	global_device_info_state.device_pcu[pcu_no].event_infomation_intercom.d2p_intercomm.d2p_intercomm_active = 0;
	pthread_mutex_unlock(&mutex_of_global_device_info);
}
