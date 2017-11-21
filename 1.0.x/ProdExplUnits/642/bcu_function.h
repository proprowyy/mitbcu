/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：bcu_function.h
 * 文件标识：
 * 摘    要：bcu处理的函数头文件
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：2012-10-24
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */

#ifndef BCU_FUNCTION_H
#define BCU_FUNCTION_H

#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <pkgconf/hal.h>
//#include <pkgconf/io_audio.h>
#include <netdb.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/hal/hal_io.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/plf_io.h>
#include <pkgconf/hal.h>
#include <cyg/fileio/fileio.h>
#include <cyg/hal/hal_if.h>
#include <pkgconf/devs_snd_vs1063a.h>
#include <cyg/io/audio/audio.h>
#include <cyg/io/audio/audio_dev.h>
#include "data_struct.h"
#include "state_machine.h"
#include "buffer_read_write_fun.h"
#include "sound_driver.h"
#include "thread_config.h"
#include "thread_comm.h"

#include "bcu_ann.h"
#include "bcu_intercom.h"
#include "log_record.h"
#include "bcu_state_machine.h"
#include "bcu_extern_button.h"

#include "network/zhw_network_main.h"

#include "touch_screen/touch_screen_function.h"
extern pthread_t thread_of_network;
extern pthread_t thread_of_control;
extern pthread_t thread_of_screen;
extern unsigned char network_priority ;
extern unsigned char ts_priority;
extern unsigned char control_priority;
//#define TEST_FOR_SHENGYANG
/*---------------------------begin of bcu beta 1.1------------------------------*/
extern unsigned char whether_updata_db;
extern char *blank_audio_data;
extern int ntwork_thread_prio;
extern int package_sequence;
extern int bcu_test_for_ts;
extern bcu_state_t bcu_state;
extern char station_informaton[2][MAX_STATION_NUMBER];
extern char src_device_info[10];
extern char this_device_info[10];
extern char audio_data_log_file_name[60];
extern FILE *fp_audio_data_log_file;
extern unsigned char whether_save_live_audio_data;
extern unsigned char whether_save_d2d_audio_data;
extern unsigned char whether_save_d2p_audio_data;
extern station_information_t station_information[2][MAX_STATION_NUMBER];
extern int whether_have_begin_broadcast_alarm_audio_data;
extern int whether_could_be_finished_d2d;
extern int whether_eant_to_delay_finished_d2d;
extern unsigned char whether_get_out3d5_button_state;
extern unsigned char last_out3d5_button_state;

//extern int last_control_flag;
//extern int control_flag;
extern int last_sample_flag;
extern int sample_flag;
extern int thread_change_flag;
extern int last_thread_change_flag;
extern int network_flag;
extern int last_network_flag;
extern unsigned char snd_timer_flag;
extern unsigned char deamon_timer_flag;
extern unsigned char key_timer_flag;
extern unsigned char sync_timer_flag;
extern unsigned char network_count;

extern unsigned char wherther_start_auto_ann;//add by pan 2015-12-25
extern unsigned char sim_auto_flag;//add by pan 2015-12-25




extern common_big_package_t *common_package_select_car_to_ocs,*iph_select,*iph_select_intercom;
extern send_infomation_t g_MonPcuCmdPakage ,g_iph_pcu;




extern common_package_t dev_vol_to_eamp,dev_vol_to_pcu,dev_vol_to_bcu,recv_dev_col_bcu,dev_vol_to_ccu;
extern unsigned char whether_ask_key_info;
extern unsigned char whether_send_to_pcu;
extern char ts_key_info_buffer[30];
enum event_trigger
{
	EVENT_INVALID = -1,

	EVENT_LIVE_BUTTON_PRESS,/*1*/
	EVENT_TRANSFER_TO_MANUAL,/*2*/
	EVENT_TRANSFER_TO_AUTO,/*3*/
	EVENT_OCC_COMMAND,/*4*/
	EVENT_OCC_RELEASE,/*5*/
	EVENT_EMERGENCY_BUTTON_PRESS,/*6*/

	EVENT_AUTO_MODE,/*7*/
	EVENT_MANUAL_MODE,/*8*/

	EVENT_RETURN_BUTTON_PRESS,/*9*/

	EVENT_TMS_SIGNAL,/*10*/
	EVENT_TMS_SIGNAL_OVER,/*11*/
	EVENT_TMS_EMERGENCY,/*12*/

