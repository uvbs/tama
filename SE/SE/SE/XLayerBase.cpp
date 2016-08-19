#include "stdafx.h"
#include "XLayerAll.h"
#include "FrameView.h"
#include "XKeyBase.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSPBaseLayer XBaseLayer::sCreate( const xLayerInfo& layerInfo )
{
	auto spLayer = sCreate( layerInfo.m_bitType );
	if( XASSERT(spLayer) ) {
		spLayer->SetidLayer( layerInfo.m_idLayer );
		spLayer->SetnLayer( layerInfo.m_nLayer );
		spLayer->SetfLastFrame( layerInfo.m_fLastFrame );
	}
	return spLayer;
}

XSPBaseLayer XBaseLayer::sCreate( xSpr::xtLayer type )
{
	XSPBaseLayer spBaseLayer;
	switch( type ) {
	case xSpr::xLT_IMAGE: 
		spBaseLayer = xSPLayerImage( new XLayerImage );
		break;
	case xSpr::xLT_OBJECT:
		spBaseLayer = XSPLayerObject( new XLayerObject );
		break;
	case xSpr::xLT_SOUND:
		spBaseLayer = XSPLayerSound( new XLayerSound );
		break;
	case xSpr::xLT_EVENT:
		spBaseLayer = XSPLayerEvent( new XLayerEvent );
		break;
	case xSpr::xLT_DUMMY:
		spBaseLayer = XSPLayerDummy( new XLayerDummy );
		break;
	default:
		XBREAKF( 1, "알수없는 레이어 타입:type=%d", type );
		break;
	}
	if( spBaseLayer )
		spBaseLayer->OnCreate();
	return spBaseLayer;
}
LPCTSTR XBaseLayer::GetTypeStr( xSpr::xtLayer type /*= xSpr::xLT_NONE*/ ) 
{
	switch( type )
	{
	case xSpr::xLT_IMAGE:		return _T( "xIMAGE_LAYER" );
	case xSpr::xLT_OBJECT:			return _T( "xOBJ_LAYER" );
	case xSpr::xLT_SOUND:		return _T( "xSOUND_LAYER" );
	case xSpr::xLT_EVENT:		return _T( "xEVENT_LAYER" );
	case xSpr::xLT_DUMMY:	return _T( "xDUMMY_LAYER" );
	default:
		XLOG( "잘못된 레이어 타입 %d", (int)type );
		return _T( "xERROR_LAYER" );
	}
}
LPCTSTR XBaseLayer::GetTypeString( xSpr::xtLayer type /*= xSpr::xLT_NONE*/ ) const 
{
	if( type == xSpr::xLT_NONE )
		type = GetbitType();
	return GetTypeStr( type );
}

/**
 파일스트림에서 데이터를 읽어 그에맞는 레이어객체를 생성시키고 값을 입력한다.
*/
// XSPBaseLayer XBaseLayer::sCreateRead( XResFile *pRes, int verSprDat )
// {
// 	BYTE b0;
// 	ID idLayer;
// 	(*pRes) >> idLayer;
// 	(*pRes) >> b0;	auto type = (xSpr::xtLayer)b0;
// 	//
// 	auto spLayer = XBaseLayer::sCreate( type );
// 	if( spLayer == nullptr )
// 		return XSPBaseLayer();
// 	spLayer->m_idLayer = idLayer;
// 	(*pRes) >> b0;		spLayer->m_nLayer = b0;
// 	(*pRes) >> spLayer->m_fLastFrame;
// 	XE::VEC2 vAdjAxis;
// 	(*pRes) >> vAdjAxis.x;
// 	(*pRes) >> vAdjAxis.y;
// 	if( verSprDat < 15 ) {
// 		vAdjAxis *= 0.5f;
// 	}
// 	if( spLayer->IsTypeLayerMove() ) {
// 		auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
// 		if( XASSERT(spLayerMove) )
// 			spLayerMove-> SetvAdjustAxis( vAdjAxis );
// 	}
// 	return spLayer;
// }

