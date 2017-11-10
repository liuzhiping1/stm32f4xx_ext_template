#include "usmart.h"
#include "usmart_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
#include "delay.h"	 
#include "fattester.h"  
#include "piclib.h"  
											  
//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		    
  	(void*)LCD_Fast_DrawPoint,"void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)",
 	(void*)LCD_ReadPoint,"u16  LCD_ReadPoint(u16 x,u16 y)",
  	(void*)LCD_ShowString,"void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)",
 
 	(void*)LCD_SetDisplayLayer,"void  LCD_SetDisplayLayer(u8 layer)",
 	(void*)LCD_SetOperateLayer,"void  LCD_SetOperateLayer(u8 layer)",
 	(void*)LCD_Display_Dir,"void LCD_Display_Dir(u8 dir)",
 	(void*)LCD_Scan_Dir,"void LCD_Scan_Dir(u8 dir)",
 	(void*)LCD_BackLightSet,"void LCD_BackLightSet(u8 pwm)",
	(void*)LCD_EndXSet,"void LCD_EndXSet(u16 x)",
 	(void*)LCD_Clear," void LCD_Clear(u16 color)",
 	(void*)LCD_Init,"void LCD_Init(void)",
		
	(void*)mf_scan_files,"u8 mf_scan_files(u8 * path)", 	 
	(void*)ai_load_picfile,"u8 ai_load_picfile(const u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 fast)", 	 
  	(void*)bmp_encode,"u8 bmp_encode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 mode)", 
};			  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















