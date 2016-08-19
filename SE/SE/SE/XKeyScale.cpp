#include "stdafx.h"
#include "XKeyScale.h"
#include "XLayerMove.h"
#include "SEFont.h"
#include "DlgScaleProp.h"
#include "SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////// Key Scale /////////////////////////////////////////
XKeyScale::XKeyScale( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float sx, float sy )
	: XBaseKey( spAction, type, sGetChannelType(), fFrame, spLayer )
{
	Init();
	m_vScaleOrig.Set( sx, sy );
}
XBaseKey* XKeyScale::CreateCopy()
{
	auto pKeyNew = new XKeyScale( *this );		// 기본카피
	return pKeyNew;
}
void XKeyScale::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	BYTE b1;
	DWORD dw1 = 0;
	b1 = (BYTE)m_Interpolation;		pRes->Write( &b1, 1 );
	short s0;
	s0 = (short)(m_vRangeX.v1 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRangeX.v2 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRangeY.v1 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRangeY.v2 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRangeXY.v1 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRangeXY.v2 * 100.f);		pRes->Write( &s0, 2 );
// 	pRes->Write( &dw1, 4 );		// reserved
// 	pRes->Write( &dw1, 4 );		// reserved
	pRes->Write( &m_vScaleOrig.x, 4 );
	pRes->Write( &m_vScaleOrig.y, 4 );
}
void XKeyScale::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = ( xSpr::xtInterpolation )b1;
	if( ver >= 31 ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRangeX.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeX.v2 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeY.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeY.v2 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeXY.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeXY.v2 = (float)s0 / 100.f;
	} else {
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
	}
	pRes->Read( &m_vScaleOrig.x, 4 );
	pRes->Read( &m_vScaleOrig.y, 4 );

}

/**
@brief 키의 위치값을 얻는다. 아직 랜덤적용이 안된상태면 랜덤적용 시킨값을 생성해 리턴한다.
*/
XE::VEC2 XKeyScale::GetvScaleRandomed()
{
	if( m_vScaleRandomed.IsInit() ) {
		m_vScaleRandomed = AssignRandom();
	}
	return m_vScaleRandomed;
}

XE::VEC2 XKeyScale::AssignRandom() const
{
	XE::VEC2 vRandomed;
	vRandomed = m_vScaleOrig;
	if( !m_vRangeXY.IsZero() ) {
		const float rangeRandom = xRandomF( m_vRangeXY.v1, m_vRangeXY.v2 );
		vRandomed += XE::VEC2( rangeRandom, rangeRandom );
	} else {
		if( !m_vRangeX.IsZero() ) {
			vRandomed.x += xRandomF( m_vRangeX.v1, m_vRangeX.v2 );
		}
		if( !m_vRangeY.IsZero() ) {
			vRandomed.y += xRandomF( m_vRangeY.v1, m_vRangeY.v2 );
		}
	}
	return vRandomed;
}

void XKeyScale::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
// 	const auto bPlaying = spActObj->GetpSprObj()->IsPlaying();
	auto pSprObj = spActObj->GetpSprObj();
	auto bPlaying = pSprObj->IsPlaying();
	if( pSprObj->GetpParent() )
		bPlaying = pSprObj->GetpParent()->IsPlaying();
	auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
	if( XASSERT( spLayerMove ) ) {
		auto& cn = spLayerMove->GetcnScaleMutable();
		// 실시간으로 이 키의 뒤에 같은타입의 키가 있는지 찾는다
		auto pNextKey = dynamic_cast<XKeyScale *>( GetNextKey() );
		if( pNextKey ) {
			cn.interpolation = m_Interpolation;
			if( bPlaying )
				cn.vScaleDest = pNextKey->GetvScaleRandomed();
			else
				cn.vScaleDest = pNextKey->GetvScaleOrig();
//			cn.vScaleSrc = m_vScaleOrig;
			cn.vScaleSrc = GetvScaleRandomed();
			cn.fNextKeyFrame = pNextKey->GetfFrame();
			cn.fStartKeyFrame = GetfFrame();
		} else {
			cn.interpolation = xSpr::xNONE;
			cn.vScaleDest = m_vScaleOrig;
			cn.vScaleSrc = GetvScaleRandomed();
			cn.fNextKeyFrame = GetfFrame();
			cn.fStartKeyFrame = GetfFrame();
		}
		if( bPlaying )
			m_vScaleRandomed = AssignRandom();		// 다음 보간을 위해 랜덤값 만듬.
		else
			m_vScaleRandomed = m_vScaleOrig;		// 에디팅 모드에선 오리지날 값만 씀.
	}