// void XBaseLayer::sSave( XResFile *pRes, XSPBaseLayer spLayer )
// {
// 	(*pRes) << spLayer->m_idLayer;
// 	(*pRes) << (BYTE)spLayer->m_Type;
// 	(*pRes) << (BYTE)spLayer->m_nLayer;
// 	(*pRes) << spLayer->m_fLastFrame;
// // 	pRes->Write( spLayer->m_idLayer );
// // 	pRes->Write( (BYTE)spLayer->m_Type );		// layer type
// // 	pRes->Write( (BYTE)spLayer->m_nLayer );
// // 	pRes->Write( spLayer->m_fLastFrame );
// 	XE::VEC2 vAdjustAxis;
// 	if( spLayer->IsTypeLayerMove() ) {
// 		auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );     
// 		if( XASSERT( spLayerMove ) )
// 			vAdjustAxis = spLayerMove->GetvAdjustAxis();
// 	}
// 	(*pRes) << vAdjustAxis;
// // 	pRes->Write( vAdjustAxis.x );		// ver 8
// // 	pRes->Write( vAdjustAxis.y );
// }
// void XBaseLayer::Load( XResFile *pRes )
// {
// }
// +표시를 눌렀는지 검사
BOOL XBaseLayer::CheckPushPlus( CPoint point )
{
	if( point.x > 8 )	return FALSE;
	return TRUE;
}
CString XBaseLayer::GetLabel()
{
	CString str;
#ifdef _DEBUG
	str.Format( _T("%s (id:%d)"), (LPCTSTR)GetstrLabel(), GetidLayer() );
#else
	str.Format( _T("%s"), (LPCTSTR)GetstrLabel() );
#endif
	return str;

}

void XBaseLayer::DrawLabel( float left, float top, XCOLOR colFont )
{
	float y = top+Getscry()+3;
	XCOLOR color = XCOLOR_WHITE;
	if( !GetbShow() )			
		color = XCOLOR_LIGHTGRAY;		// hide된 레이어는 다른색으로 그림
	SE::g_pFont->SetColor( colFont );
	SE::g_pFont->DrawString( left + 10, y, (LPCTSTR)GetLabel() );		y += KEY_HEIGHT;
	{
		if( GetbOpen() ) {
			SE::g_pFont->DrawString( left + 20, y, _T("Pos") );		y += KEY_HEIGHT;
			if( m_dwDrawChannel & CHA_ROT ) {
				SE::g_pFont->DrawString( left + 20, y, _T("Rotate") );	y += KEY_HEIGHT;
			}
			if( m_dwDrawChannel & CHA_SCALE ) {
				SE::g_pFont->DrawString( left + 20, y, _T("Scale") );		y += KEY_HEIGHT;
			}
			if( m_dwDrawChannel & CHA_EFFECT ) {
				SE::g_pFont->DrawString( left + 20, y, _T("Effect") );		y += KEY_HEIGHT;
			}
		}
	}
}
// FrameView에서 표시될 레이어의 세로크기를 구한다. 
float XBaseLayer::GetHeight()
{
	float sh = KEY_HEIGHT;			// text label 부분
	if( GetbOpen() ) {
		sh += KEY_HEIGHT;				// 기본 pos채널
		if( GetBitDrawChannel( CHA_ROT ) )
			sh += KEY_HEIGHT;			// Rotate 채널이 열려있으면 한줄 더 추가
		if( GetBitDrawChannel( CHA_SCALE ) )
			sh += KEY_HEIGHT;			
		if( GetBitDrawChannel( CHA_EFFECT ) )
			sh += KEY_HEIGHT;			
	}
	return sh;
}

bool XBaseLayer::IsSameLayer( XBaseKey* pKey ) const
{
	if( !pKey )
		return false;
	return m_idLayer == pKey->GetidLayer();
}
