
// IPProxyPool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIPProxyPoolApp: 
// �йش����ʵ�֣������ IPProxyPool.cpp
//

class CIPProxyPoolApp : public CWinApp
{
public:
	CIPProxyPoolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIPProxyPoolApp theApp;