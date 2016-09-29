// DlgGmMsg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "resource.h"
#include "DlgGmMsg.h"
#include "afxdialogex.h"


// DlgGmMsg 대화 상자입니다.

CString DlgGmMsg::s_strMsg;		// 이전 메시지

IMPLEMENT_DYNAMIC(DlgGmMsg, CDialogEx)

DlgGmMsg::DlgGmMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgGmMsg::IDD, pParent)
	, m_strMsg( _T( "" ) )
{
	CString str = _T("test");
	CONSOLE("%s", str );
}

DlgGmMsg::~DlgGmMsg()
{
}

void DlgGmMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strMsg );
	DDV_MaxChars(pDX, m_strMsg, 255);
}


BEGIN_MESSAGE_MAP(DlgGmMsg, CDialogEx)
END_MESSAGE_MAP()


// DlgGmMsg 메시지 처리기입니다.


void DlgGmMsg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	s_strMsg = m_strMsg;

	CDialogEx::OnOK();
}
