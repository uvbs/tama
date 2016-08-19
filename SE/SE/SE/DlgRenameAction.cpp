// DlgRenameAction.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgRenameAction.h"
#include "Tool.h"
#include "ConstantDefine.h"
// CDlgRenameAction 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRenameAction, CDialog)

CDlgRenameAction::CDlgRenameAction(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRenameAction::IDD, pParent)
	, m_strName(_T(""))
	, m_index(0)
{
	indexToID = NULL;
}

CDlgRenameAction::~CDlgRenameAction()
{
	SAFE_DELETE_ARRAY( indexToID );
}

void CDlgRenameAction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strName);
	DDX_CBIndex(pDX, IDC_COMBO1, m_index);
}


BEGIN_MESSAGE_MAP(CDlgRenameAction, CDialog)
	ON_CBN_SELCHANGE( IDC_COMBO1, &CDlgRenameAction::OnCbnSelchangeCombo1 )
END_MESSAGE_MAP()


// CDlgRenameAction 메시지 처리기입니다.
//#include "defineAct.h"		// define ACT_XXX 때문에
/*static DWORD indexToID[] = { 0,
							ACT_IDLE1,
							ACT_IDLE2,
							ACT_WALK,
							ACT_RUN,
							ACT_ATTACK1,
							ACT_ATTACK2,
							ACT_ATTACK3,
							ACT_ATTACK4,
							ACT_DAMAGE,
							ACT_DIE }; */
BOOL CDlgRenameAction::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CComboBox *pCombo = (CComboBox *)GetDlgItem( IDC_COMBO1 );
	if( pCombo )
	{
		CDefine *pDefineAct = TOOL->GetpDefineAct();
		int i  = 0;
		int size = 0;
		if( pDefineAct )
		{
			size = pDefineAct->GetnMaxDef() + 1;
			indexToID = new DWORD[ size ];
			for( i = 1; i < size; i ++ )
			{
				indexToID[i] = pDefineAct->Get( i-1 )->m_nValue;
			}
			for( i = 0; i < size; i ++ )
			{
				if( indexToID[i] == m_dwID )		// 외부에서받은 dwID값과 테이블을 비교해서 인덱스를 알아낸다
					break;
			}
			if( i == size )			// m_dwID를 테이블에서 못찾은 경우
				i = 0;				// 기본 선택콤보는 0번 자동부여
		}
		CString str;
		if( i == 0 )			// 기존ID로 넘어온것이 자동아이디면..
		{
			str.Format( _T("자동ID부여 ID=%d"), m_dwID );		// 기존ID를 보여준다
			m_bDefaultAutoType = TRUE;
		}
		else
		{
			str.Format( _T("자동ID부여 ID=%d"), SPRDAT->GenerateActID() );		// 자동부여ID는 실시간으로 다시만들어서 콤보에 보여준다
			m_bDefaultAutoType = FALSE;
		}
		pCombo->AddString( str );	// idx = 0
		if( pDefineAct )
		{
			for( int k = 1; k < size; k ++ )
			{
				pCombo->AddString( pDefineAct->Get( k-1 )->m_szRemark );
			}
		}
		pCombo->SetCurSel( i );			// 찾은 인덱스값으로 초기 설정
	} // if pCombo
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgRenameAction::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UPDATE_C2D();
	if( m_index > 0 )		// 선택한게 자동아이디가 아니면
	{
		m_dwID = indexToID[ m_index ];		// 선택한 인덱스를 아이디로 바꾼다
	} else
	if( m_index == 0 )		// 자동아이디를 선택함
	{
		if( m_bDefaultAutoType == FALSE )		// 디폴트값이 고정아이디였나
			m_dwID = SPRDAT->GenerateActID();		// 아이디를 자동생성해서 돌려준다
	}

	CDialog::OnOK();
}


void CDlgRenameAction::OnCbnSelchangeCombo1()
{
	UPDATE_C2D();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CONSOLE("%d", m_index );
	CDefine *pDefineAct = TOOL->GetpDefineAct();
	if( pDefineAct == NULL )
		return;
	if( m_index > 0 )
	{
		CString strName = pDefineAct->Get(m_index - 1)->m_strName;
		CWnd *pWnd = GetDlgItem( IDC_EDIT1 );
		pWnd->SetWindowText( strName );
//		m_strName = strName;
//		UPDATE_D2C();
	}
}
