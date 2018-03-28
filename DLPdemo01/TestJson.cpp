// TestJSON.cpp : 定义控制台应用程序的入口点。
//
#pragma once
#include  <stdlib.h>
#include  <string.h>
#include <stdio.h>  
#include <windows.h>  
#include <iostream>  
#include <atlstr.h> 
#include <hash_map>
#include <atlfile.h>
#include "D:\pro\trunk\EndpointProduct\TEPCode\interface\xmlmsg\XmlMessage.h"
#include "D:\pro\trunk\EndpointProduct\TEPCode\EApp\include\DlpHttpQuery.h"
#include "D:\pro\trunk\EndpointProduct\TEPCode\interface\CMiscHelper.h"
#include "D:\pro\trunk\EndpointProduct\TEPCode\EApp\Thrid\json\inc\json.h"
#include "D:\pro\trunk\EndpointProduct\TEPCode\interface\msgbus\IMsgReceiver.h"



/*
#include <atlbase.h>
#include "../../include/db/CStorageFileTable.h"
#include "../../include/db/CStorageIsolateTable.h"
#include "../../../TEPCode/interface/IActiveObject.h"
#include "..\..\..\TEPCode\interface\plugin\CPluginBase.h"
#include "..\..\..\TEPCode\interface\LiShareLock.h"
#include "..\..\..\TEPCode\interface\plugin\CPluginBase.h"
*/
using namespace std;
int main(){
	char* content_of_policy;
	CDlpHttpHelper http_helper;
	CString url_get_scan;
	CXmlMessage xm;
	url_get_scan = L"https://172.18.37.161/services/get_scan.php?sid=6";
	if(http_helper.DownLoadUrlData(url_get_scan,content_of_policy,FALSE))
	{
		int bytes = MultiByteToWideChar(CP_UTF8,0,content_of_policy,-1,0,0);
		WCHAR* temp_buffer = new WCHAR[bytes];
		memset(temp_buffer,0,bytes);
		MultiByteToWideChar(CP_UTF8,0,content_of_policy,-1,temp_buffer,bytes);
		CStringW str = temp_buffer;

		xm.Create(L"",L"PolicyCenter",L"SendToCheck",L"UpdateSafeDir");
		xm.AppendXml(str);
		xm.Save(str);
	}
	getchar();
	return 0;
}



