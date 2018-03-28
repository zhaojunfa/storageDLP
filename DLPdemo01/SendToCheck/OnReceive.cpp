#include "stdafx.h"
#include "OnReceive.h"
#include "SendToCheck.h"
#include "DirectoryChange.h"

CMainFrame::CMainFrame(IMsgBus* msgbus):m_bus(msgbus){}

CMainFrame::~CMainFrame(void){}

ULONG CMainFrame::OnReceive(CStringW& msg)
{
	SetInfo(msg);
	SetScanDir(msg);
	return 1;
}


BOOL CMainFrame::OnStartup()
{
	OutputDebugString(L"SendtoCheck CMainFrame::OnStartup Begin,this->OnStart\n");
	if (OnStart())//create thread Run;
	{
		//SetScanInfo();//get scan path from reg;watchchanges;
		//Drive_File_Watchchange();//create thread watch_changes;
		//StartRead();//create thread set_list
	}
	return TRUE;
}

void CMainFrame::OnStop(void)
{
	FileWatchRelease();

	OnStopCheck();
}