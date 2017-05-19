
// IPProxyPoolDlg.h : ͷ�ļ�
//

#pragma once
#include "curl_comm.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "WorkThread.h"

// CIPProxyPoolDlg �Ի���
class CIPProxyPoolDlg : public CDialogEx
{
// ����
public:
	CIPProxyPoolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPPROXYPOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
private:
	HANDLE				m_hSignleMutex;							//��֤�������Ե�������
	int					m_nWorkFlag;

	//��ʼ�������߳�
	void InitWorkThread();
	int Init_Env();
	void InitCtrlList();
	void CloseDlg();
	//������
	int CheckTaskOver();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	TASK_STATIS	m_task_statis[MAX_TASK_PROC_NUM];			//���������������ҳ����

	void LockData();
	void UnLockData();

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CListCtrl m_IPList;
	CStatic m_Tips;
};
