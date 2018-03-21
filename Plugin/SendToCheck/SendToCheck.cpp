// SendToCheck.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SendToCheck.h"
#include "iostream"
#include "queue"
#include "imagehlp.h"
#include "DirectoryChange.h"
using namespace std;

HANDLE				g_pipingEvent;		// 消息队列处理信号,		此信号最大等待时间为10s， 增加容错率
HANDLE              g_setlist;
CLiShareLock		g_Lock;				// 消息队列锁
queue<StorageFile>	g_piping;			// 消息队列
int g_iqueuelen = 0 ;

HANDLE		handle[64];
int			g_THREAD_NUM;

CString     g_LocalIp;
CString     g_CompterName;
CString		g_serverip;
CString		g_maxnummer;
CString		g_isolationDir;
CString		g_safeDir;
CString		g_scanDir;
void *		g_pServiceHandle = NULL;
HANDLE	g_ReadSQLHandle = INVALID_HANDLE_VALUE;

BOOL FindLocalIp(CString &Infomation)
{
	CString Ip;
	CString Info;
	Info = Infomation;
	Ip = CMiscHelper::RegGetString(L"storageDLP\\LocalIp");
	OutputDebugString(L"FindIp:"+Info+L"--Ip:"+Ip);
	while(TRUE)
	{
		int index =Ip.Find(_T(";"));
		if (index == -1)
		{
			return FALSE;
		}
		CString Dir = Ip.Left(index);
		if (Info.Find(Dir) == 2)
		{
			g_LocalIp = Dir;
			CMiscHelper::RegSetString(L"storageDLP\\CurrentIP", g_LocalIp);
			Infomation =Info.Right(Info.GetLength()-Dir.GetLength()-3);
			OutputDebugString(L"SendToCheck PathIn:"+Infomation);
			return TRUE;
		}
		Ip = Ip.Right(Ip.GetLength()-index-1);
	}
}

BOOL GetTokenByName(HANDLE &hToken,LPTSTR lpName)  
{  
	if(!lpName)  
	{  
		return FALSE;  
	}  
	HANDLE         hProcessSnap = NULL;   
	BOOL           bRet      = FALSE;   
	PROCESSENTRY32 pe32      = {0};   

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
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

CString GetCurrentUserName()  
{  
	HANDLE hToken;  

	//得到shell的token  
	if(!GetTokenByName(hToken,_T("EXPLORER.EXE")))  
	{  
		return FALSE;  
	}  
	DWORD        cbti     = 0;  
	PTOKEN_USER  ptiUser  = NULL;  
	SID_NAME_USE snu;  

	//取得所需空间大小  
	if (GetTokenInformation(hToken, TokenUser, NULL, 0, &cbti))   
	{  
		DWORD error = GetLastError();
		CloseHandle(hToken);  
		return FALSE;  
	}  

	//分配空间  
	ptiUser = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbti);  
	if(!ptiUser)  
	{  
		CloseHandle(hToken);  
		return FALSE;  
	}  

	//取得token信息  
	if (!GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti))  
	{  
		CloseHandle(hToken);  
		HeapFree(GetProcessHeap(), 0, ptiUser);
		return FALSE;  
	}  

	TCHAR szUser[50] = _T("");  
	TCHAR szDomain[50] = _T("");  
	DWORD nUser = 50;  
	DWORD nDomain = 50;  

	//根据用户的sid得到用户名和domain  

	if (!LookupAccountSid(NULL, ptiUser->User.Sid, szUser, &nUser,   
		szDomain, &nDomain, &snu))  
	{  
		CloseHandle(hToken);  
		HeapFree(GetProcessHeap(), 0, ptiUser);  
		return FALSE;  
	}   
	CloseHandle(hToken); 
	HeapFree(GetProcessHeap(), 0, ptiUser);  
	CString ComputerName = szDomain;
	CString ComputerUser = szUser; 
	CString ComputerUserName = ComputerName + L"/"+ComputerUser; 
	return ComputerUserName;  
}  

