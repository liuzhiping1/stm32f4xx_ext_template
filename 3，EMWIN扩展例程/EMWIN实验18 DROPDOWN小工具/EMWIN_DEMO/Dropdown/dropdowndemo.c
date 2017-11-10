#include "dropdowndemo.h"
#include "DIALOG.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemWin DROPDOWN控件使用
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x02)
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x04)

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 88, 24, 109, 60, 0, 0x0, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			hItem = pMsg->hWin;
			//初始化FRAMEWIN
			FRAMEWIN_SetText(hItem, "DROPDOWN ");
			FRAMEWIN_SetFont(hItem, GUI_FONT_16_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			//初始化DROPDOWN
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			DROPDOWN_SetFont(hItem, GUI_FONT_13H_ASCII);
			DROPDOWN_SetAutoScroll(hItem,1);	//启用自动使用滚动条
			DROPDOWN_AddString(hItem, "China");
			DROPDOWN_AddString(hItem, "Cambodia");
			DROPDOWN_AddString(hItem, "Japan");
			DROPDOWN_AddString(hItem, "Australien");
			DROPDOWN_AddString(hItem, "Canada");
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_DROPDOWN_0: //DROPDOWN通知代码
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							break;
						case WM_NOTIFICATION_SEL_CHANGED:
							break;
					}
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

//创建对话框
void dropdown_demo(void) 
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	while(1)
	{
		GUI_Delay(100);
	}
}
