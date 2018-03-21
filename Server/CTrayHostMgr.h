#pragma once
#include "..\..\TEPCode\interface\IActiveObject.h"
#include "..\..\TEPCode\interface\CProcessHelper.h"

//#define TRAYHOST_NAME	L"TrayHost.exe"

extern WCHAR* TRAYHOST_NAME;


class CTrayHostMgr : public IActiveObject
{
public:
	static CTrayHostMgr* Instance()
	{
		static CTrayHostMgr obj;

		return &obj;
	}

	~CTrayHostMgr()
	{

	}

	BOOL FindProcess( CString targetProcess)
	{
		BOOL bRet = FALSE;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof( pe32 );
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return bRet;

		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while(bMore)
		{
			CString temp = pe32.szExeFile;
			if (targetProcess.CompareNoCase( temp ) == 0)
			{
				bRet = TRUE;
				break;
			}

			bMore = ::Process32Next(hProcessSnap, &pe32);
		}
		::CloseHandle(hProcessSnap);
		return bRet;
	}
	
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

	BOOL StartTrayHost()
	{
		OutputDebugString(L"TepServer: StartTrayHost in !!");

		BOOL ret = FALSE;
		CString path = CMiscHelper::GetAppPath();

		CString hostpath = path + TRAYHOST_NAME;

		// 获取explorer令牌， 以用户权限启动进程
		HANDLE hToken = INVALID_HANDLE_VALUE;
		if(GetTokenByName(hToken, L"EXPLORER.EXE"))
		{
			PROCESS_INFORMATION pi = { 0 };
			SECURITY_ATTRIBUTES sa = { 0 };
			sa.nLength = sizeof( SECURITY_ATTRIBUTES ); 
			STARTUPINFO si = { 0 };
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.cb = sizeof( STARTUPINFO );
			si.wShowWindow = SW_SHOW;

			SetLastError(0);
			ret = CreateProcessAsUser( hToken, hostpath, NULL, &sa, &sa, FALSE, 
				NORMAL_PRIORITY_CLASS| CREATE_NEW_CONSOLE, NULL, path, &si, &pi );

		}
		OutputDebugString(L"TepServer: StartTrayHost out !!");
		return ret;
	}

	BOOL StartTrayHostV2()
	{
		BOOL ret = FALSE;
		HANDLE hToken = NULL;
		HANDLE hTokenDup = NULL;
		LPVOID pEnv = NULL;
		CString path = CMiscHelper::GetAppPath() + TRAYHOST_NAME;

		do 
		{
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
				break;

			if(!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
				break;
			
			DWORD dwSessionId = WTSGetActiveConsoleSessionId();
			
			if(!SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD)))
				break;

			// 设置登录身份
			//{
			//	PTOKEN_USER pexpTokenUser=NULL;
			//	DWORD size = 0;
			//	HANDLE expToken = CProcessHelper::DuplicateUserToken(CProcessHelper::GetConsoleShellPid());
			//	if(!GetTokenInformation(expToken , TokenUser , NULL , 0 , &size))
			//	{
			//		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			//			break;
			//		pexpTokenUser = (PTOKEN_USER)new UCHAR[size];
			//		if(!pexpTokenUser)
			//			break;
			//		if(GetTokenInformation(expToken , TokenUser , pexpTokenUser , size , &size))
			//		{
			//			SetLastError(0);
			//			BOOL SSSS = SetTokenInformation(expToken, TokenUser, pexpTokenUser, size);
			//			DWORD dwssss = GetLastError(); //87  
			//			SSSS = SetTokenInformation(hTokenDup, TokenUser, pexpTokenUser, size);

			//			dwssss = GetLastError();


			//			CString siddd = CSecurityHelper::SID2String(pexpTokenUser->User.Sid);
			//			size = MAX_PATH;
			//			WCHAR  wzPath[MAX_PATH];
			//			WCHAR  wzDomain[MAX_PATH];
			//			SID_NAME_USE SidType;
			//			if(LookupAccountSid(NULL , pexpTokenUser->User.Sid , wzPath , &size , wzDomain , &size , &SidType))
			//			{
			//				CString user = (LPCWSTR)wzPath;
			//				CString Domain = wzDomain;
			//			}
			//		}
			//		//delete[] (PUCHAR)pexpTokenUser;
			//	}




// 				PTOKEN_USER pTokenUser=NULL;
// 				if(!GetTokenInformation(hTokenDup , TokenUser , NULL , 0 , &size))
// 				{
// 					if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
// 						break;
// 					pTokenUser = (PTOKEN_USER)new UCHAR[size];
// 					if(!pTokenUser)
// 						break;
// 					if(GetTokenInformation(hTokenDup , TokenUser , pTokenUser , size , &size))
// 					{
// 						CString siddd = CSecurityHelper::SID2String(pTokenUser->User.Sid);
// 						size = MAX_PATH;
// 						WCHAR  wzPath[MAX_PATH];
// 						WCHAR  wzDomain[MAX_PATH];
// 						SID_NAME_USE SidType;
// 						if(LookupAccountSid(NULL , pTokenUser->User.Sid , wzPath , &size , wzDomain , &size , &SidType))
// 						{
// 							CString user = (LPCWSTR)wzPath;
// 							CString Domain = wzDomain;
// 						}
// 					}
// 					delete[] (PUCHAR)pTokenUser;
// 				}
// 			}


			STARTUPINFO si;
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb			= sizeof(STARTUPINFO);
			si.lpDesktop	= _T("WinSta0\\Default");
			si.wShowWindow	= SW_SHOW;
			si.dwFlags		= STARTF_USESHOWWINDOW;
			
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));

			DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
			if(!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
				break;

			ret  = CreateProcessAsUser(hTokenDup, NULL, path.GetBuffer(), NULL, NULL, FALSE, dwCreationFlag, pEnv, NULL, &si, &pi);

		} while (FALSE);

		if(pEnv)
			DestroyEnvironmentBlock(pEnv);
		if(hTokenDup != NULL && hTokenDup != INVALID_HANDLE_VALUE)  
			CloseHandle(hTokenDup);  
		if(hToken != NULL && hToken != INVALID_HANDLE_VALUE)  
			CloseHandle(hToken); 

		return ret;
	}
private:
	void Run()
	{
		do 
		{
			if (!FindProcess(TRAYHOST_NAME))
			{
				if(!StartTrayHostV2())
				{
					OutputDebugString(L"客户端启动失败！！！");
				}
			}	  
		} while (!WaitStopNotify(5000));
	}
private:
	CTrayHostMgr(){}
};
