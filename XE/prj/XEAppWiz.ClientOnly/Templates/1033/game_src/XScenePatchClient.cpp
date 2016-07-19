#include "StdAfx.h"
#ifdef _XPATCH
#include "MWScenePatchClient.h"
#include "XLoginInfo.h"
#include "XT3.h"
#include "XResMng.h"
#include "XResObj.h"
#include "XT3Main.h"
#include "XT3Wnd.h"
#include "Network/XDownloadTask.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 일단 코어 패치만 받는걸로 ...

#define FILE_UPDATE_VER		"LastUpdate.txt"
#define FILE_PACKET_VER		"VerPacket.h"
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
void MWScenePatchClient::Destroy() 
{
	// 원래대로 복구시킨다.
	XSYSTEM::RestoreAutoSleepMode();
}

MWScenePatchClient::MWScenePatchClient( void ) 
	: XBaseScene( GAME, xSCENE::xSC_CORE_PATCH )
{ 
	Init(); 
}

// 업데이트 버전 파일을 읽는다.
int MWScenePatchClient::GetUpdateVer( LPCTSTR szRes )
{
	CToken token;
	TCHAR szPath[ 256 ];
	_tcscpy_s( szPath, _T(XUPDATE_DIR) );
	_tcscat_s( szPath, _T(XCORE_DIR) );
	_tcscat_s( szPath, szRes );
	XPLATFORM_PATH( szPath );
	// 워크에서 우선 읽어보고 없으면 패키지에서 읽는다.
	if( token.LoadFile( szPath, XE::TXT_EUCKR ) == xFAIL )
		return 0;
	token.GetToken();		// Ver
	int ver = token.GetNumber();
	return ver;
}

int MWScenePatchClient::GetUpdateVerFromPackage( LPCTSTR szFile )
{
	_tstring strRes = _T(XUPDATE_DIR);
	strRes += _T(XCORE_DIR);
	strRes += szFile;
	DWORD size = 0;
	BYTE *pMem = XE::CreateMemFromPackage( strRes.c_str(), &size );
	XBREAKF( pMem == NULL, "pMem == NULL:%s", strRes.c_str() );
	if( pMem == NULL )
		return -1;
	CToken token;
	token.LoadString( C2SZ((const char*)pMem) );
	token.GetToken();	// Ver
	int ver = token.GetNumber();
	SAFE_DELETE_ARRAY( pMem );
	return ver;
}

void MWScenePatchClient::Create( void )
{
	// 내 로컬에 있는 업데이트 버전을 읽는다.
	m_MyUpdateVer = GetUpdateVer( _T(FILE_UPDATE_VER) );
	// 다운로드 태스크 객체 생성하고 패치서버에 접속
	XDownloadTask *pTask = new XDownloadTask;
	if( pTask->Create( CONNECT_INI.m_cIPPatch, XPATCH_URL ) == FALSE )
	{
		// 태스크 생성에 실패 했다면 보통 폰 네트워크를 쓸수없거나 서버에 접속할수 없는 상태
		if( XE::IsHave( XTEXT(7) ) )
		{
			XWND_ALERT( "alert.network.fail", "%s", XTEXT(7) );
		}
		else
		{
			XWND_ALERT( "alert.network.fail", "%s", "could not connect to the network. Please check the status of the network." );
		}
		ChangeState( xST_ERROR_COULD_NOT_CONNECT );
	} else
	{
		// 다운받는 동안은 자동으로 슬립모드가 되지 않게 한다.
		XSYSTEM::SetAutoSleepMode(OFF);
	}
	Add( pTask );
	m_pTask = pTask;
	// 3G,WiFi체크를 먼저함.
	ChangeState( xST_CHECK_FIRST );
	// 서버들 프로토콜 버전파일 전송을 요청해서 도착하면 szDstFullPath에 저장한다.
//	ChangeState( xST_REQ_VER_PACKET );
	XLOGXN( "core version=%d", m_MyUpdateVer );
	if( m_MyUpdateVer <= 0 || (DWORD)m_MyUpdateVer == TOKEN_ERROR )
	{
		XBREAKF( 1, "core update ver file damaged." );
		XLOGXN("아마도 core에 LastVersion이 없다면 MakePackage가 잘못된것. 패치툴 실행후 패키징 할것");
		// 내 로컬업데이트 버전이 0이면 풀버전 다시 받을것.
		// 정상이라면 패키지에서 업데이트 퍄일을 카피하기때문에 0일될수가 없지만
		// 파일 손상으로 잘못된 값을 읽을수 있다.
#pragma message("check this")
	}
}

