/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： touch_screen_function.h
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
#ifndef TSFunction_h
#define TSFunction_h
#include "ts_head_file.h"
#include "ts_simulate.h"
#include"../link.h"


extern char password_buffer[10];
extern char monitor_volume_digit_2_string_buffer[10];
extern char car_volume_digit_2_string_buffer[10];
extern char intercomm_volume_digit_2_string_buffer[10];
extern char d2d_intercomm_volume_digit_2_string_buffer[10];



extern unsigned char d2p_reponse_sursor;
extern unsigned int gwCurrCarNo;
extern unsigned int gwCurrIphNO;
extern unsigned int gwCurrBcuNo;
extern unsigned int gwCurD2dCarNo;
extern Fl_Group *gp_main_file_active_page;
extern Fl_Group *p_current_ann_group;
extern Fl_Group *p_current_intercomm_group ;
extern unsigned char whether_intercomm_button_is_active;
extern unsigned char select_car_recoed;

/** SetPassword function description
 *   输入用户密码
 *
 *  @param   密码位数
 *  @param   密码缓存区
 *  @return  null
 */
void SetPassword(const int password_number,char param_password_buffer[]);

/** ShowPasswordMask function description
 *   设置用户输出密码显示为*
 *
 *  @param   密码位数
 *  @return  null
 */
void ShowPasswordMask(int number);

/** ShowTitleOfInputPassword function description
 *   根据用户输入的密码正确与否显示相应的提示信息
 *
 *  @param   密码正确与否
 *  @return  null
 */
void ShowTitleOfInputPassword(int param_title_type);

/** RecovoryBtnState function description
 *   恢复广播按钮状态
 *
 *  @param   当前状态按钮索引
 *  @return  null
 */
void RecovoryBtnState(int index);

/** ChangeBtnState function description
 *   设置广播按钮状态
 *
 *  @param   当前状态按钮索引
 *  @return  null
 */
void ChangeBtnState(int index);

/** SetLineAndRegionNoCmd function description
 *   设置区间号命令包
 *
 *  @param   线路号
 *  @param   区间号
 *  @return  null
 */
void SetLineAndRegionNoCmd(int param_line_no,int param_region_no);

/** ExchangeBtnPosition function description
 *   根据优先级改变按钮位置信息
 *
 *  @param   null
 *  @return  null
 */
void ExchangeBtnPosition();

/** ShowTMSInfoOnStaticPage function description
 *   在静态显示界面线索TCMS信息
 *
 *  @param   null
 *  @return  null
 */
void ShowTMSInfoOnStaticPage();

/** ShowWholeTmsTip function description
 *   显示整个TCMS 信息，目前尚未使用
 *
 *  @param   null
 *  @return  null
 */
void ShowWholeTmsTip();

/** UpdataTmsTipInfo function description
 *   更新TCMS信息
 *
 *  @param   TCMS显示界面页码
 *  @return  null
 */
void UpdataTmsTipInfo(int param_current_page_number);


/** ShowCarFailure function description
 *   显示车厢设备故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowCarFailure();


/** ShowRegion function description
 *   显示区间信息
 *
 *  @param   区间游标
 *  @return  null
 */
void ShowRegion(int param_cursor);

/** SetRegionInfo function description
 *   设置区间信息
 *
 *  @param   区间游标
 *  @return  null
 */
void SetRegionInfo(int param_region_no);

/** ShowRegiontStation function description
 *   设置区间站代码信息
 *
 *  @param   起点站代码
 *  @param   终点站代码
 *  @param   当前站代码
 *  @param   下一站代码
 *  @paeam   上下行
 *  @return  null
 */
void ShowRegiontStation(int p_begin_station_no,int p_terminal_station_no,int p_cur_station_no,int p_next_station_no,int p_running_direction);

/** UpdateStationAccordingToRegionNumber function description
 *  根据区间号设置区间信息
 *
 *  @param  区间号
 *  @return  null
 */
void UpdateStationAccordingToRegionNumber(int p_region_number);

/** SetSelectStationPageInfo function description
 *  系统区间选择
 *
 *  @param  起点站代码
 *  @param  终点站代码
 *  @return  null
 */
void SetSelectStationPageInfo(int p_b_s,int p_t_s);

/** PrintSystemStationInfo function description
 *  打印系统站信息
 *
 *  @param  null
 *  @return  null
 */
void PrintSystemStationInfo();

/** AdjustEmergInfo function description
 *  调节紧急广播显示内容
 *
 *  @param  null
 *  @return  null
 */
void AdjustEmergInfo();

