#include "stdafx.h"
#include "XWndButton.h"
#include "XWndMng.h"
#include "xUtil.h"
#include "SprObj.h"
#ifdef WIN32
#include "XGraphicsD3DTool.h"
#else
#endif
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
XWndButton* XWndButton::Find( XWndMng *pWndMng, ID idWnd ) 
{
	XWnd *pWnd = pWndMng->FindWnd( idWnd );
	if( pWnd == NULL )	return NULL;
	return dynamic_cast<XWndButton*>( pWnd );
}

// 생성자
XWndButton::XWndButton( float x, float y, LPCTSTR szSpr,
									DWORD idActUp, DWORD idActDown, DWORD idActDisable, 
									BOOL bSprNum, XToolTip *pToolTip ) 
	: XWnd( NULL, x, y ) 
{
	Init();
	XBREAK( idActUp == 0 );
	m_idActUp = idActUp;
	if( idActDown == 0 )
		m_idActDown = idActUp + 1;		// 보통은 +1 액션을 눌린것으로 쓴다
	else 
		m_idActDown = idActDown;
	if( idActDisable == 0 )
		m_idActDisable = idActDown + 1;
	else
		m_idActDisable = idActDisable;
	m_bSprNum = bSprNum;
	SetprefToolTip( pToolTip );
	//
	// 이 버튼이 독자적으로 쓸 sprobj 생성
	XSprObj *pSprObj = m_pSprObj = new XSprObj( szSpr );
	if( bSprNum )
	{
		XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite( GetidActUp() );
		SetSizeLocal( pSpr->GetSize() );
	} else
	{
		pSprObj->SetAction( GetidActUp() );
//		pSprObj->FrameMove(0);
		SetSizeLocal( pSprObj->GetWidth(), pSprObj->GetHeight() );
	}
}

// 생성자
XWndButton::XWndButton( float x, float y, XSprObj *pSprObj,
									DWORD idActUp, DWORD idActDown, DWORD idActDisable ) 
	: XWnd( NULL, x, y ) 
{
	Init();
	Create( x, y, pSprObj, idActUp, idActDown, idActDisable );
}

void XWndButton::Create( float x, float y, XSprObj *pSprObj,
									DWORD idActUp, DWORD idActDown, DWORD idActDisable ) 
{
	XBREAK( idActUp == 0 );
	m_idActUp = idActUp;
	if( idActDown == 0 )
		m_idActDown = idActUp + 1;		// 보통은 +1 액션을 눌린것으로 쓴다
	else 
		m_idActDown = idActDown;
	if( idActDisable == 0 )
		m_idActDisable = idActDown + 1;
	else
		m_idActDisable = idActDisable;

	// 이 버튼이 독자적으로 쓸 sprobj 생성
	m_pSprObj = pSprObj;
	m_pSprObj->SetAction( GetidActUp() );
//	m_pSprObj->FrameMove(0);
	SetSizeLocal( m_pSprObj->GetWidth(), m_pSprObj->GetHeight() );
}

