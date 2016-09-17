#include "stdafx.h"
#include "JWWnd.h"
#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XFramework/XConstant.h"
//#include "XWindow.h"
#include "XImageMng.h"
#include "XFramework/client/XLayout.h"
#include "XAccount.h"
#include "XSceneWorld.h"
//#include "XUnitHero.h"
#include "XBaseItem.h"
#include "XSockGameSvr.h"
#include "XSceneUnitOrg.h"
#include "XHero.h"
#include "XWndTemplate.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"

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
// 함정완 전용 윈도우UI 구현코드
//////////////////////////////////////////////////////////////////////////
XWndShopElem::XWndShopElem( const _tstring& idsProduct )
	: m_idsProduct( idsProduct )
	, XWndImage( PATH_UI( "shop_listitem.png" ), XE::xPF_ARGB8888, 0.f, 0.f )
{
	Init();
// 	m_pCashItem = XGlobalConst::sGet()->GetCashItem( idsProduct );
// 	XBREAK( m_pCashItem == nullptr );
}
XWndShopElem::XWndShopElem( XPropItem::xPROP* pEtcItemProp )
	: m_pEtcItemProp(pEtcItemProp)
	, XWndImage( PATH_UI( "shop_listitem.png" ), XE::xPF_ARGB8888, 0.f, 0.f )
{
	Init();
}

//////////////////////////////////////////////////////////////////////////
XWndStoragyItemElem* XWndStoragyItemElem::sUpdateCtrl( XWnd* pParent, const XE::VEC2& vPos, XHero* pHero, const std::string& ids )
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
XWndStoragyItemElem* XWndStoragyItemElem::sUpdateCtrlToList( XWndList *pWndList, XHero *pHero )
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
/**
 @brief 빈칸만 있는 버전
*/
XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos )
	: XWnd( vPos )
{
	Init();
	SetbUpdate( true );
}
XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos
																				, const XGAME::xReward& reward )
	: XWnd( vPos )
{
	Init();
// 	m_Reward = reward;
	switch( reward.rewardType ) {
	case xtReward::xRW_ITEM: {
		if( XASSERT(reward.idReward) )
			m_pProp = PROP_ITEM->GetpProp( reward.idReward );
	} break;
	case xtReward::xRW_GOLD: {
		XBREAK(1);
	} break;
	case xtReward::xRW_RESOURCE: {
		auto szImg = XGAME::GetResourceIconBig( (XGAME::xtResource)reward.idReward );
		if( reward.num > 1 )
			SetNum( reward.num );
		if( XE::IsHave(szImg) )
			m_pItemImg = IMAGE_MNG->Load( XE::MakePath( DIR_ICON, szImg) );
	} break;
	case xtReward::xRW_CASH: {
		m_pItemImg = IMAGE_MNG->Load( PATH_UI("shop_cash1.png") );
	} break;
	case xtReward::xRW_HERO: {
		if( reward.idReward ) {
			SetHero( reward.idReward );
// 			auto pProp = PROP_HERO->GetpProp( reward.idReward );
// 			if( XASSERT( pProp ) ) {
// 				_tstring resFace = XE::MakePath( DIR_IMG, pProp->strFace );
// 				m_pItemImg = IMAGE_MNG->Load( resFace, XE::xPF_ARGB8888 );
// 			}
		}
// 		SetvScaleItemImg( XE::VEC2(0.789f, 0.781f) );
	} break;
	case xtReward::xRW_GUILD_POINT: {
		m_pItemImg = IMAGE_MNG->Load( PATH_UI( "guild_coin.png" ) );
	} break;
	}
	m_Reward = reward;		// 기존거와 비교하기위해 업데이트가 끝난후 대입함.
	SetbUpdate( true );
}
XWndStoragyItemElem::XWndStoragyItemElem( const XE::VEC2& vPos, XHero *pHero )
	: XWndStoragyItemElem( vPos, XGAME::xReward(pHero) )
{
	SetHero( pHero );
// 	if( pHero ) {
// 		m_snHero = pHero->GetsnHero();
// //		m_Reward.SetHero( pHero->GetidProp(), 0 );
// 		// 영혼석이 아닌경우에만 별 그림
// 		auto numStar = pHero->GetGrade();
// 		for( int i = 0; i < numStar; ++i )
// 			m_aryStar.Add( IMAGE_MNG->Load( PATH_UI( "common_etc_smallstar.png" ) ) );
// 	}
// 	SetbUpdate( true );
}

