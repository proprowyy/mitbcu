/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_intercomm.cxx
 * 文件标识：
 * 摘    要：  触摸屏关于对讲的处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "touch_screen_intercomm.h"
#include "intercomm_info.h"
#include "../link.h"

char d2p_request_buffer[32][20] = {"\0"};//PCU请求缓存表
unsigned char currend_d2P_position = 1;//当前PCU位置
unsigned char is_intercomming_with_pcu  = 0;//当前是否与PCU通话
unsigned char connecting_d2p_response_sursor = 1;//通过PCU的页码
char current_recept_pcu_no_buffer_language[30] = "通话中:pcu";
char current_recept_pcu_no_buffer[30] = "\0";//当前接受PCU缓存区
int current_is_being_d2d = 0;//当前坐在司机对讲
void SetReceptPCUNo(unsigned char param_pcu_position,unsigned char vn)
{///<设置当前接收的PCU号
	//diag_printf("%d:%s\n",__LINE__,__FUNCTION__);
	//diag_printf("param_pcu_position = %d\n",param_pcu_position);
	//diag_printf("vn = %d\n",vn);
	if(param_pcu_position == 0)
	{
		return ;
	}
	SetIntercomBigPackage(vn,param_pcu_position,iph_select_intercom);
	int ret = BlockBufferWrite(bcu_state.comm_server_send_big_buffer_id,iph_select_intercom,sizeof(common_big_package_t));
	bcu_state.pcu_request_info.recept_pcu_no =param_pcu_position;
	Disable_D2p_All_Btn();
}

void ShowD2PRequest(){

	int i=0;
	static char buffer[12][20];
	diag_printf("Pcu_request_number = %d\n",bcu_state.pcu_request_info.request_number);
	Node *temp;
    temp=PCURequsthead->next;
    if(bcu_state.pcu_request_info.request_number>12){
    	for(i = 0;i < 12;i ++){
		if(temp != NULL){
			sprintf(buffer[i],"%s%d%s%s%s%d","T-",temp->vehicle_number,":",temp->devices_name,"-",temp->devices_id);
			(gp_intercomm->child(i))->label(buffer[i]);
			(gp_intercomm->child(i ))->show();
			temp=temp->next;
			}
	    }
    }
    else{
    	for(i = 0;i < bcu_state.pcu_request_info.request_number; i ++){
    		if(temp!=NULL){
    			sprintf(buffer[i],"%s%d%s%s%s%d","T-",temp->vehicle_number,":",temp->devices_name,"-",temp->devices_id);
				(gp_intercomm->child(i ))->show();
				(gp_intercomm->child(i ))->label(buffer[i]);
				temp=temp->next;
			}
	      }
       }
for(i = bcu_state.pcu_request_info.request_number; i < 12;i ++)
{
	(gp_intercomm->child(i))->hide();
}
}


void ShowD2DRequest(){
	int i=0;
	static char buffer[8][20];
	Node *temp;
    temp=BCURequsthead->next;
    if(bcu_state.bcu_request_number>8){
    	for(i = 0;i < 8;i ++){
		if(temp != NULL){
			sprintf(buffer[i],"%s%d%s%s%s%d","T-",temp->vehicle_number,":",temp->devices_name,"-",temp->devices_id);
			(D2D_intercom_page->child(i))->label(buffer[i]);
			(D2D_intercom_page->child(i ))->show();
			temp=temp->next;
			}
	    }
    }
    else{
    	for(i = 0;i < bcu_state.bcu_request_number; i ++){
    		if(temp!=NULL){
    			sprintf(buffer[i],"%s%d%s%s%s%d","T-",temp->vehicle_number,":",temp->devices_name,"-",temp->devices_id);
				(D2D_intercom_page->child(i ))->show();
				(D2D_intercom_page->child(i ))->label(buffer[i]);
				temp=temp->next;
			}
	      }
		for(i = bcu_state.bcu_request_number; i < 8;i ++){
			(D2D_intercom_page->child(i))->hide();
		  }
       }
    		wz_window_view->value(D2D_intercom_page);
}
void SetD2PCmd(int enter_or_exit,int param_response_type,int param_pcu_no,int param_bcu_no)
{///<发送D2P 命令
	bcu_screen_param_t  touch_cmd_param = {0};
	touch_cmd_param.event_type = D2P_INTERCOMM_EVENT;
				
	if(enter_or_exit == 0)//enter
	{
		touch_cmd_param.state_active = 1;
		touch_cmd_param.begin_or_over = 1;

	}
	else //exit
	{
		touch_cmd_param.state_active = 0;
		touch_cmd_param.begin_or_over = 0;

	}
	touch_cmd_param.dst_device_number = param_pcu_no;
	TouchScreenSetCmd( (void *)&touch_cmd_param);
}

