#include "jpegdisplay.h"
#include "EmWinHZFont.h"
#include "GUI.h"
#include "malloc.h"
#include "ff.h"
#include "ili93xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin JPEGͼƬ��ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

static FIL JPEGFile;
static char jpegBuffer[JPEGPERLINESIZE];
/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       _GetData
*
* Function description
*   This routine is called by GUI_JPEG_DrawEx(). The routine is responsible
*   for setting the data pointer to a valid data location with at least
*   one valid byte.
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*   StartOfFile - If this flag is 1, the data pointer should be set to the
*                 beginning of the data stream.
*
* Return value:
*   Number of data bytes available.
*/
static int JpegGetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{
	static int readaddress=0;
	FIL * phFile;
	UINT NumBytesRead;
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	phFile = (FIL *)p;
	
	if (NumBytesReq > sizeof(jpegBuffer)) 
	{
		NumBytesReq = sizeof(jpegBuffer);
	}

	//�ƶ�ָ�뵽Ӧ�ö�ȡ��λ��
	if(Off == 1) readaddress = 0;
	else readaddress=Off;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�����ٽ���
	#endif
		
	f_lseek(phFile,readaddress); 
	
	//��ȡ���ݵ���������
	f_read(phFile,jpegBuffer,NumBytesReq,&NumBytesRead);
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();//�˳��ٽ���
	#endif
	
	*ppData = (U8 *)jpegBuffer;
	return NumBytesRead;//���ض�ȡ�����ֽ���
}

//��ָ��λ����ʾ���ص�RAM�е�JPEGͼƬ
//JPEGFileName:ͼƬ��SD�����������洢�豸�е�·��(���ļ�ϵͳ֧�֣�)
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
int displyjpeg(u8 *JPEGFileName,u8 mode,u32 x,u32 y,int member,int denom)
{
	u16 bread;
	char *jpegbuffer;
	char result;
	int XSize,YSize;
	GUI_JPEG_INFO JpegInfo;
	float Xflag,Yflag;
	
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	result = f_open(&JPEGFile,(const TCHAR*)JPEGFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����JPEGMEMORYSIZE
	if((result != FR_OK) || (JPEGFile.fsize>JPEGMEMORYSIZE)) 	return 1;
	
	jpegbuffer=mymalloc(SRAMEX,JPEGFile.fsize);	//�����ڴ�
	if(jpegbuffer == NULL) return 2;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();		//�ٽ���
	#endif
		
	result = f_read(&JPEGFile,jpegbuffer,JPEGFile.fsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//�˳��ٽ���
	#endif
	
	GUI_JPEG_GetInfo(jpegbuffer,JPEGFile.fsize,&JpegInfo); //��ȡJEGPͼƬ��Ϣ
	XSize = JpegInfo.XSize;	//��ȡJPEGͼƬ��X���С
	YSize = JpegInfo.YSize;	//��ȡJPEGͼƬ��Y���С
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_JPEG_Draw(jpegbuffer,JPEGFile.fsize,x,y);	//��ָ��λ����ʾJPEGͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_JPEG_DrawScaled(jpegbuffer,JPEGFile.fsize,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				GUI_JPEG_Draw(jpegbuffer,JPEGFile.fsize,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				Xflag = (float)XSize*((float)member/(float)denom);
				Yflag = (float)YSize*((float)member/(float)denom);
				XSize = (lcddev.width-(int)Xflag)/2-1;
				YSize = (lcddev.height-(int)Yflag)/2-1;
				GUI_JPEG_DrawScaled(jpegbuffer,JPEGFile.fsize,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&JPEGFile);			//�ر�JPEGFile�ļ�
	myfree(SRAMEX,jpegbuffer);	//�ͷ��ڴ�
	return 0;
}

//��ָ��λ����ʾ������ص�RAM�е�BMPͼƬ(���ļ�ϵͳ֧�֣�����СRAM���Ƽ�ʹ�ô˷�����)
//JPEGFileName:ͼƬ��SD�����������洢�豸�е�·��
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
int displayjpegex(u8 *JPEGFileName,u8 mode,u32 x,u32 y,int member,int denom)
{
	char result;
	int XSize,YSize;
	float Xflag,Yflag;
	GUI_JPEG_INFO JpegInfo;
	
	result = f_open(&JPEGFile,(const TCHAR*)JPEGFileName,FA_READ);	//���ļ�
	//�ļ��򿪴���
	if(result != FR_OK) 	return 1;
	
	GUI_JPEG_GetInfoEx(JpegGetData,&JPEGFile,&JpegInfo);
	XSize = JpegInfo.XSize;	//JPEGͼƬX��С
	YSize = JpegInfo.YSize;	//JPEGͼƬY��С
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_JPEG_DrawEx(JpegGetData,&JPEGFile,x,y);//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_JPEG_DrawScaledEx(JpegGetData,&JPEGFile,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				GUI_JPEG_DrawEx(JpegGetData,&JPEGFile,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				Xflag = (float)XSize*((float)member/(float)denom);
				Yflag = (float)YSize*((float)member/(float)denom);
				XSize = (lcddev.width-(int)Xflag)/2-1;
				YSize = (lcddev.height-(int)Yflag)/2-1;
				GUI_JPEG_DrawScaledEx(JpegGetData,&JPEGFile,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&JPEGFile);		//�ر�BMPFile�ļ�
	return 0;
}	

void jpegdisplay_demo(void)
{
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_Clear();
	
	while(1)
	{
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ�ż��ص�RAM�е�JPEGͼƬ",240,0);
		displyjpeg("0:/PICTURE/JPEG/ʵ��ͼƬ.jpg",0,0,0,	1,1);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ������2���ļ��ص�RAM�е�JPEGͼƬ",240,0);
		displyjpeg("0:/PICTURE/JPEG/jpeg0.jpg",1,0,	0,2,1);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ����С1/2�ļ��ص�RAM�е�JPEGͼƬ",240,0);
		displyjpeg("0:/PICTURE/JPEG/SIM900A.jpg",1,0,0,1,2);
		GUI_Delay(1000);
		GUI_Clear();
		
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ��������ص�JPEGͼƬ",240,0);
		displayjpegex("0:/PICTURE/JPEG/ʵ��ͼƬ.jpg",0,0,0,1,1);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ������2����������ص�JPEGͼƬ",240,0);
		displayjpegex("0:/PICTURE/JPEG/jpeg0.jpg",1,0,0,2,1);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ����С1/2��������ص�JPEGͼƬ",240,0);
		displayjpegex("0:/PICTURE/JPEG/SIM900A.jpg",1,0,0,1,2);
		GUI_Delay(1000);
		GUI_Clear();
	}
}
