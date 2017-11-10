#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ATK-RM04 WIFI模块 串口无线（COM-WIFI AP）驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/10/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//ATK-RM04 WIFI AP测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_rm04_wifiap_test(void)
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
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	atk_rm04_send_cmd("at+netmode=3","ok",500);		//设置WIFI AP模式
	atk_rm04_send_cmd("at+dhcpd=1","ok",500);		//DHCP服务器开启(仅AP模式有效,默认路由器模式也有效) 
	atk_rm04_send_cmd("at+dhcpc=0","ok",500);		//DHCP客户端禁止(net_ip有效)
	
	//设置DHCP IP地址范围,以及子网掩码和网关等,一般不用变.
	//这里设置DHCP范围:192.168.16.100~192.168.16.200,网关为:192.168.16.1
	atk_rm04_send_cmd("at+dhcpd_ip=192.168.16.100,192.168.16.200,255.255.255.0,192.168.16.1","ok",500);
	atk_rm04_send_cmd("at+dhcpd_dns=192.168.16.1,0.0.0.0","ok",500);//DHCP服务器DNS设置
	//网络IP设置.设置模块对外的IP地址.子网掩码以及网关等.
	//这里设置模块对外地址为:192.168.16.254,子网掩码和网关与前面一样
	atk_rm04_send_cmd("at+net_ip=192.168.16.254,255.255.255.0,192.168.16.1","ok",500);
	atk_rm04_send_cmd("at+net_dns=192.168.16.1,0.0.0.0","ok",500);//设置DNS设置
	 
	//WIFI AP模式模块对外的WIFI网络名称/加密方式/密码,这几个参数可以根据您自己的需要进行修改!!
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifiap_ssid,wifiap_encryption,wifiap_password);//设置无线参数:ssid,加密方式,密码
	atk_rm04_send_cmd(p,"ok",500);					//配置无线参数
PRESTA:
	netpro=atk_rm04_netpro_sel(50,30,(u8*)ATK_RM04_NETMODE_TBL[3]);	//选择网络模式
	if(netpro&0X02)atk_rm04_send_cmd("at+remotepro=udp","ok",500);	//UDP协议
	else atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCP协议
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//设置 端口号. 
	if(netpro&0X01)	//客户端
	{
		if(atk_rm04_ip_set("WIFI-AP 远端IP设置",(u8*)ATK_RM04_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入 
		sprintf((char*)p,"at+remoteip=%s",ipbuf);
		atk_rm04_send_cmd(p,"ok",500);					//设置远端(连接)IP地址. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//设置为客户端		
	}else atk_rm04_send_cmd("at+mode=server","ok",500);	//设置为服务端				 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-RM04 WIFI-AP 测试",16,240); 
	Show_Str(30,50,200,16,"正在配置ATK-RM04模块,请稍等...",12,0); 	
	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//提交网络配置,最长可能需要等待40s
 	{ 
		LCD_Fill(30,50,239,50+12,WHITE);	//清除之前的显示
		Show_Str(30,50,200,16,"配置ATK-RM04模块失败!",12,0); 
		delay_ms(800);        
		res=1; 
	}else
	{	
		atk_rm04_send_cmd("at+reconn=1","ok",500);		//重启串口转换服务
		LCD_Fill(30,50,239,50+12,WHITE);//清除之前的显示
		Show_Str(30,50,200,16,"配置ATK-RM04模块成功!",12,0);		
		Show_Str(30,50,210,16,"KEY_UP:退出测试  KEY0:发送数据",12,0); 
		while((netpro&0X01)==0)//服务器模式
		{
			atk_rm04_quit_trans();		//退出透传
			atk_rm04_get_wanip(ipbuf);	//获取WAN IP
			if(ipbuf[0])break;
 		}
		sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
		Show_Str(30,65,200,12,p,12,0);				//显示IP地址和端口	
		Show_Str(30,80,200,12,"状态:",12,0); 		//连接状态
		Show_Str(120,80,200,12,"模式:",12,0); 		//连接状态
		Show_Str(30,100,200,12,"发送数据:",12,0); 	//发送数据
		Show_Str(30,115,200,12,"接收数据:",12,0);	//接收数据
		atk_rm04_wificonf_show(30,180,"ATK-RM04模块无线参数:",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
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




























