#include "ttffontcreate.h"
#include "ff.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin TTF��ʽ������ʾ   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

static GUI_TTF_CS	Cs0,Cs1,Cs2,Cs3;

FIL TTFFontFile;

GUI_FONT TTF12_Font;
GUI_FONT TTF18_Font;
GUI_FONT TTF24_Font;
GUI_FONT TTF36_Font;

GUI_TTF_DATA TTFData;

//����TTF���壬��EMWINʹ��
//fxpath:���TTF�ֿ��·��
//����ֵ��0,�ɹ�������ֵ��ʧ��
int Create_TTFFont(u8 *fxpath) 
{
	int result;
	u16 bread;
	char *TtfBuffer;
	
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	result = f_open(&TTFFontFile,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	if(result != FR_OK) return 1;
	
	if(TTFFontFile.fsize>500*1024) return 2; //�ļ�����500k������!
	else
	{
		TtfBuffer=mymalloc(SRAMEX,TTFFontFile.fsize);//�����ڴ�
		if(TtfBuffer==NULL) return 3;//�ڴ�����ʧ��
	}

	//��ȡ��������
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
		
	result = f_read(&TTFFontFile,TtfBuffer,TTFFontFile.fsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 4;	//�ļ���ʧ�ܣ�����
	
	f_close(&TTFFontFile);	//�ر�TTFFointFile�ļ�
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//�˳��ٽ���
	#endif
	
	TTFData.pData=TtfBuffer;	//ָ���ļ���ַ
	TTFData.NumBytes=TTFFontFile.fsize; //�ļ���С
	
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
	
	result = GUI_TTF_CreateFont(&TTF12_Font,&Cs0);	//��������
	if(result) return 5;	//���崴��ʧ��
	result = GUI_TTF_CreateFont(&TTF18_Font,&Cs1);	//��������
	if(result) return 5;	//���崴��ʧ��
	result = GUI_TTF_CreateFont(&TTF24_Font,&Cs2);	//��������
	if(result) return 5;	//���崴��ʧ��
	result = GUI_TTF_CreateFont(&TTF36_Font,&Cs3);	//��������
	if(result) return 5;	//���崴��ʧ��
	return 0;
}



