#include "stdafx.h"
#include "XWndButton.h"
#include "XWndMng.h"
#include "xUtil.h"
#include "SprObj.h"
#ifdef WIN32
#include "XGraphicsD3DTool.h"
#else
#endif
#include "XClientMain.h"
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

//int (XWnd::*XWndButton::s_phSoundDown)( XWnd*, DWORD dwParam1, DWORD dwParam2 ) = NULL;
XWND_MESSAGE_MAP XWndButton::s_mmSoundDown;		// 디폴트 메시지. 사운드.
XWND_MESSAGE_MAP XWndButton::s_mmCreate;		
XWND_MESSAGE_MAP XWndButton::s_mmQuestClicked;		// 디폴트 메시지. 버튼이 눌리면 퀘스트 객체로 이벤트 전달.

/*
XWndButton* XWndButton::Find( XWndMng *pWndMng, ID idWnd ) 
{
	XWnd *pWnd = pWndMng->FindWnd( idWnd );
	if( pWnd == NULL )	return NULL;
	return dynamic_cast<XWndButton*>( pWnd );
}
*/
// 생성자
/*XWndButton::XWndButton( float x, float y, LPCTSTR szSpr,
									DWORD idActUp, DWORD idActDown, DWORD idActDisable, 
									BOOL bSprNum, XToolTip *pToolTip ) */
XWndButton::XWndButton( float x, float y, LPCTSTR szSpr,
					   DWORD idActUp, DWORD idActDown, DWORD idActDisable,
					   XToolTip *pToolTip )
	: XWnd( NULL, x, y )
{
	Init();
	XBREAK( idActUp == 0 );
	m_idActUp = idActUp;
	if( idActDown == 0 )
//		m_idActDown = idActUp + 1;		// 보통은 +1 액션을 눌린것으로 쓴다
		m_idActDown = idActUp;			// 이방식으로 바꿈
	else 
		m_idActDown = idActDown;
	if( idActDisable == 0 )
//		m_idActDisable = idActDown + 1;
		m_idActDisable = idActUp;
	else
		m_idActDisable = idActDisable;
//	m_bSprNum = bSprNum;
	;	SetprefToolTip( pToolTip );
	//
	if( szSpr )
	{
		// 이 버튼이 독자적으로 쓸 sprobj 생성
		XSprObj *pSprObj = m_pSprObj = new XSprObj( szSpr );
//		if( bSprNum )
		{
			// 앞으로 이방식 사용금지. 스프라이트 인덱스를 직접 갖다 쓰는거라 리소스 관리하기 어려워지고 거의 하드코딩과 맞먹는 수준. 필요하다면 차라리 PNG로 버튼을 만들것.
//			XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite( GetidActUp() );
//			SetSizeLocal( pSpr->GetSize() );
		}
//		else
		{
			pSprObj->SetAction( GetidActUp() );
	//		pSprObj->FrameMove(0);
			SetSizeLocal( pSprObj->GetWidth(), pSprObj->GetHeight() );
		}
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
XWndButton::XWndButton( float x, float y, LPCTSTR szImgUp, 
													LPCTSTR szImgDown, 
													LPCTSTR szImgDisable, 
													XToolTip *pToolTip, DWORD xfFlag )
	: XWnd( NULL, x, y )
{
	Init();
//	XBREAK( szImgUp == NULL );
	SetprefToolTip( pToolTip );
	if( XE::IsHave( szImgUp ) )
	{
		m_pSurface[0] = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgUp ) );
		if( XE::IsHave( szImgDown ) )
			m_pSurface[1] = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgDown ) );
		if( XE::IsHave( szImgDisable ) )
			m_pSurface[2] = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgDisable ) );
		SetSizeLocal( m_pSurface[0]->GetSize() );

	}
/*	if( m_pSurface[0] )	이거 왜 이렇게 해놨지?
	{	
		m_pSurface[0] = IMAGE_MNG->Load( TRUE, szImgUp );
		if( szImgDown )
			m_pSurface[1] = IMAGE_MNG->Load( TRUE, szImgDown );
		if( szImgDisable )
			m_pSurface[2] = IMAGE_MNG->Load( TRUE, szImgDisable );
		SetSizeLocal( m_pSurface[0]->GetSize() );
	} */
}
void XWndButton::SetFlag( DWORD xfFlag, XWnd *pParent )
{
	if( xfFlag & xfALIGN_HCENTER )
	{
		XE::VEC2 vSizeParent;
		if( pParent )
			vSizeParent = pParent->GetSizeFinal();
		else
		{
			XBREAK( GetpParent() == NULL );
			GetpParent()->GetSizeFinal();
		}
		if( m_pSurface[0] )
		{
			float left = (vSizeParent.w / 2.f) - (m_pSurface[0]->GetWidth() / 2.f);
			XE::VEC2 vPos = GetPosLocal();
			vPos.x = left;
			SetPosLocal( vPos );
		}
	}
}

