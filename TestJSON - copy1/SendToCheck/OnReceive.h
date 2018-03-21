#pragma once
#include "stdafx.h"

class CMainFrame : public IMsgReceiver
{
public:
	explicit CMainFrame(IMsgBus* msgbus);
	~CMainFrame(void);

public:
	BOOL OnStartup(void);
	void OnStop(void);

	virtual ULONG OnReceive(CStringW&);

	virtual void OnPost(CStringW& msg){};

	virtual LPCWSTR	GetFilter()
	{
		return L"SendToCheck";
	}

	virtual LPCWSTR	GetName()
	{
		return L"SendToCheck";
	}

public:
	IMsgBus* m_bus;
};
