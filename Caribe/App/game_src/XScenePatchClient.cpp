#include "StdAfx.h"
#include "XScenePatchClient.h"
#include "XLoginInfo.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "Network/XDownloadTask.h"
#include "XPatch.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndProgressBar.h"
#include "XSystem.h"
#include "XResMng.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif // _CHEAT


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/*
코어파일이 변한경우( 파일수가 증가한경우 )
	.클라는 그 파일을 안가지고 있을 수 있다.
		1.패키징을 다시해서 apk를 새로 받는다.
		2.실시간으로 없는 파일은 요청해서 받는다.( 이방법이 실수를 없앨수 있다 )
*/

// 일단 코어 패치만 받는걸로 ...

#define FILE_LAST_UPDATE		"LastUpdate.txt"
#define FILE_PACKET_VER		"VerPacket.h"
#define FILE_DL_XTF		"dl.xtf"
/**
모두 Work폴더에 받는걸로 하고 캐쉬 폴더의 개념을 따로 만들자.
Work: iOS의 Document폴더의 개념 지워지지 않는 폴더
Cache: 지워지는 폴더. 임시폴더.
*/
/*
	배경한장에  현재받은파일수/전체받을파일수 를 나타내는 프로그레스바.(노란바탕에 검은배경)
	그리고 %표시. 간략한 안내문(업데이트된 파일을 갱신중입니다. 잠시만 기다려 주세요"
	디버그치트모드에서는 현재 받고 있는 파일명과 전체 받을 파일리스트도 표시
*/
void XScenePatchClient::Destroy() 
{
	// 원래대로 복구시킨다.
	XSYSTEM::RestoreAutoSleepMode();
}

XScenePatchClient::XScenePatchClient( void ) 
	: XSceneBase( GAME, XGAME::xSC_PATCH )
{ 
	Init(); 
}

/**
 @brief OnCreate
*/
void XScenePatchClient::Create( void )
{
	XTRACE("111:%s", __TFUNC__);
	CONSOLE_TAG("patch", "%s", __TFUNC__);
	m_verLastUpdateInApk = GetUpdateVerFromApk( _T(FILE_LAST_UPDATE) );
	// 내 로컬에 있는 업데이트 버전(LastUpdate.txt)을 읽는다.
	m_LocalLastUpdate = GetUpdateVerFromWork( _T(FILE_LAST_UPDATE) );
	// 다운로드 태스크 객체 생성하고 패치서버에 접속
	XBREAK( XE::IsEmpty(CONNECT_INI.m_cIPPatch) );
	//
	XDownloadTask *pTask = new XDownloadTask;
	if( pTask->Create( CONNECT_INI.m_cIPPatch, XPATCH_URL ) == FALSE ) {
		// 태스크 생성에 실패 했다면 보통 폰 네트워크를 쓸수없거나 서버에 접속할수 없는 상태
		XWND_ALERT( "alert.network.fail", "%s", "could not connect to the network. Please check the status of the network." );
		ChangeState( xST_ERROR_COULD_NOT_CONNECT );
	} else {
		pTask->SetEvent( XWM_ERROR_DOWNLOAD, this, &XScenePatchClient::OnErrorDownload );
		// 다운받는 동안은 자동으로 슬립모드가 되지 않게 한다.
		XSYSTEM::SetAutoSleepMode(OFF);
	}
	Add( pTask );
	m_pTask = pTask;
	// 서버들 프로토콜 버전파일 전송을 요청해서 도착하면 szDstFullPath에 저장한다.
//	ChangeState( xST_REQ_VER_PACKET );
	XLOGXN( "core version=%d", m_LocalLastUpdate );
	if( m_LocalLastUpdate < 0 || (DWORD)m_LocalLastUpdate == TOKEN_ERROR )
	{
		XBREAKF( 1, "core update ver file damaged." );
		// 파일은 있는데 이상한 값이 읽힌 경우. 파일이 깨졋다고 보고 lastUpdate를 지워주는게 나을듯.
// 		XLOGXN("아마도 core에 LastVersion이 없다면 MakePackage가 잘못된것. 패치툴 실행후 패키징 할것");
		// 내 로컬업데이트 버전이 0이면 풀버전 다시 받을것.
		// 정상이라면 패키지에서 업데이트 퍄일을 카피하기때문에 0일될수가 없지만
		// 파일 손상으로 잘못된 값을 읽을수 있다.
		// 파일이 손상되었으면 지우고 풀버전을 다시 받도록 한다.
		std::string strPath = XE::GetPathWorkA();
		strPath += FILE_LAST_UPDATE;
		XSYSTEM::RemoveFile( strPath.c_str() );
		m_LocalLastUpdate = 0;
#pragma message("check this")
	}
	// VerPacket과 LastUpdate를 받는다.
	ChangeState( xST_REQ_VER );
	// 최초 다운로드
	if( m_LocalLastUpdate == 0 )
	{
	}
	m_timerCreateUI.Set(2.f);
	m_secPatchStart = XTimer2::sGetTime();
	XDownloaderCurl::sClearReadTotal();
}

