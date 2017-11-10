#include "ttffontcreate.h"
#include "ff.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemwin TTF格式字体显示   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

static GUI_TTF_CS	Cs0,Cs1,Cs2,Cs3;

FIL TTFFontFile;

GUI_FONT TTF12_Font;
GUI_FONT TTF18_Font;
GUI_FONT TTF24_Font;
GUI_FONT TTF36_Font;

GUI_TTF_DATA TTFData;

//创建TTF字体，共EMWIN使用
//fxpath:存放TTF字库的路径
//返回值：0,成功；其他值，失败
int Create_TTFFont(u8 *fxpath) 
{
	int result;
	u16 bread;
	char *TtfBuffer;
	
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	result = f_open(&TTFFontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件
	if(result != FR_OK) return 1;
	
	if(TTFFontFile.fsize>500*1024) return 2; //文件大于500k，跳出!
	else
	{
		TtfBuffer=mymalloc(SRAMEX,TTFFontFile.fsize);//申请内存
		if(TtfBuffer==NULL) return 3;//内存申请失败
	}

	//读取字体数据
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//临界区
	#endif
		
	result = f_read(&TTFFontFile,TtfBuffer,TTFFontFile.fsize,(UINT *)&bread); //读取数据
	if(result != FR_OK) return 4;	//文件打开失败，跳出
	
	f_close(&TTFFontFile);	//关闭TTFFointFile文件
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//退出临界区
	#endif
	
	TTFData.pData=TtfBuffer;	//指向文件地址
	TTFData.NumBytes=TTFFontFile.fsize; //文件大小
	
	Cs0.pTTF		= &TTFData;	
	Cs0.PixelHeight	= 12;
	Cs0.FaceIndex	= 0;
	
	Cs1.pTTF		= &TTFData;
	Cs1.PixelHeight	= 18;
	Cs1.FaceIndex	= 0;
	
	Cs2.pTTF		= &TTFData;
	Cs2.PixelHeight	= 24;
	Cs2.FaceIndex	= 0;
	
	Cs3.pTTF		= &TTFData;
	Cs3.PixelHeight	= 36;
	Cs3.FaceIndex	= 0;
	
	result = GUI_TTF_CreateFont(&TTF12_Font,&Cs0);	//创建字体
	if(result) return 5;	//字体创建失败
	result = GUI_TTF_CreateFont(&TTF18_Font,&Cs1);	//创建字体
	if(result) return 5;	//字体创建失败
	result = GUI_TTF_CreateFont(&TTF24_Font,&Cs2);	//创建字体
	if(result) return 5;	//字体创建失败
	result = GUI_TTF_CreateFont(&TTF36_Font,&Cs3);	//创建字体
	if(result) return 5;	//字体创建失败
	return 0;
}



