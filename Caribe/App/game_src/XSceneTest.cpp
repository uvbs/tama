#include "stdafx.h"
#include "XSceneTest.h"
#include "XGame.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "XFramework/XSplitNode.h"
#include "OpenGL2/XTextureAtlas.h"
#include "Sprite/SprObj.h"
#include "etc/XSurfaceDef.h"
#include "Opengl2/XBatchRenderer.h"
#include "XGameWnd.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSplit;
const char* s_files[] = {
// "arrow.spr",
// "arrow2.spr",
// "arrow3.spr",
// "arrow_bleed.spr",
// "arrow_critical.spr",
"arrow_paraly.spr",
// "arrow_stuck.spr",
// "arrow_vopal.spr",
// "brilliant.spr",
// "eff_eye1.spr",
// "eff_fire01.spr",
// "eff_flame.spr",
// "eff_freezing.spr",
// "eff_ghost_diesmoke.spr",
//"eff_hit01.spr",
 "eff_hit02.spr",
// "eff_hit_bite.spr",
// "eff_hit_knock.spr",
// "eff_hit_knock_01.spr",
// "eff_hit_sword02.spr",
// "eff_ice01.spr",
// "eff_meteo.spr",
// "eff_ravfire_rava.spr",
// "eff_skill_noticebar.spr",
// "eff_sword_trail03.spr",
// "eff_thunder.spr",
// "eff_ui_battle_gauge.spr",
"hero_bruxa.spr",
"hero_congent.spr",
// "hero_enloque.spr",
// "hero_fei.spr",
// "hero_frio.spr",
// "hero_hetribs.spr",
// "hero_laynar.spr",
// "hero_ollen.spr",
// "hero_range02.spr",
// "hero_range03.spr",
// "hero_sinceridade.spr",
// "hero_skar.spr",
// "hero_speed01.spr",
// "hero_speed03.spr",
// "hero_tanker01.spr",
// "hero_tanker03.spr",
// "hero_unluny.spr",
// "hero_zanta.spr",
// "rock.spr",
// "rock_adaman.spr",
// "select_unit.spr",
// "sfx_arrow_power.spr",
// "sfx_attack_spear.spr",
"sfx_awaken.spr",
// "sfx_bastard_blitz.spr",
// "sfx_blind.spr",
// "sfx_blood_strike.spr",
// "sfx_buf_blue.spr",
// "sfx_buf_green.spr",
// "sfx_buf_red.spr",
// "sfx_burn.spr",
// "sfx_circle_woong.spr",
// "sfx_close_combat.spr",
// "sfx_confusion.spr",
// "sfx_cowbell.spr",
// "sfx_crescent.spr",
// "sfx_death_eye.spr",
// "sfx_debuf_blue.spr",
// "sfx_debuf_green.spr",
// "sfx_debuf_red.spr",
// "sfx_destruct.spr",
// "sfx_dust.spr",
// "sfx_earthquake.spr",
// "sfx_entangle.spr",
// "sfx_firestrom01.spr",
// "sfx_fire_0.spr",
// "sfx_fire_blue.spr",
// "sfx_fire_particle.spr",
// "sfx_flag.spr",
// "sfx_flame_armor.spr",
// "sfx_frozen.spr",
// "sfx_fury.spr",
// "sfx_glow.spr",
// "sfx_glow_blue.spr",
// "sfx_glow_box32_woong.spr",
// "sfx_glow_green.spr",
// "sfx_glow_red.spr",
// "sfx_glow_yellow.spr",
// "sfx_haste.spr",
// "sfx_heal.spr",
// "sfx_heal2.spr",
// "sfx_heal2_2.spr",
// "sfx_healing2.spr",
// "sfx_holy_pray.spr",
// "sfx_horror.spr",
// "sfx_last_hit.spr",
// "sfx_lightning01.spr",
// "sfx_mad_dog.spr",
// "sfx_multishot.spr",
// "sfx_nova.spr",
// "sfx_paraly_explo.spr",
// "sfx_particle.spr",
// "sfx_phoenix.spr",
// "sfx_photosynthesis.spr",
// "sfx_phyton.spr",
// "sfx_pierce.spr",
// "sfx_power_bolt.spr",
// "sfx_pvp_attack.spr",
// "sfx_pvp_buff.spr",
// "sfx_pvp_defense.spr",
// "sfx_rage.spr",
// "sfx_rampage.spr",
// "sfx_resurrect.spr",
// "sfx_roar.spr",
// "sfx_rush.spr",
// "sfx_shield.spr",
// "sfx_shield2.spr",
// "sfx_smoke.spr",
// "sfx_star.spr",
// "sfx_stigma.spr",
// "sfx_sudden_attack.spr",
// "sfx_swing.spr",
// "sfx_taunt.spr",
// "sfx_tiger.spr",
// "sfx_transm.spr",
// "sfx_twinkle_cross_cyan.spr",
// "sfx_windstrom.spr",
// "sfx_wind_blade.spr",
// "sfx_wing.spr",
// "sfx_wolf.spr",
// "spear.spr",
// "spot_academy.spr",
// "spot_barrack.spr",
// "spot_campaign.spr",
// "spot_cash.spr",
// "spot_castle.spr",
// "spot_cathedral.spr",
// "spot_daily.spr",
// "spot_eff.spr",
// "spot_embassy.spr",
// "spot_guild_raid.spr",
// "spot_hero_camp.spr",
// "spot_indicate.spr",
// "spot_jewel.spr",
// "spot_labor.spr",
// "spot_mandrake.spr",
// "spot_market.spr",
// "spot_medal_camp.spr",
// "spot_npc.spr",
// "spot_npc3.spr",
// "spot_npc4.spr",
// "spot_npc_boss.spr",
// "spot_npc_empty.spr",
// "spot_private_raid.spr",
// "spot_special.spr",
// "spot_sulfur.spr",
// "spot_tavern.spr",
// "spot_visit.spr",
// "stamp.spr",
// "unit_archer.spr",
// "unit_archer2.spr",
// "unit_cyclops.spr",
// "unit_cyclops2.spr",
// "unit_fallen_angel2.spr",
// "unit_golem2.spr",
// "unit_lycan.spr",
// "unit_lycan2.spr",
// "unit_minotaur.spr",
// "unit_minotaur2.spr",
// "unit_paladin2.spr",
// "unit_spearman.spr",
// "unit_spearman2.spr",
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

XSceneTest::XSceneTest( XGame *pGame, SceneParamPtr& spParam ) 
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
#ifdef _XTEST
	for( int i = 0; i < MAX_SPR1 * MAX_SPR2; ++i ) {
		const int maxFiles = XNUM_ARRAY( s_files );
		const _tstring strFile = C2SZ( s_files[xRandom(maxFiles)] );
		m_psoTest[i] = new XSprObj( _T( "title.spr" ), XE::xHSL(), true, false, false, nullptr );
// 		m_psoTest[i] = new XSprObj( strFile );
		auto pso = m_psoTest[i];
//		pso->SetAction( 4 );
//		pso->SetScale( xRandomF( 0.5f, 1.f) );
//		m_psoTest[i]->SetAction( ACT_IDLE1 - (i%4) );
// 		const ID idAct = m_psoTest[i]->GetidActByRandom();
// 		if( idAct )
// 			m_psoTest[i]->SetAction( idAct );
// 		m_psoTest[i]->SetfAlpha( xRandomF( 0.2f, 1.f ) );
//		m_psoTest[i]->SetRotateZ( (float)xRandom( 360 ) );
		// 		m_psoTest[i]->SetDrawMode( (xDM_TYPE)(xDM_NORMAL + xRandom(5)) );
	}
#endif // _XTEST
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
			if( pSprObj->GetpObjActCurr() == nullptr ) {
				const ID idAct = m_psoTest[i]->GetidActByRandom();
				if( idAct )
					m_psoTest[i]->SetAction( idAct );
			}
			m_psoTest[i]->FrameMove( dt );
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
	if( m_psoTest ) {
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
		SET_RENDERER( XEContent::sGet()->GetpRenderer() ) {
			for( int i = 0; i < MAX_SPR2; ++i ) {
				for( int k = 0; k < MAX_SPR1; ++k ) {
					MatrixTranslation( mWorld, vPos.x + k * 40.f, vPos.y + i * 60.f, 0 );
					int idx = i * MAX_SPR1 + k;
					m_psoTest[idx]->Draw( 0, 0, mWorld );
				}
			}
		} END_RENDERER;
	}
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
// 	{ auto glErr = glGetError();
// 	XASSERT( glErr == GL_NO_ERROR ); }
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

