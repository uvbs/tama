#include "stdafx.h"
#include "XWndButtonRadio.h"
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#else
#endif
#include "XFramework/XEProfile.h"

using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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

