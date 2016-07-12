#include "stdafx.h"
#ifndef _XDBMNG2
#include "XDBMng.h"
#include "XDatabase.h"
#include "XDBUAccount.h"
#include "server/XWinConnection.h"
#include "XClientConnection.h"
#include "XMain.h"
#if defined ( _LOGIN_SVR )
#include "XSocketForClient.h"
#include "XGameSvrSocket.h"
#elif defined ( _DBAGENT_SVR)
#include "XSocketForGameSvr.h"
#endif

#define BTRACE		XTRACE
//#define BTRACE(...)		(0);

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XDBMng *DBMNG = NULL;
//DWORD XDBMng::s_idGlobal = 0;

//////////////////////////////////////////////////////////////////////////
static unsigned long __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	XDatabase *pDB = static_cast<XDatabase*>( param );
	//
	DBMNG->WorkThread( pDB );
	return 1;
}

//////////////////////////////////////////////////////////////////////////
XDBMng::XDBMng( int maxRequest ) 
{ 
	Init(); 
	// DB검색 요청 큐버퍼를 만든다.
	XQueueRequest *pQueueRequest = &m_shoQRequest.GetSharedObj();
	m_shoQRequest.ReleaseSharedObj();
	// 검색한 결과를 담을 큐버퍼를 만든다.
	XQueueFound *pQueueFound = &m_shoQFound.GetSharedObj();
	m_shoQFound.ReleaseSharedObj(); 
	//
}

void XDBMng::Destroy()
{
	m_bExit = TRUE;
	Sleep(1000);		// 스레드 빠져나올시간 줌.
	for( int i = 0; i < MAX_DB; ++i )
	{
//		TerminateThread( m_hThread[i], 0 );
		CloseHandle( m_hThread[i] );
	}

	XBREAK( m_shoQRequest.IsLock() == TRUE );
	//
	XQueueRequest *pQueueRequest = &m_shoQRequest.GetSharedObj();
	while( pQueueRequest->size() > 0 )
	{
		xREQUEST req = pQueueRequest->front();
		pQueueRequest->pop();
		SAFE_DELETE( req.pOut );
	}
	m_shoQRequest.ReleaseSharedObj();
	//
	XBREAK( m_shoQFound.IsLock() == TRUE );
	// Lock: idRequest의 번호로 끝난 결과물이 있는지 찾아본다.
	SHARED_OBJ_BLOCK( pQueue, XQueueFound, m_shoQFound )
	{
		xREQUEST req;
		while( pQueue->size() > 0 )
		{
			req = pQueue->front();
			pQueue->pop();
			SAFE_DELETE( req.pOut );
		}
/*		LIST_MANUAL_LOOP( *pQueue, xREQUEST, itor, req )
		{
			SAFE_DELETE( req.pOut );
		} END_LOOP;
		pQueue->clear(); */
	} END_BLOCK;
	for( int i = 0; i < MAX_DB; ++i )
		SAFE_DELETE( m_pDB[i] );
}

BOOL XDBMng::Create( void )
{
	// DB 스레드 생성
	for( int i = 0; i <MAX_DB; ++i ) 
	{
		m_pDB[i] = new XDatabase;
		DWORD idThread;
		m_hThread[i] = CreateThread( NULL, 0, _WorkThread, (LPVOID) m_pDB[i], 0, &idThread ); 
	}
	return TRUE;
}

