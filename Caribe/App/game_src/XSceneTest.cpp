#include "stdafx.h"
#ifdef _XTEST
#include "XSceneTest.h"
#include "XGame.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "XFramework/XSplitNode.h"
#include "OpenGL2/XTextureAtlas.h"
#include "Sprite/SprObj.h"
#include "etc/XSurfaceDef.h"
#include "Opengl2/XBatchRenderer.h"
#include "XGameWnd.h"
#include "XWndStorageItemElem.h"
#include "_Wnd2/XWndButton.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#endif // _VER_ANDROID

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSplit;
const char* s_files[] = {
"arrow_paraly.spr",
 "eff_hit02.spr",
"hero_bruxa.spr",
"hero_congent.spr",
"sfx_awaken.spr",
"unit_treant2.spr" 
};

/////////////////////////////////////////////////////////////////////////////////////////
XSceneTest* XSceneTest::s_pSingleton = nullptr;
XSceneTest* XSceneTest::sGet(){	return s_pSingleton;}
//static const XE::VEC2 c_sizeAtlas = XE::VEC2( 4096, 4096 );
//////////////////////////////////////////////////////////////////////////
void XSceneTest::Destroy() 
{	
//	SAFE_DELETE_ARRAY( m_pAtlas );
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneTest::XSceneTest( XGame *pGame, XSPSceneParam& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_TEST )
//	, m_Layout(_T("scene_Sample.xml"))
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	Init(); 
//	m_Layout.CreateLayout("main", this);
	//
	// TODO: 이곳에 코딩하시오
	//
	XE::VEC2 vPos( 100, 100 );
	const XE::VEC2 size( 100, 100 );
	auto pButt = new XWndButtonDebug( vPos, size, _T("test") );
	Add( pButt );
	pButt->SetEvent2( XWM_CLICKED, [this]( XWnd* ) {
#ifdef _VER_ANDROID
		//JniHelper::DoTest();
		static bool bShow = false;
		bShow = !bShow;
		JniHelper::ShowAdmob( bShow, 500, 200 );
#endif // _VER_ANDROID
	} );
// 	m_pAtlas = new DWORD[ (int)c_sizeAtlas.Size() ];
// 	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof(DWORD) );
	SetbUpdate( true );
}

/**
 @brief 
*/
void XSceneTest::Create( void )
{
	XSceneBase::Create();
// 	for( int i = 0; i < MAX_SPR1 * MAX_SPR2; ++i ) {
// 		const int maxFiles = XNUM_ARRAY( s_files );
// 		const _tstring strFile = C2SZ( s_files[xRandom(maxFiles)] );
// 		m_psoTest[i] = new XSprObj( _T( "ui_levelup.spr" ), XE::xHSL(), false, false, false, nullptr );
// 		m_psoTest[i]->SetAction( 3 );
// // 		m_psoTest[i] = new XSprObj( strFile );
// 		auto pso = m_psoTest[i];
// //		pso->SetAction( 4 );
// //		pso->SetScale( xRandomF( 0.5f, 1.f) );
// //		m_psoTest[i]->SetAction( ACT_IDLE1 - (i%4) );
// // 		const ID idAct = m_psoTest[i]->GetidActByRandom();
// // 		if( idAct )
// // 			m_psoTest[i]->SetAction( idAct );
// // 		m_psoTest[i]->SetfAlpha( xRandomF( 0.2f, 1.f ) );
// //		m_psoTest[i]->SetRotateZ( (float)xRandom( 360 ) );
// 		// 		m_psoTest[i]->SetDrawMode( (xDM_TYPE)(xDM_NORMAL + xRandom(5)) );
// 	}
// 	XGAME::xReward reward;
// 	reward.SetHero( 100 );
// 	float scale = 0.5f;
// 	XE::VEC2 vPos( 100, 100 );
// 	for( int i = 0; i < 3; ++i ) {
// 		auto pCtrl = new XWndStoragyItemElem( vPos, reward, false );
// //		pCtrl->SetbShowNum( true );
// 		pCtrl->SetLevel( 10 );
// 		pCtrl->SetScaleLocal( scale, scale );
// 		m_aryCtrl.push_back( pCtrl );
// 		Add( pCtrl );
// 		vPos.x += 200.f;
// 		scale *= 2.f;
// 	}
// 	m_aryCtrl.push_back(  );
// 	m_aryCtrl.push_back( new XWndStoragyItemElem( XE::VEC2( 200, 100 ), reward, true ) );
// 	m_aryCtrl.push_back( new XWndStoragyItemElem( XE::VEC2( 400, 100 ), reward, true ) );
// 	//	m_pCtrl->SetLevel( 10 );
// 	m_pCtrl->SetbShowNum( true );
// 	Add( m_pCtrl );
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneTest::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneTest::Process( float dt ) 
{ 
#ifdef _XTEST
	if( m_psoTest ) {
		for( int i = 0; i < MAX_SPR1 * MAX_SPR2; ++i ) {
			auto pSprObj = m_psoTest[i];
			if( pSprObj ) {
				if( pSprObj->GetpObjActCurr() == nullptr ) {
					const ID idAct = m_psoTest[i]->GetidActByRandom();
					if( idAct )
						m_psoTest[i]->SetAction( idAct );
				}
				m_psoTest[i]->FrameMove( dt );
			}
		}
	}
#endif // _XTEST
	//
	return XSceneBase::Process( dt );
}

//
void XSceneTest::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
	//
	if( m_glTexture ) {
		// 256으로 축소해서 찍음
 		GRAPHICS_GL->DrawTexture( m_glTexture, 0, 0, 256.f, 256.f, FALSE );
		{ auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR ); }
	}
