
// RemindToolDlg.h : header file
//

#pragma once
#include "afxcmn.h"

// CRemindToolDlg dialog
class CRemindToolDlg : public CDialogEx
{
// Construction
public:
	CRemindToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMINDTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:

	NOTIFYICONDATA		m_nid;
	CListCtrl			m_list;
	vector<BUSS_INFO>	list;

	int					nTimeFirstFlag;

	char				szSelfTool[MAX_PATH];

	void InitTray();
	void InitCtrlList();
	int CheckTimeReady();
	void UpdateUIData();
	void CheckOneRecord(PBUSS_INFO pBI);
	void PropTips(PBUSS_INFO pBI, int i);
	void ShowBallon(char* title, char* msg, DWORD dwType, DWORD timeout);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
