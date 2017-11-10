#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//RTC ��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  

//�ȴ�ͬ��
//����ֵ:0,�ɹ�;1,ʧ��
u8 RTC_Wait_Synchro(void)
{
	u8 value;
	if(RTC_WaitForSynchro() == SUCCESS) value = 0;
	else value = 1;
	return value;
}

//RTC�����ʼ��ģʽ
//����ֵ:0,�ɹ� 1,ʧ��
u8 RTC_Init_Mode(void)
{
	u8 value;
	if(RTC_EnterInitMode() == SUCCESS) value = 0;
	else value = 1;
	return value;
}

//RTCд�������SRAM
//BKPx:�����Ĵ������,���:0~19
//data:Ҫд�������,32λ����
void RTC_Write_BKP(u32 BKPx,u32 data)
{
	RTC_WriteBackupRegister(BKPx,data);
}

//RTC��ȡ������
//BKPx:�����Ĵ������,���0~19
//����ֵ:��ȡ��������
u32 RTC_Read_BKP(u32 BKPx)
{
	 u32 temp;
	 temp = RTC_ReadBackupRegister(BKPx);
	 return temp;
}

//ʮ����ת��ΪBCD��
//val:Ҫת����ʮ������
//����ֵ:BCD��
u8 RTC_DEC2BCD(u8 val)
{
	u8 bcdhigh = 0;
	while(val >= 10)
	{
		bcdhigh++;
		val -= 10;
	}
	return ((u8)(bcdhigh<<4)|val);
}

//BCD��ת��Ϊʮ��������
//val:Ҫת����BCD��
//����ֵ:ʮ��������
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp = 0;
	temp = (val>>4)*10;
	return (temp+(val&0x0f));
}

//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:AM/PM,0=AM/24H,1=PM
//����ֵ:0,�ɹ�
//			 1,�����ʼ��ģʽʧ��
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	u8 temp;
	u8 RTC_Time_Mode;
	RTC_TimeTypeDef RTC_TimeStructure;

	if(ampm == 0) RTC_Time_Mode = RTC_H12_AM;  //����
	else if (ampm == 1) RTC_Time_Mode = RTC_H12_PM; //����
		
	while(RTC_Wait_Synchro());  //�ȴ�ͬ�����
	RTC_TimeStructure.RTC_H12 = RTC_Time_Mode;
	RTC_TimeStructure.RTC_Hours = RTC_DEC2BCD(hour);
	RTC_TimeStructure.RTC_Minutes = RTC_DEC2BCD(min);
	RTC_TimeStructure.RTC_Seconds = RTC_DEC2BCD(sec);
	if(RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure) == SUCCESS) temp = 0;//����RTCʱ��
	else temp = 1; //ʱ������ʧ��
	return temp;
}

//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ��
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	u8 temp;
	RTC_DateTypeDef RTC_DateStructure;
	
	while(RTC_Wait_Synchro());  //�ȴ�ͬ�����
	
	RTC_DateStructure.RTC_Year = RTC_DEC2BCD(year);
	RTC_DateStructure.RTC_Month = RTC_DEC2BCD(month);
	RTC_DateStructure.RTC_Date = RTC_DEC2BCD(date);
	RTC_DateStructure.RTC_WeekDay = week;
	if(RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure) == SUCCESS) temp=0;
	else temp=1; //��������ʧ��
	return temp;
}

