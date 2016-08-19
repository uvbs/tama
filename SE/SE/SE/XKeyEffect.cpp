#include "stdafx.h"
#include "XKeyEffect.h"
#include "XLayerMove.h"
#include "SEFont.h"
#include "DlgDrawProperty.h"
#include "SprObj.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////// Key Effect ////////////////////////////////////////
XKeyEffect::XKeyEffect( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer )
	: XBaseKey( spAction, type, xSpr::xKTS_EFFECT, fFrame, spLayer )
{
	Init();
	//m_spLayer = spLayer;
}

XBaseKey* XKeyEffect::CreateCopy()
{
//	return new XKeyEffect( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeyEffect( *this );		// 기본카피
//	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}
void XKeyEffect::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
	auto pSprObj = spActObj->GetpSprObj();
	auto bPlaying = pSprObj->IsPlaying();
	if( pSprObj->GetpParent() )
		bPlaying = pSprObj->GetpParent()->IsPlaying();
	auto spLayerMove = std::static_pointer_cast<XLayerMove>(spLayer);
	if( XASSERT( spLayerMove ) ) {
		auto& cn = spLayerMove->GetcnEffectMutable();
		cn.interpolation = m_Interpolation;
		if( bPlaying ) {
			cn.fAlphaSrc = GetOpacityRandomed();
		} else {
			cn.fAlphaSrc = m_fOpacityOrig;
		}
		cn.fStartKeyFrame = GetfFrame();
		auto pNextKey = dynamic_cast<XKeyEffect *>(GetNextKey());
		if( pNextKey ) {
			if( bPlaying ) {
				cn.fAlphaDest = pNextKey->GetOpacityRandomed();
			} else {
				cn.fAlphaDest = pNextKey->GetfOpacityOrig();
			}
			cn.fNextKeyFrame = pNextKey->GetfFrame();
		} else {
			cn.interpolation = xSpr::xNONE;
			cn.fNextKeyFrame = GetfFrame();
		}
		cn.DrawMode = m_DrawMode;
		cn.dwDrawFlag = m_dwFlag;
		if( bPlaying ) {
			m_fOpacityRandomed = AssignRandom();
		} else {
			m_fOpacityRandomed = m_fOpacityOrig;
		}
	} // spLayerMove

	// 	auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
// 	if( XASSERT( spLayerMove ) )
// 		spLayerMove->GetcnEffectMutable().fAlpha = m_fOpacity;
// 	// 실시간으로 이 키의 뒤에 포지션키가 있는지 찾는다
// 	XKeyEffect *pNextKey = dynamic_cast<XKeyEffect *>( GetNextKey() );
// 
// 	if( m_Interpolation && pNextKey )			// 다음키까지 보간모드로 변해야 하고 다음키가 있으면
// 		spLayerMove->SetcnEffect( m_Interpolation, pNextKey->GetfOpacity(), m_fOpacity, m_dwFlag, m_DrawMode, GetfFrame(), pNextKey->GetfFrame() );
// 	else
// 		spLayerMove->SetcnEffect( xSpr::xNONE, 0, 0, m_dwFlag, m_DrawMode, 0, 0 );
}

void XKeyEffect::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	BYTE b1;
	short s0;
	b1 = (BYTE)m_Interpolation;							pRes->Write( &b1, 1 );
	b1 = (BYTE)m_DrawMode;									pRes->Write( &b1, 1 );
	s0 = (short)(m_fOpacityOrig * 100.f);		pRes->Write( &s0, 2 );
	pRes->Write( &m_dwFlag, 4 );
	s0 = (short)(m_vRange.v1 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRange.v2 * 100.f);		pRes->Write( &s0, 2 );
	//	pRes->Write( &dw1, 4 );			// reserved

