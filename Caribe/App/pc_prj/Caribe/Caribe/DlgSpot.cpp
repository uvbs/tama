// DlgSpot.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSpot.h"
#include "afxdialogex.h"
#include "DlgSubCastle.h"
#include "DlgSubJewel.h"
#include "DlgSubSulfur.h"
#include "DlgSubMandrake.h"
#include "DlgSubNpc.h"
#include "DlgSubCamp.h"
#include "DlgSubVisit.h"
#include "DlgSubCash.h"
#include "DlgSubCommon.h"

// CDlgSpot 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSpot, CDialogEx)

CDlgSpot::CDlgSpot(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSpot::IDD, pParent)
	, m_strIds( _T( "" ) )
	, m_idSpot( 0 )
	, m_strName( _T( "" ) )
	, m_x( 0 )
	, m_y( 0 )
	, m_strArea( _T( "" ) )
	, m_strWhen( _T( "" ) )
	, m_idWhen( 0 )
	, m_strWhenDel( _T( "" ) )
	, m_idWhenDel( 0 )
	, m_strCode( _T( "" ) )
	, m_idCode( 0 )
	, m_idName( 0 )
	, m_idArea( 0 )
	, m_strSpr( _T( "" ) )
{
	XCLEAR_ARRAY( m_numProduce );
  XCLEAR_ARRAY( m_nParam );
}

CDlgSpot::~CDlgSpot()
{
}

void CDlgSpot::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_IDS, m_strIds );
	DDX_Text( pDX, IDC_EDIT_ID, m_idSpot );
	DDX_Text( pDX, IDC_EDIT_NAME, m_strName );
	DDX_Text( pDX, IDC_EDIT_X, m_x );
	DDX_Text( pDX, IDC_EDIT_Y, m_y );
	DDX_Text( pDX, IDC_EDIT_AREA, m_strArea );
	DDX_Text( pDX, IDC_EDIT_WHEN, m_strWhen );
	DDX_Text( pDX, IDC_EDIT_WHEN2, m_idWhen );
	DDX_Text( pDX, IDC_EDIT_WHEN_DEL, m_strWhenDel );
	DDX_Text( pDX, IDC_EDIT_WHEN_DEL2, m_idWhenDel );
	DDX_Text( pDX, IDC_EDIT_CODE, m_strCode );
	DDX_Text( pDX, IDC_EDIT_CODE2, m_idCode );
	DDX_Text( pDX, IDC_EDIT_NAME2, m_idName );
	DDX_Text( pDX, IDC_EDIT_AREA2, m_idArea );
	DDX_Text( pDX, IDC_EDIT_CODE3, m_strSpr );
}


