#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RTC 驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  

//等待同步
//返回值:0,成功;1,失败
u8 RTC_Wait_Synchro(void)
{
	u8 value;
	if(RTC_WaitForSynchro() == SUCCESS) value = 0;
	else value = 1;
	return value;
}

//RTC进入初始化模式
//返回值:0,成功 1,失败
u8 RTC_Init_Mode(void)
{
	u8 value;
	if(RTC_EnterInitMode() == SUCCESS) value = 0;
	else value = 1;
	return value;
}

//RTC写入后备区域SRAM
//BKPx:后备区寄存器编号,编号:0~19
//data:要写入的数据,32位长度
void RTC_Write_BKP(u32 BKPx,u32 data)
{
	RTC_WriteBackupRegister(BKPx,data);
}

//RTC读取后备区域
//BKPx:后备区寄存器编号,编号0~19
//返回值:读取到的数据
u32 RTC_Read_BKP(u32 BKPx)
{
	 u32 temp;
	 temp = RTC_ReadBackupRegister(BKPx);
	 return temp;
}

//十进制转换为BCD码
//val:要转换的十进制数
//返回值:BCD码
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

//BCD码转换为十进制数据
//val:要转换的BCD码
//返回值:十进制数据
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp = 0;
	temp = (val>>4)*10;
	return (temp+(val&0x0f));
}

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:AM/PM,0=AM/24H,1=PM
//返回值:0,成功
//			 1,进入初始化模式失败
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	u8 temp;
	u8 RTC_Time_Mode;
	RTC_TimeTypeDef RTC_TimeStructure;

	if(ampm == 0) RTC_Time_Mode = RTC_H12_AM;  //上午
	else if (ampm == 1) RTC_Time_Mode = RTC_H12_PM; //下午
		
	while(RTC_Wait_Synchro());  //等待同步完成
	RTC_TimeStructure.RTC_H12 = RTC_Time_Mode;
	RTC_TimeStructure.RTC_Hours = RTC_DEC2BCD(hour);
	RTC_TimeStructure.RTC_Minutes = RTC_DEC2BCD(min);
	RTC_TimeStructure.RTC_Seconds = RTC_DEC2BCD(sec);
	if(RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure) == SUCCESS) temp = 0;//设置RTC时间
	else temp = 1; //时间设置失败
	return temp;
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:0,成功
//       1,进入初始化模式失败
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	u8 temp;
	RTC_DateTypeDef RTC_DateStructure;
	
	while(RTC_Wait_Synchro());  //等待同步完成
	
	RTC_DateStructure.RTC_Year = RTC_DEC2BCD(year);
	RTC_DateStructure.RTC_Month = RTC_DEC2BCD(month);
	RTC_DateStructure.RTC_Date = RTC_DEC2BCD(date);
	RTC_DateStructure.RTC_WeekDay = week;
	if(RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure) == SUCCESS) temp=0;
	else temp=1; //日期设置失败
	return temp;
}

//获取RTC时间
//*hour,*min,*sec:小时,分钟,秒钟 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;

	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure); //读取时间
	
	*hour = RTC_BCD2DEC(RTC_TimeStructure.RTC_Hours); //获取小时
	*min = RTC_BCD2DEC(RTC_TimeStructure.RTC_Minutes); //获取分钟
	*sec = RTC_BCD2DEC(RTC_TimeStructure.RTC_Seconds); //获取秒钟
	*ampm = RTC_TimeStructure.RTC_H12;  //获取ampm
}
//获取RTC日期
//*year,*mon,*date:年,月,日
//*week:星期
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
	*year = RTC_BCD2DEC(RTC_DateStructure.RTC_Year);
	*month = RTC_BCD2DEC(RTC_DateStructure.RTC_Month);
	*date = RTC_BCD2DEC(RTC_DateStructure.RTC_Date);
	*week = RTC_DateStructure.RTC_WeekDay;
}

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8  RTC_Timer_Init(void)
{
	u8 temp;
	RTC_InitTypeDef RTC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); //使能电源接口时钟
	PWR_BackupAccessCmd(ENABLE); //使能(RTC+SRAM)区域
	
	if(RTC_Read_BKP(0) != 0X5050) //是否是第一次配置
	{
		RCC_LSEConfig(RCC_LSE_ON); //开启LSE
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //选择LSE作为RTC时钟
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); //等待LSE准备好
		
		RCC_RTCCLKCmd(ENABLE);  //使能RTC时钟
		while(RTC_Wait_Synchro());  //等待APB与RTC同步
		
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;  //24小时格式
		RTC_InitStructure.RTC_SynchPrediv = 0XFF;  //同步预分频器 小于0X7FFF 必须先设置同步预分频器rtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC_InitStructure.RTC_AsynchPrediv = 0X7F; //异步预分频器 小于
		if(RTC_Init(&RTC_InitStructure) == SUCCESS) temp = 0; //初始化成功
		else temp = 1; //初始化失败
		
		RTC_Set_Time(23,59,50,1); //设置时间
		RTC_Set_Date(14,7,1,1);   //设置日期
		
		RTC_Write_BKP(0,0X5050);  //标记已经初始化过了
		return temp;
	}
	//RTC_Set_WakeUp(4,0); //配置WAKE UP中断,1秒中断一次
	return 0;
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7)
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	while(RTC_Wait_Synchro()); //等待APB与RTC同步
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM; //AM
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = RTC_DEC2BCD(hour); //小时
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_DEC2BCD(min); //分钟
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_DEC2BCD(sec); //秒
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = week; //周
	
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None; //不设置任何掩码,即当前时钟要和闹钟设置的所有值相等才能触发闹钟
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay; //按星期闹钟
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE); //开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟A
	
	RTC_ClearITPendingBit(RTC_FLAG_ALRAF); //清除闹钟A中断标志位
	
	EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线17上的标志
	EXTI_InitStructure.EXTI_Line = EXTI_Line17; //中断线17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; //RTC闹钟中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); //中断配置
}

//周期性唤醒定时器设置
//wksel:000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
//      10x,ck_spre,1Hz;11x,1Hz/2^16.
//注意:RTC就是RTC计数器的时钟频率!
//cnt:自动重装载值.减到0,产生中断.
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
	
	while(RTC_Wait_Synchro()); //等待APB与RTC同步
	RTC_WakeUpCmd(DISABLE); //关闭WAKEUP
	while(RTC_GetFlagStatus(RTC_FLAG_WUTWF) == 0);//等待WAKEUP允许被更改
	RTC_WakeUpClockConfig(temp);//设置wakeup频率
	RTC_SetWakeUpCounter(cnt);  //设置WAKEUP自动重装值
	RTC_ClearFlag(RTC_FLAG_WUTF);  //清除WAKEUP标志位
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKEUP中断
	RTC_WakeUpCmd(ENABLE); //开启WAKEUP
	
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22上的标志
	EXTI_InitStructure.EXTI_Line = EXTI_Line22; //中断线22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; //RTC闹钟中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); //中断配置
}

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	LED1 = !LED1;
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET) //判断闹钟A中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA); //清除闹钟A中断标志
		printf("ALARM A!\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线17上的标志
}


//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	LED1 = !LED1;
	if(RTC_GetITStatus(RTC_IT_WUT) == SET) //判断wake up中断
	{
		RTC_ClearITPendingBit(RTC_IT_WUT); //清除wake up中断标志
		printf("WAKE UP!\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22上的标志
}











