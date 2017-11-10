#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "beep.h"
#include "key.h"
#include "includes.h"
//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��12-1 UCOSIII�¼���־��

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
#define MAIN_TASK_PRIO		4
//�����ջ��С	
#define MAIN_STK_SIZE 		128
//������ƿ�
OS_TCB Main_TaskTCB;
//�����ջ	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);

//�������ȼ�
#define FLAGSPROCESS_TASK_PRIO	5
//�����ջ��С	
#define FLAGSPROCESS_STK_SIZE 	128
//������ƿ�
OS_TCB Flagsprocess_TaskTCB;
//�����ջ	
CPU_STK FLAGSPROCESS_TASK_STK[FLAGSPROCESS_STK_SIZE];
//������
void flagsprocess_task(void *p_arg);


//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

////////////////////////�¼���־��//////////////////////////////
#define KEY0_FLAG		0x01
#define KEY1_FLAG		0x02
#define KEYFLAGS_VALUE	0X00						
OS_FLAG_GRP	EventFlags;		//����һ���¼���־��
						
						
//����������
void ucos_load_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 12-1");
	LCD_ShowString(30,50,200,16,16,"Event Flags");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2014/12/9");
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,130,234,314);	//������
	POINT_COLOR = BLUE;
	LCD_ShowString(30,110,220,16,16,"Event Flags Value:0");
}


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
	BEEP_Init();		//��ʼ��������
	FSMC_SRAM_Init();	//��ʼ��SRAM
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	ucos_load_main_ui();//������UI
	
	OSInit(&err);		    //��ʼ��UCOSIII
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
		
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����һ���¼���־��
	OSFlagCreate((OS_FLAG_GRP*)&EventFlags,		//ָ���¼���־��
                 (CPU_CHAR*	  )"Event Flags",	//����
                 (OS_FLAGS	  )KEYFLAGS_VALUE,	//�¼���־���ʼֵ
                 (OS_ERR*  	  )&err);			//������
	//����������
	OSTaskCreate((OS_TCB*     )&Main_TaskTCB,		
				 (CPU_CHAR*   )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK*    )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);						
	//����MSGDIS����
	OSTaskCreate((OS_TCB*     )&Flagsprocess_TaskTCB,		
				 (CPU_CHAR*   )"Flagsprocess task", 		
                 (OS_TASK_PTR )flagsprocess_task, 			
                 (void* 	  )0,					
                 (OS_PRIO	  )FLAGSPROCESS_TASK_PRIO,     
                 (CPU_STK* 	  )&FLAGSPROCESS_TASK_STK[0],	
                 (CPU_STK_SIZE)FLAGSPROCESS_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLAGSPROCESS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void* 	  )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR* 	  )&err);	
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//�������������
void main_task(void *p_arg)
{
	u8 key,num;
	OS_FLAGS flags_num;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);  //ɨ�谴��
		if(key == KEY0_PRES)
		{
			//���¼���־��EventFlags���ͱ�־
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY0_FLAG,
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
					             (OS_ERR*	  )&err);
			LCD_ShowxNum(174,110,flags_num,1,16,0);
			printf("�¼���־��EventFlags��ֵ:%d\r\n",flags_num);
		}
		else if(key == KEY1_PRES)
		{
			//���¼���־��EventFlags���ͱ�־
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY1_FLAG,
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
								 (OS_ERR*     )&err);
			LCD_ShowxNum(174,110,flags_num,1,16,0);
			printf("�¼���־��EventFlags��ֵ:%d\r\n",flags_num);
		}
		num++;
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}

//�¼���־�鴦������
void flagsprocess_task(void *p_arg)
{
	u8 num;
	OS_ERR err; 
	while(1)
	{
		//�ȴ��¼���־��
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
				   (OS_FLAGS	)KEY0_FLAG+KEY1_FLAG,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		num++;
		LED1 = ~LED1;
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);
		printf("�¼���־��EventFlags��ֵ:%d\r\n",EventFlags.Flags);
		LCD_ShowxNum(174,110,EventFlags.Flags,1,16,0);
	}
}





	


