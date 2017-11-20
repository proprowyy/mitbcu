/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： global_param.cxx
 * 文件标识：
 * 摘    要： 触摸屏使用的全局变量
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "global_param.h"

int ts_buffer_id = 0;//触摸屏线程中使用到的缓存区ID
int ts_volume_id = 0;//触摸屏调节音量使用的缓存区ID
int current_is_occ_or_3d5 = 0;//3D5或者OCC标志位，用于界面识别，这两种状态共用一个界面
int current_arbitrate_state_id = 0;//触摸屏系统当前状态
int whether_show_arbitrate_page = 0;//当出现资源竞争时，该字段表明是提供给用户选择还是直接进行切换
int desire_event_id = 0;//期望事件ID
int ts_package_sequence = 0;//触摸屏打包的序列号
unsigned char db_version_type = 0;///<版本号类型

unsigned char switchFlag = 0;//控制线程通过设置该变量，触摸屏线程进行相应的操作
unsigned char whether_want_to_update_page = 0;//是否需要更新界面信息

unsigned char current_is_on_static_page = 0;///表明当前是否在静态显示界面
unsigned char pa_dev_failure = 0;

unsigned char cmu_old_3_stat = 0;
unsigned char cmu_old_4_stat = 0;

/*BCU-0   CCU-1   EAMP-2  PCU-3    FDU-4    SDU-5    IDU-6   dm-7   nd-8*/


tms_tip_buffer_info_t tms_info_static_show;//提示信息，目前没有使用

/**
	The decleartion of  tms tip big show
*/
char tms_tip_page_info_buffer[20] = "\0";//提示信息缓存区，目前没有使用
unsigned char tms_tip_page_number = 0;//提示信息总页数，目前没有使用

unsigned char current_btn_failure_stataics_color = 2;//故障统计按钮默认颜色标志位

unsigned char whether_change_d2d_to_idle = 0;//是否需要结束D2D标志位
unsigned char whether_have_been_to_station_for_select_open_door_side = 0;//到站信息，是否需要显示开么侧选择界面

char dev_full_failure_info_button_name[12][15+15];//设备故障信息按钮名称缓存区
unsigned char all_device_failure_state_flag = 0;//设备故障状态标志位

/**
	触摸屏线程中使用到的
*/
int current_region_position;//当前区间位置（区间位置，在区间链表中的位置）

int current_station_no;//当前站代码
int terminal_station_no;//终点站站代码
int next_station_no;//下一站站代码
int begin_station_no;//起始站站代码

unsigned char whether_have_been_to_manual_ann = 0;//当前是否是手动报站模式


char light_bcu_state_buffer[32] = "\0";//静态显示界面BCU信息缓存区
char light_ccu_state_buffer[32] = "\0";//静态显示界面CCU信息缓存区
char light_eamp_state_buffer[32] = "\0";//静态显示界面EAMP信息缓存区
char light_pcu_state_buffer[32] = "\0";//静态显示界面PCU信息缓存区



char region_station_name_buffer[4][100] = {"\0"};//区间显示信息缓存区
char region_station_show_label_buffer[4][20]={"\0"};//区间详细信息显示缓存区
int select_station_cursor = 1;//选择站当前页吗，每一页12个button，第二页第一个button为1+12*1
int select_page_current_position = 1;/*The last press button's number，when we on the page of select station*/
int manual_station_cursor = 1;/*The cursor of station,which is on the page of maunal announcement*/
int manual_ann_page_current_position = 1;/*The position of the button,which is on the page of manual announcement*/
int whether_have_been_to_station = 0;/*Judge the train whether have been to the station*/
char emerg_page_info[20] = "\0";/*The page of emerg information*/

int region_station_cursor = 1;//区间选择界面页码数
int running_up_or_down =  1 ;/*0:down 1:up*/
int temp_select_language = 0;//默认选择的语言

int whether_have_enter_to_auto_manual_manner = 0;//是否已经进入手动报站界面
unsigned char pis_tcms_simulate_function_flag = 0;//TCMS模拟功能开关标志
unsigned char pis_tcms_simulate_function_speed_info = 0;//TCMS模拟功能速度信号
unsigned char pis_tcms_simulate_function_open_door_info = 0;//TCMS模拟功能开门信号
void TouchScreenInit()
{//触摸屏界面初始化
	int i = 0;

	//获取默认音量信息
	bcu_state.device_volume.car_volume = DEFAULT_CAR_VOLUME;
	bcu_state.device_volume.intercomm_volume = DEFAULT_INTERCOMM_VOLUME;
	bcu_state.device_volume.monitor_volume = DEFAULT_MONITOR_VOLUME;
	//设置默认触摸屏密码
	strcpy(bcu_state.device_volume.correct_password,"012345");


}