	EVENT_PTT_OR_DRIVER_CALL,/*13*/
	EVENT_PTT_RELEASE_AND_DRIVER_RELEASE,/*14*/
	EVENT_CONFIRM_PCU_CALL,/*15*/
	EVENT_CONFIRM_RELEASE_PCU,/*16*/
	EVENT_TIME_OUT,/*17*/
	EVENT_TRANSFER_TO_D2D_OR_DRIVER_CALL,/*18*/
	EVENT_CONFIRM_TRANSFER_TO_D2P,/*19*/
	D2P_MONITOR_EVENT_CALL,
	D2P_MONITOR_EXIT_CALL,
	EVENT_MAX
};

extern common_package_t network_send_package_ptt_release;
extern common_package_t network_send_package_ptt_press;
extern common_package_t network_send_package_ptt_release_to_pcu;
extern common_package_t network_send_package_ptt_press_to_pcu;
extern void SetBCUKeyState(network_send_package_t recv_network_info_from_network);

/** InitBCUState function description
 *  This function belongs to bcu,which is used to initialize bcu_state
 *
 *  @param   null
 *  @return  null
 */
void InitBcuAndState();

/** BcuGetNetTxCmdBuffId function description
 *  This function belongs to bcu,which is used to get the network send cmd information buffer id
 *
 *  @param   null
 *  @return  null
 */
int BcuGetNetTxCmdBuffId(void);


/** InitBuffer function description
 *  This function belongs to bcu，it's used to initialize buffer
 *  @param   null
 *  @return  null
 */
int InitBuffer(void);

/** ClearAllAudioDataBuffer function description
 *  This function belongs to bcu,which is used to clear all audio data buffer
 *
 *  @param   null
 *  @return  null
 */
void ClearAllAudioDataBuffer();


/** ClearAllAudioDataBuffer function description
 *  This function belongs to bcu,which is used to clear all audio data buffer
 *
 *  @param   null
 *  @return  null
 */
void ClearAllAudioDataBuffer();

/** SetPTTStateInfoPackage function description
 *  This function belongs to bcu,which is used to set ptt state information
 *
 *  @param   null
 *  @return  null
 */
void SetPTTStateInfoPackage();

/** BCURuleTableInit function description
 *  This function belongs to bcu,which is used to initial bcu rule table
 *
 *  @param   null
 *  @return  null
 */
void BCURuleTableInit();

/** BCURuleInit function description
 *  This function belongs to bcu,which is used to initial BCU rule
 *
 *  @param   null
 *  @return  null
 */
void BCURuleInit();

/** BCUAddRule function description
 * This function belongs to bcu,which is used to Add bcu rule
 *
 *  @param   rule
 *  @return  null
 */
void BCUAddRule(state_machine_rule_t xRule);

/** BCUAddRule function description
 * This function belongs to bcu,which is used to remove bcu rule
 *
 *  @param   rule
 *  @return  null
 */
void BCURemoveRule(state_machine_rule_t xRule);

/** BCUDisableRule function description
 * This function belongs to bcu,which is used to disable bcu rule
 *
 *  @param   rule
 *  @return  null
 */
void BCUDisableRule(state_machine_rule_t xRule);


/** BCUEnableRule function description
 * This function belongs to bcu,which is used to enable bcu rule
 *
 *  @param   rule
 *  @return  null
 */
void BCUEnableRule(state_machine_rule_t xRule);

/** RuleSet function description
 *  This function belongs to bcu,which is used to set the rule
 *
 *  @param   original state
 *  @param   destination state
 *  @param   trigger signal
 *  @return  rule
 */
state_machine_rule_t RuleSet(state_process_t *src_state,state_process_t *dst_state,int trigger_event);

/** RegisterCommonBufferRecvBuffer function description
 * 注册相关通用通道包命令接收缓存区
 *
 *  @param   null
 *  @return  null
 */
void RegisterCommonBufferRecvBuffer();

/** BCUControlMainBufferInit function description
 * This function belongs to bcu,which is used to Initialize the bcu control thread data buffer
 *
 *  @param   null
 *  @return  null
 */
void BCUControlMainBufferInit();


/** BCUStateInit function description
 * This function belongs to bcu,which is used to initialize the original state of bcu
 *
 *  @param   null
 *  @return  null
 */
void BCUStateInit();


/** SetD2POverCMD function description
 * This function belongs to bcu,set d2p over cmd package
 *
 *  @param   pointer to send information
 *  @return  null
 */
