/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：data_struct.h
 * 文件标识：
 * 摘    要：控制信息结构体
 *
 * 当前版本：V1.0.3
 * 作    者：wilson
 * 完成日期：2012-10-08
 *
 * 取代版本：V1.0.2
 * 原作者  ：wilson
 * 完成日期：2012-9-27
 *
 *  取代版本：V1.0.1
 * 原作者  ：wilson
 * 完成日期：2012-8-25
 *
 * 取代版本：V1.0.0
 * 原作者  ：周魏
 * 完成日期：2012-08-23
 */

#ifndef DATA_STRUCT_H_
#define DATA_STRUCT_H_
#include "struct_type.h"
#include "dynamic_map.h"
#include "tms_module.h"
#include "ts_type.h"

//#define CONFIG_TEST_SND_IN_MULTI_THREAD   ///< debug for sound_card, 01-10

#ifdef  CONFIG_BCU_SYNC_FROM_CCU_NET
#include "bcu_get_data_from_ccu_type.h"
#endif

/** 本联合体功能：广播事件信息标识
 *  本联合体包含所有广播事件信息，但每时刻只能标识一个事件信息
 */
typedef union
{

	live_ann_t live_announce;   				///<口播状态
	unsigned char r[208];

}event_infomation_ann_t;

/** 本联合体功能：对讲事件信息标识
 *  本联合体包含所有对讲事件信息，但每时刻只能标识一个事件信息
 */
typedef union
{
	d2d_intercomm_t d2d_intercomm;			///<司机对讲
	d2p_intercomm_t d2p_intercomm;			///<乘客对讲
}event_infomation_intercomm_t;


typedef struct
{

unsigned int event_type;
unsigned int car_no;
unsigned int car_state[11];
unsigned char affirm;
unsigned char cancel;
unsigned int reserve[4];

}select_vehicle_info;


/** 本结构体功能：广播控制盒状态
 *	本结构体主要是标识广播控制盒状态信息
 */
typedef struct
{
	  char devices_name[DEVICE_NAME_LENGTH];					///<设备名称
	  int devices_no;												///<设备编号
	  unsigned char current_state_ann;	
	  unsigned char  request_sequence;						///<设备广播当前状态
	  event_infomation_ann_t event_infomation_ann;				///<设备广播信息
	  unsigned char current_state_intercom;						///<设备对讲当前状态
	  event_infomation_intercomm_t event_infomation_intercom;	///<设备对讲信息
	  int time;	///<时间
	  unsigned int config_db_version;  ///<add, 2013
	  unsigned int ann_db_version;  ///<add, 2013	
	  unsigned char driver_key_inserted; ///<ach add, 2013
	  unsigned char is_db_server; ///<ach add, 2013
	  unsigned int state_count;  ///<add, 2013	 
}device_bcu_state_t;

/** 本结构体功能：功放状态
 *	本结构体主要是标识功放状态信息
 */
typedef struct
{
	  char devices_name[DEVICE_NAME_LENGTH];				///<设备名称
	  int devices_no;											///<设备编号
	  unsigned char current_state;							///<设备当前状态
	  event_infomation_ann_t event_infomation_ann; 		///<设备广播信息
	  int time;												///<时间
	  unsigned int config_db_version;  ///<add, 2013
	  unsigned int ann_db_version;  ///<add, 2013	
	  unsigned char is_db_server; ///<ach add, 2013
	  unsigned int state_count;  ///<add, 2013
}device_eamp_state_t;

/** 本结构体功能：乘客报警器状态
 *	本结构体主要是标识乘客报警器状态信息
 */
