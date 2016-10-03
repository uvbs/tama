#include "stdafx.h"
#include "XWndTemplate.h"
#include "XWndStorageItemElem.h"
#include "skill/XSkillDat.h"
#include "XSkillMng.h"
#include "XWndWorld.h"
#include "XGame.h"
#include "XSceneTech.h"
#include "XGameWnd.h"
#include "XStruct.h"
#include "XWndResCtrl.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "XWndSpots.h"
#include "XOption.h"
#include "Sprite/SprMng.h"
#include "XSockGameSvr.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif // _CHEAT
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace XE;
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


////////////////////////////////////////////////////////////////
XWndRank::XWndRank( const XVector<XSceneWorld::xRankInfo*>& aryRank )
	: XWndPopup( _T( "rank.xml" ), "popup_rank" )
{
	Init();
	SetbModal( TRUE );
	XSceneWorld::xRankInfo* pMe = nullptr;
	for( auto pRank : aryRank ) {
		if( pRank->m_idAccount == ACCOUNT->GetidAccount() ) {
			pMe = pRank;
			m_rankMe = pRank->m_nRank;
			break;
		}
	}
	auto pWndList = xGET_LIST_CTRL( this, "list.user.rank" );
	if( pWndList ) {
//		for( int i = 0; i < (int)aryRank.size(); i++ ){
		ID idWnd = 1;
		for( auto pRank : aryRank ) {
			auto pElem = new XWndRankingElem( pRank->m_nRank
																			, pRank->m_strName.c_str()
																			, pRank->m_nLadderPoint );
			pWndList->AddItem( idWnd++, pElem );
		}
	}
	//
	if( pMe ) {
		auto pElem = new XWndRankingElem( pMe->m_nRank
																			, pMe->m_strName.c_str()
																			, pMe->m_nLadderPoint );
	//	pElem->SetPosLocal( )
		Add( pElem );
	}
}

void XWndRank::Update()
{
	XWndPopup::Update();
}

//////////////////////////////////////////////////////////////////////////
XWndRankingElem::XWndRankingElem( int nRank, LPCTSTR szName, int nLadder )
	: XWnd( 1, 0, 331, 14 )
{
	Init();
	auto 
	pText = new XWndTextString( XE::VEC2( 0, 0 ), XE::Format( _T( "%d" ), nRank ), FONT_NANUM_BOLD, 30.f );
	pText->SetLineLength( 52.f );
	pText->SetAlign( XE::xALIGN_HCENTER );
	Add( pText );
	pText = new XWndTextString( XE::VEC2( 52, 0 ), szName, FONT_MNLS, 30.f );
	pText->SetLineLength( 147.f );
	pText->SetAlign( XE::xALIGN_HCENTER );
	Add( pText );
	pText = new XWndTextString( XE::VEC2( 199, 0 ), XE::Format( _T( "%d" ), nLadder ), FONT_NANUM_BOLD, 30.f );
	pText->SetLineLength( 116.f );
	pText->SetAlign( XE::xALIGN_HCENTER );
	Add( pText );
}

////////////////////////////////////////////////////////////////
XWndMail::XWndMail()
	: XWndPopup( _T("mail.xml"), "popup_mail")
{
	Init();
	SetbModal( TRUE );
	const auto& listPost = ACCOUNT->GetlistPost();
	const auto sizeMail = listPost.size();
	xSET_SHOW( this, "text.no.mail", sizeMail == 0 );		// 받은 메일이 없음.
	auto pWndList = xGET_LIST_CTRL( this, "list.mail" );
	if( pWndList ) {
		pWndList->SetbShow( sizeMail > 0 );
		int idx = 0;
		//
		XE::VEC2 vPos;
		for( auto pPostInfo : listPost ) {
			auto pElem = new XWndMailElem( pPostInfo );
			pElem->SetpPostInfo( pPostInfo );
			pElem->SetTitle( pPostInfo->GetstrTitle() );
//			pElem->SetPosLocal( XE::VEC2( 0, idx * ( pElem->GetpSurface()->GetHeight() + 10 ) ) );
//			pWndList->AddItem( idx + 1, pElem );
			pElem->SetstrIdentifierf( "mail.%x", pPostInfo->GetsnPost() );
			pWndList->AddItem( pElem );
//			pWndList->Add( idx + 1, pElem );
			bool bAdd = pPostInfo->GetnumItems() > 0;
			if( bAdd ) {
// 				auto pWndClip = new XWndImage( PATH_UI( "mail_attachment.png" )
// 																	, XE::xPF_ARGB8888
// 																	, pElem->GetpSurface()->GetWidth() - 43.0f, 0.f );
				const auto sizeElem = pElem->GetpSurface()->GetSize();
				auto pWndClip = new XWndSprObj(_T("ui_mail_presentbox.spr"), 1
																			, sizeElem.w - 20.0f, sizeElem.h * 0.5f );
				pElem->Add( pWndClip );
//				pWndClip->AutoLayoutVCenter();
// 				pElem->SetY( vPos.y );
// 				vPos.y += sizeElem.h * 1.2f;
			}
			++idx;
		}// END_LOOP;
		pWndList->SetEvent( XWM_SELECT_ELEM
			, SCENE_WORLD, &XSceneWorld::OnClickMailboxElem );
	}
}

//////////////////////////////////////////////////////////////////////////
XWndMailElem::XWndMailElem( XPostInfo* pPostInfo )
	: XWndImage( PATH_UI( "legion_unit_blank_mail.png" ), xPF_ARGB8888, 0.f, 0.f )
{
	Init();

#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pWndText = new XWndTextString( XE::VEC2(0)
																			, XFORMAT("%d", pPostInfo->GetsnPost()) );
		Add( pWndText );
	}
