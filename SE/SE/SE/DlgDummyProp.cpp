// DlgDummyProp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgDummyProp.h"


// CDlgDummyProp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDummyProp, CDialog)

CDlgDummyProp::CDlgDummyProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDummyProp::IDD, pParent)
	, m_idDummy(0)
	, m_bActive(FALSE)
{

}

CDlgDummyProp::~CDlgDummyProp()
{
}

void CDlgDummyProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_idDummy);
	DDX_Check(pDX, IDC_CHECK1, m_bActive);
}


BEGIN_MESSAGE_MAP(CDlgDummyProp, CDialog)
END_MESSAGE_MAP()


// CDlgDummyProp 메시지 처리기입니다.
