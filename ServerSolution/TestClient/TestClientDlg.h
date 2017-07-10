
// TestClientDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

// CTestClientDlg 대화 상자
class CTestClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CTestClientDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	
	void LogView(LPCTSTR lpszFormat, ...);
	void LogView(CString log_str);
	void ChatView(CString chat_str);

	void UserConnected(CString connect_user_nick);
	void UserDisConnected(CString disconnect_user_nick);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	friend class ClientSession;

	ClientSession* client_session_;
	CString log_str_;
	CString chat_;
	CString user_list_str_;
	std::list<CString> connect_users_;
	
	CEdit log_view_;
	CEdit reg_id_;
	CEdit reg_nick_;
	CEdit user_list_view_;
	CEdit chat_view_;
	CEdit chat_str_;
	
public:
	afx_msg void OnBnClickedVersionCert();
	afx_msg void OnBnClickedUserReg();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedChatSend();
};
