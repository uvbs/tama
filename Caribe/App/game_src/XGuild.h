#pragma once
#include <stdlib.h>
#include "XArchive.h"
#include <list>
#include "XGuildH.h"

// const int DEFAULT_GUILD_MEMBER_COUNT = 30;
// #define VER_GUILD_SERIALIZE		2
// 
// namespace xnGuild {
// struct xMember;
// }
// 
// //////////////////////////////////////////////////////////////////////////
// /****************************************************************
// * @brief 
// * @author xuzhu
// * @date	2016/05/09 12:07
// *****************************************************************/
// class XDelegateGuild
// {
// public:
// 	XDelegateGuild() {}
// 	virtual ~XDelegateGuild() {}
// 	// public member
// 	virtual void DelegateGuildAccept( XGuild* pGuild, const xnGuild::xMember& memberJoin ) {}
// 	virtual void DelegateGuildReject( XGuild* pGuild, const xnGuild::xMember& memberReject ) {}
// 	virtual void DelegateGuildChangeMemberInfo( XGuild* pGuild, const xnGuild::xMember& memberTarget, const xnGuild::xMember& memberConfirm ) {}
// 	virtual void DelegateGuildKick( XGuild* pGuild, const xnGuild::xMember& memberKick ) {}
// 	virtual void DelegateGuildOut( XGuild* pGuild, const xnGuild::xMember& memberOut ) {}
// 	virtual void DelegateGuildUpdate( XGuild* pGuild, const xnGuild::xMember& member ) {}
// }; // class XDelegateGuild
////////////////////////////////////////////////////////////////
class XGuild
{
	static XDelegateGuild* s_pDelegate;
public:
	static void sSetpDelegate( XDelegateGuild* pDelegate ) {
		s_pDelegate = pDelegate;
	}
	static void sSerialize( XGuild* pGuild, XArchive& ar );
	static XGuild* sCreateDeserialize( XArchive& ar );
	static void sSerializeGuildInfo( XGuild* pGuild, XArchive& ar );
	static XGuild* sCreateDeserializeInfo( XArchive& ar );
	static void sSerializeUpdate( XGuild* pGuild, XGAME::xtBitGuildUpdate bitUpdate, XArchive* pOut );
	static void sDeSerializeUpdate( XArchive& ar, XGuild* pGuild );
	static void sUpdateByEvent( XGuild* pGuild, XGAME::xtGuildEvent event, XArchive& arParam );
public:
	struct SGuildBuildOption{
		int		s_NeedGold;
		int		s_NeedResource[XGAME::xRES_MAX];		
		bool s_bAutoAccept;			//자동 승낙.
		bool s_bBlockReqJoin;		//신청 거부
	};
	struct SGuildMember {
		ID m_idAcc;		
		int m_lvAcc;	// 길드원 Level.
		XGAME::xtGuildGrade	m_Grade;		// 길드 내 등급.	(XGAME::xtGuildGrade)
		_tstring m_strName;		// 길드원 닉네임
//		_tstring	s_strContext;		// 길드원 설명? 묘사? 필요한 정보. (길드 가입 요청 했을때는 요청한 시간 정도를 넣어 줄까?)
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int ver );
		void UpdateWithxMember( const xnGuild::xMember& member );
		void ToxMember( xnGuild::xMember* pOut ) const;
	};
