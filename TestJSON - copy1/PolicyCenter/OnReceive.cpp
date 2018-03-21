#include "stdafx.h"
#include "OnReceive.h"
#include "D:\pro\trunk\code\interface\ZHead.h"
#include "D:\pro\trunk\EndpointProduct\v2Code/code/DLP/Thrid/json/inc/json.h"
CMainFrame::CMainFrame(IMsgBus* MsgBus):m_bus(MsgBus){}

CMainFrame::~CMainFrame(){}

DWORD CMainFrame::OnReceive(CStringW& msg)
{
	CXmlMessage xm;
	if(xm.Load(msg) != S_OK)
	{
		OutputDebugString(L"PolicyCenter Load msg failed");
		return 0;
	}

	CString str_type = xm.GetType();
	if(str_type.CompareNoCase(L"SignalSync") == 0)
	{
		CString result = xm.GetValue(L"Signals/Signal/@name");
		CString state =xm.GetValue(L"Signals/Signal");

		if(result.CompareNoCase(L"Started") == 0 && _ttoi(state) == 1)
		{
			OutputDebugString(L"start receive policy");
			Start();//此处跳过了
		}
		return 1;
	}
	else
	{
		return 0;
	}
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
	/*
	while (!WaitStopNotify(60 * 10 * 1000))
	{
		UpdatePolicyV2();
	}
	*/

	UpdatePolicyV2();
}


