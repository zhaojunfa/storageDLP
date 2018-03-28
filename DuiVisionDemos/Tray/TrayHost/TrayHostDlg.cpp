#include "StdAfx.h"
#include "TrayHostDlg.h"
#include "registry.h"


//#include "..\..\interface\xmlmsg\XmlMessage.h"


#define XML_ABOUT_DLG _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"����[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\" />\
	</base>\
	<body>\
		<area name=\"area-1\" pos=\"0,0,-0,40\" begin-transparent=\"100\" end-transparent=\"30\" />\
		<area name=\"area-2\" pos=\"0,40,-0,-0\" begin-transparent=\"30\" end-transparent=\"30\" />\
		<area name=\"area-3\" pos=\"0,-37,-0,-36\" begin-transparent=\"70\" end-transparent=\"70\" />\
		<area name=\"area-4\" pos=\"0,-36,-0,-0\" begin-transparent=\"88\" end-transparent=\"88\" />\
		\
		<img name=\"icon\" pos=\"25,45\" width=\"128\" height=\"128\" image=\"skins\\scriptnet.jpg\" mode=\"normal\" framesize=\"0\" response=\"0\" />\
		\
		<text crtext=\"000000\" pos=\"170,45,-25,65\" title=\"[APP_NAME] [APP_VER]\" />\
		<text crtext=\"000000\" pos=\"170,65,-25,85\" title=\"��Ȩ����2013-2014\" />\
		<linkbtn name=\"linkbtn1\" crtext=\"800000\"\
			pos=\"170,100,-25,130\" title=\"���������\" href=\"http://www.blueantstudio.net\" />\
		<text crtext=\"000080\" pos=\"170,140,-25,160\" title=\"�˶Ի����ɶ����XML���ݴ���\" />\
		<button name=\"button.ok\" skin=\"IDB_BT_DEFAULT\" title=\"[OK]\" pos=\"-100,-30,-20,-6\" />\
	</body>\
</dlg>");

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CTrayHostDlg::CTrayHostDlg(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
	m_bMouseDownImageNormal = FALSE;
}

CTrayHostDlg::~CTrayHostDlg(void)
{

}

