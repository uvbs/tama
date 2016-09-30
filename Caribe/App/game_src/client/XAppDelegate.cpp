#include "stdafx.h"
#include "XAppDelegate.h"
#include "client/XAppMain.h"
#include "JWWnd.h"
#include "XWndTemplate.h"
#include "XWndStorageItemElem.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XAppDelegate* XAppDelegate::s_pInstance = nullptr;
////////////////////////////////////////////////////////////////
XAppDelegate::XAppDelegate()
{
	XBREAK( s_pInstance != nullptr );
	Init();
	s_pInstance = this;
}

XClientMain* XAppDelegate::OnCreateAppMain( XE::xtDevice device, float widthPhy, float heightPhy )
{
	return XAppMain::sCreate( device, (int)widthPhy, (int)heightPhy );
//	return XMain::sCreate( device, widthPhy, heightPhy );
}

XWnd* XAppDelegate::DelegateCreateCustomCtrl( const std::string& strcCtrl
																						, TiXmlElement* pElemCtrl
																						, const XWnd *pParent
																						, const XLayout::xATTR_ALL& attrAll) const
{
	XWnd* pWndCreated = nullptr;
	if( strcCtrl == "hero_ctrl" ) {
		// 영웅 초상화 컨트롤
		xReward reward;
		reward.SetHero( 0 );		// 영웅타입으로만 생성하도록.
		auto pWndFace = new XWndStoragyItemElem( attrAll.vPos, reward );
		const DWORD idHero = attrAll.m_Param.GetDword("param");
		if( idHero ) {
			pWndFace->SetHero( idHero );
		}
		pWndFace->SetNum(0);
		pWndFace->SetScaleLocal( attrAll.vScale );
		pWndCreated = pWndFace;
	} else
	if( strcCtrl == "unit_ctrl" ) {
//		pWndCreated = XGAME::CreateUnitFace( pParent, xUNIT_NONE );
		auto unit = static_cast<XGAME::xtUnit>( attrAll.m_Param.GetDword( "param" ) );
		pWndCreated = new XWndCircleUnit( unit, XE::VEC2(), nullptr );
		pWndCreated->SetPosLocal( attrAll.vPos );
		pWndCreated->SetScaleLocal( attrAll.vScale );
		pWndCreated->SetAlphaLocal( attrAll.alpha );
	}


	return pWndCreated;
}