private:
	DWORD		m_Ver;					// 계정 시리얼라이즈 버전
	_tstring	m_strName;				// 길드 이름
	_tstring	m_strMasterName;		// 비밀번호
	_tstring	m_strGuildContext;
	_tstring	m_strBuildTime;			// 생성시간. 년-월-일-시-분
	ID			m_GuildIndex;			// Guild생성 번호.
	ID			m_CurrSN;				//길드내 사용 SN
	ID			m_idAccMaster;	//길드 Master Idaccount;
	int			m_MaxMemberCount;
	int			m_MemberCount;
	int			m_joinreqMemberCount;
	int			m_Level;
	SGuildBuildOption					m_Option;
	std::list<SGuildMember*>			m_listMember;		//길드원 List
	std::list<SGuildMember*>			m_listJoinReqer;		//길드 가입 요청 목록
  XList4<xCampaign::CampObjPtr> m_listRaid;    // 길드레이드 캠페인 리스트
	XSPLock m_spLock;

	void Init() {
		m_Ver = 0;
		m_CurrSN = 0;	
		m_MemberCount = 0;
		m_joinreqMemberCount = 0;
		m_GuildIndex = 0;			// Guild생성 번호.		
		m_idAccMaster = 0;	//길드 Master Idaccount;
		m_MaxMemberCount = DEFAULT_GUILD_MEMBER_COUNT;
		m_Level = 0;
		m_Option.s_NeedGold = 0;
		m_Option.s_bAutoAccept = FALSE;
		m_Option.s_bBlockReqJoin = FALSE;
	}
	void Destroy();
public:	
	void ClearGuild(){ Destroy(); }
	GET_SET_ACCESSOR_CONST(int, MaxMemberCount);
	XGuild();
	XGuild(ID idGuildIndex);
	XGuild(ID idGuildIndex, _tstring  szGuildName);
	virtual ~XGuild() { Destroy(); }
	//
	ID GetGuildSN() { 
		return ++m_CurrSN;
	}
	GET_SET_ACCESSOR_CONST(ID, GuildIndex);
	ID GetidGuild() const {
		return m_GuildIndex;
	}
	GET_SET_ACCESSOR_CONST(int, MemberCount);
	GET_SET_ACCESSOR_CONST(ID, Ver);
	GET_SET_ACCESSOR_CONST(ID, CurrSN);
	GET_SET_ACCESSOR_CONST(ID, idAccMaster);
	GET_SET_TSTRING_ACCESSOR(strName);
	GET_SET_TSTRING_ACCESSOR(strMasterName);
	GET_SET_TSTRING_ACCESSOR(strGuildContext);
	GET_ACCESSOR( XSPLock, spLock );

	BOOL AddGuildMember(SGuildMember* pMember);
	BOOL AddGuildMember( ID idAcc, const _tstring& strUsername, int lvAcc, XGAME::xtGuildGrade grade );
	BOOL AddGuildMember( const xnGuild::xMember& userNew );
	bool AddGuildMemberConst( const SGuildMember* pUser );
	bool AddGuildMemberInReqerList( ID idAcc );
	SGuildMember* AddGuilJoinTodMember( ID idaccount, _tstring  strUsername, int level, XGAME::xtGuildGrade grade );
	SGuildMember* AddJoinAcceptMember(_tstring  Buff);

	void SetGuildOption(bool bAutoAccept, bool bBlockAccept);

	BOOL RemoveGuildMember(ID idAcc, xnGuild::xMember* pOut);
	ID RemoveGuildMember(const _tstring& strUserName);
	BOOL UpdateGuildMemberInfo(SGuildMember* pData);
	BOOL UpdateGuildOption(SGuildBuildOption* pData);
	ID FindidAccMemberByUserName(const _tstring& strUsername) const;
	const SGuildMember* FindpMemberByidAcc(ID idAcc) const;
	SGuildMember* FindpMemberByidAccMutable( ID idAcc );

	//대기열
	BOOL AddGuildJoinReqMember(ID idaccount, _tstring  strUsername, int level);
	BOOL RemoveJoinReqer( const _tstring& strUsername );
	BOOL RemoveJoinReqer( ID idAcc, xnGuild::xMember* pOut );
	BOOL IsJoinReqerUser( ID idacc ) const;
	ID FindGuildJoinMemberIdaccount(_tstring  strUsername);

	SGuildMember* GetpReqerUserByidAcc( const _tstring& strUserName );
	SGuildMember* GetpReqerUserByidAcc(ID idAcc);

	
	void Serialize( XArchive& ar );
	void DeSerialize(XArchive& ar);

	void SerializeGuildInfo(XArchive& ar);
	void DeSerializeGuildInfo(XArchive& ar);
	
	void SerializeGuildMemberlist(XArchive& ar);
	void DeSerializeGuildMemberlist(XArchive& ar);

	void SerializeGuildReqMemberlist(XArchive& ar);
	void DeSerializeGuildReqMemberlist(XArchive& ar);

	void SerializeGuildOption(XArchive& ar);
	void DeSerializeGuildOption(XArchive& ar);

	int	GetnumMembers() const { 
		return (int)m_listMember.size(); 
	}
	int	GetnumReqerList() const { 
		return (int)m_listJoinReqer.size();
	}
	bool IsFullMember() const {
		return m_listMember.size() >= XGAME::MAX_GUILD_MEMBER_COUNT;
	}

	//
	const std::list<SGuildMember*>& GetListMember() const {
		return m_listMember;
	}
	const std::list<SGuildMember*>& GetListJoinReq() const {
		return m_listJoinReqer;
	}
	const SGuildBuildOption& GetGuildOption() const {
		return m_Option;
	}
	void SetOptionAutoAccept( bool bOption) {
		m_Option.s_bAutoAccept = bOption;
	}
	void SetOptionBlockReqJoin( bool bOption) {
		m_Option.s_bBlockReqJoin = bOption;
	}
	xCampaign::CampObjPtr GetAndCreateRaidCampaign( LPCTSTR idsCamp );
	xCampaign::CampObjPtr GetAndCreateRaidCampaign( const _tstring& strCamp ) {
		return GetAndCreateRaidCampaign( strCamp.c_str() );
	}
	xCampaign::CampObjPtr GetAndCreateRaidCampaign( ID idCamp );
	// 현재 생성된 길드레이드 수
	int GetnumCampaignByRaid() {
		return m_listRaid.size();
	}
	xCampaign::CampObjPtr FindspRaidCampaign( ID idCamp );
	int DeSerializeUpdateGuildRaidCamp( XArchive& arCamp, ID idCamp );
	XGAME::xtGuildGrade GetGradeByMember( ID idAcc );
	bool IsMemberByidAcc( ID idAcc ) const {
		return (FindpMemberByidAcc( idAcc ) != nullptr);
	}
	bool IsMemberByUserName( const _tstring& strName ) const {
		return (FindidAccMemberByUserName( strName ) != 0);
	}
	void UpdateWithxMember( const xnGuild::xMember& member );
