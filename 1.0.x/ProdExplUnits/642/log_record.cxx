/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： log_record.c
 * 文件标识：
 * 摘    要：The source file of log record
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "log_record.h"
#include "bcu_function.h"

struct timeval tp_log;

/*show the detail of cmd information*/
int DisplayNetworkCmd(network_send_package_t  param_cmd_info){

		if(param_cmd_info.send_information.event_type_intercom==D2P_INTERCOMM_EVENT){
				diag_printf("------------------D2PCmd-DisPlay------------------------\n");
				diag_printf("\tsrc\tdev_no\tdst\tdev_no\tenvent\tconnecting\n");
				diag_printf("\t%s\t%d\t%s\t%d\t%d\t%d\t\n",
						param_cmd_info.send_information.src_devices_name,
						param_cmd_info.send_information.src_devices_no,
						param_cmd_info.dst_devices_name,
						param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no,
						param_cmd_info.send_information.event_type_intercom,
						param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting);
				diag_printf("--------------------------------------------------------\n");
				diag_printf("\tppt\tpending\treq_or_over\tresponse\n");
				diag_printf("\t%d\t%d\t%d\t\t%d\n",
						param_cmd_info.send_information.event_ptt_state,
						param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pending,
						param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over,
						param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response);
				diag_printf("-----------------------end------------------------------\n\n");
		}

	return 0;
}




void WriteNetworkErrorToLog(char *info)
{
      return ;
}

void SetSoftwareVersion(char *Version,char *Date)

{

		strncpy(bcu_state.pa_software_version.software_version,Version,30);
		strncpy(bcu_state.pa_software_version.software_date,Date,30);
		strncpy(bcu_state.pa_software_version.db_version_audio,"NULL",30);
		strncpy(bcu_state.pa_software_version.db_version_config,"NULL",30);
		diag_printf("Compile date:%s\n",__DATE__);
		diag_printf("Version:%s\n",bcu_state.pa_software_version.software_version);
		diag_printf("Debug date:%s\n",bcu_state.pa_software_version.software_date);
		diag_printf("Ann:%s\n",bcu_state.pa_software_version.db_version_audio);
		diag_printf("Config:%s\n",bcu_state.pa_software_version.db_version_config);

}

int DisplaySendCmd(send_infomation_t  param_cmd_info)
{

	if(param_cmd_info.event_type_ann==LIVE_ANN_EVENT)
	{
		diag_printf("--------------LIVE----------------------\n");
		diag_printf("active:%d;be_over:%d;requ:%d;\n",
				param_cmd_info.event_info_ann.live_announce.live_active,
				param_cmd_info.event_info_ann.live_announce.live_begin_or_over,
				param_cmd_info.event_info_ann.live_announce.live_response);
		diag_printf("--------------END----------------------\n");

	}
	if(param_cmd_info.event_type_intercom==D2P_INTERCOMM_EVENT){
					diag_printf("---D2P_INTERCOMM_EVENT-DisPlay----\n");
					diag_printf("SrcName = %s\nSrcId = %d\nDtsId = %d\nEvent = %d\nActive = %d\n",
							param_cmd_info.src_devices_name,
							param_cmd_info.src_devices_no,
							param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no,
							param_cmd_info.event_type_intercom,
							param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_active);
					diag_printf("Ppt = %d\nPending = %d\nReqOver = %d\ntResponse = %d\n",
							param_cmd_info.event_ptt_state,
							param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_pending,
							param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over,
							param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_response);
					diag_printf("-------------------\n\n");
	}

return 0;
}