#endif // _CHEAT
}

void XWndMailElem::SetTitle( LPCTSTR title )
{
	m_pTitle = new XWndTextString( XE::VEC2(0), title, FONT_NANUM, 30, XCOLOR_WHITE );
	Add( m_pTitle );
	m_pTitle->AutoLayoutCenter();
}

////////////////////////////////////////////////////////////////

XWndMailEach::XWndMailEach( XSPAccConst spAcc, ID snPost )
	: XWndPopup( _T("mail.xml"), "popup_each" )
{
	Init();
	m_spAcc = spAcc;
	m_snPost = snPost;
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pWndText = new XWndTextString( XE::VEC2(10,10)
																			, XFORMAT("snPost:%d", snPost) );
		Add( pWndText );
	}
#endif // _CHEAT
}

// BOOL XWndMailEach::OnCreate()
// {
// 	return XWndPopup::OnCreate();
// }

// void XWndMailEach::Update()
// {
// 	XWndPopup::Update();
// }

//////////////////////////////////////////////////////////////////////////
XWndProfileForSpot::XWndProfileForSpot( XSpot* m_pBaseSpot, const std::string& strFbUserId )
{
	Init();
	XBREAK( strFbUserId.empty() );		// 페북계정이 없으면 액자는 아예뜨면 안됨.
	m_strcFbUserId = strFbUserId;
	m_vwSpot = m_pBaseSpot->GetpBaseProp()->vWorld;
	m_idSpot = m_pBaseSpot->GetidSpot();
	// pImg는 여기서 보관하지 않는다. 어차피 쉐어드ptr이 아니라 밖에서 파괴되면 무용지물 이기 때문.
	m_spLayoutObj = std::shared_ptr<XLayoutObj>( new XLayoutObj( _T( "profile_picture.xml" ) ) );
	m_spLayoutObj->CreateLayout( "profile", this );
	// this의 최대 사이즈로 this의 사이즈를 정한다.
	auto sizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocal );
	XE::VEC2 vLT;
	vLT.x = m_vwSpot.x -( sizeLocal.w	* 0.5f );
	vLT.y = m_vwSpot.y - sizeLocal.h - 20.f;
	SetPosLocal( vLT );
	// 10초 표시 10초 히든
	m_timerFade.Set( 2 );
	m_State = -1;
	m_timerDelay.Set( (float)(1 + xRandom(10)) );
}
void XWndProfileForSpot::SetProfileImage( XSurface *psfcProfile )
{
	if( psfcProfile == nullptr ) {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
		m_psfcProfile = IMAGE_MNG->Load( PATH_UI("fb_empty.png") );
	} else {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
		m_psfcProfile = psfcProfile;
		m_psfcProfile->IncRefCnt();
	}
	if( m_psfcProfile ) {
		// 이미지 컨트롤의 서피스를 교체한다.
		auto pImg = xGET_IMAGE_CTRL( this, "img.profile" );
		if( pImg && psfcProfile )
				pImg->SetSurface( psfcProfile );
	}
}

int XWndProfileForSpot::Process( float dt )
{
	if( m_State == -1 ) {
		m_State = 0;
		m_timerDelay.Set( (float)( 1 + xRandom( 10 ) ) );
		SetAlphaLocal( 0 );
	}
	if( m_State == 0 ) {		// 없어진상태
		if( m_timerDelay.IsOver() ) {
			m_State = 1;				// 나타나기 시작
			m_timerFade.Set( 2 );
		}
	} else
	if( m_State == 1 ) {		// 나타나는중
		float lerp = m_timerFade.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		float alpha = XE::xiLinearLerp( lerp );
		SetAlphaLocal( alpha );
		if( lerp >= 1.f ) {
			SetAlphaLocal( 1.f );
			m_State = 2;
			m_timerDelay.Set( 10 );
		}
	} else
	if( m_State == 2 ) {		// 다 나타남
		if( m_timerDelay.IsOver() ) {
			m_State = 3;
			m_timerFade.Set( 2 );
		}
	} else
	if( m_State == 3 ) {		// 사라지는중
		float lerp = m_timerFade.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		float alpha = XE::xiLinearLerp( 1.f - lerp );
		SetAlphaLocal( alpha );
		if( lerp >= 1.f ) {
			SetAlphaLocal( 0 );
			m_State = 0;
			m_timerDelay.Set( 10 );
		}
	}

	return XWnd::Process( dt );
}

void XWndProfileForSpot::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
}

////////////////////////////////////////////////////////////////
XWndHello::XWndHello( ID idSpot, const XE::VEC2& vwSpot )
: XWnd( vwSpot )
//	: XWndImage( PATH_UI( "ui_say.png" ), vwSpot )
{
	Init();
	m_idSpot = idSpot;
	m_vSpot = vwSpot;
	auto 
	pImg = new XWndImage( PATH_UI("ui_say.png"), XE::VEC2(0) );
	pImg->SetstrIdentifier("img.bg");
	Add( pImg );
	pImg = new XWndImage( PATH_UI( "ui_say_tail.png" ), XE::VEC2( 0 ) );
	pImg->SetstrIdentifier( "img.bg.tail" );
	Add( pImg );
	const auto sizeThis = pImg->GetSizeFinal();
	SetSizeLocal( sizeThis );
	SetPosLocal( vwSpot );
	auto pText = new XWndTextString( XE::VEC2( 0, 2 ), _T( "" ), FONT_MNLS, 15.f, XCOLOR_BLACK );
	pText->SetstrIdentifier( "text.hello" );
	pText->SetLineLength( sizeThis.w );
//	pText->SetAlignHCenter();
	Add( pText );
	m_timerDelay.Set( (float)(xRandom(5)+1) );
	m_State = 0;
	SetAutoUpdate( 0.25f );
}