void XDBMng::WorkThread( XDatabase *pDB )
{
	while(1)
	{
		XQueueRequest *pQueueRequest = NULL;
		while(1)
		{
			if( m_bExit )
				return;
			// Lock
			pQueueRequest = &m_shoQRequest.GetSharedObj();
			if( pQueueRequest->size() > 0 )
				break;
			m_shoQRequest.ReleaseSharedObj();
#ifdef _XLOW_SERVER
			Sleep(1);
#else
			Sleep(0);
#endif
		}
		{
			XPROF_OBJ("main");
			{
				XPROF_OBJ("db request");
				// 요청큐에서 하나 꺼냄
				xREQUEST req = pQueueRequest->front();
				pQueueRequest->pop();
				// Unlock: 
				m_shoQRequest.ReleaseSharedObj();
		//
				BOOL bFind = FALSE;
				switch( req.msgFind )
				{
				case xNEW_ACCOUNT: req.bFound = pDB->CreateNewAccount( req.pOut, req.GetstrNickname().c_str(), req.GetstrUUID().c_str() );			break;
				case xFIND_UUID: req.bFound = pDB->FindAccountByUUID( req.pOut, req.GetstrUUID().c_str() );										break;
				case xFIND_IDACCOUNT: req.bFound = pDB->FindAccountFromIdAccount( req.pOut, req.GetidAccount());										break;				
				case xSAVE_ACCOUNT:
					{
						pDB->SaveAccount( req.GetidAccount(), req.pOut );		// pOut의 계정데이타를 저장한다.
						SAFE_RELEASE_REF( req.pOut );			
						break;
					}
				case xFIND_USER_BY_SCORE:	
					req.bFound = pDB->FindUserByPower( req.pOut, 
														req.GetScoreMin(), 
														req.GetScoreMax(), 
														req.GetidFinder() );
					break;
				case xGET_LEGION_BY_IDACC:
					req.bFound = pDB->GetLegionByIdAcc( req.pOut, 0, req.GetidFindAcc() );
					break;
			//	case xFIND_ID_NAME:					req.bFound = pDB->FindAccount( req.pOut, req.GetstrID().c_str() );														break;
			
			//	case xFIND_FB_USERID:					req.bFound = pDB->FindAccountFromFaceBook( req.pOut, req.GetstrID().c_str(), req.GetstrPassword().c_str() );		break;
			//	case xNEW_ACCOUNT_IDPW:			req.bFound = pDB->CreateNewAccountIDPW( req.pOut, req.GetstrID().c_str(), req.GetstrPassword().c_str() );		break;
			//	case xNEW_ACCOUNT_FACEBOOK:	req.bFound = pDB->CreateNewAccountFaceBook( req.pOut, req.GetstrID().c_str(), req.GetstrPassword().c_str() , req.GetstrNickname().c_str());	break;
			
			//	case xREGISTER_ACCOUNT:
			//		{
			//			BYTE bResult = pDB->RegistAccount( req.GetidAccount(), req.GetstrNickname().c_str(), 
			//														req.GetstrID().c_str(), req.GetstrPassword().c_str() );
			//			if( bResult == 1 )			// 등록성공
			//				req.bFound = TRUE;
			//			else if( bResult == 2 )		// 실패: 중복된 idAccount
			//				req.bFound = FALSE;
			//		}
			//		break;
			//	case xFIND_FB_REGIST_USERID:	
			//		req.bFound = pDB->RegistFBAccount(	req.GetidAccount(), 
			//														req.GetstrID().c_str(), 
			//														req.GetstrPassword().c_str(), 
			//														req.GetstrNickname().c_str() ); 
			//		break;

			//	case xADD_USERLOG: pDB->AddUserLog( req.param1, req.param2, req.str1.c_str(), req.str2.c_str() );										break;				
			//	case XFIND_DUPLICATE_VERIFY:
			//		{				
			//			req.bFound = pDB->FindDuflicateVerify( req.GetidAccount(), req.GetstrID().c_str());
			//			CONSOLE("XFIND_DUPLICATE_VERIFY %d(%d)", req.GetidAccount(), req.bFound );
			//			break;
			//		}
				default:
					XBREAKF( 1, "알수없는 요청 메시지" );
					break;
				}
				// 검색결과 큐에 넣음.
				// Lock:
				if( req.pCallback )		// 요청결과를 다시 받고 싶은경우에만 넣는다.
				{
					XQueueFound *pQueueFound = &m_shoQFound.GetSharedObj();
					//
					pQueueFound->push( req );
					// Unlock:
					m_shoQFound.ReleaseSharedObj();
				}
			}
		}
	}

}

