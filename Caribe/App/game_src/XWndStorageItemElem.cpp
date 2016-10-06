#include "stdafx.h"
#include "constGame.h"
#include "XPropItem.h"
//#include "XSoundMng.h"
//#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
//#include "XFramework/XConstant.h"
//#include "XWindow.h"
#include "XImageMng.h"
#include "XFramework/client/XLayout.h"
#include "XAccount.h"
//#include "XSceneWorld.h"
#include "XBaseItem.h"
//#include "XSockGameSvr.h"
//#include "XSceneUnitOrg.h"
#include "XHero.h"
#include "XWndTemplate.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"
#include "XWndStorageItemElem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

class XSceneStorage;
//////////////////////////////////////////////////////////////////////////
XWndStoragyItemElem* XWndStoragyItemElem::sUpdateCtrl( XWnd* pParent, 
																											 const XE::VEC2& vPos, 
																											 XHero* pHero, 
																											 const std::string& ids )
{
	XWndStoragyItemElem* pWndHero = nullptr;
	auto pWnd = pParent->Find( ids );
	if( pWnd ) {
		pWndHero = SafeCast2<XWndStoragyItemElem*>( pWnd );
	}
	if( pWndHero == nullptr ) {
		pWndHero = new XWndStoragyItemElem( vPos, pHero );
		pWndHero->SetstrIdentifier( ids );
		pParent->Add( pWndHero );
	}
	return pWndHero;
}

/**
 @brief pHero초상화 UI를 pWndList에 추가시킨다.
*/
XWndStoragyItemElem* XWndStoragyItemElem::sUpdateCtrlToList( XWndList *pWndList, 
																														 XHero *pHero )
{
	if( pHero == nullptr )
		return nullptr;
	std::string idsElem = XE::Format( "elem.hero.%s", SZ2C( pHero->GetstrIdentifer() ) );
	auto pWndElem
		= SafeCast2<XWndStoragyItemElem*>( pWndList->Find( idsElem ) );
	if( pWndElem == nullptr ) {
		pWndElem = new XWndStoragyItemElem( XE::VEC2(0), pHero );
		pWndElem->SetstrIdentifier( idsElem );
		pWndList->AddItem( pHero->GetsnHero(), pWndElem );
	}
	return pWndElem;
}

void XWndStoragyItemElem::Destory()
{
	SAFE_DELETE( m_pfoName );
	SAFE_DELETE( m_pfoLevel );
	SAFE_DELETE( m_pfoNum );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgGradation );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcEquip );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgLevel );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgName );
	SAFE_RELEASE2( IMAGE_MNG, m_pBG );
	SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
	for( auto pImg : m_aryStar ) {
		SAFE_RELEASE2( IMAGE_MNG, pImg );
	}
	SAFE_RELEASE2( IMAGE_MNG, m_psfcSelected );
	SAFE_RELEASE2( IMAGE_MNG, m_pSoulStone );
}

/**
 @brief 빈칸만 있는 버전
*/
XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos )
	: XWnd( vPos )
{
	Init();
	SetbUpdate( true );
}

/**
 @brief 
 이 컨트롤내에선 배치렌더를 하도록 한다.
*/
XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, 
																					const XGAME::xReward& reward,
																					bool bBatch )
	: XWnd( vPos )
{
	Init();
	m_bBatch = bBatch;
//	m_bBatch = true;
// 	m_Reward = reward;
	switch( reward.rewardType ) {
	case xtReward::xRW_ITEM: {
		if( XASSERT(reward.idReward) )
			m_pProp = PROP_ITEM->GetpPropMutable( reward.idReward );
	} break;
	case xtReward::xRW_GOLD: {
		XBREAK(1);
	} break;
	case xtReward::xRW_RESOURCE: {
		auto szImg = XGAME::GetResourceIconBig( (XGAME::xtResource)reward.idReward );
		if( reward.num > 1 )
			SetNum( reward.num );
		if( XE::IsHave(szImg) )
			m_pItemImg = IMAGE_MNG->Load( XE::MakePath( DIR_ICON, szImg), bBatch, XE::xPF_ARGB4444, true, true );
	} break;
	case xtReward::xRW_CASH: {
		m_pItemImg = IMAGE_MNG->Load( PATH_UI("shop_cash1.png"), bBatch, XE::xPF_ARGB4444, true, true );
	} break;
	case xtReward::xRW_HERO: {
		if( reward.idReward ) {
			SetHero( reward.idReward );
		}
	} break;
	case xtReward::xRW_GUILD_POINT: {
		m_pItemImg = IMAGE_MNG->Load( PATH_UI( "guild_coin.png" ), bBatch, XE::xPF_ARGB4444, true, true );
	} break;
	}
	m_Reward = reward;		// 기존거와 비교하기위해 업데이트가 끝난후 대입함.
	SetbUpdate( true );
}

XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, 
																					const XGAME::xReward& reward )
	: XWndStoragyItemElem( vPos, reward, false ) { }


XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, XHero *pHero )
	: XWndStoragyItemElem( vPos, XGAME::xReward(pHero) )
{
	SetHero( pHero );
}

XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, 
																					XHero *pHero,
																					bool bBatch )
	: XWndStoragyItemElem( XE::VEC2(0,0), XGAME::xReward( pHero ), bBatch )
{
	SetHero( pHero );

}

XWndStoragyItemElem::XWndStoragyItemElem(XBaseItem* pItem)
{
	Init();
	if( pItem ) {
 		m_pItem = pItem;
		m_Reward.SetItem( pItem->GetidProp(), pItem->GetNum() );
		m_pProp = const_cast<XPropItem::xPROP*>( pItem->GetpProp() );
		m_snItem = pItem->GetsnItem();
	}
	SetbUpdate( true );
}

XWndStoragyItemElem::XWndStoragyItemElem(ID idItem)
{
	Init();
	m_Reward.SetItem( idItem, 1 );
	if (idItem) {
		m_pProp = const_cast<XPropItem::xPROP*>( PROP_ITEM->GetpProp( idItem ));
	}
	SetbUpdate( true );
}

XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, const _tstring& idsItem )
	: XWnd( vPos )
{
	Init();
	auto pProp = PROP_ITEM->GetpProp( idsItem );
	if( XASSERT(pProp) ) {
		m_Reward.SetItem( pProp->idProp, 0 );
//		m_idItem = pProp->idProp;
		m_pProp = pProp;
	}
	SetbUpdate( true );
}

XWndStoragyItemElem::XWndStoragyItemElem( LPCTSTR szImg, int num )
{
	SetItemImg( szImg );
	m_Reward.num = num;
	SetbUpdate( true );
}

BOOL XWndStoragyItemElem::OnCreate()
{
	// 안쌓이는 아이템은 개수표시 안함.
	if( m_pProp && m_pProp->maxStack <= 1 )
		m_Reward.num = 0;
	m_psfcBgGradation = IMAGE_MNG->Load( PATH_UI( "bg_hero.png" ), m_bBatch, XE::xPF_ARGB8888, true, true );
	m_pBG = IMAGE_MNG->Load( PATH_UI( "common_bg_frame.png" ), m_bBatch, XE::xPF_ARGB1555, true, true );
	SetSizeLocal( m_pBG->GetWidth(), m_pBG->GetHeight() );
	// 아이템일경우 별등급표시
	if( m_Reward.GetidItem() ) {
		XBREAK( m_pProp == nullptr );
		if( m_pProp->IsSoul() ) {
			m_pSoulStone = IMAGE_MNG->Load( PATH_UI( "frame_soul.png" ), m_bBatch, XE::xPF_ARGB1555, true, true );
		} else {
			// 영혼석이 아닌경우에만 별 그림
			auto numStar = m_pProp->grade;
			for( int i = 0; i < numStar; ++i ) {
				auto pImg = IMAGE_MNG->Load( PATH_UI( "common_etc_smallstar.png" ), m_bBatch, XE::xPF_ARGB4444, true, true );
				m_aryStar.push_back( pImg );
			}
		}
		m_pItemImg = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, m_pProp->strIcon.c_str() ), m_bBatch, XE::xPF_ARGB1555, true, true );
	}
	return TRUE;
}


