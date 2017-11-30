/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： state_machine.h
 * 文件标识：
 * 摘    要： 状态机头文件
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2012-10-16
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_


#include "data_struct.h"
#include "ts_type.h"
#include "bcu_get_data_from_ccu_type.h"
#include "touch_screen/intercomm_info.h"
#include "touch_screen/station_info.h"


/*the number of rules*/
#define RECORD_OF_RULES 50

/*identify the owner of mic*/
#define ANNOUNCE 0
#define INTERCOM 1
#define NONE 2
#define OCC_OR_3D5 3



/** 本结构体功能：状态机处理函数结构体
 *	本结构体主要是标识状态机的处理函数
 */
typedef struct
{
	void (*enter)(send_infomation_t *);
	void (*exit)();
	void (*process)(send_infomation_t *);
	int state_id;
}state_process_t;

/** 本结构体功能：状态机规则记录
 *	本结构体主要是标识状态机的状态转换规则
 */
typedef struct
{
	state_process_t *psrc_state;
	state_process_t *dest_state;
	int trigger_event;
	int is_empty;
}state_machine_rule_t;

/** 本结构体功能：状态机规则表
 */
typedef struct
{
	state_machine_rule_t xrule;
	unsigned char rule_enable;
}state_rule_table_t;


typedef struct
{
	state_process_t *idle;
	state_process_t *auto_announcement;
	state_process_t *manual_announcement;
	state_process_t *emergency_announcement;
	state_process_t *live_announcement;
	state_process_t *occ;
}state_process_ann_t;

typedef struct
{
	state_process_t *idle;
	state_process_t *d2d_intercom;
	state_process_t *d2p_intercom;
	state_process_t *d2d_hang_up_d2p;
	state_process_t *d2p_moniort_state;
}state_process_intercom_t;

typedef struct
{
	unsigned char occ_prio;
	unsigned char outer_3d5_prio;
	unsigned char live_prio;
	unsigned char emerg_prio;
	unsigned char manual_prio;
}bcu_event_priority_t;

typedef struct
{
	void (*RuleInit)();
	void (*AddRule)(state_machine_rule_t);
	void (*RemoveRule)(state_machine_rule_t);
	void (*DisableRule)(state_machine_rule_t);
	void (*EnableRule)(state_machine_rule_t);

	state_process_ann_t state_process_ann;
	state_process_intercom_t state_process_intercom;
	state_rule_table_t bcu_state_rule_table[RECORD_OF_RULES];
	state_process_t *bcu_active_ann_state;
	state_process_t *bcu_active_intercom_state;

	int mic_owner;
	//缓冲标志
	int cmd_send_buffer_id;
	int cmd_recv_buffer_id;

	int audio_data_send_buffer_id;
	int audio_data_recv_buffer_id;

	int comm_cmd_send_buffer_id;
	int comm_cmd_recv_buffer_id;

	int shell_recv_buffer_id;
	int shell_send_buffer_id;

	int comm_server_send_big_buffer_id;
	int comm_server_recv_big_buffer_id;

	int all_dev_state_buffer_id;
	int local_audio_data_recv_buffer_id;
	int software_version_buffer_id;
	int recv_cmd_from_touch;
	int pending_buffer_id;
	int local_audio_data_buffer_id;
	int alarm_audio_data_buffer_id;
	int ppt_state_recv_buffer_id;
	int dev_vol_info_buffer_id;

	//×××××××××××××××××××××××××//
	//button state
		unsigned char live_button_state;
		unsigned char d2d_button_state;
		unsigned char d2p_button_state;
	//×××××××××××××××××××××××××//

	device_bcu_state_t  bcu_info;
	unsigned char dev_type;

	int other_bcu_ptt_state;
	int this_bcu_ptt_state;
	int opposite_bcu_no;

	/*add wilson,which is used on touch screen*/
	device_volume_t device_volume;
	event_priority_t event_priority;
	pcu_request_info_t pcu_request_info;
	global_region_info_t bcu_region_info[2];

	/*---------end-----------------*/
	unsigned char car_occupy_state[11];
    unsigned char car_select_flag[11];
    unsigned char car_select_finish;


    unsigned char car_select_count_for_monitor;
    unsigned char iph_monitor_cur_page;
    int bcu_request_number;
    unsigned char this_bcu_request;

	int recv_cmd_sequence;//包的序列号
	unsigned char system_language;
	unsigned char this_bcu_is_active;

	unsigned char this_bcu_is_d2p_intercomming;
	unsigned char other_bcu_intercomm_state;
	int udp_common_recv_dev_colueme;
	pa_software_version_t pa_software_version;
	unsigned char ts_current_state;
	unsigned char this_bcu_intercomm_state;
}bcu_state_t;

#endif /* STATE_MACHINE_H_ */

