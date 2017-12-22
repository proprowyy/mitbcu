/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： change_chinese_line.cxx
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
#include "change_chinese_line.h"
int ConvertStringFromFile(char dest[], const char src[], int Width)
{//自动换行处理函数
	int charaStop = 0;
	int newLine = 0;
	int stopLength = 0;
	int currentLength = 0;
	int chineseCharaNum = 0; //UTF-8 , 1 chinese character = 3 bits
	int english12 = 0;
	int chinese24 = 0;
	int allLineNum = 0;
	charaStop = 0;
	newLine = 0;
	stopLength = 0;
	currentLength = 0;
	chineseCharaNum = 0;
	int c, i;

	for (i = 0; i < strlen(dest); i++) {
		dest[i] = 32;
	}

	for (i = 0; i < strlen(src); i++)
	{
		if ((src[i] & 0x80)) // chinese character
		{
			chineseCharaNum++;
			charaStop = 0;
			if (chineseCharaNum == 3) {
				chineseCharaNum = 0;
				charaStop = 1;
				chinese24++;
			}

		}
		else
		{
			charaStop = 1;
			english12++;
		}

		stopLength++;

		if (src[i] == '\n')
		{
			allLineNum++;
			english12 = 0;
			chinese24 = 0;
			continue;
		}

		if (((english12 * ENGLISHLENGTH + chinese24 * CHINESELENGTH
				+ CHINESELENGTH) > Width) && charaStop == 1) {
			english12 = 0;
			chinese24 = 0;

			for (c = currentLength; c < currentLength + stopLength; c++) {

				dest[c + newLine] = src[c];

			}

			newLine = newLine + 1;
			currentLength = stopLength + currentLength;
			if (currentLength + newLine > 0) {
				dest[currentLength + newLine - 1] = '\n'; // insert \n to string
				allLineNum++;
			}
			stopLength = 0;

		}
	}

	for (c = currentLength; c < currentLength + stopLength; c++)
	{
		dest[c + newLine] = src[c];
	}

	dest[c + newLine] = '\0';//wilson 2013-06-18

	if (stopLength == 0)
		return allLineNum;
	else
		return allLineNum + 1;
}


int GetFileLength(const char param_file_name[])
{
	FILE *fp;
	int nLen = 0;
	char t_error_tip_buffer[100] = "";
	if((fp = fopen(param_file_name,"r")) == NULL)
	{

		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	nLen = ftell(fp);
	rewind(fp);
	fclose(fp);
	return nLen;
}

void ReadFileBrowser(char dest[],const char * file_name)
{
	char ch;
	FILE *fp;
	fp = fopen(file_name, "r"); // read mode
	char t_error_tip_buffer[100] = "";
	if (fp == NULL)
	{

		exit(1);
	}


	int i = 0;
	ch = fgetc(fp);
	ch = fgetc(fp);
	ch = fgetc(fp);
	while ((ch = fgetc(fp)) != EOF )
	{
		if(ch != 13)
		{
			dest[i] = ch;
			i++;
		}
	}

	fclose(fp);

//	int j = 0;
//	for(j = 0 ;j < i ;j ++)
//	{
//		printf("%d\n",dest[j]);
//	}

}
int ReadFileBrowser_wilson(char dest[],const char * file_name,int param_total_number,long param_offset)
{
  return 0;
	char ch;
	int ret = 0;
	int file_length = 0;
	FILE *fp;
	fp = fopen(file_name, "r"); // read mode
	char t_error_tip_buffer[100] = "";
	if (fp == NULL)
	{

		exit(1);
	}
	file_length = GetFileLength(file_name);

	if(param_offset >= file_length)
	{
		param_offset = 0;
	}
	else
	{
		fseek(fp,param_offset,SEEK_SET);
	}


	int i = 0;
	ch = fgetc(fp);
	ch = fgetc(fp);
	ch = fgetc(fp);
	while ((ch = fgetc(fp)) != EOF && i < param_total_number -3)
	{
		if(ch != 13)
		{
			dest[i] = ch;
			i++;
		}
	}
	ret = i;

	if(dest[i - 1] < 0 && dest[i - 2] > 0 )
	{
		dest[i - 1] = '\0';
		ret = i -1;
	}
	if(dest[i - 3] > 0 && dest[i - 2] < 0 && dest[i - 1] < 0)
	{
		dest[i - 2] = '\0';
		dest[i - 1] = '\0';
		ret = i - 2;
	}

	fclose(fp);
	return ret;
}
void GetSomeLinesFromString(char dest[], char src[], int start, int end)
{
	int count =0,k = 0,i;

	for ( i = 0; i < (int)strlen(src); i++)
	{
		if (src[i] == '\n')
		{
			count++;
			//src[i] = ' ';
		}
		if(count>=start-1)
		{
			dest[k]=src[i];
			k++;
		}
		dest[k] = '\0';

		if(count==end)
			break;
	}
}