BEGIN_MESSAGE_MAP(CDlgSpot, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDlgSpot 메시지 처리기입니다.
int CDlgSpot::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CDialogEx::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

BOOL CDlgSpot::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if( XASSERT( m_pDlgSub ) )
		m_pDlgSub->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
/*
	switch( m_Type )
	{
	case XGAME::xSPOT_CASTLE: {
//		auto pDlg = new CDlgSubCastle;
		if( XASSERT(m_pDlgSub) )
			m_pDlgSub->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
// 		m_pDlgSub = pDlg;
// 		m_pDlgSub->m_numWood = m_numProduce[ 0 ];
// 		m_pDlgSub->m_numIron = m_numProduce[ 1 ];
//		pDlg->SettypeDrop( m_typeDrop );
	} break;
	case XGAME::xSPOT_JEWEL: {
		auto pDlg = new CDlgSubJewel;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_Idx = m_Idx;
		pDlg->m_Produce = m_numProduce[ 0 ];
		} break;
	case XGAME::xSPOT_SULFUR: {
		auto pDlg = new CDlgSubSulfur;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_Produce = m_numProduce[ 0 ];
		pDlg->m_secRegen = m_secRegen;
		} break;
	case XGAME::xSPOT_MANDRAKE: {
		auto pDlg = new CDlgSubMandrake;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_Idx = m_Idx;
		pDlg->m_Produce = m_numProduce[ 0 ];
		} break;
	case XGAME::xSPOT_NPC: {
		auto pDlg = new CDlgSubNpc;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_secRegen = m_secRegen;
		} break;
	case XGAME::xSPOT_DAILY: 
		break;
	case XGAME::xSPOT_SPECIAL: 
		break;
	case XGAME::xSPOT_CAMPAIGN: {
		auto pDlg = new CDlgSubCamp;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_strCamp = m_str1.c_str();
		} break;
	case XGAME::xSPOT_VISIT: {
		auto pDlg = new CDlgSubVisit;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_strDialog = m_str1.c_str();
		} break;
	case XGAME::xSPOT_CASH: {
		auto pDlg = new CDlgSubCash;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_numMin = (int)m_numProduce[0];
		pDlg->m_numMax = (int)m_numProduce[1];
	} break;
	case XGAME::xSPOT_COMMON: {
		auto pDlg = new CDlgSubCommon;
		pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
		m_pDlgSub = pDlg;
		pDlg->m_strType = m_strType;
		pDlg->m_strParam = m_str1.c_str();
		pDlg->m_nParam1 = m_nParam[ 0 ];
		pDlg->m_nParam2 = m_nParam[ 1 ];
		pDlg->m_nParam3 = m_nParam[ 2 ];
		pDlg->m_nParam4 = m_nParam[ 3 ];
	} break;
//   case XGAME::xSPOT_GUILD_RAID: {
//     // x0508
// //     auto pDlg = new CDlgSubGuildRaid;
// //     pDlg->CRHCreateGenericChildDialog( this, IDC_STATIC2, 0, NULL );
// //     m_pDlgSub = pDlg;
// //     pDlg->m_numMin = (int)m_numProduce[ 0 ];
// //     pDlg->m_numMax = (int)m_numProduce[ 1 ];
//   } break;
//   case XGAME::xSPOT_PRIVATE_RAID: {
//   } break;
	default:
    XBREAK(1);
		break;
	}*/
	if( m_pDlgSub )
		m_pDlgSub->UPDATE_D2C();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDlgSpot::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_pDlgSub )
	{
		m_pDlgSub->UpdateData( TRUE );
/*
		switch( m_Type )
		{
		case XGAME::xSPOT_CASTLE: {
			auto pDlg = dynamic_cast<CDlgSubCastle*>( m_pDlgSub );
			if( pDlg )
			{
				if( pDlg->m_numWood == 0 && pDlg->m_numIron == 0 )
				{
					XALERT("생산량이 0이어선 안됩니다.");
					return;
				}
				m_numProduce[0] = pDlg->m_numWood;
				m_numProduce[1] = pDlg->m_numIron;
			}
		} break;
		case XGAME::xSPOT_JEWEL: {
			auto pDlg = dynamic_cast<CDlgSubJewel*>( m_pDlgSub );
			if( pDlg )
			{
				m_Idx = pDlg->m_Idx;
				if( pDlg->m_Produce == 0 )
				{
					XALERT( "생산량이 0이어선 안됩니다." );
					return;
				}
				m_numProduce[ 0 ] = pDlg->m_Produce;
			}
		} break;
		case XGAME::xSPOT_SULFUR: {
			auto pDlg = dynamic_cast<CDlgSubSulfur*>( m_pDlgSub );
			if( pDlg )
			{
				if( pDlg->m_Produce == 0 )
				{
					XALERT( "생산량이 0이어선 안됩니다." );
					return;
				}
				if( pDlg->m_Produce == 0 )
				{
					XALERT( "생산량이 0이어선 안됩니다." );
					return;
				}
				m_numProduce[ 0 ] = pDlg->m_Produce;
				if( pDlg->m_secRegen < 3.f )
				{
					XALERT( "리스폰 주기가 너무 짧습니다." );
					return;
				}
				m_secRegen = pDlg->m_secRegen;
			}
		} break;
		case XGAME::xSPOT_MANDRAKE: {
			auto pDlg = dynamic_cast<CDlgSubMandrake*>( m_pDlgSub );
			if( pDlg )
			{
				m_Idx = pDlg->m_Idx;
				if( pDlg->m_Produce == 0 )
				{
					XALERT( "생산량이 0이어선 안됩니다." );
					return;
				}
				m_numProduce[ 0 ] = pDlg->m_Produce;
			}
		} break;
		case XGAME::xSPOT_NPC: {
			auto pDlg = dynamic_cast<CDlgSubNpc*>( m_pDlgSub );
			if( pDlg )
			{
				if( pDlg->m_secRegen < 3.f )
				{
					XALERT( "리스폰 주기가 너무 짧습니다." );
					return;
				}
				m_secRegen = pDlg->m_secRegen;
			}
		} break;
		case XGAME::xSPOT_DAILY:
		case XGAME::xSPOT_SPECIAL:
			break;
		case XGAME::xSPOT_CAMPAIGN: {
			auto pDlg = dynamic_cast<CDlgSubCamp*>( m_pDlgSub );
			if( pDlg )
			{
				m_str1 = (LPCTSTR)pDlg->m_strCamp;
			}
		} break;
		case XGAME::xSPOT_VISIT: {
			auto pDlg = dynamic_cast<CDlgSubVisit*>( m_pDlgSub );
			if( pDlg )
			{
				m_str1 = (LPCTSTR)pDlg->m_strDialog;
			}
		} break;
		case XGAME::xSPOT_CASH: {
			auto pDlg = dynamic_cast<CDlgSubCash*>( m_pDlgSub );
			if( pDlg )
			{
				if( pDlg->m_secRegen < 3.f )
				{
					XALERT( "리스폰 주기가 너무 짧습니다." );
					return;
				}
				if( pDlg->m_numMin > 10 || pDlg->m_numMax > 10 )
				{
					XALERT( "캐쉬스팟:캐쉬제공량이 넘 많은 거 아님?" );
					return;
				}
				m_secRegen = (float)pDlg->m_secRegen;
				m_numProduce[ 0 ] = (float)pDlg->m_numMin;
				m_numProduce[ 1 ] = (float)pDlg->m_numMax;
			}
		} break;
		case XGAME::xSPOT_COMMON: {
			auto pDlg = dynamic_cast<CDlgSubCommon*>( m_pDlgSub );
			if( pDlg ) {
				m_strType = pDlg->m_strType;
				m_str1 = pDlg->m_strParam;
				m_nParam[ 0 ] = pDlg->m_nParam1;
				m_nParam[ 1 ] = pDlg->m_nParam2;
				m_nParam[ 2 ] = pDlg->m_nParam3;
				m_nParam[ 3 ] = pDlg->m_nParam4;
			}
		} break;
		default:
			XBREAK( 1 );
			break;
		}
*/
	}
	CDialogEx::OnOK();
}
