#include "stdafx.h"
#include "Onreceive.h"
using namespace std;  
#include "..\..\..\..\code\interface\ZHead.h"
#include "..\..\..\v2Code\code\DLP\Thrid\json\inc\json.h"
CMainFrame::CMainFrame(IMsgBus* msgbus)
{
	m_bus = msgbus;
}
CMainFrame::~CMainFrame(void)
{

}

ULONG CMainFrame::OnReceive(CStringW& msg)
{
	CString		strType;
	CXmlMessage xm;
	
	if(S_OK != xm.Load(msg))
		return 0;
	strType = xm.GetType();
	if (strType.CompareNoCase(L"SignalSync") == 0)
	{
		CString result = xm.GetValue(L"Signals/Signal/@name");
		CString state = xm.GetValue(L"Signals/Signal");
		if (0 == result.CompareNoCase(L"Started") && _ttoi(state) == 1)
		{
			OutputDebugString(L"接收策略启动");
			Start();
		}
	}
	return 1;
}

BOOL CMainFrame::OnStartup()
{
	m_bus->RegisterReceiver(this);

	return TRUE;
}

void CMainFrame::OnStop()
{
	m_bus->UnRegisterReceiver(this);
}

void CMainFrame::Run()
{
	do 
	{
		UpdataPolicy();
	} while (!WaitStopNotify( 10 * 60 * 1000));
}

