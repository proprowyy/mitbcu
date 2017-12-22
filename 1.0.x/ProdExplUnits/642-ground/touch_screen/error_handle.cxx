/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： error_handle.cxx
 * 文件标识：
 * 摘    要： 触摸屏错误处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "error_handle.h"
#ifdef TS_ERROR_CHECK
Tree error_handle_tree;//触摸屏错误记录树
Tree err_touch_screen;//触摸屏界面错误记录点
Tree err_window_view;//触摸屏功能界面错误记录点
Tree err_select_window;//触摸屏选择界面错误记录节点

FILE *fp_operation_log;//触摸屏操作log日志文件描述符
FILE *fp_debug_log;//触摸屏调试log日志文件描述符
FILE *fp_system_error;//触摸屏系统log日志文件描述符


int GetTime(char *out, int fmt)
{//获取系统时间，并且以一定格式输出
	if(out == NULL)
	{
		return -1;
	}
        
    time_t t;
    struct tm *tp;
    t = time(NULL);

    tp = localtime(&t);
    if(fmt == 0)
	{
		sprintf(out, "%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	}		
    else if(fmt == 1)
	{
        sprintf(out, "%2.2d-%2.2d-%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
	}
    else if(fmt == 2)
	{
        sprintf(out, "%2.2d:%2.2d:%2.2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
	}
    return 0;
}

FILE * openFile(const char *fileName, const char *mode)
{//打开触摸屏错误文件log日志
  return NULL;
	FILE *fp = fopen(fileName, mode);

	if(fp == NULL)
	{
		//printf("Open %s is error\n",fileName);
		exit(0);
	}
	else
	{
		//printf("Open %s is correct\n",fileName);
	}	
    return fp;
}

int closeFile(FILE *fp)
{//关闭触摸屏错误文件log日志
	return fclose(fp);
}

void CreateErrorHandleTree()
{//创建触摸屏界面错误记录树
	error_handle_tree = (Tree)malloc(sizeof(TNode));

	error_handle_tree->parent = NULL;
	error_handle_tree->firstChild =NULL;
	error_handle_tree->nextSibling = NULL;

	error_handle_tree->size_w = TOUCHSXREENWIDTH;
	error_handle_tree->size_h = TOUCHSXREENHEIGTH;

	strcpy((char *)error_handle_tree->component_name,"TouchScreen");
}

void CreateErrorTouchScreenNode()
{//创建触摸屏界面错误记录节点
	char temp_log_info[100] = "";
	int is_there_have_error = 0;
	err_touch_screen = (Tree)malloc(sizeof(TNode));
	err_touch_screen->firstChild = NULL;
	err_touch_screen->nextSibling = NULL;
	err_touch_screen->parent = error_handle_tree;

	error_handle_tree->firstChild = err_touch_screen;
	
	strcpy((char *)err_touch_screen->component_name,"wd_touch_screen");

	err_touch_screen->size_h = wd_touch_screen->h();
	err_touch_screen->size_w = wd_touch_screen->w();

	
	if(err_touch_screen->size_h > err_touch_screen->parent->size_h)
	{
		sprintf(temp_log_info,"%s's heigth is error:heigth = %d\n",err_touch_screen->component_name,err_touch_screen->size_h);
		WriteLog(fp_system_error,temp_log_info,1);
				is_there_have_error = 1;
	}
	
	if(err_touch_screen->size_w > err_touch_screen->parent->size_w)
	{
		sprintf(temp_log_info,"%s's heigth is error:width = %d\n",err_touch_screen->component_name,err_touch_screen->size_w);
		WriteLog(fp_system_error,temp_log_info,1);
				is_there_have_error = 1;
	}
	
	if(is_there_have_error == 1)
	{
		exit(0);
	}
}

void CreateErrorHadleWindowView()
{//创建触摸屏界面-功能操作界面错误记录根节点--触摸屏上面部分
	char temp_log_info[100] = "";
	int is_there_have_error = 0;
	err_window_view = (Tree)malloc(sizeof(TNode));
	err_window_view->firstChild = NULL;
	err_window_view->nextSibling = NULL;
	err_window_view->parent = err_touch_screen;
	
	err_touch_screen->firstChild = err_window_view;
	
	strcpy((char *)err_window_view->component_name,"wizard window view");
	
	err_window_view->size_h = wz_window_view->h();
	err_window_view->size_w = wz_window_view->w();
	err_window_view->position_x = wz_window_view->x();
	err_window_view->position_y = wz_window_view->y();

	if(err_window_view->position_x < 0)
	{
		sprintf(temp_log_info,"%s's orientation of x is error,x() = %d\n",err_window_view->component_name,err_window_view->position_x);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_window_view->position_y < 0)
	{
		sprintf(temp_log_info,"%s's orientation of y is error,y() = %d\n",err_window_view->component_name,err_window_view->position_y);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_window_view->size_h > err_window_view->parent->size_h)
	{
		sprintf(temp_log_info,"%s's heigth is error,heigth = %d\n",err_window_view->component_name,err_window_view->size_h);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_window_view->size_w > err_window_view->parent->size_w)
	{
		sprintf(temp_log_info,"%s's width is error,width = %d\n",err_window_view->component_name,err_window_view->size_w);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(is_there_have_error == 1)
	{
		exit(0);
	}
}

void CreateErrorHadleSelectWindow()
{//创建触摸屏界面-选择操作界面错误记录根节点--触摸屏下面部分
	char temp_log_info[100] = "";
	int is_there_have_error = 0;
	err_select_window = (Tree)malloc(sizeof(TNode));
	err_select_window->firstChild = NULL;
	err_select_window->nextSibling = NULL;

	err_select_window->parent = err_touch_screen;

	err_window_view->nextSibling = err_select_window;

	strcpy((char *)err_select_window->component_name,"wizard select window");

	err_select_window->size_h = wz_select_window->h();
	err_select_window->size_w = wz_select_window->w();
	err_select_window->position_x = wz_select_window->x();
	err_select_window->position_y = wz_select_window->y();

	if(err_select_window->position_x < 0)
	{
		sprintf(temp_log_info,"%s's orientation of x is error,x() = %d\n",err_select_window->component_name,err_select_window->position_x);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_select_window->position_y < 0)
	{
		sprintf(temp_log_info,"%s's orientation of y is error,y() = %d\n",err_select_window->component_name,err_select_window->position_y);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_select_window->size_h > err_select_window->parent->size_h)
	{
		sprintf(temp_log_info,"%s's heigth is error,heigth = %d\n",err_select_window->component_name,err_select_window->size_h);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_select_window->size_w > err_select_window->parent->size_w)
	{
		sprintf(temp_log_info,"%s's width is error,width = %d\n",err_select_window->component_name,err_select_window->size_w);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(is_there_have_error == 1)
	{
		exit(0);
	}
}

void CodeErrorCheck()
{//触摸屏界面错误检查
	CreateErrorHandleTree();
	CreateErrorTouchScreenNode();
	CreateErrorHadleWindowView();
	CreateErrorHadleSelectWindow();


	AddErrorHandleGroupCheck(err_window_view,gp_main_file,"gp_main_file");
	AddErrorHandleGroupCheck(err_window_view,gp_manual_ann,"gp_manual_ann");
	AddErrorHandleGroupCheck(err_window_view,gp_emerg_ann,"gp_emerg_ann");
	AddErrorHandleGroupCheck(err_window_view,gp_live,"gp_live");
	AddErrorHandleGroupCheck(err_window_view,gp_intercomm,"gp_intercomm");
	AddErrorHandleGroupCheck(err_window_view,gp_static_show,"gp_static_show");
	
	AddErrorHandleGroupCheck(err_window_view,gp_input_password,"gp_input_password");
	AddErrorHandleGroupCheck(err_window_view,gp_select_station,"gp_select_station");
	AddErrorHandleGroupCheck(err_window_view,gp_skip_station,"gp_skip_station");
//	AddErrorHandleGroupCheck(err_window_view,gp_data,"gp_data");
	AddErrorHandleGroupCheck(err_window_view,gp_failure_statics,"gp_failure_statics");
	
	AddErrorHandleGroupCheck(err_window_view,gp_setting,"gp_setting");
//	AddErrorHandleGroupCheck(err_window_view,gp_station_select_page,"gp_station_select_page");
	AddErrorHandleGroupCheck(err_select_window,main_group,"main_group");

//	AddErrorHandleGroupCheck(err_window_view,gp_tms_failure_tip,"gp_tms_failure_tip");
	AddErrorHandleGroupCheck(err_window_view,gp_setting_items,"gp_setting_items");
	AddErrorHandleGroupCheck(err_window_view,gp_set_language,"gp_set_language");
	AddErrorHandleGroupCheck(err_window_view,gp_wait_for_get_data,"gp_wait_for_get_data");
	AddErrorHandleGroupCheck(err_window_view,gp_select_open_door_side,"gp_select_open_door_side");
	
	GroupErrorCheck(err_window_view,gp_main_file,"gp_main_file");
	GroupErrorCheck(err_window_view,gp_manual_ann,"gp_manual_ann");
	GroupErrorCheck(err_window_view,gp_emerg_ann,"gp_emerg_ann");
	GroupErrorCheck(err_window_view,gp_live,"gp_live");
	GroupErrorCheck(err_window_view,gp_intercomm,"gp_intercomm");
	GroupErrorCheck(err_window_view,gp_static_show,"gp_static_show");
	GroupErrorCheck(err_window_view,gp_input_password,"gp_input_password");
	GroupErrorCheck(err_window_view,gp_select_station,"gp_select_station");
	GroupErrorCheck(err_window_view,gp_skip_station,"gp_skip_station");
//	GroupErrorCheck(err_window_view,gp_data,"gp_data");
	GroupErrorCheck(err_window_view,gp_failure_statics,"gp_failure_statics");
	GroupErrorCheck(err_window_view,gp_setting,"gp_setting");
//	GroupErrorCheck(err_window_view,gp_station_select_page,"gp_station_select_page");
//	GroupErrorCheck(err_window_view,gp_tms_failure_tip,"gp_tms_failure_tip");
	GroupErrorCheck(err_window_view,gp_setting_items,"gp_setting_items");
	GroupErrorCheck(err_window_view,gp_set_language,"gp_set_language");
	GroupErrorCheck(err_window_view,gp_wait_for_get_data,"gp_wait_for_get_data");
	GroupErrorCheck(err_window_view,gp_select_open_door_side,"gp_select_open_door_side");

	GroupErrorCheck(err_select_window,main_group,"main_group");
	
	WriteLog(fp_system_error,"System starts normally, no error!",1);

}

void AddErrorHandleGroupCheck(Tree p_parent_tree,Fl_Group *p_gp,const char p_gp_name[])
{// 添加叶子节点到树中-添加group信息到错误检测树中
	Tree err_hdl_node;
	Tree current_node_point;

	err_hdl_node = (Tree)malloc(sizeof(TNode));
	strcpy((char *)err_hdl_node->component_name,p_gp_name);
	err_hdl_node->firstChild = NULL;
	err_hdl_node->nextSibling = NULL;
	err_hdl_node->parent = p_parent_tree;
	err_hdl_node->position_x= p_gp->x();;
	err_hdl_node->position_y = p_gp->y();
	err_hdl_node->size_h = p_gp->h();
	err_hdl_node->size_w = p_gp->w();


	if(p_parent_tree->firstChild == NULL)
	{
		p_parent_tree->firstChild = err_hdl_node;
	}
	else
	{
		current_node_point = p_parent_tree->firstChild;
		while(current_node_point->nextSibling != NULL)
		{
			current_node_point = current_node_point->nextSibling;
		}
		current_node_point->nextSibling = err_hdl_node;
	}
	
	
	char temp_log_info[100] = "";
	int is_there_have_error = 0;
	

	
	if(err_hdl_node->position_x < 0)
	{
		sprintf(temp_log_info,"%s's orientation of x is error,x() = %d\n",err_hdl_node->component_name,err_hdl_node->position_x);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_hdl_node->position_y < 0)
	{
		sprintf(temp_log_info,"%s's orientation of y is error,y() = %d\n",err_hdl_node->component_name,err_hdl_node->position_y);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}

	if(err_hdl_node->position_x + 	err_hdl_node->size_w  > err_hdl_node->parent->size_w )
	{
		sprintf(temp_log_info,"%s's width is more than his father:f(%d,%d,%d,%d)  c(%d,%d,%d,%d)\n",err_hdl_node->component_name,
				err_hdl_node->parent->position_x,err_hdl_node->parent->position_y,err_hdl_node->parent->size_w,err_hdl_node->parent->size_h,
				err_hdl_node->position_x,err_hdl_node->position_y,err_hdl_node->size_w,err_hdl_node->size_h);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}
	
	if(err_hdl_node->position_x + 	err_hdl_node->size_h  > err_hdl_node->parent->size_h)
	{
		sprintf(temp_log_info,"%s's heigth is more than his father:f(%d,%d,%d,%d)  c(%d,%d,%d,%d)\n",err_hdl_node->component_name,
				err_hdl_node->parent->position_x,err_hdl_node->parent->position_y,err_hdl_node->parent->size_w,err_hdl_node->parent->size_h,
				err_hdl_node->position_x,err_hdl_node->position_y,err_hdl_node->size_w,err_hdl_node->size_h);
		WriteLog(fp_system_error,temp_log_info,1);
		is_there_have_error = 1;
	}
	
	if(is_there_have_error == 1)
	{
		exit(0);
	}
}

void GroupErrorCheck(Tree p_parent_tree,Fl_Group *p_gp,const char p_gp_name[])
{// 检查group错误信息
	int child_number = p_gp->children();
	int i = 0;
	int is_there_have_error = 0;
	char temp_log_info[100] = "";

	for(i = 0;i < child_number;i++)
	{
		if(p_gp->child(i)->x() < 0)
		{
			sprintf(temp_log_info,"%s's son%d orientation of x is error,x() = %d\n",p_gp_name,i,p_gp->child(i)->x());
			WriteLog(fp_system_error,temp_log_info,1);
			is_there_have_error = 1;
		}

		if(p_gp->child(i)->y() < 0)
		{
			sprintf(temp_log_info,"%s's son%d orientation of y is error,y() = %d\n",p_gp_name,i,p_gp->child(i)->y());
			WriteLog(fp_system_error,temp_log_info,1);
			is_there_have_error = 1;
		}

		if(p_gp->child(i)->x() + p_gp->child(i)->w() > p_parent_tree->size_w + p_parent_tree->position_x)
		{
			sprintf(temp_log_info,"%s's son%d width  is error. p(%d,%d,%d,%d)--c(%d,%d,%d,%d)\n",p_gp_name,i,
					p_parent_tree->position_x,p_parent_tree->position_y,p_parent_tree->size_w,p_parent_tree->size_h,
					p_gp->child(i)->x(),p_gp->child(i)->y(),p_gp->child(i)->w(),p_gp->child(i)->h());
			WriteLog(fp_system_error,temp_log_info,1);
			is_there_have_error = 1;
		}

		if(p_gp->child(i)->y() + p_gp->child(i)->h() > p_parent_tree->size_h + p_parent_tree->position_y)
		{
			sprintf(temp_log_info,"%s's son%d orientation of y is error,y() = %d\n",p_gp_name,i,p_gp->child(i)->y());
			WriteLog(fp_system_error,temp_log_info,1);
			is_there_have_error = 1;
		}

	}

	if(is_there_have_error == 1)
	{
		exit(0);
	}
}
#endif
int WriteLog(FILE *fp, const char *str, int bLog)
{//将相关信息写入到指定文件中去
  return 0;
	char curTime[100] = {0};
    int ret = -1;
    if(bLog)
    {
#ifdef TS_ERROR_CHECK
         GetTime(curTime, 0);
#endif
        ret = fprintf(fp, "[%s] %s\n", curTime, str);
    }
    else
    {
    	ret = fprintf(fp, "%s\n", str);
    }


    if(ret >= 0)
    {
        fflush(fp);
        return 0;
    }
    else
    {
    	return -1;
    }
}



