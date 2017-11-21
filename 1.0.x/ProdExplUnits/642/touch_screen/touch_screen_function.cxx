/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_function.cxx
 * 文件标识：
 * 摘    要： 触摸屏功能处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "touch_screen_function.h"

Fl_Button *p_to_btn_first = NULL;//优先级较低按钮指针
Fl_Button *p_to_btn_second = NULL;//优先级中等按钮指针
Fl_Button *p_to_btn_third = NULL;//优先级较高按钮指针
Fl_Group *gp_main_file_active_page;//当前主菜单界面
Fl_Group *p_current_ann_group = NULL;//当前有效广播界面
Fl_Group *p_current_intercomm_group = NULL;//当前有效的对讲界面

char password_buffer[10] = "";//密码缓存区
char monitor_volume_digit_2_string_buffer[10] = "";//监听音量信息显示缓存区
char car_volume_digit_2_string_buffer[10] = "";//广播音量信息显示缓存区
char intercomm_volume_digit_2_string_buffer[10] = "";//紧急对讲音量信息显示缓存区
char d2d_intercomm_volume_digit_2_string_buffer[10] = "";//司机对讲音量信息显示缓存区

unsigned char d2p_reponse_sursor = 1;//选择PCU位置信息
unsigned char whether_intercomm_button_is_active = 0;//紧急对讲按钮是否可以操作
unsigned int gwCurrCarNo=0;

void SetPassword(const int password_number,char param_password_buffer[])
{///<接收用户输入的密码
	int length = (int)strlen(param_password_buffer);
	if(length > 5)
	{
		param_password_buffer[5] = password_number + 48;
	}
	else
	{
		ShowPasswordMask(length + 1);
		sprintf(param_password_buffer,"%s%d",param_password_buffer,password_number);
	}

	if(length == 0)
	{
		ShowTitleOfInputPassword(INPUT_PASSWORD);
	}
	if(length == 5)
	{
		btn_input_password_ok->activate();
	}
}

void ShowPasswordMask(int number)
{///<根据输入的密码在触摸屏上mask显示
	switch(number)
	{
		case 0:
			tile_out_password->label("      ");
			break;
		case 1:
			tile_out_password->label("*     ");
			break;
		case 2:
			tile_out_password->label("**    ");
			break;
		case 3:
			tile_out_password->label("***   ");
			break;
		case 4:	
			tile_out_password->label("****");
			break;
		case 5:
			tile_out_password->label("***** ");
			break;
		case 6:
			tile_out_password->label("******");
			break;
		default:
			tile_out_password->label("!!!!!!");
			break;
	}
}

void ShowTitleOfInputPassword(int param_title_type)
{///<在触摸屏上根据密码是否正确显示相关文字
	switch(param_title_type)
	{
		case PASSWORD_ERROR:
			title_please_input_password->label("");
			tile_out_password->label("");
			title_warning_and_forget_password->label((char *)password_error_name);
			break;
		case FORGET_PASSWORD:
			title_please_input_password->label("");
			tile_out_password->label("");
			title_warning_and_forget_password->label(forget_password_name);
			break;
		case INPUT_PASSWORD:
			title_please_input_password->label((char *)title_please_input_password_name);
			title_warning_and_forget_password->label("");
			break;
		default:
			break;
	}
}

void RecovoryBtnState(int index)
{///<恢复触摸屏下方功能选择按钮
	switch(index)
	{
		case 2:///<手动广播有效

			break;
		case 3:///<紧急广播有效
			btn_emerg_ann->activate();
			btn_emerg_ann->color((Fl_Color)50);
			btn_emerg_ann->redraw();

			if( btn_emerg_ann == p_to_btn_first)
			{
				p_to_btn_second->activate();
				p_to_btn_second->color((Fl_Color)50);
				p_to_btn_second->redraw();

				p_to_btn_third->activate();
				p_to_btn_third->color((Fl_Color)50);
				p_to_btn_third->redraw();
			}
			else if(btn_emerg_ann == p_to_btn_second)
			{
				p_to_btn_third->activate();
				p_to_btn_third->color((Fl_Color)50);
				p_to_btn_third->redraw();
			}
			break;
		case 4:///<口播有效
			btn_live->activate();
			btn_live->color((Fl_Color)50);
			btn_live->redraw();

			if( btn_live == p_to_btn_first)
			{
				p_to_btn_second->activate();
				p_to_btn_second->color((Fl_Color)50);
				p_to_btn_second->redraw();

				p_to_btn_third->activate();
				p_to_btn_third->color((Fl_Color)50);
				p_to_btn_third->redraw();
			}
			else if(btn_live == p_to_btn_second)
			{
				p_to_btn_third->activate();
				p_to_btn_third->color((Fl_Color)50);
				p_to_btn_third->redraw();
			}
			break;
		case 6:

			btn_enter->color((Fl_Color)50);
			btn_enter->redraw();
			break;
		case 7:
					int i;
					for(i=0;i<11;i++){(gp_select_car_ann_page->child(i))->activate();}
					break;

		case 8:
					 lable_state->hide();
					 lable_state->label("提示:请规范操作,谢谢!");
						lable_state->redraw();
						lable_state->show();
						break;
		default:break;
	}
}


