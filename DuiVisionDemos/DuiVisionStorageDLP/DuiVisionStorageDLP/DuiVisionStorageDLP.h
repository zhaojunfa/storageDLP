// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuiVisionStorageDLPApp:
// �йش����ʵ�֣������ DuiVisionStorageDLP.cpp
//

class CDuiVisionStorageDLPApp : public CWinApp
{
public:
	CDuiVisionStorageDLPApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionStorageDLPApp theApp;