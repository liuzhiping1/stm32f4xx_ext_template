#include "checkboxdemo.h"
#include "GUI.h"
#include "DIALOG.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemWin CHECKBOX控件使用
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Check box sample",   0,   10,  10, 300, 220, FRAMEWIN_CF_MOVEABLE},
  { TEXT_CreateIndirect,     "Enabled:",           0,    5,  10, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK0,    5,  30, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK1,    5,  60, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK2,    5,  90, 120,   20 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK3,    5, 125, 120,   26 },
  { TEXT_CreateIndirect,     "Disabled:",          0,  150,  10, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK4,  150,  30, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK5,  150,  60, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK6,  150,  90, 120,   26 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK7,  150, 125, 120,   26 },
  { BUTTON_CreateIndirect,   "OK",         GUI_ID_OK,   10, 165,  80,  30 },
  { BUTTON_CreateIndirect,   "Cancel", GUI_ID_CANCEL,  200, 165,  80,  30 },
};

//复选框文本
static const char* _apLabel[]={
	"Default",
	"3 States",
	"Box XL",
	"Box XXL"
};

//回调函数
static void _cbCallback(WM_MESSAGE *pMsg)
{
	int i;
	int NCode,Id;
	WM_HWIN hDlg,hItem;
	hDlg = pMsg->hWin;
	switch(pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			hItem = WM_GetDialogItem(hDlg,GUI_ID_CHECK0);	//获取CHECKBOX的句柄
			for(i=0;i<8;i++)
			{
				int Index=i%4;
				hItem = WM_GetDialogItem(hDlg,GUI_ID_CHECK0+i); //获取CHECKBOX的句柄
				CHECKBOX_SetText(hItem,_apLabel[Index]);
				switch(Index)
				{
					case 1:
						CHECKBOX_SetNumStates(hItem,3);
						CHECKBOX_SetImage(hItem,&_abmBar[0],CHECKBOX_BI_INACTIV_3STATE);
						CHECKBOX_SetImage(hItem,&_abmBar[1],CHECKBOX_BI_ACTIV_3STATE);
						CHECKBOX_SetState(hItem,2);
						break;
					case 2:
						CHECKBOX_SetState(hItem,1);
						CHECKBOX_SetImage(hItem,&_abmXL[0],CHECKBOX_BI_INACTIV_CHECKED);
						CHECKBOX_SetImage(hItem,&_abmXL[1],CHECKBOX_BI_ACTIV_CHECKED);
						CHECKBOX_SetFont(hItem,&GUI_FontComic18B_ASCII);
						break;
					case 3:
						CHECKBOX_SetState(hItem,1);
						CHECKBOX_SetImage(hItem,&_abmXXL[0],CHECKBOX_BI_INACTIV_CHECKED);
						CHECKBOX_SetImage(hItem,&_abmXXL[1],CHECKBOX_BI_ACTIV_CHECKED);
						CHECKBOX_SetFont(hItem,&GUI_FontComic24B_ASCII);
						break;
				}
				//失能对话框右边的所有CHECK小工具
				if(i>=4)
				{
					WM_DisableWindow(hItem);
				}	
			}
			break;
		case WM_NOTIFY_PARENT:
			Id=WM_GetId(pMsg->hWinSrc);	//小工具ID			 
			NCode=pMsg->Data.v;			//通知代码
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
					if(Id==GUI_ID_OK)
					{
						GUI_EndDialog(hDlg,0);	//关闭对话框
					}
					if(Id==GUI_ID_CANCEL)
					{
						GUI_EndDialog(hDlg,1);	//关闭对话框
					}
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

//CHECKBOX小工具演示Demo
void CheckBoxDemo(void)
{
	GUI_CURSOR_Show();
	#if GUI_SUPPORT_MEMDEV
		WM_SetCreateFlags(WM_CF_MEMDEV);
	#endif
	WM_SetDesktopColor(GUI_BLACK);
	while(1)
	{
		GUI_ExecDialogBox(_aDialogCreate,GUI_COUNTOF(_aDialogCreate),&_cbCallback,0,0,0);
		GUI_Delay(500);
	}
}

