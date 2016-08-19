// XDlgLuaEditor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "XDlgLuaEditor.h"
#include "Tool.h"

// XDlgLuaEditor 대화 상자입니다.

IMPLEMENT_DYNAMIC(XDlgLuaEditor, CDialog)

XDlgLuaEditor::XDlgLuaEditor(CWnd* pParent /*=nullptr*/)
	: CDialog(XDlgLuaEditor::IDD, pParent)
	, m_strLua(_T(""))
	, m_strTestLua(_T(""))
{

}

XDlgLuaEditor::~XDlgLuaEditor()
{
}

void XDlgLuaEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strLua);
	DDX_Text(pDX, IDC_EDIT2, m_strTestLua);
}


BEGIN_MESSAGE_MAP(XDlgLuaEditor, CDialog)
	ON_BN_CLICKED(IDOK, &XDlgLuaEditor::OnBnClickedOk)
END_MESSAGE_MAP()


// XDlgLuaEditor 메시지 처리기입니다.

void XDlgLuaEditor::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
	// 테스트용 글루함수
	strcpy_s( XSprDat::s_cGlobalLua, Convert_TCHAR_To_char( m_strTestLua ) );
	TOOL->SaveINI();				// 글로벌 테스트 코드를 저장한다
//	XResFile file;
//	file.Open( "comptest.txt", XBaseRes::xWRITE );
//	file.Write( cBuff, strlen(cBuff) ); 
}	
