
// MsgClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMsgClientDlg �Ի���
class CMsgClientDlg : public CDialogEx
{
// ����
public:
	CMsgClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MSGCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


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
	afx_msg void OnBnClickedButton1();


	int		s;
	int		_nOpenFlag;
	int		_nSendFlag;
	CIPAddressCtrl m_ip;
	CEdit m_port;
	CEdit m_Send;
	CEdit m_recv;
	afx_msg void OnBnClickedBtnSend();
};
