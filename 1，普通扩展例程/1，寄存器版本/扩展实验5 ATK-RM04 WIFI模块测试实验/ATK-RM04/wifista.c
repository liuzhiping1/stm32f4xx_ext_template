#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ATK-RM04 WIFI模块 串口无线（COM-WIFI STA）驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/10/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-RM04 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_rm04_wifista_test(void)
{
	u8 netpro=0;	//网络模式
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 *p; 
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(SRAMIN,100);							//申请32字节内存
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//设置WIFI STA(wifi网卡)模式
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP服务器关闭(仅AP模式有效) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP客户端使能(net_ip无效)
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//设置无线参数:ssid,加密方式,密码
	atk_rm04_send_cmd(p,"ok",500);					//配置无线参数
PRESTA:
	netpro=atk_rm04_netpro_sel(50,30,(u8*)ATK_RM04_NETMODE_TBL[2]);	//选择网络模式
	if(netpro&0X02)atk_rm04_send_cmd("at+remotepro=udp","ok",500);	//UDP协议
	else atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCP协议
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//设置 端口号. 
	if(netpro&0X01)	//客户端
	{
		if(atk_rm04_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_RM04_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入 
		sprintf((char*)p,"at+remoteip=%s",ipbuf);
		atk_rm04_send_cmd(p,"ok",500);					//设置远端(连接)IP地址. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//设置为客户端		
	}else atk_rm04_send_cmd("at+mode=server","ok",500);	//设置为服务端				 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-RM04 WIFI-STA 测试",16,240); 
	Show_Str(30,50,200,16,"正在配置ATK-RM04模块,请稍等...",12,0); 	
	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//提交网络配置,最长可能需要等待40s
 	{ 
		LCD_Fill(30,50,239,50+12,WHITE);	//清除之前的显示
		Show_Str(30,50,200,16,"配置ATK-RM04模块失败!",12,0); 
		delay_ms(800);        
		res=1; 
	}else
	{	
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//重启串口转换服务
		LCD_Fill(30,50,239,50+12,WHITE);			//清除之前的显示
		Show_Str(30,50,200,16,"配置ATK-RM04模块成功!",12,0);		
		delay_ms(600);
		Show_Str(30,50,210,16,"KEY_UP:退出测试  KEY0:发送数据",12,0); 
		atk_rm04_quit_trans();						//退出透传
		while(atk_rm04_get_wifista_state()==0)		//等待ATK-RM04连接上路由器 
		{ 	
			Show_Str(30,80,200,12,"ATK-RM04 连接路由器失败",12,0); //连接失败	 
			delay_ms(800);     
			Show_Str(30,80,200,12,"ATK-RM04 等待连接中....",12,0);	  
			delay_ms(800); 
		}
		LCD_Fill(30,80,239,80+12,WHITE);
		if((netpro&0X01)==0)atk_rm04_get_wanip(ipbuf);//服务器模式,获取WAN IP
 		sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
		Show_Str(30,65,200,12,p,12,0);				//显示IP地址和端口	
		Show_Str(30,80,200,12,"状态:",12,0); 		//连接状态
		Show_Str(120,80,200,12,"模式:",12,0); 		//连接状态
		Show_Str(30,100,200,12,"发送数据:",12,0); 	//发送数据
		Show_Str(30,115,200,12,"接收数据:",12,0);	//接收数据
		atk_rm04_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
		POINT_COLOR=BLUE;
		Show_Str(120+30,80,200,12,(u8*)ATK_RM04_WORKMODE_TBL[netpro],12,0); 		//连接状态
		USART3_RX_STA=0;
		while(1)
		{
			key=KEY_Scan(0);
			if(key==WKUP_PRES)			//KEY_UP 退出测试		 
			{   
				res=0;
				break;												 
			}else if(key==KEY0_PRES)	//KEY0 发送数据 
			{
				sprintf((char*)p,"ATK-RM04 %s测试%02d\r\n",ATK_RM04_WORKMODE_TBL[netpro],t/10);//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);		//发送该数据到ATK-RM04模块
				timex=100;
			}
			if(timex)timex--;
			if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
			t++;
			delay_ms(10);
			if(USART3_RX_STA&0X8000)		//接收到一次数据了
			{ 
				rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
				USART3_RX_BUF[rlen]=0;		//添加结束符 
				printf("%s",USART3_RX_BUF);	//发送到串口   
				sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
				LCD_Fill(30+54,115,239,130,WHITE);
				POINT_COLOR=BRED;
				Show_Str(30+54,115,156,12,p,12,0); 			//显示接收到的数据长度
				POINT_COLOR=BLUE;
				LCD_Fill(30,130,239,319,WHITE);
				Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
				USART3_RX_STA=0;
				if(constate==0)t=1000;		//状态为还未连接,立即更新连接状态
				else t=0;                   //状态为已经连接了,10秒后再检查
			}  
			if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
			{
				constate=atk_rm04_consta_check()-'0';//得到连接状态
				if(constate)Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
				else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
				t=0;
			}
			if((t%20)==0)LED0=!LED0;
			atk_rm04_at_response(1);
		}
	} 
	myfree(SRAMIN,p);		//释放内存 
	atk_rm04_quit_trans();	//退出透传
	return res;
} 




























