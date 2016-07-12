#include "stdafx.h"
#include "XEUserMng.h"
#include "XServerMain.h"
#include "XWinSocketSvr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

float XEUserMng::s_secAutoSave = 60.f;			// 자동세이브 주기
int XEUserMng::s_bResetSaveTimer = 0;

//////////////////////////////////////////////////////////////////////
XEUserMng::XEUserMng( XEWinSocketSvr *pParent, int maxUser )
{
	Init();
	m_pParentSocketSvr = pParent;
	XBREAK( m_pParentSocketSvr == NULL );
//	m_listUser.Create( maxUser );
//	m_listAddReserve.Create( maxUser );
//	m_listDelReserve.Create( maxUser );
}

// 계정아이디로 유저를 찾는다. 
XSPUserBase XEUserMng::GetspUserFromidAcc( ID idAccount )
{
//	m_pParentSocketSvr->EnterCS();
	auto itor = m_mapUser.find( idAccount );
	if( itor != m_mapUser.end() ) {
		auto spUser = (*itor).second;
		XBREAK( spUser == nullptr );		// 이런경우는 없어야 한다.
		return spUser;
	}
	return nullptr;
}

/**
 매니저에서 유저를 빼고 각종 핸들러를 불러준다.
 윈소켓서버에서 유저가 삭제되기 직전에 불려진다.
*/
void XEUserMng::DelUser( XSPUserBase spUser )
{
	// 죽기전 마지막 핸들러를 보내준다.
	const ID idUser = spUser->GetidUser();
	spUser->Save();
	spUser->OnDestroy();
	// 하위 유저매니저에게도 알린다.
	OnDestroyUser( spUser );
	// 맵에서도 삭제시켜 검색도 안되게함.
	XBREAK( spUser->GetspDBAccount() == nullptr );
	m_mapUser.erase( idUser );
	// 리스트에서도 빼버림
//	int idxResult = m_listUser.Del( pUser );
//	ID idUser = spUser->getid();
	XBREAK( idUser == 0 );
	int idxResult = m_listUser.DelByID( idUser );
	XBREAK( idxResult == -1 );			// 삭제실패. 비상상황.
	spUser->ClearConnect();		// 커넥션 소유권 포기(커넥션과 상호참조하고 있어서 미리 끊음)
}
//
bool XEUserMng::Add( const XSPUserBase spUser ) 
{ 
	bool bRet = true;
	do {
		XBREAK( spUser == nullptr );
		XBREAK( spUser->GetspConnectConst() == nullptr );
		XBREAK( spUser->GetspDBAccountConst() == nullptr );
		XBREAK( spUser->GetidUser() == 0 );
		const ID idUser = spUser->GetidUser();
		// 같은 아이디가 이미 있으면 안됨
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		auto spExist = GetspUserFromidAcc( idUser );
		XBREAK( spExist != nullptr );
#endif
		m_mapUser[ idUser ] = spUser;	// 계정번호기반 검색용  맵에 추가
		m_listUser.Add( spUser );
	} while(0);
	return bRet;
}

//
void XEUserMng::Process( float dt ) 
{
	if( this == nullptr )
		return;
	const bool bResetSaveTimer = (s_bResetSaveTimer != 0);
	if( m_listUser.size() <= 0 )
		return;
	for( auto spUser : m_listUser ) {
		// 이것때문에라도 유저매니저 프로세스가 필요하긴 함.
		if( spUser->GettimerSave().IsOver() ) {		// 유저별로 주기적으로 세이브
#ifndef _DEBUG
//			spUser->Save();
#endif
			spUser->GettimerSave().Reset();
		}
		if( !spUser->IsDestroy() ) {
			spUser->Process( dt );
			if( bResetSaveTimer ) {
				const auto sec = s_secAutoSave;
				if( sec > 0 ) {
					if( XASSERT( sec > 30.f ) )		// 최소 30초는 되어야 함.
						spUser->GettimerSave().Set( sec );
				} else {
					spUser->GettimerSave().Off();
				}
			}
		}
	}
}

// XWinSocketSvr의 스레드이외의 스레드에서 부르지 말것.
void XEUserMng::Save( void )
{
	XTRACE("%d", m_listUser.size() );
	for( auto spUser : m_listUser ) {
		spUser->Save();
	}
}
/////////////////////////////////////////////////////////////////

// void XEUserMng::ResetAutoSaveTimer( float sec )
// {
// 	for( auto spUser : m_listUser ) {
// 		if( !spUser->IsDestroy() ) {
// 			if( sec > 0 ) {
// 				if( XASSERT(sec > 30.f) )		// 최소 30초는 되어야 함.
// 					spUser->GettimerSave().Set( sec );
// 			} else
// 				spUser->GettimerSave().Off();
// 		}
// 	}
// }