void XWndStoragyItemElem::SetHero( ID idProp )
{
	if( m_Reward.GetidHero() != idProp ) {
		// 기존과 다른 영웅이 지정되었을때만 
		auto pProp = PROP_HERO->GetpProp( idProp );
		if( XASSERT(pProp) ) {
			const _tstring resFace = XE::MakePath( DIR_IMG, pProp->strFace );
			XBREAK( resFace.empty() );
			SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
			m_pItemImg = IMAGE_MNG->Load( resFace, 
																		XE::xPF_ARGB8888, 
																		false, 
																		false, 
																		false ); // async
			SetvScaleItemImg( XE::VEC2( 0.789f, 0.781f ) );
			SetbUpdate( true );
			m_Reward.SetHero( idProp );
		}
	}
}

void XWndStoragyItemElem::SetHero( XHero* pHero )
{
	if( pHero ) {
		m_snHero = pHero->GetsnHero();
		// 영혼석이 아닌경우에만 별 그림
		const auto numStar = pHero->GetGrade();
		for( int i = 0; i < numStar; ++i )
			m_aryStar.Add( IMAGE_MNG->Load( PATH_UI( "common_etc_smallstar.png" ) ) );
		SetbUpdate( true );
	}
}

void XWndStoragyItemElem::ClearHero()
{
	m_Reward.SetHero( 0, 0 );
	m_snHero = 0;
	SAFE_RELEASE2( IMAGE_MNG, m_pItemImg );
}

XWndStoragyItemElem::XWndStoragyItemElem(XBaseItem* pItem)
{
	Init();
	if( pItem ) {
 		m_pItem = pItem;
		m_Reward.SetItem( pItem->GetidProp(), pItem->GetNum() );
		m_pProp = pItem->GetpProp();
		m_snItem = pItem->GetsnItem();
	}
	SetbUpdate( true );
}

XWndStoragyItemElem::XWndStoragyItemElem(ID idItem)
{
	Init();
	m_Reward.SetItem( idItem, 1 );
	if (idItem) {
		m_pProp = PROP_ITEM->GetpProp( idItem );
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
	m_psfcBgGradation = IMAGE_MNG->Load( PATH_UI( "bg_hero.png" ), XE::xPF_ARGB8888 );
// 	m_pBG = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "common_bg_item.png" ) ) );
	m_pBG = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "common_bg_frame.png" ) ) );
	SetSizeLocal( m_pBG->GetWidth(), m_pBG->GetHeight() );
// 	m_pSelect = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "common_bg_item_sel.png" ) ) );
//	m_pNumImg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "common_etc_namecard_s.png" ) ) );
	// 아이템일경우 별등급표시
	if( m_Reward.GetidItem() ) {
		XBREAK( m_pProp == nullptr );
		if( m_pProp->IsSoul() ) {
			m_pSoulStone = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "frame_soul.png" ) ) );
		} else {
			// 영혼석이 아닌경우에만 별 그림
			auto numStar = m_pProp->grade;
			for( int i = 0; i < numStar; ++i )
				m_aryStar.Add( IMAGE_MNG->Load( TRUE, PATH_UI( "common_etc_smallstar.png" ) ) );
		}
		m_pItemImg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_IMG, m_pProp->strIcon.c_str() ) );
	}
// 	if( m_Reward.GetidHero() ) {
// //		m_psfcNameBg = IMAGE_MNG->Load( PATH_UI("bg_name.png") );
// 		auto pImg = new XWndImage( PATH_UI("bg_name.png"), 0, 37 );
// 		Add( )
// 	}
	return TRUE;
}

