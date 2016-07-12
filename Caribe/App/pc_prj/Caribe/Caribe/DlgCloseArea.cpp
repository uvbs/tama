// DlgCloseArea.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgCloseArea.h"
#include "afxdialogex.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// CDlgCloseArea 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCloseArea, CDialogEx)

CDlgCloseArea::CDlgCloseArea(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCloseArea::IDD, pParent)
	, m_strArea( _T( "" ) )
{

}

CDlgCloseArea::~CDlgCloseArea()
{
}

void CDlgCloseArea::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strArea );
}


BEGIN_MESSAGE_MAP(CDlgCloseArea, CDialogEx)
END_MESSAGE_MAP()


// CDlgCloseArea 메시지 처리기입니다.