void ChangeIntercommLabelState(int param_current_intercomm_state)
{///<改变对讲按钮状态
	int i = 0;
	intercomm_debug_print(("ChangeIntercommLabelState:param_current_intercomm_state = %d\n",param_current_intercomm_state));
	diag_printf("ChangeIntercommLabelState:param_current_intercomm_state = %d\n",param_current_intercomm_state);
	if(param_current_intercomm_state == 1)
	{///<当前是司机对讲
		for(i = 0;i < 12;i ++)
		{
			(gp_intercomm->child(i))->deactivate();
		}
		///<显示强制切换到司机乘客对讲按钮
		btn_intercomm_accept->deactivate();
		btn_intercomm_accept->show();
		btn_intercomm_refuse->deactivate();
		btn_intercomm_refuse->show();

	}
	else
	{
		for(i = 0;i < 12;i ++)
		{
			(gp_intercomm->child(i))->activate();
		}
		if(is_intercomming_with_pcu == 0)
		{
			btn_intercomm_accept->activate();
			btn_intercomm_accept->show();
		}

		btn_intercomm_refuse->activate();
		btn_intercomm_refuse->show();
	}
	intercomm_debug_print(("ChangeIntercommLabelState finish"));
}

void ClearAllPCURequest()
{///<清除所有PCU请求
	bcu_state.pcu_request_info.request_number = 0;
	bcu_state.pcu_request_info.hang_up_pcu_no = 0;
	bcu_state.pcu_request_info.recept_pcu_no = 0;
}

void ChangeD2DBreakUpD2PBtnState()
{///显示打断D2D切换回D2P按钮，使能D2P按钮
	current_is_being_d2d = 1;
	ChangeIntercommLabelState(current_is_being_d2d);
	btn_intercomm->activate();
	whether_intercomm_button_is_active = 1;
	btn_intercomm->color((Fl_Color)71);
}


void RecoverD2DBreakUpD2PBtnState()
{///恢复打断D2D切换回D2P按钮，使能D2P按钮
	diag_printf("RecoverD2DBreakUpD2PBtnState-------------------\n");
	current_is_being_d2d = 0;
	ChangeIntercommLabelState(current_is_being_d2d);
	btn_intercomm->activate();
	whether_intercomm_button_is_active = 1;
	btn_intercomm->color((Fl_Color)71);

}
void ShowNoPCURequest()
{///<显示PCU请求界面没有PCU请求，自动切换到主界面，并且失效D2P按钮
	(gp_intercomm->child(currend_d2P_position - 1))->color((Fl_Color)50);
	(gp_intercomm->child(currend_d2P_position - 1))->redraw();

	is_intercomming_with_pcu  = 0;
	btn_intercomm->activate();
	whether_intercomm_button_is_active = 1;

	ClearAllPCURequest();

	d2p_reponse_sursor = 1;
	currend_d2P_position = 1;


	btn_intercomm->deactivate();
	whether_intercomm_button_is_active = 0;
	btn_intercomm->color((Fl_Color)50);
	btn_intercomm->redraw();

		if(bcu_state.this_bcu_is_active == 1)
		{
			if(bcu_state.bcu_active_ann_state->state_id == ANN_IDLE)
			{
				btn_main_file->color((Fl_Color)71);
				btn_main_file->redraw();

				wz_window_view->value(gp_main_file);
				wz_select_window->value(main_group);
			}
			else
			{
				wz_window_view->value(p_current_ann_group);
				wz_select_window->value(main_group);
			}
		}
		else
		{
			wz_window_view->value(gp_static_show);
			wz_select_window->value(main_group);
		}

}
void disable_recv_refuse()
{
	btn_intercomm_accept->deactivate();
	btn_intercomm_refuse->deactivate();

}