/** ClearSkipStationSetting function description
 *  清除越站信息设置
 *
 *  @param  null
 *  @return  null
 */
void ClearSkipStationSetting();

/** PrintSkipStationSetting function description
 *  打印越站信息
 *
 *  @param  null
 *  @return  null
 */
void PrintSkipStationSetting();

/** BCUActivejudge function description
 *   BCU激活端判断
 *
 *  @param   null
 *  @return  null
 */
void  BCUActivejudge();

/** SystemInitBeforeRun function description
 *   触摸屏界面创建之后但没有开始运行之间系统初始化
 *
 *  @param   null
 *  @return  null
 */
void SystemInitBeforeRun();

/** UpdateRegionInfo function description
 *   更新区间信息
 *
 *  @param   null
 *  @return  null
 */
void UpdateRegionInfo();

/** ShowMainPage function description
 *   显示主菜单界面
 *
 *  @param   null
 *  @return  null
 */
void ShowMainPage();

/** UpdateEmergInfo function description
 *   更新紧急广播信息
 *
 *  @param   null
 *  @return  null
 */
void UpdateEmergInfo();

/** UpdatePCUInfo function description
 *   更新PCU信息
 *
 *  @param   null
 *  @return  null
 */
void UpdatePCUInfo();

/** TSInitAfterGetDataFromCCU function description
 *   数据库获取数据库之后初始化操作
 *
 *  @param   null
 *  @return  null
 */
void TSInitAfterGetDataFromCCU();

/** ShowRunningDirection function description
 *   显示运行方向
 *
 *  @param   null
 *  @return  null
 */
void ShowRunningDirection();

/** CalculateNextStationDuringSetRegion function description
 *   设置区间时计算下一站
 *
 *  @param   线路号
 *  @param   当前站
 *  @param   区间位置
 *  @return  下一站站代码
 */
int CalculateNextStationDuringSetRegion(int param_cur_line_no,int param_cur_station_no,int param_cur_region_position);

/** CalculateNextStationNo function description
 *   计算下一站站代码
 *
 *  @param   当前站站代码
 *  @return  下一站站代码
 */
int CalculateNextStationNo(int param_station_no);

/** ShowWaitGettingDataPage function description
 *   显示数据获取界面
 *
 *  @param   null
 *  @return  null
 */
void ShowWaitGettingDataPage();

/** ClearWaitGettingDataPage function description
 *   隐藏数据获取界面
 *
 *  @param   null
 *  @return  null
 */
void ClearWaitGettingDataPage();

/** InitBCreateBCUEmergSpaceCUEmergInfo function description
 *   创建紧急广播信息空间
 *
 *  @param   null
 *  @return  null
 */
void CreateBCUEmergSpace(int param_emerg_total_items);

/** InitBCUEmergInfo function description
 *   初始化紧急广播信息
 *
 *  @param   null
 *  @return  null
 */
void InitBCUEmergInfo();

/** CreateBCURegionSpace function description
 *   创建BCU 区间信息空间
 *
 *  @param   线路号
 *  @param   总区间数
 *  @return  null
 */
void CreateBCURegionSpace(int param_line_no,int param_total_items);

/** InitBCURegionInfo function description
 *   初始化BCU 区间信息
 *
 *  @param   null
 *  @return  null
 */
void InitBCURegionInfo();

/** GetCurrentregionPosition function description
 *   获取区间号对应的位置
 *
 *  @param   区间号
 *  @return  null
 */
void GetCurrentregionPosition(unsigned int param_re3gion_no);

/** ShowTSSkipStationInfo function description
 *   显示越站信息
 *
 *  @param   null
 *  @return  null
 */
void ShowTSSkipStationInfo();

/** SyncSkipStationInfo function description
 *   同步越站信息
 *
 *  @param   线路号
 *  @return  null
 */
void SyncSkipStationInfo(int param_line_no);

/** ForceExitLatestState function description
 *   强制退出当前状态
 *
 *  @param   null
 *  @return  null
 */
void ForceExitLatestState();

void ForceExitLiveStateBeforeIntercomm();

/** ForceIntoD2D function description
 *   强制切换到D2D
 *
 *  @param   null
 *  @return  null
 */
void ForceIntoD2D();

/** ShowDeviceVolumeInfo function description
 *   显示设备音量界面信息
 *
 *  @param   设备音量设置类型
 *  @return  null
 */
void ShowDeviceVolumeInfo(int param_select_item);

/** UpdateDeviceVolumePageInfo function description
 *   更新设备音量界面信息
 *
 *  @param   设备音量设置类型
 *  @return  null
 */