void SetD2POverCMD(send_infomation_t *param_cmd_info);

/** D2DHangUpD2PTimeOutHandle function description
 * This function belongs to bcu,D2D hangup D2P timeout handle
 *
 *  @param   null
 *  @return  null
 */
void D2DHangUpD2PTimeOutHandle();

/** StateMachineExchange  function description
 *  This function belongs to bcu,which is used to change bcu state
 *
 *  @param   pointer to source state pointer
 *  @param   trigger signal
 *  @param   send cmd information
 *  @return  null
 */
void StateMachineExchange(state_process_t **,int ,send_infomation_t *);
extern void MoniortStateMachineExchange(state_process_t **,int,send_infomation_t *);
/** PthreadPriorityChangeForSchedRr function description
 *  Chaneg thread's priority
 *
 *  @param   thread ID
 *  @param   new thread priority
 *  @return  null
 *  @note  只有目标优先级与当前优先级不同才会执行该函数
 */
int PthreadPriorityChangeForSchedRr( pthread_t  target_thread_id, int  new_prior );

/** JudgeWhetherD2DHaveFinished function description
 *  Judge whether have finish d2d
 *
 *  @param   null
 *  @return  null
 */
void JudgeWhetherD2DHaveFinished();

/** NetFinishD2D function description
 *  对端司机结束司机对讲
 *
 *  @param   null
 *  @return  null
 */
void NetFinishD2D();

/** ExitD2DAccordingToCMU function description
 *  CMU结束司机对讲
 *
 *  @param   null
 *  @return  null
 */
void ExitD2DAccordingToCMU();

/** JudegWhetherRequestD2D function description
 *  Judge whether want to enter D2D state
 *
 *  @param   null
 *  @return  null
 */
void JudegWhetherRequestD2D();

/** SendD2DAccordingToCMU function description
 *  联挂对讲
 *
 *  @param   null
 *  @return  null
 */
void SendD2DAccordingToCMU();

/** JudgeWhetherHaveOuter3D5Triger function description
 *  判断是否有外接3D5触发信号
 *
 *  @param   null
 *  @return  null
 */
void JudgeWhetherHaveOuter3D5Triger();

/** JudgeWhetherOCCTriger function description
 *  判断是否有OCC触发信号
 *
 *  @param   null
 *  @return  null
 */
void JudgeWhetherOCCTriger(network_send_package_t recv_network_info_from_network);

/** BcuUpdateLineNumber function description
 *  Update Line numbe
 *
 *  @param   the pointer of cmd informationl
 *  @return  null
 */
int BcuUpdateLineNumber( send_infomation_t *pcmd_from_touch );

/** UpdateSkipStationInformation function description
 *  Update skip station information
 *
 *  @param   the pointer of cmd informationl
 *  @return  null
 */
void UpdateSkipStationInformation(send_infomation_t *param_cmd_info);

/** ChangeLineNumber function description
 *  Change line no between 3 and 4
 *
 *  @param   the pointer of cmd informationl
 *  @return  null
 */
void ChangeLineNumber(send_infomation_t *param_cmd_info);

/** ANNStateHangle function description
 *  Handle announcement event
 *
 *  @param   the pointer of cmd informationl
 *  @param   the pointer of cmd informationl
 *  @return  null
 */
void ANNStateHangle(send_infomation_t *,send_infomation_t *);

/** ANNStateHangle function description
 *  Handle intercom event
 *
 *  @param   the pointer of cmd informationl
 *  @param   the pointer of cmd informationl
 *  @return  null
 */
void IntercomStateHangle(send_infomation_t *,send_infomation_t *);

/** WhetherWantToExchangeState function description
 *  Judge whether want to exchange state
 *
 *  @param   cmd informationl
 *  @return  null
 */
int WhetherWantToExchangeState(send_infomation_t );

/** JudgeWhetherHaveD2DRequest function description
 *  Judge whether have D2D request from network
 *
 *  @param   pointer of send cmd informationl
 *  @param   pointer of network cmd informationlcmd
 *  @return  null
 */
void JudgeWhetherHaveD2DRequest(send_infomation_t *,network_send_package_t *);

/** SendPTTStateToBCU function description
 *  Send ptt state to pcu，ptt状态发生变化时发送
 *
 *  @param   null
 *  @return  null
 */
void SendPTTStateToPCU();

