// LogView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "TestClient.h"
#include "LogView.h"
#include "afxdialogex.h"


// LogView ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LogView, CDialogEx)

LogView::LogView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTCLIENT_DIALOG, pParent)
{

}

LogView::~LogView()
{
}

void LogView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(LogView, CDialogEx)
	
END_MESSAGE_MAP()


// LogView �޽��� ó�����Դϴ�.