// 버튼이미지를 교체
// 0: up, 1:down, 2:disable
void XWndButton::SetpSurface( int idx, LPCTSTR szImg )
{
	XBREAK( idx < 0 || idx > 2 );
	SAFE_RELEASE2(IMAGE_MNG, m_pSurface[ idx ] );
	if( szImg )
	{
		m_pSurface[ idx ] = IMAGE_MNG->Load( TRUE, szImg );
		if( idx == 0 )
			SetSizeLocal( m_pSurface[ idx ]->GetSize() );
	}
}
// 새로주어지는 szSpr로 m_pSprObj를 다시 생성한다.
void XWndButton::SetSprObj( LPCTSTR szSpr, ID idActUp, ID idActDown )
{
	SAFE_DELETE( m_pSprObj );
	XSprObj *pSprObj = m_pSprObj = new XSprObj( szSpr );
	if( idActUp != 0 )
		m_idActUp = idActUp;
	pSprObj->SetAction( GetidActUp() );
	if( idActDown != 0 )
		m_idActDown = idActDown;
	SetSizeLocal( pSprObj->GetWidth(), pSprObj->GetHeight() );
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
	Draw( GetPosFinal() );
}

void XWndButton::Draw( const XE::VEC2& vPos )
{
	XSprObj *pSprObj = NULL;
	
	pSprObj = m_pSprObj;		// 널일수도 있으니(다이나믹버튼) 사용하기전에 널검사 해야함
	if( pSprObj == NULL )
	{
		DrawFromSurface( vPos );
		return;
	}
	if( m_bEnable )
	{
		if( m_bPush )
		{
//			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActDown, xRPT_DEFAULT );
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() );
				pSprObj->Draw( vPos );
			}
/*			else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActDown);
				XE::VEC2 vScale = GetScaleFinal(); 
				pSpr->SetScale( vScale );
				pSpr->Draw( vPos );
			} */
		}
		else
		{
//			if( m_bSprNum==FALSE )
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
			}
/*			else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActUp);
				XE::VEC2 vScale = GetScaleFinal(); // m_pWndMng->GetScale() * GRAPHICS->GetvGScale() * this->GetvScale();
				pSpr->SetScale( vScale );
				pSpr->Draw(vPos);
			} */
		}
	} else
	{		// disable
		if( m_idActDisable == 0xffffffff )		// disable action이 딱히 정해져 있지 않다면.
		{
//			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActUp, xRPT_DEFAULT );	// up action을
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() * 0.5f );		// 반투명하게
				pSprObj->Draw( vPos );
			}
/*			else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite( m_idActUp );	
				XE::VEC2 vScale = GetScaleFinal();
				pSpr->SetScale( vScale );
				pSpr->SetfAlpha( 0.5f );
				pSpr->Draw(vPos);
			} */
		} else
		{
//			if( m_bSprNum==FALSE )
			{
				pSprObj->SetAction( m_idActDisable, xRPT_DEFAULT );
				pSprObj->SetScale( GetScaleFinal() );
				pSprObj->SetRotateZ( GetRotateFinal() );
				pSprObj->SetfAlpha( GetAlphaFinal() );
				pSprObj->Draw( vPos );
			}
/*			else {
				XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(m_idActDisable);
				XE::VEC2 vScale = GetScaleFinal();
				pSpr->SetScale( vScale );
				pSpr->Draw(vPos);
			} */
		}
	}
	XWnd::Draw();
}

