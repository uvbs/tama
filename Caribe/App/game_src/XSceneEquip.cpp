#include "StdAfx.h"
#include "XSceneEquip.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSceneArmory.h"
#include "XHero.h"
#include "XSoundMng.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "XWndStorageItemElem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneEquip *SCENE_EQUIP = NULL;

void XSceneEquip::Destroy()
{	
	XBREAK(SCENE_EQUIP == NULL);
	XBREAK(SCENE_EQUIP != this);
	SCENE_EQUIP = NULL;
}

XSceneEquip::XSceneEquip(XGame *pGame, XSPSceneParam& spBaseParam)
	: XSceneBase(pGame, XGAME::xSC_EQUIP)
	, m_Layout(_T("layout_equip.xml"))
	, m_LayoutTooltip( _T( "layout_item.xml" ) )
{ 
	XBREAK(SCENE_EQUIP != NULL);
	SCENE_EQUIP = this;
	Init(); 
	auto spParam = std::static_pointer_cast<xSPM_EQUIP>(spBaseParam);
	m_BaseParam.idxParts = spParam->idxParts;
	m_BaseParam.snHero = spParam->snHero;
	m_pHero = ACCOUNT->GetHero(m_BaseParam.snHero);
	m_Layout.CreateLayout("equip", this);
	xSET_ENABLE(this, "butt.equip", FALSE);

	switch (m_BaseParam.idxParts)	{
	case XGAME::xPARTS_HEAD:	{
		xSET_TEXT(this, "text.title", XTEXT(80085));
	}		break;
	case XGAME::xPARTS_CHEST:	{
		xSET_TEXT(this, "text.title", XTEXT(80087));
	}		break;
	case XGAME::xPARTS_HAND:	{
		xSET_TEXT(this, "text.title", XTEXT(80088));
	}		break;
	case XGAME::xPARTS_FOOT:	{
		xSET_TEXT(this, "text.title", XTEXT(80089));
	}	break;
	case XGAME::xPARTS_ACCESSORY: {
		xSET_TEXT(this, "text.title", XTEXT(80090));
	}	break;
	}

	XWndList *pWndList = SafeCast<XWndList*, XWnd*>(Find("list.equip"));
	XList4<XBaseItem*> listItem;
	ACCOUNT->GetInvenItem(listItem);
	ID snEquipItem = 0;
	if (pWndList)	{
		auto pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("legion_equip_ban.png")), 0.f, 0.f);
		pWndList->AddItem(100, pImg);
		const auto size = pImg->GetSizeLocal();
		auto pText = new XWndTextString( XE::VEC2(0,45), size, XTEXT(2287) );
		pText->SetAlignHCenter();
		pText->SetStyleStroke();
		pText->SetColorText( XCOLOR_YELLOW );
		pImg->Add( pText );
		BOOL bFirst = TRUE;
		int i = 0;
		for( auto pItem : listItem )		{
			if (pItem->GetType() == XGAME::xIT_EQUIP && pItem->GetpProp()->parts == m_BaseParam.idxParts)			{
				auto pElem = new XWndStoragyItemElem(pItem);
				pWndList->AddItem(pItem->GetsnItem(), pElem);
				//if (pItem->GetbEquip()) {
				if( pElem->GetbEquip() ) {
					//XBaseItem *pHeroItem = m_pHero->GetsnEquipItem(m_BaseParam.idxParts);
					ID snItem = m_pHero->GetsnEquipItem( m_BaseParam.idxParts );
					auto pHeroItem = ACCOUNT->GetpItemBySN( snItem );
					if (pHeroItem && pHeroItem->GetsnItem() == pItem->GetsnItem())
						snEquipItem = pItem->GetsnItem();
					pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("armory_bg_item_black.png")), 0.f, 0.f);
					pElem->Add(pImg);
				}

				if (bFirst) {
					m_pSelectItem = pElem;
					m_pSelectItem->SetbSelected( true );
					m_pSelectItem->SetbGlowAnimation( true );
					bFirst = FALSE;
				}
			}
			++i;
		}
		pWndList->SetEvent(XWM_SELECT_ELEM, this, &XSceneEquip::OnSelectItem, snEquipItem);
	}