void ChangeBtnState(int index)
{///<改变按钮状态
	switch(index)
	  {
		case 1: ///<手动广播与紧急广播按钮有效

			btn_emerg_ann->activate();
			break;
		case 2://manual  ann button
		
			btn_main_file->color((Fl_Color)50);
			btn_main_file->redraw();

			break;
		case 3://emerg ann button
			btn_main_file->color((Fl_Color)50);
			btn_main_file->redraw();
			
			
			btn_emerg_ann->color((Fl_Color)71);
			btn_emerg_ann->redraw();


			break;
		case 4://live button  	
			btn_main_file->color((Fl_Color)50);
			btn_main_file->redraw();
		
			btn_live->color((Fl_Color)71);
			btn_live->redraw();
			
			if( btn_live == p_to_btn_first)
			{
				p_to_btn_second->deactivate();
				p_to_btn_second->color((Fl_Color)38);
				p_to_btn_second->redraw();
				
				p_to_btn_third->deactivate();
				p_to_btn_third->color((Fl_Color)38);
				p_to_btn_third->redraw();
			}
			else if(btn_live == p_to_btn_second)
			{
				p_to_btn_third->deactivate();
				p_to_btn_third->color((Fl_Color)38);
				p_to_btn_third->redraw();
			}
			break;
		case 5://intercomm button
			btn_main_file->color((Fl_Color)50);
			btn_main_file->redraw();
			btn_intercomm->color((Fl_Color)71);
			btn_intercomm->redraw();
			break;
		case 6:
			btn_enter->color((Fl_Color)71);
			btn_enter->redraw();
			break;

		case 7:
			int i;
			for(i=0;i<11;i++){(gp_select_car_ann_page->child(i))->deactivate();}
			break;
		case 8:
			 lable_state->hide();
		       lable_state->label("车地广播中....\n\n\n取消广播后,请取消选车.");
				lable_state->redraw();
				lable_state->show();
				break;

		default:break;
	  }

}


void ExchangeBtnPosition()
{///<根据优先级改变按钮位置
	int max = 0;
	int min = 0;

	bcu_state.event_priority.manual_event_priority = BcuGetAnnStatePriority(MANUAL_ANN_EVENT);
	bcu_state.event_priority.emerg_event_priority = BcuGetAnnStatePriority(EMERG_ANN_EVENT);
	bcu_state.event_priority.live_event_priority = BcuGetAnnStatePriority(LIVE_ANN_EVENT);
		min = bcu_state.event_priority.manual_event_priority > bcu_state.event_priority.emerg_event_priority && bcu_state.event_priority.manual_event_priority > bcu_state.event_priority.live_event_priority ?
			  bcu_state.event_priority.manual_event_priority :(bcu_state.event_priority.emerg_event_priority > bcu_state.event_priority.live_event_priority ? bcu_state.event_priority.emerg_event_priority : bcu_state.event_priority.live_event_priority);
			  
		max = bcu_state.event_priority.manual_event_priority < bcu_state.event_priority.emerg_event_priority && bcu_state.event_priority.manual_event_priority < bcu_state.event_priority.live_event_priority ?
			  bcu_state.event_priority.manual_event_priority :(bcu_state.event_priority.emerg_event_priority < bcu_state.event_priority.live_event_priority ? bcu_state.event_priority.emerg_event_priority : bcu_state.event_priority.live_event_priority);
		

		if( bcu_state.event_priority.emerg_event_priority == max)
		{
			p_to_btn_first = btn_emerg_ann;
		}
		else if( bcu_state.event_priority.emerg_event_priority == min)
		{
			p_to_btn_third = btn_emerg_ann;
		}
		else
		{
			p_to_btn_second = btn_emerg_ann;
		}
		
		if( bcu_state.event_priority.live_event_priority == max)
		{
			p_to_btn_first = btn_live;
		}
		else if( bcu_state.event_priority.live_event_priority == min)
		{
			p_to_btn_third = btn_live;
		}
		else
		{
			p_to_btn_second = btn_live;
		}
		
	//	p_to_btn_first->position(172,397);
		p_to_btn_first->position(492,397);
		
		p_to_btn_second->position(332,397);
		
	//	p_to_btn_third->position(492,397);
		p_to_btn_third->position(172,397);
}









