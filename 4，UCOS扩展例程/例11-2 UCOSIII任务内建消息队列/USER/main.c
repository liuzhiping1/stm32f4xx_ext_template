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
//��11-2 UCOSIII �����ڽ���Ϣ����

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
#define MSGDIS_TASK_PRIO	6
//�����ջ
#define MSGDIS_STK_SIZE		128
//������ƿ�
OS_TCB	Msgdis_TaskTCB;
//�����ջ
CPU_STK	MSGDIS_TASK_STK[MSGDIS_STK_SIZE];
//������
void msgdis_task(void *p_arg);

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

#define TASK_Q_NUM	4		//�������ڽ���Ϣ���еĳ���
						
////////////////////////��ʱ��////////////////////////////////
u8 tmr1sta=0; 	//��Ƕ�ʱ���Ĺ���״̬
OS_TMR	tmr1;	//����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����
						
//����������
void ucos_load_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(10,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(10,30,200,16,16,"UCOSIII Examp 11-2");
	LCD_ShowString(10,50,200,16,16,"Task Queue");
	LCD_ShowString(10,70,220,16,16,"KEY_UP:Tmr1");
	LCD_ShowString(10,90,200,16,16,"2015/5/20");
	
	POINT_COLOR = BLACK;
	LCD_DrawLine(0,107,239,107);		//����
	POINT_COLOR = RED;
	LCD_ShowString(30,130,100,16,16,"tmr1 state:");
	LCD_ShowString(30,170,120,16,16,"Task_QMsg Size:");
	LCD_ShowString(30,210,120,16,16,"Task_QMsg rema:");
	LCD_ShowString(30,250,100,16,16,"Task_QMsg:");
	POINT_COLOR = BLUE;
	LCD_ShowString(40,150,100,16,16,"TMR1 STOP! ");
}

//��ѯDATA_Msg��Ϣ�����е��ܶ���������ʣ���������
void check_msg_queue(u8 *p)
{
	CPU_SR_ALLOC();
	u8 msgq_remain_size;	//��Ϣ����ʣ���С
	OS_CRITICAL_ENTER();	//�����ٽ��
	msgq_remain_size =Msgdis_TaskTCB.MsgQ.NbrEntriesSize-Msgdis_TaskTCB.MsgQ.NbrEntries;
	p = mymalloc(SRAMIN,20);	//�����ڴ�
	sprintf((char*)p,"Total Size:%d",Msgdis_TaskTCB.MsgQ.NbrEntriesSize);	//��ʾDATA_Msg��Ϣ�����ܵĴ�С
	LCD_ShowString(40,190,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//��ʾDATA_Msgʣ���С
	LCD_ShowString(40,230,100,16,16,p);
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	OS_CRITICAL_EXIT();		//�˳��ٽ��
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
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	//����������
	OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK   * )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);					
	//����MSGDIS����
	OSTaskCreate((OS_TCB 	* )&Msgdis_TaskTCB,		
				 (CPU_CHAR	* )"Msgdis task", 		
                 (OS_TASK_PTR )msgdis_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MSGDIS_TASK_PRIO,     
                 (CPU_STK   * )&MSGDIS_TASK_STK[0],	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE/10,	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM,		//����Msgdis_task��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ4					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf = mymalloc(SRAMIN,10);	//����10���ֽ�
	if(pbuf)	//�����ڴ�ɹ�
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//������Ϣ
		OSTaskQPost((OS_TCB*	)&Msgdis_TaskTCB,	//������Msgdis������Ϣ
                    (void*		)pbuf,
                    (OS_MSG_SIZE)10,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			myfree(SRAMIN,pbuf);	//�ͷ��ڴ�
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
			tmr1sta = !tmr1sta;
			LCD_ShowString(40,150,100,16,16,"TMR1 STOP! ");
		}
	}	
}

//�������������
void main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	u8 *p;
	while(1)
	{
		key = KEY_Scan(0);  //ɨ�谴��
		if(key==WKUP_PRES)
		{
			tmr1sta = !tmr1sta;
			if(tmr1sta) 
			{
				OSTmrStart(&tmr1,&err);
				LCD_ShowString(40,150,100,16,16,"TMR1 START!");
			}
			else
			{
				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
				LCD_ShowString(40,150,100,16,16,"TMR1 STOP! ");
			}
		}
		num++;
		if(num%10==0) check_msg_queue(p);//���DATA_Msg��Ϣ���е�����
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}

//��ʾ��Ϣ�����е���Ϣ
void msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err; 
	while(1)
	{
		//������Ϣ
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
		LCD_ShowString(40,270,100,16,16,p);
		myfree(SRAMIN,p);	//�ͷ��ڴ�
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}





	


