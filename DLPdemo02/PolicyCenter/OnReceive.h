#pragma once
#include "stdafx.h"

class CMainFrame :public IMsgReceiver,public IActiveObject
{
public:
	explicit CMainFrame(IMsgBus* MsgBus);
	~CMainFrame();

	BOOL OnStartup();
	void OnStop();
	void Run();
	//void UpdatePolicy();

	BOOL UpdatePolicyV2();
	virtual DWORD OnReceive(CStringW& msg);
	


	virtual void OnPost(CStringW& msg){}
	
	virtual LPCWSTR GetFilter()
	{
		return L"PolicyCenter";
	}

	virtual LPCWSTR GetName()
	{
		return L"PolicyCenter";
	}

public:
	IMsgBus* m_bus;
};