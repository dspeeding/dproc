
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#define  _CRT_SECURE_NO_WARNINGS
#include "basetype.h"

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "dproclib.lib")
#pragma comment(lib, "pcre.lib")			//https://www.psyon.org/projects/pcre-win32/

class CIPProxyPoolDlg;

typedef struct _DEF_TASK_INFO
{
	int			nTaskID;
	int			nPage;
	int			nExt;
	int			nSleepTime;
}TASK_INFO;

typedef struct _DEF_WORKINFO
{
	int					nWorkID;
	int					nWorkCircle;
	HANDLE				hWorkHandle;
	HANDLE				hWorkEvent[3];
	long				lTime;
	int					nIsWorking;
	char*				pBuffer;
	TASK_INFO			taskInfo;
	CIPProxyPoolDlg*	pDlg;
}WORKINFO;

typedef struct _DEF_CHECKINFO
{
	int					nWorkID;
	int					nWorkCircle;
	HANDLE				hWorkHandle;
	long				lTime;
	int					nIsWorking;
	char*				pBuffer;
	CIPProxyPoolDlg*	pDlg;
}CHECKINFO;

typedef struct _DEF_PROXY_INFO
{
	int			nTaskID;
	char		ip[128];
	char		port[12];
	char		area[128];
	char		type[32];
	char		protocol[32];
	char		delay[32];
	char		time[64];
}PROXY_INFO;

typedef struct _TDEF_TASK_STATIS
{
	int		nSuccCall;
	int		nFailCall;
	int		nSuccessNum;
	int		nFailNum;
}TASK_STATIS;


#define MAX_THR_WORK_NUM		4			//�����̸߳���
#define MAX_THR_CHECK_NUM		20			//����߳�

//����ģʽ
#define SINGLE_FLAG						"DP_IPProxyPool_MUTEX_CLASS"
#define LOG_PRIFX						"IPProxyPool"					//LOGǰ׺
#define LOG_FILE						".\\Log\\IPProxyPool"			//LOG�ļ�����(����Ҫ����չ�������Զ�����_xxxx_xx_xx.log)
#define IPPROXYPOOL_MUTEX				"DP_IPProxyPool_MUTEX"
#define IPPROXYPOOL_SEMP				"DP_IPProxyPool_SEMP"
#define IPPROXYPOOL_FULL_SEMP			"DP_IPProxyPool_FULL_SEMP"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


