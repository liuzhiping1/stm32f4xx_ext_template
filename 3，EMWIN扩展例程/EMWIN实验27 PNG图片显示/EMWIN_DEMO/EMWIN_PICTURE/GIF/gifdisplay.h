#ifndef _JIFDISPLAY_H
#define _JIFDISPLAY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin GIFͼƬ��ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//ʹ��GUI_GIF_Draw()��������BMPͼƬ�Ļ�
//ͼƬ�Ǽ��ص�RAM�еģ���˲��ܴ���GIFMEMORYSIZE
//ע�⣺��ʾGIFͼƬʱ�ڴ�����ʹ�õ�EMWIN���ڴ����뺯�������
//GIFMEMORYSIZE���ܴ������Ǹ�EMWIN������ڴ�ش�С
#define GIFMEMORYSIZE	500*1024	//ͼƬ��С������500kb

//����������ص�RAM�е�GIFͼƬʱ��ͼƬÿ�е��ֽ���
#define GIFPERLINESIZE	2*1024	

int displaygif(char *GIFFileName,u8 mode,u32 x,u32 y,int member,int denom);
int displaygifex(char *GIFFileName,u8 mode,u32 x,u32 y,int member,int denom);
void gifdisplay_demo(void);
#endif