private:
	void DestroyJoinReqList();
};

// namespace xnGuild {
// struct xResult {
// 	XGAME::xtGuildGrade m_gradeMember = XGAME::xGGL_NONE;
// 	// 거부되었다면 거부사유
// };
// // (전송용)멤버 한명에 대한 구조체
// struct xMember {
// 	ID m_idAcc = 0;
// 	int m_lvAcc = 0;
// 	XGAME::xtGuildGrade m_Grade = XGAME::xGGL_NONE;
// 	_tstring m_strName;
// 	xMember() {}
// 	xMember( ID idAcc, const _tstring& strName, int lvAcc, XGAME::xtGuildGrade grade )
// 		: m_idAcc( idAcc ), m_strName( strName ), m_lvAcc( lvAcc ), m_Grade( grade ) {}
// 	xMember( const XGuild::SGuildMember& gMember ) {
// 		Set( gMember );
// 	}
// 	void Set( const XGuild::SGuildMember& gMem ) {
// 		m_idAcc = gMem.m_idAcc;
// 		m_lvAcc = gMem.m_lvAcc;
// 		m_strName = gMem.m_strName;
// 		m_Grade = gMem.m_Grade;
// 	}
// 	void Serialize( XArchive& ar ) const;
// 	void DeSerialize( XArchive& ar, int ver );
// };
// };
