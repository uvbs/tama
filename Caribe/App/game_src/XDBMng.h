#pragma once
#ifndef _XDBMNG2

#include "XLock.h"
#include "etc/ContainerSTL.h"
#include "server/XWinConnection.h"
#include <queue>

class XDBUAccount;
class XEWinConnectionInServer;
class XDatabase;
class XMain;

class XDBMng
{
private:
//	static DWORD s_idGlobal;
#ifdef _XNUM_CORE
	enum { MAX_DB = _XNUM_CORE };
#else
  #ifdef _XEProfile
	enum { MAX_DB=1 };
  #else
	enum { MAX_DB=4 };
  #endif
#endif
	enum xtMsg { xFIND_NONE,
		xFIND_UUID,
		xFIND_ID_NAME,
		xFIND_ID_ACCOUNT,
		xFIND_ID_PASSWORD,
		xNEW_ACCOUNT,
		xNEW_ACCOUNT_IDPW,		
		xNEW_ACCOUNT_FACEBOOK,
		xSAVE_ACCOUNT,
		xREGISTER_ACCOUNT,	
		xFIND_IDACCOUNT,
		xFIND_FB_USERID, 
		xFIND_FB_REGIST_USERID,
		XFIND_DUPLICATE_VERIFY,
		xADD_USERLOG,
		xFIND_USER_BY_SCORE,
		xGET_LEGION_BY_IDACC,
	};
public:
	struct xREQUEST {
//		ID idRequest;		// 요청 아이디(나중에 결과찾을때 쓰인다)
		xtMsg msgFind;			// 검색 종류
		XDBUAccount *pOut;		// 검색후 데이타를 받을 메모리 공간.
		BOOL bFound;			// 찾았으면 TRUE
//		bool bRecvResult;		// 요청결과를 받기/안받기.

		ID idConnect;			// 콜백용 아이디. 아이디 방식으로 바뀜
		void ( XEWinConnectionInServer::*pCallback)( BOOL , XDBUAccount*, xREQUEST* );

		_tstring str1;
		_tstring str2;
		_tstring str3;
		DWORD param1;
		DWORD param2;
		DWORD param3;
		DWORD param4;
		DWORD param5;
		DWORD param6;

		xREQUEST() {
			idConnect = 0;
			pOut = NULL;
			bFound = FALSE;
			msgFind = xFIND_NONE;
			param1 = 0;
			param2 = 0;
			param3 = 0;
			param4 = 0;
			param5 = 0;
			pCallback = NULL; 

		} 
		_tstring& GetstrUUID() { return str1; }
		_tstring& GetstrID() { return str1; }
		_tstring& GetstrFBUserID() { return str1; }

		_tstring& GetstrPassword() { return str2; }
		_tstring& GetstrFBUserName() { return str2; }
		
		_tstring& GetstrNickname() { return str3; }
		void SetstrUUID( LPCTSTR szUUID ) { 
			XASSERT( str1.empty() == true );
			str1 = szUUID; 
		}
		void SetstrID( LPCTSTR szIDName ) { 
			XASSERT( str1.empty() == true );
			str1 = szIDName; 
		}
		void SetFbID( LPCTSTR szFbId ) {
			XASSERT( str1.empty() == true );
			str1 = szFbId;
		}
		void SetFbUserName( LPCTSTR szFbUserName ) {
			XASSERT( str2.empty() == true );
			str2 = szFbUserName;
		}
		void SetstrPassword( LPCTSTR szPassword ) { 
			XASSERT( str2.empty() == true );
			str2 = szPassword; 
		}
		void SetstrNickname( LPCTSTR szNickname ) { 
			XASSERT( str3.empty() == true );
			str3 = szNickname; 
		}
		
		
		ID GetIDConnect() {return idConnect;}
		void SetIDConnect( DWORD idConn ) { idConnect = idConn; }		
		ID GetIDClientConnect() { return param3; };
		void SetIDClientConnect( ID idConnect ) { param3 = idConnect; }
		ID GetidAccount() { return (ID)param2; }
		void SetidAccount( DWORD idAccount ) { param2 = idAccount; }		
		int GetScoreMin( void ) {
			return (int)param1;
		}
		void SetScoreMin( int score ) {
			param1 = (DWORD)score;
		}
		int GetScoreMax( void ) {
			return (int)param2;
		}
		void SetScoreMax( int score ) {
			param2 = (DWORD)score;
		}
		ID GetidFindAcc( void ) {
			return (ID)param1;
		}
		void SetidFindAcc( ID idFindAcc ) {
			param1 = (DWORD)idFindAcc;
		}
		ID GetidSpot( void ) {
			return (ID)param4;
		}
		void SetidSpot( ID idSpot ) {
			param4 = (DWORD)idSpot;
		}
		ID GetidPacket( void ) {
			return (ID)param5;
		}
		void SetidPacket( ID idPacket ) {
			param5 = (DWORD)idPacket;
		}
		ID GetidFinder( void ) {
			return (ID)param6;
		}
		void SetidFinder( ID idFinder ) {
			param6 = (DWORD)idFinder;
		}
	};
private:
//	typedef XQueueArray<xREQUEST> XQueueRequest;
	typedef std::queue<xREQUEST> XQueueRequest;
	typedef std::queue<xREQUEST> XQueueFound;
	XSharedObj<XQueueRequest> m_shoQRequest;		// 검색요청 큐
	XSharedObj<XQueueFound> m_shoQFound;		// 검색결과 큐
	HANDLE m_hThread[ MAX_DB ];		// work thread
	XDatabase *m_pDB[ MAX_DB ];
	volatile BOOL m_bExit;
	//
	void Init() {
		XCLEAR_ARRAY( m_hThread );
		XCLEAR_ARRAY( m_pDB );
		m_bExit = FALSE;
	}
	void Destroy();
public:
	XDBMng( int maxRequest );
	virtual ~XDBMng() { Destroy(); }
	//
	BOOL Create( void );
	void WorkThread( XDatabase *pDB );
	void Process( void );
	// 요청 결과 받기
//	BOOL GetCompleteRequest( ID idRequest, XDBMng::xREQUEST *pOut );
	// UUID로 검색 요청

