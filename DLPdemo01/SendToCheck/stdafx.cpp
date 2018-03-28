// stdafx.cpp : 只包括标准包含文件的源文件
// SendToCheck.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


char* QXUnicode2Utf8(const wchar_t* unicode,int &size)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	if (szUtf8 == NULL)
		return NULL;
	memset(szUtf8, 0, len + 1);
	size = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL,NULL);
	return szUtf8;
}