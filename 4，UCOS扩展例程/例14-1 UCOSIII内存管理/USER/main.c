#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sram.h"
#include "beep.h"
#include "key.h"
#include "includes.h"
//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��14-1 UCOSIII �ڴ����

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
//������
void main_task(void *p_arg);

//�������ȼ�
#define MEMMANAGE_TASK_PRIO	5
//�����ջ��С
#define MEMMANAGE_STK_SIZE	128
//������ƿ�
OS_TCB	MemManage_TaskTCB;
//�����ջ
CPU_STK MEMMANAGE_TASK_STK[MEMMANAGE_STK_SIZE];
//������
void memmanage_task(void *p_arg);

//����һ���洢��
OS_MEM INTERNAL_MEM;	
//�洢���д洢������
#define INTERNAL_MEM_NUM		5
//ÿ���洢���С
//����һ��ָ�����ռ��4�ֽ����Կ�Ĵ�Сһ��ҪΪ4�ı���
//���ұ������һ��ָ�����(4�ֽ�)ռ�õĿռ�,����Ļ��洢�鴴�����ɹ�
#define INTERNAL_MEMBLOCK_SIZE	100  
//�洢�����ڴ�أ�ʹ���ڲ�RAM
CPU_INT08U Internal_RamMemp[INTERNAL_MEM_NUM][INTERNAL_MEMBLOCK_SIZE];