ULONG SetInfo(CStringW& msg)
{
	CString		strType;
	CXmlMessage xm;

	if(S_OK != xm.Load(msg))
		return 0;
	strType = xm.GetType();
	if(strType.CompareNoCase(L"UpdateSeverIp") == 0)
	{
		CString temp = xm.GetValue(L"./policy/serverip");
		if (CMiscHelper::RegSetString(L"storageDLP\\CheckIP", temp))
		{
			xm.AddTail(L".", L"result", 1);
			g_serverip = temp;
		}
		xm.Save(msg);
	}
	else if(strType.CompareNoCase(L"UpdateMaxNummer") == 0)
	{
		CString number = xm.GetValue(L"./policy/maxthread");
		int num=_ttoi(number);
		if (num <= 64 && num > 0)
		{
			xm.AddTail(L".", L"result", 1);	
			CMiscHelper::RegSetString(L"storageDLP\\MaxThreads", number);
		}
		xm.Save(msg);
	}
	else if(strType.CompareNoCase(L"UpdateIsolationDir") == 0)
	{
		CString temp = xm.GetValue(L"./policy/isolationdir");
		temp =temp +L";";
		int i =0;
		if (-1 != temp.Find(L";"))
		{
			while(TRUE)
			{
				int index = temp.Find(_T(";"));
				if (index == -1)
				{
					break;
				}
				CString Dir = temp.Left(index);
				if (FindLocalIp(Dir))
				{
					OutputDebugString(L"SendToCheck:pathout"+Dir);
					if (PathIsDirectory(Dir))
					{
						xm.AddTail(L".", L"result", 1);	
						CMiscHelper::RegSetString(L"storageDLP\\IsolationDir", Dir);
						xm.Save(msg);
						g_Lock.Lock();
						g_isolationDir = Dir;
						g_Lock.Unlock();
						break;
					}			
				}	
				temp = temp.Right(temp.GetLength()-index-1);
			}
		}
		else
		{
			if (FindLocalIp(temp))
			{
				if (PathIsDirectory(temp))
				{
					xm.AddTail(L".", L"result", 1);	
					CMiscHelper::RegSetString(L"storageDLP\\IsolationDir", temp);
					xm.Save(msg);
					g_Lock.Lock();
					g_isolationDir = temp;
					g_Lock.Unlock();
				}
			}		
		}	
	}
	else if(strType.CompareNoCase(L"UpdateSafeDir") == 0)
	{
		CString temp = xm.GetValue(L"./policy/safedir");
		int i =0;
		if (-1 != temp.Find(L";"))
		{
			while(TRUE)
			{
				int index =temp.Find(_T(";"));
				if (index == -1)
				{
					break;
				}
				CString Dir = temp.Left(index);
				if (FindLocalIp(Dir))
				{
					OutputDebugString(L"safepath out:"+Dir);
					if (PathIsDirectory(Dir))
					{
						xm.AddTail(L".", L"result", 1);	
						CMiscHelper::RegSetString(L"storageDLP\\SafeDir", Dir);
						xm.Save(msg);
						g_Lock.Lock();
						g_safeDir = Dir;
						g_Lock.Unlock();
						break;
					}
				}
				temp = temp.Right(temp.GetLength()-index-1);
			}
		}
		else
		{
			if (FindLocalIp(temp))
			{
				if (PathIsDirectory(temp))
				{
					xm.AddTail(L".", L"result", 1);	
					CMiscHelper::RegSetString(L"storageDLP\\SafeDir", temp);
					xm.Save(msg);
					g_Lock.Lock();
					g_safeDir = temp;
					g_Lock.Unlock();
				}
			}		
		}
	}
	else if (strType.CompareNoCase(L"CleanIsoDir") == 0)
	{
		g_Lock.Lock();
		CMiscHelper::RegSetString(L"storageDLP\\IsolationDir", L"");
		g_isolationDir =L"";
		xm.AddTail(L".", L"result", 1);
		xm.Save(msg);
		g_Lock.Unlock();
	}
	else if (strType.CompareNoCase(L"CleanSafeDir") == 0)
	{
		g_Lock.Lock();
		CMiscHelper::RegSetString(L"storageDLP\\SafeDir", L"");
		g_safeDir =L"";
		xm.AddTail(L".", L"result", 1);
		xm.Save(msg);
		g_Lock.Unlock();
	}
	return 1;
}

