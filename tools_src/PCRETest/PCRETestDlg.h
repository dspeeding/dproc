
// PCRETestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CPCRETestDlg 对话框
class CPCRETestDlg : public CDialogEx
{
// 构造
public:
	CPCRETestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PCRETEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	void InitCtrlList(int ct);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_url;
	CEdit m_pcre;
	CEdit m_num;
	CListCtrl m_res;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnClose();
};
