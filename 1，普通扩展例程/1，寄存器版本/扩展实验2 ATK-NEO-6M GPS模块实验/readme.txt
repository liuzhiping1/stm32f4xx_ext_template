实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习ATK-NEO-6M GPS模块的使用，实现GPS定位
	
硬件资源:
	1,DS0(连接在PF9) 
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS) 
	4,按键KEY0(PE4)
	5,串口3(波特率:38400,PB10/PB11连接ATK-NEO-6M GPS模块)
	6,ATK MODULE接口&ATK-NEO-6M GPS模块(模块直接插ATK MODULE接口即可),连接关系如下: 
	STM32开发板-->ATK-NEO-6M GPS模块
			PF6-->PPS
		   PB10-->RXD
		   PB11-->TXD 
			GND-->GND
			 5V-->VCC
	
实验现象: 
	通过串口2连接ATK-NEO-6M GPS模块，然后通过液晶显示GPS信息，包括精度、纬度、高度、速度、
	用于定位的卫星数、可见卫星数、UTC时间等信息。同时，可以通过USMART工具，设置GPS模块的刷
	新速率（最大支持5Hz刷新）和时钟脉冲的配置。另外，通过KEY0按键，可以开启或关闭NMEA数据的
	上传（即输出到串口1，方便开发调试）。
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	3,本例程不支持ALIENTEK CPLD方案的7寸电容触摸屏（V1版本）的测试!!
	4,ATK-NEO-6M模块的波特率默认设置为38400(对应开发板的串口3)。
	5,务必短接探索者STM32F4开发板P10的USART3_RX和GBC_TX以及USART3_TX和GBC_RX
	6,开发板的串口1（USB_232转出来的串口）的波特率是115200。
	7,GPS模块必须放置于室外或者窗户旁边，否则有可能收不到GPS信号。
	8,如有其他问题，请参考<ATK-NEO-6M GPS模块问题汇总.pdf>。
	9,本例程详细说明，见光盘：9，增值资料\1，ALIENTEK 产品资料\2，ATK-NEO-6M GPS模块\ATK-NEO-6M GPS模块使用说明_AN1409A.pdf
 
 
					正点原子@ALIENTEK
					2014-10-27
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com