// 
/**
 @brief	업데이트 버전 파일을 읽는다.
*/
int XScenePatchClient::GetUpdateVerFromWork( LPCTSTR szRes )
{
	CToken token;
	// 워크에서 읽는다.
	if( token.LoadFromWork( szRes, XE::TXT_EUCKR ) == xFAIL )	{
		XLOGXN("GetUpdateVer:LoadFromWork failed:%s", szRes );
		return 0;
	}
	token.GetToken();		// Ver
	int ver = token.GetNumber();
  XLOGXN( "GetUpdateVer:ver=%d", ver );
	if( token.IsError() )	{// 안드로이드에선 IsFail()로 검사하면 걸림. EOF에 걸림. 왜죠?
		XLOGXN( "GetUpdateVer:token.IsFail:%s", token.m_Token );
		ver = 0;
	}
	return ver;
}

int XScenePatchClient::GetUpdateVerFromApk( LPCTSTR szRes )
{
	CToken token;
	// 워크에서 읽는다.
	if( token.LoadFromPackage( szRes, XE::TXT_EUCKR ) == xFAIL ) {
		XWND_ALERT( "%s", _T("not found lastUpdate in Package") );
		return 0;
	}
	token.GetToken();		// Ver
	int ver = token.GetNumber();
	XLOGXN( "%s:ver=%d", __TFUNC__, ver );
	if( token.IsError() ) {// 안드로이드에선 IsFail()로 검사하면 걸림. EOF에 걸림. 왜죠?
		XLOGXN( "%s: token failed: token=%s", __TFUNC__, token.m_Token );
		ver = 0;
	}
	return ver;
}

int XScenePatchClient::ChangeState( xtState state )
{
	XBREAK( m_State == state );
	DoFSMState( m_State, xACT_LEAVE );
	//
	XLOGXN("MWScenePatchClient: changeState %d -> %d", (int)m_State, (int)state );
	m_StateOld = m_State;
	m_State = state;
	//
	DoFSMState( state, xACT_ENTER );
	return 1;
}


int XScenePatchClient::DoFSMState( xtState state, xtAct event, std::string *pFilename )
{
	int ret = 0;
	switch( state )
	{
	case xST_NONE:
		break;
	case xST_REQ_VER:
		ret = FSMReqVersion( event );
		break;
	case xST_REQ_FULL_LIST:
		ret = FSMReqFullList( event );
		break;
	case xST_AFTER_DOWNLOAD:
		ret = FSMAfterDownload( event );
		break;
	case xST_BRANCH_UPDATE_LIST:
		ret = FSMBranchUpdateList( event );
		break;
	case xST_REQ_UPDATE_LIST:
		ret = FSMReqUpdateList( event );
		break;
	case xST_REQ_RECV_RES:
		ret = FSMReqRecvRes( event );
		break;
	case xST_EXIT_SCENE:
		ret = FSMExitScene( event );
		break;
	case xST_ERROR:
	case xST_ERROR_DOWNLOAD:
	case xST_ERROR_UPDATE_LIST:
		ret = FSMError( event );
		break;
	default:
		XBREAKF( 1, "알수없는 패치클라 상태:%d", state );
		break;
	}
	return ret;
}

/**
 @brief 배경이미지와 텍스트등 필요한 UI객체를 생성한다.
*/
void XScenePatchClient::CreateUI()
{
	m_bDrawProgress = true;
	if( GAME->GetbFirst() ) {
		// 최초 앱 실행시엔 오프닝을 함께 보여줌
		if( Find("spr.opening") == nullptr ) {
			auto pWnd = new XWndSprObj( _T("opening.spr"), 1, XE::GetGameSize() * 0.5f + XE::VEC2(0,-20) );
			pWnd->SetstrIdentifier( "spr.opening" );
			pWnd->SetpDelegateBySprObj( GAME );
			pWnd->SetScaleLocal( 1.5f );
			Add( pWnd );
		}
	} else {
		// 배경 이미지 로딩
		if( Find( "bg.patch" ) == nullptr )	{
			XWnd *pWnd = new XWndImage( PATH_UI( "bg_patch.png" ), XE::xPF_ARGB8888, XE::VEC2(0) );
			XBREAKF( pWnd == nullptr, "patchClient: bg_patch.png not found" );
			if( pWnd )	{
				pWnd->SetstrIdentifier( "bg.patch" );
				Add( pWnd );
			}
		}
	}
	if( Find( "text.patch" ) == nullptr )	{
		auto pText = new XWndTextString( 0, 320, _T( "" ) );
		pText->SetStyleStroke();
		if( pText )	{
#ifdef _CHEAT
			if( XAPP->m_bDebugMode ) {
				const _tstring str = C2SZ( CONNECT_INI.m_cIPPatch );
				pText->SetText( XFORMAT("connecting to server......%s", str.c_str() ) );
			} else 
#endif // _CHEAT
			{
				pText->SetText( _T( "connecting to server......" ) );
			}
			pText->SetstrIdentifier( "text.patch" );
			Add( pText );
		}
		m_pWndText = pText;
	}
}

