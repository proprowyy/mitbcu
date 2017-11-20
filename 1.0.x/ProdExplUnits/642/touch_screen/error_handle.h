
/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： error_handle.h
 * 文件标识：
 * 摘    要： 触摸屏错误处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef ErrorHandle_h
#define ErrorHandle_h

#include "ts_head_file.h"
#include "ts_simulate.h"
#ifdef TS_ERROR_CHECK
typedef struct TNode{
	int position_x;//界面位置-X
	int position_y;//界面位置-Y
	int size_w;//界面宽度
	int size_h;//界面高度
	char component_name[30];//组件名称
	struct TNode *firstChild;//第一个子节点
	struct TNode *parent;//父节点
	struct TNode *nextSibling;//兄弟节点
}TNode,*Tree;//触摸屏界面信息，这里采用树的形式来表现的
extern Tree error_handle_tree;
extern  Tree err_touch_screen;
extern  Tree err_window_view;
extern  Tree err_select_window;

extern  FILE *fp_operation_log;
extern  FILE *fp_debug_log;
extern  FILE *fp_system_error;

/** GetTime function description
 *  获取系统时间，并且以一定格式输出
 *
 *  @param   目标地址
 *  @param   格式参数
 *  @return  if successf return 0
 */
int GetTime(char *out, int fmt);

/** openFile function description
 *  打开触摸屏错误文件log日志
 *
 *  @param   文件路径
 *  @param   打开模式
 *  @return  文件描述符
 */
FILE * openFile(const char *fileName, const char *mode);

/** closeFile function description
 *  关闭触摸屏错误文件log日志
 *
 *  @param   文件路径
 *  @return  if success return 0
 */
int closeFile(FILE *fp);

/** CreateErrorHandleTree function description
 *  创建触摸屏界面错误记录树
 *
 *  @param   null
 *  @return  null
 */
void CreateErrorHandleTree();

/** CreateErrorTouchScreenNode function description
 *  创建触摸屏界面错误记录节点
 *
 *  @param   null
 *  @return  null
 */
void CreateErrorTouchScreenNode();

/** CreateErrorHadleWindowView function description
 *  创建触摸屏界面-功能操作界面错误记录根节点--触摸屏上面部分
 *
 *  @param   null
 *  @return  null
 */
void CreateErrorHadleWindowView();

/** CreateErrorHadleWindowView function description
 *  创建触摸屏界面-选择操作界面错误记录根节点--触摸屏下面部分
 *
 *  @param   null
 *  @return  null
 */
void CreateErrorHadleSelectWindow();

/** CodeErrorCheck function description
 *  触摸屏界面错误检查
 *
 *  @param   null
 *  @return  null
 */
void CodeErrorCheck();

/** AddErrorHandleGroupCheck function description
 *  添加叶子节点到树中-添加group信息到错误检测树中
 *
 *  @param   父节点
 *  @param   group界面信息
 *  @param   group名称
 *  @return  null
 */
void AddErrorHandleGroupCheck(Tree p_parent_tree,Fl_Group *p_gp,const char p_gp_name[]);

/** GroupErrorCheck function description
 *  检查group错误信息
 *
 *  @param   父节点
 *  @param   group界面信息
 *  @param   group名称
 *  @return  null
 */
void GroupErrorCheck(Tree p_parent_tree,Fl_Group *p_gp,const char p_gp_name[]);
#endif
/** GroupErrorCheck function description
 *  将相关信息写入到指定文件中去
 *
 *  @param   错误log日志文件描述符
 *  @param   错误信息
 *  @param   错误记录格式
 *  @return  if success return 0
 */
int WriteLog(FILE *fp, const char *str, int bLog);

/** WriteOperationLog function description
 *  将触摸屏操作日志写入到文件中
 *
 *  @param   触摸屏操作log日志
 *  @return  null
 */
void WriteOperationLog(const char p_str[]);

/** WriteSystemErrorLog function description
 *  将触摸屏系统日志写入到文件中
 *
 *  @param   触摸屏系统log日志
 *  @return  null
 */
void WriteSystemErrorLog(const char p_str[]);

#endif
