// TrayHost.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TrayHost.h"
#include "TrayHostDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrayHostApp

BEGIN_MESSAGE_MAP(CTrayHostApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTrayHostApp 构造

CTrayHostApp::CTrayHostApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTrayHostApp 对象

CTrayHostApp theApp;


// CTrayHostApp 初始化

BOOL CTrayHostApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	//CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("北京天融信网络安全科技有限公司"));

	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T("xml\\resource.xml"), 1116, IDD_TRAYHOST_DIALOG, _T(""));


	// 检查是否已经有进程在运行
	CString strAppMutex = DuiSystem::Instance()->GetConfig(_T("appMutex"));	// 从配置文件中获取互斥量名字
	if(!strAppMutex.IsEmpty())
	{
		::CreateMutex(NULL,TRUE, _T("Global\\") + strAppMutex);
		if(ERROR_ALREADY_EXISTS == GetLastError() || ERROR_ACCESS_DENIED == GetLastError())
		{
			// 读取命令行参数,如果不需要处理命令行可以直接退出
			CString strCmd = _T("");
			if(__argc > 1)
			{
				strCmd = __targv[1];
				DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("Command line:%s"), strCmd);
			}

			// 发送进程间消息(lParam为1表示不显示界面,appMutex作为应用名,信息参数传递命令行参数)
			CString strAppName = DuiSystem::Instance()->GetConfig(_T("appMutex"));
			DuiSystem::Instance()->SendInterprocessMessage(0, DuiSystem::Instance()->GetAppID(), 1, strAppName, strCmd);
			// 释放DuiVision界面库的资源
			DuiSystem::Release();

			return FALSE; // Here we quit this application
		}
	}

	// 注册WKE控件
	//REGISTER_DUICONTROL(CDuiWkeView, CDuiWkeView::WkeShutdown);
	// 注册MFCDateTime控件
	REGISTER_DUICONTROL(CMFCDateTime, NULL);



	// 创建主窗口
	CDlgBase* pMainDlg = DuiSystem::CreateDuiDialog(_T("dlg_main"), NULL, _T(""), TRUE);
	// 给主窗口注册事件处理对象
	CTrayHostDlg* pHandler = new CTrayHostDlg();
	pHandler->SetDialog(pMainDlg);
	DuiSystem::RegisterHandler(pMainDlg, pHandler);

	// 初始化提示信息窗口
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));

	// 按照非模式对话框创建主窗口,可以默认隐藏
	pMainDlg->Create(pMainDlg->GetIDTemplate(), NULL);
	pMainDlg->ShowWindow(SW_HIDE);

	INT_PTR nResponse = pMainDlg->RunModalLoop();

	// 如果是按照模式对话框运行主窗口,只要改为如下代码就可以
	//INT_PTR nResponse = pMainDlg->DoModal();

	// 释放DuiVision界面库的资源
	DuiSystem::Release();


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}