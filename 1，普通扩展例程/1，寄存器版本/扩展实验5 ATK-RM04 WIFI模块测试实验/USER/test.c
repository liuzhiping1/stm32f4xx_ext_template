#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"	
#include "touch.h"		
#include "usart3.h"
#include "common.h" 
//ALIENTEK探索者STM32F407开发板 扩展实验5
//ATK-RM04 WIFI模块测试实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

 
int main(void)
{
	u8 key,fontok=0; 
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	tp_dev.init();				//初始化触摸屏
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	key=KEY_Scan(0);  
	if(key==KEY0_PRES)		//强制校准
	{
		LCD_Clear(WHITE);	//清屏
		TP_Adjust();  		//屏幕校准 
		TP_Save_Adjdata();	  
		LCD_Clear(WHITE);	//清屏
	}
	fontok=font_init();		//检查字库是否OK
	if(fontok||key==KEY1_PRES)//需要更新字库				 
	{
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(60,50,200,16,16,"ALIENTEK STM32");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//从SD卡更新
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}  
	atk_rm04_init();		//初始化RM04模块
	atk_rm04_test();		//进入RM04测试
}
