void UpdateDeviceVolumePageInfo(int param_select_item);

/** SetDeviceVolumeCMDPackage function description
 *   设置设备音量通信包
 *
 *  @param   设备类型
 *  @param   设备音量
 *  @return  null
 */
void SetDeviceVolumeCMDPackage(int param_device_type,int param_device_volume);


/** WhetherThisPCUHaveBeingRequest function description
 *   当前PCU是否已经请求判断
 *
 *  @param   PCU设备号
 *  @return  当前PCU已经请求返回1否则返回0
 */
int WhetherThisPCUHaveBeingRequest(int param_source_device_number);

/** ShowTMSAutoTMSInfoOnStaticPage function description
 *   静态界面显示TCMS子女在
 *
 *  @param   TCMS网络控制命令
 *  @return  null
 */
void ShowTMSAutoTMSInfoOnStaticPage(network_send_package_t param_tms_auto_info);

/*-------------------在触摸屏控制线程中使用------------*/
/** ChangeIntercommButtonColor function description
 *   改变紧急对讲按钮状态
 *
 *  @param   是否已经切换过标志位
 *  @return  null
 */
void ChangeIntercommButtonColor(int *param_whether_ts_is_running);

/** ShowArbitratePage function description
 *   显示仲裁界面
 *
 *  @param   事件ID
 *  @return  null
 */
void ShowArbitratePage(int param_event_id);

/** ShowOCCOR3D5Btn function description
 *   显示occ或者3d5按钮
 *
 *  @param   事件ID
 *  @param   进入或者退出
 *  @return  null
 */
void ShowOCCOR3D5Btn(int param_event_id,int param_enter_or_over);

/** HideArbitratePage function description
 *   隐藏仲裁界面
 *
 *  @param   null
 *  @return  null
 */
void HideArbitratePage();

/** SetCurActiveAnnPage function description
 *   设置当前有效的ann界面
 *
 *  @param   当前状态ID
 *  @return  null
 */
void SetCurActiveAnnPage(int param_event_id);

/** AdjustLineInfo function description
 *   调整线路号显示按钮信息
 *
 *  @param   线路号信息
 *  @return  null
 */
void AdjustLineInfo(int param_line_no);

/** ExchangeToManual function description
 *   进入手动广播，有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExchangeToManual();

/** ExchangeToLive function description
 *   进入紧急广播，有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExchangeToEmerg();


/** ExchangeToLive function description
 *   进入口播，有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExchangeToLive();

/** ExchangeToLiveWithoutTS function description
 *   进入口播，没有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExchangeToLiveWithoutTS();

/** ExitLive function description
 *   退出口播，有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExitLive();


void SyncSelectCarInfo();

void UpdateSelectCarInfo(unsigned int car_no);

/** ExitLiveWithoutTS function description
 *   退出口播，没有触摸屏操作
 *
 *  @param   null
 *  @return  null
 */
void ExitLiveWithoutTS();

/** ShowD2Ppage function description
 *   显示D2P界面
 *
 *  @param   null
 *  @return  null
 */
void ShowD2Ppage();

/** RecvD2PRequest function description
 *   接受D2P请求
 *
 *  @param   null
 *  @return  null
 */
void RecvD2PRequest();

/** RefuseD2PRequest function description
 *   拒绝D2P请求
 *
 *  @param   null
 *  @return  null
 */
void RefuseD2PRequest();

/** GetIntercommButtonState function description
 *   获取紧急对讲按钮状态
 *
 *  @param   null
 *  @return  紧急对讲按钮状态
 */
unsigned char GetIntercommButtonState();

/** ExitEmerg function description
 *   退出紧急广播
 *
 *  @param   null
 *  @return  null
 */
void ExitEmerg();

/** ExitManual function description
 *   退出手动报站
 *
 *  @param   null
 *  @return  null
 */
void ExitManual();

/** RecoverIdleButtonState function description
 *   系统返回到空闲状态，恢复相关按钮显示状态
 *
 *  @param   null
 *  @return  null
 */
void RecoverIdleButtonState();

/** DisableThisBCUD2PFunction function description
 *   失效D2P功能
 *
 *  @param   null
 *  @return  null
 */
void DisableThisBCUD2PFunction();

/** EnableThisBCUD2PFunction function description
 *   恢复D2P功能
 *
 *  @param   null
 *  @return  null
 */
void EnableThisBCUD2PFunction();

/** BCUDisableAnnFunctionAccordingToTCMS function description
 *   TCMS有效，退出非自动报站状态
 *
 *  @param   null
 *  @return  null
 */
