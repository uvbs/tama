// DlgSubCastle.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubCastle.h"
#include "afxdialogex.h"


// CDlgSubCastle 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubCastle, CDialogEx)

CDlgSubCastle::CDlgSubCastle(CWnd* pParent /*=NULL*/)
: 
// m_numWood( 0 )
// , m_numIron( 0 )
// , m_numJewel( 0 )
// , m_numSulfur( 0 )
// , m_numMandrake( 0 )
m_aryResource( XGAME::xRES_MAX )
, m_idxRadio( 0 )
{

}

CDlgSubCastle::~CDlgSubCastle()
{
}

void CDlgSubCastle::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange( pDX );
  DDX_Text( pDX, IDC_EDIT_WOOD, m_aryResource[XGAME::xRES_WOOD ] );
  DDX_Text( pDX, IDC_EDIT_IRON, m_aryResource[XGAME::xRES_IRON ] );
  DDX_Text( pDX, IDC_EDIT_JEWEL, m_aryResource[XGAME::xRES_JEWEL] );
  DDX_Text( pDX, IDC_EDIT_SULFUR, m_aryResource[XGAME::xRES_SULFUR ] );
  DDX_Text( pDX, IDC_EDIT_MANDRAKE, m_aryResource[XGAME::xRES_MANDRAKE ] );
  DDX_Radio( pDX, IDC_RADIO_NONE, m_idxRadio );
}


BEGIN_MESSAGE_MAP(CDlgSubCastle, CDialogEx)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_NONE, IDC_RADIO_SPHERE, OnRadioControl )
END_MESSAGE_MAP()


// CDlgSubCastle 메시지 처리기입니다.
// int CDlgSubCastle::CRHGetDialogID()
// {
// 	return( IDD );
// }


// void CDlgSubCastle::OnBnClickedRadioNone()
// {
//   // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//   m_typeDrop2 = 0;
//   UPDATE_D2C();
// }
// 
// 
// void CDlgSubCastle::OnBnClickedRadioMedal()
// {
//   // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//   m_typeDrop2 = 1;
//   UPDATE_D2C();
// }
// 
// 
// void CDlgSubCastle::OnBnClickedRadioSphere()
// {
//   // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//   m_typeDrop2 = 2;
//   UPDATE_D2C();
// }


void CDlgSubCastle::SettypeDrop( XGAME::xtDropCastle type )
{
  m_idxRadio = 0;
  switch( type )
  {
  case XGAME::xDC_NONE:
  case XGAME::xDC_NO_DROP:
    m_idxRadio = 0;
    break;
  case XGAME::xDC_MEDAL:
    m_idxRadio = 1;
    break;
  case XGAME::xDC_SCROLL:
    m_idxRadio = 2;
    break;
  default:
    XBREAK(1);
    break;
  }
//  UpdateData( FALSE );
}
void CDlgSubCastle::OnRadioControl(UINT id )
{
  UpdateData( TRUE );
  switch( m_idxRadio )
  {
  case 0:
    m_typeDrop = XGAME::xDC_NO_DROP;
    break;
  case 1:
    m_typeDrop = XGAME::xDC_MEDAL;
    break;
  case 2:
    m_typeDrop = XGAME::xDC_SCROLL;
    break;
  default:
    break;
  }
}