// 	UpdateItemInfo();
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot )
	{
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}

	xSET_BUTT_HANDLER(this, "butt.equip", &XSceneEquip::OnEquip);
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneEquip::OnBack);

	//m_Layout.CreateLayout("sample", this);
	/*XWndButtonString *pButt =
		new XWndButtonString(0.f, 0.f, XTEXT(2033), XCOLOR_WHITE, FONT_NANUM_BOLD, 40.f, BUTT_BIG );
	pWnd->Add( pButt );
	pWnd->SetEvent( XWM_CLICKED, this, &XSceneSample::OnClick );*/
	SetbUpdate( true );
}

void XSceneEquip::Create(void)
{
	XSceneBase::Create();
}

void XSceneEquip::Update()
{
	UpdateItemInfo();
	XSceneBase::Update();
}

int XSceneEquip::Process(float dt)
{ 
	return XSceneBase::Process( dt );
}

//
void XSceneEquip::Draw(void)
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneEquip::OnLButtonDown(float lx, float ly)
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneEquip::OnLButtonUp(float lx, float ly) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneEquip::OnMouseMove(float lx, float ly) {
	XSceneBase::OnMouseMove( lx, ly );
}

int XSceneEquip::OnEquip(XWnd *pWnd, DWORD p1, DWORD p2)
{
	//SetbEnable(FALSE);
	//여긴 서버에 장비 장착 정보 보내고 씬 전환해야함
	//RecvReqHeroEquip(m_pHero->GetsnHero(), m_pSelectItem->GetsnItem(), 1);
	GAMESVR_SOCKET->SendReqEqiupItem(this, m_pHero->GetsnHero(), m_pSelectItem->GetsnItem(), 1);
	return 1;
}
int XSceneEquip::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	auto spParam = std::shared_ptr<xSPM_EQUIP>( new xSPM_EQUIP() );
	spParam->idxParts = m_BaseParam.idxParts;
	spParam->snHero = m_BaseParam.snHero;
	DoExit(XGAME::xSC_UNIT_ORG, spParam);
	return 1;
}

void XSceneEquip::RecvReqHeroEquip(ID snHero, ID snItem, ID typeAction)
{
	XSPHero pHero = ACCOUNT->GetHero(snHero);
	XBaseItem *pItem = ACCOUNT->GetItem(snItem);
	if (pHero && pItem && 
		pItem->GetpProp()->parts == m_BaseParam.idxParts)
	{
		auto spParam = std::shared_ptr<xSPM_EQUIP>( new xSPM_EQUIP() );
		spParam->idxParts = m_BaseParam.idxParts;
		spParam->snHero = m_BaseParam.snHero;
		if (typeAction == 0)
			pHero->SetUnequip(pItem);
		else
			pHero->SetEquip(pItem);
		SOUNDMNG->OpenPlaySound(12);
		DoExit(XGAME::xSC_UNIT_ORG, spParam);
	}
}

int XSceneEquip::OnSelectItem(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (p2 == 100)
	{
		//여기는 아이템 탈착하는부분
		if (p1 == 0)
		{
			auto spParam = std::shared_ptr<xSPM_EQUIP>( new xSPM_EQUIP() );
			spParam->idxParts = m_BaseParam.idxParts;
			spParam->snHero = m_BaseParam.snHero;
			DoExit(XGAME::xSC_UNIT_ORG, spParam);
			return 1;
		}
		GAMESVR_SOCKET->SendReqEqiupItem(this, m_pHero->GetsnHero(), p1, 0);
		return 1;
	}
	if (m_pSelectItem)
		m_pSelectItem->SetbSelected( false );
	m_pSelectItem = SafeCast<XWndStoragyItemElem*, XWnd*>(pWnd->Find(p2));
	m_pSelectItem->SetbSelected( true );
	m_pSelectItem->SetbGlowAnimation( true );
	UpdateItemInfo();
	SetbUpdate( true );
	return 1;
}

void XSceneEquip::UpdateItemInfo()
{
	if (m_pSelectItem)	{
		auto pProp = m_pSelectItem->getpProp();
		if( pProp == nullptr )
			return;
		if (m_pSelectItem->GetbEquip() == false) {
			xSET_ENABLE(this, "butt.equip", TRUE);
		} else
			xSET_ENABLE(this, "butt.equip", FALSE);
		XWnd *pRoot = Find( "wnd.item.tooltip" );
		if( pRoot )
			XGAME::UpdateItemTooltip( pProp, pRoot, _T(""), 0 );
	}
}