// 	if( XASSERT( spLayerMove ) ) {
// 		spLayerMove->SetScale( m_vScale );
// 		CHANNEL_SCALE cn;
// 		cn.vScale = m_vScale;
// 		cn.interpolation = m_Interpolation;
// 		// 		spLayerMove->SetScale( m_fScaleX, m_fScaleY );
// 		// 실시간으로 이 키의 뒤에 같은타입의 키가 있는지 찾는다
// 		auto pNextKey = dynamic_cast<XKeyScale *>( GetNextKey() );
// 		if( m_Interpolation && pNextKey ) {			// 다음키가 있으면 보간으로 움직이도록 한다
// 			cn.vScaleDest = pNextKey->GetvScale();
// 			cn.vScaleSrc = m_vScale;
// 			cn.fNextKeyFrame = pNextKey->GetfFrame();
// 			cn.fStartKeyFrame = GetfFrame();
// 		} else {
// 			cn.interpolation = xSpr::xNONE;
// 			cn.vScaleDest.Set( 1.f );
// 			cn.vScaleSrc.Set( 1.f );
// 			cn.fNextKeyFrame = 0;
// 			cn.fStartKeyFrame = 0;
// 		}
// 		// 채널정보 씀
// 		spLayerMove->SetcnScale( cn );
// 	}
}
BOOL XKeyScale::EditDialog()
{
	CDlgScaleProp dlg;
	dlg.m_fScaleX = m_vScaleOrig.x;
	dlg.m_fScaleY = m_vScaleOrig.y;
// 	dlg.m_rangeXY1 = m_vRangeXY.v1;
// 	dlg.m_rangeXY2 = m_vRangeXY.v2;
// 	dlg.m_rangeX1 = m_vRangeX.v1;
// 	dlg.m_rangeX2 = m_vRangeX.v2;
// 	dlg.m_rangeY1 = m_vRangeY.v1;
// 	dlg.m_rangeY2 = m_vRangeY.v2;
	dlg.m_rangeXY = m_vRangeXY;
	dlg.m_rangeX = m_vRangeX;
	dlg.m_rangeY = m_vRangeY;
	dlg.m_bInterpolation = ( GetInterpolation() ) ? TRUE : FALSE;
	dlg.m_Interpolation = GetInterpolation();
	if( dlg.DoModal() == IDOK )	{
// 		CONSOLE("3(%.1f, %.1f), (%.1f, %.1f), (%.1f, %.1f) "
// 						 , dlg.m_rangeX1, dlg.m_rangeX2
// 						 , dlg.m_rangeY1, dlg.m_rangeY2 
// 						 , dlg.m_rangeXY1, dlg.m_rangeXY2 );
// 		CONSOLE("==========================");
		m_vScaleOrig.Set( dlg.m_fScaleX, dlg.m_fScaleY );
// 		m_vRangeXY.v1 = dlg.m_rangeXY1;
// 		m_vRangeXY.v2 = dlg.m_rangeXY2;
// 		m_vRangeX.v1 = dlg.m_rangeX1;
// 		m_vRangeX.v2 = dlg.m_rangeX2;
// 		m_vRangeY.v1 = dlg.m_rangeX1;
// 		m_vRangeY.v2 = dlg.m_rangeY2;
// 		CONSOLE("1");
// 		XBREAK( m_vRangeX.v1 != 0 );
// 		m_vScaleOrig.Set( dlg.m_fScaleX, dlg.m_fScaleY );
// 		CONSOLE( "2" );
// 		m_vRangeXY.v1 = dlg.m_rangeXY1;
// 		m_vRangeXY.v2 = dlg.m_rangeXY2;
// 		CONSOLE( "3" );
// 		m_vRangeX.v1 = dlg.m_rangeX1;
// 		CONSOLE( "4" );
// 		XBREAK( m_vRangeX.v1 != 0 );
// 		m_vRangeY.v2 = dlg.m_rangeY1;
// 		CONSOLE( "5" );
// 		XBREAK( m_vRangeX.v1 != 0 );
// 		m_vRangeXY.v2 = dlg.m_rangeXY2;
// 		CONSOLE( "6" );
// 		XBREAK( m_vRangeX.v1 != 0 );
// 		m_vRangeX.v2 = dlg.m_rangeX2;
// 		CONSOLE( "7" );
// 		XBREAK( m_vRangeX.v1 != 0 );
// 		m_vRangeY.v2 = dlg.m_rangeY2;
// 		XBREAK( m_vRangeX.v1 != 0 );
		m_vRangeXY = dlg.m_rangeXY;
		m_vRangeX = dlg.m_rangeX;
		m_vRangeY = dlg.m_rangeY;
		m_Interpolation = dlg.m_Interpolation;
		if( dlg.m_bInterpolation == FALSE )
			m_Interpolation = xSpr::xNONE;
		else
		if( !m_Interpolation )
			m_Interpolation = xSpr::xLINEAR;
		return TRUE;
	}
	return FALSE;
}
void XKeyScale::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 6;
	*h += SE::g_pFont->GetFontHeight() * 2;
}
CString XKeyScale::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str;
	str.Format( _T( "보간:%d\nscale x:%.2f, y:%.2f" ), (int)m_Interpolation, m_vScaleOrig.x, m_vScaleOrig.y );
#ifdef _DEBUG
	str = XBaseKey::GetToolTipString( spLayer ) + str;
#endif
	str += GetToolTipStringTail();
	return str;
}