void BCUActivejudge()
{///<BCU界面是否有效判断
	if(bcu_state.this_bcu_is_active == 1)
	{
		wz_select_window->activate();
		wz_window_view->activate();
		wz_window_view->value(gp_static_show);
		wz_select_window->value(main_group);

	}
	else
	{
		switch(bcu_state.bcu_active_ann_state->state_id)
		{
			case ANN_IDLE:
					RecoverIdleButtonState();
					ShowMainPage();
				break;
			case LIVE_ANN_EVENT:
					ExitLive();///<change touch screen
					ShowMainPage();
				BlockBufferClear(bcu_state.recv_cmd_from_touch);

				bcu_state.bcu_active_ann_state->exit();
				bcu_state.bcu_active_ann_state = &idle_ann_state;
				bcu_state.bcu_active_ann_state->enter(NULL);

				SendCMDToEAMP(LIVE_ANN_EVENT);
				break;
		}

		wz_select_window->deactivate();
		wz_window_view->deactivate();
	}
	if(bcu_state.bcu_active_intercom_state->state_id == D2D_INTERCOMM_EVENT)
	{
		play_audio = 0;
		whe_d2d_souder_have_been_worked = 0;
		whe_d2d_souder_stable = 0;
	}

}
void ShowMainPage()
{///<显示主菜单界面
	btn_main_file->color((Fl_Color)50);
	gp_main_file_active_page->hide();
	wz_window_view->value(gp_static_show);
}



void UpdatePCUInfo()
{///<更新PCU信息-根据数据库信息初始化
	//InitPCURequestTable();
	bcu_state.pcu_request_info.recept_pcu_no = 0;
}

void SystemInitBeforeRun()
{///<触摸屏在获取数据库信息之前初始化相关界面信息
	BcuSimulateStateInit();
	TouchScreenInit();///<触摸屏界面初始化
	ShowButtonName(bcu_state.system_language,3);

	p_current_intercomm_group = gp_main_file;
	p_current_ann_group = gp_main_file;

	memset(CarNumberSelectAndRecordBtn,0,sizeof(CarNumberSelectAndRecordBtn));

	ShowDeviceVolumeInfo(255);
}


void ShowTSSkipStationInfo()
{
	int i = 0;
	for(i = 1; i <= bcu_state.region_station_info.current_region_station[0];i++)
	{

	}

}


void ForceExitLatestState()
{///<强制退出当前状态
	send_infomation_t temp_send_infomation;
	temp_send_infomation.src_devices_no = bcu_state.bcu_info.devices_no;
	strcpy((char *)temp_send_infomation.src_devices_name,bcu_state.bcu_info.devices_name);
	temp_send_infomation.event_type_ann = current_arbitrate_state_id;
	temp_send_infomation.event_type_intercom = INTERCOM_IDLE;

	 if(current_arbitrate_state_id == LIVE_ANN_EVENT)
	{
		temp_send_infomation.event_info_ann.live_announce.live_active = 0;
		temp_send_infomation.event_info_ann.live_announce.live_begin_or_over = 0;
		temp_send_infomation.event_info_ann.live_announce.live_response = 0;
	}

	bcu_state.bcu_active_ann_state->exit();
	bcu_state.bcu_active_ann_state = &idle_ann_state;
	bcu_state.bcu_active_ann_state->enter(&temp_send_infomation);
}

void ForceExitLiveStateBeforeIntercomm()
{///<强制退出当前状态
	send_infomation_t temp_send_infomation;
	temp_send_infomation.src_devices_no = bcu_state.bcu_info.devices_no;
	strcpy((char *)temp_send_infomation.src_devices_name,bcu_state.bcu_info.devices_name);
	temp_send_infomation.event_type_ann = LIVE_ANN_EVENT;
	temp_send_infomation.event_type_intercom = INTERCOM_IDLE;

	temp_send_infomation.event_info_ann.live_announce.live_active = 0;
	temp_send_infomation.event_info_ann.live_announce.live_begin_or_over = 0;
	temp_send_infomation.event_info_ann.live_announce.live_response = 0;

	bcu_state.bcu_active_ann_state->exit();
	bcu_state.bcu_active_ann_state = &idle_ann_state;
	bcu_state.bcu_active_ann_state->enter(&temp_send_infomation);
}
void ForceIntoD2D()
{///<强制切换到D2D
	ts_debug_print(("ForceIntoD2D\n"));
	send_infomation_t temp_send_infomation;
	temp_send_infomation.src_devices_no = 0;
	strcpy((char *)temp_send_infomation.src_devices_name,"");
	ts_debug_print(("active intercomm id = %d\n",bcu_state.bcu_active_intercom_state->state_id));
	bcu_state.bcu_active_intercom_state->exit();
	bcu_state.bcu_active_intercom_state = &d2d_intercomm_state;
	bcu_state.bcu_active_intercom_state->enter(&temp_send_infomation);
}

