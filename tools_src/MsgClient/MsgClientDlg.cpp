
// MsgClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MsgClient.h"
#include "MsgClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMsgClientDlg �Ի���



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


// CMsgClientDlg ��Ϣ�������

void InitWeb()
{
	LOG_Init(LOG_LEV_DEBUG, LOG_PRIFX, LOG_FILE);
	WSADATA wsaData;
	if(WSAStartup( MAKEWORD(2,2), &wsaData ) != 0)
	{
		LOG_E("WSAStartup����ʧ��[%d]", WSAGetLastError());
		return ;
	}
}

BOOL CMsgClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	InitWeb();
	_nOpenFlag = 0;
	_nSendFlag = 0;

	m_ip.SetWindowTextA("172.16.1.178");
	m_port.SetWindowTextA("7009");

	m_Send.SetLimitText(UINT_MAX);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMsgClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		GetDlgItem(IDC_BUTTON1)->SetWindowTextA("����");
		_nOpenFlag = 0;
		return ;
	}

	memset(szTemp, 0, sizeof(szTemp));
	m_ip.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("������IP");
		return ;
	}

	sprintf(szIP, "%s", szTemp);
	LOG_D("get IP=[%s]", szIP);
	memset(szTemp, 0, sizeof(szTemp));
	m_port.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("������Port");
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
	GetDlgItem(IDC_BUTTON1)->SetWindowTextA("�Ͽ�����");
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
		pMainDlg->MessageBox("��������ʧ��");
		return 0;
	}

	pMainDlg->m_recv.SetWindowTextA(szTemp);
	pMainDlg->_nSendFlag = 0;
	return 0;
}

//���ͱ���
void CMsgClientDlg::OnBnClickedBtnSend()
{
	char	szTemp[500 * 1024] = {0};

	if(_nOpenFlag == 0)
	{
		MessageBox("�����ӷ���");
		return ;
	}
	if(_nSendFlag == 1)
	{
		MessageBox("���ڷ�������..�Ժ�����");
		return ;
	}

	m_Send.GetWindowTextA(szTemp, sizeof(szTemp));
	if(szTemp[0] == 0)
	{
		MessageBox("�����뱨��");
		return ;
	}

	CreateThread(NULL, 0, SendProc, this, 0, NULL);
}
