#pragma once
#include "XArchive.h"

const int DEFAULT_GUILD_MEMBER_COUNT = 30;
#define VER_GUILD_SERIALIZE		2

class XGuild;
namespace xnGuild {
struct xMember;
}

//////////////////////////////////////////////////////////////////////////
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/05/09 12:07
*****************************************************************/
class XDelegateGuild
{
public:
	XDelegateGuild() {}
	virtual ~XDelegateGuild() {}
	// public member
	virtual void DelegateGuildAccept( XGuild* pGuild, const xnGuild::xMember& memberJoin ) {}
	virtual void DelegateGuildReject( XGuild* pGuild, const xnGuild::xMember& memberReject ) {}
	virtual void DelegateGuildChangeMemberInfo( XGuild* pGuild, const xnGuild::xMember& memberTarget, const xnGuild::xMember& memberConfirm ) {}
	virtual void DelegateGuildKick( XGuild* pGuild, const xnGuild::xMember& memberKick ) {}
	virtual void DelegateGuildOut( XGuild* pGuild, const xnGuild::xMember& memberOut ) {}
	virtual void DelegateGuildUpdate( XGuild* pGuild, const xnGuild::xMember& member ) {}
}; // class XDelegateGuild

namespace xnGuild {
struct xResult {
	XGAME::xtGuildGrade m_gradeMember = XGAME::xGGL_NONE;
	// 거부되었다면 거부사유
};
// (전송용)멤버 한명에 대한 구조체
struct xMember {
	ID m_idAcc = 0;
	int m_lvAcc = 0;
	XGAME::xtGuildGrade m_Grade = XGAME::xGGL_NONE;
	_tstring m_strName;
	xMember() {}
	xMember( ID idAcc, const _tstring& strName, int lvAcc, XGAME::xtGuildGrade grade )
		: m_idAcc( idAcc ), m_strName( strName ), m_lvAcc( lvAcc ), m_Grade( grade ) {}
// 	xMember( const XGuild::SGuildMember& gMember ) {
// 		Set( gMember );
// 	}
// 	void Set( const XGuild::SGuildMember& gMem ) {
// 		m_idAcc = gMem.m_idAcc;
// 		m_lvAcc = gMem.m_lvAcc;
// 		m_strName = gMem.m_strName;
// 		m_Grade = gMem.m_Grade;
// 	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int ver );
};
};