void XWndHello::SetText( const _tstring& strText )
{
	m_strHello = strText;
	xSET_TEXT( this, "text.hello", strText );
	// 말풍선 크기 조절
	UpdateSize();
	// 말풍선 위치 조절
	UpdatePos();
}

void XWndHello::Update()
{
	UpdateSize();
	UpdatePos();
	XWnd::Update();
}

int XWndHello::Process( float dt )
{
	if( m_State == 0 ) {
		SetbShow( true );
		if( m_timerDelay.IsOver() ) {
			m_timerDelay.Set( 5.f );
			m_State = 1;
		}
	} else {
		SetbShow( false );
		if( m_timerDelay.IsOver() ) {
			m_timerDelay.Set( 5.f );
			m_State = 0;
		}
	}
	return XWnd::Process( dt );
}

/**
 @brief 윈도우 위치를 재 조정한다.
*/
void XWndHello::UpdatePos()
{
	do {
		auto pRoot = SCENE_WORLD->GetpLayerByProfilePictures();	// 프로필사진 전용레이어에 설치.
		if( !pRoot ) 
			break;
		auto pImgBg = xGET_IMAGE_CTRL( this, "img.bg" );
		if( !pImgBg )
			break;
		auto pText = xGET_TEXT_CTRL( this, "text.hello" );
		if( !pText ) 
			break;
		const auto _sizeBgScaled = pImgBg->GetSizeLocal();		// 검증용.
		const auto sizeHello = _sizeBgScaled;	// 확/축된 상태의 말풍선 전체 크기
		auto vLT = m_vSpot;
		auto pWndProfile = SafeCast2<XWndProfileForSpot*>( pRoot->Findf( "wnd.profile.%d", m_idSpot ) );
		if( pWndProfile && pWndProfile->IsShowProfile() ) {
			// 액자가 있는상태
			const auto vWndProfile = pWndProfile->GetPosLocal();
			vLT.x = -( sizeHello.w * 0.5f );
			vLT.y = (vWndProfile.y - m_vSpot.y) - sizeHello.h;
		} else {
			// 액자가 없거나 사라진 상태
			vLT.x = -( sizeHello.w * 0.5f );
			vLT.y = -(40.f + sizeHello.h);
		}
		vLT += m_vSpot;
		SetPosLocal( vLT );
		SetSizeLocal( _sizeBgScaled );
	} while (0);
}

void XWndHello::UpdateSize()
{
	do {
		auto pRoot = SCENE_WORLD->GetpLayerByProfilePictures();	// 프로필사진 전용레이어에 설치.
		if( !pRoot ) 
			break;
		auto pImgBg = xGET_IMAGE_CTRL( this, "img.bg" );
		if( !pImgBg )
			break;
		/*
		.일단 가로 40문자 기준으로 FTGL에게 렌더링시켜서 폰트 레이아웃크기를 구함
		.이 크기를 기준으로 ui_say.png를 얼마나 확대축소 해야하는지 계산
		.렌더링된 폰트를 말풍선 안에 배치
		*/
		auto pText = xGET_TEXT_CTRL( this, "text.hello" );
		if( !pText ) 
			break;
// 		const XE::VEC2 sizeTextOrig( 84, 11 );		// 원본그림에서 순수 텍스트 영역의 크기
		const XE::VEC2 sizeTextOrig( 80, 8 );		// 원본그림에서 순수 텍스트 영역의 크기
		const XE::VEC2 sizeMaxText( 113, 29 );		// 최대로 늘어난 말풍선에서 텍스트영역의 크기
		pText->SetLineLength( sizeMaxText.w );
//		const auto sizeBgOrig = pImgBg->GetSizeLocal();	// 말풍선 원본 크기
		const auto sizeBgOrig = pImgBg->GetSizeLocalNoTrans();
		const auto sizeText = pText->GetSizeNoTransLayout();		// 찍어야할 텍스트영역 크기
		auto scale = sizeText / sizeTextOrig;		// 확축해야할 텍스트영역 비율
		if( scale.x < 0.5f )
			scale.x = 0.5f;
		if( scale.y < 1.f )
			scale.y = 1.f;
// 		sizeBgScaled.w = sizeText.w + ( 5.f + 9.f ) * scale.x;	// 좌우로 여분 x픽셀씩
// 		sizeBgScaled.h = sizeText.h + ( 2.f + 4.f ) * scale.y;	// 상하 여분
//		const auto vLTBg = -(sizeBgScaled * 0.5f);		// 원점기준 말풍선이 찍혀야할 좌상귀
		pImgBg->SetScaleLocal( scale );
		pImgBg->SetPosLocal( 0 );
// 		pText->SetPosLocal( XE::VEC2( 5, 4 ) * scale );
		const XE::VEC2 sizeBgScaled = pImgBg->GetSizeLocal();	// 확대되었을때 크기
		XE::VEC2 vLTText;
		vLTText.x = (sizeBgScaled.w * 0.5f) - (sizeText.w * 0.5f);
		vLTText.y = ((sizeBgScaled.h - (3.f * scale.y)) * 0.5f) - (sizeText.h * 0.5f);
		pText->SetPosLocal( vLTText );
		auto pImgTail = xGET_IMAGE_CTRL( this, "img.bg.tail" );
		if( pImgTail ) {
			const auto sizeTail = pImgTail->GetSizeLocal();
			XE::VEC2 vTail;
			vTail.x = (sizeBgScaled.w * 0.5f) - (sizeTail.x * 0.5f);
			vTail.y = 12.f * scale.y;
			pImgTail->SetPosLocal( vTail );
			
		}
	} while (0);
}

