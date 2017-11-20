/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： change_chinese_line.h
 * 文件标识：
 * 摘    要： 中文换行处理
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef LineExchange_h
#define LineExchange_h

#include "ts_head_file.h"

/** ConvertStringFromFile function description
 *  将src字符串根据给定的宽度添加换行符号，在FLTK中，中文换行为空格
 *
 *  @param   目标地址
 *  @param   源地址
 *  @param   宽度(一个中文三个字节，宽度为英文字符个数)
 *  @return  if create successfully return 0
 */
int ConvertStringFromFile(char dest[], const char src[], int Width);

/** GetFileLength function description
 *  获取文件长度
 *
 *  @param   文件名
 *  @return  文件长度
 */
int GetFileLength(const char param_file_name[]);

/** ReadFileBrowser_wilson function description
 *  浏览文件内容，目前已删除，不再使用
 *
 *  @param   目标地址
 *  @param   文件路径
 *  @param   总数
 *  @param  偏移量
 *  @return  文件长度
 */
int ReadFileBrowser_wilson(char dest[],const char * file_name,int param_total_number,long param_offset);

/** GetSomeLinesFromString function description
 *  将字符串以行的形式送入到目标地址中
 *
 *  @param   目标地址
 *  @param   源地址
 *  @param   起始点
 *  @param   终止点
 *  @return  文件长度
 */
void GetSomeLinesFromString(char dest[], char src[], int start, int end);

#endif  
