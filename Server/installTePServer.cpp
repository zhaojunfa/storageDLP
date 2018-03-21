#include "installTePServer.h"
#include "..\..\TepCode\interface\xmlmsg\XmlMessage.h"
#include "CTepMsgBus.h"
#define _TEP_SERVER_
#include "..\..\TepCode\interface\CTepPipe.h"
#include "..\..\TEPCode\interface\CMiscHelper.h"
#include "CTrayHostMgr.h"

WCHAR* TRAYHOST_NAME = L"TrayHost.exe";

CTepMsgBus* g_bus = NULL;

BOOL bRunning = FALSE;

SERVICE_STATUS servicestatus;

SERVICE_STATUS_HANDLE g_hstatus;

BOOL GetTokenByName(HANDLE &hToken,LPTSTR lpName)  
{  
	if(!lpName)  
	{  
		OutputDebugString(_T("lpName"));
		return FALSE;  
	}  
	HANDLE         hProcessSnap = NULL;   
	BOOL           bRet      = FALSE;   
	PROCESSENTRY32 pe32      = {0};   

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString(_T("hProcessSnap"));
		return (FALSE);   
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);   

	if (Process32First(hProcessSnap, &pe32))   
	{  
		do   
		{  
			if(!_tcscmp(_tcsupr(pe32.szExeFile),_tcsupr(lpName)))  
			{  
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,  
					FALSE,pe32.th32ProcessID);  
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS,&hToken);  
				CloseHandle (hProcessSnap); 
				OutputDebugString(_T("return (bRet);"));
				return (bRet);  
			}  
		}   
		while (Process32Next(hProcessSnap, &pe32));   
		bRet = TRUE;   
	}   
	else   
		bRet = FALSE;  

	CloseHandle (hProcessSnap);   
	return (bRet);  
}




void WINAPI CtrlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_PAUSE:
		{
			servicestatus.dwCurrentState = SERVICE_PAUSED;
		}
		break;
	case SERVICE_CONTROL_CONTINUE:
		{
			servicestatus.dwCurrentState = SERVICE_RUNNING;
		}
		break;
	case SERVICE_CONTROL_STOP:
		{
			servicestatus.dwWaitHint		= 0;
			servicestatus.dwCheckPoint		= 0;
			servicestatus.dwCurrentState	= SERVICE_STOPPED;
			servicestatus.dwWin32ExitCode	= 0;
			SetServiceStatus(g_hstatus, &servicestatus);

			//DeInitTePMsgBus( msgBus );
			//cTrayHost.Stop();
			

			CTepPipeServer::Instance()->Stop();

			CTrayHostMgr::Instance()->Stop();

			CTepPipeClient::Instance()->KillProcess(L"TrayHost.exe");

			if (g_bus)
				delete g_bus;

			//dlp_tp_deinit(ptp);
			bRunning = FALSE;
		}
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	}
}


void WINAPI ServiceMain(int argc, char** argv)
{
	HRESULT ComInit = CoInitializeEx( NULL,  COINIT_MULTITHREADED );

	HANDLE		m_hDevice = NULL;
    servicestatus.dwServiceType = SERVICE_WIN32;
    servicestatus.dwCurrentState = SERVICE_START_PENDING;
    servicestatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN|SERVICE_ACCEPT_STOP;
    servicestatus.dwWin32ExitCode = 0;
    servicestatus.dwServiceSpecificExitCode = 0;
    servicestatus.dwCheckPoint = 0;
    servicestatus.dwWaitHint = 0;
    g_hstatus = ::RegisterServiceCtrlHandler(L"TePServer", CtrlHandler);
    if (g_hstatus == 0)
    {
        return;
    }
    //��SCM ��������״̬
    servicestatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (g_hstatus, &servicestatus);
	bRunning = TRUE;

	// ������Ϣ����
	g_bus = new CTepMsgBus();
	if (g_bus)
	{
		g_bus->LoadAllPlugin();
	}

	// �����ܵ�
	CTepPipeServer::Instance()->Start();

	// �����������
	CTrayHostMgr::Instance()->Start();

	while (bRunning)
	{
		Sleep(3000);
	}


	OutputDebugString(L"TepServer:TePServer stop\n");
	if (ComInit == S_OK)
		CoUninitialize();
	return ;
}


void main()
{

    SERVICE_TABLE_ENTRY entrytable[2];

    entrytable[0].lpServiceName=L"TePServer";

    entrytable[0].lpServiceProc=(LPSERVICE_MAIN_FUNCTION)ServiceMain;

    entrytable[1].lpServiceName=NULL;

    entrytable[1].lpServiceProc=NULL;

    StartServiceCtrlDispatcher(entrytable);

}