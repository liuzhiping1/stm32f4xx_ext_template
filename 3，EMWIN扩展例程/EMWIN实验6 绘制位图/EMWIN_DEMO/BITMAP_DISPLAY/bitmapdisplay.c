#include "bitmapdisplay.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "arm_math.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemWin ����λͼ
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern GUI_BITMAP bmALIENTEK;  //ALIENTEKͼ��

//��ʾC�ļ���ʽ��λͼ
void draw_bitmap(void)
{
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_YELLOW);
	GUI_Clear();
	
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_SetTextMode(GUI_TM_TRANS);	//͸����ʾ
	GUI_DispStringHCenterAt("ALIENTEK BITMAP DISPLAY",400,0 );
	
	GUI_DrawBitmap(&bmALIENTEK,295,194);  //����ALIENTEKͼ��	
}

//����ʾ��������λͼ
//Xmag:X����ı������ӣ���λΪǧ��֮
//Ymag:Y����ı������ӣ���λΪǧ��֮
void zoom_bitmap(int Xmag,int Ymag)
{
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_DrawBitmapEx(&bmALIENTEK,400,240,105,46,Xmag,Ymag); //���ձ�������λͼ
}