// ��ʼ��
void CTrayHostDlg::OnInit()
{
	// ��ʼ������ͼ��
	DuiSystem::Instance()->InitTray();
	// ��tabҳ3ע���¼��������
	//DuiSystem::RegisterHandler(m_pDlg, new CDuiHandlerTab3(), _T("tab3"));
	// ������ҳע���¼��������
	//CDuiHandlerTools* pDuiHandlerTools = new CDuiHandlerTools();
	//pDuiHandlerTools->SetDialog(m_pDlg);
	//DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerTools, _T("tab.tools"));
	//pDuiHandlerTools->OnInit();
	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);

	m_uTimerTray = DuiSystem::AddDuiTimer(500);

	// ��ʼ��ԭ���ؼ�
	//CDuiNativeWnd* pNativeWnd = (CDuiNativeWnd*)GetControl(_T("nativewnd_1"));
	//if(pNativeWnd)
	//{
	//	CEdit* pEdit = new CEdit;
	//	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL;
	//	pEdit->Create(dwStyle, CRect(0,0,0,0), pNativeWnd->GetPaintWnd(), 1111);
	//	pNativeWnd->SetNativeWnd(pEdit);

	//	/*CListViewCtrlEx* pWndList = new CListViewCtrlEx;
	//	DWORD dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
	//	pWndList->Create(pNativeWnd->GetPaintHWnd(), NULL, NULL,
	//		dwStyle, 0, 1112, NULL);
	//	pWndList->SetObserverWindow(pNativeWnd->GetPaintHWnd());
	//	pWndList->InsertColumn(0, L"��1", LVCFMT_CENTER, 50);
	//	pWndList->InsertColumn(1, L"��2", LVCFMT_CENTER, 150);
	//	pWndList->InsertColumn(2, L"��3", LVCFMT_CENTER, 100);
	//	pWndList->SetItemHeight(22);
	//	int nItem = pWndList->AppendTitle("row1", Color(80,80,80), LISTITEM_EXPANDABLE|LISTITEM_BOLD);
	//	pWndList->AppendSubItem(nItem, "test1", SUBITEM_LINK);
	//	pNativeWnd->SetNativeWnd(pWndList);*/
	//}

	// ��ʾ�б�ؼ�ͨ��API����ָ�����д��ڿɼ���Χ
	//CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	//if(pListCtrl)
	//{
	//	pListCtrl->EnsureVisible(10, TRUE);
	//}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_policy"));
	//if(pGridCtrl)
	//	{
	//		// ������Ӱ�ť
	//		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
	//		if(pControl)
	//		{
	//			pControl->SetName(_T("grid1_updateinfo_link"));
	//			pControl->SetPosStr(_T("100, 3, -10, 25"));
	//			pControl->SetTitle(_T("��������"));
	//			pControl->SetLink(_T("http://www.blueantstudio.net"));
	//			pGridCtrl->AddSubItemControl(1, 0, pControl);
	//		}
	//
	//		// ��������
	//		CDuiEdit* pControlEdit = (CDuiEdit*)DuiSystem::CreateControlByName(_T("edit"), m_pDlg->GetSafeHwnd(), NULL);
	//		if(pControlEdit)
	//		{
	//			pControlEdit->SetName(_T("edit_gridctrl_btnName"));
	//			pControlEdit->SetTitle(_T("100"));
	//			pControlEdit->SetPosStr(_T("220, 10, -20, 40"));
	//			pControlEdit->OnAttributeSkin(_T("skin:IDB_EDIT"), TRUE);
	//			pGridCtrl->AddSubItemControl(4, 0, pControlEdit);
	//		}
	//	}

	// ��ʾͨ��API���gridctrl��
	if(0)
	{
		for(int i=0; i<10; i++)
		{
			CString strId;
			strId.Format(_T("id_%d"), i);
			int nRow = pGridCtrl->InsertRow(-1,	// ����������,-1��ʾ��ӵ����
				strId,							// ��id�ַ���
				-1,								// �����ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				Color(0, 0, 0, 0),				// ��������ɫ,ȫ0��ʾĬ��(��ʹ����������ɫ,ʹ�ñ��ȫ����ɫ)
				_T("skins\\icon\\scriptnet.png"),	// ������ͼƬ�ļ�
				-1,								// ���Ҳ�ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				_T(""),							// ���Ҳ��ͼƬ�ļ�
				0);						// �����ļ���״̬(-1��ʾ����ʾ����)
			CString strText;
			strText.Format(_T("c:\\Program Files\\test%d.txt"), i);
			pGridCtrl->SetSubItem(nRow, 0, strText);
			pGridCtrl->SetSubItem(nRow, 1, _T("2017-07-23"));
			pGridCtrl->SetSubItem(nRow, 2, _T("�����ļ�"));

			CTextButton* pControlBtn = (CTextButton*)DuiSystem::CreateControlByName(_T("textbtn"), m_pDlg->GetSafeHwnd(), NULL);
			if(pControlBtn)
			{
				pControlBtn->SetName(_T("gridctrl_restorebtn"));
				pControlBtn->SetTitle(_T("�ָ�"));
				pControlBtn->SetPosStr(_T("0, 15, -20, 30"));
				pControlBtn->OnAttributeSkin(_T("skin:IDB_BT_DEFAULT"), TRUE);
				
				pGridCtrl->AddSubItemControl(nRow, 3, pControlBtn);
			}

			int ks = 0;
		}
	}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
// 	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
// 	if(pTreeCtrl)
// 	{
// 		HDUITREEITEM hNode = pTreeCtrl->GetNodeById(_T("1-0"));
// 		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
// 		if(pControl)
// 		{
// 			pControl->SetName(_T("tree1_updateinfo_link"));
// 			pControl->SetPosStr(_T("10, 5, -10, 25"));
// 			pControl->SetTitle(_T("��������"));
// 			pControl->SetLink(_T("http://www.blueantstudio.net"));
// 			pTreeCtrl->AddSubItemControl(hNode, 2, pControl);
// 		}
// 	}

	// ��ʾ���ؼ�ָ��ĳ���ڵ��ڿɼ���Χ��
// 	pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
// 	if(pTreeCtrl)
// 	{
// 		HDUITREEITEM hNode = pTreeCtrl->GetNodeById(_T("2-7-1-1-1"));
// 		pTreeCtrl->EnsureVisible(hNode, TRUE);
// 	}

	// ��ʾ��div�ж�̬����ӿؼ�(��ӵ������ؼ��ı༭��ҳ��)
	/*CControlBase* pDiv = (CControlBase*)GetControl(_T("layout-1"));
	if(pDiv)
	{
		CDuiEdit* pControlEdit = static_cast<CDuiEdit*>(DuiSystem::CreateControlByName(_T("edit"), m_pDlg->GetSafeHwnd(), pDiv));
		if(pControlEdit)
		{
			pDiv->AddControl(pControlEdit);
			pControlEdit->SetName(_T("btnName"));
			pControlEdit->SetTitle(_T("��̬��ӵı༭��ؼ�"));
			pControlEdit->SetPosStr(_T("50,220,250,250"));
			pControlEdit->OnAttributeSkin(_T("skin:IDB_EDIT"), FALSE);
			pControlEdit->OnAttributeLeftImage(_T("skin:IDB_COMPUTER"), FALSE);
			pControlEdit->OnPositionChange();
		}
	}*/

	InitControls();
}

