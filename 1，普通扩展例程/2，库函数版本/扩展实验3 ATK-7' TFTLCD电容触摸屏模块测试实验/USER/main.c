#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	 
#include "string.h" 	 
#include "timer.h" 	 
#include "gt811.h" 	


//ALIENTEK探索者STM32F407开发板 扩展实验3
//ATK-7' TFTLCD 电容触摸屏模块测试实验  -库函数版本 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//01，电容触摸屏测试部分   

//电容触摸测试gui
void ctouch_paint_gui(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,lcddev.width,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
}
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
//5个触控点的颜色												 
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};  
//01,电容触摸屏测试
//测试电容触摸屏，最大支持5点触控。
void ctouch_paint_test(void)
{		
 	u8 i=0,t=0;	
	u16 lastpos[5][2];		//最后一次的数据   
	u8 	ctout[5];			//5个触摸点的释放计时器   	 		 	
 	LCD_Clear(WHITE);
	POINT_COLOR=RED;		//设置字体为红色
	Show_Str(60,50,lcddev.width,16,"测试1：电容触摸屏测试",16,0); 
	Show_Str(60,70,lcddev.width,16,"最大同时触摸点数：5点",16,0);    
	Show_Str(60,90,lcddev.width,16,"清屏：点击右上角的的‘RST’可以清除整个屏幕",16,0);    
  	while(ctp_dev.init())		   	//初始化电容触摸屏
	{
		Show_Str(60,110,lcddev.width,16,"电容触摸屏初始化失败！",16,0); 
		delay_ms(200);
		Show_Str(60,110,lcddev.width,16,"      请检查！！！    ",16,0); 
 		delay_ms(200);
	};
 	Show_Str(60,110,lcddev.width,16,"电容触摸屏初始化成功！",16,0); 
	delay_ms(1500);
	delay_ms(1500);
 	ctouch_paint_gui();	 
 	for(i=0;i<5;i++)
	{
		lastpos[i][0]=0XFFFF;	//全部设置为非法值
		lastpos[i][1]=0XFFFF;
		ctout[i]=0;	   			//计时器清零
	}
	while(1)
	{
 		ctp_dev.scan();	     
		if(ctp_dev.tpsta&0X1F)		//触摸屏被按下
		{	   	
			for(t=0;t<5;t++)
			{
				if(ctp_dev.tpsta&(1<<t))
				{
				 	if(ctp_dev.x[t]<lcddev.width&&ctp_dev.y[t]<lcddev.height)
					{	
						if(ctp_dev.x[t]>(lcddev.width-24)&&ctp_dev.y[t]<16)ctouch_paint_gui();//清除
						else 
						{
							if(lastpos[t][0]==0XFFFF)//属于第一次按下
							{
								lastpos[t][0]=ctp_dev.x[t];
								lastpos[t][1]=ctp_dev.y[t];	   
							}
	 						lcd_draw_bline(lastpos[t][0],lastpos[t][1],ctp_dev.x[t],ctp_dev.y[t],ctp_dev.ppr[t]/3+2,POINT_COLOR_TBL[t]);//画线
							lastpos[t][0]=ctp_dev.x[t];
							lastpos[t][1]=ctp_dev.y[t];	   
						}													  			   
					}
					ctout[t]=0;	
				}
			} 
			ctp_dev.tpsta=0;	 
		}else
		{						  		   
			delay_ms(5);	//没有按键按下的时候
			for(t=0;t<5;t++)
			{
				ctout[t]++;	
				if(ctout[t]>10)//判定此点以松开
				{
					lastpos[t][0]=0XFFFF;	 
					ctout[t]=0;
				}
			}  	 
 		}
		i++;
		if(i==20)
		{
			i=0;
			LED0=!LED0;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//02,图片显示测试部分

//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	return rval;
}   
//02,图片显示测试
//循环显示SD卡，PICTURE文件夹下面的图片文件。
void picture_display_test(void)
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u16 *picindextbl;	//图片索引表
	LCD_Clear(WHITE);
	Show_Str(60,50,lcddev.width,16,"测试2：图片显示测试",16,0); 
	Show_Str(60,70,lcddev.width,16,"KEY0:下一张图片",16,0); 
	Show_Str(60,90,lcddev.width,16,"KEY1:上一张图片",16,0); 
	Show_Str(60,110,lcddev.width,16,"KEY_UP:暂停/继续 自动播放",16,0); 
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(60,130,lcddev.width,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(60,130,lcddev.width,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//清除显示	     
		delay_ms(200);				  
	}	  
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(60,130,lcddev.width,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.index;								//记录当前index
	        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	delay_ms(1200);
	Show_Str(60,130,lcddev.width,16,"开始显示...",16,0); 
	delay_ms(1800);  
	piclib_init();										//初始化画图	   	   
  	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
  	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面 
		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);				//扫描按键
			if(t>250&&(pause==0))key=KEY0_PRES;//非暂停状态,每2.5秒，模拟一次按下KEY0  
			if(key==KEY1_PRES)				//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)		//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1=!pause; 	//暂停的时候LED1亮.  
			}	  
			t++;
			if((t%20)==0)LED0=!LED0;   	   
			delay_ms(10); 
		}					    
		res=0;  
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picindextbl);			//释放内存
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//03,液晶屏自测试