/** SendPTTStateToBCU function description
 *  Send ptt state to bcu，ptt状态发生变化时发送
 *
 *  @param   null
 *  @return  null
 */
void SendPTTStateToBCU();

/** SendPTTStateToBCU_Once function description
 *  Send ptt state to bcu，只发一次
 *
 *  @param   null
 *  @return  null
 */
void SendPTTStateToBCU_Once();

/** SendCurrentBCUPTTStateToPCU function description
 *  Send ptt state to pcu，只发与一次
 *
 *  @param   null
 *  @return  null
 */
void SendCurrentBCUPTTStateToPCU();

/** ReadOtherBCUPttState function description
 *  Acquire other bcu ptt state
 *
 *  @param   null
 *  @return  null
 */
void ReadOtherBCUPttState();

/** GetEventType function description
 *  Acquire event type
 *
 *  @param   current state pointer
 *  @param   cmd information
 *  @return  signal
 */
int GetEventType(state_process_t *,send_infomation_t );

/** SendCmd function description
 *
 *  This function is used to send cmd information
 *  @param   the point of cmd information
 *  @param   destination device name
 *  @param   destination device no
 *  @return  null
 */
void SendCmd(send_infomation_t **send_cmd_info,char *dst_device_name,int dst_device_no);

/** ShowStationInformation function description
 *
 *  This function is used to show the station information
 *  @param   line number
 *  @return  null
 */
void ShowStationInformation(int line_number_of_skip_station);

/** SetSrcDevInfo function description
 *  Set the source device information
 *
 *  @param   the name of the source device
 *  @param   the number of the source device
 *  @return  null
 */
void SetSrcDevInfo(char name[],unsigned char number);

/** SetThisDevInfo function description
 *  Set the destination device information
 *
 *  @param   the name of the destination device
 *  @param   the number of the destination device
 *  @return  null
 */
void SetThisDevInfo(char name[],unsigned char number);

/** SetSrcDevInfo function description
 *  Set the source device information
 *
 *  @param   the name of the source device
 *  @param   the number of the source device
 *  @return  null
 */
void SetSrcDevInfo(char name[],unsigned char number);

/** SetThisDevInfo function description
 *  Set the destination device information
 *
 *  @param   the name of the destination device
 *  @param   the number of the destination device
 *  @return  null
 */
void SetThisDevInfo(char name[],unsigned char number);

/** UpdataSettingSaveLiveAudioData function description
 *  Update the setting of saving live audio data
 *
 *  @param   the attribute
 *  @return  null
 */
void UpdataSettingSaveLiveAudioData(unsigned char param_data);


/** UpdataSettingSaveD2DAudioData function description
 *  Update the setting of saving d2d audio data
 *
 *  @param   the attribute
 *  @return  null
 */
void UpdataSettingSaveD2DAudioData(unsigned char param_data);

/** UpdataSettingSaveD2PAudioData function description
 *  Update the setting of saving d2p audio data
 *
 *  @param   the attribute
 *  @return  null
 */
void UpdataSettingSaveD2PAudioData(unsigned char param_data);

/** StartSaveAudioData function description
 *  Starat the function of saving audio data
 *
 *  @param   the attribut
 *  @return  null
 */
void StartSaveAudioData(int param_attribution,send_infomation_t *param_send_info,char param_event_type[]);

/** EndSaveAudioData function description
 *  Update the setting of saving d2p audio data
 *
 *  @param   the attribute
 *  @return  null
 */
void EndSaveAudioData(int param_attribution,char param_event_type[]);

/** JudgeWhetherHaveFinishedAcquireData function description
 *  Judge whether have finished get data from bcu
 *
 *  @param   null
 *  @return  null
 */
void JudgeWhetherHaveFinishedAcquireData();

/** ClearProcessPackage function description
 *  清除状态机处理函数里面的控制命令包
 *
 *  @param   控制命令包指针
 *  @return  null
 */
void ClearProcessPackage(send_infomation_t *p_send_info);

/** SendBCUPTTPressStateTOBCU function description
 *  发送BCU的PTT松开个对端BCU
 *
 *  @param   null
 *  @return  null
 */
void SendBCUPTTPressStateTOBCU();

/** SendBCUPTTReleaseStateTOBCU function description
 *  J发送BCU的PTT按下开个对端BCU
 *
 *  @param   null
 *  @return  null
 */
void SendBCUPTTReleaseStateTOBCU();

