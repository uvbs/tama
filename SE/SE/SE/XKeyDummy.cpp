#include "stdafx.h"
#include "XKeyDummy.h"
#include "XLayerDummy.h"
#include "SEFont.h"
#include "DlgDummyProp.h"
#include "SprObj.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
XKeyDummy::XKeyDummy( XSPAction spAction, float fFrame, XSPLayerDummy spLayer, BOOL bActive )
	: XBaseKey( spAction, xSpr::xKT_DUMMY, xSpr::xKTS_MAIN, fFrame, spLayer )
{
	Init();
	m_bActive = bActive;
	//m_spLayer = spLayer;
}
// void XKeyDummy::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerDummy>( spLayer );
// }
// XBaseKey* XKeyDummy::CopyDeep()
// {
// 	return new XKeyDummy( *this );
// 	// 	XKeyDummy *pKey = new XKeyDummy( spAction, GetfFrame(), GetLayerType(), GetnLayer(), m_bActive );
// 	// 	pKey->SetidKey( GetidKey() );
// 	// 	return pKey;
// }
XBaseKey* XKeyDummy::CreateCopy()
{
//	return new XKeyDummy( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeyDummy( *this );		// 기본카피
//	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}
void XKeyDummy::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	pRes->Write( &m_bActive, 4 );				// do on/off
	pRes->Write( &m_id, 4 );
	DWORD dw[ 4 ] = {0, };
	pRes->Write( dw, sizeof( DWORD ), 3 );		// reserved
}
void XKeyDummy::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerDummy>( XBaseKey::GetspLayer() );
	pRes->Read( &m_bActive, 4 );
	DWORD dw[ 4 ] = {0, };
	pRes->Read( &m_id, 4 );
	pRes->Read( dw, sizeof( DWORD ), 3 );
}
void XKeyDummy::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
	XBaseKey::Execute( spActObj, spLayer, fOverSec );
	//	auto spLayer = SafeCast<XLayerDummy*, SPBaseLayer>( GetspLayer( pSprObj ) );
// 	auto spLayerDummy = pSprObj->GetspActObjCurr()->GetspLayerByidLayer<XLayerDummy>( GetidLayer() );
	auto spLayerDummy = std::static_pointer_cast<XLayerDummy>( spLayer );;
	if( XASSERT( spLayerDummy ) )
		spLayerDummy->SetbActive( m_bActive );		// 더미레이어의 더미를 켜거나 끈다.
}

BOOL XKeyDummy::EditDialog()
{
	CDlgDummyProp dlg;
	dlg.m_idDummy = m_id;
	dlg.m_bActive = m_bActive;
	if( dlg.DoModal() == IDOK )
	{
		m_bActive = dlg.m_bActive;
		m_id = dlg.m_idDummy;
		return TRUE;
	}
	return FALSE;
}
void XKeyDummy::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 5;
	*h += SE::g_pFont->GetFontHeight() * 2;
}
CString XKeyDummy::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str;
	str.Format( _T( "%s\nID:%d" ), ( m_bActive ) ? _T( "On" ) : _T( "Off" ), m_id );
	str = XBaseKey::GetToolTipString( spLayer ) + str;
	str += GetToolTipStringTail();
	return str;
}
