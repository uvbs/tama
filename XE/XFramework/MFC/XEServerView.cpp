#include "stdafx.h"
#include "XEServerView.h"
#include "XFramework/server/XNetworkConnection.h"
#include "XEServerApp.h"
#include "XFramework/server/XServerMain.h"
#include "XFramework/server/XWinConnection.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

CView* XEServerView::s_pView = NULL;
//XEServerView* XEServerView::s_pServerView = NULL;
void XEServerView::Destroy()
{
	s_pView = nullptr;
}
/*
void XEServerView::OnInitialUpdate( _tstring strINI )
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
	// MFC코드 부분을 코드 템플릿으로 만들어 낼수 있다면 이부분은 MFC부분으로 내려보내야 한다. 여긴 엔진딴임.
	XBREAK( MAIN != NULL );
	MAIN->Create( strINI.c_str() )sdf

*/
void XEServerView::OnDraw( int nTextID )
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CWnd *pWnd = s_pView->GetDlgItem( nTextID );
	if( pWnd && XEServerMain::sGet() )
	{
		_tstring str = XE::Format(_T("fps::%d\n"), XEServerMain::sGet()->m_FPS );
		XARRAYLINEAR_LOOP( XEServerMain::sGet()->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
		{
			str += XE::Format( _T("[%s] 접속자 수: %d/%d 최대큐:%d 생성된커넥션수:%d\n"), 
									pSocketSvr->GetszName(),
									pSocketSvr->GetnumConnected(), pSocketSvr->GetnumLogined(),
									pSocketSvr->GetsizeMaxQ(),
//									XENetworkConnection::sGetmaxQueueRatio(),
									XEWinConnectionInServer::sGetNumCreateObj());
			pSocketSvr->ClearSizeMaxQ();
		} END_LOOP;
/*
		LPCTSTR szText = XE::Format( _T("fps:%d 접속자 수: %d 최대큐:%.1f%%"), 
									MAIN->m_FPS, 
									m_numConnect, 
									XNetworkConnection::sGetmaxQueueRatio());
*/
		// 서브클래스에 str을 바꿀기회를 줌
		OnDrawBefore( str );
		// 
		pWnd->SetWindowText( str.c_str() );
	}
}