#ifdef _XTEST
		MATRIX mWorld;
		XE::VEC2 vPos( 200, 200 );
// 		for( int i = 0; i < MAX_SPR2; ++i ) {
// 			for( int k = 0; k < MAX_SPR1; ++k ) {
// 				MatrixTranslation( mWorld, vPos.x + k * 100.f, vPos.y + i * 100.f, 0 );
// 				int idx = i * MAX_SPR1 + k;
// 				m_psoTest[idx]->Draw( 0, 0, mWorld );
// 			}
// 		}
//		vPos = INPUTMNG->GetMousePos();
		//		MatrixTranslation( mWorld, 100.f, 100.f, 0 );
//		SET_RENDERER( XEContent::sGet()->GetpRenderer() ) {
			for( int i = 0; i < MAX_SPR2; ++i ) {
				for( int k = 0; k < MAX_SPR1; ++k ) {
					MatrixTranslation( mWorld, vPos.x + k * 40.f, vPos.y + i * 60.f, 0 );
					int idx = i * MAX_SPR1 + k;
					if( m_psoTest[idx] )
						m_psoTest[idx]->Draw( 0, 0, mWorld );
				}
			}
//		} END_RENDERER;
#endif // _XTEST
}

void XSceneTest::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}

void XSceneTest::OnLButtonUp( float lx, float ly ) 
{
	XSceneBase::OnLButtonUp( lx, ly );
	//
// 	if( m_pRoot == nullptr ) {
// 		m_pRoot = new xSplit::XNode( XE::VEC2( 256 ) );
// // 		m_pRoot->SetRect( XE::VEC2(0), XE::VEC2(255) );
// 	}
// 	const XE::VEC2 sizeImg( (float)xRandom( 32 ) + 32.f, (float)xRandom( 32 ) + 32.f);
// 	DWORD* pCurrImg = new DWORD[ (int)sizeImg.Size() ];
// 	// fill color
// 	{
// 		XCOLOR col = XCOLOR_RGBA( xRandom( 50, 255 ), xRandom( 50, 255 ), xRandom( 50, 255 ), 255 );
// 		for( int i = 0; i < sizeImg.Size(); ++i ) {
// 			pCurrImg[i] = col;
// 		}
// 	}
// 	XE::xRect2 rc;
// 	XE::VEC2 sizeAtlas;
// 	// glTexture아틀라스에 새 사각형을 배치
// 	auto glTexture = XTextureAtlas::sGet()->ArrangeImg( m_glTexture,
// 																											&rc,
// 																											pCurrImg,
// 																											sizeImg,
// 																											XE::xPF_ARGB8888,
// 																											XE::xPF_ARGB8888,
// 																											&sizeAtlas );
// 	if( glTexture ) {
// 		m_glTexture = glTexture;
// 		CONSOLE( "%.1fx%.1f, addNode=%.1fx%.1f-%.1fx%.1f", 
// 						 sizeImg.x, sizeImg.y,
// 						 rc.vLT.x, rc.vLT.y,
// 						 rc.vRB.x, rc.vRB.y );
// 	} else {
// 		CONSOLE( "%.1fx%.1f", sizeImg.x, sizeImg.y );
// 	}
// 	SAFE_DELETE_ARRAY( pCurrImg );

}
void XSceneTest::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

// void XSceneTest::ReplaceTexture( DWORD* pAtlas, const XE::xRECT& rect, int wMax )
// {
// 	XCOLOR col = XCOLOR_RGBA( xRandom(50, 255), xRandom(50, 255), xRandom(50, 255), 255 );
// 	const int w = (int)rect.GetWidth();
// 	const int h = (int)rect.GetHeight();
// 	for( int y = (int)rect.vLT.y; y < (int)rect.vLT.y + h; ++y ) {
// 		for( int x = (int)rect.vLT.x; x < (int)rect.vLT.x + w; ++x ) {
// 			m_pAtlas[ y * wMax + x ] = col;
// 		}
// 	}
// }

void XSceneTest::OnRButtonUp( float lx, float ly ) {
 	XSceneBase::OnRButtonUp( lx, ly );
	XWND_ALERT( "%s", _T( "test" ) );
	// 	::memset( m_pAtlas, 0, (int)c_sizeAtlas.Size() * sizeof( DWORD ) );
// 	m_idCurr = 0;
// 	::glDeleteTextures( 1, &m_glTexture );
// 	m_glTexture = 0;
// 	SAFE_DELETE( m_pRoot );
}

#endif // _TEST
