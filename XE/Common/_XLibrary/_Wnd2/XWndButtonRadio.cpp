#include "stdafx.h"
#include "XWndButtonRadio.h"
// #include "etc/xUtil.h"
// #include "sprite/SprObj.h"
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#else
#endif
// #include "XFramework/client/XClientMain.h"
// #include "XFramework/client/XEContent.h"
// #include "XSoundMng.h"
using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////
// static
////////////////////////////////////////////////////////////////
// XWndButtonRadio::XWndButtonRadio( float x, float y,
// 																	LPCTSTR szText,
// 																	XCOLOR colText,
// 																	XBaseFontDat *pFontDat,
// 																	LPCTSTR szImgUp,
// 																	LPCTSTR szImgDown,
// 																	LPCTSTR szImgDisable )
// 	: XWnd( x, y ), XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) 
// {
// 	Init();
// 	XE::VEC2 vSize = GetSizeLocal();
// 	//			m_pTextArea = new XWndTextStringArea( XE::VEC2(0), vSize, szText, pFontDat, colText );
// 	XWnd *pWnd = new XWndTextString( XE::VEC2( 0 ), vSize, szText, pFontDat, colText );
// 	Add( pWnd );
// }
// XWndButtonRadio::XWndButtonRadio( ID idGroup,		// 라디오 그룹 아이디. 같은 그룹내 라디오버튼들은 동일한 아이디를 부여해야 한다.
// 																	float x, float y,
// 																	LPCTSTR szText,
// 																	XCOLOR colText,
// 																	LPCTSTR szFont,
// 																	float sizeFont,
// 																	LPCTSTR szImgUp,
// 																	LPCTSTR szImgDown,
// 																	LPCTSTR szImgDisable )
// 	: XWnd( x, y ),	XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) 
// {
// 	Init();
// 	m_idGroup = idGroup;
// 	XE::VEC2 vSize = GetSizeLocal();
// 	m_pText = new XWndTextString( XE::VEC2( 0 ), vSize, szText, szFont, sizeFont, colText );
// 	Add( m_pText );
// }

void XWndButtonRadio::OnNCLButtonUp( float lx, float ly ) 
{
	m_bFirstPush = FALSE;
}
void XWndButtonRadio::OnLButtonUp( float lx, float ly )
{
	ID idWnd = 0;
	if( m_bFirstPush ) {
		if( GetpParent() ) {
			// 그룹내의 모든 라디오 버튼을 Push = FALSE상태로 만든다
			// 구버전과의 호환성을 위해 남겨둠
			for( auto pChild : GetpParent()->GetlistItems() ) {
				auto pButt = dynamic_cast<XWndButton*>( pChild );
				if( pButt ) {
					XBREAK( GetidGroup() == 0 );
					if( GetidGroup() && pButt->GetidGroup() == GetidGroup() ) {
						pButt->SetPush( FALSE );
					}
				}
			}
		}
		m_bPush = TRUE;		// 이버튼을 눌림상태로 만든다
		CallEventHandler( XWM_CLICKED );
		idWnd = GetID();
	}
	m_bFirstPush = FALSE;
}

void XWndButtonRadio::Draw( void )
{
	XPROF_OBJ_AUTO();
	XWndButton::Draw();
	//
}

