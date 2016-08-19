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
#include "XFactory.h"

XBaseObj::XBaseObj( LPCTSTR szSprName, ID idAct, xRPT_TYPE playType ) 
{
	_Init();
	m_pSprObj = FACTORY->CreateSprObj( szSprName, this );		
	m_pSprObj->SetAction( idAct, playType );
}
