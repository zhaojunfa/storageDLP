//DlpDirectoryChange.cpp : 定义 DLL 应用程序的导出函数。


#include "stdafx.h"
#include <Windows.h>
#include "DirectoryChange.h"
#include "SendToCheck.h"
#include "../../include/db/CStorageFileTable.h"
#include "../../../TEPCode/interface/xmlmsg/XmlMessage.h"

CString g_WatchDirectory;
HANDLE	g_WatchThreadHandle = INVALID_HANDLE_VALUE;
HANDLE	g_WatchDirHandle = INVALID_HANDLE_VALUE;

void SetScanInfo()
{
	g_WatchDirectory = CMiscHelper::RegGetString(L"storageDLP\\ScanDir");
}

ULONG SetScanDir(CStringW& msg)
{
	CString strType;
	CXmlMessage xm;
	if(S_OK != xm.Load(msg))
		return 0;
	strType = xm.GetType();
	if(strType.CompareNoCase(L"UpdateScanDir") == 0)
	{
		CString Temp = xm.GetValue(L"./policy/scandir");
		Temp.Replace(L"\n",L";");
		Temp = Temp +L";";
		if (-1 != Temp.Find(_T(";")))
		{
			while(TRUE)
			{
				int index =Temp.Find(_T(";"));
				if (index == -1)
				{
						break;
				}
				CString Dir = Temp.Left(index);
				if (FindLocalIp(Dir))
				{
					OutputDebugString(L"scanpath out:"+Dir);
					if (PathIsDirectory(Dir))
					{
						OutputDebugString(L"scanpath set success:"+Dir);
						xm.AddTail(L".", L"result", 1);	
						CMiscHelper::RegSetString(L"storageDLP\\scandir", Dir);
						xm.Save(msg);
						g_WatchDirectory = Dir;
						Drive_File_Watchchange();
						break;
					}
				}		
				Temp = Temp.Right(Temp.GetLength()-index-1);
			}
		}
		else
		{
			if (FindLocalIp(Temp))
			{
				if (PathIsDirectory(Temp))
				{
					xm.AddTail(L".", L"result", 1);	
					CMiscHelper::RegSetString(L"storageDLP\\ScanDir", Temp);
					xm.Save(msg);
					g_WatchDirectory = Temp;
					Drive_File_Watchchange();
				}	
			}		
		}
	}
	else if (strType.CompareNoCase(L"deleteisolate") == 0)
	{
		CString guid = xm.GetValue(L"./guid");
		if (!guid.IsEmpty())
		{
			CStorageIsolateTableInstance()->DeleteRecord();

			xm.AddTail(L".", L"result", 1);	
			xm.Save(msg);
		}
			
	}
	else if (strType.CompareNoCase(L"ScanStop") == 0)
	{
		OutputDebugString(L"Policy Stop Scan!!");
		FileWatchRelease();
	}
	return 1;
}
void FileWatchRelease()
{
	if(g_WatchThreadHandle != INVALID_HANDLE_VALUE)
	{
		TerminateThread(g_WatchThreadHandle, EXIT_SUCCESS);
		CMiscHelper::RegSetString(L"storageDLP\\ScanStatus",L"1");
		OutputDebugString(L"Scan is Stop!!");
		CloseHandle(g_WatchThreadHandle);
		if(g_WatchDirHandle != INVALID_HANDLE_VALUE)
			CloseHandle(g_WatchDirHandle);	
	}
}
void Drive_File_Watchchange( )
{ 
	OutputDebugString(L"mpz:Drive_File_Watchchange");
	//OutputDebugString(L"mpz:"+ g_WatchDirectory);

	FileWatchRelease();
	g_WatchThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WatchChanges,NULL,0,NULL);

}