typedef struct
{
	char dst_devices_name[32];			//目的设备名	0-31
	int dst_devices_no;					//目的设备号	32-35
	int  times;							//36-39
	unsigned char monitor;
	unsigned char reserve_00[7];					//保留	41-47
	char src_devices_name[32];			//源设备名	48-79
	unsigned char src_devices_no;		//源设备号	80
	unsigned char event_ptt_state;		//PTT状态	81
	unsigned char event_type_ann;		//事件类型	82
	unsigned char reserver_01;			//保留	83
	int reserver_02[52];					//保留	84-291
	unsigned char event_type_intercom;	//事件类型	292
	unsigned char d2p_intercomm_active;			//对讲状态请求	293
	unsigned char d2p_intercomm_request_or_over;	//对讲请求或结束	294
	unsigned char d2p_intercomm_pending;			//对讲挂起	295
	unsigned char d2p_intercomm_response;			//对讲响应	296
	unsigned char d2p_intercomm_bcu_device_no;	//对讲目标设备号	297
	unsigned char d2p_intercomm_pcu_device_no;	//对讲目标设备号	298
	unsigned char d2p_intercomm_whether_is_connecting;	//当前PCU是否已经连接中	299
	unsigned char reserver_03[20];							//保留	300-319
}network_pcu_t;

typedef struct
{
	  char devices_name[DEVICE_NAME_LENGTH];						///<设备名称
	  int devices_no;													///<设备编号
	  unsigned char current_state_intercom;							///<设备当前状态,wilson 2012-10-26 current_state->current_state_intercom
	  event_infomation_intercomm_t event_infomation_intercom;		///<设备对讲信息
	  unsigned char  request_sequence;														///<时间
	  unsigned int config_db_version;  ///<add, 2013
	  unsigned int ann_db_version;  ///<add, 2013	
	  unsigned char is_db_server; ///<ach add, 2013
	  unsigned int state_count;  ///<add, 2013
}device_pcu_state_t;

/** 本结构体功能：中央控制器状态
 *	本结构体主要是标识中央控制器状态信息
 */
typedef struct
{
	  char devices_name[DEVICE_NAME_LENGTH];	///<设备名称
	  int devices_no;								///<设备编号
	  unsigned char current_state;				///<设备当前状态
	  int time;									///<时间
	  unsigned int config_db_version;  ///<add, 2013
	  unsigned int ann_db_version;  ///<add, 2013	
	  unsigned char is_db_server; ///<ach add, 2013
	  unsigned int state_count;  ///<add, 2013
}device_ccu_state_t;

//add begin, 2013
#define GD_VALUE_MAX_MACRO(a,b)	((a)>=(b)?(a):(b))
#define TMP_MAX_A (GD_VALUE_MAX_MACRO(sizeof(device_eamp_state_t), sizeof(device_pcu_state_t)))
#define TMP_MAX_B (GD_VALUE_MAX_MACRO(TMP_MAX_A, sizeof(device_bcu_state_t)))
#define TMP_MAX_C	(GD_VALUE_MAX_MACRO(TMP_MAX_B, sizeof(device_ccu_state_t)))

#ifdef CONFIG_BCU_SYNC_FROM_CCU_NET
#define GD_DEV_INFO_MAX_LEN	((GD_VALUE_MAX_MACRO(TMP_MAX_C, sizeof(bcu_get_db_data_from_ccu_t)))+4) ///< last 4 bytes are CRC
#else
#define GD_DEV_INFO_MAX_LEN	(TMP_MAX_C+4) ///< last 4 bytes are CRC
#endif

//add end, 2013


/** 本结构体功能：包含所有的控制信息
 *	本结构体是所有设备决定接下来操作的依据
 *	主要用来传递不同设备之间的相互请求
 */
