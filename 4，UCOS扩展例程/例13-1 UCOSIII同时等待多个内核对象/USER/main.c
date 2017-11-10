#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "sram.h"
#include "malloc.h"
#include "includes.h"
//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��13-1 UCOSIII ͬʱ�ȴ�����ں˶���

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


//�������ȼ�
#define TASK2_TASK_PRIO		5
//�����ջ��С	
#define TASK2_STK_SIZE 		128
//������ƿ�
OS_TCB Task2_TaskTCB;
//�����ջ	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
void task2_task(void *p_arg);

//�������ȼ�
#define MULTI_TASK_PRIO		6
//�����ջ��С	
#define MULTI_STK_SIZE 		128
//������ƿ�
OS_TCB Multi_TaskTCB;
//�����ջ	
CPU_STK MULTI_TASK_STK[MULTI_STK_SIZE];
void multi_task(void *p_arg);

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

OS_SEM	Test_Sem1;			//�ź���1		
OS_SEM	Test_Sem2;			//�ź���2
OS_Q	Test_Q;				//��Ϣ����						
#define QUEUE_NUM		10	//��Ϣ���г���
#define CORE_OBJ_NUM	3	//�ں˶��������һ��3����2���ź�����һ����Ϣ����						
						
//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  	//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);  //���ڳ�ʼ��
	LED_Init();         //LED��ʼ��	
	LCD_Init();			//LCD��ʼ��	
	KEY_Init();			//������ʼ��
	FSMC_SRAM_Init();	//��ʼ��SRAM
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 13-1");
	LCD_ShowString(30,50,200,16,16,"Pend Multi");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2015/5/20");
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,234,314);	
	LCD_DrawLine(5,130,234,130);
	POINT_COLOR = RED;
	LCD_ShowString(50,111,200,16,16,"ObjRdy_NUM: 0");
	POINT_COLOR = BLUE;
	
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
		
	OS_CRITICAL_ENTER();	//�����ٽ���
	//�����ź���Test_Sem1
	OSSemCreate ((OS_SEM*	)&Test_Sem1,
                 (CPU_CHAR*	)"Test_Sem1",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//�����ź���Test_Sem2
	OSSemCreate ((OS_SEM*	)&Test_Sem2,
                 (CPU_CHAR*	)"Test_Sem2",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//������Ϣ����
	OSQCreate ((OS_Q*		)&Test_Q,	//��Ϣ����
                (CPU_CHAR*	)"KEY Msg",	//��Ϣ��������
                (OS_MSG_QTY	)QUEUE_NUM,	//��Ϣ���г���
                (OS_ERR*	)&err);		//������
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
	//����MULTI��������
	OSTaskCreate((OS_TCB 	* )&Multi_TaskTCB,		
				 (CPU_CHAR	* )"Multi task", 		
                 (OS_TASK_PTR )multi_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MULTI_TASK_PRIO,     
                 (CPU_STK   * )&MULTI_TASK_STK[0],	
                 (CPU_STK_SIZE)MULTI_STK_SIZE/10,	
                 (CPU_STK_SIZE)MULTI_STK_SIZE,		
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
	u8 key;
	OS_ERR err;
	u8 num;
	u8 *pbuf;
	static u8 msg_num;
	pbuf=mymalloc(SRAMIN,10);	//�����ڴ�
	while(1)
	{
		key = KEY_Scan(0);  //ɨ�谴��
		switch(key)
		{
			case KEY1_PRES:
				OSSemPost(&Test_Sem1,OS_OPT_POST_1,&err);//�����ź���1
				break;
			case KEY0_PRES:
				OSSemPost(&Test_Sem2,OS_OPT_POST_1,&err);//�����ź���2
			case WKUP_PRES:
				msg_num++;
				sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		
				//������Ϣ
				OSQPost((OS_Q*		)&Test_Q,		
						(void*		)pbuf,
						(OS_MSG_SIZE)10,
						(OS_OPT		)OS_OPT_POST_FIFO,
						(OS_ERR*	)&err);
				break;
		}
		num++;
		if(num==50)
		{
			num=0;
			LED0=~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}

//�ȴ�����ں˶����������
void multi_task(void *p_arg)
{	
	u8 num;
	OS_ERR err;
	OS_OBJ_QTY index;
	OS_PEND_DATA pend_multi_tbl[CORE_OBJ_NUM];	
	
	pend_multi_tbl[0].PendObjPtr=(OS_PEND_OBJ*)&Test_Sem1;
	pend_multi_tbl[1].PendObjPtr=(OS_PEND_OBJ*)&Test_Sem2;
	pend_multi_tbl[2].PendObjPtr=(OS_PEND_OBJ*)&Test_Q;
	
	while(1)
	{
		index=OSPendMulti((OS_PEND_DATA*	)pend_multi_tbl,	
						  (OS_OBJ_QTY		)CORE_OBJ_NUM,	//�ں�����
						  (OS_TICK		   	)0,		
						  (OS_OPT         	)OS_OPT_PEND_BLOCKING,
						  (OS_ERR*			)&err);
		LCD_ShowNum(147,111,index,1,16);  					//��ʾ��ǰ�м����ں˶���׼������
		num++;
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);		//ˢ��
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //��ʱ1s
	}
}



	