void XWndHello::OnAutoUpdate()
{
	UpdatePos();
}

////////////////////////////////////////////////////////////////
bool XWndBattleLog::s_bAlert = false;   
XWndBattleLog::XWndBattleLog()
	: XWndPopup( _T( "layout_log.xml" ), "popup_log" )
{
	Init();
	SetCancelButton( "butt.close" );
	auto pButtAtk = xSetButtHander( this, this, "butt.tab.attack", &XWndBattleLog::OnClickLogTab, 1 );
	auto pButtDef = xSetButtHander( this, this, "butt.tab.defense", &XWndBattleLog::OnClickLogTab, 0 );
	SetbUpdate( true );
	m_bAttackLog = true;
	if( m_bAttackLog ) {
		pButtAtk->SetPush( TRUE );
		pButtDef->SetPush( FALSE );
	} else {
		pButtAtk->SetPush( FALSE );
		pButtDef->SetPush( TRUE );
	}
}

void XWndBattleLog::Destroy() 
{
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( true );
}

void XWndBattleLog::Update()
{
	auto pButtDef = Find( "butt.tab.defense" );
	if( pButtDef ) {
		if( s_bAlert ) {
			// 느낌표가 안붙어 있으면 달아준다.
			if( pButtDef->Find( "icon.alert" ) == nullptr ) {
				auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), 1, 80, 6 );
				pMark->SetstrIdentifier( "icon.alert" );
				pButtDef->Add( pMark );
			}
		}
		else {
			// 느낌표 삭제
			pButtDef->DestroyWndByIdentifier( "icon.alert" );
		}
	}
	auto pList = xGET_LIST_CTRL( this, "list.log" );
	pList->DestroyAllItem();
	//
	if( m_bAttackLog ) {
		auto plistLog = ACCOUNT->GetlistBattleLogAttack();
		ID idWnd = 1;
		//
		for( auto itor = plistLog.rbegin(); itor != plistLog.rend(); ++itor ) {
			auto& log = ( *itor );
			auto pElem = new XWnd( GetpLayout(), "elem" );
			_tstring strName = log.strName;
			if( log.bWin )
				strName += XFORMAT(" %s!", XTEXT(2273));		// 승리!
			else
				strName += XFORMAT(" %s!", XTEXT(2274));		// 패배!
#ifdef _CHEAT
			if( XAPP->m_bDebugMode ) {
				strName += XFORMAT( "(%d)", log.idEnemy );
			}
#endif 
			xSET_TEXT( pElem, "text.name.target", strName );
			xSET_TEXT( pElem, "text.level.target", XFORMAT("%d", log.m_lvAcc) );
			xSET_TEXT( pElem, "text.score.target", XE::NumberToMoneyString( log.score ) );
			xSET_TEXT( pElem, "text.power.target", XE::NumberToMoneyString( log.powerBattle ) );
			if( log.sec ) {
				auto secPass = XTimer2::sGetTime() - log.sec;
				_tstring strTime = XSceneTech::_sGetResearchTime( secPass );
				strTime += _T( " " );
				strTime += XTEXT( 2118 );   // ~전
				xSET_TEXT( pElem, "text.time", strTime );
			}
//			XE::VEC2 v( 139, 4 );
			auto pWndRes = pElem->Find( "wnd.res1" );
			if( pWndRes ) {
				auto pWndReses = new XWndResourceCtrl();
				pWndRes->Add( pWndReses );
				for( int i = 0; i < 2; ++i ) {
					if( i >= (int)log.aryLoot.size() )
						break;
					const auto& res = log.aryLoot[i];
					pWndReses->AddRes( res );
				}
				pWndReses->AutoLayoutCenter();
			}
			pWndRes = pElem->Find( "wnd.res2" );
			if( pWndRes ) {
				if( log.aryLoot.size() >= 3 ) {
					auto pWndReses = new XWndResourceCtrl();
					pWndReses->SetScaleLocal( 0.9f );
					pWndRes->Add( pWndReses );
					for( int i = 2; i < 5; ++i ) {
						if( i >= (int)log.aryLoot.size() ) 
							break;
						const auto& res = log.aryLoot[i];
						pWndReses->AddRes( res );
					}
					pWndReses->AutoLayoutCenter();
				}
			}
// 			xSET_SHOW( pElem, "butt.replay", false );
// 			xSET_SHOW( pElem, "butt.revenge", false );
			pList->AddItem( idWnd++, pElem );
		}
	}	else {
		auto plistLog = ACCOUNT->GetlistBattleLogDefense();
		ID idWnd = 1;
		//for( auto& log : plistLog )
		for( auto itor = plistLog.rbegin(); itor != plistLog.rend(); ++itor )
		{
			auto& log = (*itor);
			auto pElem = new XWnd( GetpLayout(), "elem" );
			_tstring strName = log.strName;
			if( log.bWin )
				strName += XFORMAT( "    vs %s!", XTEXT( 2273 ) );		// 승리!
			else
				strName += XFORMAT( "    vs %s!", XTEXT( 2274 ) );		// 패배!
			xSET_TEXT( pElem, "text.name.target", strName );
			xSET_TEXT( pElem, "text.level.target", XFORMAT( "%d", log.m_lvAcc ) );
			xSET_TEXT( pElem, "text.score.target", XE::NumberToMoneyString( log.score ) );
			xSET_TEXT( pElem, "text.power.target", XE::NumberToMoneyString( log.powerBattle ) );
			if( log.sec ) {
				auto secPass = XTimer2::sGetTime() - log.sec;
				_tstring strTime = XSceneTech::_sGetResearchTime( secPass );
				strTime += _T( " " );
				strTime += XTEXT( 2118 );   // ~전
				xSET_TEXT( pElem, "text.time", strTime );
			}
//			XE::VEC2 v( 9, 35 );
			auto pWndRes = pElem->Find( "wnd.res1" );
			if( pWndRes ) {
				auto pWndReses = new XWndResourceCtrl();
				pWndRes->Add( pWndReses );
				for( int i = 0; i < 2; ++i ) {
					if( i >= (int)log.aryLoot.size() )
						break;
					const auto& res = log.aryLoot[i];
					pWndReses->AddRes( res );
				}
				pWndReses->AutoLayoutCenter();
			}
			pWndRes = pElem->Find( "wnd.res2" );
			if( pWndRes ) {
				if( log.aryLoot.size() >= 3 ) {
					auto pWndReses = new XWndResourceCtrl();
					pWndReses->SetScaleLocal( 0.9f );
					pWndRes->Add( pWndReses );
					for( int i = 2; i < 5; ++i ) {
						if( i >= (int)log.aryLoot.size() ) 
							break;
						const auto& res = log.aryLoot[i];
						pWndReses->AddRes( res );
					}
					pWndReses->AutoLayoutCenter();
				}
			}
// 			auto pWndReses = new XWndResourceCtrl( v, log.aryLoot, false );
// 			pElem->Add( pWndReses );
// 			xSET_SHOW( pElem, "butt.replay", false );
// 			xSET_SHOW( pElem, "butt.revenge", false );
			pList->AddItem( idWnd++, pElem );
		}
	}
	XWndPopup::Update();
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndBattleLog::OnClickLogTab( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnClickLogTab" );
	//
	m_bAttackLog = ( p1 != 0 );
	if( m_bAttackLog == false )
		s_bAlert = false;   // 확인했으므로 느낌표 해제
	SetbUpdate( true );
	return 1;
}

