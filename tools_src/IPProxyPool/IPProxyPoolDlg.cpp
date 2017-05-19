
// IPProxyPoolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPProxyPool.h"
#include "IPProxyPoolDlg.h"
#include "afxdialogex.h"
#include "WorkThread.h"
#include "curl_comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIPProxyPoolDlg 对话框



CIPProxyPoolDlg::CIPProxyPoolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IPPROXYPOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	for (int i = 0; i < MAX_THR_WORK_NUM; i++)
	{
		m_WorkInfo[i].hWorkEvent[0] = NULL;
		m_WorkInfo[i].hWorkEvent[1] = NULL;
		m_WorkInfo[i].hWorkEvent[2] = NULL;
		m_WorkInfo[i].hWorkHandle = NULL;
		m_WorkInfo[i].pBuffer = NULL;
		m_WorkInfo[i].nWorkCircle = 0;
		m_WorkInfo[i].nWorkID = 0;
		m_WorkInfo[i].nIsWorking = 0;
	}
	for (int i = 0; i < MAX_THR_CHECK_NUM; i++)
	{
		m_CheckInfo[i].nWorkID = 0;
		m_CheckInfo[i].nWorkCircle = 0;
		m_CheckInfo[i].nIsWorking = 0;
	}
	for (int i = 0; i < MAX_TASK_PROC_NUM; i++)
	{
		m_task_statis[i].nSuccCall = 0;
		m_task_statis[i].nFailCall = 0;
		m_task_statis[i].nSuccessNum = 0;
		m_task_statis[i].nFailNum = 0;
	}
	m_max_get_num = 0;
	m_fail_num = 0;
	m_nWorkFlag = 0;
}

void CIPProxyPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IPLIST, m_IPList);
	DDX_Control(pDX, IDC_STATIC_TIPS, m_Tips);
}

BEGIN_MESSAGE_MAP(CIPProxyPoolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CIPProxyPoolDlg::OnBnClickedBtnStart)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIPProxyPoolDlg 消息处理程序

void CIPProxyPoolDlg::CloseDlg()
{
	int		i;
	HANDLE	handle[MAX_THR_WORK_NUM+ MAX_THR_CHECK_NUM];

	for (i = 0; i < MAX_THR_WORK_NUM; i++)
	{
		m_WorkInfo[i].nWorkCircle = 0;
		handle[i] = m_WorkInfo[i].hWorkHandle;
	}
	for (i = 0; i < MAX_THR_CHECK_NUM; i++)
	{
		m_CheckInfo[i].nWorkCircle = 0;
		handle[i+MAX_THR_WORK_NUM] = m_CheckInfo[i].hWorkHandle;
	}

	WaitForMultipleObjects(MAX_THR_WORK_NUM+ MAX_THR_CHECK_NUM, handle, TRUE, INFINITE);
	
	for (int i = 0; i < MAX_THR_WORK_NUM; i++)
	{
		Mem_Free(m_WorkInfo[i].pBuffer);
	}
	for (int i = 0; i < MAX_THR_CHECK_NUM; i++)
	{
		Mem_Free(m_CheckInfo[i].pBuffer);
	}

	CloseHandle(m_hMutex);
	CloseHandle(m_hMutex_UI);
	CloseHandle(m_hSemp);
	CloseHandle(m_hFullSemp);
	DestroyArrayQueue(m_Queue);

	//如果是Debug 则在最后的时候打印内存泄露信息
#ifdef _DEBUG
	Mem_Leak();
#endif

	curl_uninit();
	CloseHandle(m_hSignleMutex);
	EndDialog(0);
}

void CIPProxyPoolDlg::LockData()
{
	WaitForSingleObject(m_hMutex, INFINITE);
}

void CIPProxyPoolDlg::UnLockData()
{
	ReleaseMutex(m_hMutex);
}

