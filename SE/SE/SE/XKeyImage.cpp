#include "stdafx.h"
#include "XKeyImage.h"
#include "sprite.h"
#include "XLayerImage.h"
#include "XAniAction.h"
#include "SprDat.h"
#include "SprObj.h"
#include "XLuaSprObj.h"
#include "SEFont.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XKeyImage::XKeyImage( XSPAction spAction, float fFrame, xSPLayerImage spLayer, int idxSpr )
	: XBaseKey( spAction, xSpr::xKT_IMAGE, xSpr::xKTS_MAIN, fFrame, spLayer ) 
{
	Init();
//	m_nSpr = idxSpr;
	auto pSprDat = spAction->GetpSprDat();
	if( XASSERT(pSprDat) )
		m_pSprite = pSprDat->GetSpriteIndex( idxSpr );
	//m_spLayer = spLayer;
	XBREAK( m_pSprite == nullptr );
}

XBaseKey* XKeyImage::CreateCopy()
{
//	return new XKeyImage( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeyImage( *this );		// 기본카피
// 	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}
// void XKeyImage::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerImage>( spLayer );
// }
// XBaseKey* XKeyImage::CopyDeep()
// {
// 	return new XKeyImage( *this );
// // 	XKeyImage *pKey = new XKeyImage( spAction, GetfFrame(), GetLayerType(), GetnLayer(), m_pSprite );
// // 	pKey->SetidKey( GetidKey() );
// // 	return pKey;
// }
void XKeyImage::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes)
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
//	int dwData = m_pSprite->GetnIdx();
//	pRes->Write( &dwData, 4 );					// sprite index
	(*pRes) << m_pSprite->GetnIdx();

}
void XKeyImage::Load( XResFile *pRes, XSPAction spAction, int ver )
{
//	XBaseKey::Load( pRes, spAction, ver );
//	//m_spLayer = std::static_pointer_cast<XLayerImage>( XBaseKey::GetspLayer() );
//	pRes->Read( &m_nSpr, 4 );
	auto pSprDat = spAction->GetpSprDat();
	XBREAK( pSprDat == nullptr );
	int idxSpr;
	(*pRes) >> idxSpr;
	m_pSprite = pSprDat->GetSpriteIndex( idxSpr );
	XBREAK( m_pSprite == nullptr );
}

void XKeyImage::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
	XBaseKey::Execute( spActObj, spLayer, fOverSec );
	int execute = 1;
	// 루아가 있으면 실행
	if( GetpcLua() && XE::IsHave( GetpcLua() ) ) {
		if( GetpLua() == nullptr )
			SetpLua( spActObj->GetpSprObj()->CreateScript() );		// virtual create
		execute = GetpLua()->DoString( GetpcLua() );	// 각키에 할당된 루아코드를 실행
	}
	// 디폴트 기능을 실행하도록 되어있으면 실행
	if( execute ) {
		XBREAK( m_pSprite == nullptr );
//		auto spLayerImage = pSprObj->GetspActObjCurr()->GetspLayerByidLayer<XLayerImage>( GetidLayer() );
//		auto spLayerImage = m_spAction->GetspLayerByidLayer<XLayerImage>( GetidLayer() );
		auto spLayerImage = std::static_pointer_cast<XLayerImage>( spLayer );
		if( XASSERT(spLayerImage) )
			spLayerImage->SetpSpriteCurr( m_pSprite );
	}
}

void XKeyImage::DrawToolTip( XSprDat *pSprDat, float x, float y ) {
// 	auto pSpr = pSprDat->GetSpriteIndex( m_nSpr );
	auto pSpr = m_pSprite;
	if( pSpr )
		pSpr->DrawNoAdjust( x, y );
}
void XKeyImage::GetToolTipSize( float *w, float *h ) 
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 4;
	*h += SE::g_pFont->GetFontHeight();
}
CString XKeyImage::GetToolTipString( XSPBaseLayer spLayer ) 
{
	CString str;
	str.Format( _T( "SprNum:%d" ), m_pSprite->GetnIdx() );
	str = XBaseKey::GetToolTipString( spLayer ) + str;
	str += GetToolTipStringTail();
	return str;
	return CString();
}

int XKeyImage::GetnSpr()
{
	if( XASSERT( m_pSprite ) )
		return m_pSprite->GetnIdx();
	return 0;
}