XWndButton::XWndButton( float x, float y, float w, float h, XToolTip *pToolTip )
	: XWnd( NULL, x, y, w, h )
{
	Init();
	SetprefToolTip( pToolTip );
}
void XWndButton::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	XSprObj *pSprObj = NULL;
	
	pSprObj = m_pSprObj;		// 널일수도 있으니(다이나믹버튼) 사용하기전에 널검사 해야함
	if( pSprObj == NULL )
		return;
	if( m_bEnable )
	{
		if( m_bPush )
		{
			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActDown, xRPT_DEFAULT );
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() );
				pSprObj->Draw( vPos );
			} else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActDown);
				XE::VEC2 vScale = GetScaleFinal(); 
				pSpr->SetScale( vScale );
				pSpr->Draw( vPos );
			}
		}
		else
		{
			if( m_bSprNum==FALSE )
			{
				if( pSprObj->GetPlayType() == xRPT_1PLAY )
				{
					if( pSprObj->IsFinish() )	// 버튼눌린상태에서 여기로 왔다면 눌림애니메이션이 끝까지 플레이 할수 있도록 보장해준다. 
						pSprObj->SetAction( m_idActUp, xRPT_DEFAULT );
				} else
					pSprObj->SetAction( m_idActUp, xRPT_DEFAULT );
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() );
				pSprObj->Draw( vPos );
			} else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActUp);
				XE::VEC2 vScale = GetScaleFinal(); // m_pWndMng->GetScale() * GRAPHICS->GetvGScale() * this->GetvScale();
				pSpr->SetScale( vScale );
				pSpr->Draw(vPos);
			}
		}
	} else
	{		// disable
		if( m_idActDisable == 0xffffffff )		// disable action이 딱히 정해져 있지 않다면.
		{
			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActUp, xRPT_DEFAULT );	// up action을
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() * 0.5f );		// 반투명하게
				pSprObj->Draw( vPos );
			} else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite( m_idActUp );	
				XE::VEC2 vScale = GetScaleFinal();
				pSpr->SetScale( vScale );
				pSpr->SetfAlpha( 0.5f );
				pSpr->Draw(vPos);
			}
		} else
		{
			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActDisable, xRPT_DEFAULT );
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() );
				pSprObj->Draw( vPos );
			} else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActDisable);
				XE::VEC2 vScale = GetScaleFinal();
				pSpr->SetScale( vScale );
				pSpr->Draw(vPos);
			}
		}
	}
}
int XWndButton::Process( float dt )
{
	XWnd::Process( dt );
	if( m_pSprObj )
		m_pSprObj->FrameMove( dt );

	return 0;
}

void XWndButton::OnNCMouseMove( float lx, float ly ) 
{
	m_bPush = FALSE;	// 영역을 벗어나면 뗀걸로 표시
}
XWnd* XWndButton::OnMouseMove( float lx, float ly ) 
{ 
	if( m_bFirstPush )	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
		m_bPush = TRUE;
	else 
		m_bPush = FALSE;
	return this;
}

XWnd* XWndButton::OnLButtonDown( float lx, float ly ) 
{ 
	{
		m_bPush = TRUE;
		m_bFirstPush = TRUE;
		return this;
	}
	return NULL;
}
void XWndButton::OnNCLButtonUp( float lx, float ly ) 
{
	m_bPush = FALSE;			// 버튼영역 밖에서 떼도 클리어 해줘야한다
	m_bFirstPush = FALSE;
}
ID XWndButton::OnLButtonUp( float lx, float ly ) 
{ 
	ID idWnd = 0;
	if( m_bFirstPush && m_bPush /*&& IsWndAreaIn( lx, ly ) */ && m_bEnable )	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
	{
		CallEventHandler( XWM_CLICKED );
		idWnd = GetID();
	}
	m_bFirstPush = FALSE;
	m_bPush = FALSE;
	return idWnd;
}

