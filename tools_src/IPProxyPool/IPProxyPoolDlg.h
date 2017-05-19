
// IPProxyPoolDlg.h : 头文件
//

#pragma once
#include "curl_comm.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "WorkThread.h"

// CIPProxyPoolDlg 对话框
class CIPProxyPoolDlg : public CDialogEx
{
// 构造
public:
	CIPProxyPoolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPPROXYPOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	HANDLE				m_hSignleMutex;							//保证本程序以单例运行
	int					m_nWorkFlag;

	//初始化工作线程
	void InitWorkThread();
	int Init_Env();
	void InitCtrlList();
	void CloseDlg();
	//检测结束
	int CheckTaskOver();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	WORKINFO	m_WorkInfo[MAX_THR_WORK_NUM];
	CHECKINFO	m_CheckInfo[MAX_THR_CHECK_NUM];
	HANDLE		m_hMutex;
	HANDLE		m_hMutex_UI;
	HANDLE		m_hSemp;
	HANDLE		m_hFullSemp;
	ArrayQueue*	m_Queue;
	int			m_max_get_num;
	int			m_fail_num;
	TASK_STATIS	m_task_statis[MAX_TASK_PROC_NUM];			//这里数组个数是网页个数

	void LockData();
	void UnLockData();

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CListCtrl m_IPList;
	CStatic m_Tips;
};