////////////////////////////////////////////////////////////////
XWndSpotMsg::XWndSpotMsg( ID idSpot, LPCTSTR szText, float size, XCOLOR col )
{
	Init();
	m_strText = szText;
	m_Size = size;
	if( SCENE_WORLD ) {
		if( XASSERT(idSpot) ) {
			XWnd* pWnd = SCENE_WORLD->Find( idSpot );
			if( pWnd ) {
				SetPosLocal( pWnd->GetPosLocal() );
				auto pText = new XWndTextString( XE::VEC2( 0 ), szText, FONT_NANUM, size, col );
				pText->SetStyleStroke();
				Add( pText );
			}
		}
	}
	SetAutoSize();
	m_timerLife.Set( 3.f );
	m_State = 0;
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 v = GetPosLocal();
	v -= vSize / 2.f;
	SetPosLocal( v );
}
XWndSpotMsg::XWndSpotMsg( const XE::VEC2& vPos, LPCTSTR szText, float size, XCOLOR col )
{
	Init();
	m_strText = szText;
	m_Size = size;
	if( SCENE_WORLD ) {
// 		if( idSpot ) {
// 			XWnd* pWnd = SCENE_WORLD->Find( idSpot );
// 			if( pWnd ) {
// 				SetPosLocal( pWnd->GetPosLocal() );
		SetPosLocal( vPos );
				auto pText = new XWndTextString( XE::VEC2( 0 ), szText, FONT_NANUM, size, col );
				pText->SetStyleStroke();
				Add( pText );
// 			}
//		}
	}
	SetAutoSize();
	m_timerLife.Set( 3.f );
	m_State = 0;
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 v = GetPosLocal();
	v -= vSize / 2.f;
	SetPosLocal( v );
}

void XWndSpotMsg::Destroy()
{
}

int XWndSpotMsg::Process( float dt )
{
	XE::VEC2 v = GetPosLocal();
	do {
		if( m_State == 0 ) {
			if( m_timerLife.IsOver() ) {
				++m_State;
				m_timerLife.Set( 2.f );
				SetAlphaLocal( 1.0f );
				break;
			}
			v.y -= 0.3f;
			SetPosLocal( v );
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( lerp );
		} else
		if( m_State == 1 ) {
			SetAlphaLocal( 1.0f );
			if( m_timerLife.IsOver() ) {
				++m_State;
				m_timerLife.Set( 1.f );
				break;
			}
		} else
		if( m_State == 2 ) {
			if( m_timerLife.IsOver() ) {
				SetbDestroy( TRUE );
				break;
			}
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( 1.0f - lerp );
		}
	} while( 0 );
	return XWnd::Process( dt );
}

////////////////////////////////////////////////////////////////
XWndProduceMsg::XWndProduceMsg( const XE::VEC2& vwPos
															, const std::vector<XGAME::xRES_NUM>& aryRes
															, XCOLOR colText )
	: XWndResourceCtrl( vwPos, aryRes, true )
{
//	m_idSpot = idSpot;
	XWndResourceCtrl::SetColText( colText );
	m_timerLife.Set( 2.f );
}