void CTrayHostDlg::InitControls()
{
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_isolate"));
	if (pGridCtrl)
	{
		pGridCtrl->ClearItems();

		CAtlList<StorageIsoalte> list;
		CStorageIsolateTableInstance()->QueryRecord(list);

		POSITION pos = list.GetHeadPosition();
		while (pos)
		{
			StorageIsoalte item = list.GetNext(pos);

			int nRow = pGridCtrl->InsertRow(-1,		// ����������,-1��ʾ��ӵ����
				item.Guid,							// ��id�ַ���
				-1,									// �����ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				Color(0, 0, 0, 0),					// ��������ɫ,ȫ0��ʾĬ��(��ʹ����������ɫ,ʹ�ñ��ȫ����ɫ)
				_T(""),								// ������ͼƬ�ļ�
				-1,									// ���Ҳ�ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				_T(""),								// ���Ҳ��ͼƬ�ļ�
				0);									// �����ļ���״̬(-1��ʾ����ʾ����)
			pGridCtrl->SetSubItem(nRow, 0, item.FileName, item.FilePath);
			pGridCtrl->SetSubItem(nRow, 1, item.IsolateTime, item.IsolatePath);
			pGridCtrl->SetSubItem(nRow, 2, item.IsolateReason);
		}
	}

	CDuiGridCtrl* pGrid = (CDuiGridCtrl*)GetControl(_T("gridctrl_config"));

	if (pGrid)
	{
		pGrid->SetSubItem(0, 1, CMiscHelper::RegGetString(L"StorageDLP\\CheckIP"));
		pGrid->SetSubItem(1, 1, CMiscHelper::RegGetString(L"StorageDLP\\MaxThreads"));
		pGrid->SetSubItem(2, 1, CMiscHelper::RegGetString(L"StorageDLP\\ScanDir"));
		pGrid->SetSubItem(3, 1, CMiscHelper::RegGetString(L"StorageDLP\\IsolationDir"));
		pGrid->SetSubItem(4, 1, CMiscHelper::RegGetString(L"StorageDLP\\SafeDir"));

		CString connectState =  CMiscHelper::RegGetString(L"StorageDlp\\ConnectionState", L"0");
		if (connectState == L"0")
		{
			connectState = L"��������������";
		}
		else
		{
			connectState = L"�����������쳣";
		}
		pGrid->SetSubItem(5, 1, connectState);
	}
}


// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
LRESULT CTrayHostDlg::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}

// ����˫����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	

	m_pDlg->ShowWindow(SW_SHOW);

	return S_OK;
}

// �������������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTrayIconLButtonDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//DuiSystem::ShowDuiDialog(_T("dlg_about"), NULL);
	return TRUE;
}

// ��ʾ��ʾ����1��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ1��\n�����ʾ���񣬴��ڴ�С����Ϊ400*250���˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T("�Զ������"), 5000, 400, 250);
	return TRUE;
}

// ��ʾ��ʾ����2��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ2��\n�����ʾ���񣬴��ڴ�С����Ϊ300*200���˴���3���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// ��ʾ��ʾ����3��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �Զ����Notify��ʾ����
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
	// ���ñ��������
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), _T("�Զ�����ʾ����"));
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), _T("������ʽ��ʾ���ڣ����Զ��رգ���СΪ250*300����ʾȷ����ť��................."));
	// ����ͼ��
	DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
	// ��������,5������,��Ҫ�Ѳ��õ��������ص�
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.0"), TRUE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.1"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.2"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.3"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.4"), FALSE);
	// ���ô�С
	DuiSystem::SetNotifyMsgBoxSize(250, 300);
	// ��ʾ����
	DuiSystem::ShowNotifyMsgBox();
	return TRUE;
}

// ��ʾ��Ϣ�Ի���1��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	return TRUE;
}

