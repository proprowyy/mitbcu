/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_ann.h
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
#ifndef TSANN_h
#define TSANN_h
#include "ts_head_file.h"
#include "touch_screen_language.h"

extern Fl_Group *p_active_manual_ann_page;
extern line_station_door_side_t bcu_get_open_door_info_from_ccu;

/*-----------------------------------------------Manual  begin---------------------------------------*/
/** UpdateRegionStationInfo function description
 *  更新区间站台信息
 *
 *  @param   线路号
 *  @param   区间号
 *  @return  null
 */
void UpdateRegionStationInfo(unsigned char param_line_no,unsigned char param_region_no);

/** UpdateTCMSRegionStationInfo function description
 *  更新TCMS区间信息
 *
 *  @param   线路号
 *  @return  null
 */
void UpdateTCMSRegionStationInfo(unsigned char param_line_no);

/** ShowSetSkipStationInfo function description
 *  显示越站信息设置界面
 *
 *  @param   越站界面页码
 *  @return  null
 */
void ShowSetSkipStationInfo(int param_current_cursor);

/** SetManualAnnouncementStationState function description
 *  设置手动报站站台按钮状态
 *
 *  @param   站位置信息
 *  @return  null
 */
void SetManualAnnouncementStationState(int number);

/** SetManualCmd function description
 *  手动广播命令
 *
 *  @param   状态的进入或者退出
 *  @param   开始或者结束
 *  @param   线路号
 *  @param   当前站
 *  @param   到站还是遇到站
 *  @param   是否预读
 *  @return  null
 */
void SetManualCmd(int enter_or_exit,int param_begin_or_over,int param_line_no,
		int param_station_code,int param_stop_or_leave,int param_whether_pre_read);

/** SetSkipStationCmd function description
 *  越站信息设置
 *
 *  @param   线路号
 *  @param   当前站代码
 *  @param   越站标志位
 *  @return  null
 */
void SetSkipStationCmd(int param_line_no,int param_station_code,int param_skip_flag);

/** ShowManualStationInfo function description
 *  手动报站设置界面站台信息
 *
 *  @param   站在当前页码中的位置
 *  @param   显示或者隐藏箭头
 *  @return  null
 */
void ShowManualStationInfo(int param_current_cursor,int param_current_position);

/** ShowManualArrowInfo function description
 *  手动报站设置界面站台之间先后顺序箭头指示
 *
 *  @param   站在当前页码中的位置
 *  @param   显示或者隐藏箭头
 *  @return  null
 */
void ShowManualArrowInfo(int param_current_cursor,int param_current_position);

/** ShowSkipStationArrowInfo function description
 *  越站设置界面站台之间先后顺序箭头指示
 *
 *  @param   站在当前页码中的位置
 *  @param   显示或者隐藏箭头
 *  @return  null
 */
void ShowSkipStationArrowInfo(int param_current_index,int param_show_or_hide);

/** UpdateManualButtonState function description
 *  更新手动广播界面按钮状态
 *
 *  @param   当前手动报站页码
 *  @return  null
 */
void UpdateManualButtonState(int param_current_sursor);

/** ShowSelectOpenDoorSideInfo function description
 *  显示开么侧显示界面
 *
 *  @param   站代码
 *  @return  null
 */


/** SetTSManualCMDPackage function description
 *  设置手动广播控制命令
 *
 *  @param   到站或者遇到站
 *  @param   开么侧
 *  @return  null
 */
void SetTSManualCMDPackage(int param_stop_or_leave,int param_open_door_flag);
/*-----------------------------------------------Manual  over---------------------------------------*/


/*-----------------------------------------------Emerg  begin---------------------------------------*/
/** ShowEmergBroadcastTimes function description
 *  显示紧急次数
 *
 *  @param   紧急广播次数
 *  @return  null
 */
void ShowEmergBroadcastTimes(int param_times);

/** ShowEmergPageInfo function description
 *  显示紧急页码信息
 *
 *  @param   null
 *  @return  null
 */
void ShowEmergPageInfo();

/** EmergTimesRecoveryButtonState function description
 *  恢复紧急广播次数设定按钮
 *
 *  @param   紧急广播序次数
 *  @return  null
 */
void EmergTimesRecoveryButtonState(int param_last_times);

/** ShowEmergInfo function description
 *  显示紧急广播内容
 *
 *  @param   紧急广播序列号
 *  @return  null
 */
void ShowEmergInfo(int param_total_emerg_iteams);

/** SetEmergCmd function description
 *  设置紧急广播相关命令
 *
 *  @param   状态进入或者退出
 *  @param   紧急广播序列号
 *  @param   紧急广播次数
 *  @return  null
 */
void SetEmergCmd(int enter_or_exit,int temp_emerg_sequence,int temp_broadcast_times);
/*-----------------------------------------------Emerg  over---------------------------------------*/



/*-----------------------------------------------Live  begin---------------------------------------*/
/** SetLiveCmd function description
 *  设置口播进入或者退出命令包
 *
 *  @param   口播进入或者退出标志位
 *  @return  null
 */
void SetLiveCmd(int enter_or_exit);

/*-----------------------------------------------Live  begin---------------------------------------*/
/** ExchangeAnnButton function description
 *  交换button按钮的位置
 *
 *  @param   当前状态号
 *  @param   进入或者退出状态
 *  @return  null
 */
void ExchangeAnnButton(int param_event_no,int param_enter_or_exit);

/** ShowOCCOR3D5Info function description
 *  显示3D5或者OCC界面
 *
 *  @param   当前状态号
 *  @param   进入或者退出状态
 *  @return  null
 */
void ShowOCCOR3D5Info(int param_event_no,int param_enter_or_exit);

/** SendOCCOverPackage function description
 *  发送OCC结束命令包，测试使用
 *
 *  @param   null
 *  @return  null
 */
void SendOCCOverPackage();

/** SendOCCRequestPackage function description
 *  发送OCC请求命令包，测试使用
 *
 *  @param   null
 *  @return  null
 */
void SendOCCRequestPackage();

/** Send3D5OverPackage function description
 *  发送3D5结束命令包
 *
 *  @param   null
 *  @return  null
 */
void Send3D5OverPackage();

/** ShowPISTCMSSimulateBtn function description
 *  显示TCMS模拟功能界面按钮
 *
 *  @param   null
 *  @return  null
 */
void ShowPISTCMSSimulateBtn();

/** HidePISTCMSSimulateBtn function description
 *  隐藏TCMS模拟功能界面按钮
 *
 *  @param   null
 *  @return  null
 */
void HidePISTCMSSimulateBtn();

/** SendSimulatedduInfoToCCU function description
 *  发送TCMS模拟信号给CCU
 *
 *  @param   模拟功能开关
 *  @param   速度信号
 *  @param   门状态
 *  @return  null
 */
void SendSimulatedduInfoToCCU(int whether_start_simulateddu_flag,unsigned char speed,unsigned char door_status);

#endif
