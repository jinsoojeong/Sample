#pragma once
#include "afxwin.h"


// LogView ��ȭ �����Դϴ�.

class LogView : public CDialogEx
{
	DECLARE_DYNAMIC(LogView)

public:
	LogView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LogView();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTCLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
};
