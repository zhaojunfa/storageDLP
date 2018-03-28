// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "OnReceive.h"
HMODULE	g_hInst;

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInst = hModule;
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	return TRUE;
}


HANDLE Startup(IMsgBus* bus)
{

	//OutputDebugString(L"DlpPolicyCenter: Startup !!!!!!! ");

	ATLASSERT(bus);
	CMainFrame* Frame = new CMainFrame(bus);
	ATLASSERT(Frame);


	if(!Frame)
		return NULL;

	if(!Frame->OnStartup())
	{
		delete Frame;
		return NULL;
	}

	//MainFrameRef.SetMainFrame(Frame);
	return (HANDLE)Frame;
}

// Stop - 停止
VOID StopUp(HANDLE hPlugin)
{
	CMainFrame* Frame = (CMainFrame*)hPlugin;

	ATLASSERT(Frame);
	try
	{
		Frame->OnStop();
		//MainFrameRef.SetMainFrame(NULL);
		delete Frame;

	}
	catch(...)
	{
	}
}


STDAPI DllRegisterServer(void)
{
	HRESULT hr = S_OK;
	CRegKey root;
	CString	str;
	TCHAR	szPath[MAX_PATH];
	GetModuleFileName(g_hInst , szPath , MAX_PATH);

	do 
	{
		str = _T("Software\\Topsec\\StorageDLP\\Plugins");
		str += "\\";
		str += _T("{44D56991-694A-45E9-80AF-5941D487CBBC}");
		hr = root.Create(HKEY_LOCAL_MACHINE ,str , REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE);
		if(ERROR_SUCCESS != hr)
			break;
		hr = root.SetStringValue(_T("Name") , _T("PolicyCenter"));
		hr = root.SetStringValue(_T("Path") , szPath);
		hr = root.SetStringValue(_T("Description") ,_T("数据防泄露插件"));
		root.Close();
	}while(FALSE);

	return hr;
}


// DllUnregisterServer - 将项从系统注册表中移除
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = S_OK;
	CRegKey root;
	do 
	{
		hr = root.Open(HKEY_LOCAL_MACHINE ,_T("Software\\Topsec\\StorageDLP\\Plugins") ,KEY_WRITE);
		if(ERROR_SUCCESS != hr)
			break;
		hr = root.DeleteSubKey(_T("{44D56991-694A-45E9-80AF-5941D487CBBC}"));
		root.Close();
	}while(FALSE);
	return hr;
}

