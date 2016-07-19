#include "stdafx.h"
#include "client/XAppMain.h"
#include "DlgEnterName.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XAppMain *XAPP = NULL;

XAppMain* XAppMain::sCreate( XE::xtDevice device, int widthPhy, int heightPhy )
{
	XBREAK( XAPP != NULL );
	XAPP = new XAppMain;
	XAPP->Create( device, 
					widthPhy,	// 하드웨어 물리적 해상도
					heightPhy,	// 
					(float)XRESO_WIDTH,		// 게임의 논리적 해상도
					(float)XRESO_HEIGHT );
	return XAPP;
}
//////////////////////////////////////////////////////////////////////////
XAppMain::XAppMain()
{
	XBREAK( XAPP != NULL );
	XAPP = this;
	Init();
	//
	SetbDebugMode(TRUE);
}

void XAppMain::Destroy() 
{
	XBREAK( XAPP == NULL );
	XAPP = NULL;
}

void XAppMain::DidFinishCreate( void )
{
}

// 엔진 초기화 완료.
void XAppMain::DidFinishInitEngine( void )
{
}

// 매 루프 돌아가야 하는 게임객체가 있다면 생성을 하위에 맡긴다.
XEContent* XAppMain::CreateGame( void )
{
	XGame *pGame = new XGame;
	pGame->Create();
	return pGame;
}

void XAppMain::ConsoleMessage( LPCTSTR szMsg )
{
#ifdef WIN32
//	if( GetDlgConsole() )
//		GetDlgConsole()->MessageString( szMsg );
#endif
}

void XAppMain::SaveCheat( FILE *fp )
{
//	fprintf( fp, "sample_mode1 = %d\r\n", m_SampleMode1 );
//	fprintf( fp, "sample_mode2 = %d\r\n", m_SampleMode2 );
}
void XAppMain::LoadCheat( CToken& token )
{
	if( token == _T("sample_mode1") )
	{
		//token.GetToken();	// =
		//m_SampleMode1 = token.GetNumber();
	} else
	if( token == _T("sample_mode2") )
	{
		//token.GetToken();	// =
		//m_SampleMode2 = token.GetNumber();
	}

}

void XAppMain::Draw( void )
{
	XClientMain::Draw();
#ifdef _VER_IOS
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
	{
		if( m_timerMem.IsOff() )
			m_timerMem.Set( 1.0f );
		if( m_timerMem.IsOver() )
		{
			IOS::GetFreeMem( &m_memInfo );
			m_timerMem.Reset();
		}
		float freeMB = m_memInfo.free / 1000.f / 1000.f;
		float totalMB = m_memInfo.total / 1000.f / 1000.f;
		XCOLOR colFree = XCOLOR_BLUE;
		if( freeMB < totalMB * 0.1f )
			colFree = XCOLOR_RED;
		PUT_STRINGF( 0, 50, colFree, "free:%.2fMB", freeMB );
		PUT_STRINGF( 0, 60, XCOLOR_BLUE, "total:%.2fMB", totalMB );
	}
#endif
#endif
}

void XAppMain::OnError( XE::xtError codeError, DWORD p1, DWORD p2 )
{
	switch( codeError )
	{
		// 기본 시스템 폰트가 없거나 손상되었다.
	case XE::xERR_FAIL_CREATE_ASIC_FONT:
		{
			if( XE::GetLoadType() == XE::xLT_WORK_TO_PACKAGE_COPY )
			{
				// 패키지에 있는 파일을 워크에 카피해 넣는다.
				LPCTSTR szFont = (LPCTSTR)p1;
				XE::CopyPackageToWork( XE::MakePath( DIR_FONT, szFont ) );
				XALERT("error! app restart please.");
				DoExit();
			}
		}
		break;
	}
}

BOOL XAppMain::DoEditBox( TCHAR *pOutText, int lenOut ) 
{ 
#ifdef WIN32
	XBREAK( pOutText == NULL );
	CDlgEnterName dlg;		// 이거 CIV전용이니까 일반화 시킬것
	dlg.m_strName = pOutText;
	if( dlg.DoModal() == IDOK )
	{
		if( dlg.m_strName.GetLength() < lenOut )
		{
			_tcscpy_s( pOutText, lenOut, dlg.m_strName );
			return TRUE;
		} else
			XLOGXN("input editbox string too long: buffsize=%d, string size=%d", lenOut, dlg.m_strName.GetLength() );
	}
#endif

	return FALSE; 
}