void GetIpInfo()
{
	CString IpInfo;
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	BYTE * pBInfo = NULL;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pBInfo = new BYTE[stSize];
		pIpAdapterInfo = (PIP_ADAPTER_INFO)pBInfo;
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
	}
	if (ERROR_SUCCESS == nRel)
	{
		//可能网卡有多IP,因此通过循环去判断
		IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
		do 
		{
			CString Ip( pIpAddrString->IpAddress.String);
			IpInfo = IpInfo + Ip + L";";
			pIpAddrString=pIpAddrString->Next;
		} while (pIpAddrString);
		pIpAdapterInfo = pIpAdapterInfo->Next;
	}
	CMiscHelper::RegSetString(L"storageDLP\\LocalIp",IpInfo);
	OutputDebugString(L"SendToCheck:"+IpInfo);
	//释放内存空间
	if (pBInfo != NULL)
	{
		delete[] pBInfo;
		pBInfo=NULL;
	}
	OutputDebugString(L" getip info end");
	return;
}


DWORD WINAPI SetList(LPVOID lpParam)
{	
	do 
	{
		g_Lock.Lock();
		if( g_iqueuelen<= 0 )
		{
			CAtlList<StorageFile>recordList;
			if (CStorageFileTableInstance()->QueryRecord(recordList) && !recordList.IsEmpty())
			{
				POSITION pos = recordList.GetHeadPosition();
				g_iqueuelen = recordList.GetCount();
				while (pos)
				{
					StorageFile record = recordList.GetNext(pos);

					g_piping.push(record);
					ReleaseSemaphore(g_pipingEvent, 1, NULL);
				}
				recordList.RemoveAll();	
			}
			else
			{
				enumpath(g_scanDir);
			}
		}

		g_Lock.Unlock();
		Sleep(10 * 1000);
		

	} while ( 1 );
	int i =GetLastError();
	CString err;
	err.Format(L"setlist err is :%d",i);
	OutputDebugString(err);
	CMiscHelper::RegSetString(L"storageDLP\\errmag",err);
	Sleep(5*1000);
	StartRead();
	return 1;
}

void StartRead()
{
	g_ReadSQLHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SetList,NULL,0,NULL);
}

BOOL OnStart()
{
	//g_pipingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_pipingEvent = CreateSemaphore(NULL, 0, 1000, (LPCWSTR)"Global\\streamSemaphore");
	// 初始化配置
	g_serverip = CMiscHelper::RegGetString(L"storageDLP\\CheckIP");
	g_maxnummer = CMiscHelper::RegGetString(L"storageDLP\\MaxThreads",L"1");
	g_isolationDir = CMiscHelper::RegGetString(L"storageDLP\\IsolationDir");
	g_safeDir = CMiscHelper::RegGetString(L"storageDLP\\SafeDir");
	g_scanDir = CMiscHelper::RegGetString(L"storageDLP\\ScanDir");
	CMiscHelper::RegSetString(L"storageDLP\\version",L"0");
	g_THREAD_NUM = _ttoi(g_maxnummer);
	g_LocalIp = CMiscHelper::RegGetString(L"storageDLP\\CurrentIP",L"0.0.0.0");
	OutputDebugString(g_LocalIp);
	StorageFile ChangeBuf;
	ChangeBuf.FileName = L"1";
	ChangeBuf.FilePath = L"";
	ChangeBuf.Guid  = CStorageFileTableInstance()->NewGuid();
	ChangeBuf.StorageTime = L"1";
	CStorageFileTableInstance()->UpdateRecord(ChangeBuf);
	// 初始化检测	
	int iRet =  InitDlpRpcEnv();
	int len = 0;
	char *sendmsg = QXUnicode2Utf8(g_serverip,len);
	iRet = InitDlpRpcConnection( sendmsg ,1344 ,"storeagentreq" , g_THREAD_NUM ,&g_pServiceHandle );
	if(sendmsg)
		delete sendmsg;
	CString CptName  = GetCurrentUserName();
	CMiscHelper::RegSetString(L"storageDLP\\CompterName",CptName);
	g_CompterName = CptName;
	GetIpInfo();
	if (iRet != 0)
	{
		MessageBox(NULL,L"警告",L"服务器地址错误",MB_OK);
		return FALSE;
	}
	BeginCheck();
	return TRUE;
}


