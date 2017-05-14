
// TestClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TestClient.h"
#include "TestClientDlg.h"
#include "afxdialogex.h"
#include "ClientSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CTestClientDlg 대화 상자

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

// CTestClientDlg 메시지 처리기

BOOL CTestClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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

	//출력에 1000자가 넘으면 삭제하고 다시 출력
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

	//출력에 1000자가 넘으면 삭제하고 다시 출력
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

	// 패킷을 전송합니다.
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
	
	// 패킷을 전송합니다.
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

	// 패킷을 전송합니다.
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

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// 패킷을 전송합니다.
	CString cstr_chat;
	chat_str_.GetWindowText(cstr_chat);

	std::wstring chat = user_nick_ + L" : " + cstr_chat.operator LPCWSTR();
	client_session_->SendChatReq(chat);

	chat_str_.SetWindowText(L"");
}