#include "stdafx.h"
#include "OnReceive.h"
#include "MyCheckCenter.h"

CMainFrame::CMainFrame(IMsgBus* msgbus):m_bus(msgbus){}

CMainFrame::~CMainFrame(){}

ULONG CMainFrame::OnReceive(CStringW& msg)
{
	//
	return 1;
}

BOOL CMainFrame::OnStartup(){
	if(OnStart())
	{

	}
	return TRUE;
}

void CMainFrame::OnStop()
{

}