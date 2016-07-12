#include "stdafx.h"
#include "XNetworkDelegate.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XNetworkDelegate::DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error )
{
	// 사용자가 특별히 델리게이트를 설정하지 않았을때 기본처리들
	switch( error )
	{
	case XE::xN_FAILED_CREATE_SOCKET:
		XALERT( "create socket failed" );
		break;
	case XE::xN_FAILED_BIND_SOCKET: {
		XBREAK( 1 );
		XALERT( "Network bind error" );
	} break;
	case XE::xN_TIMEOUT:
		XALERT( "connect timeout!" );
		break;
	default:
		XALERT( "Network error!" );
		break;
	}
}


