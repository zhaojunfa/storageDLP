// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuiVisionDemoApp:
// �йش����ʵ�֣������ DuiVisionDemo.cpp
//

class CDuiVisionDemoApp : public CWinApp
{
public:
	CDuiVisionDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionDemoApp theApp;