void ShowDeviceVolumeInfo(int param_select_item)
{///<在触摸屏上显示设备音量信息
	///<BCU VOlume
	int device_volume = 0;
	if(param_select_item == 0 || param_select_item == 255)
	{
		if(bcu_state.device_volume.monitor_volume == 0)///<低
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.monitor_volume == 1)///<中
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","低");
			device_volume = 33;
		}
		else if(bcu_state.device_volume.monitor_volume == 2)///<较高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","中");
			device_volume = 66;
		}
		else if(bcu_state.device_volume.monitor_volume == 3)///<高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 100;
		}
		else if(bcu_state.device_volume.monitor_volume == 4)///<高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","高");
			device_volume = 100;
		}
		title_setting_monitor_volume->label((char *)monitor_volume_digit_2_string_buffer);



	}
	device_volume = 0;


	///<eamp
	if(param_select_item == 1 || param_select_item == 255)
	{

		sprintf(car_volume_digit_2_string_buffer,"%d",bcu_state.device_volume.car_volume);
		title_setting_car_volume->label((char *)car_volume_digit_2_string_buffer);

	}

	///<PCU
	if(param_select_item == 2 || param_select_item == 255)
	{
		if(bcu_state.device_volume.intercomm_volume == 0)///<低
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.intercomm_volume == 1)///<中
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","低");
			device_volume = 25;
		}
		else if(bcu_state.device_volume.intercomm_volume == 2)///<中
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","中");
			device_volume = 50;
		}
		else if(bcu_state.device_volume.intercomm_volume == 3)///<高
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 75;
		}
		else if(bcu_state.device_volume.intercomm_volume == 4)///<高
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","高");
			device_volume = 100;
		}
		title_setting_intercomm_volume->label((char *)intercomm_volume_digit_2_string_buffer);

	}
	///<BCU
	if(param_select_item == 4 || param_select_item == 255)
	{
		if(bcu_state.device_volume.d2d_volume == 0)
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.d2d_volume == 1)
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","低");
			device_volume = 25;
		}
		else if(bcu_state.device_volume.d2d_volume == 2)///<中
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","中");
			device_volume = 50;
		}
		else if(bcu_state.device_volume.d2d_volume == 3)///<高
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 75;
		}
		else if(bcu_state.device_volume.d2d_volume == 4)///<高
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","高");
			device_volume = 99;
		}
		title_setting_d2d_intercomm_volume->label((char *)d2d_intercomm_volume_digit_2_string_buffer);
		slider_d2d_intercomm_volume->value(100 - device_volume);
		slider_d2d_intercomm_volume->redraw();
	}


}

void SetDeviceVolumeCMDPackage(int param_device_type,int param_device_volume)
{///<设置设备音量命令包
	ts_dev_volume_info_t temp_dev_vol_info;
	temp_dev_vol_info.device_volume = param_device_volume;
	temp_dev_vol_info.device_no = 230;
	switch(param_device_type)
	{
		case 0:
			strcpy(temp_dev_vol_info.device_name,"EAMPMON");
			break;
		case 1:
			strcpy(temp_dev_vol_info.device_name,"EAMP");
			break;
		case 2:
			strcpy(temp_dev_vol_info.device_name,"PCU");
			break;
		case 4:
			strcpy(temp_dev_vol_info.device_name,"BCU");
			break;
		default:
			break;
	}

	BlockBufferWrite(ts_volume_id, &temp_dev_vol_info, sizeof(temp_dev_vol_info));
//	diag_printf("%s,%d,ret = %d\n",temp_dev_vol_info.device_name,temp_dev_vol_info.device_volume,a);
}

