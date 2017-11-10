#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "lcd.h"   
#include "w25qxx.h" 	 
#include "touch.h" 	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ATK-RM04 WIFIģ�� ������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/10/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
#define AT_MODE_CONFIG		0				//0,����ES/RSTӲ�����ƽ���ATģʽ;
											//1,���ô�����������(+++1B1B1B)����ATģʽ
//ES/RST�źſ���IO��.
#define ES_CTRL				PFout(6)		//ES/RST��������
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_rm04_init(void);

void atk_rm04_at_response(u8 mode);
u8* atk_rm04_check_cmd(u8 *str);
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 atk_rm04_quit_trans(void);
u8 atk_rm04_consta_check(void);
void atk_rm04_load_keyboard(u16 x,u16 y);
void atk_rm04_key_staset(u16 x,u16 y,u8 keyx,u8 sta);
u8 atk_rm04_get_keynum(u16 x,u16 y);
void atk_rm04_get_wanip(u8* ipbuf);
u8 atk_rm04_get_wifista_state(void);
void atk_rm04_msg_show(u16 x,u16 y,u8 wanip);
void atk_rm04_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password);
u8 atk_rm04_netpro_sel(u16 x,u16 y,u8* name);
void atk_rm04_mtest_ui(u16 x,u16 y);

u8 atk_rm04_ip_set(u8* title,u8* mode,u8* port,u8* ip);
void atk_rm04_test(void);



u8 atk_rm04_cometh_test(void);	//������̫������
u8 atk_rm04_wifista_test(void);	//WIFI STA����
u8 atk_rm04_wifiap_test(void);	//WIFI AP����

//�û����ò���
extern const u8* portnum;			//���Ӷ˿�
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA ���ܷ�ʽ
extern const u8* wifista_password; 	//WIFI STA ����

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP ���ܷ�ʽ
extern const u8* wifiap_password; 	//WIFI AP ����

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_RM04_NETMODE_TBL[4];
extern const u8* ATK_RM04_WORKMODE_TBL[4];
#endif