void BCUDisableAnnFunctionAccordingToTCMS();

/** UpdateTSDevFailureInfo function description
 *   更新触摸屏设备故障界面信息
 *
 *  @param  设备故障状态
 *  @return  null
 */
void UpdateTSDevFailureInfo();
void UpdateTSDevFailureInfoForce();

/** ChangeFailureStatisticsButtonColor function description
 *   改变故作统计按钮颜色
 *
 *  @param  设备故障状态
 *  @return  null
 */
void ChangeFailureStatisticsButtonColor(unsigned char param_current_state);

/** ShowSDUFailure function description
 *   显示SDU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowSDUFailure();

/** ShowFDUFailure function description
 *   显示FDU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowFDUFailure();

/** ShowIDUFailure function description
 *   显示IDU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowIDUFailure();

/** ShowNDFailure function description
 *   显示噪检故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowNDFailure();

/** ShowDMFailure function description
 *   显示动态地图故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowDMFailure();

/** ShowBCUFailure function description
 *   显示BCU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowBCUFailure();

/** ShowPCUFailure function description
 *   显示PCU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowPCUFailure();

/** ShowCCUFailure function description
 *   显示CCU故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowCCUFailure();

/** ShowEAMPFailure function description
 *   显示EAMP故障信息
 *
 *  @param   null
 *  @return  null
 */
void ShowEAMPFailure();

/** UpdateVolumeInfo function description
 *   更新音量信息
 *
 *  @param   音量类型代码
 *  @return  null
 */
void UpdateVolumeInfo(int param_update_flag);

/** AlarmTSToChangeScreen function description
 *   通知触摸屏线程，控制线程需要改变触摸屏相关界面
 *
 *  @param   更新界面操作代码
 *  @return  null
 */
void AlarmTSToChangeScreen(int param);

/** SetD2PTipButtonState function description
 *   设置紧急对讲按钮状态
 *
 *  @param   当前紧急对讲按钮是否有效
 *  @return  null
 */
void SetD2PTipButtonState(unsigned char param);

/** ChangeLineButtonLabel function description
 *   改变线路号切换按钮文字--根据当前线路号信息
 *
 *  @param   null
 *  @return  null
 */
void ChangeLineButtonLabel();

/** EnableTS function description
 *   触摸屏操作有效
 *
 *  @param   null
 *  @return  null
 */
void EnableTS();

/** DisableTS function description
 *   触摸屏操作失效
 *
 *  @param   null
 *  @return  null
 */
void DisableTS();

/** UpdateConcereteDevFailueInfo function description
 *  更新详细的设备故障
 *
 *  @param   设备代码
 *  @return  null
 */
void UpdateConcereteDevFailueInfo(int flag);

/** SendConnectTwoCarsStateToOtherBCU function description
 *  告知对端BCU 当前BCU 联挂状态
 *
 *  @param   老车类型
 *  @param   标志
 *  @return  null
 */
void SendConnectTwoCarsStateToOtherBCU(unsigned char state,unsigned char flag);

/** ShowDeviceFailurePageButtonNameInfo function description
 *  根据故障统计界面详细设备类型，来改变故障界面按钮上显示的名字以及隐藏或者显示按钮
 *
 *  @param   详细故障设备类型
 *  @return  null
 */
void ShowDeviceFailurePageButtonNameInfo(unsigned char flag);

extern void Disable_D2p_All_Btn();

extern int G_SetAndClearPakage(int op,int pn,send_infomation_t *temp);

extern int Select_car_count(unsigned int vn, int op,common_big_package_t  *parame,unsigned int *count);

extern int SetMonitorBigPakage(int iph,unsigned int vn);

extern int CannelMonitorBigPakage(int iph,unsigned int vn);

extern int SetIntercomBigPackage(int vn,int iph,common_big_package_t  *parame);

extern int sync_select_page(unsigned char *flag);

extern int sum_select_car();

extern void chack_slecet_car_sum_enable_btn();

extern int parse_btn_lable_value(const char *src,unsigned char *dst,unsigned char *vn);

extern int parse_btn_lable_value_bcu(const char *src,int *dst_device,int *dst_vn);

extern void SetD2dCmdPackage(unsigned int vn,unsigned int bcu_no,send_infomation_t *param_send_infomation);

extern void SetD2dRefuseCmdPackage(unsigned int vn,unsigned int bcu_no,send_infomation_t *param_send_infomation);

extern void send_cannel_d2d_big_package(unsigned int vn,unsigned int bcu_no);


#endif
