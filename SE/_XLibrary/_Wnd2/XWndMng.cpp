/*
 *  XWndMng.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 1. 13..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "XWndMng.h"
#include "XImage.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//XWndMng *WNDMNG = NULL;

ID XWndMng::s_idGlobal = 0x80000000;		// 자동 아이디 생성용. 이거는 프로그램실행후엔 초기화시키는일이 없어야함. 버프아이콘윈도우 삭제땜에 그럼

BOOL XWndMng::LoadResAll( void )
{
	LoadResFrame( _T("baseDialog.png"), &m_Res.frameDialog );
	LoadResFrame( _T("baseButton_up.png"), &m_Res.frameButtonUp );
	LoadResFrame( _T("baseButton_down.png"), &m_Res.frameButtonDown );
	LoadResFrame( _T("baseWindow.png"), &m_Res.frameView );
	return TRUE;
}

BOOL XWndMng::LoadResFrame( LPCTSTR szFile, XWND_RES_FRAME *pFrame, int surfaceBlockSize )
{
	_tcscpy_s( pFrame->szFrame, szFile );
//	LPCTSTR szRes = XE::MakePath( DIR_UI, pFrame->szFrame );
	LPCTSTR szRes = XE::MakePath( DIR_UI, szFile );
	XASSERT( surfaceBlockSize == 0 || surfaceBlockSize == 16 || surfaceBlockSize == 32 || surfaceBlockSize == 64 );
	pFrame->nBlockSize = surfaceBlockSize;
	// png를 읽음
#ifdef WIN32
	XImageD3D<DWORD> image( TRUE );		// UI리소스는 모두 고해상도 리소스를 사용
#else
	XImageiOS<DWORD> image( TRUE );
#endif
	if( image.Load( szRes ) == FALSE )		
		return FALSE;
	// 블럭사이즈 자동계산
	if( surfaceBlockSize == 0 )
	{
		XBREAKF( (image.GetWidth() % 3) != 0, "%s의 이미지가 3으로 나누어 떨어져야 합니다", szRes );
		pFrame->nBlockSize = surfaceBlockSize = image.GetWidth() / 3;
	}
	int iw = image.GetMemWidth();		// 이미지 메모리 크기
	int ih = image.GetMemHeight();
	int memBlockSize = image.ConvertToMemSize( (float)surfaceBlockSize );	// 실제 메모리 블럭 사이즈
	// 9개의 조각으로 서피스를 나눔
	DWORD *pImg = image.GetTextureData();
	pFrame->psfcFrame[0] = GRAPHICS->CreateSurface( TRUE, 0, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 첫째줄
	pFrame->psfcFrame[1] = GRAPHICS->CreateSurface( TRUE, memBlockSize, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[2] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[3] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 둘째줄
	pFrame->psfcFrame[4] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[5] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[6] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 셋째줄
	pFrame->psfcFrame[7] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[8] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	return TRUE;
}