int WhetherThisPCUHaveBeingRequest(int param_source_device_number)
{///<判断当前PCU 是否已经在请求
	int ret = 0;
	ts_debug_print(("param_source_device_number = %d\n",param_source_device_number));
	ts_debug_print(("AAA = %d\n",bcu_state.pcu_request_info.pcu_request_table[param_source_device_number]));
	int i = 0;
	for(i = 1;i <= bcu_state.pcu_request_info.request_number;i++)
	{
		if(bcu_state.pcu_request_info.pcu_request_table[i] == param_source_device_number)
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

/*-------------------在触摸屏控制线程中使用------------*/
void ChangeIntercommButtonColor(int *param_whether_ts_is_running)
{///<PCU有请求时，改变相关按钮颜色与使能


	if(bcu_state.pcu_request_info.request_number > 0 && whether_intercomm_button_is_active == 0)
	{

		AlarmTSToChangeScreen(13);//使能触摸屏
		debug_print(("Ts_Enable-01\n"));
		whether_intercomm_button_is_active = 1;
		AlarmTSToChangeScreen(2);
		*param_whether_ts_is_running = 1;
	    debug_print(("ChangeIntercommButtonColor-02\n"));
	}
	else if(*param_whether_ts_is_running == 1 &&bcu_state.pcu_request_info.request_number == 0)
	{

		whether_intercomm_button_is_active = 0;

		AlarmTSToChangeScreen(3);//恢复对讲按钮颜色
		*param_whether_ts_is_running = 0;
		debug_print(("ChangeIntercommButtonColor-02\n"));

	}
}



void ExitLive()
{

	p_current_intercomm_group = gp_main_file;
	p_current_ann_group = gp_main_file;

	SetLiveCmd(2);///<设置退出紧急广播命令

}

void SyncSelectCarInfo()
{
	int i;
	for(i = 0; i < 11 ; ++i)
	{
		if(bcu_state.car_occupy_state[i] == 1)
				{
					if(bcu_state.car_select_flag[i] == 1 && bcu_state.car_select_finish == 0)
					{
						(gp_select_car_ann_page->child(i))->color((Fl_Color)50);
						(gp_select_car_ann_page->child(i))->redraw();
						bcu_state.car_select_flag[i] = 0;
					}
					(gp_select_car_ann_page->child(i))->deactivate();
				}
				else
				{
					printf("i %d\n",i);
					(gp_select_car_ann_page->child(i))->color((Fl_Color)50);
					(gp_select_car_ann_page->child(i))->redraw();
					bcu_state.car_select_flag[i] = 0;
					(gp_select_car_ann_page->child(i))->activate();
				}
				bcu_state.car_occupy_state[i] = 0;
			}
}

void UpdateSelectCarInfo(unsigned int car_no)
{
	if(bcu_state.car_select_finish == 1) 
	{
		return;
	}
	gwCurrCarNo=car_no;
	bcu_state.car_select_flag[car_no-1]= 1 - bcu_state.car_select_flag[car_no-1];
	enter_select->activate();
	for(int i = 0; i < 11; ++i)
	{
		if(bcu_state.car_select_flag[i] == 1)
		{
			(gp_select_car_ann_page->child(i))->color((Fl_Color)2);
			(gp_select_car_ann_page->child(i))->redraw();
		}
		else
		{
			(gp_select_car_ann_page->child(i))->color((Fl_Color)50);
			(gp_select_car_ann_page->child(i))->redraw();
		}
	}
}


void ExchangeToLive()
{///<切换到口播状态
	if(bcu_state.mic_owner == NONE || bcu_state.mic_owner == ANNOUNCE )
	{


		if(bcu_state.bcu_active_ann_state->state_id != LIVE_ANN_EVENT)
		{
			SetLiveCmd(0);
		}

	}
}


void ShowD2Ppage()
{///<显示PCU请求界面
	ChangeBtnState(5);
	current_is_on_static_page = 0;
	//当前没有对讲
	if(is_intercomming_with_pcu == 0)
	{
		(gp_intercomm->child(currend_d2P_position - 1))->color((Fl_Color)94);
	}

	p_current_intercomm_group = gp_intercomm;//当前是在对讲页面

	ShowD2PRequest();//显示请求pcu

	if(is_intercomming_with_pcu == 0)
	{///<当前拒绝或者挂断PCU，设置拒绝PCU号
		diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
		SetReceptPCUNo(PCURequsthead->next->devices_id);
	}

	bcu_state.d2p_button_state = 1;
	wz_window_view->value(gp_intercomm);
	wz_select_window->value(main_group);
}

void RecvD2PRequest()
{///<接收PCU请求
	if(bcu_state.bcu_active_ann_state->state_id == LIVE_ANN_EVENT)
	{
		ForceExitLiveStateBeforeIntercomm();
		RecovoryBtnState(4);
		p_current_ann_group = gp_main_file;
		btn_main_file->color((Fl_Color)11);
		btn_main_file->redraw();
	}
	is_intercomming_with_pcu = 1;
	btn_intercomm_accept->deactivate();

	(gp_intercomm->child(currend_d2P_position - 1))->color((Fl_Color)2);
	(gp_intercomm->child(currend_d2P_position - 1))->redraw();
	bcu_state.pcu_request_info.current_conect_pcu_no = bcu_state.pcu_request_info.recept_pcu_no;
	diag_printf("bcu_state.pcu_request_info.current_conect_pcu_no=%d\n",bcu_state.pcu_request_info.current_conect_pcu_no);
	ts_package_sequence = 1;
	SetD2PCmd(0,0,bcu_state.pcu_request_info.recept_pcu_no,1);//进入、运行、退出-响应类型-pcu_no-bcu_no
	bcu_state.d2p_button_state = 2;
}

void RefuseD2PRequest()
{///<拒绝PCU请求
	bcu_state.d2p_button_state = 0;
	if(is_intercomming_with_pcu == 1)
	{///<当前正在与PCU进行对讲
		diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
		bcu_state.pcu_request_info.request_number = bcu_state.pcu_request_info.request_number - 1;
		is_intercomming_with_pcu = 0;
		btn_intercomm_accept->activate();
		ts_package_sequence = 2;
		SetD2PCmd(1,0,bcu_state.pcu_request_info.recept_pcu_no,0);//进入、运行、退出-响应类型-pcu_no-bcu_no
	}

#if 1
	else
	{///<当前没有接通PCU，直接拒绝相关PCU请求
		if(bcu_state.pcu_request_info.request_number > 0)
		{///<注销相关PCU请求信息
			bcu_state.pcu_request_info.request_number = bcu_state.pcu_request_info.request_number - 1;

				diag_printf("%d:%s\n",__LINE__,__FUNCTION__);

				SetD2PCmd(1,0,bcu_state.pcu_request_info.recept_pcu_no,0);

		}
	}

#endif
	ShowD2Ppage();//显示请求pcu
	p_current_intercomm_group = gp_main_file;
	wz_window_view->value(gp_static_show);
	wz_select_window->value(main_group);
}
unsigned char GetIntercommButtonState()
{///<获取对讲按钮（软）状态
	return whether_intercomm_button_is_active;
}





void RecoverIdleButtonState()
{///<恢复按钮状态
	btn_emerg_ann->color((Fl_Color)50);
	btn_emerg_ann->activate();
	btn_emerg_ann->redraw();

	btn_live->color((Fl_Color)50);
	btn_live->activate();
	btn_live->redraw();

	btn_intercomm->color((Fl_Color)50);
	btn_intercomm->activate();
	btn_intercomm->redraw();


}

void DisableThisBCUD2PFunction()
{///<失效本端D2P功能
	btn_intercomm->deactivate();
	int i = 0;
	for(i = 1;i <= bcu_state.pcu_request_info.request_number;i++)
	{
		(gp_intercomm->child(i - 1))->deactivate();
	}
	btn_intercomm_accept->deactivate();
	btn_intercomm_refuse->deactivate();
}

void EnableThisBCUD2PFunction()
{///<使能本端D2P功能
	btn_intercomm->activate();
	int i = 0;
	for(i = 1;i <= bcu_state.pcu_request_info.request_number;i++)
	{
		(gp_intercomm->child(i - 1))->activate();
	}
	if(bcu_state.bcu_active_intercom_state->state_id != D2P_INTERCOMM_EVENT)
	{
		btn_intercomm_accept->activate();
	}

	btn_intercomm_refuse->activate();
}

void BCUDisableAnnFunctionAccordingToTCMS()
{///<TCMS有效，退出非自动报站状态，目前没有使用
	switch(bcu_state.bcu_active_ann_state->state_id)
	{
		case LIVE_ANN_EVENT:
			ExitLive();///<change touch screen
			ShowMainPage();
			BlockBufferClear(bcu_state.recv_cmd_from_touch);
			bcu_state.bcu_active_ann_state->exit();
			bcu_state.bcu_active_ann_state = &idle_ann_state;
			bcu_state.bcu_active_ann_state->enter(NULL);
			SendCMDToEAMP(LIVE_ANN_EVENT);
			break;
		case EMERG_ANN_EVENT:

			ShowMainPage();
			BlockBufferClear(bcu_state.recv_cmd_from_touch);
			bcu_state.bcu_active_ann_state->exit();
			bcu_state.bcu_active_ann_state = &idle_ann_state;
			bcu_state.bcu_active_ann_state->enter(NULL);
			SendCMDToEAMP(EMERG_ANN_EVENT);
			break;

		case MIC_3D5_OUTER_EVENT:
			ShowMainPage();
			BlockBufferClear(bcu_state.recv_cmd_from_touch);
			bcu_state.bcu_active_ann_state->exit();
			bcu_state.bcu_active_ann_state = &idle_ann_state;
			bcu_state.bcu_active_ann_state->enter(NULL);
			SendCMDToEAMP(MIC_3D5_OUTER_EVENT);
			break;
		case OCC_EVENT:
			ShowMainPage();
			BlockBufferClear(bcu_state.recv_cmd_from_touch);
			bcu_state.bcu_active_ann_state->exit();
			bcu_state.bcu_active_ann_state = &idle_ann_state;
			bcu_state.bcu_active_ann_state->enter(NULL);
			break;
	}
}






void UpdateVolumeInfo(int param_update_flag)
{///<更新设备音量信息
	if(gp_main_file_active_page == gp_setting || current_is_on_static_page == 1)
	{///<当前正在系统设置页面
		UpdateDeviceVolumePageInfo(param_update_flag);
	}
}

void UpdateDeviceVolumePageInfo(int param_select_item)
{///<触摸屏界面更新详细设备音量信息
	///<BCU VOlume
	int device_volume = 0;
	if(param_select_item == 1 || param_select_item == 255)
	{
		if(bcu_state.device_volume.monitor_volume == 0)///<低
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.monitor_volume == 1)///<中
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","低");
			device_volume = 33;
		}
		else if(bcu_state.device_volume.monitor_volume == 2)///<较高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","中");
			device_volume = 66;
		}
		else if(bcu_state.device_volume.monitor_volume == 3)///<高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 100;
		}
		else if(bcu_state.device_volume.monitor_volume == 4)///<高
		{
			sprintf(monitor_volume_digit_2_string_buffer,"%s","高");
			device_volume = 100;
		}
		title_setting_monitor_volume->label((char *)monitor_volume_digit_2_string_buffer);

		if(bcu_state.device_volume.monitor_volume == 0)
		{
			btn_setting_monitor_volume_sub->deactivate();
		}
		else if(bcu_state.device_volume.monitor_volume == 4)
		{
			btn_setting_monitor_volume_inc->deactivate();
		}
		else
		{
			btn_setting_monitor_volume_sub->activate();
			btn_setting_monitor_volume_inc->activate();
		}
		btn_setting_monitor_volume_sub->redraw();
		btn_setting_monitor_volume_inc->redraw();
	}
	device_volume = 0;


	///<eamp
	if(param_select_item == 2 ||param_select_item ==255)
	{
		sprintf(car_volume_digit_2_string_buffer,"%d",bcu_state.device_volume.car_volume);
		title_setting_car_volume->label((char *)car_volume_digit_2_string_buffer);

		if(bcu_state.device_volume.car_volume == 0)
		{
			btn_setting_car_volume_sub->deactivate();
		}
		else if(bcu_state.device_volume.car_volume == 100)
		{
			btn_setting_car_volume_inc->deactivate();
		}
		else
		{
			btn_setting_car_volume_sub->activate();
			btn_setting_car_volume_inc->activate();
		}
		btn_setting_car_volume_sub->redraw();
		btn_setting_car_volume_inc->redraw();
	}

	///<PCU
	if(param_select_item == 3 || param_select_item == 255)
	{
		if(bcu_state.device_volume.intercomm_volume == 0)///<低
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.intercomm_volume == 1)///<中
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","低");
			device_volume = 25;
		}
		else if(bcu_state.device_volume.intercomm_volume == 2)///<中
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","中");
			device_volume = 50;
		}
		else if(bcu_state.device_volume.intercomm_volume == 3)///<高
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 75;
		}
		else if(bcu_state.device_volume.intercomm_volume == 4)///<高
		{
			sprintf(intercomm_volume_digit_2_string_buffer,"%s","高");
			device_volume = 100;
		}
		title_setting_intercomm_volume->label((char *)intercomm_volume_digit_2_string_buffer);


		if(bcu_state.device_volume.intercomm_volume == 0)
		{
			btn_setting_intercomm_volume_sub->deactivate();
		}
		else if(bcu_state.device_volume.intercomm_volume == 4)
		{
			btn_setting_intercomm_volume_inc->deactivate();
		}
		else
		{
			btn_setting_intercomm_volume_sub->activate();
			btn_setting_intercomm_volume_inc->activate();
		}
		btn_setting_intercomm_volume_sub->redraw();
		btn_setting_intercomm_volume_inc->redraw();
	}
	///<BCU
	if(param_select_item == 4 || param_select_item == 255)
	{
		if(bcu_state.device_volume.d2d_volume == 0)
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","静音");
			device_volume = 0;
		}
		else if(bcu_state.device_volume.d2d_volume == 1)
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","低");
			device_volume = 25;
		}
		else if(bcu_state.device_volume.d2d_volume == 2)///<中
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","中");
			device_volume = 50;
		}
		else if(bcu_state.device_volume.d2d_volume == 3)///<高
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","较高");
			device_volume = 75;
		}
		else if(bcu_state.device_volume.d2d_volume == 4)///<高
		{
			sprintf(d2d_intercomm_volume_digit_2_string_buffer,"%s","高");
			device_volume = 99;
		}
		title_setting_d2d_intercomm_volume->label((char *)d2d_intercomm_volume_digit_2_string_buffer);
		slider_d2d_intercomm_volume->value(100 - device_volume);
		slider_d2d_intercomm_volume->redraw();

		if(bcu_state.device_volume.d2d_volume == 0)
		{
			btn_setting_d2d_intercomm_volume_sub->deactivate();
		}
		else if(bcu_state.device_volume.d2d_volume == 4)
		{
			btn_setting_d2d_intercomm_volume_inc->deactivate();
		}
		else
		{
			btn_setting_d2d_intercomm_volume_sub->activate();
			btn_setting_d2d_intercomm_volume_inc->activate();
		}
		btn_setting_d2d_intercomm_volume_sub->redraw();
		btn_setting_d2d_intercomm_volume_inc->redraw();
	}
}

