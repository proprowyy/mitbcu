#ifndef TSHeadFile_h
#define TSHeadFile_h
/* wilson 2013-06-20
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "station_info.h"
#include "error_handle.h"
#include "global_param.h"
#include "intercomm_info.h"
#include "change_chinese_line.h"
#include "touch_screen_function.h"
#include "touch_screen_ann.h"
#include "touch_screen_intercomm.h"
#include "TouchScreen_horizontal.h"

#include "set_cmd_package.h"

#include "../bcu_function.h"
#include "../bcu_state_machine.h"
#include "../buffer_read_write_fun.h"

#include "ts_type.h"
#include "ts_simulate.h"
#include "bcu_get_data_from_ccu_type.h"
typedef struct{

	unsigned char t1;
	unsigned char t2;
	unsigned char t3;
	unsigned char t4;
	unsigned char t5;
	unsigned char t6;
	unsigned char t7;
	unsigned char t8;
	unsigned char t9;
	unsigned char t10;
	unsigned char t11;
}RecordCarCol;

extern unsigned char CarNumberSelectAndRecordBtn[11];
extern unsigned int gw_RecordPcuMonBtnPass;
extern unsigned int ReferCarNo(unsigned char t_CarNumberSelectAndRecordBtn[],RecordCarCol *a);
#endif
