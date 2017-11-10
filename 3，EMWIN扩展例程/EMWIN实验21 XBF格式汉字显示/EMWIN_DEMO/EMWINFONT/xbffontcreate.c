#include "xbffontcreate.h"
#include "ff.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin XBF��ʽ������ʾ   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

//��������
GUI_FONT XBF12_Font;
GUI_FONT XBF16_Font;
GUI_FONT XBF24_Font;
GUI_FONT XBF36_Font;

GUI_XBF_DATA	XBF12_Data;
GUI_XBF_DATA	XBF16_Data;
GUI_XBF_DATA	XBF24_Data;
GUI_XBF_DATA	XBF36_Data;

FIL XBF16FontFile;
FIL XBF12FontFile;
FIL XBF24FontFile;
FIL XBF36FontFile;

//�ص�������������ȡ��������
//������Off:		��XBF��ƫ��(λ��)
//		NumBytes:	Ҫ��ȡ���ֽ���
//		pVoid:	Ҫ��ȡ���ļ�
//		pBuff:	��ȡ�������ݵĻ�����
//����ֵ:0 �ɹ���1 ʧ��
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) 
{
	int result;
	u16 bread; 
	FIL *hFile;

	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	hFile = (FIL*)pVoid; 
	
	//�������ļ��е�ƫ��(λ��)
	result = f_lseek(hFile,Off);
	if(result != FR_OK)	return 1; //���ش���

	//��ȡ��������
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
		
	result = f_read(hFile,pBuffer,NumBytes,(UINT *)&bread); //��ȡ����
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();		//�˳��ٽ���
	#endif
	
	if(result != FR_OK) return 1; //���ش���
	return 0; 
}

//����XBF12���壬��EMWINʹ��
//fxpath:XBF�����ļ�·��
//����ֵ:0���ɹ���1��ʧ��
u8 Create_XBF12(u8 *fxpath) 
{
	int result;
	result = f_open(&XBF12FontFile,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	
	if(result != FR_OK) return 1;
	//����XBF16����
	GUI_XBF_CreateFont(	&XBF12_Font,    //ָ��GUI_FONT�ṹ
						&XBF12_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						&XBF12FontFile);  //������ص�����_cbGetData�Ĳ���
	return 0;
}

//����XBF16���壬��EMWINʹ��
//fxpath:XBF�����ļ�·��
//����ֵ:0���ɹ���1��ʧ��
u8 Create_XBF16(u8 *fxpath) 
{
	int result;
	result = f_open(&XBF16FontFile,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	
	if(result != FR_OK) return 1;
	//����XBF16����
	GUI_XBF_CreateFont(	&XBF16_Font,    //ָ��GUI_FONT�ṹ
						&XBF16_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						&XBF16FontFile);  //������ص�����_cbGetData�Ĳ���
	return 0;
}

//����XBF24���壬��EMWINʹ��
//fxpath:XBF�����ļ�·��
//����ֵ:0���ɹ���1��ʧ��
u8 Create_XBF24(u8 *fxpath) 
{
	int result;
	result = f_open(&XBF24FontFile,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	if(result != FR_OK) return 1;
	//����XBF16����
	GUI_XBF_CreateFont(	&XBF24_Font,    //ָ��GUI_FONT�ṹ
						&XBF24_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						&XBF24FontFile);  //������ص�����_cbGetData�Ĳ���
	return 0;
}

//����XBF36���壬��EMWINʹ��
//fxpath:XBF�����ļ�·��
//����ֵ:0���ɹ���1��ʧ��
u8 Create_XBF36(u8 *fxpath) 
{
	int result;
	result = f_open(&XBF36FontFile,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	if(result != FR_OK) return 1;	
	//����XBF16����
	GUI_XBF_CreateFont(	&XBF36_Font,    //ָ��GUI_FONT�ṹ
						&XBF36_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						&XBF36FontFile);  //������ص�����_cbGetData�Ĳ���
	return 0;
}