void XWndStoragyItemElem::Update()
{
	if( m_bSelected && m_psfcSelected == nullptr ) {
		if( m_psfcSelected == nullptr )
			m_psfcSelected = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, _T( "common_bg_item_glow.png" ) ) );
	}
	int num = m_Reward.num;
	if( m_Reward.GetidItem() ) {
		auto pProp = PROP_ITEM->GetpProp( m_Reward.GetidItem() );
		XBREAK( pProp == nullptr );
		int maxStack = pProp->maxStack;
		if( m_pItem ) {
			num = m_pItem->GetNum();	// 실시간 갱신
		}
		auto pWndEquip = Find( "img.icon.equip" );
		if( m_pItem && ACCOUNT->IsEquip( m_snItem ) )
			m_bEquip = true;
		else
			m_bEquip = false;
		if( m_bEquip ) {
			if( pWndEquip == nullptr ) {
				auto pImgEquip = new XWndImage( PATH_ICON( "icon_equip.png" ), 3, 11 );
				pImgEquip->SetstrIdentifier( "img.icon.equip" );
				Add( pImgEquip );
			} else
				pWndEquip->SetbShow( TRUE );
		} else
			if( pWndEquip )
				pWndEquip->SetbShow( FALSE );
	}
	// 이름
	if( m_bShowName ) {
		if( m_Reward.GetidHero() ) {
			int lvHero = 0;		// 0은 레벨을 표시하지 않는다.
			if( !Find(GetIdsName()) ) {
				auto pImg = new XWndImage( PATH_UI( "bg_name.png" ), XE::VEC2(2.5f, 44.f) );
				pImg->SetstrIdentifier( GetIdsName() );
//				pImg->SetstrIdentifier( "img.bg.name" );
				Add( pImg );
				if( m_snHero ) {
					auto pHero = ACCOUNT->GetHero( m_snHero );
					if( pHero )
						lvHero = pHero->GetLevel();
				}
				auto pPropHero = PROP_HERO->GetpProp( m_Reward.GetidHero() );
				if( XASSERT(pPropHero) ) {
					const auto vScale = GetScaleLocal();
					float sizeFont = (float)((int)18.f * vScale.y);
					if( lvHero > 0 ) {
						auto pImgLv = XGAME::UpdateLevelByHero( pImg, "img.level", lvHero );
						if( pImgLv ) {
							pImgLv->AutoLayoutVCenter();
							pImgLv->SetX( -3.f * vScale.x );
						}
					}
					auto pText = new XWndTextString( pPropHero->GetstrName(), FONT_NANUM, sizeFont );
					pText->SetAlignHCenter();
					pText->SetStyleStroke();
					pImg->Add( pText );
				}
			}
		}
	}
	// 개수 표시
	auto pText = xGET_TEXT_CTRL( this, "storage.list.itemnum" );
	if( m_Reward.num > 0 ) {
		if( pText == nullptr ) {
			float size = 20.f;
#ifdef _VER_DX
			// FontDX에는 자체 스케일기능이 없어서 폰트사이즈 자체를 스케일링해서 첨에 넣어줌.
			size *= GetScaleLocal().x;
#endif // _VER_DX
			pText = new XWndTextString( XE::VEC2( 0, 42 ), _T( "" ), FONT_RESNUM, size );
			pText->SetStyle( xFONT::xSTYLE_STROKE );
			float sizeW = 52.f;
//			sizeW *= GetScaleLocal().x;
			pText->SetLineLength( sizeW );	// 그림실제사이즈와 화면에서 보이는것과 차이가 있어 직접 넣음.
//			pText->SetAlign( XE::xALIGN_RIGHT );
			pText->SetAlign( XE::xALIGN_HCENTER );
			pText->SetstrIdentifier( "storage.list.itemnum" );
			Add( pText );
		}
		if( pText ) {
			pText->SetbShow( true );
			// 개수 텍스트 갱신.
			auto vSize = GetSizeFinal();
			pText->SetText( XE::NumberToMoneyString( num ) );
		}
	} else {
		if( pText )
			pText->SetbShow( false );
	}
	XWnd::Update();
}