XWndProduceMsg::XWndProduceMsg( ID idSpot
															, const std::vector<XGAME::xRES_NUM>& aryRes
															, XCOLOR colText )
	: XWndResourceCtrl( XE::VEC2(0), aryRes, true )
{
//	m_idSpot = idSpot;
	XWndResourceCtrl::SetColText( colText );
	if( SCENE_WORLD ) {
		auto pWndSpot = SCENE_WORLD->GetpWndSpot( idSpot );
		if( pWndSpot ) {
			const auto sizeCtrl = pWndSpot->GetSizeLocal();
			const auto vwPos = pWndSpot->GetPosLocal() + XE::VEC2( -(sizeCtrl.w * 0.5f), -50 );
			SetPosLocal( vwPos );
		}
	}
	m_timerLife.Set( 2.f );
}

XWndProduceMsg::XWndProduceMsg( const XE::VEC2& vwPos, XGAME::xtResource typeRes, int num, XCOLOR colText )
	: XWndResourceCtrl( vwPos, typeRes, num, colText )
{
	m_timerLife.Set( 2.f );
}

XWndProduceMsg::XWndProduceMsg( ID idSpot, XGAME::xtResource typeRes, int num, XCOLOR colText )
	: XWndResourceCtrl( XE::VEC2(0), typeRes, num, colText )
{
	if( SCENE_WORLD ) {
		auto pWndSpot = SCENE_WORLD->GetpWndSpot( idSpot );
		if( pWndSpot ) {
			const auto sizeCtrl = GetSizeLocal();
			const auto vwPos = pWndSpot->GetPosLocal() + XE::VEC2( -( sizeCtrl.w * 0.5f ), -50 );
			SetPosLocal( vwPos );
		}
	}
	m_timerLife.Set( 2.f );
}

// XWndProduceMsg::XWndProduceMsg( ID idSpot, LPCTSTR szText, float size, XCOLOR col )
// 	: XWndSpotMsg( idSpot, szText, size, col )
// {
// 	Init();
// 	m_timerLife.Set( 2.f );
// 	auto v = GetPosLocal();
// 	v.y -= 25.f;
// 	SetPosLocal( v );
// }
// 
// XWndProduceMsg::XWndProduceMsg( const XE::VEC2& vPos, LPCTSTR szText, float size, XCOLOR col )
// 	: XWndSpotMsg( vPos, szText, size, col )
// {
// 	Init();
// 	m_timerLife.Set( 2.f );
// 	auto v = GetPosLocal();
// 	v.y -= 25.f;
// 	SetPosLocal( v );
// }

int XWndProduceMsg::Process( float dt )
{
	XE::VEC2 v = GetPosLocal();
	do {
		v.y -= 0.3f * dt;
		SetPosLocal( v );
		if( m_State == 0 ) {
			if( m_timerLife.IsOver() ) {
				++m_State;
				m_timerLife.Set( 1.f );
				SetAlphaLocal( 1.0f );
				break;
			}
		} else
		if( m_State == 1 ) {
			if( m_timerLife.IsOver() ) {
				SetbDestroy( TRUE );
				break;
			}
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( 1.0f - lerp );
		}

	} while( 0 );
	return XWndResourceCtrl::Process( dt );
}

////////////////////////////////////////////////////////////////
XWndOption::XWndOption( XSPAcc spAcc )
	: XWndPopup( _T( "option.xml" ), "popup_option" )
{
	Init();
	m_spAcc = spAcc;
	SetbModal( TRUE );
	SetButtHander( this, "img.world.checkbox1", &XWndOption::OnClickSound );
	SetButtHander( this, "img.world.checkbox2", &XWndOption::OnClickLanguage );
	SetButtHander( this, "img.world.checkbox3", &XWndOption::OnClickRegist );
	SetButtHander( this, "img.butt.check4", &XWndOption::OnClickLow );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		XE::VEC2 vPos(369, 271);
		const XE::VEC2 vSize(30, 30);
		auto pButt = new XWndButtonDebug( vPos, vSize + XE::VEC2(10,10), _T( "bLog" ) );
		pButt->SetEvent( XWM_CLICKED, SCENE_WORLD, &XGame::OnClickDebugShowLog, 2 );
		pButt->SetbCheck( XE::GetbAllowLog() );
		Add( pButt );
		vPos.x += vSize.w + 10.f;
		pButt = new XWndButtonDebug( vPos, vSize, _T( "log" ) );
		pButt->SetEvent( XWM_CLICKED, SCENE_WORLD, &XGame::OnClickDebugShowLog, 0 );
		Add( pButt );
		vPos.x += vSize.w + 10.f;
		pButt = new XWndButtonDebug( vPos, vSize, _T( "del log" ) );
		pButt->SetEvent( XWM_CLICKED, SCENE_WORLD, &XGame::OnClickDebugShowLog, 1 );
		Add( pButt );
// 		pButt = new XWndButtonDebug( XE::VEC2( 369 + 80, 271 ), XE::VEC2( 30 ), _T( "low" ) );
// 		pButt->SetbCheck( XAPP->m_bLow );
// 		pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickDebugShowLog, 2 );
// 		Add( pButt );
	}
#endif // _CHEAT
}

