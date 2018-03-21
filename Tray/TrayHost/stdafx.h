
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#define WINVER		0x0501

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


//#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


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

// 注册表定义
#define REG_CONFIG_SUBKEY		_T("Software\\Blueant\\DuiVision\\Config")
#define REG_CONFIG_BKTYPE		_T("BkType")	// 背景类型(PIC/COLOR)
#define REG_CONFIG_BKPIC_RES	_T("BkPicRes")	// 背景图片资源ID
#define REG_CONFIG_BKPIC_FILE	_T("BkPicFile")	// 背景图片文件名
#define REG_CONFIG_BKCOLOR		_T("BkColor")	// 背景颜色


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


