#include "pngdisplay.h"
#include "EmWinHZFont.h"
#include "GUI.h"
#include "malloc.h"
#include "ff.h"
#include "ili93xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin PNGͼƬ��ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////////	
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

static FIL PNGFile;
//static char pngBuffer[PNGPERLINESIZE];
/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       PngGetData
*
* Function description
*   This routine is called by GUI_PNG_DrawEx(). The routine is responsible
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
static int PngGetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{
	static int readaddress=0;
	FIL * phFile;
	U8 *pData;
	UINT NumBytesRead;
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif
	
	pData = (U8*)*ppData;
	phFile = (FIL *)p;
	
	//�ƶ�ָ�뵽Ӧ�ö�ȡ��λ��
	if(Off == 1) readaddress = 0;
	else readaddress=Off;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
	f_lseek(phFile,readaddress); 
	
	//��ȡ���ݵ���������
	f_read(phFile,pData,NumBytesReq,&NumBytesRead);
	
		
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();//�˳��ٽ���
	#endif
	return NumBytesRead;//���ض�ȡ�����ֽ���
}

//��ָ��λ����ʾ���ص�RAM�е�PNGͼƬ(PNGͼƬ�������ţ�)
//PNGFileName:ͼƬ��SD�����������洢�豸�е�·��(���ļ�ϵͳ֧�֣�)
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//����ֵ:0 ��ʾ����,���� ʧ��
int displaypng(char *PNGFileName,u8 mode,u32 x,u32 y)
{
	u16 bread;
	char *pngbuffer;
	char result;
	int XSize,YSize;
	
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	result = f_open(&PNGFile,(const TCHAR*)PNGFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����JPEGMEMORYSIZE
	if((result != FR_OK) || (PNGFile.fsize>PNGMEMORYSIZE)) 	return 1;

	pngbuffer=mymalloc(SRAMEX,PNGFile.fsize);
	if(pngbuffer == NULL) return 2;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
		
	result = f_read(&PNGFile,pngbuffer,PNGFile.fsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//�˳��ٽ���
	#endif
	
	XSize = GUI_PNG_GetXSize(pngbuffer,PNGFile.fsize);	//��ȡPNGͼƬ��X���С
	YSize = GUI_PNG_GetYSize(pngbuffer,PNGFile.fsize);	//��ȡPNGͼƬ��Y���С
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			GUI_PNG_Draw(pngbuffer,PNGFile.fsize,x,y);//��ʾPNGͼƬ
			break;
		case 1:	//��LCD�м���ʾͼƬ
			GUI_PNG_Draw(pngbuffer,PNGFile.fsize,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);	
			break;
	}
	f_close(&PNGFile);		//�ر�PNGFile�ļ�
	myfree(SRAMEX,pngbuffer);
	return 0;
}

//��ָ��λ����ʾ������ص�RAM�е�PNGͼƬ(���ļ�ϵͳ֧�֣�����СRAM���Ƽ�ʹ�ô˷�����PNGͼƬ�������ţ�)
//PNGFileName:ͼƬ��SD�����������洢�豸�е�·��
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//����ֵ:0 ��ʾ����,���� ʧ��
int displaypngex(char *PNGFileName,u8 mode,u32 x,u32 y)
{
	char result;
	int XSize,YSize;

	result = f_open(&PNGFile,(const TCHAR*)PNGFileName,FA_READ);	//���ļ�
	//�ļ��򿪴���
	if(result != FR_OK)	return 1;
	
	XSize = GUI_PNG_GetXSizeEx(PngGetData,&PNGFile);//PNGͼƬX��С
	YSize = GUI_PNG_GetYSizeEx(PngGetData,&PNGFile);//PNGͼƬY��С
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			GUI_PNG_DrawEx(PngGetData,&PNGFile,x,y);
			break;
		case 1:	//��LCD�м���ʾͼƬ
			GUI_PNG_DrawEx(PngGetData,&PNGFile,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			break;
	}
	f_close(&PNGFile);	//�ر�PNGFile�ļ�
	return 0;
}

//PNGͼƬ��ʾ����
void pngdisplay_demo(void)
{
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_SetColor(GUI_RED);
	GUI_Clear();
	while(1)
	{
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ�ż��ص�RAM�е�PNGͼƬ",400,0);
		displaypng("0:/PICTURE/PNG/�ʹ�.png",0,200,100);
		GUI_Delay(1000);
		GUI_Clear();

		displaypng("0:/PICTURE/PNG/CS.png",1,0,0);
		GUI_Delay(1000);
		GUI_Clear();	
		
		displaypng("0:/PICTURE/PNG/�㽶.png",1,0,0);
		GUI_Delay(1000);
		GUI_Clear();
		
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ��������ص�PNGͼƬ",400,0);
		displaypngex("0:/PICTURE/PNG/¿.png",0,200,100);
		GUI_Delay(1000);
		GUI_Clear();
		
		displaypngex("0:/PICTURE/PNG/�ļ���.png",1,0,0);
		GUI_Delay(1000);
		GUI_Clear();
		
		displaypngex("0:/PICTURE/PNG/����.png",1,0,0);
		GUI_Delay(1000);
		GUI_Clear();
	}
}