void XWndButton::DrawFromSurface( const XE::VEC2& vPos )
{
	if( m_pSurface[0] == NULL )		return;
//	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vScale = GetScaleFinal();
	float rotZ = GetRotateFinal();
	float alpha = GetAlphaFinal();
	if( m_bEnable )
	{
		if( m_bPush )
		{
			XSurface *pSurface = ( m_pSurface[1] )? m_pSurface[1] : m_pSurface[0];
			XBREAK( pSurface == NULL );
			pSurface->SetScale( vScale );
			pSurface->SetRotateZ( rotZ );
			pSurface->SetfAlpha( alpha );
			pSurface->Draw( vPos );
		} else
		{
			m_pSurface[0]->SetScale( vScale );
			m_pSurface[0]->SetRotateZ( rotZ );
			m_pSurface[0]->SetfAlpha( alpha );
			m_pSurface[0]->Draw( vPos );
		}
	} else
	{
		// disable
		if( m_pSurface[2] == NULL )
		{
			m_pSurface[0]->SetScale( vScale );
			m_pSurface[0]->SetRotateZ( rotZ );
			m_pSurface[0]->SetfAlpha( alpha * 0.5f );	// up이미지를 더 반투명 시켜서 찍음
			m_pSurface[0]->Draw( vPos );
		} else
		{
			m_pSurface[2]->SetScale( vScale );
			m_pSurface[2]->SetRotateZ( rotZ );
			m_pSurface[2]->SetfAlpha( alpha );
			m_pSurface[2]->Draw( vPos );
		}
	}
	XWnd::Draw();
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
void XWndButton::OnMouseMove( float lx, float ly ) 
{ 
	if( m_bFirstPush )	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
		m_bPush = TRUE;
	else 
		m_bPush = FALSE;
}

void XWndButton::OnLButtonDown( float lx, float ly ) 
{ 
	m_bPush = TRUE;
	m_bFirstPush = TRUE;
	CallEventHandler( XWM_LBUTTONDOWN );
	CallEventHandler( XWM_SOUND_DOWN );
}
void XWndButton::OnNCLButtonUp( float lx, float ly ) 
{
	m_bPush = FALSE;			// 버튼영역 밖에서 떼도 클리어 해줘야한다
	m_bFirstPush = FALSE;
}
void XWndButton::OnLButtonUp( float lx, float ly ) 
{ 
	ID idWnd = 0;
	if( m_bFirstPush && m_bPush && m_bEnable )	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
	{
		int ret = CallEventHandler( XWM_CLICKED	);
		CallEventHandler( XWM_SOUND_UP );
		CallEventHandler( XWM_HELP_CLICKED, GetID() );
		if( ret )	// 클릭핸들러에서 실패리턴값을 받으면 이건 호출하지 않는다.
			CallEventHandler( XWM_QUEST_CLICKED, GetID() );
		idWnd = GetID();
	}
	m_bFirstPush = FALSE;
	m_bPush = FALSE;
}

BOOL XWndButton::IsWndAreaIn( float lx, float ly )
{
	if( m_pSprObj )
	{
		XE::VEC2 vAdj = m_pSprObj->GetAdjust();
//		XSprite *pSpr = m_pSprObj->GetSprite();
//		if( pSpr )
		{
//			XE::VEC2 v = XE::VEC2( lx, ly ) - pSpr->GetAdjust();
			XE::VEC2 v = XE::VEC2( lx, ly ) - vAdj;
			return XWnd::IsWndAreaIn( v.x, v.y );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
		}
	} else
	{
		return XWnd::IsWndAreaIn( lx, ly );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
	}
	return FALSE;
}

void XWndButton::OnAddWnd( void )
{
    if( XE::GetMain()->GetpGame() )
    {
        XLua *pLua = XE::GetMain()->GetpGame()->GetpLua();
        if( pLua )
        {
            if( pLua->IsHaveFunc( "OnEventAddWnd" ) )
            {
//                CONSOLE( "OnEventAddWnd: idWnd=%d", GetID() );
                pLua->Call<void, XWnd*>( "OnEventAddWnd", this );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
BOOL XWndButtonDebug::IsWndAreaIn( float lx, float ly )
{
	return XWnd::IsWndAreaIn( lx, ly );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
}
int XWndButtonDebug::Process( float dt )
{
	XWnd::Process( dt );
	return 1;
}
void XWndButtonDebug::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 vCenter = vPos + vSize / 2.f;	
	if( m_bEnable )
	{
		if( m_bPush )
		{
			GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_WHITE );
			GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
		} else
		{
			GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_GRAY );
			GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
		}
		m_pFontObj->SetColor( XCOLOR_BLACK );
		m_pFontObj->DrawString( vPos, m_szText );
	} else
	{
		GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_WHITE );
		GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
		m_pFontObj->SetColor( XCOLOR_LIGHTGRAY );
		m_pFontObj->DrawString( vPos, m_szText );
	}
}
//////////////////////////////////////////////////////////////////////////
/*void XWndDynamicButton::Draw( void )
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
}*/
//////////////////////////////////////////////////////////////////////////
void XWndButtonCheck::OnMouseMove( float lx, float ly ) 
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
}
void XWndButtonCheck::OnLButtonDown( float lx, float ly ) 
{ 
	m_bFirstPush = TRUE;
}
void XWndButtonCheck::OnNCLButtonUp( float lx, float ly ) 
{
	m_bFirstPush = FALSE;
}
void XWndButtonCheck::OnLButtonUp( float lx, float ly ) 
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
}

