#include "textdisplay.h"
#include "GUI.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemWin �ı���ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//WEWIN�ı���ʾ����
void emwin_texttest(void)
{
	int i;
	char acText[]	= "This example demostrates text wrapping";
	GUI_RECT Rect	={200,240,259,299};	//���������ʾ����
	GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE,
						  GUI_WRAPMODE_CHAR,
						  GUI_WRAPMODE_WORD};
				  
	GUI_SetBkColor(GUI_BLUE);		//���ñ�����ɫ
	GUI_Clear();					//����
	GUI_SetFont(&GUI_Font24_ASCII); //��������
	GUI_SetColor(GUI_YELLOW);       //����ǰ��ɫ(���ı������ߵ���ɫ)
	GUI_DispString("HELLO WORD!");
	
	GUI_SetFont(&GUI_Font8x16);		//��������
	GUI_SetPenSize(10);				//���ñʴ�С
	GUI_SetColor(GUI_RED);			//��ɫ����
	GUI_DrawLine(300,50,500,130);	//����
	GUI_DrawLine(300,130,500,50);	//����
	GUI_SetBkColor(GUI_BLACK);		//���ú�ɫ����
	GUI_SetColor(GUI_WHITE);		//����������ɫΪ��ɫ
	GUI_SetTextMode(GUI_TM_NORMAL);	//����ģʽ
	GUI_DispStringHCenterAt("GUI_TM_NORMAL",400,50);
	GUI_SetTextMode(GUI_TM_REV);	//��ת�ı�
	GUI_DispStringHCenterAt("GUI_TM_REV"   ,400,66);
	GUI_SetTextMode(GUI_TM_TRANS);	//͸���ı�
	GUI_DispStringHCenterAt("GUI_TM_TRANS" ,400,82);
	GUI_SetTextMode(GUI_TM_XOR);   	//����ı�
	GUI_DispStringHCenterAt("GUI_TM_XOR"   ,400,98);
	GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_REV);//͸����ת�ı�
	GUI_DispStringHCenterAt("GUI_EM_TRANS|GUI_TM_REV",400,114);
	
	GUI_SetTextMode(GUI_TM_TRANS);  //͸���ı�
	for(i=0;i<3;i++)
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringInRectWrap(acText,&Rect,GUI_TA_LEFT,aWm[i]);//�ڵ�ǰ����ָ���ľ�����������ʾ�ַ���(�����Զ�����)
		Rect.x0 += 70;
		Rect.x1 += 70;
	}	
}



