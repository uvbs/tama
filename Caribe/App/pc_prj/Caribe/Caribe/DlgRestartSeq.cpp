// DlgRestartSeq.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgRestartSeq.h"
#include "afxdialogex.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// CDlgRestartSeq 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRestartSeq, CDialogEx)

CDlgRestartSeq::CDlgRestartSeq(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRestartSeq::IDD, pParent)
	, m_strIds( _T( "" ) )
{

}

CDlgRestartSeq::~CDlgRestartSeq()
{
}

void CDlgRestartSeq::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strIds );
}


BEGIN_MESSAGE_MAP(CDlgRestartSeq, CDialogEx)
END_MESSAGE_MAP()


// CDlgRestartSeq 메시지 처리기입니다.