void XWndStoragyItemElem::SetNum( int num )
{
	m_Reward.num = num;
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
	XE::VEC2 vPos = GetPosFinal();
//	float scale = GetScaleLocal().x;
	const auto vScale = GetScaleFinal();
	const auto sizeFrame = GetSizeLocal();
	auto typeItem = XGAME::xIT_NONE;
	if( m_pProp )
		typeItem = m_pProp->type;
	if (!m_lockButt) {
		m_pBG->SetScale( vScale );
		m_pBG->Draw(vPos.x, vPos.y);
		if( m_Reward.GetidHero() ) {
			XBREAK( m_psfcBgGradation == nullptr );
			m_psfcBgGradation->SetScale( vScale );
			m_psfcBgGradation->Draw( vPos + XE::VEC2( 3,1 ) * vScale );
		}
	}
	// 액자안에 들어갈 이미지(아이템,영웅등)
	if (m_pItemImg) {
		//m_pItemImg->SetScale(XE::VEC2(0.81f, 0.6f));
		auto vScaleSoul = vScale;
		if (typeItem == XGAME::xIT_SOUL)
			vScaleSoul *= 0.77f;
		else
			vScaleSoul *= m_vScaleItemImg;
		m_pItemImg->SetScale(vScaleSoul);
		auto v = vPos + XE::VEC2(3,2) * vScale;
		m_pItemImg->SetBlendFunc( GetblendFunc() );
		m_pItemImg->Draw( v );
// 		m_pItemImg->Draw(vPos.x + (4*scale), vPos.y + (4*scale));
		if( m_pSoulStone && typeItem == XGAME::xIT_SOUL ) {
			m_pSoulStone->SetScale( vScale );
			m_pSoulStone->Draw( vPos.x, vPos.y );
		}
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
		}
		m_psfcSelected->SetfAlpha( lerp );
		m_psfcSelected->SetScale( vScale );
		m_psfcSelected->SetBlendFunc( XE::xBF_ADD );
		m_psfcSelected->Draw( vPos.x - 3.f, vPos.y - 3.f );
	}
	// 별
	XARRAYLINEARN_LOOP_IDX( m_aryStar, auto, i, pImg ) {
		if (pImg != NULL) {
			auto vs = vScale * 0.8f;
			pImg->SetScale(vs);
			auto v = vPos;
			v.x += (4*vScale.x) + i * (9*vScale.x);
			v.y += -1.f;
			pImg->Draw( v );
		}
	} END_LOOP;

	if( m_bNotUse ) {
		const auto vSize = XE::VEC2(51,50) * vScale;		//
		const auto v = vPos + (XE::VEC2(2,2) * vScale);
		GRAPHICS->FillRectSize( v, vSize, XCOLOR_RGBA(255, 0, 0, 128) );
	}
	XWnd::Draw();
}

// void XWndStoragyItemElem::DrawDrag(const XE::VEC2& vMouse)
// {
// 	/*if (m_pFace)
// 	{
// 		XE::VEC2 vSize = m_pFace->GetSize();
// 		m_pFace->SetfAlpha(0.5f);
// 		m_pFace->Draw(vMouse - vSize / 2.f);
// 	}*/
// }
void XWndStoragyItemElem::SetItemImg(LPCTSTR imgPath)
{
	m_pItemImg = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, imgPath));
}

void XWndStoragyItemElem::SetSlotLock()
{
//	m_slotLock = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("storage_lock2.png")));
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

//////////////////////////////////////////////////////////////////////////
XWndSliderOption::XWndSliderOption(float x, float y, float w, float h, float min, float max, float curr)
	: XWndSlider(x, y, w, h, min, max, curr)
{
	Init();

	m_psfcSlider = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("trader_slider_point.png")));
};

void XWndSliderOption::Destroy()
{
	SAFE_RELEASE2(IMAGE_MNG, m_psfcSlider);
}

void XWndSliderOption::Draw(void)
{
	float lerp = (m_Curr - m_Min) / (m_Max - m_Min);

	XE::VEC2 vSize = m_psfcSlider->GetSize() / GetScaleFinal();
	XE::VEC2 vPos = GetPosFinal();

	vPos.x += (lerp * GetWidthLocal());
	vPos -= vSize / 2.f;
	vPos.y += GetHeightLocal() / 2.f;
	m_psfcSlider->Draw(vPos);
}