// ��ʾ��Ϣ�Ի���2��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ���Զ������\n2.��ťʹ��ȷ��ȡ��(MB_OKCANCEL)��ť\n3.ͼ��ʹ�ô���ͼ��"), _T("�Զ������"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// ��ʾ��Ϣ�Ի���3��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���3��\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ���Ƿ�ȡ��(MB_YESNOCANCEL)��ť\n3.ͼ��ʹ�ü��ͼ��\n4.�Զ���Ի���Ĵ�С"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// ������ʾ��ʱ����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ������������ʱ����20�룡"));
	return TRUE;
}

// ֹͣ��ʾ��ʱ����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ����ֹͣ��"));
	return TRUE;
}

// ��ʾXML�Ի�����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ʾͨ�������XML���ݼ��ش���
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// ������tabҳ�е�ĳ��ҳ����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgHideTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, FALSE);
		//pTabCtrl->DeleteItem(6);
		//pTabCtrl->DeleteItem(L"tab_6");
	}
	return TRUE;
}

// ��ʾ��tabҳ�е�ĳ��ҳ����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgShowTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, TRUE);
	}
	return TRUE;
}

// ɾ��Webҳ��Tabҳ��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgDelWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->DeleteItem(_T("tab.activex"));
	}
	return TRUE;
}

// ����Webҳ��Tabҳ��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgLoadWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->LoadTabXml(_T("duivision\\tab_ext_web.xml"));
	}
	return TRUE;
}

// �˵�1��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	// ��ʾ����ڲ˵�����ʱ����Ĳ˵������ʾ���⡢�ɼ��ԡ��Ƿ���á��Ƿ�ѡ�������
	// �����ڵ���LoadXmlFile֮ǰͨ���˵���������������Ӧ�˵��������
	pDuiMenu->SetItemTitle(_T("item_login"), _T("��֤--�޸�Title"));
	pDuiMenu->SetItemCheck(_T("item_setup"), 0);
	pDuiMenu->SetItemVisible(_T("item_help"), FALSE);
	pDuiMenu->SetItemDisable(_T("item_about"), TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �˵�2��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	if(pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �б�ؼ������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ,���������wParam��ʾ�ؼ��к�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CDlgPopup* pDlgPopup = new CDlgPopup;
		CRect rc = pListCtrl->GetRect();
		rc.OffsetRect(50, 20);
		pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
		CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
		if(pControlTitle)
		{
			pControlTitle->SetTitle(pRowInfo->strTitle);
		}
		CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
		if(pControlTime)
		{
			pControlTime->SetTitle(pRowInfo->strTime);
		}
		CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
		if(pControlContent)
		{
			//CString strTmp = L"www\nwww\nj";
			pControlContent->SetTitle(pRowInfo->strContent);
		}
		pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
	}
	return TRUE;
}

// �б�ؼ������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��
	// ���������wParam��ʾ�ؼ��к�,lParam��ʾ��������ӵ�����(����1��2�ֱ�Ϊ0��1)
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		if(lParam != -1)
		{
			// ������е�ĳ������
			CString strMsg;
			strMsg.Format(_T("������б��е�����\n�����ݣ�%s\n���������ţ�%d"), pRowInfo->strTitle, lParam);
			DuiSystem::DuiMessageBox(NULL, strMsg);
		}
	}

	return TRUE;
}

// �б�ؼ�˫����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫�����б�ؼ�ĳһ��,���������wParam��ʾ�ؼ��к�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫�����б��е���\n�кţ�%d\n�����ݣ�%s"), wParam, pRowInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// ���ؼ�˫����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgGridCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫���˱��ؼ�ĳһ��,���������wParam��ʾ�ؼ��к�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫���˱���еĵ�Ԫ��\n�кţ�%d\n�кţ�%d\n�����ݣ�%s"), wParam, lParam, pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}
/*
//
// ɾ��gridctrl�е�������
//
class CDuiDeleteGridRowTask : public DuiVision::IBaseTask
{
public:
	CDuiDeleteGridRowTask(LONG type, CDuiGridCtrl* pGridCtrl, int nRow)
		: DuiVision::IBaseTask(type), m_pGridCtrl(pGridCtrl), m_nRow(nRow)
	{
		SetUITask(TRUE);	// ����Ϊ��ҪתUI�̴߳��������
	}

	// ������
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		if(m_pGridCtrl != NULL)
		{
			m_pGridCtrl->DeleteRow(m_nRow);
		}
		return TRUE;
	}

protected:
	CDuiGridCtrl*	m_pGridCtrl;	// Grid�ؼ�����
	int				m_nRow;			// ɾ������
};
*/
// ���ؼ���ɾ����ť�ӿؼ������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgGridCtrlDelBtnClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����˱��ؼ���ɾ���а�ť�ӿؼ�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	pGridCtrl->DeleteRow(2);
	/*DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiDeleteGridRowTask(2, pGridCtrl, 2));
		pTaskMgr->StartTask();
	}*/
	
	return TRUE;
}

