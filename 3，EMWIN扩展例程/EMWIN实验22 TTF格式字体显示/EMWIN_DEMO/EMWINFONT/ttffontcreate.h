#ifndef _TTFFONTCREATE_H
#define _TTFFONTCREATE_H
#include "sys.h"
#include "GUI.h"
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

extern GUI_FONT TTF12_Font;
extern GUI_FONT TTF18_Font;
extern GUI_FONT TTF24_Font;
extern GUI_FONT TTF36_Font;

int Create_TTFFont(u8 *fxpath);
#endif