/**
 @brief VerPacket.h과 LastUpdate.txt파일을 받는다.
*/
int XScenePatchClient::FSMReqVersion( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: { 
		m_listWillDownload.clear();
		{
		// VerPacket파일 다운로드를 요청한다.
			_tstring strDstFile = _T(FILE_PACKET_VER);
			m_pTask->AddRequest( _T( XFULL_DIR ), strDstFile, 
												strDstFile + _T("_") );
		} {
		// LastUpdate파일 다운로드를 요청함.
			_tstring strFile /*= _T( XCORE_DIR )*/;
			strFile = _T( FILE_LAST_UPDATE );
			m_pTask->AddRequest( _T(XCORE_DIR), strFile.c_str(), 
										strFile + _T("_") );
		}
		// 완료되면 case xACT_RECV가 실행된다.
		m_pTask->SetEvent( XWM_ALL_COMPLETE, this,
									&XScenePatchClient::OnRecvVersions, (DWORD)m_State );
		m_pTask->Go();
	}	break;
	case xACT_PROCESS:
		break;
	//////////////////////////////////////////////////////////////////////////
	// 요청한 파일 다운로드 완료
	case xACT_RECV: {	{
			std::string strOld = SZ2C( XE::GetPathWork() );
			std::string strNew = strOld;
			strOld += "VerPacket.h_";
			strNew += "VerPacket.h";
			// 현재는 사용하지 않으나 버전체크등을 위해 남겨둔듯..
			XSYSTEM::RemoveFile( strNew.c_str() );
			XSYSTEM::RenameFile( strOld.c_str(), strNew.c_str() );
		} {
		// 다운받은 Lastupdate.txt_ 파일을 읽는다.
			_tstring strVer = _T(FILE_LAST_UPDATE);
			int verServer = GetUpdateVerFromWork( strVer + _T("_") );
			if( XBREAKF( verServer == 0, "verServer == 0, %s", strVer.c_str() ) ) {
				// 서버의 버전이 0이란 소린데 다운로드가 잘못된거임.
				ChangeState( xST_ERROR );
				return 1;
			}
			m_LastUpdateOnServer = verServer;
		}
		// 이미 다운완료된 파일의 목록을 꺼낸다.
		LoadDownloadedList( m_LastUpdateOnServer );
		// 최초 실행이냐
		if( IsFirstPlay() ) {
			// full_list요청
			XBREAK(1);		// 이제 apk에 LastUpdate파일은 반드시 들어가야하므로 여기로 들어오는일은 없어야 함.
			ChangeState( xST_REQ_FULL_LIST );
			// 다운받아야 하므로 다운 ui띄움.(배경만 fade in해서 짧은 다운인경우 다시 사라지도록)
			CreateUI();
			m_bPatched = true;
		} else {
			// 최초실행이 아님
			// 업데이트 버전 비교 상태로 전환
			ChangeState( xST_BRANCH_UPDATE_LIST );
		}

	}	break;
// 	case xACT_ON_YES:
// 		// full_list요청
// 		ChangeState( xST_REQ_FULL_LIST );
// 		// 다운받아야 하므로 다운 ui띄움.(배경만 fade in해서 짧은 다운인경우 다시 사라지도록)
// 		CreateUI();
// 		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
} // FSMReqVersion

// CDMA에서 다운로드를 받겠다 Yes누름.
// int XScenePatchClient::OnYesCDMADownload( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	DoFSMState( xST_REQ_VER, xACT_ON_YES );	
// 	return 1;
// }

int XScenePatchClient::OnYesCDMADownloadInRecvRes( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	DoFSMState( xST_REQ_RECV_RES, xACT_ON_YES );
	return 1;
}

int XScenePatchClient::FSMBranchUpdateList( xtAct event )
{
	switch( event ) {
	case xACT_ENTER: {
		// 버전 비교
		XLOGXN( "core version compare: my.ver=%d, ser.Ver=%d, apk.ver=%d", m_LocalLastUpdate, m_LastUpdateOnServer, m_verLastUpdateInApk );
		XBREAK( m_verLastUpdateInApk <= 0 );
		XBREAK( m_LastUpdateOnServer <= 0 );
		XBREAK( m_LocalLastUpdate == 0 );
		XBREAKF( m_LocalLastUpdate > m_LastUpdateOnServer
																								, "m_LocalLastUpdate(%d) > m_LastUpdateOnServer(%d)"
																									, m_LocalLastUpdate, m_LastUpdateOnServer );
		do {
			// 업데이트 목록파일(updateXX.txt) 요청
			if( m_LocalLastUpdate < m_verLastUpdateInApk ) {
				// work버전이 apk버전보다 낮음. apk로부터 패치카피(apk버전 우선 체크)
				m_modePatchFrom = xPF_FROM_APK;
			} else
			if( m_LocalLastUpdate < m_LastUpdateOnServer ) {
				// work버전이 패치서버 버전보다 낮음.
				m_modePatchFrom = xPF_FROM_SERVER;
			} else {
				// 업데이트를 받을 필요가 없다.
				// 씬 빠져나가기.
				ChangeState( xST_EXIT_SCENE );
				break;
			}
			ChangeState( xST_REQ_UPDATE_LIST );
			CreateUI();
			m_bPatched = true;
		} while(0);
	} break;
	case xACT_PROCESS: {

	} break;
	case xACT_LEAVE:	
		break;
	default:
		XBREAK(1);
		break;
	}
	return 1;
}