BOOL XWndOption::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndOption::Update()
{
	auto pOption = GAME->GetpOption();
	xSET_SHOW( this, "img.world.check1", !pOption->GetbSound() );
	xSET_SHOW( this, "img.world.check2", !m_spAcc->Get_strID().empty() );
	xSET_ENABLE( this, "img.world.checkbox3", m_spAcc->Get_strID().empty() );
	xSET_SHOW( this, "img.world.check4", pOption->GetbLow() );
	auto pImgFlag = xGET_IMAGE_CTRL( this, "img.flag" );
	auto& strLangKey = XE::LANG.GetstrKey();
	if( pImgFlag ) {
		if( strLangKey == _T( "korea" ) ) {
			pImgFlag->SetSurfaceRes( PATH_UI( "option_ko.png" ) );
		} else
		if( strLangKey == _T( "english" ) ) {
			pImgFlag->SetSurfaceRes( PATH_UI( "option_en.png" ) );
		} else {
			pImgFlag->SetSurfaceRes( PATH_UI( "res_question.png" ) );
		}
	}
//	xSET_SHOW( this, "butt.facebook", m_spAcc->GetstrFbUserId().empty() );
	auto pButt = xGET_BUTT_CTRL( this, "butt.facebook" );
	if( pButt ) {
		const bool bRegisteredFacebook = !m_spAcc->GetstrFbUserId().empty();
// 		if( bRegisteredFacebook )
// 			pButt->SetblendFunc( XE::xBF_GRAY );
// 		else
// 			pButt->SetblendFunc( XE::xBF_MULTIPLY );
		pButt->SetbEnable( !bRegisteredFacebook );
//		pButt->SetbShow( m_spAcc->GetstrFbUserId().empty() );
	}

	XWndPopup::Update();
}

/**
 @brief 
*/
int XWndOption::OnClickLow( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLow");
	//
	auto pOption = GAME->GetpOption();
	pOption->TogglebLow();
	static auto s_secSprCache = XSprMng::s_secCache;
	if( pOption->IsbLow() ) {
		XSprMng::s_secCache = 0;
		XSprMng::sGet()->DoFlushCache();
	} else {
		XSprMng::s_secCache = s_secSprCache;
	}
	SetbUpdate( true );
	return 1;
}

/**
 @brief 
*/
int XWndOption::OnClickSound( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickSound");
	//
// 	auto bFlag = GAME->ToggleBGM();
	GAME->ToggleSound();
	GAME->ToggleBGM();
// 	xSET_SHOW( this, "img.world.check1", !bFlag );
	SetbUpdate( true );

	return 1;
}

/**
 @brief 언어선택
*/
int XWndOption::OnClickLanguage(XWnd* pWnd, DWORD p1, DWORD p2)
{
	//언어선택
	if( !Find("popup.select.lang") ) {
		auto pPopup = new XWndPopup( _T("select_language.xml"), "select_lang" );
		if( pPopup ) {
			pPopup->SetstrIdentifier( "popup.select.lang" );
	///<		람다 예제
	//  		pPopup->SetButtHander( pPopup, "butt.ko", []()->void{
	//  			XE::LANG.SetSelectedKey( "english" );
	//  		});
			pPopup->SetButtHander( pPopup, "butt.ko", &XWndOption::OnClickSelectLanguage, 1 );
	///<		람다 예제
	// 			pPopup->SetButtHander( pPopup, "butt.ko", [this](XWnd*,DWORD p1,DWORD)->int{
	// 				GAME->GetpOption()->SetstrcKeyCurrLang( "hello" );
	// 			}, 1 );
			pPopup->SetButtHander( pPopup, "butt.en", &XWndOption::OnClickSelectLanguage, 2 );
		}
		Add( pPopup );
	}
	return 1;
}
/**
 @brief 
*/
int XWndOption::OnClickSelectLanguage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickSelectLanguage");
	//
	std::vector<std::string> aryKeys = { std::string(), std::string("korea"), std::string("english") };
	GAME->GetpOption()->SetstrcKeyCurrLang( aryKeys[p1] );
	XE::LANG.SetSelectedKey( aryKeys[p1] );
// 	SAFE_DELETE( TEXT_TBL );
// 	GAME->LoadTextTable();
	GAME->GetpOption()->Save();
	GAME->OnGotoStart( nullptr, 1, 0 );
	return 1;
}

/**
 @brief 계정 등록 버튼 누름.
*/
int XWndOption::OnClickRegist(XWnd* pWnd, DWORD p1, DWORD p2)
{
	SetbDestroy( true );
	//계정등록
	auto pPopup = new XWndRegistAcc();
	pPopup->SetstrIdentifier( "popup.account.regist" );
	GAME->GetpScene()->Add( pPopup );



// 	XWnd *pPopup = new XWndPopup( _T("regist_acc.xml"), "popup" );
// 	pPopup->SetstrIdentifier("popup.account.regist");
// 	pPopup->SetbModal(TRUE);
// 	Add(pPopup);
// 
// 	auto pImg = SafeCast<XWndImage*>(Find("img.box.email"));
// 	if (pImg)
// 		pImg->SetEvent(XWM_CLICKED, this, &XSceneWorld::OnClickEmailBox);
// 	pImg = SafeCast<XWndImage*, XWnd*>(Find("img.box.password"));
// 	if (pImg)
// 		pImg->SetEvent(XWM_CLICKED, this, &XSceneWorld::OnClickPasswordBox);
// 	pImg = SafeCast<XWndImage*, XWnd*>(Find("img.box.password.verify"));
// 	if (pImg)
// 		pImg->SetEvent(XWM_CLICKED, this, &XSceneWorld::OnClickVerifyBox);
// 
// 	xSET_BUTT_HANDLER(this, "butt.popup.register", &XSceneWorld::OnClickRegister);
// 	xSET_BUTT_HANDLER(this, "butt.close", &XSceneWorld::OnClosePopupBySideMenu);

	return 1;
}