int CIPProxyPoolDlg::Init_Env()
{
	int		ret;

	m_hMutex = CreateMutexA(NULL, FALSE, IPPROXYPOOL_MUTEX);
	if (m_hMutex == NULL)
	{
		LOG_E("CreateMutexA m_hMutex error");
		MessageBox("初始化互斥量失败");
		return 1;
	}

	m_hMutex_UI = CreateMutexA(NULL, FALSE, IPPROXYPOOL_MUTEX);
	if (m_hMutex_UI == NULL)
	{
		LOG_E("CreateMutexA m_hMutex_UI error");
		MessageBox("初始化互斥量失败");
		return 1;
	}

	m_hSemp = CreateSemaphoreA(NULL, 0, 1000, IPPROXYPOOL_SEMP);		//初值为0上限为1000
	if (m_hSemp == NULL)
	{
		LOG_E("CreateSemaphoreA error");
		MessageBox("初始化信号量失败");
		return 1;
	}

	m_hFullSemp = CreateSemaphoreA(NULL, 1000, 1000, IPPROXYPOOL_FULL_SEMP);		//初值为1000上限为1000
	if (m_hFullSemp == NULL)
	{
		LOG_E("CreateSemaphoreA error");
		MessageBox("初始化信号量失败");
		return 1;
	}

	ret = InitArrayQueue(&m_Queue, sizeof(PROXY_INFO), 100);
	if (ret != ERR_OK)
	{
		LOG_E("InitArrayQueue error");
		MessageBox("初始化Queue失败");
		return 1;
	}

	if (curl_init())
	{
		LOG_E("curl_init error");
		MessageBox("初始化curl库失败");
		return 1;
	}

	//初始化工作线程
	InitWorkThread();
	return 0;
}

void CIPProxyPoolDlg::InitCtrlList()
{
	LONG	lStyle;
	lStyle = GetWindowLong(m_IPList.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK;			//清除显示方式位
	lStyle |= LVS_REPORT;
	SetWindowLong(m_IPList.m_hWnd, GWL_STYLE, lStyle);	//设置style

	DWORD	dwStyle = m_IPList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;	//选中某行使整行高亮
	dwStyle |= LVS_EX_GRIDLINES;		//网格线
										//dwStyle |= LVS_EX_CHECKBOXES;		//item前生成checkbox控件
	m_IPList.SetExtendedStyle(dwStyle);
	m_IPList.DeleteAllItems();

	m_IPList.InsertColumn(0, "IP", LVCFMT_LEFT, 100);
	m_IPList.InsertColumn(1, "PORT", LVCFMT_LEFT, 60);
	m_IPList.InsertColumn(2, "类型", LVCFMT_LEFT, 70);
	m_IPList.InsertColumn(3, "协议", LVCFMT_LEFT, 80);
	m_IPList.InsertColumn(4, "延迟", LVCFMT_LEFT, 70);
	m_IPList.InsertColumn(5, "验证时间", LVCFMT_LEFT, 100);
	m_IPList.InsertColumn(6, "地区", LVCFMT_LEFT, 180);
	
}

BOOL CIPProxyPoolDlg::OnInitDialog()
{
	int		nLogLevel;

	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

									//单例运行
	m_hSignleMutex = CreateMutex(NULL, FALSE, _T(SINGLE_FLAG));
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			::MessageBoxA(NULL, "程序已打开", "错误", MB_OK);
			CloseHandle(m_hSignleMutex);
			EndDialog(1);
			return TRUE;
		}
	}

	nLogLevel = GetPrivateProfileIntA("LOG", "LOG_LEVEL", LOG_LEV_DEBUG, "config.ini");
	LOG_Init(nLogLevel, LOG_PRIFX, LOG_FILE);

#ifdef _DEBUG
	Mem_SetDbgOn();
