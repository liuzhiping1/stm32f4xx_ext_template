#ifndef _JPEGDISPLAY_H
#define _JPEGDISPLAY_H
#include "sys.h"
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


//ʹ��GUI_JPEG_Draw()��������BMPͼƬ�Ļ�
//ͼƬ�Ǽ��ص�RAM�еģ���˲��ܴ���JEGPMEMORYSIZE
//ע�⣺��ʾBMPͼƬʱ�ڴ�����ʹ�õ�EMWIN���ڴ����뺯�������
//JPEGMEMORYSIZE���ܴ������Ǹ�EMWIN������ڴ�ش�С
#define JPEGMEMORYSIZE	500*1024	//ͼƬ��С������500kb

//����������ص�RAM�е�BMPͼƬʱ��ͼƬÿ�е��ֽ���
#define JPEGPERLINESIZE	2*1024		

int displyjpeg(u8 *JPEGFileName,u8 mode,u32 x,u32 y,int member,int denom);
int displayjpegex(u8 *JPEGFileName,u8 mode,u32 x,u32 y,int member,int denom);
void jpegdisplay_demo(void);
#endif
