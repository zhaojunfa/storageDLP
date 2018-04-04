#pragma once
#include "C:\work\trunk\EndpointProduct\v2Code\code\include\msgbus\IMsgBus.h"
#include "C:\work\trunk\EndpointProduct\v2Code\code\include\msgbus\IMsgReceiver.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\kxml\kstr.h"

typedef HANDLE(* pStartup)(IMsgBus* bus);
typedef VOID(* pStop)(HANDLE hPlugin);


typedef struct _PLUGIN_INFO_
{
	CString filter;
	CString name;

	HMODULE	_hModle;

	IMsgReceiver*	_plugin;
}PluginInfo, *PPluginInfo;

CAtlMap<CString, PluginInfo> PluginMap;

class CTepMsgBus : public IMsgReceiver , public IMsgBus
{
public:
	CTepMsgBus(){InitLock();}
	~CTepMsgBus()
	{

		POSITION pos = PluginMap.GetStartPosition();
		while (pos)
		{
			PluginInfo info = PluginMap.GetNextValue(pos);

			pStop p = (pStop)GetProcAddress(info._hModle, "Stop");
			if (p)
				p((HANDLE)info._plugin);

			FreeLibrary(info._hModle);
		}

		PluginMap.RemoveAll();
		DeInitLock();
	}

private:
	CRITICAL_SECTION m_lock;
	void InitLock()
	{
		::InitializeCriticalSection(&m_lock);
	}
	void Lock()
	{
		::EnterCriticalSection(&m_lock);  
	}
	void UnLock()
	{
		::LeaveCriticalSection(&m_lock);
	}
	void DeInitLock()
	{
		::DeleteCriticalSection(&m_lock);
	}

public:
	virtual LPCWSTR	GetFilter(){return L"";};

	virtual LPCWSTR	GetName(){return L"";};

	virtual ULONG OnReceive(CStringW& msg){return 0;};

	virtual void OnPost(CStringW& msg){};
public:
	
	virtual VOID RegisterReceiver(IMsgReceiver* receiver)
	{
	}
	
	virtual VOID UnRegisterReceiver(IMsgReceiver* receiver)
	{
	}

	
	virtual ULONG Send(IMsgReceiver* sender , CStringW& xmsg)
	{
		Lock();
		CXmlMessage xmlsmg;
		if(xmlsmg.Load(xmsg) == S_OK)
		{
			CString filter = xmlsmg.GetTo();
			POSITION pos = PluginMap.GetStartPosition();
			PluginInfo info;
			while (pos)
			{
				info = PluginMap.GetNextValue(pos);
				if(KslStringMultiMatchW(filter, info.filter))
					//if (filter.Find(info.filter) != -1)
				{
					if (info._plugin)
					{
						info._plugin->OnReceive(xmsg);
					}
				}
			}
		}
		UnLock();

		return 1;
	}

	//投递消息
	virtual void Post(IMsgReceiver* sender , CStringW& xmsg)
	{
	}

public:
	// 完成信号
	VOID SendCompelete()
	{
		CXmlMessage xmlmsg;
		if(xmlmsg.Create(0, L"dlp.bus", L"*", L"SignalSync"))
		{
			OutputDebugString(L"完成信号创建失败\n");
			return ;
		}
		xmlmsg.AddTail(L".", L"Signals");
		xmlmsg.AddTail(L"./Signals", L"Signal", 1);
		xmlmsg.AddTail(L"./Signals/Signal", L"@name", L"Started");

		CString xml;
		xmlmsg.Save(xml);

		Send(NULL, xml);
	}

	
	VOID LoadAllPlugin()
	{
		CRegKey key;
		CString keyPath = L"Software\\Topsec\\StorageDLP\\Plugins";
		CString	valname;

		if(ERROR_SUCCESS != key.Open(HKEY_LOCAL_MACHINE , keyPath))
			return;

		WCHAR subKeyName[MAX_PATH];
		DWORD dwLength = MAX_PATH;

		for(int i = 0; key.EnumKey(i, subKeyName, &dwLength) != ERROR_NO_MORE_ITEMS; dwLength = MAX_PATH, i++)
		{
			CRegKey subKey;
			CString subKeyPath;
			TCHAR value[MAX_PATH] = {0};
			ULONG len;
			PluginInfo info = {0};
			//PluginInfo info;
			subKeyPath = keyPath+ L"\\" + subKeyName;
			if( ERROR_SUCCESS == subKey.Open(HKEY_LOCAL_MACHINE, subKeyPath))
			{
				len = MAX_PATH;
				ZeroMemory(value, MAX_PATH);

				if (subKey.QueryStringValue(L"Name",value, &len) == ERROR_SUCCESS)
				{
					info.name = value;
				}
				OutputDebugString(L"\nTepServer bus: LoadName：\n" + info.name);
				len = MAX_PATH;
				ZeroMemory(value, MAX_PATH);

				if (subKey.QueryStringValue(L"Path",value, &len) == ERROR_SUCCESS)
				{
					info._hModle = LoadLibrary(value);
					if(info._hModle != INVALID_HANDLE_VALUE)
					{
						pStartup p = (pStartup)GetProcAddress(info._hModle, "Startup");
						if (p)
						{
							info._plugin = (IMsgReceiver*)p(this);
							if (info._plugin != NULL)
							{
								OutputDebugString(L"TepServer bus: init\n ");
								info.filter = info._plugin->GetFilter();
								OutputDebugString(L"\nTepServer bus: _plugin->GetFilter\n " + info.filter);

								OutputDebugString(L"\nTepServer bus: info.name is：\n" + info.name);
								PluginMap.SetAt(info.name, info);

							}
							else
								OutputDebugString(L"\nTepServer bus: 失败！！！加载插件：" + info.name);

						}
					}
				}

				subKey.Close();
			}
		}
		key.Close();

		// 完成信号
		OutputDebugString(L"TepServer bus: 插件加载完成，发送完成信号...\n");
		//SendCompelete();
	}

};