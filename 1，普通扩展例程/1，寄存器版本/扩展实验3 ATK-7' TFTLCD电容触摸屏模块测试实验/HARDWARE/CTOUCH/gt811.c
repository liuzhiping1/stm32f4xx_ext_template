#include "gt811.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//7寸电容触摸屏驱动-GT811部分	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2013/3/17
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////	 

//电容触摸屏控制器
_m_ctp_dev ctp_dev=
{
	GT811_Init,
	GT811_Scan,
	0,
	0,
	0,
	0,
};	  																				   
//触摸屏配置参数(触摸屏厂家提供)
const u8 GTP_CFG_DATA[]=
{
    0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0x05,0x55,0x15,0x55,0x25,0x55,
    0x35,0x55,0x45,0x55,0x55,0x55,0x65,0x55,0x75,0x55,0x85,0x55,0x95,0x55,0xA5,0x55,
    0xB5,0x55,0xC5,0x55,0xD5,0x55,0xE5,0x55,0xF5,0x55,0x1B,0x03,0x00,0x00,0x00,0x13,
    0x13,0x13,0x0F,0x0F,0x0A,0x50,0x30,0x05,0x03,0x64,0x05,0xe0,0x01,0x20,0x03,0x00,
    0x00,0x32,0x2C,0x34,0x2E,0x00,0x00,0x04,0x14,0x22,0x04,0x00,0x00,0x00,0x00,0x00,
    0x20,0x14,0xEC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x30,
    0x25,0x28,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x01, 
};	 

//触摸屏中断处理
void EXTI1_IRQHandler(void)
{
 	if(CT_INT==0)	 //有触摸中断
	{				  
		ctp_dev.tpsta|=0X80;//标记有有效触摸
	}		 
	EXTI->PR=1<<1;  //清除LINE1上的中断标志位  	   
} 
//向GT811写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 GT811_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	
 	CT_IIC_Send_Byte(CT_CMD_WR);   	//发送写命令 	 
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//发数据
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//产生一个停止条件	    
	return ret; 
}
//从GT811读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void GT811_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(CT_CMD_WR);   	//发送写命令 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(CT_CMD_RD);    //发送读命令		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    CT_IIC_Stop();//产生一个停止条件	    
}
//向GT811发送配置参数
//buf:配置参数表指针
//cfg_len:配置参数长度
//返回值:0,成功;1,失败.
u8 GT811_Send_Cfg(u8 * buf,u16 cfg_len)
{
	u8 ret=0;
	u8 retry=0;
	for(retry=0;retry<5;retry++)
	{
		ret=GT811_WR_Reg(CT_CONFIG_REG,buf,cfg_len);
		if(ret==0)break;
		delay_ms(10);	 
	}
	return ret;
}
//GT811唤醒
void GT811_Wakeup_Sleep(void)
{
	CT_RST=0;	//复位
	delay_ms(10);
 	CT_RST=1;	//释放复位		    
	delay_ms(100);
}	    
//GT811初始化
//返回值:初始化结果
//0,初始化成功;
//1,发送配置参数错误
//2,版本错误
u8 GT811_Init(void)
{
 	u16 version=0;
	u8 temp;
	RCC->AHB1ENR|=1<<1;    	//使能PORTB时钟 
	RCC->AHB1ENR|=1<<2;    	//使能PORTC时钟  
	GPIO_Set(GPIOB,PIN1,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//PB1设置为上拉输入
	GPIO_Set(GPIOC,PIN13,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PC13设置为推挽输出
  	Ex_NVIC_Config(GPIO_B,1,FTIR); 		//下降沿触发		   
	MY_NVIC_Init(3,3,EXTI1_IRQn,2);		//抢占3，子优先级3，组2	  									   								
 	CT_IIC_Init();						//电容触摸屏部分,IIC初始化
	GT811_Wakeup_Sleep(); 				//唤醒GT811 
  	GT811_RD_Reg(0X717,&temp,1);		//读取版本高八位
	version=(u16)temp<<8; 
	GT811_RD_Reg(0X718,&temp,1);		//读取版本低八位  
	version|=temp;
	printf("version:%x\r\n",version); 
	if(version==0X2010)					//版本正确,发送配置参数
	{ 
		temp=GT811_Send_Cfg((u8*)GTP_CFG_DATA,sizeof(GTP_CFG_DATA));//发送配置参数
	}else temp=2;	//版本错误
	return temp;   
}	   
//扫描GT811
//得到的结果保存在ctp_dev结构体内								   									 
void GT811_Scan(void)
{			   
	u8 buf[34];//一次读取34字节
	if((ctp_dev.tpsta&0X80)==0)return;			//有有效触摸,则读取数据,否则直接退出
  	GT811_RD_Reg(CT_READ_XY_REG,buf,34);		//一次读取34个字节
	ctp_dev.tpsta=buf[0]&0X1F;					//触摸点标记位,同时清除有效触摸标记
#if CT_EXCHG_XY==1								//调转XY
	ctp_dev.y[0]=480-(((u16)buf[2]<<8)+buf[3]);	//触摸点0坐标
	ctp_dev.x[0]=((u16)buf[4]<<8)+buf[5];	  
	ctp_dev.ppr[0]=buf[6];	 
	ctp_dev.y[1]=480-(((u16)buf[7]<<8)+buf[8]);	//触摸点1坐标
	ctp_dev.x[1]=((u16)buf[9]<<8)+buf[10];	  
	ctp_dev.ppr[1]=buf[11];
	ctp_dev.y[2]=480-(((u16)buf[12]<<8)+buf[13]);//触摸点2坐标
	ctp_dev.x[2]=((u16)buf[14]<<8)+buf[15];	  
	ctp_dev.ppr[2]=buf[16];					   
	ctp_dev.y[3]=480-(((u16)buf[17]<<8)+buf[24]);//触摸点3坐标
	ctp_dev.x[3]=((u16)buf[25]<<8)+buf[26];	  
	ctp_dev.ppr[3]=buf[27];
	ctp_dev.y[4]=480-(((u16)buf[28]<<8)+buf[29]);//触摸点4坐标
	ctp_dev.x[4]=((u16)buf[30]<<8)+buf[31];	  
	ctp_dev.ppr[4]=buf[32];
#else 
	ctp_dev.y[0]=((u16)buf[2]<<8)+buf[3];		//触摸点0坐标
	ctp_dev.x[0]=800-(((u16)buf[4]<<8)+buf[5]);	  
	ctp_dev.ppr[0]=buf[6];	 
	ctp_dev.y[1]=((u16)buf[7]<<8)+buf[8];		//触摸点1坐标
	ctp_dev.x[1]=800-(((u16)buf[9]<<8)+buf[10]);	  
	ctp_dev.ppr[1]=buf[11];
	ctp_dev.y[2]=((u16)buf[12]<<8)+buf[13];		//触摸点2坐标
	ctp_dev.x[2]=800-(((u16)buf[14]<<8)+buf[15]);	  
	ctp_dev.ppr[2]=buf[16];					   
	ctp_dev.y[3]=((u16)buf[17]<<8)+buf[24];		//触摸点3坐标
	ctp_dev.x[3]=800-(((u16)buf[25]<<8)+buf[26]);	  
	ctp_dev.ppr[3]=buf[27];
	ctp_dev.y[4]=((u16)buf[28]<<8)+buf[29];		//触摸点4坐标
	ctp_dev.x[4]=800-(((u16)buf[30]<<8)+buf[31]);	  
	ctp_dev.ppr[4]=buf[32];
#endif	 		    
}




















