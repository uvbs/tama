// DlgSoundProp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgSoundProp.h"


// CDlgSoundProp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSoundProp, CDialog)

CDlgSoundProp::CDlgSoundProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSoundProp::IDD, pParent)
	, m_idSound(0)
{

}

CDlgSoundProp::~CDlgSoundProp()
{
}

void CDlgSoundProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_idSound);
	DDV_MinMaxInt(pDX, m_idSound, 0, 999999);
}


BEGIN_MESSAGE_MAP(CDlgSoundProp, CDialog)
END_MESSAGE_MAP()


// CDlgSoundProp 메시지 처리기입니다.