void XWndStoragyItemElem::SetHero( ID idProp )
{
	if( m_psfcBgName == nullptr ) {
		m_psfcBgName = IMAGE_MNG->Load( PATH_UI( "bg_name.png" ),
																		m_bBatch,
																		XE::xPF_ARGB4444,
																		true,
																		true );
	}
	if( idProp ) {
		if( m_Reward.GetidHero() != idProp ) {
			// 기존과 다른 영웅이 지정되었을때만 
			auto pProp = PROP_HERO->GetpProp( idProp );
			if( XASSERT( pProp ) ) {
				const _tstring resFace = XE::MakePath( DIR_IMG, pProp->strFace );
				XBREAK( resFace.empty() );
				SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
				m_pItemImg = IMAGE_MNG->Load( resFace,
																			m_bBatch,
																			XE::xPF_ARGB8888,
																			true,		// atlas
																			true ); // async
				SetvScaleItemImg( XE::VEC2( 0.789f, 0.781f ) );
				SetbUpdate( true );
				m_Reward.SetHero( idProp );
			}
		}
	} else {
		SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
		m_Reward.SetHero( 0 );
	}
}

/** //////////////////////////////////////////////////////////////////
 @brief 영웅레벨의 뒤에 빛효과를 켠다.
*/
// void XWndStoragyItemElem::SetHeroLevelLight( bool bFlag )
// {
// 	if( m_psoLight == nullptr ) {
// 		m_psoLight = new XSprObj( _T("brilliant.spr"), 
// 															XE::xHSL(), 1, xRPT_LOOP, true, 
// 															)
// 	}
// }

void XWndStoragyItemElem::SetHero( const XHero* pHero )
{
	if( pHero ) {
		SetbShowNum( false );
		m_Level = pHero->GetLevel();
		m_snHero = pHero->GetsnHero();
		//		m_Reward.SetHero( pHero->GetidProp(), 1 );
		SetHero( pHero->GetidProp() );
		// 영혼석이 아닌경우에만 별 그림
		ReleaseStar();
		if( m_aryStar.size() == 0 ) {
			const auto numStar = pHero->GetGrade();
			for( int i = 0; i < numStar; ++i )
				m_aryStar.Add( IMAGE_MNG->Load( PATH_UI( "common_etc_smallstar.png" ),
				m_bBatch,
				XE::xPF_ARGB4444,
				true, true ) );
		}
		SetbUpdate( true );
	} else {
		ClearHero();
	}
}

void XWndStoragyItemElem::ReleaseStar()
{
	for( auto pImg : m_aryStar ) {
		SAFE_RELEASE2( IMAGE_MNG, pImg );
	}
	m_aryStar.clear();
}

void XWndStoragyItemElem::ClearHero()
{
	m_pPropHero = nullptr;
	m_Reward.SetHero( 0, 0 );
	m_snHero = 0;
	m_Level = 0;
	m_bSelected = false;
	SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
	ReleaseStar();
}

bool XWndStoragyItemElem::IsHero() const
{
	return (/*m_snHero ||*/ m_Reward.GetidHero() != 0);
}