VOID InsertCheckTable(CString dir, FILE_NOTIFY_INFORMATION* pNextNotification)
{
  
	CString strNotifyName = pNextNotification->FileName;
	CString strDetectFileName = strNotifyName.Left( pNextNotification->FileNameLength / 2 );

	/*****送检*******/
	CString watchDirectory ( dir );
	CString FilePath=watchDirectory+L"\\"+strDetectFileName;	
	
	if (PathIsDirectory(FilePath))
	{
		enumpath(FilePath);
	}
	else
	{
		CString strTime = CTime::GetCurrentTime().Format(L"%c");
		StorageFile ChangeBuf;
		ChangeBuf.FileName = strDetectFileName;
		ChangeBuf.FilePath = FilePath;
		ChangeBuf.Guid  = CStorageFileTableInstance()->NewGuid();
		ChangeBuf.StorageTime = strTime;
		CStorageFileTableInstance()->UpdateRecord(ChangeBuf);
		OutputDebugString(L"path is to SQL :"+FilePath);
	}
}
DWORD WINAPI WatchChanges( LPVOID lpParam )
{
	OutputDebugString(L"Scan is Start"+g_WatchDirectory);
	enumpath(g_WatchDirectory);
	CString tempWatchDir;
	tempWatchDir = g_WatchDirectory;

	if(tempWatchDir.IsEmpty())
	{
		OutputDebugString(L"Scan Path is Empty!!!");
		return 1;
	}
	
	CMiscHelper::RegSetString(L"storageDLP\\ScanStatus",L"0");
	char notify[10240] = {0};
	FILE_NOTIFY_INFORMATION * pNotification=(FILE_NOTIFY_INFORMATION*)notify;
	
	//创建一个目录句柄
	g_WatchDirHandle = CreateFile(
		tempWatchDir, //指向监测目录
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
		);

	if (g_WatchDirHandle == INVALID_HANDLE_VALUE )
	{
		DWORD ERROR_DIR = GetLastError();
		OutputDebugString(L"打开目录失败");
		return 1;
	}
	DWORD bytes = 0;
	BOOL watch_state = FALSE;
	CString FilePath;//最终获取的改变文件的完整路径



	DWORD nBufferLength = 10240 ;

	while(1)
	{
		memset(notify,0,10240);
		bytes = 0;
		watch_state = ReadDirectoryChangesW(
			g_WatchDirHandle,  
			(LPVOID)pNotification,    
			nBufferLength,  
			TRUE,  
			FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_LAST_ACCESS|FILE_NOTIFY_CHANGE_SECURITY|
			FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_DIR_NAME,
			(LPDWORD)&bytes,
			NULL,  
			NULL);

		int iNeedDetect = 1 ;
		CString strNotifyName ;
		CString strDetectFileName;

		//返回值为1，说明函数已经成功执行
		if (GetLastError() == ERROR_INVALID_FUNCTION) //系统不支持
			continue;
		else if (watch_state == 0) //监控失败
			continue;
		else if (GetLastError() == ERROR_NOTIFY_ENUM_DIR) // 内存溢出
			continue;
		else
		{

			FILE_NOTIFY_INFORMATION* pNextNotification = NULL;
 			switch(pNotification->Action)
 			{
//  			case FILE_ACTION_RENAMED_OLD_NAME://重命名
//  				{
// 
//  					FILE_NOTIFY_INFORMATION* pNextNotification = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotification + pNotification->NextEntryOffset);  
//  					strNotifyName = pNextNotification->FileName;
//  					strDetectFileName = strNotifyName.Left( pNextNotification->FileNameLength / 2 );		
// 					break;
// 
//  				}
//  			case FILE_ACTION_MODIFIED://修改文件
//  				{
// 
//  					strNotifyName = pNotification->FileName;
//  					strDetectFileName = strNotifyName.Left( pNotification->FileNameLength / 2 );
//  					break;
//  				}
 			case FILE_ACTION_ADDED:
 				{
					FILE_NOTIFY_INFORMATION* pNextNotification = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotification + pNotification->NextEntryOffset);
					if (pNextNotification->Action == 1)
						InsertCheckTable(tempWatchDir, pNextNotification);
 					break;
 				}	
			case  FILE_ACTION_MODIFIED:
				{
					pNextNotification = pNotification;
					while (pNextNotification->NextEntryOffset > 0)
					{
						if (pNextNotification->Action == 1)
							InsertCheckTable(tempWatchDir, pNextNotification);

						pNextNotification = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNextNotification + pNextNotification->NextEntryOffset);
					}
				}	
 			default:
 				{
 					iNeedDetect = 0 ;
 					break;
 				}
 			}
 		}	
 
 		if( 0==iNeedDetect )
 		{
 			continue;
 		}

		

	}	
//	delete [] lpParam;
}

static BOOL IsRoot(LPCTSTR lpszPath)
{
	TCHAR szRoot[460];
	wsprintf(szRoot, L"%c:\\", lpszPath[0]);
	return (lstrcmp(szRoot, lpszPath) == 0);
}

BOOL enumpath(CString lpPath)
{
	TCHAR szFind[MAX_PATH];
	lstrcpy(szFind, lpPath);
	if (!IsRoot(szFind))
		lstrcat(szFind, L"\\");
	lstrcat(szFind, L"*.*"); // 找所有文件
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(szFind, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return FALSE;

	do
	{
		if (wfd.cFileName[0] == '.')
			continue; // 过滤这两个目录
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR szFile[MAX_PATH];
			if (IsRoot(lpPath))
				wsprintf(szFile, L"%s%s", lpPath, wfd.cFileName);
			else
				wsprintf(szFile, L"%s\\%s", lpPath, wfd.cFileName);
			enumpath(szFile); // 如果找到的是目录，则进入此目录进行递归
		}
		else
		{
			TCHAR szFile[MAX_PATH];
			if (IsRoot(lpPath))
				wsprintf(szFile, L"%s%s", lpPath, wfd.cFileName);
			else
				wsprintf(szFile, L"%s\\%s", lpPath, wfd.cFileName);

			//判断文件是否在被操作
			HANDLE isopen = CreateFile(szFile, 
				GENERIC_READ,  
				0,  
				NULL,  
				OPEN_EXISTING,  
				NULL,  
				NULL);
			if ( INVALID_HANDLE_VALUE == isopen)
			{
				continue;
			}
			CloseHandle(isopen);
			// 对文件进行操作
			CString strTime = CTime::GetCurrentTime().Format(L"%c");
			StorageFile ChangeBuf;
			ChangeBuf.FileName = wfd.cFileName;
			ChangeBuf.FilePath = szFile;
			ChangeBuf.Guid  = CStorageFileTableInstance()->NewGuid();
			ChangeBuf.StorageTime = strTime;
			CStorageFileTableInstance()->UpdateRecord(ChangeBuf);
			OutputDebugString(L"path add SQL:"+ChangeBuf.FilePath);
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind); // 关闭查找句柄
	return TRUE;
}




