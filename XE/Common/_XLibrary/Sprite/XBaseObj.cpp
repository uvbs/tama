/*
 *  XBaseObj.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 1. 27..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "XBaseObj.h"
//#include "XFactory.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseObj::XBaseObj( LPCTSTR szSprName, ID idAct, xRPT_TYPE playType ) 
{
	_Init();
	m_pSprObj = CreateSprObj( szSprName, this );		
	m_pSprObj->SetAction( idAct, playType );
}

XSprObj* XBaseObj::CreateSprObj( LPCTSTR szSpr, XDelegateSprObj *pDelegate ) 
{
	return new XSprObj( szSpr, pDelegate );
}