void XDBMng::Process( void )
{
	xREQUEST req;
	// Lock: 찾은 결과물 데이타의 큐를 각 커넥션 객체에게 통보해준다.
	SHARED_OBJ_BLOCK( pQueue, XQueueFound, m_shoQFound )
	{
//		LIST_MANUAL_LOOP( *pQueue, xREQUEST, itor, req )
		// 큐가 비워질때까지 다 퍼냄
		while( pQueue->size() > 0 )
		{
			req = pQueue->front();
			pQueue->pop();
			XEWinConnectionInServer *pConnect = NULL;
			XBREAK( req.idConnect == 0 );

#if defined ( _LOGIN_SVR )
			if( MAIN->GetpSocketForClient() )
			{
				pConnect = MAIN->GetpSocketForClient()->FindConnection( req.idConnect );
				if( pConnect == NULL )
					pConnect = MAIN->GetpSocketForGameSvr()->FindConnection( req.idConnect );
			}
#elif defined ( _DBAGENT_SVR)
			if( MAIN->GetpSocketForGameSvr() )
			{
				pConnect = MAIN->GetpSocketForGameSvr()->FindConnection( req.idConnect );
				if( pConnect == NULL )
					pConnect = MAIN->GetpSocketForGameSvr()->FindConnection( req.idConnect );
			}
#endif
			if( pConnect ) 
			{
				if( req.pCallback )	// 콜백이 있으면 콜백날려줌.
					(pConnect->*req.pCallback)( req.bFound, req.pOut, &req );
			}				
		}
	} END_BLOCK;
	
}

// UUID로 검색 요청
void XDBMng::RequestFindWithUUID( XDBUAccount *pOut, LPCTSTR szUUID, 
											XEWinConnectionInServer *pConnect, 
											WC_CALLBACK pCallback ) 