// 判断目录是否存在  
bool FolderExists(CString s)  
{  
	DWORD attr;   
	attr = GetFileAttributes(s);   
	return (attr != (DWORD)(-1) ) &&  
		( attr & FILE_ATTRIBUTE_DIRECTORY);   
}  

// 递归创建目录， 如果目录已经存在或者创建成功返回TRUE  
static bool SuperMkDir(CString P)  
{  
	int len=P.GetLength();  
	if ( len <2 ) return false;   

	if('\\'==P[len-1])  
	{  
		P=P.Left(len-1);  
		len=P.GetLength();  
	}  
	if ( len <=0 ) return false;  

	if (len <=3)   
	{  
		if (FolderExists(P))return true;  
		else return false;   
	}  

	if (FolderExists(P))return true;  



	CString Parent;  
	Parent=P.Left(P.ReverseFind('\\') );  

	if(Parent.GetLength()<=0)return false;   

	bool Ret=SuperMkDir(Parent);   

	if(Ret)   
	{  
		SECURITY_ATTRIBUTES sa;  
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);  
		sa.lpSecurityDescriptor=NULL;  
		sa.bInheritHandle=0;  
		Ret=(CreateDirectory(P,&sa)==TRUE);  
		return Ret;  
	}  
	else  
		return false;  
}  

void MoveSafeDir(PStorageFile record)
{
	if (g_safeDir.IsEmpty())
	{
		return;
	}
	CString safeDir;
	safeDir = g_safeDir;
	OutputDebugString(L"movesafeDir :"+record->FileName);
	g_Lock.Lock();
	CString temp;
	if (record->FilePath.Left(record->FilePath.ReverseFind('\\')).GetLength() == g_scanDir.GetLength())
	{
		temp = safeDir+ L"\\" + record->FileName;
	}
	else
	{
		temp = safeDir+record->FilePath.Right(record->FilePath.GetLength()-g_scanDir.GetLength());
	}
	if (0 !=MoveFile(record->FilePath, temp ))
	{
		CStorageFileTableInstance()->DeleteRecord(record->Guid);
	}
	else 
	{
		for (int i =0 ;TRUE ; i++)
		{
			if (3 == GetLastError())
			{
				CString buf =temp.Left(temp.ReverseFind('\\')); 
				SuperMkDir(buf);
				continue;
			}
			CString tempnum;
			CString changename ;	
			tempnum.Format(L"(%d)",i);
			changename = temp.Left(temp.ReverseFind('.'))+tempnum + temp.Right(temp.GetLength()-temp.ReverseFind('.'));		
//			temp = safeDir+ L"\\" + tempnum+record->FileName;
			if (0 !=MoveFile(record->FilePath, changename ))
			{
				CStorageFileTableInstance()->DeleteRecord(record->Guid);
				break;
			}
			if (183 !=GetLastError() )
			{
				CStorageFileTableInstance()->DeleteRecord(record->Guid);
				break;
			}
		}
	}
	g_Lock.Unlock();
}