#endif

	InitCtrlList();

	if (Init_Env())
	{
		CloseHandle(m_hSignleMutex);
		EndDialog(1);
		return TRUE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIPProxyPoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIPProxyPoolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIPProxyPoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//初始化工作线程
void CIPProxyPoolDlg::InitWorkThread()
{
	for (int i = 0; i < MAX_THR_WORK_NUM; i++)
	{
		m_WorkInfo[i].nWorkID = i;
		m_WorkInfo[i].nWorkCircle = 1;
		m_WorkInfo[i].lTime = GetTickCount();
		m_WorkInfo[i].nIsWorking = 0;
		m_WorkInfo[i].pDlg = this;

		for (int j = 0; j < 3; j++)
		{
			m_WorkInfo[i].hWorkEvent[j] = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (m_WorkInfo[i].hWorkEvent[j] == NULL)
			{
				LOG_E("工作线程[%d]创建Event[%d]信号量失败[%#x]", i, j, GetLastError());
				::MessageBoxA(m_hWnd, "创建信号量失败", "错误", MB_OK);
				CloseDlg();
				return;
			}
		}

		m_WorkInfo[i].pBuffer = (char *)Mem_Malloc(MAX_BUFFER_SIZE);
		if (m_WorkInfo[i].pBuffer == NULL)
		{
			LOG_E("申请BUFFER[%d]失败[%#x]", i, GetLastError());
			::MessageBoxA(m_hWnd, "申请BUFFER失败", "错误", MB_OK);
			CloseDlg();
			return;
		}
		memset(m_WorkInfo[i].pBuffer, 0, MAX_BUFFER_SIZE);

		m_WorkInfo[i].hWorkHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThreadProc, &m_WorkInfo[i], 0, NULL);
		if (m_WorkInfo[i].hWorkHandle == NULL)
		{
			LOG_E("创建工作线程[%d]失败[%#x]", i, GetLastError());
			::MessageBoxA(m_hWnd, "创建工作线程失败", "错误", MB_OK);
			CloseDlg();
			return;
		}
	}

	for (int i = 0; i < MAX_THR_CHECK_NUM; i++)
	{
		m_CheckInfo[i].nWorkID = i;
		m_CheckInfo[i].nWorkCircle = 1;
		m_CheckInfo[i].lTime = GetTickCount();
		m_CheckInfo[i].nIsWorking = 0;
		m_CheckInfo[i].pDlg = this;
		m_CheckInfo[i].pBuffer = (char *)Mem_Malloc(MAX_BUFFER_SIZE);
		if (m_CheckInfo[i].pBuffer == NULL)
		{
			LOG_E("申请BUFFER[%d]失败[%#x]", i, GetLastError());
			::MessageBoxA(m_hWnd, "申请BUFFER失败", "错误", MB_OK);
			CloseDlg();
			return;
		}
		memset(m_CheckInfo[i].pBuffer, 0, MAX_BUFFER_SIZE);
		m_CheckInfo[i].hWorkHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckThreadProc, &m_CheckInfo[i], 0, NULL);
		if (m_CheckInfo[i].hWorkHandle == NULL)
		{
			LOG_E("创建分析线程[%d]失败[%#x]", i, GetLastError());
			::MessageBoxA(m_hWnd, "创建分析线程失败", "错误", MB_OK);
			CloseDlg();
			return;
		}
	}
}

//开始分配任务
int StartWorkProc(void* param)
{
	CIPProxyPoolDlg*	pDlg;
	int					i,j;
	int					nCt = 0;
	int					nTotalCt = 0;
	TASK_INFO			ti[5* MAX_TASK_PROC_NUM];
	int					nFlag = 0;

	pDlg = (CIPProxyPoolDlg*)param;

	memset(ti, 0, sizeof(TASK_INFO) * 5 * MAX_TASK_PROC_NUM);
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < MAX_TASK_PROC_NUM; j++)
		{
			ti[nTotalCt].nPage = i;
			ti[nTotalCt].nTaskID = j;
			ti[nTotalCt].nSleepTime = rand() % 10000 + 3000;
			ti[nTotalCt].nExt = 0;
			nTotalCt++;
		}
	}

	nCt = 0;
	while (nCt < nTotalCt)
	{
		nFlag = 0;
		for (i = 0; i < MAX_THR_WORK_NUM; i++)
		{
			if (pDlg->m_WorkInfo[i].nIsWorking != 0)
			{
				continue;
			}
			nFlag = 1;
			memcpy(&pDlg->m_WorkInfo[i].taskInfo, &ti[nCt], sizeof(TASK_INFO));
			SetEvent(pDlg->m_WorkInfo[i].hWorkEvent[0]);
			Sleep(1000);
			nCt++;
			break;
		}

		if (nFlag == 0)
		{
			Sleep(1000);
		}
	}

	LOG_D("task alread over...");
	return 1;
}

