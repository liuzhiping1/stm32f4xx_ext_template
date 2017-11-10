#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	 
#include "string.h" 	 
#include "timer.h" 	 
#include "gt811.h" 	


//ALIENTEK̽����STM32F407������ ��չʵ��3
//ATK-7' TFTLCD ���ݴ�����ģ�����ʵ��  -�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//01�����ݴ��������Բ���   

//���ݴ�������gui
void ctouch_paint_gui(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,lcddev.width,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
}
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
//5�����ص����ɫ												 
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};  
//01,���ݴ���������
//���Ե��ݴ����������֧��5�㴥�ء�
void ctouch_paint_test(void)
{		
 	u8 i=0,t=0;	
	u16 lastpos[5][2];		//���һ�ε�����   
	u8 	ctout[5];			//5����������ͷż�ʱ��   	 		 	
 	LCD_Clear(WHITE);
	POINT_COLOR=RED;		//��������Ϊ��ɫ
	Show_Str(60,50,lcddev.width,16,"����1�����ݴ���������",16,0); 
	Show_Str(60,70,lcddev.width,16,"���ͬʱ����������5��",16,0);    
	Show_Str(60,90,lcddev.width,16,"������������Ͻǵĵġ�RST���������������Ļ",16,0);    
  	while(ctp_dev.init())		   	//��ʼ�����ݴ�����
	{
		Show_Str(60,110,lcddev.width,16,"���ݴ�������ʼ��ʧ�ܣ�",16,0); 
		delay_ms(200);
		Show_Str(60,110,lcddev.width,16,"      ���飡����    ",16,0); 
 		delay_ms(200);
	};
 	Show_Str(60,110,lcddev.width,16,"���ݴ�������ʼ���ɹ���",16,0); 
	delay_ms(1500);
	delay_ms(1500);
 	ctouch_paint_gui();	 
 	for(i=0;i<5;i++)
	{
		lastpos[i][0]=0XFFFF;	//ȫ������Ϊ�Ƿ�ֵ
		lastpos[i][1]=0XFFFF;
		ctout[i]=0;	   			//��ʱ������
	}
	while(1)
	{
 		ctp_dev.scan();	     
		if(ctp_dev.tpsta&0X1F)		//������������
		{	   	
			for(t=0;t<5;t++)
			{
				if(ctp_dev.tpsta&(1<<t))
				{
				 	if(ctp_dev.x[t]<lcddev.width&&ctp_dev.y[t]<lcddev.height)
					{	
						if(ctp_dev.x[t]>(lcddev.width-24)&&ctp_dev.y[t]<16)ctouch_paint_gui();//���
						else 
						{
							if(lastpos[t][0]==0XFFFF)//���ڵ�һ�ΰ���
							{
								lastpos[t][0]=ctp_dev.x[t];
								lastpos[t][1]=ctp_dev.y[t];	   
							}
	 						lcd_draw_bline(lastpos[t][0],lastpos[t][1],ctp_dev.x[t],ctp_dev.y[t],ctp_dev.ppr[t]/3+2,POINT_COLOR_TBL[t]);//����
							lastpos[t][0]=ctp_dev.x[t];
							lastpos[t][1]=ctp_dev.y[t];	   
						}													  			   
					}
					ctout[t]=0;	
				}
			} 
			ctp_dev.tpsta=0;	 
		}else
		{						  		   
			delay_ms(5);	//û�а������µ�ʱ��
			for(t=0;t<5;t++)
			{
				ctout[t]++;	
				if(ctout[t]>10)//�ж��˵����ɿ�
				{
					lastpos[t][0]=0XFFFF;	 
					ctout[t]=0;
				}
			}  	 
 		}
		i++;
		if(i==20)
		{
			i=0;
			LED0=!LED0;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//02,ͼƬ��ʾ���Բ���

//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	return rval;
}   
//02,ͼƬ��ʾ����
//ѭ����ʾSD����PICTURE�ļ��������ͼƬ�ļ���
void picture_display_test(void)
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO picfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u16 *picindextbl;	//ͼƬ������
	LCD_Clear(WHITE);
	Show_Str(60,50,lcddev.width,16,"����2��ͼƬ��ʾ����",16,0); 
	Show_Str(60,70,lcddev.width,16,"KEY0:��һ��ͼƬ",16,0); 
	Show_Str(60,90,lcddev.width,16,"KEY1:��һ��ͼƬ",16,0); 
	Show_Str(60,110,lcddev.width,16,"KEY_UP:��ͣ/���� �Զ�����",16,0); 
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(60,130,lcddev.width,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(60,130,lcddev.width,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}	  
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
 	{	    
		Show_Str(60,130,lcddev.width,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,130,lcddev.width,130+16,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.index;								//��¼��ǰindex
	        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	delay_ms(1200);
	Show_Str(60,130,lcddev.width,16,"��ʼ��ʾ...",16,0); 
	delay_ms(1800);  
	piclib_init();										//��ʼ����ͼ	   	   
  	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
  	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں��� 
		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);				//ɨ�谴��
			if(t>250&&(pause==0))key=KEY0_PRES;//����ͣ״̬,ÿ2.5�룬ģ��һ�ΰ���KEY0  
			if(key==KEY1_PRES)				//��һ��
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)		//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1=!pause; 	//��ͣ��ʱ��LED1��.  
			}	  
			t++;
			if((t%20)==0)LED0=!LED0;   	   
			delay_ms(10); 
		}					    
		res=0;  
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//03,Һ�����Բ���

//�õ�һ�������
//(min,max)�������������Χ
//����ֵ:���������������ֵ
u16 speed_test_get_rval(u16 min,u16 max)
{
	u16 t=0Xffff;
	while((t<min)||(t>max))t=rand();	  
	return t;
}												 
const u16 SPEED_COLOR_TBL[10]={RED,GREEN,BLUE,BROWN,GRED,BRRED,CYAN,YELLOW,GRAY,MAGENTA};
//�õ��ٶȲ���һ�����������Ϣ
//*x,*y,*width,*height,*color:��ȡ�����������/�ߴ�/��ɫ����Ϣ 
void speed_test_get_fill_parameter(u16 *x,u16 *y,u16 *width,u16 *height,u16 *color)
{
	*x=speed_test_get_rval(0,700);
	*y=speed_test_get_rval(0,380);
	*width=speed_test_get_rval(80,800-*x);
	*height=speed_test_get_rval(80,480-*y);
	*color=SPEED_COLOR_TBL[speed_test_get_rval(0,9)]; 
}   
u16 speed_run_time;	 	//�ٶȲ��Բ���ʱ�䳤��(��λΪ10ms)
//3-1�ٶȲ���
void tftlcd_speed_test(void)
{
 	u16 x,y,width,height,color;
	u32 pixelcnt=0;
	u8 *str;
	LCD_Clear(WHITE);
 	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"����3-1���ٶȲ���",16,0); 
	Show_Str(60,70,lcddev.width,16,"ͨ����LCD����λ���������䲻ͬ�ߴ�ĵ�ɫ����,�������ٶ�",16,0); 
	Show_Str(60,90,lcddev.width,16,"����ʱ��ԼΪ5��,���Խ������ʾ��LCD��",16,0); 
 	delay_ms(1500);delay_ms(1500);
 	LCD_Clear(RED);
	str=mymalloc(SRAMIN,60);	//����60���ֽڵ��ڴ�
	srand(TIM3->CNT);
	TIM3->CR1&=~(1<<0);    		//�رն�ʱ��3
	TIM3->CNT=0;				//����
	speed_run_time=0;			//��ʱ������
	while(1)
	{		
		speed_test_get_fill_parameter(&x,&y,&width,&height,&color);//�õ�������
		pixelcnt+=width*height;
		TIM3->CR1|=1<<0;  		//������ʱ��3
		LCD_Fill(x,y,x+width-1,y+height-1,color);
		TIM3->CR1&=~(1<<0);		//�رն�ʱ��3
		if(speed_run_time>300)break;//����3����
	}
	sprintf((char*)str,"LCD�ٶȲ��Խ��:%d ����/��",(pixelcnt*100)/speed_run_time);
 	Show_Str(270,230,lcddev.width,16,str,16,0); 				 
	myfree(SRAMIN,str);		//�ͷ��ڴ�
	delay_ms(1800);delay_ms(1800);delay_ms(1400); 
}
//3-2�������
void tftlcd_mirror_test(void)
{  					     
	LCD_Clear(WHITE);
	Show_Str(60,50,lcddev.width,16,"����3-2���������",16,0); 
	Show_Str(60,70,lcddev.width,16,"���Բ���:Ĭ����ʾ/���µ�ת/���ҵ�ת/�������Ҷ���ת",16,0); 
  	delay_ms(1500);delay_ms(1500);
 	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_DrawRectangle(30,30,180,130);
	LCD_Fill(lcddev.width-30-150,lcddev.height-30-100,lcddev.width-30,lcddev.height-30,BLUE);
	Draw_Circle(90,lcddev.height-90,60);
	gui_fill_circle(lcddev.width-90,90,60,GRED);
 	POINT_COLOR=BLUE;
	Show_Str(140,150,lcddev.width,16,"ATK-7' TFTLCDģ�龵���ܲ���",16,0);     
	LCD_Scan_Dir(L2R_U2D);//Ĭ��		 
   	delay_ms(1200);delay_ms(1300);		
 	LCD_Scan_Dir(L2R_D2U);//���µ�ת		 									    
   	delay_ms(1200);delay_ms(1300);		
  	LCD_Scan_Dir(R2L_U2D);//���ҵ�ת		 									    
   	delay_ms(1200);delay_ms(1300);		
	LCD_Scan_Dir(R2L_D2U);//�������Ҷ���ת		 									    
   	delay_ms(1200);delay_ms(1300);		
	LCD_Scan_Dir(L2R_U2D);//�ָ�Ĭ������	 
}																						    
//3-3�໺�����
void tftlcd_multi_gram_test(void)
{
	u8 i;
	u8 *str; 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"����3-3���໺�����",16,0);	// 
	Show_Str(60,70,lcddev.width,16,"ATK-7' TFTLCDģ��ӵ�ж��8ҳLCD����,�����Խ�����ȫ��8ҳ����",16,0); 
  	delay_ms(1500);delay_ms(1500);		
	str=mymalloc(SRAMIN,60);			//����60���ֽڵ��ڴ�
	for(i=0;i<8;i++)
	{
	 	LCD_SetOperateLayer(i);			//���õ�ǰ��������
		LCD_SetDisplayLayer(i);			//���õ�ǰ��ʾ����
		LCD_Clear(SPEED_COLOR_TBL[i]);
		sprintf((char*)str,"���ǵ�%dҳ����",i);
		POINT_COLOR=BLACK;
 		Show_Str(360,230,lcddev.width,16,str,16,0); 				 
   		delay_ms(1200);delay_ms(1300);		
	}
	myfree(SRAMIN,str);		//�ͷ��ڴ�
	//�ָ�Ĭ������
	LCD_SetOperateLayer(0);
	LCD_SetDisplayLayer(0); 
}
//3-4�������
void tftlcd_backlight_test(void)
{
	u8 i;
	u8 *str; 
	float bkl=0;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str(60,50,lcddev.width,16,"����3-4���������",16,0); 
	Show_Str(60,70,lcddev.width,16,"ATK-7' TFTLCDģ���Դ�������ƹ���,ֻ�跢�����ָ������ñ�������",16,0); 
  	delay_ms(1500);delay_ms(1500);		
 	str=mymalloc(SRAMIN,60);			//����60���ֽڵ��ڴ�
	for(i=0;i<8;i++)
	{													    
		LCD_BackLightSet(i*8+7);	//������������
		bkl=(float)(i+1)*8/64;
		sprintf((char*)str,"��ǰ��������:%3.1f%%",bkl*100);
		POINT_COLOR=BLUE;
 		Show_Str(330,230,lcddev.width,16,str,16,0); 				 
   		delay_ms(1200);delay_ms(1300);		
	}
	myfree(SRAMIN,str);		//�ͷ��ڴ�
}	  
//03,Һ���Բ���
//�ٶȲ���/�������/�໺�����/�������,�⼸��ѭ�����в���
void tftlcd_self_test(void)
{
	while(1)
	{
		tftlcd_speed_test();
		tftlcd_mirror_test();
		tftlcd_multi_gram_test();
		tftlcd_backlight_test();
	}
}

