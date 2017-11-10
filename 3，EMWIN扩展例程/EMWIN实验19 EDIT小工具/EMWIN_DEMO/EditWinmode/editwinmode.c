#include "editwinmode.h"
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemwin EDIT控件使用   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


#define WM_APP_SHOW_TEXT	(WM_USER +0)
#define TEXT_MAXLEN	40

WM_HWIN DialoghWin;

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", 0, 55, 90, 200, 110, FRAMEWIN_CF_MOVEABLE},
	{ EDIT_CreateIndirect, "Edit", GUI_ID_EDIT0, 24, 10, 145, 25, 0,15},
	{ BUTTON_CreateIndirect,"Ok", GUI_ID_OK,65,	50,	70,	30}
};

//背景窗口的回调函数
static void _BkCallback(WM_MESSAGE *pMsg)
{
	static WM_HWIN hWin;
	static WM_HWIN hEdit;
	static U8 text;
	char   buffer[TEXT_MAXLEN];
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_DARKGRAY);
			GUI_Clear();
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_DispStringHCenterAt("WIDGET_Edit - Sample", 160, 5);
			GUI_SetFont(&GUI_Font8x16);
			if (text) 
			{
				hEdit=WM_GetDialogItem(DialoghWin,GUI_ID_EDIT0);
				GUI_DispStringHCenterAt("The string you have modified is:", 160, 90);
				EDIT_GetText(hEdit, buffer, TEXT_MAXLEN);
				GUI_DispStringHCenterAt(buffer, 160, 110);
			} 
			else
			{	
				GUI_DispStringHCenterAt("Use keyboard to modify string...", 160, 90);
			}
			break;	
		case WM_APP_SHOW_TEXT:
			hWin=pMsg->hWinSrc;
			WM_HideWindow(hWin);	//隐藏对话框
			text=1;
			WM_InvalidateWindow(WM_HBKWIN); //背景窗口无效
			WM_CreateTimer(WM_HBKWIN,0,3000,0);
			break;
		case WM_TIMER:
			text=0;
			WM_InvalidateWindow(WM_HBKWIN);
			WM_ShowWindow(hWin);
			break;
		default:
			WM_DefaultProc(pMsg);
			break;
	}	
}


//对话框对调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	WM_MESSAGE Msg;

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//初始化FRAMEWIN
			hItem = pMsg->hWin;
			FRAMEWIN_SetText(hItem, "EDIT USER");
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
			//初始化EDIT
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
			EDIT_EnableBlink(hItem, 500, 1);
			EDIT_SetText(hItem, "Hello Word!");
			EDIT_SetFont(hItem, &GUI_Font20_ASCII);
			EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case GUI_ID_EDIT0:	//EDIT控件通知消息
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							break;
						case WM_NOTIFICATION_VALUE_CHANGED:
							break;
					}
					break;
				case GUI_ID_OK:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						
						case WM_NOTIFICATION_RELEASED:
							Msg.MsgId=WM_APP_SHOW_TEXT;
							Msg.hWinSrc=pMsg->hWin;
							WM_SendMessage(WM_HBKWIN,&Msg);
							break;
		
					}		
			}
			break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

void Editwinmode_Demo(void) 
{
	WM_SetCallback(WM_HBKWIN,_BkCallback);
	DialoghWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	while(1)
	{
		GUI_Delay(10);
	}
}






















