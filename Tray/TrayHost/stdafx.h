
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#define WINVER		0x0501

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


//#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��


// DuiVision
#include <gdiplus.h> 
using namespace Gdiplus; 

#pragma comment(lib, "gdiplus.lib")

#define _TEP_TRAYHOST_

#include "DuiDef.h"
#include "DuiVision.h"
#include "DuiXml.h"

#include "XUnZip.h"
#include "encoding.h"
//#include "common/registry.h"
#include "LogMgr.h"

#include "DuiControlEx/MFCDateTime.h"
#include "..\..\..\TEPCode\interface\CMiscHelper.h"
#include "..\..\include\db\CStorageIsolateTable.h"
#include "..\..\include\db\CStorageFileTable.h"
#include "..\..\..\TEPCode\interface\xmlmsg\XmlMessage.h"
#include "..\..\..\TEPCode\interface\CTepPipe.h"

// ע�����
#define REG_CONFIG_SUBKEY		_T("Software\\Blueant\\DuiVision\\Config")
#define REG_CONFIG_BKTYPE		_T("BkType")	// ��������(PIC/COLOR)
#define REG_CONFIG_BKPIC_RES	_T("BkPicRes")	// ����ͼƬ��ԴID
#define REG_CONFIG_BKPIC_FILE	_T("BkPicFile")	// ����ͼƬ�ļ���
#define REG_CONFIG_BKCOLOR		_T("BkColor")	// ������ɫ


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

static const UINT WM_CHECK_ITS_ME = RegisterWindowMessage(_T("##DUIVISION_APPLICATION##"));


