
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

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


#define MAX_THR_WORK_NUM		4			//工作线程个数
#define MAX_THR_CHECK_NUM		20			//检查线程

//单例模式
#define SINGLE_FLAG						"DP_IPProxyPool_MUTEX_CLASS"
#define LOG_PRIFX						"IPProxyPool"					//LOG前缀
#define LOG_FILE						".\\Log\\IPProxyPool"			//LOG文件名称(不需要加扩展名，会自动加上_xxxx_xx_xx.log)
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


