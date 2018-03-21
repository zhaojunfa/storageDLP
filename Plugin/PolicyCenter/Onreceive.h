#pragma once
#include "stdafx.h"

class CMainFrame :public IMsgReceiver, public IActiveObject
{
public:
	explicit CMainFrame(IMsgBus*);
	~CMainFrame(void);

public:
	BOOL OnStartup(void);
	void OnStop(void);

	virtual ULONG OnReceive(CStringW&);

	virtual void OnPost(CStringW& msg){};

	virtual LPCWSTR	GetFilter()
	{
		return L"PolicyCenter";
	}

	virtual LPCWSTR	GetName()
	{
		return L"PolicyCenter";
	}

	void Run();

	void UpdataPolicy();

public:
	IMsgBus* m_bus;
};