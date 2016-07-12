// DlgBattle11.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgBattle11.h"
#include "afxdialogex.h"
#include "XPropHero.h"


// CDlgBattle11 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgBattle11, CDialogEx)

CDlgBattle11::CDlgBattle11(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBattle11::IDD, pParent)
	, m_radioPlayer( 0 )
	, m_radioAI( 0 )
	, m_lvSquadLeft( 1 )
	, m_lvSquadRight( 1 )
	, m_strHero1( _T( "random" ) )
	, m_strHero2( _T( "random" ) )
	, m_lvHero1( 0 )
	, m_lvHero2( 0 )
{

}

CDlgBattle11::~CDlgBattle11()
{
}

void CDlgBattle11::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Radio( pDX, IDC_RADIO_11, m_radioPlayer );
	DDX_Radio( pDX, IDC_RADIO_21, m_radioAI );
	DDX_Text( pDX, IDC_EDIT1, m_lvSquadLeft );
	DDV_MinMaxInt( pDX, m_lvSquadLeft, 1, 50 );
	DDX_Text( pDX, IDC_EDIT2, m_lvSquadRight );
	DDV_MinMaxInt( pDX, m_lvSquadRight, 1, 50 );
	DDX_Text( pDX, IDC_EDIT3, m_strHero1 );
	DDX_Text( pDX, IDC_EDIT4, m_strHero2 );
	DDX_Text( pDX, IDC_EDIT12, m_lvHero1 );
	DDX_Text( pDX, IDC_EDIT13, m_lvHero2 );
}


BEGIN_MESSAGE_MAP(CDlgBattle11, CDialogEx)
	ON_BN_CLICKED( IDC_RADIO_11, &CDlgBattle11::OnBnClickedRadio11 )
	ON_BN_CLICKED( IDC_RADIO_12, &CDlgBattle11::OnBnClickedRadio12 )
	ON_BN_CLICKED( IDC_RADIO_13, &CDlgBattle11::OnBnClickedRadio13 )
	ON_BN_CLICKED( IDC_RADIO_14, &CDlgBattle11::OnBnClickedRadio14 )
	ON_BN_CLICKED( IDC_RADIO_15, &CDlgBattle11::OnBnClickedRadio15 )
	ON_BN_CLICKED( IDC_RADIO_16, &CDlgBattle11::OnBnClickedRadio16 )
	ON_BN_CLICKED( IDC_RADIO_17, &CDlgBattle11::OnBnClickedRadio17 )
	ON_BN_CLICKED( IDC_RADIO_18, &CDlgBattle11::OnBnClickedRadio18 )
	ON_BN_CLICKED( IDC_RADIO_19, &CDlgBattle11::OnBnClickedRadio19 )
	ON_BN_CLICKED( IDC_RADIO_20, &CDlgBattle11::OnBnClickedRadio20 )
	ON_BN_CLICKED( IDC_RADIO_21, &CDlgBattle11::OnBnClickedRadio21 )
	ON_BN_CLICKED( IDC_RADIO_22, &CDlgBattle11::OnBnClickedRadio22 )
	ON_BN_CLICKED( IDC_RADIO_23, &CDlgBattle11::OnBnClickedRadio23 )
	ON_BN_CLICKED( IDC_RADIO_24, &CDlgBattle11::OnBnClickedRadio24 )
	ON_BN_CLICKED( IDC_RADIO_25, &CDlgBattle11::OnBnClickedRadio25 )
	ON_BN_CLICKED( IDC_RADIO_26, &CDlgBattle11::OnBnClickedRadio26 )
	ON_BN_CLICKED( IDC_RADIO_27, &CDlgBattle11::OnBnClickedRadio27 )
	ON_BN_CLICKED( IDC_RADIO_28, &CDlgBattle11::OnBnClickedRadio28 )
	ON_BN_CLICKED( IDC_RADIO_29, &CDlgBattle11::OnBnClickedRadio29 )
	ON_BN_CLICKED( IDC_RADIO_30, &CDlgBattle11::OnBnClickedRadio30 )
END_MESSAGE_MAP()


// CDlgBattle11 메시지 처리기입니다.


void CDlgBattle11::OnBnClickedRadio11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_radioPlayer = 0;
	//m_numPlayer = XGAME::MAX_UNIT_SMALL;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = XGAME::MAX_UNIT_SMALL;
	m_radioPlayer = 1;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = XGAME::MAX_UNIT_SMALL;
	m_radioPlayer = 2;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 4;
	m_radioPlayer = 3;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 4;
	m_radioPlayer = 4;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 4;
	m_radioPlayer = 5;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio17()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 1;
	m_radioPlayer = 6;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio18()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 1;
	m_radioPlayer = 7;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio19()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 1;
	m_radioPlayer = 8;
	UPDATE_D2C();
}

void CDlgBattle11::OnBnClickedRadio20()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numPlayer = 1;
	m_radioPlayer = -1;	// random
	UPDATE_D2C();
}


void CDlgBattle11::OnBnClickedRadio21()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = XGAME::MAX_UNIT_SMALL;
	m_radioAI = 0;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio22()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = XGAME::MAX_UNIT_SMALL;
	m_radioAI = 1;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio23()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = XGAME::MAX_UNIT_SMALL;
	m_radioAI = 2;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio24()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 4;
	m_radioAI = 3;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio25()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 4;
	m_radioAI = 4;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio26()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 4;
	m_radioAI = 5;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio27()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 1;
	m_radioAI = 6;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio28()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 1;
	m_radioAI = 7;
	UPDATE_D2C();
}
void CDlgBattle11::OnBnClickedRadio29()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 1;
	m_radioAI = 8;
	UPDATE_D2C();
}

void CDlgBattle11::OnBnClickedRadio30()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_numAI = 1;
	m_radioAI = -1;	// random
	UPDATE_D2C();
}

void CDlgBattle11::OnOK()
{
	UPDATE_C2D();
	if( m_strHero1 != _T("random") && !m_strHero1.IsEmpty() ) {
		XPropHero::xPROP *pProp = PROP_HERO->GetpProp( m_strHero1 );
		if( pProp == nullptr ) {
			XALERT("영웅 찾을수 없음: %s", m_strHero1);
			return;
		}
	}
	if( m_strHero2 != _T( "random" ) && !m_strHero2.IsEmpty() ) {
		XPropHero::xPROP *pProp = PROP_HERO->GetpProp( m_strHero2 );
		if( pProp == nullptr ) {
			XALERT( "영웅 찾을수 없음: %s", m_strHero2 );
			return;
		}
	}
	__super::OnOK();
}

