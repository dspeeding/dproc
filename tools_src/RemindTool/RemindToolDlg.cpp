
// RemindToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemindTool.h"
#include "RemindToolDlg.h"
#include "afxdialogex.h"
#include "AddBussInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define UM_TRAYNOTIFY WM_USER + 10

CRemindToolDlg*		g_MainDlg;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CRemindToolDlg dialog

CRemindToolDlg::CRemindToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REMINDTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(&m_nid, 0, sizeof(m_nid)); // Initialize NOTIFYICONDATA struct
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
}

void CRemindToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEM, m_list);
}

BEGIN_MESSAGE_MAP(CRemindToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ADD, &CRemindToolDlg::OnBnClickedBtnAdd)
	ON_MESSAGE(UM_TRAYNOTIFY, &CRemindToolDlg::OnTrayNotify)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_DEL, &CRemindToolDlg::OnBnClickedBtnDel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRemindToolDlg message handlers

void CRemindToolDlg::InitCtrlList()
{
	char	szTips[128] = { 0 };
	LONG	lStyle;
	lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK;			//�����ʾ��ʽλ
	lStyle |= LVS_REPORT;
	SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);	//����style

	DWORD	dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;	//ѡ��ĳ��ʹ���и���
	dwStyle |= LVS_EX_GRIDLINES;		//������
	//dwStyle |= LVS_EX_CHECKBOXES;		//itemǰ����checkbox�ؼ�
	m_list.SetExtendedStyle(dwStyle);
	m_list.DeleteAllItems();

	m_list.InsertColumn(0, _T("��˾"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(1, _T("��ϵ��"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(2, _T("�绰"), LVCFMT_LEFT, 80);
	
	for (int i = 0; i < MAX_CHECK_TIME_NUM; i++)
	{
		sprintf(szTips, "����ʱ��%d", i);
		m_list.InsertColumn(3+i, szTips, LVCFMT_LEFT, 120);
	}
}

void CRemindToolDlg::InitTray()
{
	DWORD		dwRet = 0;
	m_nid.hWnd = GetSafeHwnd();
	m_nid.uCallbackMessage = UM_TRAYNOTIFY;
	// Set tray icon and tooltip
	m_nid.hIcon = m_hIcon;
	// Set tray notification tip information
	CString strToolTip = _T("RemindTool");
	_tcsncpy_s(m_nid.szTip, strToolTip, strToolTip.GetLength());
	dwRet = Shell_NotifyIcon(NIM_ADD, &m_nid);
	if (dwRet == 0)
	{
		LOG_E("Shell_NotifyIcon install icon error[%#x]", GetLastError());
	}
}

void CRemindToolDlg::UpdateUIData()
{
	int		nCurCt;
	int		nRet;
	vector<BUSS_INFO>::iterator it;
	char	tips[1024] = { 0 };

	list.clear();
	m_list.DeleteAllItems();

	nRet = LoadBussInfoList(szSelfTool, &list);
	if (nRet)
	{
		LOG_E("LoadBussInfoList err[%#x]", nRet);
		return;
	}
	
	for (it = list.begin(); it != list.end(); it++)
	{
		nCurCt = m_list.GetItemCount();
		m_list.InsertItem(nCurCt, "");
		m_list.SetItemText(nCurCt, 0, it->szComp);
		m_list.SetItemText(nCurCt, 1, it->szName);
		m_list.SetItemText(nCurCt, 2, it->szTel);
		for (int i = 0; i < MAX_CHECK_TIME_NUM; i++)
		{
			if (strlen(it->szDeadline[i]) != 0)
			{
				sprintf(tips, "%s|%s", it->szTips[i], it->szDeadline[i]);
				m_list.SetItemText(nCurCt, 3 + i, tips);
			}
		}
		
	}
}

void WriteAutoRun()
{
	HKEY	hkey; 
	DWORD	cbData = MAX_PATH; 
	char	szFileName[MAX_PATH] = { 0 };
	long	lRet;

	GetModuleFileName(NULL, szFileName, sizeof(szFileName));
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_WRITE, &hkey);
	if (lRet != ERROR_SUCCESS)
	{
		LOG_E("RegOpenKeyEx err[%#x]", lRet);
		return;
	}
	else
	{
		RegSetValueEx(hkey, _T("RemindTool"), 0, REG_SZ, (CONST BYTE *)szFileName, cbData);
	}
	RegCloseKey(hkey);
}


BOOL CRemindToolDlg::OnInitDialog()
{
	char*	p;
	char	szLogFile[MAX_PATH];
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	g_MainDlg = this;

	memset(szSelfTool, 0, sizeof(szSelfTool));
	GetModuleFileName(NULL, szSelfTool, sizeof(szSelfTool));
	p = strrchr(szSelfTool, '\\');
	*p = '\0';

	sprintf(szLogFile, "%s\\%s", szSelfTool, LOG_FILE);
	LOG_Init(LOG_LEV_DEBUG, LOG_PRIFX, szLogFile);

	HANDLE hMutex = CreateMutex(NULL, FALSE, SIGNAL_PROG);
	if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		LOG_E("already open exe program");
		CloseHandle(hMutex);
		EndDialog(0);
		return FALSE;
	}


	

	LOG_D("this program path[%s]", szSelfTool);

	//��ʼ������
	InitTray();
	//��ʼ���б�ؼ�
	InitCtrlList();
	list.clear();
	//��ʼ������
	UpdateUIData();

	WriteAutoRun();
	//��ʼ����ʱ��
	SetTimer(1, 30000, NULL);
	SetTimer(2, 10000, NULL);		//�ö�ʱ��һֱ����

	nTimeFirstFlag = 0;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemindToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemindToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemindToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�����¼�
LRESULT CRemindToolDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = (UINT)lParam;

	switch (uMsg)
	{
	case WM_RBUTTONUP:
	{
		CMenu menuTray;
		CPoint point;
		int id;
		GetCursorPos(&point);

		menuTray.LoadMenu(IDR_MENU_TRAY);
		id = menuTray.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		switch (id) {
		case ID_MENU_EXIT:
			m_nid.hIcon = NULL;
			Shell_NotifyIcon(NIM_DELETE, &m_nid);
			EndDialog(IDOK);
			break;
		case ID_MENU_SHOW:
			//����ǰ����ʾ
			SetForegroundWindow();
			ShowWindow(SW_SHOWNORMAL);
			break;
		default:
			break;
		}
		break;
	}
	case WM_LBUTTONDBLCLK:
		SetForegroundWindow();
		ShowWindow(SW_SHOWNORMAL);
		break;
	default:
		break;
	}
	return 0;
}

//�¼Ӽ�¼
void CRemindToolDlg::OnBnClickedBtnAdd()
{
	int				nRet;
	CAddBussInfoDlg	dlg;
	BUSS_INFO		bi;

	memset(&bi, 0, sizeof(bi));

	dlg.setBackInfo(&bi);

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	LOG_D("to save business info... buss name[%s]", bi.szComp);
	list.push_back(bi);
	nRet = SaveBussInfoFile(szSelfTool, &list);
	if (nRet != ERR_OK)
	{
		MessageBox("������Ϣʧ�ܣ���鿴��־");
		return;
	}

	MessageBox("������Ϣ�ɹ�");
	UpdateUIData();
}

//������Ͻǹر�
void CRemindToolDlg::OnClose()
{
	ShowWindow(SW_HIDE);
}

//ɾ��
void CRemindToolDlg::OnBnClickedBtnDel()
{
	int			nRet;
	int			nCurCt;
	CString		str;
	char		szText[128] = { 0 };

	vector<BUSS_INFO>::iterator	it;

	nCurCt = m_list.GetSelectionMark();
	if (nCurCt == -1)
	{
		MessageBox("��ѡ��Ҫɾ������");
		return;
	}

	str = m_list.GetItemText(nCurCt, 0);
	if (MessageBox("�Ƿ�ɾ�� [" + str + "] ��¼��", 0, MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	
	strcpy(szText, (LPCSTR)str);

	for (it = list.begin(); it != list.end();)
	{
		if (strcmp(szText, it->szComp) == 0)
		{
			it = list.erase(it);
		}
		else
		{
			it++;
		}
	}

	nRet = SaveBussInfoFile(szSelfTool, &list);
	if (nRet != ERR_OK)
	{
		MessageBox("��������ʧ��");
		return;
	}
	MessageBox("ɾ�����ݳɹ�");
	UpdateUIData();
}

enum BallonStyle 
{
	BS_WARNING,
	BS_ERROR,
	BS_INFO,
	BS_NONE,
	BS_USER
};

//������
void CRemindToolDlg::ShowBallon(char* title, char* msg, DWORD dwType, DWORD timeout)
{
	if (timeout <= 0)
	{
		return;
	}

	m_nid.uFlags = NIF_INFO;
	strcpy(m_nid.szInfo, msg);
	strcpy(m_nid.szInfoTitle, title);
	m_nid.uTimeout = timeout;
	switch (dwType)
	{
	case BS_WARNING:
		m_nid.dwInfoFlags = NIIF_WARNING;
		break;
	case BS_ERROR:
		m_nid.dwInfoFlags = NIIF_ERROR;
		break;
	case BS_INFO:
		m_nid.dwInfoFlags = NIIF_INFO;
		break;
	case BS_NONE:
		m_nid.dwInfoFlags = NIIF_NONE;
		break;
	}

	Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_nid));
}

//��ʾ
void CRemindToolDlg::PropTips(PBUSS_INFO pBI, int i)
{
	char	szTitle[128] = { 0 };
	char	szMsg[1024] = { 0 };

	sprintf(szTitle, "��������");
	sprintf(szMsg, "��˾[%s] ����ʱ��[%d] [%s]�Ѿ����ڣ�����", pBI->szComp, i, pBI->szTips);

	ShowBallon(szTitle, szMsg, 2, 5000);
}

void CRemindToolDlg::CheckOneRecord(PBUSS_INFO pBI)
{
	int			i;
	SYSTEMTIME	st;
	char		szCurTime[10];

	LOG_D("check comp[%s]...", pBI->szComp);

	GetLocalTime(&st);
	sprintf(szCurTime, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	for (i = 0; i < MAX_CHECK_TIME_NUM; i++)
	{
		if (strlen(pBI->szDeadline[i]) < 8)
		{
			continue;
		}

		if (strcmp(szCurTime, pBI->szDeadline[i]) >= 0)
		{
			PropTips(pBI, i);
			Sleep(1000);
		}
	}
}

//����¼��Ƿ����
DWORD WINAPI CheckOutTime(void* param)
{
	vector<BUSS_INFO>::iterator it;
	CRemindToolDlg* pDlg = (CRemindToolDlg*)param;

	LOG_D("begin to check out time...");

	for (it = pDlg->list.begin(); it != pDlg->list.end(); it++)
	{
		//���ÿ����¼��ÿ��ʱ��
		pDlg->CheckOneRecord(&*it);
	}

	LOG_D("end to check out time...");
	return 0;
}

//��ÿ���10�� ÿ���15��
int CRemindToolDlg::CheckTimeReady()
{
	SYSTEMTIME	st;
	char		szCurTime[10];

	GetLocalTime(&st);
	sprintf(szCurTime, "%02d", st.wHour);
	if (strcmp(szCurTime, "10") == 0 || strcmp(szCurTime, "15") == 0)
	{
		if (nTimeFirstFlag == 0)
		{
			nTimeFirstFlag = 1;
			return 1;
		}
	}
	else
	{
		nTimeFirstFlag = 0;
	}

	return 0;
}

void CRemindToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		KillTimer(nIDEvent);
		CreateThread(NULL, 0, CheckOutTime, this, 0, NULL);
		break;
	case 2:
		if (CheckTimeReady() == 1)
		{
			CreateThread(NULL, 0, CheckOutTime, this, 0, NULL);
		}
		break;
	}
	

	CDialogEx::OnTimer(nIDEvent);
}
