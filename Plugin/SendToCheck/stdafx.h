// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../../../TEPCode/interface/CMiscHelper.h"
#include "../../include/db/CStorageFileTable.h"
#include "../../include/db/CStorageIsolateTable.h"
#include "../../../TEPCode/interface/IActiveObject.h"
#include "../../../TEPCode/interface/xmlmsg/XmlMessage.h"
#include "../../../../code/interface/ICAPInterface.h"
#include "..\..\..\TEPCode\interface\msgbus\IMsgReceiver.h"
#include "..\..\..\TEPCode\interface\plugin\CPluginBase.h"
#include "..\..\..\TEPCode\interface\LiShareLock.h"


extern	CLiShareLock		g_Lock;
char* QXUnicode2Utf8(const wchar_t* unicode,int& size);