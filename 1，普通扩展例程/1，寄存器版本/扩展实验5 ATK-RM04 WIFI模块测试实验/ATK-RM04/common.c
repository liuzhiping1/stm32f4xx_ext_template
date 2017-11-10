#include "common.h"
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
//用户配置区

//连接端口号:8086,可自行修改为其他端口.
const u8* portnum="8086";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="ALIENTEK";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="15902020353"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="ATK-RM04";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="12345678"; 		//连接密码 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_RM04_NETMODE_TBL[4]={" ROUTER ","ETH-COM ","WIFI-STA","WIFI-AP "};	//ATK-RM04,4种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_RM04_WORKMODE_TBL[4]={"TCP服务器","TCP客户端","UDP服务器","UDP客户端"};	//ATK-RM04,4种工作模式
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-RM04模块初始化
void atk_rm04_init(void)
{
#if AT_MODE_CONFIG==0
	RCC->AHB1ENR|=1<<5;//使能PORTF时钟  
	GPIO_Set(GPIOF,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF6 输出 高电平
	ES_CTRL=1;				//PA4输出高
#endif
 	usart3_init(42,115200);	//初始化串口3 	
} 
//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_rm04_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//ATK-RM04发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_rm04_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-RM04发送命令
//cmd:发送的命令字符串(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_rm04_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//ATK-RM04退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_rm04_quit_trans(void)
{
#if AT_MODE_CONFIG==1	//使用串口特殊序列退出透传,进入AT模式
 	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms) 
	return atk_rm04_send_cmd("at","at",20);//退出透传判断.
#else					//使用ES/RST引脚控制,退出透传,进入AT模式
	ES_CTRL=0;			//拉低ES/RST
	delay_ms(120);		//等待120ms
	ES_CTRL=1;			//ES/RST恢复
	return 0;
#endif
}
//获取ATK-RM04模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_rm04_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_rm04_quit_trans())return 0;			//退出透传 
	atk_rm04_send_cmd("at+S2N_Stat=?","?",50);	//发送at+S2N_Stat=?指令,查询连接状态
	p=atk_rm04_check_cmd("\r\n"); 
	res=*(p+2);									//得到连接状态	
	atk_rm04_send_cmd("at+out_trans=0","ok",50);//发送at+out_trans=0,进入透传模式
	return res;
}
//键盘码表
const u8* kbd_tbl[13]={"1","2","3","4","5","6","7","8","9",".","0","#","DEL"}; 
u8* kbd_fn_tbl[2];
//加载键盘界面（尺寸为240*140）
//x,y:界面起始坐标（320*240分辨率的时候，x必须为0）
void atk_rm04_load_keyboard(u16 x,u16 y)
{
	u16 i;
	POINT_COLOR=RED; 
	LCD_Fill(x,y,x+240,y+140,WHITE);
	LCD_DrawRectangle(x,y,x+240,y+140);						   
	LCD_DrawRectangle(x+80,y,x+160,y+140);	 
	LCD_DrawRectangle(x,y+28,x+240,y+56);
	LCD_DrawRectangle(x,y+84,x+240,y+112);
	POINT_COLOR=BLUE;
	for(i=0;i<15;i++)
	{
		if(i<13)Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),(u8*)kbd_tbl[i],16,80);
		else Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),kbd_fn_tbl[i-13],16,80); 
	}  		 					   
}
//按键状态设置
//x,y:键盘坐标
//key:键值（0~8）
//sta:状态，0，松开；1，按下；
void atk_rm04_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>15)return;
	if(sta)LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,GREEN);
	else LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,WHITE); 
	if(j&&(i>3))Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_fn_tbl[keyx-13],16,80);
	else Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_tbl[keyx],16,80);		 		 
}
//得到触摸屏的输入
//x,y:键盘坐标
//返回值：按键键值（1~15有效；0,无效）
u8 atk_rm04_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,没有任何按键按下；1~15，1~15号按键按下
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*28+28)&&tp_dev.y[0]>(y+i*28))
				{	
					key=i*3+j+1;	 
					break;	 		   
				}
			}
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					atk_rm04_key_staset(x,y,key_x-1,0);
					key_x=key;
					atk_rm04_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}  
	}else if(key_x) 
	{
		atk_rm04_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}
