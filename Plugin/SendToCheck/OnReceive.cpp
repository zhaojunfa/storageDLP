#include "stdafx.h"
#include "OnReceive.h"
#include "SendToCheck.h"
#include "DirectoryChange.h"

CMainFrame::CMainFrame(IMsgBus* msgbus)
{
	m_bus = msgbus;
}
CMainFrame::~CMainFrame(void)
{
}

ULONG CMainFrame::OnReceive(CStringW& msg)
{
	SetInfo(msg);
	SetScanDir(msg);
	return 1;
}


BOOL CMainFrame::OnStartup()
{
	OutputDebugString(L"SendtoCheck Begin !!!!");
	if (OnStart())
	{
		SetScanInfo();
		//Drive_File_Watchchange();
		StartRead();
	}
	return TRUE;
}

void CMainFrame::OnStop(void)
{
	FileWatchRelease();

	OnStopCheck();
}