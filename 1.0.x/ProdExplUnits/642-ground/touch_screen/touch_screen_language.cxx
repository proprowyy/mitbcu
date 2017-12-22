/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_language.cxx
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
#include "touch_screen_language.h"

unsigned int gw_RecordPcuMonBtnPass=0;

char btn_back_name[13] = {"返回"};

char confirm_name[10] = {"确定"};
char cancle_name[10] = {"取消"};
char title_please_input_password_name[30] = {"请输入密码:"};
char btn_input_password_cancle_name[10] = {"清空"};
char btn_forget_password_name[25] = {"忘记密码"};
char password_error_name[20] = {"密码错误"};
char forget_password_name[20] = {"请查看PDU"};


char emerg_page_info_name[20] = {"当前页码:"};
char emerg_state_name[15] = {"紧急广播"};
char once_name[10] = {"一次"};
char three_name[10] = {"三次"};
char cycle_name[10] = {"循环"};

char inner_name[10] = {"内圈环"};
char outer_name[10] = {"外圈环"};
char inner_name_one[10] = {"内圈单"};
char outer_name_one[10] = {"外圈单"};

char up_name[10] = {"上行"};
char down_name[15] = {"下行"};

char title_skip_tip_info_name[15] = {"越站提示"};


char title_please_input_key_name[20] = {"请 插 入 钥 匙"};
void ShowButtonName(unsigned char param_languang,unsigned char param_line_no)
{///<显示按钮中文
	btn_main_file_back->label((char *)btn_back_name);
	btn_input_password_back->label((char *)btn_back_name);
	btn_setting_back->label((char *)btn_back_name);
	btn_back_setting_items->label((char *)btn_back_name);
	btn_input_password_ok->label((char *)confirm_name);
	title_please_input_password->label((char *)title_please_input_password_name);
	btn_input_password_cancle->label((char *)btn_input_password_cancle_name);
	btn_forget_password->label((char *)btn_forget_password_name);
	title_please_input_password->label((char *)title_please_input_password_name);


}

