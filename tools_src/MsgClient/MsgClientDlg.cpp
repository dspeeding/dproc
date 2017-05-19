
// MsgClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MsgClient.h"
#include "MsgClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMsgClientDlg 对话框



CMsgClientDlg::CMsgClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMsgClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMsgClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Control(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_EDIT_SEND, m_Send);
	DDX_Control(pDX, IDC_EDIT_RECV, m_recv);
}

BEGIN_MESSAGE_MAP(CMsgClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMsgClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_SEND, &CMsgClientDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CMsgClientDlg 消息处理程序

void InitWeb()
{
	LOG_Init(LOG_LEV_DEBUG, LOG_PRIFX, LOG_FILE);
	WSADATA wsaData;
	if(WSAStartup( MAKEWORD(2,2), &wsaData ) != 0)
	{
		LOG_E("WSAStartup函数失败[%d]", WSAGetLastError());
		return ;
	}
}

BOOL CMsgClientDlg::OnInitDialog()
{
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitWeb();
	_nOpenFlag = 0;
	_nSendFlag = 0;

	m_ip.SetWindowTextA("172.16.1.178");
	m_port.SetWindowTextA("7009");

	m_Send.SetLimitText(UINT_MAX);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMsgClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMsgClientDlg::OnPaint()
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
HCURSOR CMsgClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMsgClientDlg::OnBnClickedButton1()
{
	char	szTemp[128] = {0};
	char	szIP[128] = {0};
	long	port = 0;


	if(_nOpenFlag == 1)
	{
		closesocket(s);
		GetDlgItem(IDC_BUTTON1)->SetWindowTextA("连接");
		_nOpenFlag = 0;
		return ;
	}

	memset(szTemp, 0, sizeof(szTemp));
	m_ip.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("请输入IP");
		return ;
	}

	sprintf(szIP, "%s", szTemp);
	LOG_D("get IP=[%s]", szIP);
	memset(szTemp, 0, sizeof(szTemp));
	m_port.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("请输入Port");
		return ;
	}
	port = atoi(szTemp);

	//open 
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		LOG_E("invalid socket !");
		return ;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(szIP); 
	if (connect(s, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		LOG_E("connect error !");
		closesocket(s);
		return ;
	}
	GetDlgItem(IDC_BUTTON1)->SetWindowTextA("断开连接");
	_nOpenFlag = 1;
}

DWORD WINAPI SendProc(void* param)
{
	int		nRet;
	CMsgClientDlg* pMainDlg;
	char	szTemp[500*1024] = {0};

	pMainDlg = (CMsgClientDlg*)param;

	pMainDlg->m_recv.SetWindowTextA("");
	pMainDlg->_nSendFlag = 1;
	pMainDlg->m_Send.GetWindowTextA(szTemp, sizeof(szTemp));
	send(pMainDlg->s, szTemp, strlen(szTemp), 0);

	memset(szTemp, 0, sizeof(szTemp));

	
	nRet = recv(pMainDlg->s, szTemp, sizeof(szTemp), 0);
	if(nRet <= 0)
	{
		LOG_E("recv data err");
		pMainDlg->_nSendFlag = 0;
		pMainDlg->MessageBox("接收数据失败");
		return 0;
	}

	pMainDlg->m_recv.SetWindowTextA(szTemp);
	pMainDlg->_nSendFlag = 0;
	return 0;
}

//发送报文
void CMsgClientDlg::OnBnClickedBtnSend()
{
	char	szTemp[500 * 1024] = {0};

	if(_nOpenFlag == 0)
	{
		MessageBox("请连接服务");
		return ;
	}
	if(_nSendFlag == 1)
	{
		MessageBox("正在发送数据..稍后再试");
		return ;
	}

	m_Send.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("请输入报文");
		return ;
	}

	CreateThread(NULL, 0, SendProc, this, 0, NULL);
}