void MoveIsolationDir(PStorageFile record)
{
	if (g_isolationDir.IsEmpty())
	{
		return;
	}
	CString isolationDir;
	isolationDir = g_isolationDir;
	OutputDebugString(L"moveisolationDir:"+record->FileName);
	g_Lock.Lock();
	CString temp;
	if (record->FilePath.Left(record->FilePath.ReverseFind('\\')).GetLength() == g_scanDir.GetLength())
	{
		temp = isolationDir+ L"\\" + record->FileName;
	}
	else
	{
		temp = isolationDir+record->FilePath.Right(record->FilePath.GetLength()-g_scanDir.GetLength());
	}
	if (0 !=MoveFile(record->FilePath, temp ))
	{
		CStorageFileTableInstance()->DeleteRecord(record->Guid);
		StorageIsoalte isorecord;
		CString strTime = CTime::GetCurrentTime().Format(L"%c");
		isorecord.Guid = record->Guid;
		isorecord.FileName = record->FileName;
		isorecord.FilePath = record->FilePath;
		isorecord.IsolatePath = temp;
		isorecord.IsolateTime = strTime;
		CStorageIsolateTableInstance()->UpdateRecord(isorecord);
	}
	else 
	{
		for (int i =0 ;TRUE ; i++)
		{	
			if (3 == GetLastError())
			{
				CString buf =temp.Left(temp.ReverseFind('\\')); 
				SuperMkDir(buf);
				continue;
			}
			CString tempnum;
			tempnum.Format(L"(%d)",i);
			CString changename = temp;
			changename = temp.Left(temp.ReverseFind('.'))+tempnum + temp.Right(temp.GetLength()-temp.ReverseFind('.'));
			if (0 !=MoveFile(record->FilePath, changename ))
			{
				CStorageFileTableInstance()->DeleteRecord(record->Guid);
				StorageIsoalte isorecord;
				CString strTime = CTime::GetCurrentTime().Format(L"%c");
				isorecord.Guid = record->Guid;
				isorecord.FileName = record->FileName;
				isorecord.FilePath = record->FilePath;
				isorecord.IsolatePath = changename;
				isorecord.IsolateTime = strTime;
				CStorageIsolateTableInstance()->UpdateRecord(isorecord);
				break;
			}
			if (183 !=GetLastError() )
			{
				CStorageFileTableInstance()->DeleteRecord(record->Guid);
				break;
			}
		}
	}
	g_Lock.Unlock();
}



void SetState(BOOL state)
{
	g_Lock.Lock();
	CString ConState;
	ConState = CMiscHelper::RegGetString(L"storageDLP\\ConnectionState");
	//0为正常 1为连接中断
	if (state && _ttoi(ConState) != 0)
	{
		CMiscHelper::RegSetString(L"storageDLP\\ConnectionState",L"0");
	}
	else if ((!state) && _ttoi(ConState) != 1)
	{
		CMiscHelper::RegSetString(L"storageDLP\\ConnectionState",L"1");
	}
	g_Lock.Unlock();
}