void XWndStoragyItemElem::Update()
{
	const auto vScale = GetScaleLocal();
	if( m_bSelected && m_psfcSelected == nullptr ) {
		if( m_psfcSelected == nullptr )
			m_psfcSelected = IMAGE_MNG->Load( PATH_UI( "common_bg_item_glow.png" ), m_bBatch, XE::xPF_ARGB8888, true, true );
	}
	int num = m_Reward.num;
	if( m_Reward.GetidItem() ) {
		auto pProp = PROP_ITEM->GetpProp( m_Reward.GetidItem() );
		XBREAK( pProp == nullptr );
		int maxStack = pProp->maxStack;
		if( m_pItem ) {
			num = m_pItem->GetNum();	// 실시간 갱신
		}
//		auto pWndEquip = Find( "img.icon.equip" );
		m_bEquip = (m_pItem && ACCOUNT->IsEquip( m_snItem ));
		//
		if( m_bEquip && m_psfcEquip == nullptr) {
			m_psfcEquip = IMAGE_MNG->Load( PATH_UI( "icon_equip.png" ),
																		 m_bBatch,
																		 XE::xPF_ARGB4444,
																		 true,
																		 true );
		}
// 		if( m_bEquip ) {
// 			if( pWndEquip == nullptr ) {
// 				auto pImgEquip = new XWndImage( PATH_ICON( "icon_equip.png" ), 
// 																				m_bBatch, 
// 																				XE::xPF_ARGB4444, 
// 																				XE::VEC2(3, 11) );
// 				pImgEquip->SetstrIdentifier( "img.icon.equip" );
// 				pImgEquip->SetPriority( -100 );
// 				Add( pImgEquip );
// 			} else
// 				pWndEquip->SetbShow( TRUE );
// 		} else
// 			if( pWndEquip )
// 				pWndEquip->SetbShow( FALSE );
	}
	if( IsHero() ) {
		if( m_psfcBgName == nullptr ) {
			m_psfcBgName = IMAGE_MNG->Load( PATH_UI( "bg_name.png" ),
																			m_bBatch,
																			XE::xPF_ARGB4444,
																			true,
																			true );
		}
		if( m_psfcBgLevel == nullptr ) {
			m_psfcBgLevel = IMAGE_MNG->Load( PATH_UI( "bg_level.png" ),
																			 m_bBatch,
																			 XE::xPF_ARGB1555,
																			 true,
																			 true );
		}
		// 이름폰트로딩
		if( m_pfoName == nullptr ) {
//			const float sizeFont = (float)((int)18.f * vScale.y);
			m_pfoName = FONTMNG->CreateFontObj( FONT_NANUM, 18.f );
			m_pfoName->SetbBatch( m_bBatch );
			m_pfoName->SetLineLength( m_psfcBgName->GetSize().w );
			m_pfoName->SetAlign( XE::xALIGN_CENTER );
		}
		{
			const ID idHero = m_Reward.GetidHero();
			if( idHero && m_pPropHero == nullptr) {
				m_pPropHero = PROP_HERO->GetpProp( idHero );
				XASSERT( m_pPropHero );
			}
		}
		// 레벨폰트 로딩
		if( (/*m_snHero ||*/ m_Level) && m_pfoLevel == nullptr ) {
			m_pfoLevel = FONTMNG->CreateFontObj( FONT_RESNUM, 14.f );
			m_pfoLevel->SetbBatch( m_bBatch );
			m_pfoLevel->SetLineLength( m_psfcBgLevel->GetSize().w );
			m_pfoLevel->SetAlign( XE::xALIGN_CENTER );
// 			if( m_snHero ) {
// 				auto pHero = ACCOUNT->GetHero( m_snHero );
// 				if( XASSERT( pHero ) ) {
// 					m_Level = pHero->GetLevel();
// 					if( !m_pPropHero )
// 						m_pPropHero = pHero->GetpProp();
// 				}
// 			}
		}
	}
	// 개수폰트(레벨폰트와 공유한다)
	if( m_bShowNum && m_pfoNum == nullptr ) {
		m_pfoNum = FONTMNG->CreateFontObj( FONT_RESNUM, 20.f );
		m_pfoNum->SetbBatch( m_bBatch );
//		m_pfoNum->SetLineLength( GetSizeLocal().w );
		m_pfoNum->SetAlign( XE::xALIGN_LEFT );
	}
// 	// 개수 표시
// 	auto pText = xGET_TEXT_CTRL( this, "storage.list.itemnum" );
// 	if( m_Reward.num > 0 ) {
// 		if( pText == nullptr ) {
// 			float size = 20.f;
// #ifdef _VER_DX
// 			// FontDX에는 자체 스케일기능이 없어서 폰트사이즈 자체를 스케일링해서 첨에 넣어줌.
// 			size *= GetScaleLocal().x;
// #endif // _VER_DX
// 			pText = new XWndTextString( XE::VEC2( 0, 42 ), _T( "" ), FONT_RESNUM, size );
// 			pText->SetbBatch( true );
// 			pText->SetStyle( xFONT::xSTYLE_STROKE );
// 			float sizeW = 52.f;
// //			sizeW *= GetScaleLocal().x;
// 			pText->SetLineLength( sizeW );	// 그림실제사이즈와 화면에서 보이는것과 차이가 있어 직접 넣음.
// //			pText->SetAlign( XE::xALIGN_RIGHT );
// 			pText->SetAlign( XE::xALIGN_HCENTER );
// 			pText->SetstrIdentifier( "storage.list.itemnum" );
// 			Add( pText );
// 		}
// 		if( pText ) {
// 			pText->SetbShow( true );
// 			// 개수 텍스트 갱신.
// 			auto vSize = GetSizeFinal();
// 			pText->SetText( XE::NumberToMoneyString( num ) );
// 		}
// 	} else {
// 		if( pText )
// 			pText->SetbShow( false );
// 	}
	XWnd::Update();
}

