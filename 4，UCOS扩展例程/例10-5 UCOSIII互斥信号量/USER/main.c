#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "includes.h"
//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��10-5 UCOSIII�����ź���

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
#define START_TASK_PRIO			10
//�����ջ��С	
#define START_STK_SIZE 			128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define HIGH_TASK_PRIO			7
//�����ջ��С	
#define HIGH_STK_SIZE 			128
//������ƿ�
OS_TCB High_TaskTCB;
//�����ջ	
CPU_STK HIGH_TASK_STK[HIGH_STK_SIZE];
void high_task(void *p_arg);


//�������ȼ�
#define MIDDLE_TASK_PRIO		8
//�����ջ��С	
#define MIDDLE_STK_SIZE 		128
//������ƿ�
OS_TCB Middle_TaskTCB;
//�����ջ	
CPU_STK MIDDLE_TASK_STK[MIDDLE_STK_SIZE];
void middle_task(void *p_arg);

//�������ȼ�
#define LOW_TASK_PRIO			9
//�����ջ��С	
#define LOW_STK_SIZE 			128
//������ƿ�
OS_TCB Low_TaskTCB;
//�����ջ	
CPU_STK LOW_TASK_STK[LOW_STK_SIZE];
void low_task(void *p_arg);

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

OS_MUTEX	TEST_MUTEX;		//����һ�������ź���

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);   //���ڳ�ʼ��
	LED_Init();         //LED��ʼ
	LCD_Init();			//LCD��ʼ��	
	
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 10-5");
	LCD_ShowString(30,50,200,16,16,"Mutex test");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2014/12/5");
	
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
	//����һ�������ź���
	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,
				  (CPU_CHAR*	)"TEST_MUTEX",
                  (OS_ERR*		)&err);
	//����HIGH����
	OSTaskCreate((OS_TCB 	* )&High_TaskTCB,		
				 (CPU_CHAR	* )"High task", 		
                 (OS_TASK_PTR )high_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )HIGH_TASK_PRIO,     
                 (CPU_STK   * )&HIGH_TASK_STK[0],	
                 (CPU_STK_SIZE)HIGH_STK_SIZE/10,	
                 (CPU_STK_SIZE)HIGH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//����MIDDLE����
	OSTaskCreate((OS_TCB 	* )&Middle_TaskTCB,		
				 (CPU_CHAR	* )"Middle task", 		
                 (OS_TASK_PTR )middle_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MIDDLE_TASK_PRIO,     
                 (CPU_STK   * )&MIDDLE_TASK_STK[0],	
                 (CPU_STK_SIZE)MIDDLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)MIDDLE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);		
	//����LOW����
	OSTaskCreate((OS_TCB 	* )&Low_TaskTCB,		
				 (CPU_CHAR	* )"Low task", 		
                 (OS_TASK_PTR )low_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LOW_TASK_PRIO,     
                 (CPU_STK   * )&LOW_TASK_STK[0],	
                 (CPU_STK_SIZE)LOW_STK_SIZE/10,	
                 (CPU_STK_SIZE)LOW_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);					 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//�����ȼ������������
void high_task(void *p_arg)
{
	u8 num;
	OS_ERR err;
	
	CPU_SR_ALLOC();
	POINT_COLOR = BLACK;
	OS_CRITICAL_ENTER();
	LCD_DrawRectangle(5,110,115,314); 	//��һ������	
	LCD_DrawLine(5,130,115,130);		//����
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,"High Task");
	OS_CRITICAL_EXIT();
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);   		//��ʱ500ms
		num++;
		printf("high task Pend Mutex\r\n");
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//���󻥳��ź���
		printf("high task Running!\r\n");
		LCD_Fill(6,131,114,313,lcd_discolor[num%14]); 				//�������
		LED1 = ~LED1;
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);				//�ͷŻ����ź���
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);   		//��ʱ500ms
	}
}

//�е����ȼ������������
void middle_task(void *p_arg)
{	
	u8 num;
	OS_ERR err;
	CPU_SR_ALLOC();
	
	POINT_COLOR = BLACK;
	OS_CRITICAL_ENTER();
	LCD_DrawRectangle(125,110,234,314); //��һ������	
	LCD_DrawLine(125,130,234,130);		//����
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"Middle Task");
	OS_CRITICAL_EXIT();
	while(1)
	{
		num++;
		printf("middle task Running!\r\n");
		LCD_Fill(126,131,233,313,lcd_discolor[13-num%14]); //�������
		LED0 = ~LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //��ʱ1s
	}
}

//�����ȼ������������
void low_task(void *p_arg)
{	
	static u32 times;
	OS_ERR err;
	while(1)
	{
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//���󻥳��ź���
		printf("low task Running!\r\n");
		for(times=0;times<20000000;times++)
		{
			OSSched();											//�����������
		}
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);			//�ͷŻ����ź���
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   	//��ʱ1s
	}
}




	


