#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "includes.h"
//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��9-1 UCOSIII �����ʱ��

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define TASK1_TASK_PRIO		4
//�����ջ��С	
#define TASK1_STK_SIZE 		128
//������ƿ�
OS_TCB Task1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);



////////////////////////////////////////////////////////
OS_TMR 	tmr1;		//��ʱ��1
OS_TMR	tmr2;		//��ʱ��2
void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����
void tmr2_callback(void *p_tmr, void *p_arg);	//��ʱ��2�ص�����

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, RED,   BLUE,  BRED,      
						GRED,  GBLUE, BLACK,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);   //���ڳ�ʼ��
	LED_Init();         //LED��ʼ��	
	LCD_Init();			//LCD��ʼ��	
	KEY_Init();			//������ʼ��
	
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 9-1");
	LCD_ShowString(30,50,200,16,16,"KEY_UP:Start Tmr1");
	LCD_ShowString(30,70,200,16,16,"KEY0:Start Tmr2");
	LCD_ShowString(30,90,200,16,16,"KEY1:Stop Tmr1 and Tmr2");
	
	LCD_DrawLine(0,108,239,108);		//����
	LCD_DrawLine(119,108,119,319);		//����
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,115,314); 	//��һ������	
	LCD_DrawLine(5,130,115,130);		//����
	
	LCD_DrawRectangle(125,110,234,314); //��һ������	
	LCD_DrawLine(125,130,234,130);		//����
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,	 "TIMER1:000");
	LCD_ShowString(126,111,110,16,16,"TIMER2:000");
	
	OSInit(&err);		    	//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}


//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )20,			//20*10=200ms
                (OS_TICK	 )100,          //100*10=1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����					
	//������ʱ��2
	OSTmrCreate((OS_TMR		*)&tmr2,		
                (CPU_CHAR	*)"tmr2",		
                (OS_TICK	 )200,			//200*10=2000ms	
                (OS_TICK	 )0,   					
                (OS_OPT		 )OS_OPT_TMR_ONE_SHOT, 	//���ζ�ʱ��
                (OS_TMR_CALLBACK_PTR)tmr2_callback,	//��ʱ��2�ص�����
                (void	    *)0,			
                (OS_ERR	    *)&err);	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����TASK1����
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//����1��������
void task1_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:     //��key_up���µĻ��򿪶�ʱ��1
				OSTmrStart(&tmr1,&err);	//������ʱ��1
				printf("������ʱ��1\r\n");
				break;
			case KEY0_PRES:		//��key0���µĻ��򿪶�ʱ��2
				OSTmrStart(&tmr2,&err);	//������ʱ��2
				printf("������ʱ��2\r\n");
				break;
			case KEY1_PRES:		//��key1���»��͹رն�ʱ��
				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err);	//�رն�ʱ��1
				OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err);	//�رն�ʱ��2
				printf("�رն�ʱ��1��2\r\n");
				break;	
		}
		num++;
		if(num==50) //ÿ500msLED0��˸һ��
		{
			num = 0;
			LED0 = ~LED0;	
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}

//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr, void *p_arg)
{
	static u8 tmr1_num=0;
	LCD_ShowxNum(62,111,tmr1_num,3,16,0x80); //��ʾ��ʱ��1��ִ�д���
	LCD_Fill(6,131,114,313,lcd_discolor[tmr1_num%14]);//�������
	tmr1_num++;		//��ʱ��1ִ�д�����1
}

//��ʱ��2�Ļص�����
void tmr2_callback(void *p_tmr,void *p_arg)
{
	static u8 tmr2_num = 0;
	tmr2_num++;		//��ʱ��2ִ�д�����1
	LCD_ShowxNum(182,111,tmr2_num,3,16,0x80);  //��ʾ��ʱ��1ִ�д���
	LCD_Fill(126,131,233,313,lcd_discolor[tmr2_num%14]); //�������
	LED1 = ~LED1;
	printf("��ʱ��2���н���\r\n");
}




	


