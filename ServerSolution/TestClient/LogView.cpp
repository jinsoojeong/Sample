// LogView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TestClient.h"
#include "LogView.h"
#include "afxdialogex.h"


// LogView 대화 상자입니다.

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


// LogView 메시지 처리기입니다.

