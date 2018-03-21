// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  
#include<iostream>  
#include <hash_map>
// TODO: 在此处引用程序需要的其他头文件
#include "../../../TEPCode/interface/CMiscHelper.h"
#include "../../include/db/CStorageFileTable.h"
#include "../../include/db/CStorageIsolateTable.h"
#include "../../../TEPCode/interface/IActiveObject.h"
#include "../../../TEPCode/interface/xmlmsg/XmlMessage.h"
#include "..\..\..\TEPCode\interface\msgbus\IMsgReceiver.h"
#include "..\..\..\TEPCode\interface\plugin\CPluginBase.h"
#include "..\..\..\TEPCode\interface\LiShareLock.h"
#include "..\..\..\TEPCode\EApp\include\DlpHttpQuery.h"
#include "..\..\..\TEPCode\interface\plugin\CPluginBase.h"