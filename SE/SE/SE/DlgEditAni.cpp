// DlgEditAni.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgEditAni.h"


// CDlgEditAni 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEditAni, CDialog)

CDlgEditAni::CDlgEditAni(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditAni::IDD, pParent)
	, m_fMaxFrame(0)
	, m_scaleX(1.0f)
	, m_scaleY(1.0f)
	, m_rotateZ(0)
	, m_RotateY( 0 )
{

}

CDlgEditAni::~CDlgEditAni()
{
}

void CDlgEditAni::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_fMaxFrame );
	DDX_Text( pDX, IDC_EDIT_SCALE_X, m_scaleX );
	DDX_Text( pDX, IDC_EDIT_SCALE_Y, m_scaleY );
	DDX_Text( pDX, IDC_EDIT_ROTATE_Z, m_rotateZ );
	DDX_Text( pDX, IDC_EDIT_ROTATE_Y, m_RotateY );
}


BEGIN_MESSAGE_MAP(CDlgEditAni, CDialog)
END_MESSAGE_MAP()


// CDlgEditAni 메시지 처리기입니다.

BOOL CDlgEditAni::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgEditAni::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}