//����һ���洢��
OS_MEM EXTERNAL_MEM;	
//�洢���д洢������
#define EXTRENNAL_MEM_NUM		5
//ÿ���洢���С
//����һ��ָ�����ռ��4�ֽ����Կ�Ĵ�Сһ��ҪΪ4�ı���
//���ұ������һ��ָ�����(4�ֽ�)ռ�õĿռ�,����Ļ��洢�鴴�����ɹ�
#define EXTERNAL_MEMBLOCK_SIZE	100
//�洢�����ڴ�أ�ʹ���ⲿSRAM
__align(32) volatile CPU_INT08U External_RamMemp[EXTRENNAL_MEM_NUM][EXTERNAL_MEMBLOCK_SIZE]  __attribute__((at(0X68000000)));	

					
//����������
void ucos_load_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 14-1");
	LCD_ShowString(30,50,200,16,16,"Memory Manage");
	POINT_COLOR = BLUE;
	LCD_ShowString(30,70,200,16,16,"INTERNAL_MEM:");
	POINT_COLOR = RED;
	LCD_ShowString(30,90,200,16,16,"KEY_UP:malloc KEY_1:free");
	POINT_COLOR = BLUE;
	LCD_ShowString(30,110,200,16,16,"EXTERNAL_MEM:");
	POINT_COLOR = RED;
	LCD_ShowString(30,130,200,16,16,"KEY2:malloc KEY0:free");
	POINT_COLOR = BLACK;
	LCD_DrawLine(0,147,239,147);
	POINT_COLOR = BLUE;
	LCD_ShowString(5,148,200,16,16,"INTERNAL_MEM:");
	LCD_ShowString(5,228,200,16,16,"EXTERNAL_MEM:");
	POINT_COLOR = RED; 
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
	ucos_load_main_ui();//������UI1
	
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���	
	//����һ���洢����
	OSMemCreate((OS_MEM*	)&INTERNAL_MEM,
				(CPU_CHAR*	)"Internal Mem",
				(void*		)&Internal_RamMemp[0][0],
				(OS_MEM_QTY	)INTERNAL_MEM_NUM,
				(OS_MEM_SIZE)INTERNAL_MEMBLOCK_SIZE,
				(OS_ERR*	)&err);
	//����һ���洢����
	OSMemCreate((OS_MEM*	)&EXTERNAL_MEM,
				(CPU_CHAR*	)"External Mem",
				(void*		)&External_RamMemp[0][0],
				(OS_MEM_QTY	)EXTRENNAL_MEM_NUM,
				(OS_MEM_SIZE)EXTERNAL_MEMBLOCK_SIZE,
				(OS_ERR*	)&err);
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
	//����һ���ڴ��������
	OSTaskCreate((OS_TCB*     )&MemManage_TaskTCB,		
				 (CPU_CHAR*   )"MemManage task", 		
                 (OS_TASK_PTR )memmanage_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MEMMANAGE_TASK_PRIO,     
                 (CPU_STK*    )&MEMMANAGE_TASK_STK[0],	
                 (CPU_STK_SIZE)MEMMANAGE_STK_SIZE/10,	
                 (CPU_STK_SIZE)MEMMANAGE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//�������������
void main_task(void *p_arg)
{
	u8 key,num;
	static u8 internal_memget_num;
	static u8 external_memget_num;
	CPU_INT08U *internal_buf;
	CPU_INT08U *external_buf;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);  //ɨ�谴��
		switch(key)
		{
			case WKUP_PRES:		//����KEY_UP��
				internal_buf=OSMemGet((OS_MEM*)&INTERNAL_MEM,
								      (OS_ERR*)&err);
				if(err == OS_ERR_NONE) //�ڴ�����ɹ�
				{
					printf("internal_buf�ڴ�����֮��ĵ�ַΪ:%#x\r\n",(u32)(internal_buf));
					LCD_ShowString(30,180,200,16,16,"Memory Get success!  ");
					internal_memget_num++;
					POINT_COLOR = BLUE;
					sprintf((char*)internal_buf,"INTERNAL_MEM Use %d times",internal_memget_num);
					LCD_ShowString(30,196,200,16,16,internal_buf); 
					POINT_COLOR = RED;
				}
				if(err == OS_ERR_MEM_NO_FREE_BLKS) //�ڴ�鲻��
				{
					LCD_ShowString(30,180,200,16,16,"INTERNAL_MEM Empty!  ");
				}
				break;
			case KEY1_PRES:
				if(internal_buf != NULL) //internal_buf��Ϊ�վ��ͷ��ڴ�
				{
					OSMemPut((OS_MEM*	)&INTERNAL_MEM,		//�ͷ��ڴ�
							 (void*		)internal_buf,
							 (OS_ERR* 	)&err);
					printf("internal_buf�ڴ��ͷ�֮��ĵ�ַΪ:%#x\r\n",(u32)(internal_buf));
					LCD_ShowString(30,180,200,16,16,"Memory Put success!   ");
				}
				break;
			case KEY2_PRES:
				external_buf=OSMemGet((OS_MEM*)&EXTERNAL_MEM,
									  (OS_ERR*)&err);
				if(err == OS_ERR_NONE) //�ڴ�����ɹ�
				{
					printf("external_buf�ڴ�����֮��ĵ�ַΪ:%#x\r\n",(u32)(external_buf));
					LCD_ShowString(30,260,200,16,16,"Memory Get success!  ");
					external_memget_num++;
					POINT_COLOR = BLUE;
					sprintf((char*)external_buf,"EXTERNAL_MEM Use %d times",external_memget_num);
					LCD_ShowString(30,276,200,16,16,external_buf); 
					POINT_COLOR = RED;
				}
				if(err == OS_ERR_MEM_NO_FREE_BLKS) //�ڴ�鲻��
				{
					LCD_ShowString(30,260,200,16,16,"EXTERNAL_MEM Empty!  ");
				}
				break;
			case KEY0_PRES:
				if(external_buf != NULL) //external_buf��Ϊ�վ��ͷ��ڴ�
				{
					OSMemPut((OS_MEM*	)&EXTERNAL_MEM,		//�ͷ��ڴ�
							 (void*		)external_buf,
							 (OS_ERR* 	)&err);
					printf("external_buf�ڴ��ͷ�֮��ĵ�ַΪ:%#x\r\n",(u32)(external_buf));
					LCD_ShowString(30,260,200,16,16,"Memory Put success!   ");
				}
				break;
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

//�ڴ��������
void memmanage_task(void *p_arg)
{
	OS_ERR err;
	LCD_ShowString(5,164,200,16,16,"Total:  Remain:");
	LCD_ShowString(5,244,200,16,16,"Total:  Remain:");
	while(1)
	{
		POINT_COLOR = BLUE;
		LCD_ShowxNum(53,164,INTERNAL_MEM.NbrMax,1,16,0);	
		LCD_ShowxNum(125,164,INTERNAL_MEM.NbrFree,1,16,0);	
		LCD_ShowxNum(53,244,EXTERNAL_MEM.NbrMax,1,16,0);	
		LCD_ShowxNum(125,244,EXTERNAL_MEM.NbrFree,1,16,0);	
		POINT_COLOR = RED;
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ100ms
	}
}






	