//得到一个随机数
//(min,max)期望的随机数范围
//返回值:符合期望的随机数值
u16 speed_test_get_rval(u16 min,u16 max)
{
	u16 t=0Xffff;
	while((t<min)||(t>max))t=rand();	  
	return t;
}												 
const u16 SPEED_COLOR_TBL[10]={RED,GREEN,BLUE,BROWN,GRED,BRRED,CYAN,YELLOW,GRAY,MAGENTA};
//得到速度测试一次填充的相关信息
//*x,*y,*width,*height,*color:获取到的填充坐标/尺寸/颜色等信息 
void speed_test_get_fill_parameter(u16 *x,u16 *y,u16 *width,u16 *height,u16 *color)
{
	*x=speed_test_get_rval(0,700);
	*y=speed_test_get_rval(0,380);
	*width=speed_test_get_rval(80,800-*x);
	*height=speed_test_get_rval(80,480-*y);
	*color=SPEED_COLOR_TBL[speed_test_get_rval(0,9)]; 
}   
u16 speed_run_time;	 	//速度测试测试时间长度(单位为10ms)
//3-1速度测试
void tftlcd_speed_test(void)
{
 	u16 x,y,width,height,color;
	u32 pixelcnt=0;
	u8 *str;
	LCD_Clear(WHITE);
 	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"测试3-1：速度测试",16,0); 
	Show_Str(60,70,lcddev.width,16,"通过在LCD各个位置随机的填充不同尺寸的单色矩形,来测试速度",16,0); 
	Show_Str(60,90,lcddev.width,16,"测试时长约为5秒,测试结果将显示在LCD上",16,0); 
 	delay_ms(1500);delay_ms(1500);
 	LCD_Clear(RED);
	str=mymalloc(SRAMIN,60);	//分配60个字节的内存
	srand(TIM3->CNT);
	TIM3->CR1&=~(1<<0);    		//关闭定时器3
	TIM3->CNT=0;				//清零
	speed_run_time=0;			//计时器清零
	while(1)
	{		
		speed_test_get_fill_parameter(&x,&y,&width,&height,&color);//得到各参数
		pixelcnt+=width*height;
		TIM3->CR1|=1<<0;  		//开启定时器3
		LCD_Fill(x,y,x+width-1,y+height-1,color);
		TIM3->CR1&=~(1<<0);		//关闭定时器3
		if(speed_run_time>300)break;//大于3秒钟
	}
	sprintf((char*)str,"LCD速度测试结果:%d 像素/秒",(pixelcnt*100)/speed_run_time);
 	Show_Str(270,230,lcddev.width,16,str,16,0); 				 
	myfree(SRAMIN,str);		//释放内存
	delay_ms(1800);delay_ms(1800);delay_ms(1400); 
}
//3-2镜像测试
void tftlcd_mirror_test(void)
{  					     
	LCD_Clear(WHITE);
	Show_Str(60,50,lcddev.width,16,"测试3-2：镜像测试",16,0); 
	Show_Str(60,70,lcddev.width,16,"测试步骤:默认显示/上下调转/左右调转/上下左右都调转",16,0); 
  	delay_ms(1500);delay_ms(1500);
 	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_DrawRectangle(30,30,180,130);
	LCD_Fill(lcddev.width-30-150,lcddev.height-30-100,lcddev.width-30,lcddev.height-30,BLUE);
	Draw_Circle(90,lcddev.height-90,60);
	gui_fill_circle(lcddev.width-90,90,60,GRED);
 	POINT_COLOR=BLUE;
	Show_Str(140,150,lcddev.width,16,"ATK-7' TFTLCD模块镜像功能测试",16,0);     
	LCD_Scan_Dir(L2R_U2D);//默认		 
   	delay_ms(1200);delay_ms(1300);		
 	LCD_Scan_Dir(L2R_D2U);//上下调转		 									    
   	delay_ms(1200);delay_ms(1300);		
  	LCD_Scan_Dir(R2L_U2D);//左右调转		 									    
   	delay_ms(1200);delay_ms(1300);		
	LCD_Scan_Dir(R2L_D2U);//上下左右都调转		 									    
   	delay_ms(1200);delay_ms(1300);		
	LCD_Scan_Dir(L2R_U2D);//恢复默认设置	 
}																						    
//3-3多缓存测试
void tftlcd_multi_gram_test(void)
{
	u8 i;
	u8 *str; 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"测试3-3：多缓存测试",16,0);	// 
	Show_Str(60,70,lcddev.width,16,"ATK-7' TFTLCD模块拥有多达8页LCD缓存,本测试将测试全部8页缓存",16,0); 
  	delay_ms(1500);delay_ms(1500);		
	str=mymalloc(SRAMIN,60);			//分配60个字节的内存
	for(i=0;i<8;i++)
	{
	 	LCD_SetOperateLayer(i);			//设置当前操作缓存
		LCD_SetDisplayLayer(i);			//设置当前显示缓存
		LCD_Clear(SPEED_COLOR_TBL[i]);
		sprintf((char*)str,"我是第%d页缓存",i);
		POINT_COLOR=BLACK;
 		Show_Str(360,230,lcddev.width,16,str,16,0); 				 
   		delay_ms(1200);delay_ms(1300);		
	}
	myfree(SRAMIN,str);		//释放内存
	//恢复默认设置
	LCD_SetOperateLayer(0);
	LCD_SetDisplayLayer(0); 
}
//3-4背光测试
void tftlcd_backlight_test(void)
{
	u8 i;
	u8 *str; 
	float bkl=0;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"测试3-4：背光测试",16,0); 
	Show_Str(60,70,lcddev.width,16,"ATK-7' TFTLCD模块自带背光控制功能,只需发送相关指令即可设置背光亮度",16,0); 
  	delay_ms(1500);delay_ms(1500);		
 	str=mymalloc(SRAMIN,60);			//分配60个字节的内存
	for(i=0;i<8;i++)
	{													    
		LCD_BackLightSet(i*8+7);	//背光亮度设置
		bkl=(float)(i+1)*8/64;
		sprintf((char*)str,"当前背光亮度:%3.1f%%",bkl*100);
		POINT_COLOR=BLUE;
 		Show_Str(330,230,lcddev.width,16,str,16,0); 				 
   		delay_ms(1200);delay_ms(1300);		
	}
	myfree(SRAMIN,str);		//释放内存
}	  
//03,液晶自测试
//速度测试/镜像测试/多缓存测试/背光测试,这几个循环进行测试
void tftlcd_self_test(void)
{
	while(1)
	{
		tftlcd_speed_test();
		tftlcd_mirror_test();
		tftlcd_multi_gram_test();
		tftlcd_backlight_test();
	}
}

