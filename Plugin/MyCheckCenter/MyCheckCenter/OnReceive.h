#pragma once
#include "stdafx.h"

class CMainFrame : public IMsgReceiver
{
public:
	explicit CMainFrame(IMsgBus* msgbus);
	~CMainFrame();
public:
	//
	BOOL OnStartup();
	void OnStop();
	virtual ULONG OnReceive(CStringW&);

	//
	virtual void OnPost(CStringW& msg){}
	virtual LPCWSTR GetFilter()
	{
		return L"MyCheckCenter";
	}
	virtual LPCWSTR GetName()
	{
		return L"MyCheckCenter";
	}
public:
	IMsgBus* m_bus;
};