// ��ק�ļ������ؼ���Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgGridCtrlDropFile(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ק��Ϣ��wParam������λ�ã�lParam��ʾ��ǰ��ק���ļ�ȫ·����
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	CPoint* pptDropFile = (CPoint*)wParam;
	CString* pstrDropFile = (CString*)lParam;
	// ��ȡ�ļ���
	CString strFileName = *pstrDropFile;
	int nPos = strFileName.ReverseFind(_T('\\'));
	strFileName.Delete(0, nPos+1);
	// �ڱ���в���һ���ļ���Ϣ
	if(pGridCtrl)
	{
		CString strId = *pstrDropFile;
		int nRow = pGridCtrl->InsertRow(-1,	// ����������,-1��ʾ��ӵ����
			strId,							// ��id�ַ���
			-1,								// �����ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
			Color(0, 0, 0, 0),				// ��������ɫ,ȫ0��ʾĬ��(��ʹ����������ɫ,ʹ�ñ��ȫ����ɫ)
			_T("Skins\\skins\\icon\\NewIcons005.png"),	// ������ͼƬ�ļ�
			-1,								// ���Ҳ�ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
			_T(""),							// ���Ҳ��ͼƬ�ļ�
			0);
		pGridCtrl->SetSubItem(nRow, 0, strFileName, *pstrDropFile, TRUE);
		pGridCtrl->SetSubItem(nRow, 1, _T("�ļ�"));
	}

	return TRUE;
}

// ���ؼ������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HDUITREEITEM hNode = (HDUITREEITEM)wParam;	// ���ڵ���	
	int nItem = lParam;	// ��������ؼ��ĵڼ���
	if(nItem == 0)
	{
		// ��������һ��,��չ���������˽ڵ�
		pTreeCtrl->ToggleNode(hNode);
	}
	return TRUE;
}

// ���ؼ�˫����Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTreeCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫�������ؼ�ĳһ�ڵ�,���������wParam��ʾ�ؼ��ڵ�����
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HDUITREEITEM hNode = (HDUITREEITEM)wParam;	// ���ڵ���
	int nItem = lParam;	// ��������ؼ��ĵڼ���
	TreeItemInfo* pItemInfo = pTreeCtrl->GetItemInfo(hNode, nItem);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫�������ڵ�\n�ڵ����ݣ�%s"), pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// ��ʾϵͳ���öԻ���˵���Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP��ַ
	pDlg->SetControlValue(_T("config.server.ip"), _T("title"), _T("192.168.1.1"));

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return TRUE;
}

// ϵͳ���öԻ����ȷ�ϰ�ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP��ַ
	CString strServerAddr = _T("");
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(_T("config.server.ip")));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == _T(""))
		{
			// ��ַ����Ϊ����ʾ
			return TRUE;
		}
	}

	pDlg->DoOK();
	return TRUE;
}

// ��ʾFlash1��ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgBtnShowFlash1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/afternoon.swf"));
	}
	return TRUE;
}

// ��ʾFlash2��ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgBtnShowFlash2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/morning.swf"));
	}
	return TRUE;
}

// ��ʾFlash3��ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgBtnShowFlash3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/night.swf"));
	}
	return TRUE;
}

// ��ʾFlash4��ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgBtnShowFlash4(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/noon.swf"));
	}
	return TRUE;
}

// Tabҳǩ�رհ�ť��Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgTabCtrlClose(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

// image-normal�ؼ���������������Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgImageNormalMouseLDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	m_bMouseDownImageNormal = TRUE;
	m_ptControlImageNormal = *((CPoint*)lParam);

	return TRUE;
}

// image-normal�ؼ����������ſ���Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgImageNormalMouseLUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	m_bMouseDownImageNormal = FALSE;

	return TRUE;
}

// image-normal�ؼ�������ƶ���Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgImageNormalMouseMove(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	// ����϶��ؼ�
	if(m_bMouseDownImageNormal)
	{
		CPoint ptNew = *((CPoint*)lParam);	// ��ǰ���λ��
		CControlBase* pParentCtrl = (CControlBase*)(pImgCtrl->GetParent());
		if(pParentCtrl)
		{
			CRect rcParent = pParentCtrl->GetRect();
			if(!rcParent.PtInRect(ptNew))
			{
				// ������λ���Ѿ����ڸ��ؼ���Χ�����˳�
				return FALSE;
			}
		}

		// ��ȡ�ؼ��ĵ�ǰλ�ú���굱ǰλ������һ��λ�õĲ�ֵ,���ؼ���ǰλ�ü������λ�õĲ�ֵ
		CRect rc = pImgCtrl->GetRect();
		CSize offset = ptNew - m_ptControlImageNormal;
		rc.OffsetRect(offset);
		pImgCtrl->SetRect(rc);

		// ˢ�������һ��λ�õı���
		m_ptControlImageNormal = *((CPoint*)lParam);

		// ˢ�¿ؼ�
		pImgCtrl->UpdateControl(true);
	}

	return TRUE;
}

