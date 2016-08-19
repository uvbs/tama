#include "stdafx.h"
#include "XLayerEvent.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// XSPBaseLayer XLayerEvent::CopyDeep()
// {
// 	return XSPBaseLayer( new XLayerEvent( *this ) );
// }

XSPBaseLayer XLayerEvent::CreateCopy( XSPActObjConst spActObjSrc ) const
{
	auto spNewLayer = XSPLayerEvent( new XLayerEvent( *this ) );
	spNewLayer->OnCreate();
	return spNewLayer;
}