// 	pRes->Write( &b1, 1 );
// 	pRes->Write( &m_dwFlag, 4 );
// 	b1 = (BYTE)m_DrawMode;
// 	pRes->Write( &b1, 1 );
// 	pRes->Write( &m_fOpacityOrig, 4 );
// 	DWORD dw1 = 0;
// 	pRes->Write( &dw1, 4 );			// reserved
}
void XKeyEffect::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	BYTE b1;
	if( ver >= 31 ) {
		short s0;
		pRes->Read( &b1, 1 );		m_Interpolation = (xSpr::xtInterpolation)b1;
		pRes->Read( &b1, 1 );		m_DrawMode = (xDM_TYPE)b1;
		pRes->Read( &s0, 2 );		m_fOpacityOrig = ((float)s0 / 100.f);
		pRes->Read( &m_dwFlag, 4 );
		pRes->Read( &s0, 2 );		m_vRange.v1 = ((float)s0) / 100.f;
		pRes->Read( &s0, 2 );		m_vRange.v2 = ((float)s0) / 100.f;
	} else {
		pRes->Read( &b1, 1 );		m_Interpolation = (xSpr::xtInterpolation)b1;
		pRes->Read( &m_dwFlag, 4 );
		pRes->Read( &b1, 1 );		m_DrawMode = (xDM_TYPE)b1;
		pRes->Read( &m_fOpacityOrig, 4 );
		DWORD dw1;
		pRes->Read( &dw1, 4 );		// reserved
	}
}
BOOL XKeyEffect::EditDialog()
{
	CDlgDrawProperty dlg;
	dlg.m_bFlipHoriz = GetFlipHoriz();
	dlg.m_bFlipVert = GetFlipVert();
	dlg.m_fOpacity = GetfOpacityOrig();
	dlg.m_DrawMode = GetDrawMode();
	dlg.m_bInterpolation = ( GetInterpolation() ) ? TRUE : FALSE;
	dlg.m_Interpolation = GetInterpolation();
	dlg.m_vRangeAlpha = m_vRange * 100.f;
	if( dlg.DoModal() == IDOK )	{
		DWORD dwFlag = 0;
		if( dlg.m_bFlipHoriz )				
			dwFlag |= EFF_FLIP_HORIZ;
		if( dlg.m_bFlipVert )					
			dwFlag |= EFF_FLIP_VERT;
		m_vRange = dlg.m_vRangeAlpha / 100.f;
		auto funcInterpolation = m_Interpolation;
		if( dlg.m_bInterpolation == FALSE )
			funcInterpolation = xSpr::xNONE;
		else
			if( !funcInterpolation )
				funcInterpolation = xSpr::xLINEAR;
		Set( funcInterpolation, dwFlag, dlg.m_DrawMode, dlg.m_fOpacity );
		return TRUE;
	}
	return FALSE;
}
void XKeyEffect::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 7;
	*h += SE::g_pFont->GetFontHeight() * 4;
}
CString XKeyEffect::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str, strFlag, strBlend;
	if( GetFlipHoriz() )		strFlag += "좌우반전,";
	if( GetFlipVert() )		strFlag += "상하반전,";
	switch( m_DrawMode )
	{
	case xDM_NONE:		strBlend += "그리지않음"; break;
	case xDM_NORMAL:	strBlend += "Normal(Multiply)";	break;
	case xDM_ADD:		strBlend += "Add(linear dodge)"; break;
	case xDM_MULTIPLY:		strBlend += "Multiply"; break;
	case xDM_SUBTRACT:		strBlend += "Subtract"; break;
	case xDM_LIGHTEN:		strBlend += "Lighten"; break;
	case xDM_DARKEN:		strBlend += "Darken"; break;
	case xDM_SCREEN:		strBlend += "Screen"; break;
	case xDM_GRAY:		strBlend += "Gray"; break;
	}
	str.Format( _T( "보간:%d\n효과:%s\n블렌딩:%s\n투명도:%d%%" )
							, (int)m_Interpolation, strFlag, strBlend, (int)( m_fOpacityOrig * 100.0f ) );
#ifdef _DEBUG
	str = XBaseKey::GetToolTipString( spLayer ) + str;
#endif
	str += GetToolTipStringTail();
	return str;
}