BOOL CMainFrame::UpdatePolicyV2()
{
	CXmlMessage xm;
	CString checkIP = CMiscHelper::RegGetString(L"storageDLP\\checkIP");
	CString url_get_all_scan;
	char* content_all_scan;
	CString version_from_regedit = CMiscHelper::RegGetString(L"storageDLP\\version");
	CDlpHttpHelper http_helper;
	url_get_all_scan.Format(L"https://%s/services/get_all_scan.php",checkIP);
	if(http_helper.DownLoadUrlData(url_get_all_scan,content_all_scan,0) == FALSE)
	{
		return FALSE;
	}
	Json::Reader reader;
	Json::Value obj;
	if(!reader.parse(content_all_scan,obj))
	{
		obj.clear();
		if(content_all_scan)
		{
			delete [] content_all_scan;
			content_all_scan = NULL;
		}
		return FALSE;
		//http_helper.DownLoadUrlDataRelease(content_all_scan);
	}
	char* content_one_scan = NULL;
	int scan_task_id = 0;
	int scan_task_status = 0;
	int scan_task_version = 0;
	BOOL status = FALSE;
	BOOL ret = FALSE;
	CString url_get_scan;
	do 
	{
		for(int i=0;i<(int)obj.size();++i)
		{
			if(obj[i].size() < 2)
			{
				obj.clear();
				return FALSE;
			}


			scan_task_id = atoi(obj[i]["scan_task_id"].asString().c_str());
			scan_task_status = atoi(obj[i]["scan_task_status"].asString().c_str());
			scan_task_version = atoi(obj[i]["version"].asString().c_str());
			url_get_scan.Format(L"https://%s/services/get_scan.php?sid=%d",checkIP,scan_task_id);

			if(scan_task_status != 1)
				return FALSE;
			if(http_helper.DownLoadUrlData(url_get_scan,content_one_scan,0) == FALSE)//https+GET
				return FALSE;

			int bytes = MultiByteToWideChar(CP_UTF8,0,content_one_scan,-1,0,0);
			WCHAR* buffer = new WCHAR[bytes];//== null
			memset(buffer,0,bytes);

			MultiByteToWideChar(CP_UTF8,0,content_one_scan,-1,buffer,bytes);
			CStringW str = buffer;
			delete [] content_one_scan;//
			content_one_scan = NULL;//
			xm.Create(L"",L"PolicyCenter",L"SendToCheck",L"UpdateSafeDir");
			xm.AppendXml(str);
			xm.Save(str);

			if(str.IsEmpty())
				return FALSE;
			if(xm.Load(str) != S_OK)
				return FALSE;

			CString scan_path = xm.GetValue(L"message/body/scanpolicy/scansource/datasource");
			CString safe_path =  xm.GetValue(L"message/body/scanpolicy/fileprotect/actionset/copypath");
			CString iso_path = xm.GetValue(L"message/body/scanpolicy/fileprotect/actionset/isopath");

			CString iso = xm.GetValue(L"message/body/scanpolicy/fileprotect/type/isofile");
			CString safes = xm.GetValue(L"message/body/scanpolicy/fileprotect/type/copyfile");

			CString version_from_xml;
			version_from_xml.Format(L"%d",scan_task_version);

			OutputDebugString(L"scan_path is:"+scan_path);
			OutputDebugString(L"safe_path is:"+safe_path);
			OutputDebugString(L"iso_path is:"+iso_path);
			if(version_from_xml.CompareNoCase(version_from_regedit) == 0)
			{
				status = TRUE;
				break;
			}
			if(scan_path.IsEmpty())
				break;

			CXmlMessage xml_msg;
			xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"UpdateScanDir");
			CXmlNode policy_node = xml_msg.Add(L"./policy");
			policy_node.AddTail(L".",L"scandir",scan_path);
			CString xml;
			xml_msg.Save(xml);
			m_bus->Send(NULL,xml);
			xml_msg.Load(xml);

			if(xml_msg.GetValueI32(L"./result") == 1)
			{
				CMiscHelper::RegSetString(L"storageDLP\\version",version_from_xml);
				status = TRUE;
				ret = TRUE;
			}


			if(!iso_path.IsEmpty() && _ttoi(iso) == 1)
			{
				CXmlMessage xml_msg;
				xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"UpdateIsolationDir");
				CXmlNode policy_node = xml_msg.Add(L"./policy");
				policy_node.AddTail(L".",L"isolationdir",iso_path);
				CString xml;
				xml_msg.Save(xml);
				m_bus->Send(NULL,xml);
				xml_msg.Load(xml);
				if(xml_msg.GetValueI32(L"./result") == 1)
				{
					status = TRUE;
					ret = TRUE;
				}
			}

			if(!safe_path.IsEmpty() && _ttoi(safes) == 1)
			{
				CXmlMessage xml_msg;
				xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"UpdateSafeDir");
				CXmlNode policy_node  = xml_msg.Add(L"./policy");
				policy_node.AddTail(L".",L"safedir",safe_path);
				CString xml;
				xml_msg.Save(xml);
				m_bus->Send(NULL,xml);
				xml_msg.Load(xml);
				if(xml_msg.GetValueI32(L"./result") == 1)
				{
					status = TRUE;
					ret = TRUE;
				}
			}

			if(_ttoi(iso) == 0)
			{
				CXmlMessage xml_msg;
				xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"CleanIsoDir");
				CXmlNode policy_node = xml_msg.Add(L"./policy");
				CString xml;
				xml_msg.Save(xml);
				m_bus->Send(NULL,xml);
				xml_msg.Load(xml);
				if(xml_msg.GetValueI32(L"./result") == 1)
				{
					status = TRUE;
					ret = TRUE;
				}
			}

			if(_ttoi(safes) == 0)
			{
				CXmlMessage xml_msg;
				CXmlNode policy_node;
				xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"CleanSafeDir");
				policy_node = xml_msg.Add(L"./policy");
				CString xml;
				xml_msg.Save(xml);
				m_bus->Send(NULL,xml);
				xml_msg.Load(xml);
				if(xml_msg.GetValueI32(L"./result") == 1)
				{
					status = TRUE;
					ret = TRUE;
				}
			}

			if(buffer)
			{
				delete [] buffer;
				buffer = NULL;
			}

			if(ret)
				break;
		}

		if(!status)
		{
			CXmlMessage xml_msg;
			xml_msg.Create(L"",L"PolicyCenter",L"SendToCheck",L"ScanStop");
			CXmlNode policy_node = xml_msg.Add(L"./policy");
			CString xml;
			xml_msg.Save(xml);
			m_bus->Send(NULL,xml);
		}
	} while (FALSE);
	
	return TRUE;
}

