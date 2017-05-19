// AddBussInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemindTool.h"
#include "AddBussInfoDlg.h"
#include "afxdialogex.h"


// CAddBussInfoDlg dialog

IMPLEMENT_DYNAMIC(CAddBussInfoDlg, CDialogEx)

CAddBussInfoDlg::CAddBussInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ADD_BUSSINFO, pParent)
{

}

CAddBussInfoDlg::~CAddBussInfoDlg()
{
}

void CAddBussInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMP, m_comp);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_TEL, m_tel);
	for (int i = 0; i < MAX_CHECK_TIME_NUM; i++)
	{
		DDX_Control(pDX, IDC_EDIT4 + i, m_tip[i]);
	}
	for (int i = 0; i < MAX_CHECK_TIME_NUM; i++)
	{
		DDX_Control(pDX, IDC_EDIT19 + i, m_time[i]);
	}
}


BEGIN_MESSAGE_MAP(CAddBussInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OK, &CAddBussInfoDlg::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CAddBussInfoDlg message handlers

#define GET_WIN_TEXT(a,b,c,d)	do{\
	BUFCLR(a);\
	b.GetWindowTextA(a, sizeof(a));\
	if (a[0] == 0)\
	{\
		MessageBox(c);\
		return;\
	}\
	strcpy(d, a);\
	}while(0)

#define GET_WIN_TEXT_WITH_NULL(a,b,d)	do{\
	BUFCLR(a);\
	b.GetWindowTextA(a, sizeof(a));\
	strcpy(d, a);\
	}while(0)

void CAddBussInfoDlg::setBackInfo(PBUSS_INFO pBI)
{
	m_pBI = pBI;
}


void CAddBussInfoDlg::OnBnClickedBtnOk()
{
	char	szTemp[1024] = { 0 };

	GET_WIN_TEXT(szTemp, m_comp, "请输入公司名称", m_pBI->szComp);
	GET_WIN_TEXT(szTemp, m_name, "请输入联系人姓名", m_pBI->szName);
	GET_WIN_TEXT(szTemp, m_tel, "请输入联系电话", m_pBI->szTel);
	for (int i = 0; i < MAX_CHECK_TIME_NUM; i++)
	{
		GET_WIN_TEXT_WITH_NULL(szTemp, m_tip[i], m_pBI->szTips[i]);
		GET_WIN_TEXT_WITH_NULL(szTemp, m_time[i], m_pBI->szDeadline[i]);
	}
	
	EndDialog(IDOK);
}
