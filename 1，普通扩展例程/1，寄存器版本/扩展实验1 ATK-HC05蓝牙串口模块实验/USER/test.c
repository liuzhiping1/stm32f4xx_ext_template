#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 
//ALIENTEK̽����STM32F407��������չʵ��1
//ATK-HC05��������ģ��ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
 

//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,140,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(30,140,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,140,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(120,140,120,16,16,"STA:Disconnect");	 			//δ����				 
}	  
int main(void)
{	 
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;  
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200 
	usmart_dev.init(84); 		//��ʼ��USMART		
	LED_Init();					//��ʼ��LED
	KEY_Init();					//��ʼ������
 	LCD_Init();		 			//��ʼ��LCD
	usmart_dev.init(72); 		//��ʼ��USMART 	  
	POINT_COLOR=RED;
	LCD_ShowString(30,30,200,16,16,"ALIENTEK STM32F4 ^_^");	
	LCD_ShowString(30,50,200,16,16,"HC05 BLUETOOTH COM TEST");	
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	delay_ms(1000);			//�ȴ�����ģ���ϵ��ȶ�
 	while(HC05_Init()) 		//��ʼ��ATK-HC05ģ��  
	{
		LCD_ShowString(30,90,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,90,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	LCD_ShowString(30,90,210,16,16,"KEY_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,110,200,16,16,"ATK-HC05 Standby!");  
  	LCD_ShowString(30,160,200,16,16,"Send:");	
	LCD_ShowString(30,180,200,16,16,"Receive:"); 
	POINT_COLOR=BLUE;
	HC05_Role_Show();
	delay_ms(100);
	USART3_RX_STA=0;
 	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//�л�ģ����������
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//״̬ȡ��	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//��λATK-HC05ģ��
				delay_ms(200);
			}
		}else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//����/ֹͣ����  	 
			if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
		}else delay_ms(10);	   
		if(t==50)
		{
			if(sendmask)					//��ʱ����
			{
				sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//��ʾ��������	
				u3_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//���͵�����ģ��
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART3_RX_STA&0X8000)			//���յ�һ��������
		{
			LCD_Fill(30,200,240,320,WHITE);	//�����ʾ
 			reclen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART3_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
 			LCD_ShowString(30,200,209,119,16,USART3_RX_BUF);//��ʾ���յ�������
 			USART3_RX_STA=0;	 
		}	 															     				   
		t++;	
	}											    
}

 
















