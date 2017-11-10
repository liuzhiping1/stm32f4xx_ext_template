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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ATK-RM04 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/10/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
#define AT_MODE_CONFIG		0				//0,采用ES/RST硬件控制进入AT模式;
											//1,采用串口特殊序列(+++1B1B1B)进入AT模式
//ES/RST信号控制IO口.
#define ES_CTRL				PFout(6)		//ES/RST控制引脚
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



u8 atk_rm04_cometh_test(void);	//串口以太网测试
u8 atk_rm04_wifista_test(void);	//WIFI STA测试
u8 atk_rm04_wifiap_test(void);	//WIFI AP测试

//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP 加密方式
extern const u8* wifiap_password; 	//WIFI AP 密码

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_RM04_NETMODE_TBL[4];
extern const u8* ATK_RM04_WORKMODE_TBL[4];
#endif





