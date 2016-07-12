/*
 *  InputMng.cpp
 *  OmniHellMP
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "etc/InputMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XInputMng* XInputMng::s_pInputMng;
XE::xtDevice XInputMng::s_Device = XE::DEVICE_UNKNOWN;

namespace XE
{
    LPCTSTR GetDeviceString( xtDevice device )
    {
        switch( device )
        {
            case DEVICE_IPHONE: return _T("iPhone");
            case DEVICE_IPOD:   return _T("iPod");
            case DEVICE_IPAD:   return _T("iPad");
            case DEVICE_ANDROID:   return _T("Android");
            case DEVICE_WINDOWS:    return _T("Windows");
            default:    return _T("Unknown");
        }
    }
};

// WM_KEYUP에 관한 플래그를 클리어함.
/*void XInputMng::ClearUpMsg( void )
{
	m_dwInput &= ~INPUT_UPMSG;
}*/