BOOL XWndButton::IsWndAreaIn( float lx, float ly )
{
	XBREAK( m_pSprObj == NULL );
	XSprite *pSpr = m_pSprObj->GetSprite();
	if( pSpr )
	{
		XE::VEC2 v = XE::VEC2( lx, ly ) - pSpr->GetAdjust();
		return XWnd::IsWndAreaIn( v.x, v.y );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
void XWndDynamicButton::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	if( m_bEnable )
	{
		XE::VEC2 vScale = GetScaleFinal(); 
		if( m_bPush )
		{
			DrawFrame( GetpWndMng()->GetRes().frameButtonDown );		// 동적크기 프레임을 그린다
		} else
		{
			DrawFrame( GetpWndMng()->GetRes().frameButtonUp );		// 동적크기 프레임을 그린다
		}
	} else
	{
		// disable
		XBREAK(1);
	}
}
//////////////////////////////////////////////////////////////////////////
XWnd* XWndButtonCheck::OnMouseMove( float lx, float ly ) 
{ 
/*	if( IsWndAreaIn( lx, ly ) )
	{
		m_bToolTip = TRUE;
		return this;
	} else  {
		m_bToolTip = FALSE;
	} */
/*	if( m_bToolTip && m_prefToolTip )
	{
		if( TOOLTIP ) {
			XE::VEC2 vPos = GetPosFinal();
			XE::VEC2 vTT( vPos.x, vPos.y - m_prefToolTip->GetSize().h );
			TOOLTIP->SetvPos( vTT );
		}
	}
*/
	return NULL;
}
XWnd* XWndButtonCheck::OnLButtonDown( float lx, float ly ) 
{ 
//	if( IsWndAreaIn( lx, ly ) )	// this 버튼영역에 x, y가 있는지 검사
	{
//		if( m_timerPush.IsOff() )	// 타이머가 꺼져있을때만
//			m_timerPush.Set(0);	// 타이머 돌기 시작
		m_bFirstPush = TRUE;
		return this;
	}
//	return NULL;
}
void XWndButtonCheck::OnNCLButtonUp( float lx, float ly ) 
{
	m_bFirstPush = FALSE;
}
ID XWndButtonCheck::OnLButtonUp( float lx, float ly ) 
{ 
	ID idWnd = 0;
//	m_timerPush.Off();
//	m_bToolTip = FALSE;
//	TOOLTIP = NULL;
	if( m_bFirstPush /*&& IsWndAreaIn( lx, ly )*/ )	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
	{
		m_bPush = !m_bPush;
		CallEventHandler( XWM_CLICKED );
		idWnd = GetID();
	}	
	m_bFirstPush = FALSE;
	return idWnd;
}

////////////////////////////////////////////////////////////////
void XWndTextNumber::Draw( void )
{
	XE::VEC2 viScale = GetScaleFinal();		// 상속된 스케일값
	XE::VEC2 vPos = GetPosFinal();
	 
	XBREAK( m_pNumber == NULL );
	if( !IS_FLOAT_SAME(*m_pNumber, 0) )		// 0이 아닐때만 찍는다
	{
		m_pFont->SetAlign( xALIGN_HCENTER );
		m_pFont->SetScale( viScale );
		float fontHeight = m_pFont->GetFontHeight();
		XE::VEC2 vOffset = m_vOffset * viScale;
		m_pFont->DrawNumber( vPos.x+vOffset.x, vPos.y+vOffset.y-(fontHeight/2.0f), (int)(*m_pNumber) );
	}
}
////////////////////////////////////////////////////////////////
void XWndTextString::Draw( void )
{
	if( IsHave(m_szString) )	
	{
		XE::VEC2 vPos = GetPosFinal();
		if( m_pFont ) 
		{
//			m_pFont->DrawStringFit( x+GetWidthLocal()/2.0f, y+(GetHeightLocal()/2.0f), GetWidthLocal(), GetHeightLocal(), m_szString, (xAlign)(xALIGN_HCENTER | xALIGN_VCENTER) );
//		else {
			vPos += m_vAdjust;
			m_pFont->DrawString( vPos, m_szString );
		}
	}
}

void XWndTextStringArea::Draw( void )
{
	if( IsHave(m_szString) )	
	{
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
		if( m_pFont ) 
		{
			// 만들다 말음. 좌상귀좌표와 크기를 바탕으로 정렬등이 먹혀야 함. 뷰포트도 설정해야함.
//			vPos += m_vAdjust;
//			m_pFont->DrawString( vPos, m_szString );
		}
	}
}

////////////////////////////////////////////////////////////////
/*void XWndButtonNumber::Draw( void )
{
	XWndButton::Draw(); 
	XWndTextNumber::Draw();
}*/
////////////////////////////////////////////////////////////////
/*void XWndButtonCheckNumber::Draw( void )
{
	XWndButtonCheck::Draw();
	XWndTextNumber::Draw();
}*/

////////////////////////////////////////////////////////////////
void XWndButtonRadio::OnNCLButtonUp( float lx, float ly ) 
{
	m_bFirstPush = FALSE;
}
ID XWndButtonRadio::OnLButtonUp( float lx, float ly )
{
	ID idWnd = 0;
	if( m_bFirstPush )
	{
		m_pGroup->Clear();		// 그룹내의 모든 라디오 버튼을 Push = FALSE상태로 만든다
		m_bPush = TRUE;		// 이버튼을 눌림상태로 만든다
		CallEventHandler( XWM_CLICKED );
		idWnd = GetID();
	}
	m_bFirstPush = FALSE;
	return idWnd;
}

////////////////////////////////////////////////////////////////
// static
/*XWndButtonEffect* XWndButtonEffect::Find( XWndMng *pWndMng, ID idWnd ) 
{
	XWnd *pWnd = pWndMng->FindWnd( idWnd );
	if( pWnd == NULL )	return NULL;
//	return SafeCast<XWndButtonEffect*, XWnd*>( pWnd );
	return static_cast<XWndButtonEffect*>( pWnd );
}
*/