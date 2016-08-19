#include "stdafx.h"
#include "XKeySound.h"
#include "XLayerSound.h"
#include "SEFont.h"
#include "DlgSoundProp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
#include "XSoundMng.h"
XKeySound::XKeySound( XSPAction spAction, float fFrame, XSPLayerSound spLayer, ID idSound )
	: XBaseKey( spAction, xSpr::xKT_SOUND, xSpr::xKTS_MAIN, fFrame, spLayer )
{
	Init();
	m_idSound = idSound;
	//m_spLayer = spLayer;
}

XBaseKey* XKeySound::CreateCopy()
{
//	return new XKeySound( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeySound( *this );		// 기본카피
//	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}

// void XKeySound::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerSound>( spLayer );
// }
// XBaseKey* XKeySound::CopyDeep()
// {
// 	return new XKeySound( *this );
// }
void XKeySound::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
	SOUNDMNG->OpenPlaySound( m_idSound );
}

void XKeySound::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	pRes->Write( &m_idSound, 4 );
	pRes->Write( &m_fVolume, 4 );
	char buff[ 32 ];
	memset( buff, 0, sizeof( buff ) );
	pRes->Write( buff, sizeof( buff ) );		// reserved
}
void XKeySound::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerSound>( XBaseKey::GetspLayer() );
	pRes->Read( &m_idSound, 4 );
	pRes->Read( &m_fVolume, 4 );
	char buff[ 32 ];
	pRes->Read( buff, 32 );		// reserved
}

BOOL XKeySound::EditDialog()
{
	CDlgSoundProp dlg;
	dlg.m_idSound = m_idSound;
	if( dlg.DoModal() == IDOK )
	{
		m_idSound = dlg.m_idSound;
		return TRUE;
	}
	return FALSE;
}

void XKeySound::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 7;
	*h += SE::g_pFont->GetFontHeight() * 1;
}
CString XKeySound::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str;
	str.Format( _T( "사운드 ID:%04d" ), m_idSound );
#ifdef _DEBUG
	str = XBaseKey::GetToolTipString( spLayer ) + str;
#endif
	str += GetToolTipStringTail();
	return str;
}

