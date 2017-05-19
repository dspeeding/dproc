
// PCRETestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PCRETest.h"
#include "PCRETestDlg.h"
#include "afxdialogex.h"
#include "curl_comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char*	g_buffer = NULL;
char*	g_buffer_gbk = NULL;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CPCRETestDlg �Ի���



CPCRETestDlg::CPCRETestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PCRETEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCRETestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URL, m_url);
	DDX_Control(pDX, IDC_EDIT_PCRE, m_pcre);
	DDX_Control(pDX, IDC_EDIT_NUM, m_num);
	DDX_Control(pDX, IDC_LIST_RES, m_res);
}

BEGIN_MESSAGE_MAP(CPCRETestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CPCRETestDlg::OnBnClickedBtnStart)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPCRETestDlg ��Ϣ�������

BOOL CPCRETestDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	LOG_Init(LOG_LEV_DEBUG, "PCRETest", ".\\log\\PCRETest");
	if (curl_init())
	{
		LOG_E("curl_init error");
		MessageBox("��ʼ��curl��ʧ��");
		EndDialog(1);
	}

	g_buffer = (char *)Mem_Malloc(MAX_BUFFER_SIZE);
	if (g_buffer == NULL)
	{
		LOG_E("malloc buffer error");
		MessageBox("��ʼ��BUFFERʧ��");
		EndDialog(1);
	}
	memset(g_buffer, 0, MAX_BUFFER_SIZE);

	g_buffer_gbk = (char *)Mem_Malloc(MAX_BUFFER_SIZE);
	if (g_buffer_gbk == NULL)
	{
		LOG_E("malloc buffer error");
		MessageBox("��ʼ��BUFFERʧ��");
		EndDialog(1);
	}
	memset(g_buffer_gbk, 0, MAX_BUFFER_SIZE);

	((CButton*)GetDlgItem(IDC_RADIO_CODE_UTF8))->SetCheck(TRUE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPCRETestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPCRETestDlg::OnPaint()
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
HCURSOR CPCRETestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPCRETestDlg::InitCtrlList(int ct)
{
	LONG	lStyle;
	char	temp[128] = { 0 };

	lStyle = GetWindowLong(m_res.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK;			//�����ʾ��ʽλ
	lStyle |= LVS_REPORT;
	SetWindowLong(m_res.m_hWnd, GWL_STYLE, lStyle);	//����style

	DWORD	dwStyle = m_res.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;	//ѡ��ĳ��ʹ���и���
	dwStyle |= LVS_EX_GRIDLINES;		//������
										//dwStyle |= LVS_EX_CHECKBOXES;		//itemǰ����checkbox�ؼ�
	m_res.SetExtendedStyle(dwStyle);
	m_res.DeleteAllItems();

	while (m_res.DeleteColumn(0));

	for (INT i = 0; i < ct; i++)
	{
		sprintf(temp, "PART%d", i);
		m_res.InsertColumn(i, temp, LVCFMT_LEFT, 100);
	}

}

//��ʼ����
void CPCRETestDlg::OnBnClickedBtnStart()
{
	int		ret;
	char	url[1024] = { 0 };
	char	pcre[1024] = { 0 };
	char	temp[1024] = { 0 };
	int		num = 0;
	char*	buf = NULL;
	int		utf8 = 1;
	char*	p;
	int		i;
	int		ct = 0;
	char	area_part[128][1024];

	memset(g_buffer, 0, MAX_BUFFER_SIZE);
	memset(g_buffer_gbk, 0, MAX_BUFFER_SIZE);
	memset(area_part, 0, 128 * 1024);

	m_url.GetWindowTextA(url, sizeof(url));
	if (url[0] == '\0')
	{
		MessageBox("����дurl");
		return;
	}
	STR_Trim(url, TRIM_LEFT | TRIM_RIGHT);

	m_pcre.GetWindowTextA(pcre, sizeof(pcre));
	if (pcre[0] == '\0')
	{
		MessageBox("����дpcre");
		return;
	}
	STR_Trim(pcre, TRIM_LEFT | TRIM_RIGHT);

	m_num.GetWindowTextA(temp, sizeof(temp));
	if (temp[0] == '\0')
	{
		MessageBox("����д����");
		return;
	}
	num = atoi(temp);

	if (((CButton*)GetDlgItem(IDC_RADIO_CODE_UTF8))->GetCheck() == FALSE)
	{
		utf8 = 0;
	}

	InitCtrlList(num);

	ret = get_html_data(url, g_buffer, 5, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		MessageBox("get html data error");
		return;
	}

	p = g_buffer;
	if (utf8 == 1)
	{
		if (UTF8ToGBK(g_buffer, g_buffer_gbk, MAX_BUFFER_SIZE) == 0)
		{
			LOG_E("convert encode error");
			MessageBox("get html data error");
			return;
		}
		p = g_buffer_gbk;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		MessageBox("malloc buf error");
		return ;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(p, pcre, num, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		LOG_E("parse regex error");
		MessageBox("parse regex error");
		Mem_Free(buf);
		return;
	}

	p = buf;
	while (*p != '\0')
	{
		m_res.InsertItem(ct, "");
		for (i = 0; i < num; i++)
		{
			strcpy(area_part[i], p);
			m_res.SetItemText(ct, i, area_part[i]);
			p += strlen(p) + 1;
		}
		
		ct++;
		p += 1;		//���һ��+2
	}

	Mem_Free(buf);
}


void CPCRETestDlg::OnClose()
{
	Mem_Free(g_buffer);
	Mem_Free(g_buffer_gbk);
	curl_uninit();
	CDialogEx::OnClose();
}
