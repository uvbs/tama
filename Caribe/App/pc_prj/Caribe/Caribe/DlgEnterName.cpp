// DlgEnterName.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgEnterName.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


// CDlgEnterName 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEnterName, CDialog)

CDlgEnterName::CDlgEnterName(CWnd* pParent /*=nullptr*/)
	: CDialog(CDlgEnterName::IDD, pParent)
	, m_strName(_T(""))
{

}

CDlgEnterName::~CDlgEnterName()
{
}

void CDlgEnterName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strName);
	DDV_MaxChars(pDX, m_strName, 64);
}


BEGIN_MESSAGE_MAP(CDlgEnterName, CDialog)
END_MESSAGE_MAP()


// CDlgEnterName 메시지 처리기입니다.
