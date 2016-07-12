﻿#include "stdafx.h"
#include "XGame.h"
#include "XMain.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGame *XGame::s_pGame = NULL;
XGame* XGame::sCreateInstance( void ) 
{
	s_pGame = new XGame;
	return s_pGame;
}

XGame::XGame()
{
	Init();
}

void XGame::Destroy()
{
}

void XGame::Create( void )
{
	// 클라/서버 공통 데이타 로딩
	XGameCommon::Create();
	XGameCommon::CreateCommon();
	XBREAK( XGlobalConst::sGet()->IsError() );    // 스레드내부에서 로딩하지 않도록 미리 읽음.
}


void XGame::FrameMove( float dt )
{
	int n = 0;
}

