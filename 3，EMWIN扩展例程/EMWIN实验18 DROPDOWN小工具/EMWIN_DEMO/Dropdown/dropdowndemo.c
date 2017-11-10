#include "dropdowndemo.h"
#include "DIALOG.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemWin DROPDOWN�ؼ�ʹ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x02)
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x04)

//�Ի�����Դ��
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 88, 24, 109, 60, 0, 0x0, 0 },
};

//�Ի���ص�����
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			hItem = pMsg->hWin;
			//��ʼ��FRAMEWIN
			FRAMEWIN_SetText(hItem, "DROPDOWN ");
			FRAMEWIN_SetFont(hItem, GUI_FONT_16_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			//��ʼ��DROPDOWN
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			DROPDOWN_SetFont(hItem, GUI_FONT_13H_ASCII);
			DROPDOWN_SetAutoScroll(hItem,1);	//�����Զ�ʹ�ù�����
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
				case ID_DROPDOWN_0: //DROPDOWN֪ͨ����
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

//�����Ի���
void dropdown_demo(void) 
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	while(1)
	{
		GUI_Delay(100);
	}
}
