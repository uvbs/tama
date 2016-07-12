#pragma once
#include <map>
#include "XArchive.h"

#ifdef _SERVER
class XGuild;

class XGuildMgr : public XLock
{
// 	struct Deleter {
// 		void operator()( XGuildMgr *pMgr ) {
// 			if( pMgr )
// 				XGuildMgr::sRelease();
// 		}
// 	};
//	static XSharedObj<XGuildMgr*> s_soGuildMgr;
 	static XGuildMgr *s_pGuildMgr;

public:
	static void sDestroy() {
		SAFE_DELETE( s_pGuildMgr );
// 		auto p = s_soGuildMgr.GetSharedObj();
// 		SAFE_DELETE( p );
// 		s_soGuildMgr.ReleaseSharedObj();
	}
	static XGuildMgr* sGet();
// #ifdef _SERVER
// 	static std::shared_ptr<XGuildMgr> sSafeGet() {
// 		auto pGuildMgr = s_soGuildMgr.GetSharedObj();
// 		if( pGuildMgr == nullptr ) {
// 			if( s_pGuildMgr )
// 				pGuildMgr = s_pGuildMgr;
// 			else {
// 				pGuildMgr = new XGuildMgr;
// 				s_pGuildMgr = pGuildMgr;
// 			}
// 			s_soGuildMgr.Set( pGuildMgr );
// 		}
// 		return std::shared_ptr<XGuildMgr>( pGuildMgr, Deleter() );
// 	}
// #endif // _SERVER
// 	static void sRelease() {
// 		s_soGuildMgr.ReleaseSharedObj();
// 	}
private:
	void DestroyAll();
public:
	XGuildMgr();
	~XGuildMgr();
	
	BOOL RequestJoinGuild(_tstring  szGuildName, ID requseridaccount, _tstring  requsername, int level);
	
	void AddGuild(XGuild* pData);
	BOOL RemoveGuild(ID guildindex);

	XGuild* FindGuild(ID GuildID);
	XGuild* FindGuild(_tstring  szGuildName);

	void SendToAllGuildUser(XArchive& ar);
	void SendToGuildUserID(XArchive& ar);	

	BOOL SerializeGuildInfo(XArchive& ar);	
	BOOL SerializeGuildAll(XArchive& ar);

//	BOOL SerializeGuildInfo(ID guildID, XArchive& ar);
//	BOOL SerializeGuildAll(ID guildID, XArchive& ar);

	BOOL DeSerializeGuildInfo(XArchive& ar);
	BOOL DeSerializeGuildAll(XArchive& ar);

	int	 GetGuildSize(){ return (int)m_mapGuild.size(); }
private:
	std::map< ID, XGuild* > m_mapGuild;		// XGuild* shared_ptr로 바꾸고 길드 개별락도 걸어야함.
	std::map< _tstring, XGuild* > m_mapGuildID;
};

// inline XGuildMgr* GetGuildMgr() {
// 	return XGuildMgr::sGet();
// }

//typedef std::shared_ptr<XGuildMgr> GuildMgrPtr;
#endif // _SERVER
