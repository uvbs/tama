#include "stdafx.h"
#include "XEServerDelegate.h"
#include "XServerMain.h"
#include "XFramework/MFC/XEServerApp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEServerMain* XEServerDelegate::s_pMain = NULL;

void XEServerDelegate::Destroy()
{
	XTRACE( "destroy MAIN" );
	if( s_pMain ) {
		s_pMain->OnDestroy();
		SAFE_DELETE( s_pMain );
	}
	XTRACE( "destroy finish MAIN" );
}


void XEServerDelegate::Create(_tstring strINI/* =_T */ )
{
	if( strINI.empty() )		// ini를 지정하지 않았다면
	{
		if( XE::x_strCmdLine.empty() )		// 명령행 인수를 확인해서 이것도 없으면
		{
			TCHAR szFull[MAX_PATH];
			GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
			CString str = XE::GetFileTitle( szFull );
			str += _T(".ini");
			strINI = (LPCTSTR)str;			// 실행파일명 + .ini 로 쓰게 한다.
		} else
			strINI = XE::x_strCmdLine;
	}
	XBREAK( s_pMain != NULL );
	s_pMain = OnCreateMain( strINI );
	XBREAK( s_pMain == NULL );
}
