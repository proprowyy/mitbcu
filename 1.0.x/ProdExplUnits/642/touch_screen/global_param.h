/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： global_param.h
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
#ifndef GlobalParam_h
#define GlobalParam_h

#define NEW_TS_STRATEGE

#include "ts_head_file.h"
#include "ts_type.h"

extern int ts_buffer_id;//cmd buffer id,which is used to send touch screen cmd to control thread
extern int ts_volume_id;
extern int current_is_occ_or_3d5;
extern int current_arbitrate_state_id;
extern int whether_show_arbitrate_page;
extern int desire_event_id;
extern int ts_package_sequence;
extern unsigned char db_version_type;

extern unsigned char switchFlag;
extern unsigned char whether_want_to_update_page;
extern tms_tip_buffer_info_t tms_info_static_show;
extern char tms_tip_page_info_buffer[20];
extern unsigned char tms_tip_page_number;

extern unsigned char current_btn_failure_stataics_color;

extern unsigned char whether_change_d2d_to_idle;

extern unsigned char whether_have_been_to_station_for_select_open_door_side;

extern char dev_full_failure_info_button_name[12][15+15];
extern unsigned char all_device_failure_state_flag;
/**
	触摸屏线程中使用到的
*/
//extern int current_region_no ;
extern int current_region_position;

extern int current_station_no ; 
extern int terminal_station_no;
extern int next_station_no; 
extern int begin_station_no; 

extern unsigned char current_is_on_static_page;
extern unsigned char device_failure[9];
extern unsigned char device_recv_failure[9];
extern unsigned char pa_dev_failure;
//extern unsigned char bcu_failure;
//extern unsigned char ccu_failure;
//extern unsigned char eamp_failure;
//extern unsigned char pcu_failure;
//extern unsigned char fdu_failure;
//extern unsigned char sdu_failure;
//extern unsigned char idu_failure;
//extern unsigned char dm_failure;
//extern unsigned char nd_failure;
//
//extern unsigned char bcu_recv_failure;
//extern unsigned char ccu_recv_failure;
//extern unsigned char eamp_recv_failure;
//extern unsigned char pcu_recv_failure;
//extern unsigned char fdu_recv_failure;
//extern unsigned char sdu_recv_failure;
//extern unsigned char idu_recv_failure;
//extern unsigned char dm_recv_failure;
//extern unsigned char nd_recv_failure;

extern unsigned char cmu_old_3_stat;
extern unsigned char cmu_old_4_stat;

extern unsigned char whether_have_been_to_manual_ann ;

extern char light_bcu_state_buffer[32];
extern char light_ccu_state_buffer[32];
extern char light_eamp_state_buffer[32];
extern char light_pcu_state_buffer[32];

extern char region_station_name_buffer[4][100] ;
extern char region_station_show_label_buffer[4][20];
extern int select_station_cursor;
extern int select_page_current_position ;/*The last press button's number，when we on the page of select station*/
extern int manual_station_cursor ;/*The cursor of station,which is on the page of maunal announcement*/
extern int manual_ann_page_current_position ;/*The position of the button,which is on the page of manual announcement*/
extern int whether_have_been_to_station ;/*Judge the train whether have been to the station*/
extern char emerg_page_info[20] ;/*The page of emerg information*/

extern  int region_station_cursor ;
extern int running_up_or_down  ;/*0:down 1:up*/
extern int temp_select_language;

extern int whether_have_enter_to_auto_manual_manner ;
extern char car_state_buffer[6][10];
extern unsigned char pis_tcms_simulate_function_flag;
extern unsigned char pis_tcms_simulate_function_speed_info;
extern unsigned char pis_tcms_simulate_function_open_door_info;
/** TouchScreenInit function description
 *  触摸屏界面初始化
 *
 *  @param   null
 *  @return  null
 */
void TouchScreenInit();

#endif