BOOL SendCheckFile(StorageFile record)
{
	BOOL bRet = FALSE;
	char* FileContent = NULL;
	CString CheckInfo;
	CString FileSize;
	CString TaskReault;

	char * pResult = NULL;
	void * pHandle = NULL;

	do 
	{
		//送检 
		int iRet =  0 ;
		iRet = GetAvilableConnection( g_pServiceHandle , &pHandle );
		if( iRet !=0 )
		{
			bRet = FALSE;
			break;
		}

		DWORD dwFileSize = 0;
	//	GetFileInfo(record.FilePath, FileContent, dwFileSize,record.Guid);
		HANDLE pfile = ::CreateFile(record.FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL , NULL);
		if(pfile == INVALID_HANDLE_VALUE)
		{
			CStorageFileTableInstance()->DeleteRecord(record.Guid);
			CloseHandle(pfile);
			break;;
		}
		dwFileSize = GetFileSize(pfile, NULL);
		if (dwFileSize > 30*1024*1024)
		{
			CloseHandle(pfile);//关闭句柄
			break;;
		}
		FileContent = new char[dwFileSize + 1];        //最后一位为'\0',C-Style字符串的结束符。
		DWORD readsize;
		ReadFile(pfile, FileContent, dwFileSize, &readsize, NULL);
		FileContent[dwFileSize] = 0;
		if (FileContent == NULL)
		{
			delete[] FileContent;   
			CloseHandle(pfile);//关闭句柄
			break;
		}
		OutputDebugString(L"sendtocheck path is :"+record.FileName);
		FileSize.Format(L"%d", dwFileSize);
		TaskReault = L"\r\nX-TaskResult: /topdlp/bin/detect-proxy/tmp/"+record.Guid+L".xml";

		CheckInfo = L"X-FileName: "+record.FileName+L"\r\nX-FileSize: "+FileSize+TaskReault+L"\r\nX-TaskId: "+record.Guid;
		CheckInfo = CheckInfo+L"\r\nX-RemoteDir: /topdlp/data/\r\nX-LocalDir: ./\r\nX-TargetRePath: unknow";
		CheckInfo = CheckInfo+L"\r\nX-CHECKTYPE: 10\r\nX-SENDNAME: unknow\r\nX-RECVNAME: unknow\r\n";
		CheckInfo = CheckInfo +L"X-CLIENTIP: "+g_LocalIp+L"\r\nX-USERNAME: "+g_CompterName+L"\r\n";
		int len = 0;
		int lenth =0;
		char *sendmsg = QXUnicode2Utf8(CheckInfo,len);
		//char *sendbuf = QXUnicode2Utf8(FileContent,lenth);
		DWORD tempL = strlen(sendmsg);
		iRet =  DlpRpcSendData( pHandle , sendmsg, tempL, NULL, 0 , FileContent , dwFileSize );
		if (sendmsg)
			delete sendmsg;
		if (FileContent)
			delete[] FileContent;   
		CloseHandle(pfile);//关闭句柄
		if( iRet == -1 )
		{
			bRet = FALSE;
			break;
		}	

		//获取检测结果
		char * pResult = NULL;
		int iResultLen = 0 ;
		int iResult = 0 ;
		while(1)
		{
			iRet = 0;
			iResult = 0;
			if(-1 == GetDlpResponseData(  pHandle ,( void ** ) &pResult , &iResultLen,  &iResult))
			{
				SetState(FALSE);
				bRet = FALSE;
				break;
			}
			else if( 204 == iResult)
			{
				SetState(TRUE);
				bRet = FALSE;
				break;
			}
			else if(206 == iResult)
			{
				SetState(TRUE);
				bRet = FALSE;
				break;
			}
			else if(200 == iResult)
			{	
				bRet = TRUE;	
				break;
			}
			else
			{
				RealeaseData( pResult) ;
				Sleep(50);
			}
		}

	} while (FALSE);

	//释放
	if (pResult)
		RealeaseData( pResult);
	if (pHandle)
		PutAvilableConnection( g_pServiceHandle , pHandle);

	return bRet;
}

unsigned int __stdcall Run(PVOID pM)
{
	OutputDebugString(L"check is begin!!");
	do
	{
		WaitForSingleObject(g_pipingEvent, INFINITE );

		StorageFile record;

		// 容错处理
		
		g_Lock.Lock();
		if (g_piping.empty())
		{	
			g_Lock.Unlock();
			Sleep(1000);
			continue;
		}
		else
		{
			record= g_piping.front();

			g_piping.pop();
			
		}
		g_Lock.Unlock();
		
		if (record.FilePath.IsEmpty())
		{
			CStorageFileTableInstance()->DeleteRecord(record.Guid);
			g_Lock.Lock();
			g_iqueuelen--;
			g_Lock.Unlock();
			continue;
		}

		if(SendCheckFile(record))
		{
			MoveIsolationDir(&record);
		}
		else
		{
			MoveSafeDir(&record);
		}

		g_Lock.Lock();
		g_iqueuelen--;
		g_Lock.Unlock();

	}while(TRUE);

	return 0;
}

void BeginCheck()
{  
	int temp = g_THREAD_NUM;
	for (int i = 0; i < temp; i++)  
		handle[i] = (HANDLE)_beginthreadex(NULL, 0, Run, NULL, 0, NULL); 
	return;
}

BOOL OnStopCheck()
{
	for (int i = 0; i < g_THREAD_NUM; i++)
	{
		if (handle[i] != INVALID_HANDLE_VALUE)
		{
			TerminateThread(handle[i], 0);
			CloseHandle( handle[i]); 
		}
	}

	if(g_pipingEvent != INVALID_HANDLE_VALUE)
		CloseHandle(g_pipingEvent);

	if (g_setlist != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_setlist);
	}
	if(g_ReadSQLHandle != INVALID_HANDLE_VALUE)
	{
		TerminateThread(g_ReadSQLHandle, EXIT_SUCCESS);
		CloseHandle(g_ReadSQLHandle);
	}
	return TRUE;
}