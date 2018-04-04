#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <atlbase.h>
#include <atlstr.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <hash_map>




#include "C:\work\trunk\EndpointProduct\TEPCode\EApp\Thrid\json\inc\json.h"

#include "C:\work\trunk\EndpointProduct\TEPCode\interface\CMiscHelper.h"

#include "C:\work\trunk\EndpointProduct\storageDLP\DLPdemo01\include\db\CStorageFileTable.h"
#include "C:\work\trunk\EndpointProduct\storageDLP\DLPdemo01\include\db\CStorageIsolateTable.h"

#include "C:\work\trunk\EndpointProduct/TEPCode/interface/IActiveObject.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\xmlmsg\XmlMessage.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\msgbus\IMsgReceiver.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\plugin\CPluginBase.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\LiShareLock.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\EApp\include\DlpHttpQuery.h"
#include "C:\work\trunk\EndpointProduct\TEPCode\interface\plugin\CPluginBase.h"
#include "C:\work\trunk\code\interface\ICAPInterface.h"

extern CLiShareLock g_Lock;
char* QXUnicode2Utf8(const wchar_t* unicode,int& size);