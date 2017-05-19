#pragma once
#include "afxwin.h"


// CAddBussInfoDlg dialog

class CAddBussInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddBussInfoDlg)

public:
	CAddBussInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddBussInfoDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ADD_BUSSINFO };
#endif

private:
	PBUSS_INFO m_pBI;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_comp;
	CEdit m_name;
	CEdit m_tel;
	CEdit m_tip[MAX_CHECK_TIME_NUM];
	CEdit m_time[MAX_CHECK_TIME_NUM];

	void setBackInfo(PBUSS_INFO pBI);
	afx_msg void OnBnClickedBtnOk();
	
};