int main(void)
{        
	u8 key;
	u8 t=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();				//初始化LED 
	usmart_dev.init(84);	//初始化USMART
 	LCD_Init();				//LCD初始化  
 	KEY_Init();				//按键初始化  
	W25QXX_Init();			//初始化W25Q128
 	TIM3_Int_Init(100-1,8400-1);//10Khz的计数频率，计数100次为10ms     
	my_mem_init(SRAMIN);	//初始化内部内存池 
	my_mem_init(SRAMCCM);	//初始化CCM内存池 
	exfuns_init();			//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 	//挂载SD卡 
 	f_mount(fs[1],"1:",1); 	//挂载FLASH.
	POINT_COLOR=RED;     
 	while(font_init()) 		//检查字库
	{	    
		POINT_COLOR=RED;      
		LCD_Clear(WHITE);
		LCD_ShowString(60,50,lcddev.width,16,16,"ALIENTEK STM32");	
		LCD_ShowString(60,70,lcddev.width,16,16,"Font Updating..."); 	 	 
 	 	while(update_font(60,90,16,0)!=0)//字体更新出错
		{						  
			LCD_ShowString(60,90,lcddev.width,16,16," Font Update error! ");		  
			delay_ms(200);    
			LCD_ShowString(60,90,lcddev.width,16,16,"  Please Check....  ");
			delay_ms(200);    
			LED0=!LED0;	
		};			 
		LCD_Clear(WHITE);				  
	}  
	Show_Str(60,50,lcddev.width,16,"ALIENTEK ATK-7' TFTLCD电容触摸屏测试实验",16,0);				    	 
	Show_Str(60,70,lcddev.width,16,"请选择测试模式：",16,0);	
	POINT_COLOR=BLUE;			    	 
	Show_Str(60,90,lcddev.width,16, "KEY0：电容触摸屏测试（支持5点触控）",16,0);				    	 
	Show_Str(60,110,lcddev.width,16,"KEY1：图片显示测试（需要SD卡支持）",16,0);				    	 
	Show_Str(60,130,lcddev.width,16,"KEY_UP：液晶自测试（速度/镜像/多缓存/背光）",16,0);				    	 
	POINT_COLOR=RED;			    	 
	Show_Str(60,170,lcddev.width,16,"广州市星翼电子科技有限公司(ALIENTEK)",16,0);				    	 
	Show_Str(60,190,lcddev.width,16,"官方网站：www.alientek.com",16,0);				    	 
	Show_Str(60,210,lcddev.width,16,"开源电子网论坛：www.openedv.com",16,0);				    	 
	Show_Str(60,230,lcddev.width,16,"电话(传真)：020-38271790",16,0);				    	 
	Show_Str(60,250,lcddev.width,16,"2014年4月1日",16,0);
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0按下,电容触摸测试
				ctouch_paint_test();
				break;
			case KEY1_PRES://KEY1按下,图片显示测试
				picture_display_test();
				break;
			case WKUP_PRES://KEY_UP按下,液晶自测试
				tftlcd_self_test();
				break;	 
		}
		t++;
		if(t==20)
		{		 				   
			t=0;
			LED0=!LED0;
			if(LED0)Show_Str(60,70,lcddev.width,16,"请选择测试模式：",16,0);
			else LCD_Fill(60,70,60+128,70+16,WHITE);
		}else delay_ms(10);
	}   		     	
}