void AlarmTSToChangeScreen(int param)
{///<通知触摸屏，其他线程需要改变触摸屏界面
	switchFlag = param;
	while(switchFlag == param)
	{
		cyg_thread_delay(10);
		diag_printf("switchFlag=%d\n",param);
	}
}

void SetD2PTipButtonState(unsigned char param)
{///<设置D2P提示信息按钮状态
	if(param == 1)
	{
		btn_intercomm->activate();
		btn_intercomm->color((Fl_Color)71);
		btn_intercomm->redraw();
	}
	else
	{
		btn_intercomm->deactivate();
		btn_intercomm->color((Fl_Color)50);
		btn_intercomm->redraw();
	}
}



void EnableTS()
{///<使能触摸屏
	wz_window_view->box(FL_UP_BOX);
	wz_select_window->box(FL_UP_BOX);
	wz_window_view->value(gp_static_show);
	wz_select_window->value(main_group);
}

void DisableTS()
{///<失效触摸屏
	wz_window_view->box(FL_NO_BOX);
	wz_select_window->box(FL_NO_BOX);
	wz_select_window->value(gp_select_black);
	wz_window_view->value(gp_window_black_screen);
}


int SetMonitorBigPakage(int iph,unsigned int vn)
{

			  int ret;
			  common_big_package_t  parame;
			  memset(&parame,0,sizeof(common_big_package_t));
	    	  strcpy(parame.src_dev_name,"DBCU");
	    	  parame.src_dev_number =  bcu_state.bcu_info.devices_no;
	    	  strcpy(parame.dst_dev_name,"OCS");
	    	  parame.dst_dev_number = 230;
	    	  parame.pkg_type=2;
	    	  parame.common_big_data_u.car_no=vn;
	    	  parame.common_big_data_u.seat_id=bcu_state.bcu_info.devices_no;
	    	  parame.common_big_data_u.iph_select_flag[iph-1]=1;
	    	  ret = BlockBufferWrite(bcu_state.comm_server_send_big_buffer_id,&parame,sizeof(common_big_package_t));
	    	  if (ret <0){
	  			diag_printf("BlockBufferWrite faill. \n");
	  			BlockBufferWrite(bcu_state.comm_server_send_big_buffer_id,&parame,sizeof(common_big_package_t));

	    	  }
	    	  return 0;
}



