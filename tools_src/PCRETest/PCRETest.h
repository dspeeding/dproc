
// PCRETest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPCRETestApp: 
// �йش����ʵ�֣������ PCRETest.cpp
//

class CPCRETestApp : public CWinApp
{
public:
	CPCRETestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPCRETestApp theApp;