typedef struct
{
	void (*init)(void);								//结构体初始化
	void (*setinformation)(unsigned char event_type,
								char *src_devices_name,
								unsigned char src_devices_no,
								unsigned char state_request,
								unsigned char request_or_over,
								unsigned char line_number,
								unsigned char region_number,
								unsigned int broadcast_sequence,
								unsigned char broadcast_times,
								unsigned char arrived_station,
								unsigned char current_station_code,
								unsigned char station_effective,
								unsigned char whether_pre_read,
								unsigned char dst_device_number
								);		///<填充该结构体

	char src_devices_name[DEVICE_NAME_LENGTH];		///<源设备名
	unsigned char src_devices_no;					///<源设备号
	unsigned char event_ptt_state;
	unsigned char event_type_ann;//80
	unsigned char reserve1[3];
	event_infomation_ann_t event_info_ann;      	///<事件信息
	unsigned char event_type_intercom;				///<事件类型
	event_infomation_intercomm_t event_info_intercom;///<事件信息
	int  sequence;  ///<包的序列号;
	unsigned char reserve[12];
}send_infomation_t;

/** 本结构体功能：将控制信息进行二次打包，便于网络发送
 *	本结构体是在控制结构体的基础上添加目标设备名和目标设备号
 *	主要用来在网络上传递
 */

#pragma pack(4)

typedef struct{
	unsigned int  	 seat_id;
	unsigned int  	 car_no;
	unsigned int     iph_requset_no;
	unsigned int     iph_receive_no;
	unsigned int     iph_refuse_no;
	unsigned char   select_enable_flag;
	unsigned char 	car_select_flag[11];
	unsigned char 	iph_select_flag[8];
	unsigned char  	car_occupy_state[11];//占用 1,空闲 0
	unsigned char  	iph_occupy_state[8];//占用 1,空闲 0
	unsigned char   ann_event_flag;     //
	unsigned char   monitor_event_flag;
	unsigned char   intercom_event_flag;
	unsigned char   active_bcu_no[5];
}common_big_data_t;

typedef struct
{
	char dst_dev_name[DEVICE_NAME_LENGTH];//OCS
	int dst_dev_number;					//1
    char src_dev_name[DEVICE_NAME_LENGTH];//GBCU
	int src_dev_number;						//1-5
	int pkg_type; //1,选车（地面口播），2选车+IPH（地面监听），3选车+选IPH（地面紧急对讲）
				  //4.选车（ISCS口播），5.选车+IPH（ISCS监听） ，6.选车+选IPH（ISCS紧急对讲）
			      //7.sync 8,requset iph,9,recv iph,10,refuse iph;
 	common_big_data_t common_big_data_u;
}common_big_package_t;



#pragma pack()
typedef struct
{
	char dst_devices_name[DEVICE_NAME_LENGTH];		///<目的设备名
	int  dst_devices_no;								///<目的设备号
	int  time;
	send_infomation_t send_information;				///<需要发送到的控制信息
}network_send_package_t;

/** 本结构体功能：保存列车所有设备的当前信息
 *	本结构体包含来列车上所有设备的当前信息
 *	主要用来想触摸屏提供不同设备的信息
 */
typedef struct
{
	void (*init)(void);
	void (*setglobalinfo)(send_infomation_t);
	device_bcu_state_t  device_bcu[CONFIG_BCU_NUM + 1];    ///<列车广播控制盒状态
	device_pcu_state_t device_pcu[CONFIG_PECU_NUM + 1];   ///<列车乘客报警器状态
}global_device_info_state_t;


// 设备的整体状态信息
typedef struct eamp_dev_ifo
{
	int set_online; // add by zhw-2014-11-24
	int reply;
	int dev_number;
	eamp_info_t eamp_info;
	struct eamp_dev_ifo *next;
}eamp_dev_info_t;

typedef struct bcu_dev_info
{
	int set_online;
	int reply;
	int dev_number;
	bcu_info_t bcu_info;
	struct bcu_dev_info *next;
}bcu_dev_info_t;

typedef struct ccu_dev_info
{
	int set_online;
	int reply;
	int dev_number;
	ccu_info_t ccu_info;
	struct ccu_dev_info *next;
}ccu_dev_info_t;

typedef struct pcu_dev_info
{
	int set_online;
	int reply;
	int dev_number;
	pcu_info_t pcu_info;
	struct pcu_dev_info *next;
}pcu_dev_info_t;

