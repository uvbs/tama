#include "stdafx.h"
#ifdef _SERVER
#include "XGuildMgr.h"
#include "XGuild.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//XGuildMgr* GUILD_MGR = NULL;
XGuildMgr* XGuildMgr::s_pGuildMgr = nullptr;
//XSharedObj<XGuildMgr*> XGuildMgr::s_soGuildMgr;
//XLock XGuildMgr::s_Lock;

#define xLOCK_OBJ	XLockObj _lockObj( &s_Lock, __TFUNC__ );

XGuildMgr* XGuildMgr::sGet() 
{
	if( s_pGuildMgr == nullptr ) {
		static XLock s_Lock;
		s_Lock.Lock(__TFUNC__);
		if( s_pGuildMgr == nullptr )
			s_pGuildMgr = new XGuildMgr;
		s_Lock.Unlock();
	}
	return s_pGuildMgr;
}

XGuildMgr::XGuildMgr()
{
#ifdef _LOGIN_SERVER
	XBREAK(1);		// 로그인서버에선 사용금지. 컴파일 편의상 넣기만 함.
#endif // _LOGIN_SERVER
}


XGuildMgr::~XGuildMgr()
{
	DestroyAll();
}

void XGuildMgr::DestroyAll()
{
	for( auto itor : m_mapGuild ) {
		XGuild* pGuild = itor.second;
		SAFE_DELETE( pGuild );
	}
	m_mapGuild.clear();
}
BOOL XGuildMgr::RequestJoinGuild(_tstring  szGuildName, ID requseridaccount, _tstring  requsername, int level)
{
	XLOCK_OBJ;
	auto pGuild = FindGuild( szGuildName );
	if( pGuild == nullptr )
		return FALSE;
	// NOTE: spGuild->Lock()	 pGuild를 shared_ptr로 쓰면 길드 개별락으로 성능을 향상시킬수 있다.
	pGuild->AddGuildJoinReqMember(requseridaccount, requsername, level);
	return TRUE;
}

// BOOL XGuildMgr::SerializeGuildInfo(ID GuildIndex, XArchive& ar)
// {
// 	XLOCK_OBJ;
// 	auto itor = m_mapGuild.find( GuildIndex );
// //	if( itor == m_mapGuild.end() ) return FALSE;
// 	if( itor != m_mapGuild.end() ) {
// 		auto pGuild = itor->second;
// 		if( pGuild == nullptr )
// 			return FALSE;
// 		pGuild->SerializeGuildInfo( ar );
// 	} else {
// 		ar << 0;
// 	}
// 	return TRUE;
// }

// BOOL XGuildMgr::SerializeGuildAll(ID GuildIndex, XArchive& ar)
// {
// 	XLOCK_OBJ;
// 	auto itor = m_mapGuild.find( GuildIndex );
// 
// 	if( itor == m_mapGuild.end() ) return FALSE;
// 	auto pGuild = itor->second;
// 	if( pGuild == nullptr )
// 		return FALSE;
// 	pGuild->Serialize( ar );
// 	return TRUE;
// }

BOOL XGuildMgr::SerializeGuildAll(XArchive& ar)
{
	XLOCK_OBJ;
	ar << (int)m_mapGuild.size();
	for( auto itor : m_mapGuild ) {
		XGuild* pGuild = itor.second;
		pGuild->Serialize( ar );
	}
	return TRUE;
}
BOOL XGuildMgr::DeSerializeGuildAll(XArchive& ar)
{
	XLOCK_OBJ;
	DestroyAll();
	DWORD dw1;
	ar >> dw1; //size
	for( int n = 0; n < (int)dw1; n++ ) {
		XGuild* pData = new XGuild();
		pData->DeSerialize( ar );
		AddGuild( pData );
// 		if( AddGuild( pData ) == FALSE ) {
// 			RemoveGuild( pData->GetGuildIndex() );
// 			AddGuild( pData );
// 		}
	}

	return TRUE;
}

BOOL XGuildMgr::SerializeGuildInfo(XArchive& ar)
{	
	XLOCK_OBJ;
	ar << (int)m_mapGuild.size();
	if( (int)m_mapGuild.size() > 0 ) {
		for( auto itor : m_mapGuild ) {
			XGuild* pGuild = itor.second;
			XGuild::sSerializeGuildInfo( pGuild, ar );
		}
	}
	return TRUE;
}

BOOL XGuildMgr::DeSerializeGuildInfo(XArchive& ar)
{
	XLOCK_OBJ;
	DestroyAll();
	DWORD dw1 = 0;
	ar >> dw1; //size
	for( int n = 0; n < (int)dw1; n++ ) {
		XGuild* pData = new XGuild;
		pData->DeSerialize( ar );
		AddGuild( pData );
// 		if( AddGuild( pData ) == FALSE ) {
// 			RemoveGuild( pData->GetGuildIndex() );
// 			AddGuild( pData );
// 		}
	}

	return TRUE;
}

void XGuildMgr::AddGuild(XGuild* pData)
{
	XLOCK_OBJ;
	XBREAK(pData == NULL);
	const ID guildid = pData->GetGuildIndex();
	XBREAK(guildid == 0);

	m_mapGuild[ guildid ] = pData;
	_tstring strName = pData->GetstrName();
	m_mapGuildID[ strName ] = pData;
	// ??? 
// 	std::pair< std::map<ID, XGuild*>::iterator, bool > p = m_mapGuild.insert(std::map<ID, XGuild*>::value_type(guildid, pData));
// 	std::pair< std::map<_tstring, ID>::iterator, bool > r = m_mapGuildID.insert(std::map<_tstring, ID>::value_type(pData->GetstrName(), guildid));

//	return TRUE;
}

BOOL XGuildMgr::RemoveGuild(ID guildindex)
{
	XLOCK_OBJ;
	auto itor = m_mapGuild.find(guildindex);
	if( itor != m_mapGuild.end() ) {
		XGuild* ptr = itor->second;
		if( ptr != nullptr ) {
//			ptr->ClearGuild();		// ????. delete ptr하면 XGuild::Destroy자동으로 불리는데 머하러 이렇게 함?
			m_mapGuild.erase( guildindex );
			SAFE_DELETE( ptr );
		}
	}
	return TRUE;
}

XGuild* XGuildMgr::FindGuild(ID GuildID)
{
#pragma message("모든 길드객체 shared_ptr로 바꿔야 스레드 안전해짐")
	if( GuildID == 0 )
		return nullptr;
	XLOCK_OBJ;
	auto itor = m_mapGuild.find(GuildID);
	if (itor != m_mapGuild.end()) {
		return itor->second;
	}
	return nullptr;
}

XGuild* XGuildMgr::FindGuild(_tstring strGuildName)
{
	XLOCK_OBJ;
	ID findguild = 0;
	auto itor = m_mapGuildID.find(strGuildName);
	if( itor != m_mapGuildID.end() ) {
		auto pGuild = itor->second;
		XBREAK( pGuild == nullptr );
		return pGuild;
	}
	return nullptr;
}
#endif // _SERVER