//抓取IP
void CIPProxyPoolDlg::OnBnClickedBtnStart()
{
	if (m_nWorkFlag != 0)
	{
		MessageBoxA("正在进行任务...");
		return;
	}

	m_nWorkFlag = 1;
	m_max_get_num = 0;
	ClearArrayQueue(m_Queue);
	m_IPList.DeleteAllItems();
	m_Tips.SetWindowTextA("");
	for (int i = 0; i < MAX_TASK_PROC_NUM; i++)
	{
		m_task_statis[i].nSuccCall = 0;
		m_task_statis[i].nFailCall = 0;
		m_task_statis[i].nSuccessNum = 0;
		m_task_statis[i].nFailNum = 0;
	}

	KillTimer(1);
	SetTimer(1, 500, NULL);
	SetTimer(2, 2000, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartWorkProc, this, 0, NULL);
}

//检测结束  0--没有结束 1--结束
int CIPProxyPoolDlg::CheckTaskOver()
{
	int		i;
	int		nFlag;

	if (m_nWorkFlag == 0)
	{
		return 1;
	}

	nFlag = 0;
	for (i = 0; i < MAX_THR_WORK_NUM; i++)
	{
		if (m_WorkInfo[i].nIsWorking == 1)
		{
			nFlag = 1;
			break;
		}
	}
	if (nFlag == 1)
	{
		return 0;
	}

	nFlag = 0;
	for (i = 0; i < MAX_THR_CHECK_NUM; i++)
	{
		if (m_CheckInfo[i].nIsWorking == 1)
		{
			nFlag = 1;
			break;
		}
	}
	if (nFlag == 1)
	{
		return 0;
	}

	if (GetArrayQueueLen(m_Queue) != 0)
	{
		return 0;
	}
	
	return 1;
}

void CIPProxyPoolDlg::OnTimer(UINT_PTR nIDEvent)
{
	int		i;
	double	result;
	char	szTips[1024] = { 0 };
	switch (nIDEvent)
	{
	case 1:
		//刷新界面统计
		sprintf(szTips, "共获取ip个数[%d]个,验证成功[%d]个,失败[%d]个,剩余[%d]个", 
			m_max_get_num, m_IPList.GetItemCount(), m_fail_num, GetArrayQueueLen(m_Queue));
		m_Tips.SetWindowTextA(szTips);
		break;
	case 2:
		//检测是否结束
		if (CheckTaskOver() == 1)
		{
			KillTimer(2);
			m_nWorkFlag = 0;
			for (i = 0; i < MAX_TASK_PROC_NUM; i++)
			{
				if (m_task_statis[i].nSuccessNum + m_task_statis[i].nFailNum == 0)
				{
					result = 0;
				}
				else
				{
					result = m_task_statis[i].nSuccessNum * 100 / ((m_task_statis[i].nSuccessNum + m_task_statis[i].nFailNum)*1.0);
				}
				
				LOG_D("task id[%d] succCall[%d] failCall[%d] succNum[%d] failNum[%d] per[%.2lf]", 
					i, m_task_statis[i].nSuccCall, m_task_statis[i].nFailCall,
					m_task_statis[i].nSuccessNum, m_task_statis[i].nFailNum, result);
			}
			MessageBoxA("运行结束");
		}
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}

void CIPProxyPoolDlg::OnClose()
{
	CloseDlg();
}