////////////////////////////////////////////////////////////////
void XWndTextNumber::Draw( void )
{
	XE::VEC2 viScale = GetScaleFinal();		// 상속된 스케일값
	XE::VEC2 vPos = GetPosFinal();
	 
	XBREAK( m_pNumber == NULL );
	if( !IS_FLOAT_SAME(*m_pNumber, 0) )		// 0이 아닐때만 찍는다
	{
		m_pFont->SetAlign( XE::xALIGN_HCENTER );
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
		Draw( GetPosFinal() );
/*		XE::VEC2 vPos = GetPosFinal();
		if( m_pFont ) 
		{
			vPos += m_vAdjust;
			if( GetbEnable() )
				m_pFont->SetColor( m_Color );
			else
				m_pFont->SetColor( m_colDisable );
			if( m_colBackground )
			{
				XE::VEC2 vText = vPos + XE::VEC2( -1, -1 );
				XE::VEC2 vTextSize = m_sizeString + XE::VEC2( 2, 2 );
				if( m_pFont->GetAlign() == XE::xALIGN_HCENTER )
				{
					vTextSize.w = m_pFont->GetLineLength() + 2.f;
					GRAPHICS->FillRectSize( vText, vTextSize, m_colBackground );
				} else
				{
					GRAPHICS->FillRectSize( vText, vTextSize, m_colBackground );
				}
			}
			m_pFont->DrawString( vPos, m_szString );
		} */
	}
}
void XWndTextString::Draw( const XE::VEC2& v )
{
	if( IsHave(m_szString) )	
	{
		if( m_pFont ) 
		{
			XE::VEC2 vPos = v;
			vPos += m_vAdjust;
			if( GetbEnable() )
				m_pFont->SetColor( m_Color );
			else
				m_pFont->SetColor( m_colDisable );
			if( m_colBackground )
			{
				XE::VEC2 vText = vPos + XE::VEC2( -1, -1 );
				XE::VEC2 vTextSize = m_sizeString + XE::VEC2( 2, 2 );
				if( m_pFont->GetAlign() == XE::xALIGN_HCENTER )
				{
					vTextSize.w = m_pFont->GetLineLength() + 2.f;
					GRAPHICS->FillRectSize( vText, vTextSize, m_colBackground );
				} else
				{
					GRAPHICS->FillRectSize( vText, vTextSize, m_colBackground );
				}
			}
			m_pFont->DrawString( vPos, m_szString );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void XWndTextStringArea::Draw( void )
{
	if( IsHave(m_szString) )	
	{
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
		if( m_pFont ) 
		{
#ifdef WIN32
			m_pFont->DrawString( vPos, m_szString );
#else
			XE::VEC2 v = (vSize / 2.f) - (m_sizeString / 2.f);		// 텍스트가 찍힐 좌상귀
			m_pFont->DrawString( vPos + v, m_szString );
#endif
		}
	}
}

void XWndTextStringArea::Draw( const XE::VEC2& vPos )
{
	if( IsHave(m_szString) )	
	{
		XE::VEC2 vSize = GetSizeFinal();
		if( m_pFont ) 
		{
#ifdef WIN32
			m_pFont->DrawString( vPos, m_szString );
#else
			XE::VEC2 v = (vSize / 2.f) - (m_sizeString / 2.f);		// 텍스트가 찍힐 좌상귀
			m_pFont->DrawString( vPos + v, m_szString );
#endif
		}
	}
}

void XWndButtonString::Draw( void ) 
{ 
	XWndButton::Draw(); 
	if( m_idActUp == m_idActDown )
	{
/*		if( GetPush() )
		{
			XCOLOR col = XWndTextStringArea::GetColorText();
			BYTE r = 255 - XCOLOR_RGB_R( col ); 
			BYTE g = 255 - XCOLOR_RGB_G( col ); 
			BYTE b = 255 - XCOLOR_RGB_B( col ); 
			BYTE a = XCOLOR_RGB_A( col ); 
			XWndTextStringArea::SetColorText( XCOLOR_RGBA(r,g,b,a) );
		} */
	}
	if( GetbEnable() == FALSE )
		XWndTextStringArea::SetColorText( XCOLOR_DARKGRAY );
	XWndTextStringArea::Draw(); 
	XWndTextStringArea::SetColorText( m_Color );
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
void XWndButtonRadio::OnLButtonUp( float lx, float ly )
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
}

void XWndButtonRadio::Draw( void )
{
	XWndButton::Draw();
	//
	if( m_pTextArea )
	{
		XE::VEC2 vPos = GetPosFinal();
		m_pTextArea->Draw( vPos );
	}
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