/**
 @brief full_list.txt를 받는다.
 최초 패치상황
*/
int XScenePatchClient::FSMReqFullList( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: {
		// 서버에 full_list.txt를 요청함. 다운이 끝나면 case xACT_RECV가 실행됨.
		m_pTask->AddRequest( _T( XFULL_DIR ), _T(FILE_FULL_LIST) );
		m_pTask->SetEvent( XWM_ALL_COMPLETE, this, 
										&XScenePatchClient::OnRecvFullList, 
										(DWORD)m_State );
		m_pTask->Go();
	} break;
	case xACT_PROCESS:
		break;
	case xACT_RECV: {
		// full_list.txt에서 파일목록을 추려낸다.
		_tstring strFullpath = XE::MakeWorkFullPath(_T(""), _T("full_list.txt"));
		XList4<XPatch::XRES_INFO> listFulls;
		XPatch::sLoadFullList( strFullpath.c_str(), &listFulls );
		// 파일목록의 파일을 다운받도록 요청한다.
		m_listWillDownload.clear();
		for( auto& res : listFulls ) {
			std::string str = SZ2C( res.strFile.c_str() );
			XUNIX_PATH( str );
			// 이미 받은 목록에 없어야만 요청한다.
			if( !m_listCompleteFiles.Findp( str ) )
				m_listWillDownload.Add( res.strFile );
		}
		m_pTask->SetEvent( XWM_ALL_COMPLETE, this,
											&XScenePatchClient::OnRecvRes, 
											(DWORD)xDL_FULL );
//		m_pTask->Go();
		// 요청한 목록 받는 상태로 전환
		ChangeState( xST_REQ_RECV_RES );
	} break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

/**
 @brief 파일들 다운로드가 끝나면 패치의 마무리를 한다.
*/
int XScenePatchClient::FSMAfterDownload( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: {
		XLOGXN( "copy _LastUpdate -> LastUpdate" );
		std::string cstrWorkPath = XE::GetPathWorkA();
		// LastUpdate.txt 풀패스 생성
		std::string cstrUpdateVer = cstrWorkPath;
		cstrUpdateVer += FILE_LAST_UPDATE;
		// LastUpdate.txt_  풀패스 생성
		std::string cstrTempUpdateVer = cstrUpdateVer + "_";
		// 기존의 LastUpdate.txt를 삭제하고 LastUpdate.txt_ 의 이름을 바꾼다.
		XSYSTEM::DeleteFile( cstrUpdateVer.c_str() );
		XSYSTEM::RenameFile( cstrTempUpdateVer.c_str(), cstrUpdateVer.c_str() );
		ChangeState( xST_EXIT_SCENE );
	} break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

/**
 @brief 검색용 맵 작성
*/
void XScenePatchClient::LoadApkList( std::map<_tstring, int>* pMapOut )
{
	CToken token;
	// 워크에서 읽는다.
	if( token.LoadFromPackage( _T("apk_list.txt"), XE::TXT_EUCKR ) == xFAIL ) {
		XLOGXN( "apk_list loading failed" );
		return;
	}
	while(1) {
		token.GetToken();
		if( token.IsEof() )
			break;
		(*pMapOut)[ token.m_Token ] = 1;
	}
}

/**
 @brief apk_list에 있는 파일인가.
*/
bool XScenePatchClient::IsApkFile( const _tstring& strRes, std::map<_tstring, int>& mapApk )
{
	if( mapApk.empty() )
		return false;
	auto itor = mapApk.find( strRes );
	return ( itor != mapApk.end() );
}

/**
 @brief updateXX.txt 파일들 요청
*/
int XScenePatchClient::FSMReqUpdateList( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: {
		// 받아야할 Update00.txt 업데이트 목록파일명 생성
		// 업데이트 리스트 요청. 내버전에서부터 최신버전목록파일 까지 모두 받아온다.
		_tstring strDstLocal = XE::MakePath( _T(XUPDATE_DIR), _T("") );
		int verStart = 0;
		int verEnd = 0;
		XBREAK( m_LocalLastUpdate == 0 );
		XBREAK( m_modePatchFrom == xPF_NONE );
		if( m_modePatchFrom == xPF_FROM_APK ) {
			verStart = m_LocalLastUpdate + 1;
			verEnd = m_verLastUpdateInApk;
		} else
		if( m_modePatchFrom == xPF_FROM_SERVER ) {
			XBREAK( m_LastUpdateOnServer == 0 );
			XBREAK( m_LocalLastUpdate > m_LastUpdateOnServer );
			verStart = m_LocalLastUpdate + 1;
			verEnd = m_LastUpdateOnServer;
		}
		for( int i = verStart; i <= verEnd; ++i ) {
			_tstring strURL = _T(XCORE_DIR);
			strURL += XE::Format( _T("Update%d.txt"), i );
			m_pTask->AddRequest( _T(""), strURL, strDstLocal );
		} 
		XBREAKF( m_pTask->GetNumRequest() == 0, 
																			"updateXX.txt num request 0. myCorVer=%d, serverVer=%d", 
																					m_LocalLastUpdate, m_LastUpdateOnServer );
		// 다운완료되면 xACT_RECV를 실행한다.
		m_pTask->SetEvent( XWM_ALL_COMPLETE, this, 
									&XScenePatchClient::OnRecvUpdateList, (DWORD)m_State );
		m_pTask->Go();
	} break;
	case xACT_PROCESS:
		break;
	// UpdateXX.txt 목록 파일들 다받음.
	case xACT_RECV: {
		// 다운받은 파일목록을 어레이에 옮겨담는다.
		XList4<_tstring> listUpdateList;
		if( XBREAK( m_pTask->GetNumComplete() > 100 ) ) {	// apk의 변동없이 오랫동안 안들어오면 이런경우가 생길수 있다 이경우 앱스토어로 이동
			//앱스토어로 이동
			break;
		}
		for( auto& info : m_pTask->GetlistComplete() ) {
			_tstring tstrURL = C2SZ( info.strURL.c_str() );
			listUpdateList.Add( tstrURL );
		}
		// 업데이트 리스트의 목록들을 합치고 파일명들 중에 중복된건 걸러낸다.
		XList4<_tstring> listUpdate;
		DoMergeUpdateList( listUpdateList, &listUpdate );
		//
		if( listUpdate.size() == 0 )		// 이거 필요한가.
			ChangeState( xST_EXIT_SCENE );
		// UpdateXXX파일들 지움
		for( auto& info : m_pTask->GetlistComplete() ) {
			std::string strFullpath = XE::GetPathWorkA();
			strFullpath += XUPDATE_DIR;
			strFullpath += info.strURL;
#ifdef WIN32
			std::replace_if( strFullpath.begin(), strFullpath.end(), 
				[](char c)->bool{
					return ( c == '/' );
			}, '\\');
#endif // WIN32
			XSYSTEM::DeleteFile( strFullpath.c_str() );
		}
		// 추려낸 실제 리소스 파일들을 요청한다.
		for( auto& strRes : listUpdate ) {
			std::string str = SZ2C( strRes.c_str() );
			XUNIX_PATH( str );
			// 이미 받은 목록에 없어야만 요청한다.
			if( !m_listCompleteFiles.Findp( str ) )
				m_listWillDownload.Add( strRes );
//				m_pTask->AddRequest( _T(XFULL_DIR), strRes );
		}
		m_pTask->SetEvent( XWM_ALL_COMPLETE, this, 
							&XScenePatchClient::OnRecvRes, (DWORD)xDL_UPDATE );
		// 필요한 리소스 파일들을 받는 상태로 넘어간다.
		ChangeState( xST_REQ_RECV_RES );
//		m_pTask->Go();
		return 1;
	} break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

/**
 @brief 업데이트가 필요한 리소스 파일들을 다운받는다.
*/
int XScenePatchClient::FSMReqRecvRes( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: {
		XBREAK( m_modePatchFrom == xPF_NONE );
		if( m_modePatchFrom == xPF_FROM_SERVER ) {
			if( !XSYSTEM::CheckWiFi() ) {
				// 와이파이 아닌데 다운받을래?
				auto pAlert = XWND_ALERT_YESNO( "alert.cdma.check", "%s", _T( "Download the new file. If the network is no WiFi, you may excessive fee occurs." ) );
				if( pAlert ) {
					pAlert->SetEvent( XWM_YES, this, &XScenePatchClient::OnYesCDMADownloadInRecvRes );
					pAlert->SetEvent( XWM_NO, GAME, &XGame::OnExitApp );
				}
			} else {
				FSMReqRecvRes( xACT_ON_YES );
			}
		} else
		if( m_modePatchFrom == xPF_FROM_APK ) {

		}
	} break;
	case xACT_ON_YES: {
		// 다운받아야할 목록이 취합되면 여기서 한꺼번에 요청하고 시작한다.
		XBREAK( m_listWillDownload.size() == 0 );
		for( auto& strRes : m_listWillDownload ) {
			m_pTask->AddRequest( _T( XFULL_DIR ), strRes );
		}
		m_pTask->SetEvent( XWM_EACH_COMPLETE, this,
			&XScenePatchClient::OnRecvEachRes, (DWORD)m_State );
		m_pTask->Go();		// 다운로드 시작
	} break;
	case xACT_PROCESS: {
		if( m_modePatchFrom == xPF_FROM_SERVER ) {
			if( m_pTask->GetbGo() ) {
				// 다운받는 상황 보여주기
				XDownloadTask::xRECV_INFO info;
				info = m_pTask->GetCurrDownloadInfo();
				if( m_pWndText ) {
					_tstring strBuff;
					_tstring strFilename = XE::GetFileName( info.strURL );
#ifdef _CHEAT
					if( XAPP->m_bDebugMode ) {
						const xSec secPass = XTimer2::sGetTime() - m_secPatchStart;
						const auto readPerSec = XDownloaderCurl::s_readTotal / ((secPass)? secPass : 1);
						strBuff = XFORMAT( " update %d/%d(%dkb/s):%s(total:%smb avg %dkb/s)", 
											m_pTask->GetidxCurrDownload(),
											m_pTask->GetNumRequest(),
											XDownloaderCurl::s_FpsDown.GetFps() / 1024,
											XSYSTEM::GetstrTimeHMS( secPass ).c_str(),
											XE::NtS( XDownloaderCurl::s_readTotal / 1024 / 1024 ),
											readPerSec / 1024 );
					} else 
#endif // _CHEAT
					{
						strBuff = XFORMAT( " update from svr%.1f%%", 
											((float)m_pTask->GetidxCurrDownload() / m_pTask->GetNumRequest()) * 100.f );
					}
// 					XTRACE( "%s", strBuff.c_str() );
					m_pWndText->SetText( strBuff );
				}
			}
		} else
		if( m_modePatchFrom == xPF_FROM_APK ) {
			int idx = 0;
			for( auto& strRes : m_listWillDownload ) {
				// 업데이트 파일을 apk로부터 카피한다.
				XE::CopyPackageToWork( strRes.c_str() );
#ifdef _CHEAT
				if( XAPP->m_bDebugMode && m_pWndText ) {
					const _tstring strBuff = XFORMAT( " copy from apk %d/%d", idx, m_listWillDownload.size() );
					XTRACE("%s", strBuff.c_str());
					m_pWndText->SetText( strBuff );
				}
#endif // _CHEAT
				++idx;
			} // for
			// apk에 있는걸로 work에 카피
			XE::CopyPackageToWork( _T(FILE_LAST_UPDATE) );
			m_LocalLastUpdate = GetUpdateVerFromWork( _T(FILE_LAST_UPDATE) );
			XBREAK( m_LocalLastUpdate == 0 );
			ChangeState( xST_BRANCH_UPDATE_LIST );		// 다시 첨으로 돌아가서 서버패치버전을 검사한다.
#ifdef _CHEAT
			if( XAPP->m_bDebugMode && m_pWndText ) {
				m_pWndText->SetText( _T( "copy complete" ) );
			}
#endif // _CHEAT
		} // from apk copy
	} break;
		// 파일들을 받을때 한 파일의 전송이 끝나면 호출된다.
	case xACT_RECV_EACH: {
		int lenWorkPath = strlen(XE::GetPathWorkA());
		std::string strResOnly = m_strDownloadedFile.substr( lenWorkPath );
		XUNIX_PATH( strResOnly );	// /기반 패스명으로 바꾼다.
		m_listCompleteFiles.Add( strResOnly );	// 리소스패스만
		// 다운로드된 서브파일 목록을 파일에 바로바로 쓴다.
		SaveDownloadedList();
	} break;
	case xACT_RECV: {
		m_pTask->ClearEvent( XWM_EACH_COMPLETE );
	} break;
	case xACT_LEAVE:
		break;
	}
	return 1;
} // FSMReqRecvRes

int XScenePatchClient::FSMExitScene( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		if( XEBaseScene::IsExit() == FALSE )		// <<-이건 뭐징
		{
			// 다운이 완료되었으면 dl.xtf를 지운다. 
			// 혹시 예외상황으로 dl.xtf가 안지워졌을수도 있으므로 이곳에서 지우게 함.
			std::string strFile = XE::GetPathWorkA();
			strFile += FILE_DL_XTF;
			XSYSTEM::RemoveFile( strFile );
			XLOGXNA("%s: Finish patch update(core&sub)", __FUNCTION__);
			m_secTotalPatch = XTimer2::sGetTime() - m_secPatchStart;
			if( !XGame::s_bLoaded ) {
				// 최초 실행
				if( m_bPatched ) {
				} else {
				}
				GAME->CreateGameResource();	// 프로퍼티등을 로딩
			} else {
				// resume
				if( m_bPatched ) {
					// 뭔가 받은게 있으면 리소스 삭제하고 다시 로딩
					GAME->DestroyGameResource();		
					GAME->CreateGameResource();	// 프로퍼티등을 재 로딩
				} else {
					// 재로딩 하지 않음.
				}
			}
			if( m_bPatched ) {
				auto pWndButt = new XWndButton( 0, 0, XE::GetGameWidth(), XE::GetGameHeight() );
				pWndButt->SetEvent( XWM_CLICKED, this, &XScenePatchClient::OnTouch );
				Add( pWndButt );
// 				const auto vImg = ( XE::GetGameSize() * 0.5f ) + XE::VEC2( 0, 150 );
// 				auto pWndTouch = new XWndImage( PATH_UI( "ui_say.png" ), vImg );
// 				Add( pWndTouch );
// 				pWndTouch->AutoLayoutHCenter();
// 				auto pText = new XWndTextString( XTEXT( 2284 ), FONT_NANUM, 20.f, XCOLOR_BLACK );
// 				pText->SetSizeLocal( pWndTouch->GetSizeLocal() );
// 				pWndTouch->Add( pText );
// 				pText->AutoLayoutCenter();
// 				pWndTouch->GetcompMngByAlpha().AddComponentWave( "alpha" );
			} else {
				DoExit( XGAME::xSC_TITLE );
			}
			m_bPatched = false;
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

/**
 @brief 
*/
int XScenePatchClient::OnTouch( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnTouch");
	//
	DoExit( XGAME::xSC_TITLE );
	return 1;
}


int XScenePatchClient::FSMError( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER: {
		// 에러 사안별로 좀더 꼼꼼하게 에러메시지 표시할것.
#pragma message("check this")
			auto pAlert = XWND_ALERT( "alert.patch.error", "%s", XTEXT(10) );	// 문제발생. 앱다시 실행
			if( pAlert )
				pAlert->SetEvent( XWM_OK, GAME, &XGame::OnExitApp );
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int XScenePatchClient::Process( float dt ) 
{ 
	if( !GAME->Find("alert.cdma.check") ) {
		if( !m_bDrawProgress && m_timerCreateUI.IsOver() ) {
			// 아직 UI가 안만들어진상태에서 시간이 지나도 응답이 없으면 자동으로 로딩화면 띄움.
			CreateUI();
			m_timerCreateUI.Off();
		}
	}
	//
	DoFSMState( m_State, xACT_PROCESS );
	//
	return XEBaseScene::Process( dt );
}

//
void XScenePatchClient::Draw( void ) 
{
	XEBaseScene::Draw();
	if( m_bDrawProgress )
	{
		const XE::VEC2 vPos( 20, 330 );
		const XE::VEC2 vSize( 578, 10 );
		if( m_pTask->GetbGo() )
		{
			XDownloadTask::xRECV_INFO info;
			info = m_pTask->GetCurrDownloadInfo();
			XUTIL::DrawProgressBar( vPos, 
									(float)m_pTask->GetidxCurrDownload(), 
									(float)m_pTask->GetNumRequest(), 
									vSize );
		} else
		if( m_pTask->GetbComplete() )
		{
			XUTIL::DrawProgressBar( vPos, 1.0f, vSize );
		}
	}
	XEBaseScene::DrawTransition();
}

void XScenePatchClient::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XScenePatchClient::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XScenePatchClient::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

// 패킷버전 파일 받음.
int XScenePatchClient::OnRecvVersions( XWnd *pWnd, DWORD p1, DWORD )
{
	DoFSMState( (xtState)p1, xACT_RECV );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XScenePatchClient::OnRecvFullList( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnRecvFullList");
	//
	DoFSMState( (xtState)p1, xACT_RECV );
	
	return 1;
}


// 코어 업데이트 버전 파일 받음.
// int XScenePatchClient::OnRecvUpdateVer( XWnd *pWnd, DWORD, DWORD )
// {
// 	DoFSMState( xST_REQ_LAST_UPDATE, xACT_RECV );
// 	return 1;
// }

void XScenePatchClient::OnCompleteRecvFile( const char *cURL, const char *cFullpath, int size )
{
}

// UpdateXX.txt 업데이트 목록 파일들 다받음.
int XScenePatchClient::OnRecvUpdateList( XWnd *pWnd, DWORD p1, DWORD )
{
	DoFSMState( (xtState)p1, xACT_RECV );
	return 1;
}

/**
@brief 업데이트 리스트의 목록들을 합치고 파일명들 중에 중복된건 걸러낸다.
*/
int XScenePatchClient::DoMergeUpdateList( XList4<_tstring>& listUpdateList,
											XList4<_tstring> *pOut )
{
	int _size = sizeof(_tstring);
	int max = 0;
	int duplicate = 0;
// 	std::map<_tstring, int> mapApkList;
// 	if( listUpdateList.size() > 0 ) {
// 		if( listUpdateList.GetFirst() == _T("Update1.txt") ) {
// 			LoadApkList( &mapApkList );
// 		}
// 	}
	// updateXX.txt 파일들.
	for( auto& _strFile : listUpdateList )
	{
		CToken token;
		TCHAR szFile[ 256 ];
		_tcscpy_s( szFile, _T(XUPDATE_DIR) );
		_tcscat_s( szFile, _strFile.c_str() );
		XPLATFORM_PATH( szFile );
//		_tstring strFile = C2SZ( cFile );
		if( token.LoadFromWork( szFile, XE::TXT_EUCKR ) == xFAIL )
		{
			ChangeState( xST_ERROR_UPDATE_LIST );
			XBREAKF( 1, "file open error. [ %s ]", szFile );
			// 이건 중간 업데이트 파일을 못받더라도 뒷버전 목록으로 해결될수도 있으니 일단 계속 진행하고 경고만 띄워준다.
		} else	{
			int i = 0;
			XTRACE( "\r\n%s", szFile );
			while( token.GetToken() )	{
				// UpdateXX.txt파일에 있는 파일목록을 읽어서 목록에 넣는다.
				// 이미 목록에 있는 파일명이면 넣지 않는다.
				_tstring strFile = _tstring( token.m_Token );
				if( pOut->Findp( strFile ) == nullptr )
					pOut->Add( strFile ); 
				else
					++duplicate;
				XTRACE( "%d:%s", i++, token.m_Token );
			}
		}
	}
	XLOGXN( "max update list: %d", pOut->size() );
	XLOGXN( "duplicate files number: %d", duplicate );
	return pOut->size();
}

/**
 @brief 파일목록을 다운받음(다용도)
*/
int XScenePatchClient::OnRecvRes( XWnd *pWnd, DWORD p1, DWORD )
{
	XLOGXN( "OnRecvRes finished" );
	auto typeDownload = (xtDownload)p1;
	if( m_pWndText )
		m_pWndText->SetText( _T( "Update complete" ) );
	XBREAK( typeDownload == xDL_NONE );
	DoFSMState( xST_REQ_RECV_RES, xACT_RECV );
	ChangeState( xST_AFTER_DOWNLOAD );
	if( typeDownload == xDL_FULL )
	{
		XLOGXN( "full patch complete" );
	} else
	if( typeDownload == xDL_UPDATE )
	{
		XLOGXN( "core patch complete" );
	}
	return 1;
}

/****************************************************************
* @brief full리소스를 모두 다 받음.
*****************************************************************/
// int XScenePatchClient::OnRecvFullRes( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnRecvFullRes");
// 	//
// 	DoFSMState( xST_REQ_FULL_DOWNLOAD, xACT_RECV );
// 	
// 	return 1;
// }


// 앱 최초 실행인가.
BOOL XScenePatchClient::IsFirstPlay( void )
{
// 	std::string strFile = SZ2C( XE::GetPathWork() );
// 	strFile += "patch.ini";
// 	FILE *fp = nullptr;
// 	fopen_s( &fp, strFile.c_str(), "rt" );
// 	if( fp == nullptr )
// 		return TRUE;
// 	int val = 0;
// 	fscanf_s( fp, "%d", &val );
// 	fclose(fp);
// 	return val != 1;	// 1이 안써있으면 깨진걸로 생각하고 처음 실행으로 간주함
	return m_LocalLastUpdate == 0;
}

BOOL XScenePatchClient::SavePatchIni( void )
{
	std::string strFile = SZ2C( XE::GetPathWork() );
	strFile += "patch.ini";
	FILE *fp = nullptr;
	fopen_s( &fp, strFile.c_str(), "wt" );
	if( fp == nullptr )
		return FALSE;
	fprintf_s( fp, "%d", 1 );
	fclose( fp );
	return TRUE;
}

int XScenePatchClient::OnBack( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	XTRACE( "XScenePatchClient::OnBack" );
	auto pAlert = XWND_ALERT_YESNO( "wnd.exit", "%s", _T( "exit?" ) );
	if( pAlert )
		pAlert->SetEvent( XWM_YES, GAME, &XGame::OnExitApp );
	return 1;
}

/**
 @brief 현재까지 다운받은 파일을 파일로 쓴다.
*/
bool XScenePatchClient::LoadDownloadedList( int verServer )
{
	CToken token;
	if( !token.LoadFromWork( _T( FILE_DL_XTF ), XE::TXT_EUCKR ) )
		return false;
	int verInFile = token.GetNumber();
	// dl.xtf의 버전과 지금 버전이 다르면 리스트를 읽지 않고 다시 받는다.
	if( verInFile < verServer )
		return false;
	while( token.GetToken() )	{
		m_listCompleteFiles.Add( std::string( SZ2C( token.m_Token ) ) );
	}
	return TRUE;
}


/**
 @brief 현재까지 다운받은 파일을 파일로 쓴다.
*/
bool XScenePatchClient::SaveDownloadedList( void )
{
	std::string strPath = XE::GetPathWorkA();
	strPath += FILE_DL_XTF;
	FILE *fp = NULL;
	fopen_s( &fp, strPath.c_str(), "wt" );
	if( fp == NULL )
		return false;
	fprintf_s( fp, "%d\n", m_LastUpdateOnServer );	// 현재 목록의 업데이트 버전
	for( auto strFile : m_listCompleteFiles )
	{
		fprintf_s( fp, "\"%s\"\n", strFile.c_str() );
	} 
	fprintf_s( fp, "// num files %d \n", m_listCompleteFiles.size() );
	fclose(fp);
	return TRUE;
}

/****************************************************************
* @brief 
*****************************************************************/
int XScenePatchClient::OnRecvEachRes( XWnd* pWnd, DWORD p1, DWORD p2 )
{
//	CONSOLE("OnRecvEachRes");
	//
	XDownloadTask::xREQ_INFO *pInfo = ( XDownloadTask::xREQ_INFO* ) p2;
	if( pInfo->strToRename.empty() == false )
		m_strDownloadedFile = pInfo->strToRename;
	else
		m_strDownloadedFile = pInfo->strDstFullpath;
	DoFSMState( (xtState)p1, xACT_RECV_EACH );
	return 1;
}

/**
 @brief 
*/
int XScenePatchClient::OnErrorDownload( XWnd* pWnd, DWORD p1, DWORD dwCode )
{
	CONSOLE("OnErrorDownload:code=%d", dwCode );
	//
	auto codeErr = (XDownloader::xtError)dwCode;
	_tstring strMsg;
	switch( codeErr )
	{
	case XDownloader::xOK:
		break;
	case XDownloader::xERR_COULD_NOT_CONNECT:
		strMsg = _T( "could not connect to server" );
		break;
	case XDownloader::xERR_FAILED_REQUEST:
		strMsg = _T( "failed request" );
		break;
	case XDownloader::xERR_RECV_FILE_SIZE_IS_ZERO:
		strMsg = _T( "receive file size is zero" );
		break;
	case XDownloader::xERR_INVALID_REQUEST:
		strMsg = _T( "invalid_request" );
		break;
	case XDownloader::xERR_NO_RESPONSE:
		strMsg = _T( "No response from server" );
		break;
	default:
		strMsg = _T( "unknown error" );
		break;
	}
	if( !strMsg.empty() ) {
		XWND_ALERT( "%s", strMsg.c_str() );
		strMsg = _T("patch:") + strMsg;
		XTRACE("%s", strMsg.c_str() );
	}
	return 1;
}