int main(void)
{        
	u8 key;
	u8 t=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();				//��ʼ��LED 
	usmart_dev.init(84);	//��ʼ��USMART
 	LCD_Init();				//LCD��ʼ��  
 	KEY_Init();				//������ʼ��  
	W25QXX_Init();			//��ʼ��W25Q128
 	TIM3_Int_Init(100-1,8400-1);//10Khz�ļ���Ƶ�ʣ�����100��Ϊ10ms     
	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);	//��ʼ��CCM�ڴ�� 
	exfuns_init();			//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 	//����SD�� 
 	f_mount(fs[1],"1:",1); 	//����FLASH.
	POINT_COLOR=RED;     
 	while(font_init()) 		//����ֿ�
	{	    
		POINT_COLOR=RED;      
		LCD_Clear(WHITE);
		LCD_ShowString(60,50,lcddev.width,16,16,"ALIENTEK STM32");	
		LCD_ShowString(60,70,lcddev.width,16,16,"Font Updating..."); 	 	 
 	 	while(update_font(60,90,16,0)!=0)//������³���
		{						  
			LCD_ShowString(60,90,lcddev.width,16,16," Font Update error! ");		  
			delay_ms(200);    
			LCD_ShowString(60,90,lcddev.width,16,16,"  Please Check....  ");
			delay_ms(200);    
			LED0=!LED0;	
		};			 
		LCD_Clear(WHITE);				  
	}  
	Show_Str(60,50,lcddev.width,16,"ALIENTEK ATK-7' TFTLCD���ݴ���������ʵ��",16,0);				    	 
	Show_Str(60,70,lcddev.width,16,"��ѡ�����ģʽ��",16,0);	
	POINT_COLOR=BLUE;			    	 
	Show_Str(60,90,lcddev.width,16, "KEY0�����ݴ��������ԣ�֧��5�㴥�أ�",16,0);				    	 
	Show_Str(60,110,lcddev.width,16,"KEY1��ͼƬ��ʾ���ԣ���ҪSD��֧�֣�",16,0);				    	 
	Show_Str(60,130,lcddev.width,16,"KEY_UP��Һ���Բ��ԣ��ٶ�/����/�໺��/���⣩",16,0);				    	 
	POINT_COLOR=RED;			    	 
	Show_Str(60,170,lcddev.width,16,"������������ӿƼ����޹�˾(ALIENTEK)",16,0);				    	 
	Show_Str(60,190,lcddev.width,16,"�ٷ���վ��www.alientek.com",16,0);				    	 
	Show_Str(60,210,lcddev.width,16,"��Դ��������̳��www.openedv.com",16,0);				    	 
	Show_Str(60,230,lcddev.width,16,"�绰(����)��020-38271790",16,0);				    	 
	Show_Str(60,250,lcddev.width,16,"2014��4��1��",16,0);
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0����,���ݴ�������
				ctouch_paint_test();
				break;
			case KEY1_PRES://KEY1����,ͼƬ��ʾ����
				picture_display_test();
				break;
			case WKUP_PRES://KEY_UP����,Һ���Բ���
				tftlcd_self_test();
				break;	 
		}
		t++;
		if(t==20)
		{		 				   
			t=0;
			LED0=!LED0;
			if(LED0)Show_Str(60,70,lcddev.width,16,"��ѡ�����ģʽ��",16,0);
			else LCD_Fill(60,70,60+128,70+16,WHITE);
		}else delay_ms(10);
	}   		     	
}










