#include "stdafx.h"
#include "XAppDelegate.h"
#include "client/XAppMain.h"
#include "JWWnd.h"


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
																						, XWnd *pParent
																						, const XLayout::xATTR_ALL& attrAll)
{
	XWnd* pWndCreated = nullptr;
	if( strcCtrl == "hero_ctrl" ) {
		// 영웅 초상화 컨트롤
		xReward reward;
		reward.SetHero( 0 );		// 영웅타입으로만 생성하도록.
		auto pWndFace = new XWndStoragyItemElem( attrAll.vPos, reward );
		pWndFace->SetNum(0);
		pWndCreated = pWndFace;
	}


	return pWndCreated;
}
