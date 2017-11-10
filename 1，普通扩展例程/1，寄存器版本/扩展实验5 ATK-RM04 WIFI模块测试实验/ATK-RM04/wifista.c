#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ATK-RM04 WIFIģ�� �������ߣ�COM-WIFI STA����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/10/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-RM04 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 atk_rm04_wifista_test(void)
{
	u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 *p; 
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	p=mymalloc(SRAMIN,100);							//����32�ֽ��ڴ�
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//����WIFI STA(wifi����)ģʽ
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP�������ر�(��APģʽ��Ч) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP�ͻ���ʹ��(net_ip��Ч)
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//�������߲���:ssid,���ܷ�ʽ,����
	atk_rm04_send_cmd(p,"ok",500);					//�������߲���
PRESTA:
	netpro=atk_rm04_netpro_sel(50,30,(u8*)ATK_RM04_NETMODE_TBL[2]);	//ѡ������ģʽ
	if(netpro&0X02)atk_rm04_send_cmd("at+remotepro=udp","ok",500);	//UDPЭ��
	else atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCPЭ��
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//���� �˿ں�. 
	if(netpro&0X01)	//�ͻ���
	{
		if(atk_rm04_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_RM04_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP���� 
		sprintf((char*)p,"at+remoteip=%s",ipbuf);
		atk_rm04_send_cmd(p,"ok",500);					//����Զ��(����)IP��ַ. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//����Ϊ�ͻ���		
	}else atk_rm04_send_cmd("at+mode=server","ok",500);	//����Ϊ�����				 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-RM04 WIFI-STA ����",16,240); 
	Show_Str(30,50,200,16,"��������ATK-RM04ģ��,���Ե�...",12,0); 	
	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//�ύ��������,�������Ҫ�ȴ�40s
 	{ 
		LCD_Fill(30,50,239,50+12,WHITE);	//���֮ǰ����ʾ
		Show_Str(30,50,200,16,"����ATK-RM04ģ��ʧ��!",12,0); 
		delay_ms(800);        
		res=1; 
	}else
	{	
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//��������ת������
		LCD_Fill(30,50,239,50+12,WHITE);			//���֮ǰ����ʾ
		Show_Str(30,50,200,16,"����ATK-RM04ģ��ɹ�!",12,0);		
		delay_ms(600);
		Show_Str(30,50,210,16,"KEY_UP:�˳�����  KEY0:��������",12,0); 
		atk_rm04_quit_trans();						//�˳�͸��
		while(atk_rm04_get_wifista_state()==0)		//�ȴ�ATK-RM04������·���� 
		{ 	
			Show_Str(30,80,200,12,"ATK-RM04 ����·����ʧ��",12,0); //����ʧ��	 
			delay_ms(800);     
			Show_Str(30,80,200,12,"ATK-RM04 �ȴ�������....",12,0);	  
			delay_ms(800); 
		}
		LCD_Fill(30,80,239,80+12,WHITE);
		if((netpro&0X01)==0)atk_rm04_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
 		sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
		Show_Str(30,65,200,12,p,12,0);				//��ʾIP��ַ�Ͷ˿�	
		Show_Str(30,80,200,12,"״̬:",12,0); 		//����״̬
		Show_Str(120,80,200,12,"ģʽ:",12,0); 		//����״̬
		Show_Str(30,100,200,12,"��������:",12,0); 	//��������
		Show_Str(30,115,200,12,"��������:",12,0);	//��������
		atk_rm04_wificonf_show(30,180,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
		POINT_COLOR=BLUE;
		Show_Str(120+30,80,200,12,(u8*)ATK_RM04_WORKMODE_TBL[netpro],12,0); 		//����״̬
		USART3_RX_STA=0;
		while(1)
		{
			key=KEY_Scan(0);
			if(key==WKUP_PRES)			//KEY_UP �˳�����		 
			{   
				res=0;
				break;												 
			}else if(key==KEY0_PRES)	//KEY0 �������� 
			{
				sprintf((char*)p,"ATK-RM04 %s����%02d\r\n",ATK_RM04_WORKMODE_TBL[netpro],t/10);//��������
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);		//���͸����ݵ�ATK-RM04ģ��
				timex=100;
			}
			if(timex)timex--;
			if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
			t++;
			delay_ms(10);
			if(USART3_RX_STA&0X8000)		//���յ�һ��������
			{ 
				rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
				USART3_RX_BUF[rlen]=0;		//��ӽ����� 
				printf("%s",USART3_RX_BUF);	//���͵�����   
				sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
				LCD_Fill(30+54,115,239,130,WHITE);
				POINT_COLOR=BRED;
				Show_Str(30+54,115,156,12,p,12,0); 			//��ʾ���յ������ݳ���
				POINT_COLOR=BLUE;
				LCD_Fill(30,130,239,319,WHITE);
				Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
				USART3_RX_STA=0;
				if(constate==0)t=1000;		//״̬Ϊ��δ����,������������״̬
				else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
			}  
			if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
			{
				constate=atk_rm04_consta_check()-'0';//�õ�����״̬
				if(constate)Show_Str(30+30,80,200,12,"���ӳɹ�",12,0);  //����״̬
				else Show_Str(30+30,80,200,12,"����ʧ��",12,0); 	 
				t=0;
			}
			if((t%20)==0)LED0=!LED0;
			atk_rm04_at_response(1);
		}
	} 
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	atk_rm04_quit_trans();	//�˳�͸��
	return res;
} 




