typedef struct cmu_dev_info
{
	int set_online;
	int reply;
	int dev_number;
	cmu_info_t cmu_info;
	struct cmu_dev_info *next;
}cmu_dev_info_t;

typedef struct
{
	eamp_dev_info_t *eamp_dev_info_head;
	bcu_dev_info_t *bcu_dev_info_head;
	ccu_dev_info_t *ccu_dev_info_head;
	pcu_dev_info_t *pcu_dev_info_head;
	cmu_dev_info_t *cmu_dev_info_head;
}dev_status_info_t;


typedef struct PttStateType
{
      unsigned char dst_ptt;
      unsigned char src_ptt;
}ptt_state_t;

typedef struct device_volume_info
{
	char device_name[DEVICE_NAME_LENGTH];
	int device_no;
	int device_volume;
	int device_adjust_volume;
	int device_no_recept_timeout;
}ts_dev_volume_info_t;
typedef struct
{
	char monitor_speaker_state;
	char monitor_speaker_volume;
}monitor_speaker_control_t;

typedef struct
{
	unsigned char bcu_log_info[200];
}bcu_log_info_t;
typedef struct
{
	unsigned char intercomm_type;///< 0:d2d 1:d2p 2:d2d break up d2p
	unsigned char src_no;
	unsigned char dst_no;
	unsigned char begin_or_over;
}intercomm_audio_data_log_t;
typedef struct
{
	unsigned char pa_volume;
	unsigned char monitor_volume;
	unsigned char tcms_flag;///<add by wilson,inorder to tell eamp current tcms flag
	current_time_t tcms_time;
}eamp_heart_beta_t;
typedef struct
{
	unsigned char speed;
	unsigned char door_status;	//0:至少有一个门未关；1：所有门关好
	int whether_start_simulateddu_flag;
}simulate_ddu_t;

typedef struct
{
	unsigned char old_car_line_no;
	unsigned char state;
}cmu_car_type_t;
typedef struct
{
	unsigned char ack_state;
}cmu_ack_state_t;

typedef struct
{
	unsigned char get_or_set;
	unsigned char tcms_current_cmu_state;
}tcms_cmu_state_t;

typedef union
{

	ptt_state_t   comm_ptt_state; ///< pkg_type = 3
	ts_dev_volume_info_t ts_dev_volume_info;///<pkg_type = 4
	monitor_speaker_control_t monitor_speaker_control;///< 5 speaker control
	intercomm_audio_data_log_t intercomm_audio_data_log;///< 8 intercomm auido data recording
	eamp_heart_beta_t eamp_heart_beta;
	simulate_ddu_t simulate_ddu;	//TCMS模拟ddu距离报站
	cmu_car_type_t cmu_car_type;//COMMON_PACKAGE_TYPE_CMU_CAR_TYPE
	cmu_ack_state_t cmu_ack_state;//COMMON_PACKAGE_TYPE_ACK_STATE
	tcms_cmu_state_t tcms_cmu_state;//COMMON_PACKAGE_TYPE_TCMS_CMU_STATE
}common_data_t;

typedef struct
{
	char dst_dev_name[DEVICE_NAME_LENGTH];
		int dst_dev_number;
		char src_dev_name[DEVICE_NAME_LENGTH];
		int src_dev_number;
		int pkg_type; //1表示:心跳包、2表示: 、3表示: 10(TMS_PACKAGE_ID):TMS模拟;5:speaker control
		common_data_t common_data_u;
}common_package_t;

typedef struct
{
	char dst_dev_name[DEVICE_NAME_LENGTH];
	int dst_dev_number;
	char src_dev_name[DEVICE_NAME_LENGTH];
	int src_dev_number;
	int select_car_state[11];
}network_heart_package_t;

typedef struct
{//长度为30 ，与数据库版本号一致
	char get_or_set_version_info;///0:set 1:get
	char software_version[32];
	char software_date[32];
	char db_version_audio[32];
	char db_version_config[32];
}pa_software_version_t;





