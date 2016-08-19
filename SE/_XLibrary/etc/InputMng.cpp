/*
 *  InputMng.cpp
 *  OmniHellMP
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "InputMng.h"

XInputMng* XInputMng::s_pInputMng;

// WM_KEYUP에 관한 플래그를 클리어함.
void XInputMng::ClearUpMsg( void )
{
	m_dwInput &= ~INPUT_UPMSG;
}
