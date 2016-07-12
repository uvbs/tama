#include "stdafx.h"
#include "XGuild.h"
#include "XCampObj.h"
#include "XStageObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace XGAME;

XDelegateGuild* XGuild::s_pDelegate = nullptr;
//////////////////////////////////////////////////////////////////////////
void XGuild::SGuildMember::Serialize( XArchive& ar ) const {
	ar << m_idAcc;
	ar << m_lvAcc;
	ar << (DWORD)m_Grade;
	ar << m_strName;
}

void XGuild::SGuildMember::DeSerialize( XArchive& ar, int ver ) {
	DWORD dw1;
	ar >> m_idAcc;
	ar >> m_lvAcc;
	ar >> dw1;		m_Grade = (xtGuildGrade)dw1;
	ar >> m_strName;
}
void XGuild::SGuildMember::UpdateWithxMember( const xnGuild::xMember& member ) {
	m_idAcc = member.m_idAcc;
	m_strName = member.m_strName;
	m_lvAcc = member.m_lvAcc;
	m_Grade = member.m_Grade;
}
void XGuild::SGuildMember::ToxMember( xnGuild::xMember* pOut ) const {
	pOut->m_idAcc = m_idAcc;
	pOut->m_strName = m_strName;
	pOut->m_lvAcc = m_lvAcc;
	pOut->m_Grade = m_Grade;
}

//////////////////////////////////////////////////////////////////////////
void xnGuild::xMember::Serialize( XArchive& ar ) const {
	ar << m_idAcc;
	ar << (char)m_lvAcc;
	ar << (char)m_Grade;
	ar << (short)0;
	ar << m_strName;
}
void xnGuild::xMember::DeSerialize( XArchive& ar, int ver ) {
	char c0;
	ar >> m_idAcc;
	ar >> c0;	m_lvAcc = c0;
	ar >> c0;	m_Grade = (xtGuildGrade)c0;
	ar >> c0 >> c0;
	ar >> m_strName;
}
//////////////////////////////////////////////////////////////////////////
void XGuild::sSerializeUpdate( XGuild* pGuild, xtBitGuildUpdate bitUpdate, XArchive* pOut )
{
	(*pOut) << (DWORD)bitUpdate;		// 32비트 업데이트 항목들의 비트
	if( bitUpdate & xBGU_MASTER ) {
		(*pOut) << (DWORD)xBGU_MASTER;
		(*pOut) << pGuild->GetidAccMaster();
		(*pOut) << pGuild->Get_strMasterName();
	}
	if( bitUpdate & xBGU_OPTION ) {
		(*pOut) << (DWORD)xBGU_OPTION;
		pGuild->SerializeGuildOption( (*pOut) );
	}
	if( bitUpdate & xBGU_CONTEXT ) {
		(*pOut) << (DWORD)xBGU_CONTEXT;
		(*pOut) << pGuild->Get_strGuildContext();
	}
	if( bitUpdate & xBGU_REQERS ) {
		(*pOut) << (DWORD)xBGU_REQERS;
		pGuild->SerializeGuildReqMemberlist( (*pOut) );
	}
	if( bitUpdate & xBGU_MEMBERS ) {
		(*pOut) << (DWORD)xBGU_MEMBERS;
		pGuild->SerializeGuildMemberlist( (*pOut) );
	}
}
void XGuild::sDeSerializeUpdate( XArchive& ar, XGuild* pGuild )
{
	DWORD dw0;
	ar >> dw0;		auto bitUpdate = (xtBitGuildUpdate)dw0;		// 32비트 업데이트 항목들의 비트
	if( bitUpdate & xBGU_MASTER ) {
		ar >> dw0;	auto bitChunk = (xtBitGuildUpdate)dw0;
		XBREAK( bitChunk != xBGU_MASTER );
		ar >> pGuild->m_idAccMaster;
		ar >> pGuild->m_strMasterName;
	}
	if( bitUpdate & xBGU_OPTION ) {
		ar >> dw0;	auto bitChunk = (xtBitGuildUpdate)dw0;
		XBREAK( bitChunk != xBGU_OPTION );
		pGuild->DeSerializeGuildOption( ar );
	}
	if( bitUpdate & xBGU_CONTEXT ) {
		ar >> dw0;	auto bitChunk = (xtBitGuildUpdate)dw0;
		XBREAK( bitChunk != xBGU_CONTEXT );
		ar >> pGuild->m_strGuildContext;
	}
	if( bitUpdate & xBGU_REQERS ) {
		ar >> dw0;	auto bitChunk = (xtBitGuildUpdate)dw0;
		XBREAK( bitChunk != xBGU_REQERS );
		pGuild->DeSerializeGuildReqMemberlist( ar );
	}
	if( bitUpdate & xBGU_MEMBERS ) {
		ar >> dw0;	auto bitChunk = (xtBitGuildUpdate)dw0;
		XBREAK( bitChunk != xBGU_MEMBERS );
		pGuild->DeSerializeGuildMemberlist( ar );
	}
}