void XWndStoragyItemElem::SetNum( int num )
{
	m_Reward.num = num;
	if( num > 1 )
		m_bShowNum = true;
	SetbUpdate( true );
}

void XWndStoragyItemElem::SetLockButt()
{
	m_lockButt = new XWndButton(0, 0, _T("storage_lock.png"), nullptr );
	if (m_lockButt) {
		Add(m_lockButt);
	}
}

void XWndStoragyItemElem::Draw()
{
	const XE::VEC2 vPos = GetPosFinal();
	const auto vScale = GetScaleFinal();
	const auto sizeFrame = GetSizeLocal();
	auto typeItem = XGAME::xIT_NONE;
	if( m_pProp )
		typeItem = m_pProp->type;
	if (!m_lockButt) {
//		m_pBG->SetScale( vScale );
		XE::xRenderParam param;
		param.m_vPos = vPos;
		param.m_vScale = vScale;
		if( m_bBatch )
			param.m_Priority = -10;
		m_pBG->DrawByParam( param );
		if( m_Reward.GetidHero() ) {
			XBREAK( m_psfcBgGradation == nullptr );
			XE::xRenderParam param;
			param.m_vPos = vPos + XE::VEC2( 3, 1 ) * vScale;
			param.m_vScale = vScale;
			if( m_bBatch )
				param.m_Priority = -10;
			m_psfcBgGradation->DrawByParam( param );
		}
	}
	// 액자안에 들어갈 이미지(아이템,영웅등)
	if (m_pItemImg) {
		auto vScaleSoul = vScale;
// 		if (typeItem == XGAME::xIT_SOUL)
// 			vScaleSoul *= 0.77f;
// 		else
// 			vScaleSoul *= m_vScaleItemImg;
		{
			XE::xRenderParam param;
			param.m_vPos = vPos + XE::VEC2( 3, 2 ) * vScale;
			if( IsHero() || m_pSoulStone )
				param.m_vScale = vScale * 0.77f;
			else
				param.m_vScale = vScale;
			param.m_funcBlend = GetblendFunc();
			if( m_bBatch )
				param.m_Priority = -20;
			m_pItemImg->DrawByParam( param );
		}
		if( m_pSoulStone && typeItem == XGAME::xIT_SOUL ) {
			XE::xRenderParam param;
			param.m_vPos = vPos;
			param.m_vScale = vScaleSoul;
			if( m_bBatch )
				param.m_Priority = -30;
			m_pSoulStone->DrawByParam( param );
		}
	}
	if( m_bShowName && m_pPropHero ) {
		if( m_psfcBgName ) {
			XE::VEC2 vPosBgName;
			{
				// 이름 배경
				XE::xRenderParam param;
				vPosBgName = vPos + XE::VEC2( 2.5f, 44.f ) * vScale;
				param.m_vPos = vPosBgName;
				param.m_vScale = vScale;
				if( m_bBatch )
					param.m_Priority = -35;
				m_psfcBgName->DrawByParam( param );
			} {
				// 이름 텍스트
				const auto sizeLayout = m_pfoName->GetLayoutSize( m_pPropHero->GetstrName().c_str() );
				auto v = vPosBgName;
				v.y = vPosBgName.y + (m_psfcBgName->GetSize().h * 0.5f) - (sizeLayout.h * 0.5f);
				m_pfoName->SetScale( vScale );
				m_pfoName->DrawString( v, m_pPropHero->GetstrName().c_str() );
			}
			if( m_psfcBgLevel && m_Level ) {
				{
					const XE::VEC2 vPosBgLevel 
						= vPosBgName + XE::VEC2( -4.f, 0 ) * vScale;
					// 레벨 배경
					XE::xRenderParam param;
					param.m_vPos = vPosBgLevel;
					param.m_vScale = vScale;
					if( m_bBatch )
						param.m_Priority = -37;
					m_psfcBgLevel->DrawByParam( param );
					// 레벨 텍스트
					_tcscpy_s( m_szLv, XFORMAT( "%d", m_Level ) );
					const auto sizeLayout = m_pfoLevel->GetLayoutSize( m_szLv );
					auto v = vPosBgLevel;
					v.y = vPosBgLevel.y + (m_psfcBgLevel->GetSize().h * 0.5f * vScale.x) - (sizeLayout.h * 0.5f * vScale.x);
					m_pfoLevel->SetScale( vScale );
					m_pfoLevel->DrawString( v, m_szLv );
				}
			}
		}
	}
	// 개수 표시
	if( m_bShowNum && m_Reward.num ) {
		_tcscpy_s( m_szNum, XFORMAT( "%d", m_Reward.num ) );
		const auto sizeLayout = m_pfoNum->GetLayoutSize( m_szNum );
		auto v = vPos;
		v.x = vPos.x + GetSizeLocal().w - (sizeLayout.w + 4.f) * vScale.x;
		v.y = vPos.y + (2.f * vScale.y);
		m_pfoNum->SetScale( vScale );
		m_pfoNum->DrawString( v, m_szNum );
	}
	// 장착 아이콘
	if( m_bEquip && m_psfcEquip ) {
		XE::xRenderParam param;
		param.m_vPos = vPos + XE::VEC2(3,11) * vScale;
		param.m_vScale = vScale;
		if( m_bBatch )
			param.m_Priority = -38;
		m_psfcEquip->DrawByParam( param );
	}
	// 선택된 항목이면 글로우효과 덧씌움.
	if( m_bSelected && m_psfcSelected ) {
		float lerp = 1.f;
		if( m_bGlowAnimation ) {
			if( m_timerGlow.IsOff() )
				m_timerGlow.Set( 1.f );
			if( m_timerGlow.IsOver() )
				m_timerGlow.Reset();
			float lerpTime = m_timerGlow.GetSlerp();
			lerp = XE::xiCos( lerpTime, 0.5f, 1.f, 0.f );
		}	{
			XE::xRenderParam param;
			param.m_vPos = XE::VEC2(vPos.x - 3.f, vPos.y - 3.f);
			param.m_vScale = vScale;
			param.m_vColor.a = lerp;
			param.m_funcBlend = XE::xBF_ADD;
			if( m_bBatch )
				param.m_Priority = -40;
			m_psfcSelected->DrawByParam( param );
		}
	}
	// 별
	int i = 0;
	for( auto pImg : m_aryStar ) {
		if (pImg != NULL) {
			auto vs = vScale * 0.8f;
//			pImg->SetScale(vs);
			auto v = vPos;
			v.x += (4*vScale.x) + i * (9*vScale.x);
			v.y += -1.f;
//			pImg->Draw( v );
			XE::xRenderParam param;
			param.m_vPos = v;
			param.m_vScale = vs;
			if( m_bBatch )
				param.m_Priority = -50;
			pImg->DrawByParam( param );
		}
		++i;
	}

	if( m_bNotUse ) {
		const auto vSize = XE::VEC2(51,50) * vScale;		//
		const auto v = vPos + (XE::VEC2(2,2) * vScale);
		GRAPHICS->FillRectSize( v, vSize, XCOLOR_RGBA(255, 0, 0, 128) );
	}
	XWnd::Draw();
}



void XWndStoragyItemElem::SetItemImg(LPCTSTR imgPath)
{
	m_pItemImg = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, imgPath));
}

void XWndStoragyItemElem::SetSlotLock()
{
//	m_slotLock = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("storage_lock2.png")));
	m_bNotUse = true;
}

bool XWndStoragyItemElem::IsLock()
{
//	return m_slotLock != nullptr;
	return m_bNotUse;
}

void XWndStoragyItemElem::SetEventItemTooltip()
{
	XBREAK( m_Reward.GetidItem() == 0 );
	SetEvent( XWM_CLICKED, GAME, &XGame::OnClickItemTooltip, m_Reward.GetidItem() );
}

/**
 @brief 외부로부터 받은 메시지를 처리한다.
*/
bool XWndStoragyItemElem::DispatchMsg( const XE::xMsgWin& msg )
{
	XWnd::DispatchMsg( msg );
	if( msg.m_strMsg == "selected" ) {
		SetbSelected( true );
	} else
	if( msg.m_strMsg == "deselected" ) {
		SetbSelected( false );
	}
	return true;
}