typedef struct {
    int current_event;
    int audio_play_buffer_id;	
    int send_audio_buffer_id;
    int pending_audio_buffer_id;	
    unsigned char enable_play;
    unsigned char enable_sample;	
    unsigned char play_times;	
} audio_play_control_t;

typedef struct {	
    unsigned char dev_no;
    unsigned char play_waiting;
} eamp_audio_sync_t;
typedef struct {
	char src_device_name[30];
	unsigned int src_device_no;
	unsigned int  vehice_ID[11];
}SelectVehiceID;


typedef enum { BCU,CCU, EAMP, PCU }DevType_t;
typedef enum { ProtocolTestStart,CommQualityTest, SystemReboot, DevFirmwareUpdate,
			ZhwFunc4,ZhwFunc5,ZhwFunc6,ZhwFunc7,ZhwFunc8,ZhwFunc9 } CmdType_t;
typedef enum { NUL,MOUNT,UPDATE,VERIFY } SubCmdType_t;
typedef enum { INVALID,VALID,FAIL,SUCCESS } DevStatus_t;

#pragma pack(4)
typedef struct
{
	int sub_cmd;	
	char dev_name[8];
	int dev_type;	
	int	dev_index;	
	int dev_status;	
	int file_crc;	
	int file_ver;
}dfu_info_t;

typedef struct
{
	int cmd;
	dfu_info_t dfu_info;
}network_shell_package_t;

/** BCUSendInfoInit函数说明
 *  本函数隶属于BCU，主要操作是清空控制信息结构体
 *
 *  @param   void
 *  @return  null
 */
void BCUSendInfoInit(void);

/** BCUSetInformation函数说明
 *  本函数隶属于BCU，主要操作是填充控制信息结构体
 *
 *  @param 事件类型
 *  @param 源设备名称
 *  @param 源设备号
 *  @param 状态请求(0)、结束(1)
 *  @param 线路号
 *  @param 文件序列号
 *  @param 播报次数
 *  @param 是否到站
 *  @param 站代码
 *  @param 对讲状态下的目标设备号
 *  @return null
 *  @note 本函数入口参数过多，填充是需要格外注意。每一次事件请求其中只有部分信息有用。对该此请求无用的数据项可用0或“”代替
 */
void BCUSetInformation(unsigned char,char *, unsigned char, unsigned char,unsigned char,unsigned char,
					  	unsigned int, unsigned char, unsigned char,unsigned char,int);

/** InitThreadCommunicationInfo函数说明
 *  本函数隶属于BCU，主要操作是对控制-网络线程之间的通信结构体进行初始化
 *
 *  @param   void
 *  @return  null
 */
void InitThreadCommunicationInfo(void);		//modify  function name too long

/** SetThreadCommunicationInfo函数说明
 *  本函数隶属于BCU，主要操作是对控制-网络线程之间的通信结构体进行初始化
 *
 *  @param   操作类型
 *  @param   数据类型
 *  @param   目标设备名
 *  @param   目标设备号
 *  @return  null
 */
void SetThreadCommunicationInfo(char *,unsigned char);		//modify


/** GlobalInfoInit函数说明
 *  本函数隶属于BCU，主要操作是对全局设备信息进行初始化
 *
 *  @param   void
 *  @return  null
 */
//extern void InitGlobalInfo(void);

/** GlobalInfoUpdate函数说明
 *  本函数隶属于BCU，主要操作是对全局设备信息进行更新
 *
 *  @param   接收到的控制信息结构体
 *  @return  null
 */
//void UpdateGlobalInfo(send_infomation_t);

//根据设备名称判断设备类型
/** JudgeDeviceType函数说明
 *  本函数主要操作是根据设备名得出设备类型
 *
 *  @param   设备名
 *  @return  null
 */
//int JudgeDeviceType(char *devices_name);
int GetDeviceType(char *devices_name, unsigned char event_type);			//modify



#endif /* DATA_STRUCT_H_ */
