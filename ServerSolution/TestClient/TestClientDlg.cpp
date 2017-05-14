
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
	: CDialogEx(IDD_TESTCLIENT_DIALOG, pParent), client_session_(nullptr), is_connected_(false), connect_error_(0), user_id_(0), version_check_(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	IniReader ini_reader_;
	DWORD buffer_length;
	if (ini_reader_.Initialize(std::wstring(L"./ClientConfig.ini")) == false)
	{
		LogView(_T("---- Client Config Initialize Failed ----"));
		connect_error_ = 1;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"IP", ip_, &buffer_length);

	if (ip_.empty())
	{
		LogView(_T("---- Client Config Initialize Failed - ip load failed ----"));
		connect_error_ = 2;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"PORT", &port_);

	if (port_ == 0)
	{
		LogView(_T("---- Client Config Initialize Failed - port load failed ----"));
		connect_error_ = 3;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"VERSION", version_, &buffer_length);

	if (version_.empty())
	{
		LogView(_T("---- Client Config Initialize Failed - version load failed ----"));
		connect_error_ = 4;
	}

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

	for each(std::list<CString>::value_type itor in connect_users_)
		UserConnected(itor);
}

void CTestClientDlg::OnBnClickedVersionCert()
{
	if (is_connected_ == false)
	{
		LogView(_T("client not connected server"));
		return;
	}

	if (version_check_ == true)
	{
		LogView(_T("client already version check"));
		return;
	}

	if (connect_error_ != 0)
	{
		LogView(_T("client connect initialize faild - connect error : [%d]"), connect_error_);
		return;
	}

	// ��Ŷ�� �����մϴ�.
	std::wstring client_version = L"1.0.0.1";
	if (client_session_->SendCheckVersionReq(client_version) == false)
	{
		LogView(_T("SendCheckVersionReq - connect error"));
		return;
	}

	version_check_ = true;
}

void CTestClientDlg::OnBnClickedUserReg()
{
	if (is_connected_ == false)
	{
		LogView(_T("client not connected server"));
		return;
	}

	if (version_check_ == false)
	{
		LogView(_T("client not version check"));
		return;
	}

	if (connect_error_ != 0)
	{
		LogView(_T("client connect initialize faild - connect error : [%d]"), connect_error_);
		return;
	}

	if (user_id_ != 0 || user_nick_.compare(L"") != 0)
	{
		LogView(_T("client already reg user - user_id : [%d], nick : [%s]"), user_id_, user_nick_.c_str());
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
	client_session_->SendRegUserReq(ticket);		
}

void CTestClientDlg::OnBnClickedLogout()
{
	if (is_connected_ == false)
	{
		LogView(_T("client not connected server"));
		return;
	}

	if (version_check_ == false)
	{
		LogView(_T("client not version check"));
		return;
	}

	if (connect_error_ != 0)
	{
		LogView(_T("client connect initialize faild - connect error : [%d]"), connect_error_);
		return;
	}

	if (user_id_ == 0 && user_nick_.compare(L"") == 0)
	{
		LogView(_T("client already unreg user - user_id : [%d], nick : [%s]"), user_id_, user_nick_.c_str());
		return;
	}

	chat_view_.SetWindowText(L"");
	chat_str_.SetWindowText(L"");
	user_list_view_.SetWindowText(L"");
	chat_ = "";
	user_list_str_ = "";
	connect_users_.clear();

	// ��Ŷ�� �����մϴ�.
	client_session_->SendUnRegUserReq(user_id_);
}

void CTestClientDlg::OnBnClickedConnect()
{
	if (is_connected_)
	{
		LogView(_T("client is already connected server"));
		return;
	}

	if (version_check_ == true)
	{
		LogView(_T("client invalid - already version check"));
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	USES_CONVERSION;

	if (client_session_ == nullptr)
	{
		client_session_ = new ClientSession();

		if (client_session_->Initialize(W2A(ip_.c_str()), DEFAULT_PORT) == FALSE)
		{
			LogView(_T("---- Client Session Initialize Failed ----"));
			connect_error_ = 5;
		}
		else
		{ 
			LogView(L"---- Client Session Initialize Succeed ----");
			is_connected_ = true;
		}
	}
}

void CTestClientDlg::OnBnClickedChatSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (is_connected_ == false)
	{
		LogView(_T("client not connected server"));
		return;
	}

	if (version_check_ == false)
	{
		LogView(_T("client not version check"));
		return;
	}

	if (connect_error_ != 0)
	{
		LogView(_T("client connect initialize faild - connect error : [%d]"), connect_error_);
		return;
	}

	if (user_id_ == 0 && user_nick_.compare(L"") == 0)
	{
		LogView(_T("client not reg user - user_id : [%d], nick : [%s]"), user_id_, user_nick_.c_str());
		return;
	}

	// ��Ŷ�� �����մϴ�.
	CString cstr_chat;
	chat_str_.GetWindowText(cstr_chat);

	std::wstring chat = user_nick_ + L" : " + cstr_chat.operator LPCWSTR();
	client_session_->SendChatReq(chat);

	chat_str_.SetWindowText(L"");
}