// DlgCreateItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgCreateItem.h"
#include "afxdialogex.h"
#include "XPropItem.h"
#include "XAccount.h"
#include "XGame.h"
#include "XSockGameSvr.h"


// CDlgCreateItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCreateItem, CDialogEx)

CDlgCreateItem::CDlgCreateItem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCreateItem::IDD, pParent)
	, m_strIdentifier( _T( "" ) )
	, m_numItem( 1 )
{

}

CDlgCreateItem::~CDlgCreateItem()
{
}

void CDlgCreateItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strIdentifier );
	DDX_Text( pDX, IDC_EDIT2, m_numItem );
	DDV_MinMaxInt(pDX, m_numItem, 1, 1000);
}


BEGIN_MESSAGE_MAP(CDlgCreateItem, CDialogEx)
END_MESSAGE_MAP()


// CDlgCreateItem 메시지 처리기입니다.


void CDlgCreateItem::OnOK()
{
	UPDATE_C2D();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
// 	auto pProp = PROP_ITEM->GetpProp( m_strIdentifier );
// 	if( pProp == nullptr )
// 		pProp = PROP_ITEM->GetpPropFromName( m_strIdentifier );
// 	if( pProp )
// 	{
//		int numCreated = ACCOUNT->CreateItemToInven( pProp, m_numItem );
// 		if( XBREAK( numCreated < 0 ) )
// 			return;
// 		GAMESVR_SOCKET->SendReqCheatCreateItem( GAME, pProp, m_numItem );
		//
		CDialogEx::OnOK();
// 	} else
// 		XALERT( "%s의 아이템을 찾을 수 없습니다.", m_strIdentifier );
}