//��ȡRTCʱ��
//*hour,*min,*sec:Сʱ,����,���� 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;

	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure); //��ȡʱ��
	
	*hour = RTC_BCD2DEC(RTC_TimeStructure.RTC_Hours); //��ȡСʱ
	*min = RTC_BCD2DEC(RTC_TimeStructure.RTC_Minutes); //��ȡ����
	*sec = RTC_BCD2DEC(RTC_TimeStructure.RTC_Seconds); //��ȡ����
	*ampm = RTC_TimeStructure.RTC_H12;  //��ȡampm
}
//��ȡRTC����
//*year,*mon,*date:��,��,��
//*week:����
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
	*year = RTC_BCD2DEC(RTC_DateStructure.RTC_Year);
	*month = RTC_BCD2DEC(RTC_DateStructure.RTC_Month);
	*date = RTC_BCD2DEC(RTC_DateStructure.RTC_Date);
	*week = RTC_DateStructure.RTC_WeekDay;
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8  RTC_Timer_Init(void)
{
	u8 temp;
	RTC_InitTypeDef RTC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); //ʹ�ܵ�Դ�ӿ�ʱ��
	PWR_BackupAccessCmd(ENABLE); //ʹ��(RTC+SRAM)����
	
	if(RTC_Read_BKP(0) != 0X5050) //�Ƿ��ǵ�һ������
	{
		RCC_LSEConfig(RCC_LSE_ON); //����LSE
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //ѡ��LSE��ΪRTCʱ��
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); //�ȴ�LSE׼����
		
		RCC_RTCCLKCmd(ENABLE);  //ʹ��RTCʱ��
		while(RTC_Wait_Synchro());  //�ȴ�APB��RTCͬ��
		
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;  //24Сʱ��ʽ
		RTC_InitStructure.RTC_SynchPrediv = 0XFF;  //ͬ��Ԥ��Ƶ�� С��0X7FFF ����������ͬ��Ԥ��Ƶ��rtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC_InitStructure.RTC_AsynchPrediv = 0X7F; //�첽Ԥ��Ƶ�� С��
		if(RTC_Init(&RTC_InitStructure) == SUCCESS) temp = 0; //��ʼ���ɹ�
		else temp = 1; //��ʼ��ʧ��
		
		RTC_Set_Time(23,59,50,1); //����ʱ��
		RTC_Set_Date(14,7,1,1);   //��������
		
		RTC_Write_BKP(0,0X5050);  //����Ѿ���ʼ������
		return temp;
	}
	//RTC_Set_WakeUp(4,0); //����WAKE UP�ж�,1���ж�һ��
	return 0;
}

//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7)
//hour,min,sec:Сʱ,����,����
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	while(RTC_Wait_Synchro()); //�ȴ�APB��RTCͬ��
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//�ر�����A
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM; //AM
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = RTC_DEC2BCD(hour); //Сʱ
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_DEC2BCD(min); //����
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_DEC2BCD(sec); //��
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = week; //��
	
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None; //�������κ�����,����ǰʱ��Ҫ���������õ�����ֵ��Ȳ��ܴ�������
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay; //����������
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE); //��������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//��������A
	
	RTC_ClearITPendingBit(RTC_FLAG_ALRAF); //�������A�жϱ�־λ
	
	EXTI_ClearITPendingBit(EXTI_Line17);//����ж���17�ϵı�־
	EXTI_InitStructure.EXTI_Line = EXTI_Line17; //�ж���17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; //RTC�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); //�ж�����
}

//�����Ի��Ѷ�ʱ������
//wksel:000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
//      10x,ck_spre,1Hz;11x,1Hz/2^16.
//ע��:RTC����RTC��������ʱ��Ƶ��!
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{
	u32 temp;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	if(wksel == 0) temp = RTC_WakeUpClock_RTCCLK_Div16;
	else if(wksel == 1) temp = RTC_WakeUpClock_RTCCLK_Div8;
	else if(wksel == 2) temp = RTC_WakeUpClock_RTCCLK_Div4;
	else if(wksel == 3) temp = RTC_WakeUpClock_RTCCLK_Div2;
	else if(wksel >= 4) temp = RTC_WakeUpClock_CK_SPRE_16bits;
	else if(wksel >= 6) temp = RTC_WakeUpClock_CK_SPRE_17bits;
	
	while(RTC_Wait_Synchro()); //�ȴ�APB��RTCͬ��
	RTC_WakeUpCmd(DISABLE); //�ر�WAKEUP
	while(RTC_GetFlagStatus(RTC_FLAG_WUTWF) == 0);//�ȴ�WAKEUP��������
	RTC_WakeUpClockConfig(temp);//����wakeupƵ��
	RTC_SetWakeUpCounter(cnt);  //����WAKEUP�Զ���װֵ
	RTC_ClearFlag(RTC_FLAG_WUTF);  //���WAKEUP��־λ
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKEUP�ж�
	RTC_WakeUpCmd(ENABLE); //����WAKEUP
	
	EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22�ϵı�־
	EXTI_InitStructure.EXTI_Line = EXTI_Line22; //�ж���22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; //RTC�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); //�ж�����
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{
	LED1 = !LED1;
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET) //�ж�����A�ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA); //�������A�жϱ�־
		printf("ALARM A!\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line17);//����ж���17�ϵı�־
}


//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{
	LED1 = !LED1;
	if(RTC_GetITStatus(RTC_IT_WUT) == SET) //�ж�wake up�ж�
	{
		RTC_ClearITPendingBit(RTC_IT_WUT); //���wake up�жϱ�־
		printf("WAKE UP!\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22�ϵı�־
}