/** ClearSkipStationInfo function description
 *  清除越站设置
 *
 *  @param   线路号
 *  @return  null
 */
void ClearSkipStationInfo(int line_number_of_skip_station);

/** ControlCommomPackageInit function description
 *  控制线程通用包初始化
 *
 *  @param   发给EAMP包指针
 *  @param   发给PCU包指针
 *  @param   发给BCU包指针
 *  @param   发给CCU包指针
 *  @return  null
 */
void ControlCommomPackageInit(common_package_t *p_to_eamp,common_package_t *p_to_pcu,common_package_t *p_to_bcu,common_package_t *p_to_ccu);

/** GetPTTState function description
 *  获取PTT状态
 *
 *  @param   null
 *  @return  PTT状态
 */
int GetPTTState();

/** GetBCUNetworkState function description
 *  获取BCU网络状态
 *
 *  @param   null
 *  @return  null
 */
int GetBCUNetworkState();


/** SetCurrentDeviceNetworkState function description
 *  Set this device network state
 *
 *  @param   param_dev_net_state:this device network state(0:network is offline  1:network is ok)
 *  @return  if success,return 0,else return -1
 */
extern int GetCurrentDeviceNetworkState();
extern int SetCurrentDeviceNetworkState(int param_dev_net_state);
/*---------------------------end of bcu beta 1.1-------------------------------*/
#ifdef TEST_FOR_SHENGYANG
int BcuNoticePisStationCode_ChangeLineNumber(int network_send_to_pis_buffer,int temp_line_number);
int BcuNoticePisStationCode_Manual(int network_send_to_pis_buffer, void *received_cmd_ptr);
#endif

/** OpenMonitorSpeaker function description
 *  打开监听扬声器
 *
 *  @param   null
 *  @return  null
 */
void OpenMonitorSpeaker();

/** CloseMonitorSpeaker function description
 *  关闭监听扬声器
 *
 *  @param   null
 *  @return  null
 */
void CloseMonitorSpeaker();

/** ShowCurrentTime function description
 *  显示当前时间
 *
 *  @param   时间标号
 *  @return  null
 */
void ShowCurrentTime(int param_no);

/** ShowRegionInfo function description
 *  显示区间信息
 *
 *  @param   线路号
 *  @return  null
 */
void ShowRegionInfo(int param_line_no);

/** ShowAllRegionInfo function description
 *  显示所有区间信息
 *
 *  @param   线路号
 *  @return  null
 */
void ShowAllRegionInfo(int param_line_no);

/** CreateDelayD2DTimer function description
 *  创建司机对讲延时定时器，目前没有使用
 *
 *  @param   null
 *  @return  null
 */
void CreateDelayD2DTimer();

/** StartDelayD2DTimer function description
 *  启动司机对讲延时定时器，目前没有使用
 *
 *  @param   null
 *  @return  null
 */
void StartDelayD2DTimer();

/** CreateGetOut3D5ButtonStateTimer function description
 *  创建获取外接3D5定时器，目前没有使用
 *
 *  @param   null
 *  @return  null
 */
void CreateGetOut3D5ButtonStateTimer();

/** StartDelayD2DTimer function description
 *  停止司机对讲延时定时器，目前没有使用
 *
 *  @param   null
 *  @return  null
 */
void StopDelayD2DTimer();

/** GetOuterButtonStateOnTCMS function description
 *  锁屏下获取按钮状态，进行相应的操作
 *
 *  @param   null
 *  @return  null
 */
void GetOuterButtonStateOnTCMS();

/** GetOuterButtonState function description
 *  开屏下获取按钮状态，进行相应的操作
 *
 *  @param   null
 *  @return  null
 */
void GetOuterButtonState();

/** GetOuterButtonState function description
 *  开屏下获取D2P按钮状态，进行相应的操作
 *
 *  @param   null
 *  @return  null
 */
void JudegD2PButton();

/** GetOuterButtonStateOnTCMS function description
 *  锁屏下获取D2P按钮状态，进行相应的操作
 *
 *  @param   null
 *  @return  null
 */
void JudegD2PButtonTCMS();

/** ForceToIdle function description
 *  强制切换到空闲状态
 *
 *  @param   当前状态ID
 *  @return  null
 */
void ForceToIdle(int param_current_event_state_id);

/** SendCMDToEAMP function description
 *  发送当前状态给EAMP
 *
 *  @param   当前状态ID
 *  @return  null
 */
