// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// SendToCheck.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������


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