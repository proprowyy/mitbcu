/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_ann.cxx
 * 文件标识：
 * 摘    要： 触摸屏关于广播的处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "touch_screen_ann.h"

Fl_Group *p_active_manual_ann_page;
line_station_door_side_t bcu_get_open_door_info_from_ccu;
/*-----------------------------------------------Manual  begin---------------------------------------*/

void SetLiveCmd(int enter_or_exit)
{///<设置口播命令
	bcu_screen_param_t  touch_cmd_param = {0};
	touch_cmd_param.event_type = LIVE_ANN_EVENT;

				
	if(enter_or_exit == 0)//enter
	{
		touch_cmd_param.state_active = 1;
		touch_cmd_param.begin_or_over = 0;
	}
	else //exit
	{
		touch_cmd_param.state_active = 0;
		touch_cmd_param.begin_or_over = 0;
	}
	TouchScreenSetCmd((void *)&touch_cmd_param);

}
#if 0
void ExchangeAnnButton(int param_event_no,int param_enter_or_exit)
{///<改变广播按钮状态
	unsigned int cur_event_prio = 0;
	if(param_event_no == OCC_EVENT)
	{
		cur_event_prio = bcu_state.bcu_event_priority.occ_prio;
	}
	else if(param_event_no == MIC_3D5_OUTER_EVENT)
	{
		cur_event_prio = bcu_state.bcu_event_priority.outer_3d5_prio;
	}

	if(param_enter_or_exit == 1)
	{
		if(bcu_state.bcu_event_priority.manual_prio > cur_event_prio)
		{

		}
		if(bcu_state.bcu_event_priority.live_prio > cur_event_prio)
		{
//			Fl::wait();
			btn_live->color((Fl_Color)38);
			btn_live->deactivate();
			btn_live->redraw();
		}
		if(bcu_state.bcu_event_priority.emerg_prio > cur_event_prio)
		{

		}
	}
	else
	{
		if(bcu_state.bcu_event_priority.manual_prio > cur_event_prio)
		{

		}
		if(bcu_state.bcu_event_priority.live_prio > cur_event_prio)
		{
//			Fl::wait();
			btn_live->color((Fl_Color)50);
			btn_live->activate();
			btn_live->redraw();
		}
		if(bcu_state.bcu_event_priority.emerg_prio > cur_event_prio)
		{
		}
	}
}
#endif