void SendCMDToEAMP(int param_current_event_state_id);

/** RefuseAllPCUReqest function description
 *  拒绝所有PCU请求
 *
 *  @param   null
 *  @return  null
 */
void RefuseAllPCUReqest();

/** ClearAllPCURequestInfo function description
 *  清除所有PCU请求
 *
 *  @param   null
 *  @return  null
 */
void ClearAllPCURequestInfo();


/** SendToOverOtherBCUD2D function description
 *  发送司机对讲结束给对方
 *
 *  @param   null
 *  @return  null
 */
void SendToOverOtherBCUD2D();

/** ForceBreakD2DToD2P function description
 *  强制结束司机对讲，返回到司机乘客对讲
 *
 *  @param   null
 *  @return  null
 */
void ForceBreakD2DToD2P();

/** NetFinishD2D function description
 *  对端BCU结束司机对讲
 *
 *  @param   null
 *  @return  null
 */
void NetFinishD2D();

/** judgeWheteherExitD2DEnterD2P function description
 *  判断是否结束司机对讲进入紧急对讲
 *
 *  @param   null
 *  @return  null
 */
void judgeWheteherExitD2DEnterD2P();

/** UpdateDeviceState function description
 *  更新设备状态
 *
 *  @param   控制命令包
 *  @return  null
 */


void SendThisBCUAnnStateToEAMP();

/** alarm_func_handle_key_info function description
 *  钥匙信息查询定时器处理函数
 *
 *  @param   定时器句柄
 *  @param   定时器私有数据
 *  @return  null
 */
void alarm_func_handle_key_info(cyg_handle_t counter_handle, cyg_addrword_t data);

/** CreateAskKeyInfoTimer function description
 *  创建钥匙信息查询定时器
 *
 *  @param   null
 *  @return  null
 */
void CreateAskKeyInfoTimer();

/** EnableAskKeyInfoTimer function description
 *  使能钥匙信息查询定时器
 *
 *  @param   null
 *  @return  null
 */
void EnableAskKeyInfoTimer();

/** ReadNumWilson function description
 *  获取设备号
 *
 *  @param   null
 *  @return  设备号
 */
int ReadNumWilson();

/** SendNoPCUToCCU function description
 *  发送当前没有PCU请求包给CCU，退出监控联动
 *
 *  @param   null
 *  @return  null
 */
void SendNoPCUToCCU();

/** BCUSendDevVolumeInfoToPAOtherDev function description
 *  发送音量信息给广播设备
 *
 *  @param   null
 *  @return  null
 */
void BCUSendDevVolumeInfoToPAOtherDev();

/** BCUSendDevVolumeInfoToD2POtherDev function description
 *   发送音量信息给紧急对讲设备
 *
 *  @param   null
 *  @return  null
 */
void BCUSendDevVolumeInfoToD2POtherDev();

/** BCUSendDevVolumeInfoToD2DOtherDev function description
 *   发送音量信息给司机对讲设备
 *
 *  @param   null
 *  @return  null
 */
void BCUSendDevVolumeInfoToD2DOtherDev();



/** RequestBCUKeyInfo function description
 *  请求钥匙信号
 *
 *  @param   null
 *  @return  null
 */
void RequestBCUKeyInfo();

/** UpdateTSGetDBInfoState function description
 *  更新BCU从CCU获取数据库状态
 *
 *  @param   null
 *  @return  null
 */
void UpdateTSGetDBInfoState();

/** AskCCUTcmsCMUState function description
 *  向CCU 查询TCMS 当前连挂状态
 *
 *  @param   null
 *  @return  null
 */
void AskCCUTcmsCMUState();
void ChangeTSAccordingCurrentCMUFlag();
void ChangeTSAccordingCurrentTCMSFlag();

//add by panyao 2015-12-15
void SimlateAtuoANNInofFill(send_infomation_t *param_send_package);
void StartSimlateAnn();
void CloseAUTOSIMTimer();
void RestartAUTOSIMTimer();
void CreateAUTOSIMTimer();
//end of add
int TransformIntercomPackage(network_send_package_t *p_recv_network_info);
int TransformSendPackage(network_pcu_t *p_temp_pcu_network_package,network_send_package_t *p_recv_network_info);
int ProbeBigCommPackage(const common_big_package_t *p_BigConmInfo);
#endif	/*BCU_FUNCTION_H*/

