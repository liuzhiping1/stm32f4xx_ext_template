#ifndef _GT811_H
#define	_GT811_H    
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//7寸电容触摸屏驱动-GT811部分	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2013/3/17
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////	 
//相关IO定义
//IO操作函数	 
#define CT_RST    		PCout(13)	//GT811复位引脚
#define CT_INT    		PBin(1)		//GT811中断引脚	
//////////////////////////////////////////////////////////////////////////////////	 
#define CT_CMD_WR		0XBA	//写数据命令
#define CT_CMD_RD   	0XBB	//读数据命令
#define CT_EXCHG_XY		1		//调转XY坐标

#define CT_MAX_TOUCH    5		//电容触摸屏最大支持的点数
#define CT_READ_XY_REG 	0x721  	//读取坐标寄存器 
#define CT_CONFIG_REG   0x6A2	//配置参数起始寄存器

//电容触摸屏控制器
typedef struct
{
	u8   (*init)(void);			//初始化触摸屏控制器
	void (*scan)(void);			//扫描触摸屏 	 
	u16 x[CT_MAX_TOUCH];		//触摸X坐标
	u16 y[CT_MAX_TOUCH];		//触摸Y坐标
 	u8	ppr[CT_MAX_TOUCH];		//触摸点的压力	 
	u8 tpsta;					//触摸屏状态
								//[0]:0,触摸点0无效;1,触摸点0有效
								//[1]:0,触摸点1无效;1,触摸点1有效
								//[2]:0,触摸点2无效;1,触摸点2有效
								//[3]:0,触摸点3无效;1,触摸点3有效
								//[4]:0,触摸点4无效;1,触摸点4有效
								//[5:6]:保留
								//[7]:0,没有有效触摸;1,有有效触摸,可以读出数据
				   	    	  
}_m_ctp_dev;
extern _m_ctp_dev ctp_dev;

u8 GT811_WR_Reg(u16 reg,u8 *buf,u8 len);//向GT811写入一次数据						    
u8 GT811_WR_Reg(u16 reg,u8 *buf,u8 len);//从GT811读出一次数据
u8 GT811_Send_Cfg(u8 * buf,u16 cfg_len);//向GT811发送配置参数
void GT811_Wakeup_Sleep(void);			//唤醒GT811
u8 GT811_Init(void);	    			//初始化GT811						 
void GT811_Scan(void);					//扫描GT811	    
#endif  






