// ����ͷ�ı��ؼ��Ҽ��ſ���Ϣ����,��ʾ�����Ҽ��˵�
LRESULT CTrayHostDlg::OnDuiMsgGridCtrlHeaderRButtonUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ���ؼ�ĳһ��,���������wParam��ʾ����к�,lParam��ʾ�к�
// 	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_header"));
// 	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
// 	if(pItemInfo)
// 	{
// 		// ��ʾ�Ҽ��˵�
// 		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
// 		pDuiMenu->SetAutoClose(FALSE);
// 		pDuiMenu->SetParent(pGridCtrl);
// 		CPoint point;
// 		CRect rc = pItemInfo->rcItem;
// 		// ����˵�����ʾλ��
// 		point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);
// 
// 		CDlgBase* pParentDlg = GetControlDialog(uID);
// 
// 		// ����ת��Ϊ��Ļ����
// 		pParentDlg->ClientToScreen(&point);
// 
// 		CString strXmlFile = _T("duivision\\menu_tray.xml");
// 		if(pDuiMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU))
// 		{
// 			CRect rcMenu;
// 			pDuiMenu->GetWindowRect(&rcMenu);
// 			pDuiMenu->MoveWindow(rcMenu);
// 
// 			pDuiMenu->ShowWindow(SW_SHOW);
// 			pDuiMenu->SetAutoClose(TRUE);
// 		}
// 	}
	return TRUE;
}

/*
// DUI�¼�����
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerMain::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

    return 0;
}
*/
// DUI��ʱ���¼�����
void CTrayHostDlg::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		CDuiProgress* pProgress2 = (CDuiProgress*)GetControl(_T("progress_2"));
		if(pProgress2)
		{
			pProgress2->SetProgress(m_nAniIndex*10);
		}

		m_nAniIndex++;
		if(m_nAniIndex > 10)
		{
			m_nAniIndex = 0;
		}
	}else
	if(_T("timer_notify") == strTimerName)
	{
		// ��ʱ��ʾ��ʾ��
		DuiSystem::AddDuiNotifyMsgTask(_T("�����ɶ�ʱ����������ʾ��\n�������ڣ�20��\n�˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 5000);
	}
	else if (m_uTimerTray == uTimerID)
	{
		DuiSystem::Instance()->UpdateTray();
	}
}

// ���̼���Ϣ����
LRESULT CTrayHostDlg::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	if (Msg == MSG_NOTIFY_DLG)
	{
		return S_OK;
	}
	else
	{
		return OnDuiMsgInterprocessDefault(uID, strName, Msg, wParam, lParam);
	}
	return TRUE;
}
LRESULT	CTrayHostDlg::OnDuiMsgInterprocessDefault(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, _T("ִ���������в���:") + strCmd);
	}
	else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(_T("dlg_main"));
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return S_OK;
}

LRESULT CTrayHostDlg::OnDuiMsgGridCtrlRestoreMouseUp( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	return TRUE;
}

BOOL MoveFileToSafeDir(CString srcPath, CString FileName)
{
	BOOL ret = FALSE;

	CString NewPath = CMiscHelper::RegGetString(L"StorageDLP\\SafeDir");
	if (NewPath.IsEmpty())
		return FALSE;
	NewPath.TrimRight(L"\\");

	CString temp = FileName;
	for (int i = 1; i < 10; i++)
	{
		if(MoveFile(srcPath, NewPath + L"\\" + temp))
		{
			ret = TRUE;
			break;
		}

		DWORD dw = GetLastError();
		if (dw == 183)
		{
			CString temp1;
			temp1.Format(L"(%d)", i);
			int index = FileName.Find(L".");
			if (index != -1)
			{
				temp = FileName.Left(index) + temp1 + FileName.Right(FileName.GetLength() - index);
			}
		}
	}
	if (!ret)
	{
		int index = FileName.Find(L".");
		temp = FileName.Left(index) + CStorageIsolateTableInstance()->NewGuid() + FileName.Right(FileName.GetLength() - index);
		ret = MoveFile(srcPath, NewPath + L"\\" + temp);
	}
	return ret;
}