	void RequestNewAccountIDPW(	XDBUAccount *pOut, 
											LPCTSTR szID,	
											LPCTSTR szPW,
											LPCTSTR szNickName,
											XEWinConnectionInServer *pConnect, 
											void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) );

	void RequestFindWithFBUserID(	XDBUAccount *pOut,
											LPCTSTR szfacebookuserid,
											LPCTSTR szfacebookusername,
											LPCTSTR szNickname,
											XEWinConnectionInServer *pConnect, 
											void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) );
	void RequestFBNewAccount(		XDBUAccount *pOut,	
											LPCTSTR szFBuserID,
											LPCTSTR szFBusername,
											LPCTSTR szNickName,
											XEWinConnectionInServer *pConnect, 
											void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ); 
	void RequestFindWithIDName(	XDBUAccount *pOut,
											LPCTSTR szID,
											LPCTSTR szPassword,
											XEWinConnectionInServer *pConnect, 
											void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ); 
	void RequestFindWithUUIDForNewAccount(	XDBUAccount *pOut,
															LPCTSTR szUUID,
															LPCTSTR szNickName,
															XEWinConnectionInServer *pConnect, 
															void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ); 
	void RequestNewAccount(			XDBUAccount *pOut,
											LPCTSTR szUUID,
											LPCTSTR szNickName,
											XEWinConnectionInServer *pConnect, 
											void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ); 
	void RequestFindWithIDPWForNewAccount(	XDBUAccount *pOut,
															LPCTSTR szID,
															LPCTSTR szPW,
															LPCTSTR szNickName,
															XEWinConnectionInServer *pConnect,
															void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ); 
	void RequestFindWithUUID(					XDBUAccount *pOut,
														LPCTSTR szUUID, 
														XEWinConnectionInServer *pConnect, 
														void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) );
	
	void RequestFindWithUserIdAccount (	XDBUAccount *pOut,
													ID idAccount,
													ID idGameSvrConnect,
													ID idClientConnect, 
													XEWinConnectionInServer *pConnect, 
													void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) );

	void XDBMng::RequestRegisterAccount(		ID idAccount,
														LPCTSTR szNickname,
														LPCTSTR szIDName,
														LPCTSTR szPassword,
														XEWinConnectionInServer *pConnect, 
														void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ) ;
	void RequestRegisterFBAccount(				XDBUAccount *pOut,
														ID idaccount,
														LPCTSTR szFBuserID,
														LPCTSTR szFBusername,
														LPCTSTR szNickname,
														XEWinConnectionInServer *pConnect,
														void ( XEWinConnectionInServer::*pCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) );

	//Billing
	void RequestDuplicateVerify(					ID idaccount,
														int numBuyGem,
														LPCTSTR szJsonReceiptFromClient,
														XEWinConnectionInServer *pConnect,
														void ( XEWinConnectionInServer::*pSvrCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ));	
	void RequestSaveAccount( ID idAccount, XDBUAccount *pAccount );
	void RequestUserLogAdd( ID idAccount, TCHAR* szIdname, int type, TCHAR* szLog );
	void RequestFindUserByScore( XDBUAccount *pAccOut, 
								int scoreMin, int scoreMax, 
								ID idFinder,
								ID idSpot, ID idClientConnect, ID idPacket,
								XEWinConnectionInServer *pConnect, 
								void ( XEWinConnectionInServer::*pSvrCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ) ;
	void RequestGetLegionByIdAcc( XDBUAccount *pAccOut,
								ID idAcc,
								ID idSpot, ID idClientConnect, ID idPacket,
								XEWinConnectionInServer *pConnect, 
								void ( XEWinConnectionInServer::*pSvrCallback)( BOOL, XDBUAccount*, XDBMng::xREQUEST* ) ) ;

};

typedef void ( XEWinConnectionInServer::*WC_CALLBACK)( BOOL, XDBUAccount*, XDBMng::xREQUEST* );
#define _WC(F)		static_cast<WC_CALLBACK>( F )

#ifdef _xCONTENTS_SERVER_EVENT
typedef void ( XEWinConnectionInServer::*WCE_CALLBACK)( BOOL, XServerEventMng*, XDBMng::xREQUEST* );
#define _WCE(F)		static_cast<WCE_CALLBACK>( F )
#endif
extern XDBMng *DBMNG;


#endif 