int MWScenePatchClient::ChangeState( xtState state )
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


int MWScenePatchClient::DoFSMState( xtState state, xtAct event, string *pFilename )
{
	int ret = 0;
	switch( state )
	{
	case xST_CHECK_FIRST:
		ret = FSMCheckFirst( event );
		break;
	case xST_REQ_VER_PACKET:
		ret = FSMReqVerPacket( event );
		break;
	case xST_REQ_VER_UPDATE:
		ret = FSMReqVerUpdate( event );
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
	}
	return ret;
}

// 앱 최초 실행인지 검사해서 최초 실행이면 서브파일 리스트 요청 한다.
int MWScenePatchClient::FSMCheckFirst( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		if( IsFirstPlay() )
		{
			if( XSYSTEM::CheckCDMA() )
			{
				XWND_ALERT_YESNO( pAlert, "alert.cdma.check", "%s", XTEXT(537) );
				if( pAlert )
				{
					pAlert->SetEvent( XWM_YES, this, &MWScenePatchClient::OnYesCDMADownload );
					pAlert->SetEvent( XWM_NO, GAME, &XT3::OnExitApp );
				}
			} else
				FSMCheckFirst( xACT_ON_YES );
		}
		else
			ChangeState( xST_REQ_VER_PACKET );
		break;
	case xACT_ON_YES:
		{
			// 최초 실행이면 서브파일 리스트 요청 한다.
			ChangeState( xST_REQ_VER_PACKET );
			// 배경 이미지 로딩
			if( Find("bg.patch") == NULL )
			{
				XWnd *pWnd = new XWndImage( TRUE, XE::MakePath( DIR_UI, _T("bg_lobby.png") ), 0, 0 );
				XBREAKF( pWnd == NULL, "patchClient: bg_lobby.png not found" );
				if( pWnd )
				{
					pWnd->SetstrIdentifier("bg.patch");
					Add( pWnd );
				}
			}
			if( Find("text.patch") == NULL )
			{
				XWndTextString *pText = new XWndTextString( 0, 440, _T(""), XE::GetMain()->GetSystemFontDat() );
				if( pText )
				{
					pText->SetstrIdentifier("text.patch");
					Add( pText );
				}
				m_pWndText = pText;
			}
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

// CDMA에서 다운로드를 받겠다 Yes누름.
int MWScenePatchClient::OnYesCDMADownload( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	DoFSMState( xST_CHECK_FIRST, xACT_ON_YES );	
	return 1;
}


int MWScenePatchClient::FSMReqVerPacket( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		{
			// 
			_tstring strDstFile = _T("_");
			strDstFile += _T(FILE_PACKET_VER);		// _VerPacket.h 로 받음.
			m_pTask->AddRequest( _T(XFULL_DIR), _T(FILE_PACKET_VER), strDstFile.c_str() );
			m_pTask->SetEvent( XWM_ALL_COMPLETE, this, &MWScenePatchClient::OnRecvVerPacket );
			m_pTask->Go();
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_RECV:
		{
			string strOld = SZ2C( XE::GetPathWork() );
			string strNew = strOld;
			strOld += "_VerPacket.h";
			strNew += "VerPacket.h";
			XSYSTEM::RenameFile( strOld.c_str(), strNew.c_str() );
			XSYSTEM::RemoveFile( strNew.c_str() );
			// 업데이트 버전요청상태로 전환
			ChangeState( xST_REQ_VER_UPDATE );
		}
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int MWScenePatchClient::FSMReqVerUpdate( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		{
			// 서버에 코어 업데이트 버전파일을 요청함.
			_tstring strDstLocal = _T(XUPDATE_DIR);
			strDstLocal += _T(XCORE_DIR);
			strDstLocal += _T("_");
			strDstLocal += _T(FILE_UPDATE_VER);
			_tstring strFile = _T(XCORE_DIR);
			strFile += _T(FILE_UPDATE_VER);
			m_pTask->AddRequest( _T(""), strFile.c_str(), strDstLocal.c_str() );
			m_pTask->SetEvent( XWM_ALL_COMPLETE, this, &MWScenePatchClient::OnRecvUpdateVer );
			m_pTask->Go();
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_RECV:
		{
			// 여기로 들어올때는 다운로드 에러가 없음을 보증함.
			XDownloadTask::xREQ_INFO info = m_pTask->GetCompleteFromIndex(0);
			XLOGXN("FSMReqVerUpdate: xACT_RECV: %s", info.strDstFullpath.c_str() );
			string strFilename = XE::GetFileName( info.strDstFullpath.c_str() );
			// 서버의 업데이트 버전을 읽음.
			if( XBREAKF( strFilename.empty() == true, "info.strDstFullpath == null"  ) )
			{
				XLOGXN("info.strToRename: %s", info.strToRename.c_str() );
			}
			_tstring strVer = C2SZ( strFilename.c_str() );
			int verServer = GetUpdateVer( strVer.c_str() );
			m_ServerUpdateVer = verServer;
			if( XBREAKF( verServer == 0, "verServer == 0, %s", strVer.c_str() ) )
			{
				ChangeState( xST_ERROR );
				return 1;
			}
			// 패키지 버전 얻기
//			int verPackage = GetUpdateVerFromPackage( _T(FILE_UPDATE_VER) );
//			XLOGXN("core package version:%d", verPackage);
			/*
			서버3,패키지2,work1 인 경우
			 -서버에서만 받으면 된다.
			서버3,패키지3,work1 인 경우
			 -패키지에서 카피하는게 유리하다. 서버에서받는쪽으로 하면 추가작업은 없다.
			서버3,패키지1,work1 인 경우
			 -서버에서만 받는다.
			*/
			// 버전 비교
			XLOGXN( "core version compare: my.ver=%d, ser.Ver=%d", m_MyUpdateVer, verServer );
			if( m_MyUpdateVer < verServer )
			{
				// 내 버전이 낮아서 업데이트를 받아야 함. 업데이트 목록파일 요청
				ChangeState( xST_REQ_UPDATE_LIST );
				// 배경 이미지 로딩
				if( Find("bg.patch") == NULL )
				{
					XWnd *pWnd = new XWndImage( TRUE, XE::MakePath( DIR_UI, _T("bg_lobby.png") ), 0, 0 );
					if( pWnd )
					{
						pWnd->SetstrIdentifier("bg.patch");
						Add( pWnd );
					}
				}
				if( Find("text.patch") == NULL )
				{
					XWndTextString *pText = new XWndTextString( 0, 440, _T(""), XE::GetMain()->GetSystemFontDat() );
					if( pText )
					{
						pText->SetstrIdentifier("text.patch");
						Add( pText );
					}
					m_pWndText = pText;
				}
			} else
			{
				// 업데이트를 받을 필요가 없다.
				// 씬 빠져나가기.
				ChangeState( xST_EXIT_SCENE );
			}
		}
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

// updateXX.txt 파일들 요청
int MWScenePatchClient::FSMReqUpdateList( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		{
			// 받아야할 Update00.txt 업데이트 목록파일명 생성
			// 업데이트 리스트 요청. 내버전에서부터 최신버전목록파일 까지 모두 받아온다.
			_tstring strDstLocal = XE::MakePath( _T(XUPDATE_DIR), _T("") );
			for( int i = m_MyUpdateVer + 1; i <= m_ServerUpdateVer; ++i )
			{
				_tstring strURL = _T(XCORE_DIR);
				strURL += XE::Format( _T("Update%d.txt"), i );
				m_pTask->AddRequest( _T(""), strURL.c_str(), strDstLocal.c_str() );
			} 
			XBREAKF( m_pTask->GetNumRequest() == 0, "updateXX.txt num request 0. myCorVer=%d, serverVer=%d", 
																			m_MyUpdateVer, m_ServerUpdateVer );
			m_pTask->SetEvent( XWM_ALL_COMPLETE, this, &MWScenePatchClient::OnRecvUpdateList );
			m_pTask->Go();
		}
		break;
	case xACT_PROCESS:
		break;
	// UpdateXX.txt 목록 파일들 다받음.
	case xACT_RECV:
		{
			ChangeState( xST_REQ_RECV_RES );
			// 다운받은 파일목록을 어레이에 옮겨담는다.
			XList<_tstring> listUpdateList;
			XBREAK( m_pTask->GetNumComplete() > 100 );
			XLIST_LOOP( m_pTask->GetlistComplete(), XDownloadTask::xREQ_INFO, info )
			{
				_tstring tstrURL = C2SZ( info.strURL.c_str() );
				listUpdateList.Add( tstrURL );
			} END_LOOP;

			// 업데이트 리스트의 목록들을 합치고 파일명들 중에 중복된건 걸러낸다.
			XList<_tstring> listUpdate;
			DoMergeUpdateList( listUpdateList, &listUpdate );
			//
			if( listUpdate.size() == 0 )		// 이거 필요한가.
				ChangeState( xST_EXIT_SCENE );
			// 추려낸 실제 리소스 파일들을 요청한다.
			XLIST_LOOP( listUpdate, _tstring, strRes )
			{
				m_pTask->AddRequest( _T(XFULL_DIR), strRes.c_str() );
			} END_LOOP;
			m_pTask->SetEvent( XWM_ALL_COMPLETE, this, &MWScenePatchClient::OnRecvRes );
			m_pTask->Go();
			return 1;
		}
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int MWScenePatchClient::FSMReqRecvRes( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		break;
	case xACT_PROCESS:
		{
			// 다운받는 상황 보여주기
			XDownloadTask::xRECV_INFO info;
			info = m_pTask->GetCurrDownloadInfo();
			if( m_pWndText )
			{
				TCHAR szBuff[ 1024 ];
				TCHAR szFilename[ 128 ];
				_tcscpy_s( szFilename, XE::GetFileName( info.szURL ) );
#ifdef _DEV
				_tcscpy_s( szBuff, XE::Format( _T(" core update %d/%d: %s (%d bytes received)"), 
					m_pTask->GetidxCurrDownload(),
					m_pTask->GetNumRequest(),
					szFilename,
					info.sizeCurr ) );
#else
				_tcscpy_s( szBuff, XE::Format( _T(" core update %d/%d"), 
					m_pTask->GetidxCurrDownload(),
					m_pTask->GetNumRequest() ) );
#endif
				m_pWndText->SetText( szBuff );
			}
		}
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int MWScenePatchClient::FSMExitScene( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		if( XBaseScene::IsExit() == FALSE )
		{
			XLOGXNA("MWScenePatchClient::FSMExitScene: Finish patch update(core&sub)");
			// 패치 클라가 끝나면 게임리소스 생성
			GAME->OnFinishPatchClient();
			DoExit( xSCENE::xSC_TITLE );
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int MWScenePatchClient::FSMError( xtAct event )
{
	switch( event )
	{
	case xACT_ENTER:
		{
		// 에러 사안별로 좀더 꼼꼼하게 에러메시지 표시할것.
#pragma message("check this")
			XWND_ALERT_RET( pAlert, "alert.patch.error", "%s", XTEXT(413) );	// 문제발생. 앱다시 실행
			if( pAlert )
				pAlert->SetEvent( XWM_OK, GAME, &XT3::OnExitApp );
		}
		break;
	case xACT_PROCESS:
		break;
	case xACT_LEAVE:
		break;
	}
	return 1;
}

int MWScenePatchClient::Process( float dt ) 
{ 
	//
	DoFSMState( m_State, xACT_PROCESS );
	//
	return XBaseScene::Process( dt );
}

//
void MWScenePatchClient::Draw( void ) 
{
	XBaseScene::Draw();
	if( m_pTask->GetbGo() )
	{
		XDownloadTask::xRECV_INFO info;
		info = m_pTask->GetCurrDownloadInfo();
		XUTIL::DrawProgressBar( XE::VEC2( 20, 460 ), (float)m_pTask->GetidxCurrDownload(), 
																(float)m_pTask->GetNumRequest(), 
																XE::VEC2( 280, 10 ) );
	} else
	if( m_pTask->GetbComplete() )
	{
		XUTIL::DrawProgressBar( XE::VEC2( 20, 460 ), 1.0f, XE::VEC2( 280, 10 ) );
	}
	XBaseScene::DrawTransition();
}

void MWScenePatchClient::OnLButtonDown( float lx, float ly ) 
{
	XBaseScene::OnLButtonDown( lx, ly );
}
void MWScenePatchClient::OnLButtonUp( float lx, float ly ) {
	XBaseScene::OnLButtonUp( lx, ly );
}
void MWScenePatchClient::OnMouseMove( float lx, float ly ) {
	XBaseScene::OnMouseMove( lx, ly );
}

// 패킷버전 파일 받음.
int MWScenePatchClient::OnRecvVerPacket( XWnd *pWnd, DWORD, DWORD )
{
	DoFSMState( xST_REQ_VER_PACKET, xACT_RECV );
	return 1;
}

// 코어 업데이트 버전 파일 받음.
int MWScenePatchClient::OnRecvUpdateVer( XWnd *pWnd, DWORD, DWORD )
{
	DoFSMState( xST_REQ_VER_UPDATE, xACT_RECV );
	return 1;
}

void MWScenePatchClient::OnCompleteRecvFile( const char *cURL, const char *cFullpath, int size )
{
}

// UpdateXX.txt 업데이트 목록 파일들 다받음.
int MWScenePatchClient::OnRecvUpdateList( XWnd *pWnd, DWORD, DWORD )
{
	DoFSMState( xST_REQ_UPDATE_LIST, xACT_RECV );
	return 1;
}

int MWScenePatchClient::DoMergeUpdateList( XList<_tstring>& listUpdateList,
											XList<_tstring> *pOut )
//													XArrayLinearN<_tstring, 1000> *pOut )
{
	int _size = sizeof(_tstring);
	int max = 0;
	int duplicate = 0;
	// updateXX.txt 파일들.
	XLIST_LOOP( listUpdateList, _tstring, _strFile )
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
		} else
		{
			int i = 0;
			XTRACE( "\r\n%s", szFile );
			while( token.GetToken() )
			{
				// UpdateXX.txt파일에 있는 파일목록을 읽어서 목록에 넣는다.
				// 이미 목록에 있는 파일명이면 넣지 않는다.
				_tstring strFile = _tstring( token.m_Token );
				if( pOut->Find( strFile ) == FALSE )
					pOut->Add( strFile ); 
				else
					++duplicate;
				XTRACE( "%d:%s", i++, token.m_Token );
			}
		}
	} END_LOOP;
	XLOGXN( "max update list: %d", pOut->size() );
	XLOGXN( "duplicate files number: %d", duplicate );
	return pOut->size();
}

// 리소스 파일들 다받음.
int MWScenePatchClient::OnRecvRes( XWnd *pWnd, DWORD, DWORD )
{
	XLOGXN( "core patch complete" );
	if( m_pWndText )
		m_pWndText->SetText( _T("Update complete") );
	{
		// 성공적으로 업데이트가 다 끝났다.
		// LastUpdate 버전을 갱신시킴.(_LastUpdate.txt를 LastUpdate.txt로 복사)
		{
			XLOGXN( "copy _LastUpdate -> LastUpdate" );
			char cWorkPath[ 256 ];
			strcpy_s( cWorkPath, SZ2C( XE::MakeWorkFullPath( DIR_ROOT, _T("") ) ) );
			// workpath/update/core/_LastUpdate.txt  풀패스 생성
			char cTempUpdateVer[ 256 ];
			strcpy_s( cTempUpdateVer, cWorkPath );
			strcat_s( cTempUpdateVer, XUPDATE_DIR );
			strcat_s( cTempUpdateVer, XCORE_DIR );
			strcat_s( cTempUpdateVer, "_" );
			strcat_s( cTempUpdateVer, FILE_UPDATE_VER );
			// workpath + LastUpdate.txt 풀패스 생성
			char cUpdateVer[ 256 ];
			strcpy_s( cUpdateVer, cWorkPath );
			strcat_s( cUpdateVer, XUPDATE_DIR );
			strcat_s( cUpdateVer, XCORE_DIR );
			strcat_s( cUpdateVer, FILE_UPDATE_VER );
			XSYSTEM::CopyFileX( cTempUpdateVer, cUpdateVer );
		}

		ChangeState( xST_EXIT_SCENE );
	}
	XLOGXN( "OnRecvRes finished" );
	return 1;
}


// 앱 최초 실행인가.
BOOL MWScenePatchClient::IsFirstPlay( void )
{
	string strFile = SZ2C( XE::GetPathWork() );
	strFile += "patch.ini";
	FILE *fp = NULL;
	fopen_s( &fp, strFile.c_str(), "rt" );
	if( fp == NULL )
		return TRUE;
	int val = 0;
	fscanf_s( fp, "%d", &val );
	fclose(fp);
	return val != 1;	// 1이 안써있으면 깨진걸로 생각하고 처음 실행으로 간주함
}

BOOL MWScenePatchClient::SavePatchIni( void )
{
	string strFile = SZ2C( XE::GetPathWork() );
	strFile += "patch.ini";
	FILE *fp = NULL;
	fopen_s( &fp, strFile.c_str(), "wt" );
	if( fp == NULL )
		return FALSE;
	fprintf_s( fp, "%d", 1 );
	fclose( fp );
	return TRUE;
}



#endif 