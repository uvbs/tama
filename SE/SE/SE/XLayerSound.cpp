#include "stdafx.h"
#include "XLayerSound.h"
#include "XAniAction.h"
#include "XKeyBase.h"
#include "XKeySound.h"
#include "xLayerInfo.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XSPBaseLayer XLayerSound::CreateCopy( XSPActObjConst spActObjSrc ) const
{
	auto spNewLayer = XSPLayerSound( new XLayerSound( *this ) );
	spNewLayer->OnCreate();
	return spNewLayer;
}

void XLayerSound::DrawLabel( float left, float top, XCOLOR colFont )
{
	SE::g_pFont->SetColor( colFont );
	SE::g_pFont->DrawString( left + 10, top + Getscry() + 3, (LPCTSTR)GetLabel() );
}
XBaseKey* XLayerSound::DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType )
{
	XBaseKey *pNewKey = spAction->AddKeySound( fFrame, GetThis(), 0 );
	return pNewKey;
}


// XSPBaseLayer XLayerSound::CopyDeep()
// {
// 	return XSPBaseLayer( new XLayerSound( *this ) );
// }