LRESULT CTrayHostDlg::OnBtnRestorageMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_isolate"));
	if (pGridCtrl)
	{
		int count = pGridCtrl->GetRowCount();

		for (int i = 0 ; i < count; i++)
		{
			GridRowInfo* info = pGridCtrl->GetRowInfo(i);
			
			if (info && info->nCheck == 1)
			{
				// �����ݻָ�
				CAtlList<StorageIsoalte> list;
				CStorageIsolateTableInstance()->QueryRecord(list, info->strId);
				POSITION pos = list.GetHeadPosition();
				while (pos)
				{
					StorageIsoalte item = list.GetNext(pos);

					if(MoveFileToSafeDir(item.IsolatePath, item.FileName))
					{
						// ɾ�����ݿ���  �Լ�ɾ��
						if(CStorageIsolateTableInstance()->DeleteRecord(info->strId))
						{
							// ����ɾ������
							pGridCtrl->DeleteRow(i);
							i--;
							count--;
						}


						// ���͵����ɾ��
// 						CXmlMessage xmlmsg;
// 						if(xmlmsg.Create(0, PLUGIN_NAME, L"SendToCheck", L"deleteisolate") == S_OK)
// 						{
// 							xmlmsg.AddTail(L".", L"guid", info->strId);
// 						}
// 						CString xml;
// 						xmlmsg.Save(xml);
// 
// 						CTepPipeClient::Instance()->SendMsgToUi(xml);
// 
// 						xmlmsg.Load(xml);
// 						if (xmlmsg.GetValueI32(L"./result") == 1)
// 						{
// 							// ����ɾ������
// 							pGridCtrl->DeleteRow(i);
// 							i--;
// 							count--;
// 						}
					}
					else
					{
						MessageBox(NULL, L"�ָ�ʧ�ܣ�\n·����" + item.IsolatePath, L"����", MB_ICONERROR | MB_OK);
					}
				}
			}
		}
	}

	return S_OK;
}

LRESULT CTrayHostDlg::OnDlgRefreshMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	InitControls();

	return S_OK;
}
LRESULT CTrayHostDlg::OnDlgSetServerMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDlgBase* base = DuiSystem::Instance()->GetDuiDialog(L"dlg_setserver");
	if (!base)
		return S_FALSE;

	CDuiEdit* editIp = (CDuiEdit*)base->GetControl(L"edit.server.ip");
	CString ip;
	if (editIp)
	{
		ip = editIp->GetTitle();
	}
	if (!ip.IsEmpty())
	{
		CString xml;
		CXmlMessage msg;
		msg.Create(0, PLUGIN_NAME, L"SendToCheck", L"UpdateSeverIp");
		CXmlNode policyNode = msg.Add(L"./policy");

		policyNode.AddTail(L".", L"serverip", ip);

		msg.Save(xml);
		CTepPipeClient::Instance()->SendMsgToUi(xml);

		if (msg.Load(xml) != S_OK)
			return S_FALSE;

		if (msg.GetValueI32(L"./result") == 1)
		{
			DuiSystem::DuiMessageBox(NULL, _T("\n���óɹ���"), _T("����������"), MB_OK, 200, 150);
			base->DoOK();
		}
		else
			DuiSystem::DuiMessageBox(NULL, _T("\n����ʧ�ܣ�"), _T("����������"), MB_ICONWARNING, 200, 150);
	}
	
	InitControls();
	return S_OK;
}

LRESULT CTrayHostDlg::OnDlgSetScanMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDlgBase* base = DuiSystem::Instance()->GetDuiDialog(L"dlg_setscandir");
	if (!base)
		return S_FALSE;

	CDuiEdit* editdir = (CDuiEdit*)base->GetControl(L"edit.scandir");

	CString dir;
	if (editdir)
	{
		dir = editdir->GetTitle();
	}
	if (!dir.IsEmpty())
	{
		CString xml;
		CXmlMessage msg;
		msg.Create(0, PLUGIN_NAME, L"SendToCheck", L"UpdateScanDir");
		CXmlNode policyNode = msg.Add(L"./policy");

		policyNode.AddTail(L".", L"scandir", dir);

		msg.Save(xml);
		CTepPipeClient::Instance()->SendMsgToUi(xml);

		if (msg.Load(xml) != S_OK)
			return S_FALSE;

		if (msg.GetValueI32(L"./result") == 1)
		{
			DuiSystem::DuiMessageBox(NULL, _T("\n���óɹ���"), _T("ɨ��·������"), MB_OK, 200, 150);
			base->DoOK();
		}
		else
			DuiSystem::DuiMessageBox(NULL, _T("\n����ʧ�ܣ�"), _T("ɨ��·������"), MB_ICONWARNING, 200, 150);
	}
	InitControls();
	return S_OK;
}

