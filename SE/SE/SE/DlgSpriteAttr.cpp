// DlgSpriteAttr.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSpriteAttr.h"
#include "afxdialogex.h"


// CDlgSpriteAttr 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSpriteAttr, CDialogEx)

CDlgSpriteAttr::CDlgSpriteAttr(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSpriteAttr::IDD, pParent)
{

}

CDlgSpriteAttr::~CDlgSpriteAttr()
{
}

void CDlgSpriteAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_COMBO1, m_comboFormatSurface );
}


BEGIN_MESSAGE_MAP(CDlgSpriteAttr, CDialogEx)
END_MESSAGE_MAP()


// CDlgSpriteAttr 메시지 처리기입니다.
BOOL CDlgSpriteAttr::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	for( int i = 1; i < XE::xPF_MAX; ++i ) {
		const CString strEnum = XE::GetstrEnum( (XE::xtPixelFormat)i );
		m_comboFormatSurface.AddString( strEnum );
	}
	m_comboFormatSurface.SetCurSel( ((int)m_formatSurface) - 1 );
	UPDATE_D2C();
	return TRUE;
}

void CDlgSpriteAttr::OnOK()
{
	UPDATE_C2D();
	const auto numSel = m_comboFormatSurface.GetCurSel();
	m_formatSurface 
		= (XE::xtPixelFormat)( numSel + 1 );
	CDialogEx::OnOK();
}