////////////////////////////////////////////////////////////////
XWndRegistAcc::XWndRegistAcc()
	: XWndPopup( _T( "regist_acc.xml" ), "popup" )
{
	Init();
	SetbModal( TRUE );
// 	SetButtHander( this, "img.box.email", &XWndRegistAcc::OnClickEmail );
// 	SetButtHander( this, "img.box.password", &XWndRegistAcc::OnClickPassword, 0 );
//	SetButtHander( this, "img.box.password.verify", &XWndRegistAcc::OnClickPassword, 1 );
	SetButtHander( this, "butt.regist", &XWndRegistAcc::OnClickRegist );
}

BOOL XWndRegistAcc::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndRegistAcc::Update()
{
	XWndPopup::Update();
}

// /**
//  @brief 
// */
// int XWndRegistAcc::OnClickEmail( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("%s", __TFUNC__);
// 	//
// 	
// 	return 1;
// }
// 
// /**
//  @brief 
// */
// int XWndRegistAcc::OnClickPassword( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("%s", __TFUNC__);
// 	//
// 	
// 	return 1;
// }

/**
 @brief 
*/
int XWndRegistAcc::OnClickRegist( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("%s", __TFUNC__);
	//
	auto pEditEmail = SafeCast<XWndEdit*>( Find( "edit.email" ) );
	auto pEditPassword = SafeCast<XWndEdit*>( Find( "edit.password" ) );
	auto pEditPassword2 = SafeCast<XWndEdit*>( Find( "edit.password.verify" ) );
	if( pEditEmail && pEditPassword && pEditPassword2 ) {
		const _tstring strEmail = pEditEmail->GetszString();
		const _tstring strPassword = pEditPassword->GetszString();
		const _tstring strPassword2 = pEditPassword2->GetszString();
		if( strEmail.empty() || strPassword.empty() || strPassword2.empty() )
			return 1;
		if( strPassword != strPassword2 ) {
			XWND_ALERT("%s", "password different");
			return 1;
		}
// 		const std::string cstrID = SZ2C( strEmail );
// 		const std::string cstrPW = SZ2C( strPassword );
// 		const std::string cstrPW2 = SZ2C( strPassword2 );
		GAMESVR_SOCKET->SendReqRegistName( this, strEmail.c_str(), strPassword.c_str() );
	}
	
	return 1;
}

////////////////////////////////////////////////////////////////
XWndSkillTrainComplete::XWndSkillTrainComplete( XHero* pHero, 
																								const _tstring& idsSkill, 
																								int level )
	: XWndPopup( _T( "train_skill_end.xml" ), "popup" )
{
	Init();
	// 스킬 이름과 설명표시
	auto pDat = SKILL_MNG->FindByIds( idsSkill );
	if( XBREAK( pDat == nullptr ) )
		return;
	// 스킬아이콘의 레벨 표시
	auto pCtrl = SafeCast<XWndCircleSkill*>( Find( "ctrl.skill" ) );
	if( XASSERT( pCtrl ) ) {
		pCtrl->SetSkill( pDat, level );
		xSET_SHOW( pCtrl, "spr.light", true );
	}
	const _tstring strName = pDat->GetstrName();
	xSET_TEXT( this, "text.skill.name", strName );
	_tstring strDesc = pDat->GetstrDesc2( level );
	xSET_TEXT( this, "text.skill.desc", strDesc );
	// 영웅 지정
	auto pCtrlHero = ::xGetCtrlHero( this, "ctrl.hero" );
	if( XASSERT( pCtrlHero ) ) {
		pCtrlHero->SetHero( pHero );
	}
	// XXX의 훈련이 끝났습니다.
// 	const _tstring strFormat = XTEXT( 2029 );
// 	xSET_TEXT_FORMAT( this,
// 										"text.notice",
// 										strFormat.c_str(),
// 										strName.c_str() );
}

BOOL XWndSkillTrainComplete::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndSkillTrainComplete::Update()
{

	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
XWndTrainCompleteLevel::XWndTrainCompleteLevel( XHero* pHero )
	: XWndPopup( _T( "train_level_end.xml" ), "popup" )
{
	Init();
	// 영웅 지정
	auto pCtrlHero = ::xGetCtrlHero( this, "ctrl.hero" );
	if( XASSERT( pCtrlHero ) ) {
		pCtrlHero->SetHero( pHero );
		xSET_SHOW( pCtrlHero, "spr.light", true );
	}
}

BOOL XWndTrainCompleteLevel::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndTrainCompleteLevel::Update()
{

	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
XWndTrainCompleteSquad::XWndTrainCompleteSquad( XHero* pHero, 
																								xtUnit unit, 
																								int level )
	: XWndPopup( _T( "train_squad_end.xml" ), "popup" )
{
	Init();
	// 영웅 지정
	auto pCtrlHero = ::xGetCtrlHero( this, "ctrl.hero" );
	if( XASSERT( pCtrlHero ) ) {
		pCtrlHero->SetHero( pHero );
		auto pCtrlUnit = ::xGetCtrlUnit2( this, "ctrl.unit" );
		if( XASSERT( pCtrlUnit ) ) {
			pCtrlUnit->SetUnit( unit, level );
		}
		//xSET_SHOW( pCtrlHero, "spr.light", true );
	}
}

BOOL XWndTrainCompleteSquad::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndTrainCompleteSquad::Update()
{

	XWndPopup::Update();
}