void CMainFrame::UpdataPolicy()
{
	CDlpHttpHelper helper;
	TCHAR* TEMP = NULL;
	char* content = NULL;
	CString url_GetAllScan;
	char *Updata = NULL;
	CString				url_GetScan;
	CDlpHttpHelper		tempup;
	char*				allScan = NULL;	
	CXmlMessage xm;
	bool                ret = FALSE;
	bool				status = FALSE;	
	CString ip = CMiscHelper::RegGetString(L"storageDLP\\CheckIP");
	CString id = CMiscHelper::RegGetString(L"storageDLP\\version");

	url_GetAllScan.Format(L"https://%s/services/get_all_scan.php",ip);
	do 
	{
		OutputDebugString(L"ip id"+ip+L"--"+id);
		if (!tempup.DownLoadUrlData(url_GetAllScan,allScan,FALSE))
		{
			return;
		}		
		if (allScan == NULL)
			return;
		Json::Reader reader;
		Json::Value obj;
		if (!reader.parse(allScan, obj))
		{		
			obj.clear();
			CDlpHttpHelper::DownLoadUrlDataRelease(allScan);	
			OutputDebugString(L"策略Json解析失败");
			return ;
		}
			for (int i = 0; i < obj.size(); i++)
			{
				int iPos = 0;
				CString scanContent;
				OutputDebugString(L"依次申请策略");
				if (obj[i].size() < 2)
				{
					OutputDebugString(L"连接服务器错误");
					MessageBox(NULL,L"警告",L"连接服务器错误",MB_OK);
					break;;
				}
				OutputDebugString(L"策略申请成功");
				int num = atoi(obj[i]["scan_task_id"].asString().c_str());
				int statusnum = atoi(obj[i]["scan_task_status"].asString().c_str());
				int vernum = atoi(obj[i]["version"].asString().c_str());
				url_GetScan.Format(L"https://%s/services/get_scan.php?sid=%d", ip, num);
				OutputDebugString(L"解析策略");
				if (statusnum != 1)
				{
					continue;
				}
				if(tempup.DownLoadUrlData(url_GetScan, content, FALSE, L"GET"))
				{
					int bytes = MultiByteToWideChar(CP_UTF8,0,content,-1,0,0);
					TEMP= new TCHAR[bytes];
					if(TEMP==NULL)
					{
						break;
					}
					ZeroMemory(TEMP,bytes);
					MultiByteToWideChar(CP_UTF8,0,content,-1,TEMP,bytes);
					CStringW str= TEMP;
					xm.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"UpdateSafeDir");
					xm.AppendXml(str);
					xm.Save(str);
					if (str.IsEmpty())
					{
						OutputDebugString(L"策略获取为空");
						break;
					}
					if (xm.Load(str) != S_OK)
					{
						OutputDebugString(L"策略加载失败");
						break;
					}
					
					CString scanpath = xm.GetValue(L"message/body/scanpolicy/scansource/datasource");
					CString safespath =  xm.GetValue(L"message/body/scanpolicy/fileprotect/actionset/copypath");
					CString isopath = xm.GetValue(L"message/body/scanpolicy/fileprotect/actionset/isopath");
					CString iso = xm.GetValue(L"message/body/scanpolicy/fileprotect/type/isofile");
					CString safes = xm.GetValue(L"message/body/scanpolicy/fileprotect/type/copyfile");
					CString version ;/*= xm.GetValue(L"message/header/version");*/
					version.Format(L"%d",vernum);
					OutputDebugString(L"xml"+str);
					OutputDebugString(L"scan iso safe"+scanpath+L"---"+safespath+L"---"+isopath+L"id:"+version);
					if ( 0 ==version.CompareNoCase(id))
					{
							status =TRUE;
							break;	
					}
					if (!scanpath.IsEmpty())
					{
						CXmlMessage XmlMsg;
						XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"UpdateScanDir");
						CXmlNode policyNode = XmlMsg.Add(L"./policy");
						policyNode.AddTail(L".", L"scandir", scanpath);
						CString xml;
						XmlMsg.Save(xml);
						m_bus->Send(NULL,xml);
						XmlMsg.Load(xml);
						if (XmlMsg.GetValueI32(L"./result") == 1)
						{
							CMiscHelper::RegSetString(L"storageDLP\\version",version);
							status  = TRUE;
							ret = TRUE;
						}
						else
						{
							continue;
						}
					}
					else
					{
						continue;
					}
					if (!isopath.IsEmpty() && 1 == _ttoi(iso))
					{
						CXmlMessage XmlMsg;
						XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"UpdateIsolationDir");
						CXmlNode policyNode = XmlMsg.Add(L"./policy");
						policyNode.AddTail(L".", L"isolationdir", isopath);
						CString xml;
						XmlMsg.Save(xml);
						m_bus->Send(NULL,xml);
						XmlMsg.Load(xml);
						if (XmlMsg.GetValueI32(L"./result") == 1)
						{
							status  = TRUE;
							ret = TRUE;
						}
					}
					if (!safespath.IsEmpty() && 1 == _ttoi(safes))
					{
						CXmlMessage XmlMsg;
						XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"UpdateSafeDir");
						CXmlNode policyNode = XmlMsg.Add(L"./policy");
						policyNode.AddTail(L".", L"safedir", safespath);
						CString xml;
						XmlMsg.Save(xml);
						m_bus->Send(NULL,xml);
						XmlMsg.Load(xml);
						if (XmlMsg.GetValueI32(L"./result") == 1)
						{
							status  = TRUE;
							ret = TRUE;
						}
					}
					if ( 0 == _ttoi(iso))
					{
						CXmlMessage XmlMsg;
						XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"CleanIsoDir");
						CXmlNode policyNode = XmlMsg.Add(L"./policy");
						CString xml;
						XmlMsg.Save(xml);
						m_bus->Send(NULL,xml);
						XmlMsg.Load(xml);
						if (XmlMsg.GetValueI32(L"./result") == 1)
						{
							status  = TRUE;
							ret = TRUE;
						}
					}
					if (0 == _ttoi(safes))
					{
						CXmlMessage XmlMsg;
						XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"CleanSafeDir");
						CXmlNode policyNode = XmlMsg.Add(L"./policy");
						CString xml;
						XmlMsg.Save(xml);
						m_bus->Send(NULL,xml);
						XmlMsg.Load(xml);
						if (XmlMsg.GetValueI32(L"./result") == 1)
						{
							status  = TRUE;
							ret = TRUE;
						}
					}
					if (TEMP != NULL)
					{
						delete TEMP;
						TEMP = NULL;
					}
					if (ret)
					{
						break;
					}

				}
			} 
		if (!status)
		{
			CXmlMessage XmlMsg;
			XmlMsg.Create(L"" , L"PolicyCenter" , L"SendToCheck" , L"ScanStop");
			CXmlNode policyNode = XmlMsg.Add(L"./policy");
			CString xml;
			XmlMsg.Save(xml);
			m_bus->Send(NULL,xml);
		}
	}while (FALSE);

}