//											void ( XWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ) 
{
	xREQUEST req;
	req.msgFind = xFIND_UUID;
	req.pOut = pOut;
	req.SetstrUUID( szUUID );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestFindWithUserIdAccount (	XDBUAccount *pOut,
															ID idAccount,
															ID idGameSvrConnect,
															ID idClientConnect,
															XEWinConnectionInServer *pConnect,
															WC_CALLBACK pCallback )
{
	xREQUEST req;
	req.msgFind = xFIND_IDACCOUNT;
	req.pOut = pOut;
	req.SetidAccount( idAccount );
	req.SetIDConnect( idGameSvrConnect );
	req.SetIDClientConnect( idClientConnect );

	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestRegisterFBAccount( XDBUAccount *pOut, 
										ID idaccount, 
										LPCTSTR szFBuserID, 
										LPCTSTR szFBusername, 
										LPCTSTR szNickname, 
										XEWinConnectionInServer *pConnect, 
										WC_CALLBACK pCallback )
{
	xREQUEST req;
	req.msgFind = xFIND_FB_REGIST_USERID;
	req.SetidAccount( idaccount );
	req.pOut = pOut;
	req.SetFbID( szFBuserID );
	XBREAK( XE::IsEmpty(szFBusername) == TRUE );
	req.SetFbUserName( szFBusername );
	req.SetstrNickname( szNickname );
	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}
void XDBMng::RequestFindWithFBUserID( XDBUAccount *pOut, 
											LPCTSTR szfacebookuserid, 
											LPCTSTR szfacebookusername,
											LPCTSTR szNickname,
											XEWinConnectionInServer *pConnect,  
											WC_CALLBACK pCallback )
{
	xREQUEST req;
	req.msgFind = xFIND_FB_USERID;
	req.pOut = pOut;
	req.SetFbID( szfacebookuserid );
	req.SetFbUserName( szfacebookusername );
	req.SetstrNickname( szNickname );
	req.SetidAccount( pOut->GetidAccount() );
	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

// 영문ID로 검색 요청
void XDBMng::RequestFindWithIDName( XDBUAccount *pOut, LPCTSTR szID, LPCTSTR szPassword,
												XEWinConnectionInServer *pConnect, 
												WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xFIND_ID_NAME;
	req.pOut = pOut;
	req.SetstrID( szID );
	req.SetstrPassword( szPassword );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

// UUID로 검색 요청(새계정생성)
void XDBMng::RequestFindWithUUIDForNewAccount(		XDBUAccount *pOut,
																	LPCTSTR szUUID,
																	LPCTSTR szUserNickName, 
																	XEWinConnectionInServer *pConnect, 
																	WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xFIND_UUID;
	req.pOut = pOut;
	req.SetstrUUID( szUUID );
	req.SetstrNickname( szUserNickName );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestFindWithIDPWForNewAccount(	XDBUAccount *pOut,
																	LPCTSTR szID,
																	LPCTSTR szPW, 
																	LPCTSTR szUserNickName, 
																	XEWinConnectionInServer *pConnect, 
																	WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xNEW_ACCOUNT_IDPW;
	req.pOut = pOut;
	req.SetstrID( szID );
	req.SetstrPassword( szPW );
	req.SetstrNickname( szUserNickName );
	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestFBNewAccount(		XDBUAccount *pOut,
													LPCTSTR szFBUserID,
													LPCTSTR szFBUsername,
													LPCTSTR szNickName, 
													XEWinConnectionInServer *pConnect, 
													WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xNEW_ACCOUNT_FACEBOOK;
	req.pOut = pOut;
	req.SetstrID( szFBUserID );
	req.SetstrPassword( szFBUsername );
	req.SetstrNickname ( szNickName );	

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 검색요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

// 새계정 생성요청
void XDBMng::RequestNewAccount(	XDBUAccount *pOut,
								LPCTSTR szUUID,
								LPCTSTR szNickName, 
								XEWinConnectionInServer *pConnect, 
								WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xNEW_ACCOUNT;
	req.pOut = pOut;
	req.SetstrUUID( szUUID );
	if( XE::IsEmpty(szNickName) )
		req.SetstrNickname( _T("test_user") );	// 일단 닉네임이 비어있지 않게
	else
		req.SetstrNickname( szNickName );
	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

// DB에 pAccount데이타를 저장 요청
void XDBMng::RequestSaveAccount( ID idAccount,
											XDBUAccount *pAccount)
{
	xREQUEST req;
	req.msgFind = xSAVE_ACCOUNT;
	req.pOut = pAccount;
	req.param1 = idAccount;
//	req.bRecvResult = false;		// 결과는 받지 않겠음.
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

// 계정등록요청
void XDBMng::RequestRegisterAccount( ID idAccount, LPCTSTR szNickname, LPCTSTR szIDName, LPCTSTR szPassword, 
												XEWinConnectionInServer *pConnect, 
												WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xREGISTER_ACCOUNT;
	req.SetidAccount( idAccount );
	req.SetstrID( szIDName );
	req.SetstrNickname( szNickname );
	req.SetstrPassword( szPassword );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestDuplicateVerify( ID idaccount, int numBuyGem, LPCTSTR szJsonReceiptFromClient , XEWinConnectionInServer *pConnect, WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = XFIND_DUPLICATE_VERIFY;
	req.SetidAccount( idaccount );
	req.param1 = (DWORD) numBuyGem;
	req.SetstrID( szJsonReceiptFromClient );	

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestUserLogAdd( ID idAccount, TCHAR* szIdname, int type, TCHAR* szLog )
{
	xREQUEST req;
	req.msgFind = xADD_USERLOG;
	req.SetidAccount( idAccount );
	req.param1 = idAccount;
	req.param2 = type;
	req.SetstrID ( szIdname );
	req.SetstrPassword( szLog );	
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestFindUserByScore( XDBUAccount *pAccOut,
									int scoreMin, int scoreMax, 
									ID idFinder,
									ID idSpot, ID idClientConnect, ID idPacket,
									XEWinConnectionInServer *pConnect, 
									WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xFIND_USER_BY_SCORE;
	req.pOut = pAccOut;
	req.SetScoreMin( scoreMin );
	req.SetScoreMax( scoreMax );
	req.SetidSpot( idSpot );
	req.SetIDClientConnect( idClientConnect );
	req.SetidPacket( idPacket );
	req.SetidFinder( idFinder );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

void XDBMng::RequestGetLegionByIdAcc( XDBUAccount *pAccOut,
									ID idAcc,
									ID idSpot, ID idClientConnect, ID idPacket,
									XEWinConnectionInServer *pConnect, 
									WC_CALLBACK pCallback ) 
{
	xREQUEST req;
	req.msgFind = xGET_LEGION_BY_IDACC;
	req.pOut = pAccOut;
	req.SetidFindAcc( idAcc );
	req.SetidSpot( idSpot );
	req.SetIDClientConnect( idClientConnect );
	req.SetidPacket( idPacket );

	req.idConnect = pConnect->GetidConnect();
	req.pCallback = pCallback;
	BTRACE( "%s", __TFUNC__ );
	// 작업요청 큐에 밀어 넣음.
	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}

#endif // not _XDBMNG2