LRESULT CTrayHostDlg::OnDlgSetSafeMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDlgBase* base = DuiSystem::Instance()->GetDuiDialog(L"dlg_setsafedir");
	if (!base)
		return S_FALSE;

	CDuiEdit* editdir = (CDuiEdit*)base->GetControl(L"edit.safedir");

	CString dir;
	if (editdir)
	{
		dir = editdir->GetTitle();
	}

	if (!dir.IsEmpty())
	{
		CString xml;
		CXmlMessage msg;
		msg.Create(0, PLUGIN_NAME, L"SendToCheck", L"UpdateSafeDir");
		CXmlNode policyNode = msg.Add(L"./policy");

		policyNode.AddTail(L".", L"safedir", dir);

		msg.Save(xml);
		CTepPipeClient::Instance()->SendMsgToUi(xml);

		if (msg.Load(xml) != S_OK)
			return S_FALSE;

		if (msg.GetValueI32(L"./result") == 1)
		{
			DuiSystem::DuiMessageBox(NULL, _T("\n���óɹ���"), _T("��ȫĿ¼����"), MB_OK, 200, 150);
			base->DoOK();
		}
		else
			DuiSystem::DuiMessageBox(NULL, _T("\n����ʧ�ܣ�"), _T("��ȫĿ¼����"), MB_ICONWARNING, 200, 150);
	}
	InitControls();
	return S_OK;
}

LRESULT CTrayHostDlg::OnDlgMaxThreadMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDlgBase* base = DuiSystem::Instance()->GetDuiDialog(L"dlg_maxthread");
	if (!base)
		return S_FALSE;

	CDuiEdit* editmt = (CDuiEdit*)base->GetControl(L"edit.maxthread");

	CString count;
	if (editmt)
	{
		count = editmt->GetTitle();
	}

	if (!count.IsEmpty())
	{
		CString xml;
		CXmlMessage msg;
		msg.Create(0, PLUGIN_NAME, L"SendToCheck", L"UpdateMaxNummer");
		CXmlNode policyNode = msg.Add(L"./policy");

		policyNode.AddTail(L".", L"maxthread", count);

		msg.Save(xml);
		CTepPipeClient::Instance()->SendMsgToUi(xml);

		if (msg.Load(xml) != S_OK)
			return S_FALSE;

		if (msg.GetValueI32(L"./result") == 1)
		{
			DuiSystem::DuiMessageBox(NULL, _T("\n���óɹ�����������Ч��"), _T("��󲢷�����"), MB_OK, 200, 150);
			base->DoOK();
		}
		else
			DuiSystem::DuiMessageBox(NULL, _T("\n����ʧ�ܣ�"), _T("��󲢷�����"), MB_ICONWARNING, 200, 150);
	}

	InitControls();
	return S_OK;
}

LRESULT CTrayHostDlg::OnDlgSetIsoMouseDown( UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CDlgBase* base = DuiSystem::Instance()->GetDuiDialog(L"dlg_setisodir");
	if (!base)
		return S_FALSE;

	CDuiEdit* editiso = (CDuiEdit*)base->GetControl(L"edit.isodir");

	CString dir;
	if (editiso)
	{
		dir = editiso->GetTitle();
	}
	if (!dir.IsEmpty())
	{
		CString xml;
		CXmlMessage msg;
		msg.Create(0, PLUGIN_NAME, L"SendToCheck", L"UpdateIsolationDir");
		CXmlNode policyNode = msg.Add(L"./policy");

		policyNode.AddTail(L".", L"isolationdir", dir);

		msg.Save(xml);
		CTepPipeClient::Instance()->SendMsgToUi(xml);

		if (msg.Load(xml) != S_OK)
			return S_FALSE;

		if (msg.GetValueI32(L"./result") == 1)
		{
			DuiSystem::DuiMessageBox(NULL, _T("\n���óɹ���"), _T("����Ŀ¼����"), MB_OK, 200, 150);
			base->DoOK();
		}
		else
			DuiSystem::DuiMessageBox(NULL, _T("\n����ʧ�ܣ�"), _T("����Ŀ¼����"), MB_ICONWARNING, 200, 150);
	}

	InitControls();
	return S_OK;
}



