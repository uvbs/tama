// DlgSelectConnect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSelectConnect.h"
#include "afxdialogex.h"


// CDlgSelectConnect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSelectConnect, CDialogEx)

CDlgSelectConnect::CDlgSelectConnect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSelectConnect::IDD, pParent)
{

}

CDlgSelectConnect::~CDlgSelectConnect()
{
}

void CDlgSelectConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSelectConnect, CDialogEx)
  ON_BN_CLICKED( IDC_BUTTON1, &CDlgSelectConnect::OnBnClickedButton1 )
  ON_BN_CLICKED( IDC_BUTTON2, &CDlgSelectConnect::OnBnClickedButton2 )
  ON_BN_CLICKED( IDC_BUTTON3, &CDlgSelectConnect::OnBnClickedButton3 )
END_MESSAGE_MAP()


// CDlgSelectConnect 메시지 처리기입니다.


void CDlgSelectConnect::OnBnClickedButton1()
{
  // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
  m_idxSelect = 0;
  CDialogEx::OnOK();
}


void CDlgSelectConnect::OnBnClickedButton2()
{
  // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
  m_idxSelect = 1;
  CDialogEx::OnOK();
}


void CDlgSelectConnect::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_idxSelect = 2;
	CDialogEx::OnOK();
}