int G_SetAndClearAnnVehicleNumber2(int op,int iph,common_big_package_t  *parame)
{

		memset(parame,0,sizeof(common_big_package_t));
	     if(op == 0){//enter

	    	  strcpy(parame->src_dev_name,"DBCU");
	    	  parame->src_dev_number =  bcu_state.bcu_info.devices_no;
	    	  strcpy(parame->dst_dev_name,"OCS");
	    	  parame->dst_dev_number = 230;
	    	  parame->pkg_type=3;
	    	  parame->common_big_data_u.car_no=gwCurrCarNo;
	    	  parame->common_big_data_u.iph_select_flag[iph-1]=1;

	     }
	     else if(op == 1){//cancel
	    	 	 	 	  strcpy(parame->src_dev_name,"DBCU");
	    		    	  parame->src_dev_number =  bcu_state.bcu_info.devices_no;
	    		    	  strcpy(parame->dst_dev_name,"OCS");
	    		    	  parame->dst_dev_number = 230;
	    		    	  parame->pkg_type=3;
	    		    	  parame->common_big_data_u.car_no=gwCurrCarNo;
	    		    	  parame->common_big_data_u.iph_select_flag[iph-1]=0;

	     }

	     	 return 0;

}
int G_SetAndClearPakage(int op,int pn,send_infomation_t *temp){


	if(op==1)
	{
		strcpy(	temp->src_devices_name,"BCU");
		temp->event_type_ann=0;
		temp->event_type_intercom=22;
		temp->src_devices_no=bcu_state.bcu_info.devices_no;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_active=1;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no=bcu_state.bcu_info.devices_no;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no=pn;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_pending=0;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over=1;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_response=0;
		temp->event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting=0;
		temp->event_info_intercom.d2p_intercomm.monitor=1;

	}

	else{
				strcpy(	temp->src_devices_name,"BCU");
				temp->event_type_ann=0;
				temp->event_type_intercom=22;
				temp->src_devices_no=bcu_state.bcu_info.devices_no;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_active=0;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no=bcu_state.bcu_info.devices_no;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_pending=0;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over=1;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_response=0;
				temp->event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting=0;
				temp->event_info_intercom.d2p_intercomm.monitor=0;

	}

}

int parse_btn_lable_value(const char *src,unsigned char *dst)
{

	int ret=0;
	if(src==NULL)
	{
		diag_printf("src argument is null, return <0.\n");
		return (ret-1);
	}
	diag_printf("value=%s\n",src);
	diag_printf("value[8]=%c\n",src[8]);
	switch (src[8])
	{
		case 49:*dst=1;break;
		case 50:*dst=2;break;
		case 51:*dst=3;break;
		case 52:*dst=4;break;
		case 53:*dst=5;break;
		case 54:*dst=6;break;
		case 55:*dst=7;break;
		case 56:*dst=8;break;
		default:diag_printf("Without this type !\n");break;
	}
		return ret;
}