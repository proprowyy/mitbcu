/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_language.h
 * 文件标识：
 * 摘    要： 触摸屏语言信息处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef TOUCH_SCREEN_LANGUAGE_H_
#define TOUCH_SCREEN_LANGUAGE_H_

#include "ts_head_file.h"

extern char btn_line_no_name[2][15];
extern char title_please_input_password_name[30];
extern char password_error_name[20];
extern char forget_password_name[20];
extern char emerg_page_info_name[20];

extern char once_name[10];
extern char three_name[10];
extern char cycle_name[10];

extern char inner_name[10];
extern char outer_name[10];
extern char inner_name_one[10];
extern char outer_name_one[10];
extern char up_name[10];
extern char down_name[15];

extern char btn_trigger_pre_arrive_station_name[20];
extern char btn_trigger_arrive_station_name[15];
extern char btn_trigger_arrive_terminal_station_name[25];

extern char region_name[10];

extern char left_side_name[10];
extern char right_side_name[10];
extern char title_please_input_key_name[20];

/** ShowButtonName function description
 *  根据系统语言设在相应界面语言
 *
 *  @param   语言
 *  @param   线路号
 *  @return  null
 */
void ShowButtonName(unsigned char param_languang,unsigned char param_line_no);


extern unsigned char  return_curr_select_ann_carno(unsigned char a[]);


#endif /* TOUCH_SCREEN_LANGUAGE_H_ */