//获取wan ip地址
//ipbuf:ip地址输出缓存区
void atk_rm04_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_rm04_send_cmd("at+net_wanip=?","?",50))//获取WAN IP地址失败
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+2);
}
//获取WIFI STA连接状态(检查WIFI STA是否连上路由器)
//返回值:0,连接失败;1,连接成功;
u8 atk_rm04_get_wifista_state(void)
{
	u8 *p; 
	atk_rm04_send_cmd("at+wifi_ConState=?","?",20);	//获取WIFI STA连接状态
	p=atk_rm04_check_cmd("\r\n"); 
	return strstr((const char*)p,"Connected")?1:0;
}
//ATK-RM04模块信息显示
//x,y:显示信息的起始坐标.
//wanip:0,全部更新显示;1,仅更新wanip.
void atk_rm04_msg_show(u16 x,u16 y,u8 wanip)
{
	u8 *p,*p1;
	POINT_COLOR=BLUE;
	if(wanip==0)//全更新
	{
		atk_rm04_send_cmd("at+ver=?","?",20);		//获取版本号
		p=atk_rm04_check_cmd("\r\n");
		Show_Str(x,y,240,16,"版本:",16,0);Show_Str(x+40,y,240,16,p+2,16,0);
		atk_rm04_send_cmd("at+netmode=?","?",20);	//获取网络模式
		p=atk_rm04_check_cmd("\r\n");
		Show_Str(x,y+16,240,16,"网络模式:",16,0);Show_Str(x+72,y+16,240,16,(u8*)ATK_RM04_NETMODE_TBL[*(p+2)-'0'],16,0);
		atk_rm04_send_cmd("at+wifi_conf=?","?",20);	//获取wifi配置
		p=atk_rm04_check_cmd("\r\n");
		p1=(u8*)strstr((const char*)p,",");
		*p1=0;p1++;
		Show_Str(x,y+32,240,16,"SSID号:",16,0);Show_Str(x+56,y+32,240,16,p+2,16,0);
		p=p1;
		p1=(u8*)strstr((const char*)p,",");
		*p1=0;p1++;
		Show_Str(x,y+48,240,16,"加密方式:",16,0);Show_Str(x+72,y+48,240,16,p,16,0);
		Show_Str(x,y+64,240,16,"密码:",16,0);Show_Str(x+40,y+64,240,16,p1,16,0);
	}
	atk_rm04_send_cmd("at+net_wanip=?","?",20);	//获取WAN IP地址
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0; 
	Show_Str(x,y+80,240,16,"WAN IP:",16,0);Show_Str(x+56,y+80,240,16,p+2,16,0);
	POINT_COLOR=RED;
}
//ATK-RM04模块WIFI配置参数显示(仅WIFI STA/WIFI AP模式测试时使用)
//x,y:显示信息的起始坐标.
//rmd:提示信息
//ssid,encryption,password:无线网络的SSID,加密方式,密码
void atk_rm04_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password)
{ 
	POINT_COLOR=RED;
	Show_Str(x,y,240,12,rmd,12,0);//显示提示信息 
	Show_Str(x,y+20,240,12,"SSID:",12,0);
	Show_Str(x,y+36,240,12,"加密方式:",12,0);
	Show_Str(x,y+52,240,12,"密码:",12,0); 
	POINT_COLOR=BLUE;
	Show_Str(x+30,y+20,240,12,ssid,12,0);
	Show_Str(x+54,y+36,240,12,encryption,12,0);
	Show_Str(x+30,y+52,240,12,password,12,0); 	  
}
//工作模式选择
//返回值:
//0,TCP服务器
//1,TCP客户端
//2,UDP服务器
//3,UDP客户端
u8 atk_rm04_netpro_sel(u16 x,u16 y,u8* name) 
{
	u8 key,t=0,*p;
	u8 netpro=0;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	p=mymalloc(SRAMIN,50);//申请50个字节的内存
	sprintf((char*)p,"%s 工作模式选择",name);
	Show_Str_Mid(0,y,p,16,240); 				    	 
	Show_Str(x,y+30,200,16,"KEY0:下一个",16,0); 				    	 
	Show_Str(x,y+50,200,16,"KEY1:上一个",16,0);				    	 
	Show_Str(x,y+70,200,16,"KEY_UP:确定",16,0); 
	Show_Str(x,y+100,200,16,"请选择:",16,0); 
	POINT_COLOR=BLUE;
 	Show_Str(x+16,y+120,200,16,"TCP服务器",16,0); 				    	 
	Show_Str(x+16,y+140,200,16,"TCP客户端",16,0);				    	 
	Show_Str(x+16,y+160,200,16,"UDP服务器",16,0); 
	Show_Str(x+16,y+180,200,16,"UDP客户端",16,0); 
	POINT_COLOR=RED;
	Show_Str(x,y+120,200,16,"→",16,0); 
	while(1)
	{
		key=KEY_Scan(0); 
		if(key)
		{
			if(key==4)break; 
			Show_Str(x,y+120+netpro*20,200,16,"  ",16,0);//清空之前的显示
			if(key==1)//KEY0按下
			{
				if(netpro<3)netpro++;
				else netpro=0;
			}else if(key==2)//KEY1按下
			{
				if(netpro>0)netpro--;
				else netpro=3; 
			}
			Show_Str(x,y+120+netpro*20,200,16,"→",16,0);//指向新条目
				
		} 
		delay_ms(10);
		atk_rm04_at_response(1);
		if((t++)==20){t=0;LED0=!LED0;}//LED闪烁
	} 
	myfree(SRAMIN,p);
	return netpro;//返回网络模式选择值 
} 
//IP设置
//title:ip设置标题
//mode:工作模式
//port:端口号
//*ip:ip缓存区(返回IP给上层函数)
//返回值:0,确认连接;1,取消连接.
u8 atk_rm04_ip_set(u8* title,u8* mode,u8* port,u8* ip) 
{
	u8 res=0;
 	u8 key;
	u8 timex=0;  
	u8 iplen=0;			//IP长度 
	LCD_Clear(WHITE);  
	POINT_COLOR=RED; 
	Show_Str_Mid(0,30,title,16,240);		//显示标题	 	 	
	Show_Str(30,90,200,16,"工作模式:",16,0);	//工作模式显示
	Show_Str(30,110,200,16,"IP地址:",16,0);	//IP地址可以键盘设置
	Show_Str(30,130,200,16,"端口:",16,0);	//端口号
	kbd_fn_tbl[0]="连接";
	kbd_fn_tbl[1]="返回"; 
	atk_rm04_load_keyboard(0,180);			//显示键盘 
	POINT_COLOR=BLUE;
 	Show_Str(30+72,90,200,16,mode,16,0);	//显示工作模式	
 	Show_Str(30+40,130,200,16,port,16,0);	//显示端口 	
	ip[0]=0; 		
	while(1)
	{ 
		key=atk_rm04_get_keynum(0,180);
		if(key)
		{   
			if(key<12)
			{
				if(iplen<15)
				{ 
					ip[iplen++]=kbd_tbl[key-1][0];
				}
			}else
			{
				if(key==13)if(iplen)iplen--;	//删除  
				if(key==14&&iplen)break; 		//确认连接 
				if(key==15){res=1;break;}		//取消连接
			} 
			ip[iplen]=0; 
			LCD_Fill(30+56,110,239,110+16,WHITE);
			Show_Str(30+56,110,200,16,ip,16,0);			//显示IP地址 	
		} 
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		atk_rm04_at_response(1);//WIFI模块发过来的数据,及时上传给电脑
	} 
	return res;
}
//测试界面主UI
void atk_rm04_mtest_ui(u16 x,u16 y)
{ 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,y,"ATK-RM04 WIFI模块测试",16,240); 
	Show_Str(x,y+25,200,16,"请选择:",16,0); 				    	 
	Show_Str(x,y+45,200,16,"KEY0:串口以太网",16,0); 				    	 
	Show_Str(x,y+65,200,16,"KEY1:WIFI STA",16,0);				    	 
	Show_Str(x,y+85,200,16,"KEY_UP:WIFI AP",16,0); 
 	atk_rm04_msg_show(x,y+125,0);
}
//ATK-RM04模块测试主函数
void atk_rm04_test(void)
{
	u8 key;
	u8 timex;
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-RM04 WIFI模块测试",16,240); 
	atk_rm04_quit_trans();//退出透传 
	while(atk_rm04_send_cmd("at","\r\n",20))//检查WIFI模块是否在线
	{
		Show_Str(40,55,200,16,"未检测到模块!!!",16,0);
		delay_ms(800);
		LCD_Fill(40,55,200,55+16,WHITE);
		Show_Str(40,55,200,16,"尝试连接模块...",16,0);
		atk_rm04_quit_trans();//退出透传 
	} 	   
	atk_rm04_mtest_ui(32,30);
	while(1)
	{
		delay_ms(10); 
		atk_rm04_at_response(1);//检查ATK-RM04模块发送过来的数据,及时上传给电脑
		key=KEY_Scan(0); 
		if(key)
		{
			switch(key)
			{
				case KEY0_PRES://KEY0
					atk_rm04_cometh_test();	//串口以太网测试
					break;
				case KEY1_PRES://KEY1
					atk_rm04_wifista_test();//WIFI STA测试
					break;
				case WKUP_PRES://KEY_UP
					atk_rm04_wifiap_test();	//WIFI AP测试
					break;
			}
			atk_rm04_mtest_ui(32,30);
			timex=0;
		} 	
		if(timex==0)atk_rm04_msg_show(32,155,1);	//2.5秒左右更新一次 
		if((timex%20)==0)LED0=!LED0;//200ms闪烁 
		timex++;	 
	} 
}



















































