
// TestClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "TestClient.h"
#include "TestClientDlg.h"
#include "afxdialogex.h"
#include "ClientSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CTestClientDlg ��ȭ ����

CTestClientDlg::CTestClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTCLIENT_DIALOG, pParent), client_session_(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTestClientDlg::~CTestClientDlg()
{
	DELETE_TASK_MANAGER;
	SAFE_DELETE(client_session_);
}

void CTestClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, log_view_);
	DDX_Control(pDX, IDC_ID, reg_id_);
	DDX_Control(pDX, IDC_PW, reg_nick_);
	DDX_Control(pDX, IDC_USERLIST, user_list_view_);
	DDX_Control(pDX, IDC_CHAT, chat_view_);
	DDX_Control(pDX, IDC_CHAT_TEXT, chat_str_);
}

BEGIN_MESSAGE_MAP(CTestClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_VERSION_CERT, &CTestClientDlg::OnBnClickedVersionCert)
	ON_BN_CLICKED(IDC_USER_REG, &CTestClientDlg::OnBnClickedUserReg)
	ON_BN_CLICKED(IDC_LOGOUT, &CTestClientDlg::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_CONNECT, &CTestClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_CHAT_SEND, &CTestClientDlg::OnBnClickedChatSend)
END_MESSAGE_MAP()

// CTestClientDlg �޽��� ó����

BOOL CTestClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	client_session_ = new ClientSession;
	//client_session_->Initialize();

	if (TASK_MANAGER.CreateTask(TASK_EVENT_SELECT, 1, client_session_, 0, 0) == false)
		return false;

	if (TASK_MANAGER.StartTaskJob(TASK_EVENT_SELECT) == false)
		return false;

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CTestClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CTestClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestClientDlg::LogView(LPCTSTR lpszFormat, ...)
{
	CString sResult;

	va_list args = NULL;
	va_start(args, lpszFormat);

	sResult.FormatV(lpszFormat, args);

	va_end(args);

	LogView(sResult);
}

void CTestClientDlg::LogView(CString log_str)
{
	DWORD log_length = log_view_.GetWindowTextLength();

	//��¿� 1000�ڰ� ������ �����ϰ� �ٽ� ���
	if (log_length  > 1000)
	{
		log_str_.Empty();
		log_view_.SetWindowText(L"");
		log_length = log_view_.GetWindowTextLength();
	}

	log_str_.Append(log_str);
	log_str_.Append(L"\r\n");

	log_view_.SetSel(log_length, 0, TRUE);
	log_view_.ReplaceSel(log_str_);
}

void CTestClientDlg::ChatView(CString chat_str)
{
	DWORD chat_length = chat_view_.GetWindowTextLength();

	//��¿� 1000�ڰ� ������ �����ϰ� �ٽ� ���
	if (chat_length  > 1000)
	{
		chat_str.Empty();
		chat_view_.SetWindowText(L"");
		chat_length = chat_view_.GetWindowTextLength();
	}

	chat_.Append(chat_str);
	chat_.Append(L"\r\n");

	chat_view_.SetSel(chat_length, 0, TRUE);
	chat_view_.ReplaceSel(chat_);
}

void CTestClientDlg::UserConnected(CString connect_user_nick)
{
	DWORD user_list_length = user_list_view_.GetWindowTextLength();

	user_list_str_.Append(connect_user_nick);
	user_list_str_.Append(L"\r\n");

	user_list_view_.SetSel(user_list_length, 0, TRUE);
	user_list_view_.ReplaceSel(user_list_str_);
}

void CTestClientDlg::UserDisConnected(CString disconnect_user_nick)
{
	DWORD user_list_length = user_list_view_.GetWindowTextLength();

	user_list_str_ = L"";
	user_list_view_.SetWindowText(L"");
	user_list_length = user_list_view_.GetWindowTextLength();

	connect_users_.remove(disconnect_user_nick);

	for each(const std::list<CString>::value_type& itor in connect_users_)
		UserConnected(itor);
}

void CTestClientDlg::OnBnClickedConnect()
{
	//if (client_session_ == nullptr)
	//{
	//	LogView(_T("---- Connect Failed - Client Session NullPtr ----"));
	//	return;
	//}

	//if (client_session_->Connect() == false)
	//	return;
}

void CTestClientDlg::OnBnClickedVersionCert()
{
	if (client_session_ == nullptr)
	{
		LogView(_T("---- Version Check Failed - Client Session NullPtr ----"));
		return;
	}

	if (client_session_->VersionCheck() == false)
		return;
}

void CTestClientDlg::OnBnClickedUserReg()
{
	if (client_session_ == nullptr)
	{
		LogView(_T("---- User Login Failed - Client Session NullPtr ----"));
		return;
	}

	// ��Ŷ�� �����մϴ�.
	CString cstr_id;
	reg_id_.GetWindowText(cstr_id);

	CString cstr_pw;
	reg_nick_.GetWindowText(cstr_pw);

	if (cstr_id == "" || cstr_pw == "")
	{
		LogView(_T("user id or pw is empty"));
		return;
	}

	CString cstr_ticket;
	cstr_ticket.Format(L"%s|%s", cstr_id, cstr_pw);

	std::wstring ticket = cstr_ticket.operator LPCWSTR();

	if (client_session_->UserLogin(ticket) == false)
		return;
}

void CTestClientDlg::OnBnClickedLogout()
{
	if (client_session_ == nullptr)
	{
		LogView(_T("---- User Logout Failed - Client Session NullPtr ----"));
		return;
	}

	if (client_session_->UserLogOut() == false)
		return;

	chat_ = "";
	user_list_str_ = "";
	connect_users_.clear();
	chat_view_.SetWindowText(L"");
	chat_str_.SetWindowText(L"");
	user_list_view_.SetWindowText(L"");
}

void CTestClientDlg::OnBnClickedChatSend()
{
	if (client_session_ == nullptr)
	{
		LogView(_T("---- User Logout Failed - Client Session NullPtr ----"));
		return;
	}

	// ��Ŷ�� �����մϴ�.
	CString cstr_chat;
	chat_str_.GetWindowText(cstr_chat);

	std::wstring text = cstr_chat.operator LPCWSTR();

	if (client_session_->SendChatText(text) == false)
		return;

	chat_str_.SetWindowText(L"");
}