void XGuild::sUpdateByEvent( XGuild* pGuild, xtGuildEvent event, XArchive& arParam )
{
	bool bBroadcast = true;
	//
	if( !pGuild && event != xGEV_CREATE )
		return;
	XBREAK( pGuild == nullptr );
	const ID idGuild = pGuild->GetidGuild();
	//
	switch( event ) {
// 	case xGEV_CREATE: {
// 		pGuild = new XGuild();
// 		pGuild->DeSerialize( arParam );
// 		XGuildMgr::sGet()->AddGuild( pGuild );
// 	} break;
// 	case xGEV_DESTROY: {
// 		XGuildMgr::sGet()->RemoveGuild( idGuild );
// 	} break;
	case xGEV_JOIN_ACCEPT: {
		xnGuild::xMember member;
		arParam >> member;
		// 해당멤버의 데이터를 동기화 한다.
		pGuild->AddGuildMember( member );
//		pGuild->UpdateWithxMember( member );
		pGuild->RemoveJoinReqer( member.m_idAcc, nullptr );
		if( s_pDelegate )
			s_pDelegate->DelegateGuildAccept( pGuild, member );
	} break;
	case xGEV_JOIN_REJECT: {
		ID idAccTarget;
		arParam >> idAccTarget;
		xnGuild::xMember member;
		pGuild->RemoveJoinReqer( idAccTarget, &member );
		bBroadcast = false;		// 이건 방송하지 않음.
		if( s_pDelegate )
			s_pDelegate->DelegateGuildReject( pGuild, member );
	} break;
	case xGEV_CHANGE_MEMBER_INFO: {
		xnGuild::xMember memberConfirm;
		xnGuild::xMember memberTarget;
		arParam >> memberConfirm >> memberTarget;
		pGuild->UpdateWithxMember( memberConfirm );
		pGuild->UpdateWithxMember( memberTarget );
		// 길마가 바뀜
		if( memberTarget.m_Grade == xGGL_LEVEL5 ) {
			pGuild->SetidAccMaster( memberTarget.m_idAcc );
			pGuild->SetstrMasterName( memberTarget.m_strName );
		}
		if( s_pDelegate )
			s_pDelegate->DelegateGuildChangeMemberInfo( pGuild, memberTarget, memberConfirm );
	} break;
	case xGEV_KICK: {
		ID idAccKicker, idAccTarget;
		_tstring strTarget;
		arParam >> idAccKicker;		// 누구에게 쫒겨난건가.
		arParam >> idAccTarget;		// 누가 쫒겨난건가.
		arParam >> strTarget;			// 쫒겨난 유저 이름.
		xnGuild::xMember member;
		pGuild->RemoveGuildMember( idAccTarget, &member );
		if( s_pDelegate )
			s_pDelegate->DelegateGuildKick( pGuild, member );
	} break;
	case xGEV_OUT: {
		ID idAccTarget;
		_tstring strName;
		arParam >> idAccTarget;
		arParam >> strName;
		xnGuild::xMember member;
		pGuild->RemoveGuildMember( idAccTarget, &member );
		if( s_pDelegate )
			s_pDelegate->DelegateGuildOut( pGuild, member );
	} break;
	case xGEV_UPDATE: {
		XGuild::sDeSerializeUpdate( arParam, pGuild );
// 		if( s_pDelegate )
// 			s_pDelegate->DelegateGuildReject( pGuild, member );
	} break;
	default:
		XBREAK( 1 );
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void XGuild::Destroy()
{
	for( auto pNode : m_listMember ) {
		SAFE_DELETE( pNode );
	}
	m_listMember.clear();
	for( auto pNode : m_listJoinReqer ) {
		SAFE_DELETE( pNode );
	}
	m_listJoinReqer.clear();
}

XGuild::XGuild()
{
	Init();
	m_spLock = std::make_shared<XLock>();
}
XGuild::XGuild(ID idGuildIndex)
{
	Init();
	m_spLock = std::make_shared<XLock>();
}

XGuild::XGuild(ID idGuildIndex, _tstring strGuildName)
{
	Init();
	m_spLock = std::make_shared<XLock>();
}

BOOL XGuild::AddGuildMember(SGuildMember* pUser)
{
	if (FindpMemberByidAcc(pUser->m_idAcc) )
		return FALSE;	
	m_listMember.push_back(pUser);
	return TRUE;
}

BOOL XGuild::AddGuildMember( const xnGuild::xMember& userNew )
{
	if( FindpMemberByidAcc( userNew.m_idAcc ) )
		return FALSE;
	auto *pMember = new SGuildMember();
	pMember->UpdateWithxMember( userNew );
	m_listMember.push_back( pMember );
	return TRUE;
}

/**
 @brief pData를 직접 리스트에 넣지않고 카피값으로 넣는다.
*/
bool XGuild::AddGuildMemberConst( const SGuildMember* pUser )
{
	if( FindpMemberByidAcc( pUser->m_idAcc ) )
		return FALSE;
	return FALSE != AddGuildMember( pUser->m_idAcc, pUser->m_strName, pUser->m_lvAcc, pUser->m_Grade );
}

BOOL XGuild::AddGuildMember(ID idAcc, const _tstring& strUsername, int lvAcc, xtGuildGrade grade)
{
	if (FindidAccMemberByUserName(strUsername) ) 
		return FALSE;

	auto pData = new SGuildMember;
	pData->m_lvAcc = lvAcc;
	pData->m_Grade = grade;
	pData->m_idAcc = idAcc;
	pData->m_strName = strUsername;

	m_listMember.push_back(pData);
	return TRUE;
}

/**
 @brief reqer list에 있는 유저 idAcc를 가입시킨다.
*/
bool XGuild::AddGuildMemberInReqerList( ID idAcc )
{
	auto pUser = GetpReqerUserByidAcc( idAcc );
	if( !pUser )
		return false;
	auto bOk = AddGuildMember( pUser->m_idAcc, pUser->m_strName, pUser->m_lvAcc, pUser->m_Grade );
	return xBOOLToBool(bOk);
}

XGuild::SGuildMember* XGuild::AddGuilJoinTodMember(ID idaccount, _tstring  szUsername, int level, XGAME::xtGuildGrade grade)
{
	if ( FindidAccMemberByUserName(szUsername) > 0)
		return nullptr;

	SGuildMember* pData = new SGuildMember;
	pData->m_lvAcc = level;
	pData->m_Grade = grade;
	pData->m_idAcc = idaccount;
//	pData->s_strContext = szUserContext;
	pData->m_strName = szUsername;

	m_listMember.push_back(pData);
	return pData;
}

XGuild::SGuildMember* XGuild::AddJoinAcceptMember(_tstring  Buff)
{
	SGuildMember* pData = GetpReqerUserByidAcc(Buff);
	if (pData)
	{
		SGuildMember* ptr = AddGuilJoinTodMember(pData->m_idAcc, pData->m_strName.c_str(), pData->m_lvAcc, XGAME::xtGuildGrade::xGGL_LEVEL1);
		if (ptr)
		{
			if (RemoveJoinReqer(Buff))
				return ptr;
		}
	}
	return nullptr;
}

BOOL XGuild::RemoveGuildMember(ID idAcc, xnGuild::xMember* pOut )
{
	auto	itor = m_listMember.begin();
	for( ; itor != m_listMember.end(); itor++ ) {
		auto pMember = ( *itor );
		if( pMember ) {
			if( pMember->m_idAcc == idAcc ) {
				m_listMember.erase( itor );
				if( pOut )
					pMember->ToxMember( pOut );
				delete pMember;
				return TRUE;
			}
			// pData
		}
	}
	return FALSE;
}

ID XGuild::RemoveGuildMember( const _tstring& strUserName)
{
	ID idRemoveUser = 0;
	auto	itor = m_listMember.begin();
	for( ; itor != m_listMember.end(); itor++ ) {
		SGuildMember* pMember = ( *itor );
		if( pMember ) {
			if( pMember->m_strName == strUserName ) {
				idRemoveUser = pMember->m_idAcc;
				m_listMember.erase( itor );
				delete pMember;
				return idRemoveUser;
			}
			// pData
		}
	}
	return 0;
}

BOOL XGuild::UpdateGuildMemberInfo(SGuildMember* pData)
{
	XBREAK(pData == NULL);
	auto	itor = m_listMember.begin();	
	for (; itor != m_listMember.end(); itor++)
	{
		SGuildMember* pmember = (*itor);
		if (pmember && ( pmember->m_idAcc == pData->m_idAcc))
		{
			pmember->m_lvAcc = pData->m_lvAcc;	
			pmember->m_Grade = pData->m_Grade;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL XGuild::UpdateGuildOption(SGuildBuildOption* pData)
{
	m_Option.s_NeedGold = pData->s_NeedGold;
	m_Option.s_bAutoAccept = pData->s_bAutoAccept;
	m_Option.s_bBlockReqJoin = pData->s_bBlockReqJoin;
	//m_Option.s_NeedResource[XGAME::xRES_WOOD] = pData->s_NeedResource[XGAME::xRES_WOOD];
	//m_Option.s_NeedResource[XGAME::xRES_IRON] = pData->s_NeedResource[XGAME::xRES_IRON];
	//m_Option.s_NeedResource[XGAME::xRES_JEWEL] = pData->s_NeedResource[XGAME::xRES_JEWEL];
	//m_Option.s_NeedResource[XGAME::xRES_SULFUR] = pData->s_NeedResource[XGAME::xRES_SULFUR];
	//m_Option.s_NeedResource[XGAME::xRES_MANDRAKE] = pData->s_NeedResource[XGAME::xRES_MANDRAKE];

	return TRUE;	
}

ID XGuild::FindidAccMemberByUserName( const _tstring& strUserName) const
{
	for( auto pMember : m_listMember ) {
		if( pMember->m_strName == strUserName )
			return pMember->m_idAcc;
	}
	return 0;
}

const XGuild::SGuildMember* XGuild::FindpMemberByidAcc(ID idAcc) const
{
	for( auto pMember : m_listMember ) {
		if( pMember->m_idAcc == idAcc )
			return pMember;
	}
	return nullptr;
}

XGuild::SGuildMember* XGuild::FindpMemberByidAccMutable( ID idAcc )
{
	for( auto pMember : m_listMember ) {
		if( pMember->m_idAcc == idAcc )
			return pMember;
	}
	return nullptr;
}


BOOL XGuild::AddGuildJoinReqMember(ID idaccount, _tstring  szUsername, int level)
{
// 	if (FindGuildMemberIdaccount(szUsername) > 0) // 이미 가입 되어 있는 유저.
// 		return FALSE;
	if (FindGuildJoinMemberIdaccount(szUsername) > 0) //이미 가입 요청 되어 있는 유저.
		return FALSE;

	SGuildMember* pData = new SGuildMember;
	pData->m_lvAcc = level;
	pData->m_Grade = XGAME::xtGuildGrade::xGGL_NONE;
	pData->m_idAcc = idaccount;
	//pData->s_strContext = szUserContext;
	pData->m_strName = szUsername;

	m_listJoinReqer.push_back(pData);

	return TRUE;
}
BOOL XGuild::RemoveJoinReqer( ID idAcc, xnGuild::xMember* pOut )
{
	auto itor = m_listJoinReqer.begin();
	for( ; itor != m_listJoinReqer.end(); itor++ ) {
		SGuildMember* pmember = (*itor);
		if( pmember && (pmember->m_idAcc == idAcc) ) {
			m_listJoinReqer.erase( itor );
			if( pOut )
				pmember->ToxMember( pOut );
			delete pmember;
			return TRUE;
		}
	}
	return FALSE;
}

/**
 @brief strUser를 가입신청목록에서 제거
*/
BOOL XGuild::RemoveJoinReqer( const _tstring& strUsername )
{
	auto itor = m_listJoinReqer.begin();
	for( ; itor != m_listJoinReqer.end(); itor++ ) {
		SGuildMember* pmember = (*itor);
		if( pmember && (pmember->m_strName == strUsername) ) {
			m_listJoinReqer.erase( itor );
			delete pmember;
			return TRUE;
		}
	}
	return FALSE;
}

/**
 @brief 이미 가입신청한 유저인가.
*/
BOOL XGuild::IsJoinReqerUser(ID idacc) const
{
	auto	itor = m_listJoinReqer.begin();
	for( ; itor != m_listJoinReqer.end(); itor++ ) {
		SGuildMember* pmember = (*itor);
		if( pmember && (pmember->m_idAcc == idacc) ) {
			return TRUE;
		}
	}
	return FALSE;
}
ID XGuild::FindGuildJoinMemberIdaccount(_tstring  szUserName)
{
	auto	itor = m_listJoinReqer.begin();
	for( ; itor != m_listJoinReqer.end(); itor++ ) {
		SGuildMember* pmember = (*itor);
		if( pmember && (pmember->m_strName == szUserName) ) {
			return pmember->m_idAcc;
		}
	}
	return 0;
}

XGuild::SGuildMember* XGuild::GetpReqerUserByidAcc( const _tstring& strUserName)
{
	auto	itor = m_listJoinReqer.begin();
	for (; itor != m_listJoinReqer.end(); itor++)
	{
		SGuildMember* pmember = (*itor);
		if (pmember && (pmember->m_strName == strUserName))
		{
			return pmember;
		}
	}
	return nullptr;
}

XGuild::SGuildMember* XGuild::GetpReqerUserByidAcc(ID idAcc)
{
	auto	itor = m_listJoinReqer.begin();
	for (; itor != m_listJoinReqer.end(); itor++)
	{
		SGuildMember* pmember = (*itor);
		if (pmember && (pmember->m_idAcc == idAcc))
		{
			return pmember;
		}
	}
	return nullptr;
}


void XGuild::SerializeGuildOption(XArchive& ar)
{	
	ar << (DWORD)m_Option.s_bAutoAccept;
	ar << (DWORD)m_Option.s_bBlockReqJoin;
}

void XGuild::DeSerializeGuildOption(XArchive& ar)
{
	DWORD dw1;	
	ar >> dw1;	m_Option.s_bAutoAccept = (dw1 != 0);
	ar >> dw1;	m_Option.s_bBlockReqJoin = (dw1 != 0);	
}

/**
 @brief 길드정보 요약본인가?
*/
void XGuild::SerializeGuildInfo(XArchive& ar)
{
	ar << m_Ver;
	ar << m_GuildIndex;			// Guild생성 번호.
	ar << m_CurrSN;				//길드내 사용 SN
	ar << m_idAccMaster;	//길드 Master Idaccount;
	ar << m_MaxMemberCount;
	ar << m_Level;
		
	ar << m_strName;			
	ar << m_strMasterName;		// 비밀번호
	XBREAK( m_strGuildContext.length() > 256 );
	ar << m_strGuildContext;
	ar << m_strBuildTime;			// 생성시간. 년-월-일-시-분

	ar << (int)m_listMember.size();
	ar << (int)m_listJoinReqer.size();	
}
void XGuild::DeSerializeGuildInfo(XArchive& ar)
{
	ar >> m_Ver;
	ar >> m_GuildIndex;
	ar >> m_CurrSN;
	ar >> m_idAccMaster;
	ar >> m_MaxMemberCount;
	ar >> m_Level;
	
	ar >> m_strName;				// 길드 이름
	ar >> m_strMasterName;		// 비밀번호
	ar >> m_strGuildContext;
	XBREAK( m_strGuildContext.length() > 256 );
	ar >> m_strBuildTime;		// 생성시간. 년-월-일-시-분

	ar >> m_MemberCount;	
	ar >> m_joinreqMemberCount;
}

/**
 @brief 길드의 풀정보 보냄.
*/
void XGuild::sSerialize( XGuild* pGuild, XArchive& ar )
{
	if( pGuild ) {
		ar << 1;
		pGuild->Serialize( ar );
	} else {
		ar << 0;
	}
}

XGuild* XGuild::sCreateDeserialize( XArchive& ar )
{
	int bCreate;
	ar >> bCreate;
	if( bCreate ) {
		auto pGuild = new XGuild();
		pGuild->DeSerialize( ar );
		return pGuild;
	} else {
		return nullptr;
	}
}

/**
 @brief 길드정보 간략화 버전인가?
*/
void XGuild::sSerializeGuildInfo( XGuild* pGuild, XArchive& ar )
{
	if( pGuild ) {
		ar << 1;
		pGuild->SerializeGuildInfo( ar );
	} else {
		ar << 0;
	}
}

XGuild* XGuild::sCreateDeserializeInfo( XArchive& ar )
{
	int bHave;
	ar >> bHave;
	if( bHave ) {
		auto pGuild = new XGuild();
		pGuild->DeSerializeGuildInfo( ar );
		return pGuild;
	} else {
		return nullptr;
	}
}

void XGuild::Serialize(XArchive& ar)
{
	int sizeOld = ar.size();
	ar << VER_GUILD_SERIALIZE;
	ar << m_GuildIndex;
	ar << m_CurrSN;
	ar << m_idAccMaster;
	ar << m_MaxMemberCount;
	ar << m_Level;

	SerializeGuildOption(ar);

	ar << m_strName;
	ar << m_strMasterName;
	XBREAK( m_strGuildContext.length() > 256 );
	ar << m_strGuildContext;
	ar << m_strBuildTime;

	ar << (int)m_listMember.size();
	
	auto itor = m_listMember.begin();	
	for (; itor != m_listMember.end(); itor++)
	{
		SGuildMember* pData = (*itor);
		if (XASSERT(pData))
		{
			// pData
			ar << pData->m_idAcc;
			ar << pData->m_lvAcc;
			ar << (ID)pData->m_Grade;
			ar << pData->m_strName;
//			ar << pData->s_strContext;			
		}
	}

	ar << (int)m_listJoinReqer.size();
	auto itor2 = m_listJoinReqer.begin();
	for (; itor2 != m_listJoinReqer.end(); itor2++)
	{
		SGuildMember* pData = (*itor2);
		if (XASSERT(pData))
		{
			// pData
			ar << pData->m_idAcc;
			ar << pData->m_lvAcc;
			ar << (ID)pData->m_Grade;
			ar << pData->m_strName;
//			ar << pData->s_strContext;			
		}
	}
	MAKE_CHECKSUM( ar );

// 	ar << (int)m_listRaid.size();
// 	for( auto spCampObj : m_listRaid ) {
// 		XArchive arCamp;
// 		XCampObj::sSerialize( spCampObj, arCamp );
// 		if( spCampObj != nullptr ) {
// 			ar << spCampObj->GetidProp();
// 			ar << arCamp;
// 		}
// 	}
	int size = ar.size() - sizeOld;
	XBREAK( size >= 8000 );
}

void XGuild::DeSerialize(XArchive& ar)
{
	DWORD dw1;
	DWORD dw2;
//	_tstring Buff;
	ar >> dw1;  m_Ver = dw1;
	ar >> dw1;	m_GuildIndex = dw1;
	ar >> dw1;	m_CurrSN = dw1;
	ar >> dw1;	m_idAccMaster = dw1;
	ar >> dw1;	m_MaxMemberCount = dw1;
	ar >> dw1;  m_Level = dw1;

	DeSerializeGuildOption(ar);

	ar >> m_strName;
	ar >> m_strMasterName;
	ar >> m_strGuildContext;
	XBREAK( m_strGuildContext.length() > 256 );
	ar >> m_strBuildTime;

	ar >> dw1;	m_MemberCount = dw1;
	
	for (int n = 0; n < (int)dw1; n++)
	{
		SGuildMember* pData = new SGuildMember;
		ar >> dw2;  pData->m_idAcc = dw2;
		ar >> dw2;  pData->m_lvAcc = dw2;
		ar >> dw2;	pData->m_Grade = (XGAME::xtGuildGrade)dw2;
		ar >> pData->m_strName;
		m_listMember.push_back(pData);
	}

	ar >> dw1; m_joinreqMemberCount = dw1;
	for (int n = 0; n < (int)dw1; n++)
	{
		SGuildMember* pData = new SGuildMember;		
		ar >> pData->m_idAcc;
		ar >> pData->m_lvAcc;
		ar >> dw2;	pData->m_Grade = (XGAME::xtGuildGrade)dw2;
		ar >> pData->m_strName;
//		ar >> Buff;	pData->s_strContext = Buff;
		m_listJoinReqer.push_back(pData);
	}
	if( m_Ver >= 2 ) {
		RESTORE_VERIFY_CHECKSUM_NO_RETURN( ar );
// 		int size;
// 		ar >> size;
// 		for( int i = 0; i < size; ++i ) {
// 			XArchive arCamp;
// 			ID idCamp;
// 			ar >> idCamp;
// 			ar >> arCamp;
// 			auto spCampObj = FindspRaidCampaign( idCamp );
// 			if( spCampObj == nullptr ) {
// 				auto pProp = XPropCamp::sGet()->GetpProp( idCamp );
// 				if( XASSERT( pProp ) ) {
// 					spCampObj = CampObjPtr( new XCampObj( pProp ) );
// 					m_listRaid.Add( spCampObj );
// 				}
// 			}
// 			XCampObj::sDeserializeUpdate( spCampObj, arCamp );
// 		}
	}
}

void XGuild::SerializeGuildMemberlist(XArchive& ar)
{
	ar << (int)m_listMember.size();
	auto	itor = m_listMember.begin();
	for (; itor != m_listMember.end(); itor++)
	{
		SGuildMember* pData = (*itor);
		if (pData)
		{			
			ar << pData->m_idAcc;
			ar << pData->m_lvAcc;
			ar << (ID)pData->m_Grade;
			ar << pData->m_strName;		
		}
	}
}
void XGuild::DeSerializeGuildMemberlist(XArchive& ar)
{
	DWORD dw1;
	DWORD dw2;
	if (m_listMember.size() > 0)
	{
		auto itor = m_listMember.begin();
		while (itor != m_listMember.end())
		{
			SGuildMember* pData = *itor;
			itor++;
			delete pData;
			pData = nullptr;
		}		
	}
	m_listMember.clear();
	if( ar.GetPacketLength() == 0 )
		return;
	ar >> dw1; m_MemberCount = dw1;
	for (int n = 0; n < (int)dw1; n++)
	{
		SGuildMember* pData = new SGuildMember;
		_tstring Buff;		
		ar >> dw2;  pData->m_idAcc = dw2;
		ar >> dw2;  pData->m_lvAcc = dw2;
		ar >> dw2;	pData->m_Grade = (XGAME::xtGuildGrade)dw2;
		ar >> Buff;
		pData->m_strName = Buff;
//		ar >> Buff;
//		pData->s_strContext = Buff;
		
		m_listMember.push_back(pData);
	}

}

/**
 @brief member의 정보로 해당하는 멤버의 정보를 업데이트 한다.
*/
void XGuild::UpdateWithxMember( const xnGuild::xMember& member )
{
	auto pMember = FindpMemberByidAccMutable( member.m_idAcc );
	if( pMember ) {
		pMember->UpdateWithxMember( member );
	}
}
/**
 @brief 가입신청자 목록을 날린다.
*/
void XGuild::DestroyJoinReqList()
{
	for( auto pJoiner : m_listJoinReqer ) {
		SAFE_DELETE( pJoiner );
	}
	m_listJoinReqer.clear();
}


void XGuild::SerializeGuildReqMemberlist(XArchive& ar)
{
	ar << (int)m_listJoinReqer.size();
	for( auto pUser : m_listJoinReqer ) {
		if( pUser ) {
			pUser->Serialize( ar );
		}
	}
}

/**
 @brief 아카이브에 담겨있는 가입신청차 리스트를 푼다.
*/
void XGuild::DeSerializeGuildReqMemberlist(XArchive& ar)
{
	// 기존 리스트를 먼저 날림
	DestroyJoinReqList();
	//
	if( ar.GetPacketLength() == 0 )
		return;
	DWORD dw1;
	ar >> dw1;	m_joinreqMemberCount = dw1;
	for( int n = 0; n < (int)dw1; n++ ) {
		auto pData = new SGuildMember;
		pData->DeSerialize( ar, 0 );
		m_listJoinReqer.push_back( pData );
	}
}

void XGuild::SetGuildOption(bool bAutoAccept, bool bBlockAccept)
{
	m_Option.s_bAutoAccept = bAutoAccept;
	m_Option.s_bBlockReqJoin = bBlockAccept;	
}

/**
 @brief 길드 레이드 캠페인 인스턴스를 얻는다. 없을경우 생성한다.
*/
xCampaign::CampObjPtr XGuild::GetAndCreateRaidCampaign( LPCTSTR idsCamp )
{
	auto pProp = XPropCamp::sGet()->GetpProp( idsCamp );
	if( XBREAK( pProp == nullptr ) )
		return xCampaign::CampObjPtr();
	return GetAndCreateRaidCampaign( pProp->idProp );
}

/**
 @brief 길드 레이드 캠페인 인스턴스를 얻는다. 없을경우 생성한다.
*/
xCampaign::CampObjPtr XGuild::GetAndCreateRaidCampaign( ID idCamp )
{
	auto spCampObj = FindspRaidCampaign( idCamp );
	if( spCampObj != nullptr )
		return spCampObj;
	// 없을경우 새로 생성
	auto pProp = XPropCamp::sGet()->GetpProp( idCamp );
	if( XASSERT( pProp ) ) {
		spCampObj = xCampaign::XCampObj::sCreateCampObj( pProp, 0 );
		m_listRaid.Add( spCampObj );
		return spCampObj;
	}
	return xCampaign::CampObjPtr();
}

/**
 @brief 찾기만 한다.
*/
xCampaign::CampObjPtr XGuild::FindspRaidCampaign( ID idCamp )
{
	for( auto spCampObj : m_listRaid ) {
		XBREAK( spCampObj == nullptr );
		if( spCampObj->GetpProp()->idProp == idCamp )
			return spCampObj;
	}
	return xCampaign::CampObjPtr();
}

/**
 @brief arCamp에 있는 캠페인정보를 업데이트한다.
 캠페인이 없었다면 새로 생성하고 있었다면 그곳에 업데이트 한다.
*/
int XGuild::DeSerializeUpdateGuildRaidCamp( XArchive& arCamp, ID idCamp )
{
	auto spCampObj = FindspRaidCampaign( idCamp );
	if( spCampObj == nullptr ) {
		spCampObj = xCampaign::XCampObj::sCreateDeserialize( arCamp );
		m_listRaid.Add( spCampObj );
	} else {
		XCampObj::sDeserializeUpdate( spCampObj, arCamp );
	}
	return 1;
}

/**
 @brief 길드멤버 idAcc의 길드내 등급을 얻는다.
*/
xtGuildGrade XGuild::GetGradeByMember( ID idAcc )
{
	auto pMember = FindpMemberByidAcc( idAcc );
	if( pMember ) {
		return pMember->m_Grade;
	}
	return xGGL_NONE;
}