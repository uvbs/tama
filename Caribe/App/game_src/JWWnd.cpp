#include "stdafx.h"
#include "XImageMng.h"
#include "JWWnd.h"

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
XWndShopElem::XWndShopElem( const XPropItem::xPROP* pEtcItemProp )
	: m_pEtcItemProp(pEtcItemProp)
	, XWndImage( PATH_UI( "shop_listitem.png" ), XE::xPF_ARGB8888, 0.f, 0.f )
{
	Init();
}

//////////////////////////////////////////////////////////////////////////
XWndSliderOption::XWndSliderOption(float x, float y, float w, float h, float min, float max, float curr)
	: XWndSlider(x, y, w, h, min, max, curr)
{
	Init();

	m_psfcSlider = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("trader_slider_point.png")));
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