#include "stdafx.h"
#include "XAccount.h"
#include "VerPacket.h"
#include "XWorld.h"
#include "XHero.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XBaseItem.h"
#ifdef _XSINGLE
#include "client/XAppMain.h"
#endif
#include "XPropItem.h"
#include "XPropUnit.h"
#include "XPropHero.h"
#include "XPropSquad.h"
#if defined(_CLIENT) || defined(_GAME_SERVER)
#include "XPropTech.h"
#include "XPropUser.h"
#endif // _GAME_SERVER
#include "XSpot.h"
#include "XQuestMng.h"
#include "XSkillMng.h"
#include "XGuild.h"
#include "XExpTableHero.h"
#include "etc/Token.h"
#if defined(_CLIENT) || defined(_GAME_SERVER) || defined(_DB_SERVER)
#include "server/XGuildMgr.h"
#endif  // defined(_CLIENT) || defined(_GAME_SERVER) || defined(_DB_SERVER)
#include "XGlobalConst.h"
#include "XDefNetwork.h"

using namespace XGAME;
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int XAccount::s_secOfflineForSimul = xHOUR( 1 );		// 스팟침공 시뮬레이션시 오프라인 시간.
// int XAccount::s_bTraded = false;						// 무역상거래를 하면 true가 된다.
int XAccount::s_bTraderArrived = false;						// 무역상이 도착함.
#ifdef _CLIENT
XSPAcc XAccount::s_spInstance;
#endif // _CLIENT

//////////////////////////////////////////////////////////////////////////
// static
/**
@brief pProp의 아이템 인스턴스를 하나 만든다.
*/
XBaseItem* XAccount::sCreateItem( const XPropItem::xPROP *pProp, int num)
{
	auto pItem = new XBaseItem(pProp);
	pItem->SetNum(num);
	return pItem;
}

XBaseItem* XAccount::sCreateItem(ID idItem, int num)
{
	auto pProp = PROP_ITEM->GetpProp(idItem);
	if (XBREAK(pProp == nullptr))
		return nullptr;
	return XAccount::sCreateItem(pProp, num);
}

int XGAME::xBuff::Serialize( XArchive& ar ) {
	ar << sid;
	ar << strIcon;
	ar << idBuff;
	ar << idName;
	ar << idDesc;
	timer.Serialize( ar );
	return 1;
}
int XGAME::xBuff::DeSerialize( XArchive& ar, int ver ) {
	ar >> sid;
	ar >> strIcon;
	ar >> idBuff;
	ar >> idName;
	ar >> idDesc;
	timer.DeSerialize( ar, 1 );
	return 1;
}

void XAccount::sSerializeGuildJoinReq( XSPAcc spAcc, XArchive* pOut )
{
	(*pOut) << spAcc->m_listGuildJoinReq;
}

void XAccount::sDeSerializeGuildJoinReq( XArchive& ar, XSPAcc spAcc )
{
	ar >> spAcc->m_listGuildJoinReq;
}

/**
 @brief XAcc의 길드정보 전체
*/
void XAccount::sSerializeGuildInfo( XSPAcc spAcc, XArchive* pOut )
{
	const ID idGuild = spAcc->m_GuildIndex;
	XBREAK( idGuild > 0xffff );
	(*pOut) << (WORD)idGuild;
	(*pOut) << (BYTE)spAcc->m_Guildgrade;
	(*pOut) << (BYTE)0;
	(*pOut) << spAcc->m_listGuildJoinReq;
}

void XAccount::sSerializeGuildInfo( ID idGuild, xtGuildGrade grade, const XList4<ID>& listJoinReq, XArchive* pOut )
{
	XBREAK( idGuild > 0xffff );
	(*pOut) << (WORD)idGuild;
	(*pOut) << (BYTE)grade;
	(*pOut) << (BYTE)0;
	(*pOut) << listJoinReq;
}

void XAccount::sDeSerializeGuildInfo( XArchive& ar, XSPAcc spAcc )
{
	WORD w0;
	BYTE b0;
	ar >> w0;	spAcc->m_GuildIndex = w0;
	ar >> b0;	spAcc->m_Guildgrade = (xtGuildGrade)b0;
	ar >> b0;
	ar >> spAcc->m_listGuildJoinReq;
}

void XAccount::sUpdateByGuildEvent( XSPAcc spAcc, xtGuildEvent event, const XGuild* pGuild, XArchive& arParam )
{
	const ID idGuild = pGuild->GetidGuild();
	switch( event ) {
	case xGEV_JOIN_ACCEPT: {
		xnGuild::xMember member;
		ID idAccConfirm;
		arParam >> member;
		arParam >> idAccConfirm;
		spAcc->SetGuild( idGuild, member.m_Grade );
	} break;
	case xGEV_JOIN_REJECT: {
		ID idGuild, idAccTarget, idAccConfirm;
		_tstring strGuildName;
		arParam >> idGuild >> idAccTarget >> strGuildName >> idAccConfirm;
		spAcc->DelGuildJoinReq( idGuild );
	} break;
	case xGEV_CHANGE_MEMBER_INFO: {
		xnGuild::xMember memberConfirm;
		xnGuild::xMember memberTarget;
		arParam >> memberConfirm >> memberTarget;
		spAcc->SetGuildgrade( memberTarget.m_Grade );
	} break;
		// idAcc유저가 길드에서 쫒겨남.
	case xGEV_KICK: {
		ID idAccKicker, idAccTarget;
		_tstring strTarget;
		arParam >> idAccKicker;		// 누구에게 쫒겨난건가.
		arParam >> idAccTarget;		// 누가 쫒겨난건가.
		arParam >> strTarget;			// 쫒겨난 유저 이름.
		spAcc->ClearGuild();
		spAcc->ClearlistGuildJoinReq();
	} break;
	case xGEV_OUT: {
		ID idAccTarget;
		_tstring strName;
		arParam >> idAccTarget;
		arParam >> strName;
		spAcc->ClearGuild();
		spAcc->ClearlistGuildJoinReq();
	} break;
	default:
		XBREAK( 1 );
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
XAccount::XAccount()
	: m_aryResource(XGAME::xRES_MAX)
	, m_aryLegion( XGAME::MAX_LEGION )
{
	Init();
	CreateQuestMng();
// 	m_xOption.Load();
#ifdef _CLIENT
	CONSOLE("create account: %s", XE::GetTimeString());
#endif
}

XAccount::XAccount(ID idAccont)
	: m_aryResource( XGAME::xRES_MAX )
	, m_aryLegion( XGAME::MAX_LEGION )
{
	Init();
	m_idAccount = idAccont;
	CreateQuestMng();
// 	m_xOption.Load();
}
XAccount::XAccount(ID idAccount, LPCTSTR szID)
	: m_aryResource( XGAME::xRES_MAX )
	, m_aryLegion( XGAME::MAX_LEGION )
{
	Init();
	m_idAccount = idAccount;
	m_strID = szID;
	CreateQuestMng();
// 	m_xOption.Load();
}
XAccount::XAccount( ID idAcc, const _tstring& strUUID )
	: m_aryResource( XGAME::xRES_MAX )
	, m_aryLegion( XGAME::MAX_LEGION )
	, m_strUUID(strUUID)
{
	Init();
	m_idAccount = idAcc;
	CreateQuestMng();
// 	m_xOption.Load();
}

void XAccount::Destroy(void)
{
	SAFE_DELETE(m_pQuestMng);
	SAFE_DELETE(m_pWorld);
	//	XARRAYN_DESTROY( m_aryLegion );
	DestroyHeros();
	XLIST4_DESTROY(m_listItem);
	DestroyPostInfoAll();
}

void XAccount::DestroyHeros(void)
{
//	XLIST4_DESTROY(m_listHero);
}

int XAccount::Serialize(XArchive& ar)
{
	XBREAK( ar.GetMaxSizeArchive() < 30000 );	// 계정 아카이브는 큰걸 써야함.
	//	m_secSaved = 0;
	//	XBREAK( ar.GetMaxBufferSize() < 0x4000 );
	XDBAccount::Serialize(ar);
	//
	ar << VER_SERIALIZE;
	ar << m_strID.c_str();
	ar << m_strPassword.c_str();
	ar << m_strName.c_str();
	ar << m_strFbUserId;
	ar << m_strUUID.c_str();
	ar << m_StrSessionKey.c_str();
	ar << m_strHello;
	ar << m_CurrSN;
	m_Level.Serialize(ar);
	XBREAK( m_Gold > 0x7fffffff );
	ar << m_Gold;
	ar << m_Cashtem;
//	ar << m_maxItems;
	ar << (short)m_maxItems;
	ar << (char)m_GMLevel;
	ar << (char)0;
	ar << m_aryResource;
	//   DWORD secSaved = 0;
	ar << m_Ladder;     // UserDB에서 사용함.
	ar << m_PowerIncludeEmpty;		// 기존의 m_Power
	ar << m_PowerExcludeEmpty;	// 빈슬롯을 포함하지 않는값으로 바뀜
	UpdatePlayTimer();
	ar << m_secPlay;
	ar << m_timerTrader;
	ar << m_timerShop;
	ar << m_GuildIndex;
	ar << (int)m_Guildgrade;

	SerializeShopList(ar);

	SerializeAbil(ar);
	// 기타 잡스러운 데이타를 DB에 바이너리로 저장한다.
	if (SerializeEtcData(ar) == 0)
		return 0;
	// 아이템 인벤
	int sizeItems = SerializeItems(ar);
	XBREAK(sizeItems >= 16000);
	int size = 0;
	// 영웅 리스트
	SerializeHeros( ar );
	MAKE_CHECKSUM(ar);

	SerializeJoinReqGuild(ar);
	MAKE_CHECKSUM(ar);

	SerializeSubscribe(ar);
	MAKE_CHECKSUM(ar);
	//
	ar << (BYTE)VER_LEGION_SERIALIZE;
	ar << (BYTE)0;
	ar << (WORD)m_aryLegion.Size();
//	XARRAYN_LOOP_IDX(m_aryLegion, LegionPtr&, i, pLegion) {
	for( auto spLegion : m_aryLegion ) {
		if (spLegion) {
			ar << 1;
			spLegion->Serialize(ar);
		} else {
			ar << 0;
		}
	}// END_LOOP;
	MAKE_CHECKSUM(ar);
	//
	XBREAK(m_pWorld == nullptr);
	int sizeWorld = m_pWorld->Serialize(ar);
	// 현재 DB에 8000바이트씩 두개로 쪼개서 저장하므로 이것보다 커지면 안됨.
	XBREAK(sizeWorld >= 32000);		// 어차피 압축했을때 16k만 안넘어가면 되므로 상관없지만 경고성 차원에서 넣음.
	MAKE_CHECKSUM(ar);
	if (SerializeQuest(ar) == 0)
		return 0;
	SerializeAttackLog( ar );
	SerializeDefenseLog( ar );
	SerializeEncounter( ar );
	return 1;
}

int XAccount::DeSerialize( XArchive& ar )
{
	XDBAccount::DeSerialize( ar );
	//	CONSOLE("Deserialize:idAcc=%d", GetidAccount());
//	TCHAR szBuff[ 1024 ];
	WORD w0;
	BYTE b0;
	char c0;
	short s0;
	//
	ar >> m_Ver;
	ar >> m_strID;
	ar >> m_strPassword;
	ar >> m_strName;
	ar >> m_strFbUserId;
	ar >> m_strUUID;
	ar >> m_StrSessionKey;
	ar >> m_strHello;
	ar >> m_CurrSN;
	m_Level.DeSerialize( ar );
	if( m_Level.GetlvLimit() < 5 )
		m_Level.SetlvLimit( 5 );
	ar >> m_Gold;
	ar >> m_Cashtem;
//	ar >> m_maxItems;
	ar >> s0;		m_maxItems = s0;
	ar >> c0;		m_GMLevel = c0;
	ar >> c0;
	ar >> m_aryResource;
	ar >> m_Ladder;
	ar >> m_PowerIncludeEmpty;
	ar >> m_PowerExcludeEmpty;
	{
		DWORD secPlay;
		ar >> secPlay;
		UpdatePlayTimer( secPlay );
	}
	ar >> m_timerTrader;
	if( m_timerTrader.IsOn() && m_timerTrader.GetsecWait() == 0 ) {
		m_timerTrader.Set(1);		// 구버전은 여기에 값이 없었기땜에 1을 넣어줌.
	}
	ar >> m_timerShop;
	int grade = 0;
	ar >> m_GuildIndex;
	ar >> grade; m_Guildgrade = ( XGAME::xtGuildGrade )grade;
	DeSerializeShopList( ar );
	DeserializeAbil( ar );
	DeSerializeEtcData( ar );
	DeSerializeItems( ar );
	DeSerializeHeros( ar );
	RESTORE_VERIFY_CHECKSUM( ar );
	DeSerializeJoinReqGuild( ar );
	RESTORE_VERIFY_CHECKSUM( ar );
	DeSerializeSubscribe( ar );
	RESTORE_VERIFY_CHECKSUM( ar );
	int verLegion = 0;
	ar >> b0;	verLegion = b0;
	ar >> b0;
	ar >> w0;	const int size = w0;
	XBREAK( m_aryLegion.Size() != size );
	for( int i = 0; i < size; ++i ) {
		int fill;
		ar >> fill;
		if( fill ) {
			XBREAK( fill != 1 );
			XLegion *pLegion = new XLegion;
			XBREAK( pLegion == nullptr );
			pLegion->DeSerialize( ar, GetThis(), verLegion );
			m_aryLegion[ i ] = XSPLegion( pLegion );
		}
	}
	RESTORE_VERIFY_CHECKSUM( ar );
	XBREAK( m_pWorld != nullptr );
	SAFE_DELETE( m_pWorld );
	m_pWorld = new XWorld;
	m_pWorld->DeSerialize( ar );
	m_pWorld->OnCreateSpots( GetThis() );
	// secLastCalc는 pWorld로 옮겨질예정
	//	XBREAK( m_pWorld->GetsecLastCalc() == 0 );
	//	if( m_pWorld->GetsecLastCalc() == 0 )
	//		m_pWorld->SetsecLastCalc( m_tickLastCalc );
	RESTORE_VERIFY_CHECKSUM( ar );
	DeSerializeQuest( ar );
	DeSerializeAttackLog( ar );
	DeSerializeDefenseLog( ar );
	DeSerializeEncounter( ar );
	if( m_BattleSession.idSpot ) {
		// 전투중인 스팟의 군단이 있었으면 그걸로 복원시킴.
		XBREAK( m_pWorld == nullptr );
		if( m_StrSessionKey.empty() ) {
			// 처음부터 로그인이면 전투세션 보관할 필요없으니 있어도 삭제.
			m_BattleSession.Clear();
		} else {
			auto pSpot = m_pWorld->GetSpot( m_BattleSession.idSpot );
			if( pSpot ) {
				// 세션키 로그인이면 전투중에 잠시 끊긴거라 판단하고 스팟에 군단 세팅
				pSpot->SetspLegion( m_BattleSession.spEnemy );
			} else
				m_BattleSession.Clear();	// 전투중에 끊겼는데 그사이에 패치가 되어 스팟이 삭제되거나 하면 생길수 있음.
		}
	} else {
		m_BattleSession.Clear(); // 혹시 잡데이타가 들어있을지도 몰라서.
	}
	return 1;
}

void XAccount::SerializeHeros( XArchive& ar ) const
{
	int size = m_listHero.size();
	ar << (char)VER_HERO_SERIALIZE;
	ar << (char)0;
	ar << (WORD)size;
	int _sizeAr = ar.size();
	for( auto pHero : m_listHero ) {
		int sizeAr = pHero->Serialize( ar );
//		sizeAr = 0;
		XBREAK( sizeAr >= 250 );		// 용량 경고
	}
	int sizeAr = ar.size() - _sizeAr;
	XBREAK( sizeAr >= 8000 );		// 용량경고
}

bool XAccount::DeSerializeHeros( XArchive& ar )
{
	char c0;
	short w0;
	ar >> c0;		const int verHero = c0;
	ar >> c0;
	ar >> w0;		const int size = w0;
	for( int i = 0; i < size; ++i ) {
		auto pHero = std::make_shared<XHero>( GetThis() );
		pHero->DeSerialize( ar, /*GetThis(), */verHero );
		pHero->SetidPropToEquip( GetThis() );
		m_listHero.Add( pHero );
	}
	return true;
}


int XAccount::SerializeQuest(XArchive& ar)
{
	XBREAK(m_pQuestMng == nullptr);
	int sizeQuest = m_pQuestMng->Serialize(ar);
	if (XBREAK(sizeQuest >= 8000))
		return 0;
	MAKE_CHECKSUM(ar);
	return 1;
}
int XAccount::DeSerializeQuest(XArchive& ar)
{
	if (XBREAK(m_pQuestMng == nullptr))
		return 0;
	m_pQuestMng->DeSerialize(ar);
	RESTORE_VERIFY_CHECKSUM(ar);
	return 1;
}


int XAccount::SerializeResource(XArchive& ar)
{
	ar << m_aryResource;
	return 1;
}
int XAccount::DeSerializeResource( XArchive& ar )
{
	ar >> m_aryResource;
	return 1;
}
int XAccount::SerializeResource2( XArchive& ar )
{
	XAccount::sSerializeResource( ar, m_aryResource );
	return 1;
}
int XAccount::DeSerializeResource2( XArchive& ar )
{
	XAccount::sDeserializeResource2( ar, m_aryResource );
	return 1;
}

int XAccount::SerializeShopList(XArchive& ar)
{
	std::vector<ID>::iterator begin = m_listShopSell.begin();
	std::vector<ID>::iterator end = m_listShopSell.end();
	int nCount = 0;

	ar << (int)(m_listShopSell.size());
	for (; begin != end; ++begin) {
		ID iditemprop = *begin;
		if (iditemprop != 0) {
			ar << iditemprop;
			nCount++;
		}
	}
	XBREAK(m_listShopSell.size() != nCount);
	return nCount;
}

int XAccount::DeSerializeShopList(XArchive& ar)
{
	int nCount = 0;
	ID idItemProp = 0;
	ar >> nCount;

	m_listShopSell.clear();
	if (nCount == 0)return 0;
	for (int n = 0; n < nCount; n++) {
		ar >> idItemProp;
		m_listShopSell.push_back(idItemProp);
	}

	return (int)m_listShopSell.size();
}

int XAccount::SerializeSubscribe(XArchive& ar)
{
	XBREAK( m_numSubscribe > 0xffff );
	XBREAK( m_cntSubscribeOffline > 0xffff );
	ar << (short)m_numSubscribe;
	ar << (short)m_cntSubscribeOffline;
	ar << m_secNextSubscribe;
	// 24시간 타이머의 남은 시간을 저장
// 	m_secSubscribe = int(GetSubscribeTimer()->GetRemainSec());
// 	// 30일 타이머의 남은 시간을 저장.
// 	m_secSubscribeEnd = int(GetSubscribeEndTimer()->GetRemainSec());
// 	ar << m_secSubscribe;
// 	ar << m_secSubscribeEnd;
// 	ar << m_cntSubscribeOffline;
	return 1;
}
int XAccount::DeSerializeSubscribe(XArchive& ar)
{
	short s0;
	ar >> s0;		m_numSubscribe = s0;
	ar >> s0;		m_cntSubscribeOffline = s0;
	ar >> m_secNextSubscribe;
	XBREAK( m_numSubscribe < 0 );
	// 월정액상품 수령중인데 다음 지급시간이 없으면 에러
	if( XBREAK( m_numSubscribe > 0 && m_secNextSubscribe == 0 ) ) {
		// 에러 수정
		const int secInterval = XGC->m_secSubscribeCycle;
		m_secNextSubscribe = XTimer2::sGetTime() + secInterval;
	}
// 	DWORD dw1;
// 	ar >> dw1;
// 	if (dw1 > 0) {
// 		SetsecSubscribe(dw1);
// 		GetSubscribeTimer()->Set((float)m_secSubscribe);
// 	}
// 	ar >> dw1;
// 	if (dw1 > 0) {
// 		SetsecSubscribeEnd(dw1);
// 		GetSubscribeEndTimer()->Set((float)m_secSubscribeEnd);
// 	}
// 	ar >> m_cntSubscribeOffline;
	return 1;
}

int XAccount::SerializeJoinReqGuild(XArchive& ar)
{
	ar << m_listGuildJoinReq;
	return 1;
}

int XAccount::DeSerializeJoinReqGuild(XArchive& ar)
{
	ar >> m_listGuildJoinReq;
	return 1;
}

int XAccount::SerializeEtcData(XArchive& ar)
{
	int sizeOld = ar.size();
	ar << VER_ETC_SERIALIZE;
	XBREAK( GetsecLastSaved() > 0xffffffff );
	ar << (DWORD)GetsecLastSaved();
// 	ar << m_secSpecialSpotReleased;
// 	ar << m__dayWaitSpecialSpot;
	ar << 0;
	ar << 0;
	XBREAK( m_numStar > 0xffff );
	if( XBREAK( m_numHirePremium > 0xffff ) )
		m_numHirePremium = 0xffff;
	ar << (short)m_numStar;
	ar << (WORD)m_numHirePremium;
	ar << m_ptGuild;
	SerializeTrainSlot(ar);
	XBREAK(m_AP > 0xffff);
	ar << (WORD)m_AP;
	XBREAK(m_maxAP == 0);
	ar << (WORD)m_maxAP;
	if (m_timerAP.IsOff())
		m_timerAP.DoStart();
	m_timerAP.Serialize(ar);
	DWORD dw0 = *((DWORD*)(&m_bitFlagTutorial));
	ar << dw0; 
	ar << m_aryUnlockedUnit;
	SerializeUnlockPoint( ar );
	m_BattleSession.Serialize( ar );
	ar << m_numBattleByPvp;
	ar << m_numWinsByPvp;
	ar << m_bitUnlockMenu;
	int size = ar.size() - sizeOld;
	SerializeSeq( ar );
	SerializeNumStarSpot( ar );
	ar << m_vFocusWorld;
	ar << m_numBuyCash;
	ar << m_amountBuyCash;
	XBREAK( size >= 8000 );
	return 1;
}

int XAccount::DeSerializeEtcData(XArchive& ar)
{
	int verEtc;
	int dw0;
	short s0;
	ar >> verEtc;
	if( verEtc >= 19 ) {
		ar >> dw0;		m_secLastSaved = dw0;
	}
	ar >> dw0; // m_secSpecialSpotReleased;
	ar >> dw0; // m__dayWaitSpecialSpot;
//	DeSerializeTechPoint(ar);
	if( verEtc < 17 ) {
		// 기존 데이타를 읽기위한 더미배열
		XArrayLinearN<char, XGAME::xUNIT_MAX> aryTechPoint;
		ar >> aryTechPoint;
	}
//	ar >> m_ptBrave;
	ar >> s0;	m_numStar = s0;
	ar >> s0;
	ar >> m_ptGuild;
	DeSerializeTrainSlot(ar, verEtc);
	WORD w0;
	ar >> w0;	m_AP = w0;
	ar >> w0;	m_maxAP = w0;
	m_timerAP.DeSerialize(ar, verEtc);
#ifdef _CLIENT
	XBREAK( m_timerAP.GetsecPass() > 0 );	// 로그인후 클라로 보내주기전에 타이머를 리셋시켜 보내기때문에 이게 0이상인경우는 없어야 함.
#endif // _CLIENT
	ar >> dw0;	*((DWORD*)(&m_bitFlagTutorial)) = dw0;
	ar >> m_aryUnlockedUnit;
	// db 클리어하면 빼도 됨.
	m_aryUnlockedUnit[ XGAME::xUNIT_SPEARMAN ] = 2;
	m_aryUnlockedUnit[ XGAME::xUNIT_ARCHER ] = 2;
	DeSerializeUnlockPoint( ar, verEtc );
		// 배틀세션을 읽음
		m_BattleSession.DeSerialize( ar, verEtc );
		ar >> m_numBattleByPvp;
		ar >> m_numWinsByPvp;
	ar >> m_bitUnlockMenu;
	DeserializeSeq( ar, verEtc );
	if( verEtc >= 18 )
		DeSerializeNumStarSpot( ar, verEtc );
	if( verEtc >= 20 )
		ar >> m_vFocusWorld;
	if( verEtc >= 21 ) {
		ar >> m_numBuyCash;
		ar >> m_amountBuyCash;
	}
	return 1;
}

int XAccount::SerializeNumStarSpot( XArchive& ar )
{
	int size = m_mapSpotStar.size();
	ar << size;
	for( auto& itor : m_mapSpotStar ) {
		XBREAK( itor.first > 0xffff );
		ar << (WORD)itor.first;
		ar << (char)itor.second;
		ar << (BYTE)0;
	}
// 나중에 최적화 할것.
// 	DWORD dw0 = 0;
// 	int cnt = 0;
// 	for( auto& itor : m_mapSpotStar ) {
// 		dw0 << 8;
// 		dw0 |= (BYTE)(itor.second);
// 	}
	return 1;
}
int XAccount::DeSerializeNumStarSpot( XArchive& ar, int verEtc )
{
	m_mapSpotStar.clear();
	int size;
	ar >> size;
	for( int i = 0; i < size; ++i ) {
		WORD w0;
		char c0;
		ID idSpot;
		int numStar;
		ar >> w0;	idSpot = w0;;
		ar >> c0;	numStar = c0;;
		ar >> c0;
		m_mapSpotStar[ idSpot ] = numStar;
	}
	return 1;
}

int XAccount::SerializeUnlockPoint( XArchive& ar )
{
	ar << (BYTE)xboolToByte( m_bUnlockTicketForPaladin );
	XBREAK( m_numUnlockTicketForMiddleOrBig > 0xff );
	ar << (BYTE)m_numUnlockTicketForMiddleOrBig;
	ar << (BYTE)0;
	ar << (BYTE)0;
	return 1;
}
/**
 @brief verEtc가 0일경우는 버전체크를 무조건 통과한다.
*/
int XAccount::DeSerializeUnlockPoint( XArchive& ar, int verEtc )
{
	if( verEtc == 0 )
		verEtc = 9999;
	BYTE b0;
	ar >> b0;	m_bUnlockTicketForPaladin = xbyteToBool( b0 );
	ar >> b0;	m_numUnlockTicketForMiddleOrBig = b0;
	ar >> b0 >> b0;
	return 1;
}

int XAccount::SerializeTrainSlot(XArchive& ar)
{
	XBREAK(m_numFreeSlot > 0xff);
	ar << (BYTE)m_numFreeSlot;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << m_listTrainSlot;
	return 1;
}
int XAccount::DeSerializeTrainSlot(XArchive& ar, int verEtc)
{
	BYTE b0;
	ar >> b0;	m_numFreeSlot = b0;
	ar >> b0 >> b0 >> b0;
	ar >> m_listTrainSlot;
	return 1;
}

/**
@brief 인벤토리의 아이템들을 아카이빙한다.
만약 전체 아이템 바이너리 크기가 8000바이트를 넘는다면 8000단위로 쪼개서 저장하도록 한다.
*/
int XAccount::SerializeItems(XArchive& ar)
{
	int sizeAr = ar.size();
	ar << (int)VER_ITEM_SERIALIZE;
	int size = m_listItem.size();
	ar << size;
	for (auto pBaseItem : m_listItem)
	{
		XBaseItem::sSerialize(ar, pBaseItem);
		MAKE_CHECKSUM(ar);
	}
	//
	MAKE_CHECKSUM(ar);
	return ar.size() - sizeAr;
}

/**
@brief 아카이브로부터 아이템을 생성해 인벤에 추가한다.
*/
int XAccount::DeSerializeItems(XArchive& ar)
{
	int ver, size;
	ar >> ver;
	ar >> size;
	for (int i = 0; i < size; ++i)
	{
		auto pBaseItem = DeserializeAddItem(ar, ver);
		RESTORE_VERIFY_CHECKSUM(ar);
		// 		XBaseItem *pBaseItem = XBaseItem::sCreateDeSerialize( ar, ver );
		// 		AddItem( pBaseItem );
	}
	RESTORE_VERIFY_CHECKSUM(ar);
	return 1;
}


/**
@brief 모든 아이템을 새로 생성하지 않고 이미 같은 아이템이 있다면 값만 바꾸는 식으로 한다.
만약 Deserialize받은 목록에 없는 아이템이 인벤에 있다면 삭제시킨다.
*/
int XAccount::DeSerializeItems2(XArchive& ar)
{
	int ver, size;
	ar >> ver;
	ar >> size;
	XArrayLinearN<ID, 2048> aryNewID;		// 아카이브에 있던 최신 아이템 목록
	for (int i = 0; i < size; ++i)
	{
		// 일단 헤더만 읽어본다.
		ID idProp, snItem;
		int type;
		ar >> type;		// 장차 sDeSerializeHeader로 들어가야 한다.
		XBaseItem::sDeSerializeHeader(ar, &idProp, &snItem, ver);
		// 같은 번호의 아이템이 있는지 확인한다.
		auto pExist = GetItem(snItem);
		if (pExist == nullptr)
		{
			pExist = new XBaseItem;
			AddItem(pExist);
		}
		XBaseItem::sDeSerializeNoHeader(ar, pExist, idProp, snItem, ver);
		if (XBREAK(pExist->GetpProp() == nullptr))
			return 0;
		if (XBREAK(pExist->GetpProp()->type != type))
			return 0;
		aryNewID.Add(pExist->getid());
		RESTORE_VERIFY_CHECKSUM(ar);
	}
	// 새 아이템 목록에 없는 아이템을 인벤에서 지운다.

	for (auto itor = m_listItem.begin(); itor != m_listItem.end();)
	{
		// 이 아이템이 최신버전 리스트에 없으면 지운다
		auto pItem = (*itor);
		auto idItem = pItem->GetsnItem();
		if (!aryNewID.Find(idItem))
		{
			m_listItem.erase(itor++);
			// 만약 영웅이 갖고 있다면 장착해제시킨다.
			if (pItem->GetType() == XGAME::xIT_EQUIP)
				UpdateNewItemForHeros(pItem->GetsnItem(), nullptr);
			SAFE_DELETE(pItem);
		}
		else
			++itor;
	}
	return 1;
}

int XAccount::SerializeEncounter( XArchive& ar ) 
{
	ar << (BYTE)VER_ENCOUNTER;
	int size = m_aryEncounter.size();
	if( XBREAK( size > 16 ) ) {
		size = 16;
		m_aryEncounter.resize( 16 );
	}
	ar << (BYTE)size;
	ar << (BYTE)0;
	ar << (BYTE)0;
	for( auto& enc : m_aryEncounter ) {
		ar << enc;
	}
	return 1;
}
int XAccount::DeSerializeEncounter( XArchive& ar )
{
	m_aryEncounter.clear();
	BYTE b0;
	ar >> b0;	int verEnc = b0;
	ar >> b0;	int size = b0;
	ar >> b0 >> b0;
	for( int i = 0; i < size; ++i ) {
		XGAME::xEncounter enc;
		ar >> enc;
		m_aryEncounter.Add( enc );
	}
	return 1;
}

bool XAccount::AddEncounter( const XGAME::xEncounter& enc )
{
	if( m_aryEncounter.size() < 16 ) {
		if( XASSERT(enc.IsValid()) )
			m_aryEncounter.Add( enc );
		return true;
	}
	return false;
}
/**
 @brief 유황기습으로 얻은 자원을 스팟에 쌓는다.
*/
void XAccount::DoStackEncounter( xSec secOffline )
{
#ifdef _DEV
	if( m_aryEncounter.size() == 0 ) {
		if( secOffline > xHOUR(1) ) {
			// 가상으로 유황을 얻은것처럼 처리한다.
			XVector<XSpotSulfur*> ary;
			m_pWorld->GetSpotsToAry( &ary, XGAME::xSPOT_SULFUR );
			if( ary.size() > 0 ) {
				int dice = xRandom( 2 );
				if( dice == 0 ) {
					int numSpot = xRandom( ary.size() ) + 1;
					//
					for( auto pSpot : ary ) {
						if( pSpot->IsOpenedSpot( m_pWorld ) ) {
							int dice = xRandom(3);
							if( dice == 0 ) {
								xSec sec = XTimer2::sGetTime() - xRandom(600, 3600);
								int numSulfur = pSpot->GetnumSulfur();
								if( numSulfur == 0 )
									numSulfur = xRandom( 10000, 20000 );
								XGAME::xEncounter enc( pSpot->GetidSpot()
																		, 12345678, numSulfur, sec, _T("TEST_USER") );
								AddEncounter( enc );
								if( --numSpot <= 0 )
									break;
							}
						}
					}
				}
			}
		}
	}
#endif // _DEV
	if( m_aryEncounter.size() == 0 )
		return;
	for( auto& enc : m_aryEncounter ) {
		auto pBaseSpot = m_pWorld->GetSpot( enc.m_idSpot );
		// 아직 이 스팟을 안열었으면 없을수도 있음.
		if( pBaseSpot && pBaseSpot->IsOpenedSpot( m_pWorld ) ) {	
			auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
			if( XASSERT(pSpot) ) {		
				// 해당 유황스팟에 쌓는다.
				pSpot->DoStackToLocal( (float)enc.m_numSulfur, false );
				pSpot->AddEncounter( enc );
			}
		}
	}
	m_aryEncounter.Clear();
}

#include "XPropItem.h"
/**
 @brief 디폴드 계정을 만든다.
*/
#if defined(_XSINGLE) || defined(_GAME_SERVER)
void XAccount::CreateFakeAccount(void)
{
	XWorld *pWorld = new XWorld;
	m_pWorld = pWorld;
	m_Level.SetLevel(1);
	m_aryResource[XGAME::xRES_WOOD] = 1000;
	m_aryResource[XGAME::xRES_IRON] = 1000;
	m_Cashtem = 10;
	m_Gold = 2000;
	m_PowerIncludeEmpty = 1;
	m_PowerExcludeEmpty = 1;
	m_maxAP = m_AP = GetmaxAP(GetLevel());
	m_maxItems = XGAME::ITEM_INIT_COUNT;	// 초기 인벤슬롯 개수

	// 아이템 생성
	// 	for( int i = 0; i < 20; ++i )
	// 	{
	// 		XPropItem::xPROP *pProp = PROP_ITEM->GetpPropRandom();
	// 		XBREAK( pProp == nullptr );
	// 		int num = 1;
	// 		if( pProp->maxStack > 1 )
	// 			num = 1 + random( pProp->maxStack );
	// 		XBaseItem *pBaseItem = sCreateItem( pProp, num );
	// 		AddItem( pBaseItem );
	// 	}

	// 플레이어 군단정보 생성
#ifdef _XSINGLE
	// 모든유닛의 락을 해제한다.
	for( int i = 1; i < m_aryUnlockedUnit.GetMax(); ++i ) {
		m_aryUnlockedUnit[ i ] = 2;
	}
//	CreateLegionByRandom(15, 50);
#else
	// 디폴트 부대를 생성.
	XLegion *pLegion = new XLegion;
	m_aryLegion[0] = XSPLegion(pLegion);
	int tierUnit = 1;
	int levelSquad = 1;
	CreateSquadron(pLegion, 1, _T("unluny"), levelSquad, tierUnit);
	CreateSquadron(pLegion, 2, _T("ricky"), levelSquad, tierUnit);
	CreateSquadron(pLegion, 3, _T("jyulra"), levelSquad, tierUnit);
	auto pHero = GetpHeroByIdentifier(_T("unluny"));
	if( XASSERT(pHero) )
		pHero->SetGrade( XGAME::xGD_RARE );		// 언루니는 3성으로 시작.
	auto pSquad = pLegion->GetpSquadronByidxPos(1);
	if (XASSERT(pSquad))
		pLegion->SetpLeader(pSquad->GetpHero());
	///< 
#endif
	for( auto pHero : m_listHero ) {
		pHero->InitAbilMap();
	}
	// 모든 구름 리스트를 가지고 있게 한다.
	pWorld->InitCloudList( GetLevel() );
	if( GetCurrLegion() )
		UpdatePower();
}
#endif // defined(_XSINGLE) || defined(_GAME_SERVER)

#if defined(_XSINGLE) || !defined(_CLIENT)
/**
@brief 지정된 값으로 부대를 생성한다.
*/
XSquadron* XAccount::CreateSquadron(XLegion *pLegion,
																		int idxSquad,
																		LPCTSTR szHeroIdentifier,
																		int levelSquad,
																		int tierUnit )
{
// 	auto pPropHero = PROP_HERO->GetpProp(szHeroIdentifier);
// 	if (XBREAK(pPropHero == nullptr))
// 		return nullptr;
// 	auto unit = XGAME::GetRandomUnit(pPropHero->typeAtk, (XGAME::xtSize)tierUnit);
// 	auto pHero = XHero::sCreateHero(pPropHero, levelSquad, unit);
// 	AddHero(pHero);
// 	auto pSq = new XSquadron(pHero);
// 	pLegion->AddSquadron(idxSquad, pSq, false);
	auto pSq = sCreateSquadron( pLegion, idxSquad, szHeroIdentifier, levelSquad, tierUnit, GetThis() );
	if( XASSERT(pSq) ) {
			AddHero( pSq->GetpHero() );
	}
	return pSq;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief static 버전
*/
XSquadron* XAccount::sCreateSquadron( XLegion *pLegion,
																		 int idxSquad,
																		 const _tstring& idsHero,
																		 int levelSquad,
																		 int tierUnit,
																		 XSPAccConst spAcc )
{
	auto pPropHero = PROP_HERO->GetpProp( idsHero );
	if( XBREAK( pPropHero == nullptr ) )
		return nullptr;
	auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, ( XGAME::xtSize )tierUnit );
	auto pHero = XHero::sCreateHero( pPropHero, levelSquad, unit, spAcc );
	auto pSq = pLegion->CreateAddSquadron( idxSquad, pHero, false );
// 	auto pSq = new XSquadron( pHero );
// 	pLegion->AddSquadron( idxSquad, pSq, false );
	return pSq;
}
#endif // defined(_XSINGLE) || !defined(_CLIENT)

// 보석광산에서도 더미계정쓰는 개발기능이 있어 굳이 더미제네레이터 디파인으로 한정하지 않았다.
#if defined(_GAME_SERVER) && defined(_DEV)
/**
@brief 더미계정을 만든다. level을 기반으로 각종 계정데이터를 갖는다.
*/
#include "XMain.h"
int XAccount::CreateDummyAccount( int lvExtern )
{

	int level = 5 + random( 45 );
	if( lvExtern > 0 )
		level = lvExtern;
	{
		static std::map<int, int> s_mapLvAcc;		// <레벨,유저수>
		int numUser = 1;
		auto itor = s_mapLvAcc.find( level );
		if( itor == s_mapLvAcc.end() ) {
			s_mapLvAcc[ level ] = 1;
		} else {
			numUser = itor->second;
			++numUser;
			itor->second = numUser;
		}
		// 현재 이값이 저장되고 있지 않음. 아울러 닉네임도.
		m_strID = XFORMAT("test%02d%d", level, numUser );
		m_strPassword = _T("1234");
	}

	XWorld *pWorld = new XWorld;
	m_pWorld = pWorld;
	m_Level.SetLevel( level );
//	m_strName = XE::Format( _T( "user%d" ), GetidAccount() );
#ifdef _DUMMY_GENERATOR
	m_strName = MAIN->m_aryNames.GetFromRandom();
#endif // _DUMMY_GENERATOR
// 	m_aryResource[ XGAME::xRES_WOOD ] = ( level * 5000 ) + random( 5000 );
// 	m_aryResource[ XGAME::xRES_IRON ] = ( level * 5000 ) + random( 5000 );
// 	m_aryResource[ XGAME::xRES_JEWEL ] = ( level * 5000 ) + random( 5000 );
// 	m_aryResource[ XGAME::xRES_SULFUR ] = ( level * 2500 ) + random( 2500 );
// 	m_aryResource[ XGAME::xRES_MANDRAKE ] = ( level * 1000 ) + random( 1000 );
	m_Cashtem = 0;
	m_Gold = (level * 10000) + random(10000);
	m_Ladder = (level * 25) + random(50);
	m_maxAP = m_AP = GetmaxAP(GetLevel());
	// 보통 유저들은 빈 슬롯을 놔두지 않으므로 영웅수는 해당레벨대에서 최대로 있다고 가정해야 함.
	int numHeros = sGetMaxSquadByLevelForPlayer(level);
	if (XBREAK(numHeros > 15))
		numHeros = 15;
// 	InitAbilMap();
	// 테크포인트가 사라졌으므로 더미용으로 가상의 테크포인트를 사용한다.
	XArrayLinearN<char, XGAME::xUNIT_MAX> aryTechPoint;
	aryTechPoint.Fill( 0 );
	UnlockUnitForDummy(/* aryTechPoint*/ );
//	GenerateAbilityForDummy( level, aryTechPoint );
	CreateLegionByRandom(numHeros, level);
	XBREAK(GetCurrLegion()->GetNumSquadrons() == 0);
	// 각 영웅들의 특성트리를 만듬.
	for( auto pHero : m_listHero ) {
		GenerateAbilityForDummy( pHero, level );
	}
// 	m_PowerIncludeEmpty = GetMilitaryPowerCurrLegion();   // 내 전투력을 계산한다.
	UpdatePower();
	// 레벨에 맞춰 구름을 깐다.
	m_pWorld->InitCloudList(level);
	// 깐 구름밑에 있던 스팟을 생성한다.
	XArrayLinearN<XPropCloud::xCloud*, 512> ary;
	m_pWorld->GetOpenCloudsToAry( &ary );
	///< 
	XARRAYLINEARN_LOOP(ary, XPropCloud::xCloud*, pPropCloud) {
		// 이 구름 밑에 있던 스팟객체를 생성한다. 이미 생성되어있다면 다시 생성하지 않는다.
		CreateSpotsByCloud(pPropCloud, nullptr, nullptr, true );
	} END_LOOP;
	// 스팟이 만들어졌으니 정산시간도 현재시간으로 세팅되어 있어야 한다.
	m_pWorld->UpdateLastCalc();
#ifdef _DUMMY_GENERATOR
	// 더미제네레이터에는 편의상 현재시간의 24시간 전으로 세팅한다.
	m_pWorld->SetsecLastCalc(m_pWorld->GetsecLastCalc() - (60 * 60 * 24));
#endif // _DUMMY_GENERATOR
	UpdatePlayTimer(0);	// 플레이타이머 초기화
	//	UpdateTradeMerchantTimer(0);
	//	UpdateShopMerchantTimer(0);
	return 1;
}

/**
 @brief 더미계정을 만들되 특성과 군단데이타만 만든다.
*/
int XAccount::CreateDummyAccountLegion( int level )
{
	SetLevel( level );
	m_Ladder = ( level * 25 ) + random( 50 );
	// 보통 유저들은 빈 슬롯을 놔두지 않으므로 영웅수는 해당레벨대에서 최대로 있다고 가정해야 함.
	int numHeros = sGetMaxSquadByLevelForPlayer( level );
	if( XBREAK( numHeros > 15 ) )
		numHeros = 15;
//   InitAbilMap();
	// 테크포인트가 사라졌으므로 더미용으로 가상의 테크포인트를 사용한다.
	XArrayLinearN<char, XGAME::xUNIT_MAX> aryTechPoint;
	aryTechPoint.Fill( 0 );
	UnlockUnitForDummy( /*aryTechPoint*/ );
	CreateLegionByRandom( numHeros, level );
	XBREAK( GetCurrLegion()->GetNumSquadrons() == 0 );
	// 각 영웅들의 특성트리를 만듬.
	for( auto pHero : m_listHero ) {
		GenerateAbilityForDummy( pHero, level );
	}
//  m_Power = GetMilitaryPowerCurrLegion();   // 내 전투력을 계산한다.
	UpdatePower();
	return 1;
}
// 더미용
static int _sAddTechPoint( XArrayLinearN<char, XGAME::xUNIT_MAX>& aryTechPoint,
							XGAME::xtUnit unit, int add ) 
{
	auto point = aryTechPoint[ unit ];
	if( point < 25 )
		return aryTechPoint[ unit ] += add;
	return point;
}

/**
 @brief 레벨에 따라 랜덤으로 유닛을 언락한다.
*/
void XAccount::UnlockUnitForDummy( /*XArrayLinearN<char, XGAME::xUNIT_MAX>& aryTechPoint*/ )
{
	XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> aryUnits;
	XBREAK(GetLevel() == 0);
	// 레벨에 따라 선택가능한 유닛목록을 랜덤으로 뽑아낸다.
	XLegion::sGetAvailableUnitByLevel(GetLevel(), &aryUnits);
//  	m_aryTechPoint.Fill(0);
	int adj = 0;
	// 얻은 유닛들을 언락한다.
	int numUnit = 0;
	m_bUnlockTicketForPaladin = true;
	m_numUnlockTicketForMiddleOrBig = 6;
	XARRAYLINEARN_LOOP_AUTO(aryUnits, unit) {
		++numUnit;
		if( unit == XGAME::xUNIT_PALADIN )
			m_bUnlockTicketForPaladin = true;
		SetUnlockUnit( unit );
	} END_LOOP;
}
/**
@brief 더미 계정을 위한 특성포인트를 설정한다.
*/
void XAccount::GenerateAbilityForDummy( XSPHero pHero, int lvAcc )
{
	// 레벨에 따라 각 유닛별 특성포인트를 계산한다.
	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
		auto unit = (XGAME::xtUnit) i;
		if (IsUnlockUnit(unit) && pHero->IsResearbleUnit(unit) ) {
			pHero->GenerateAbilityForDummy( unit, lvAcc );
		}
	}

}

#endif // #if defined(_GAME_SERVER) && defined(_DEV)
//#if defined(_XSINGLE) || !defined(_CLIENT)
#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
/**
@brief 더미계정이나 싱글플레이를 위한 군단 생성기. numSquad수만큼의 부대를 랜덤생성한다.
@param byLevel 레벨대에 맞춰 유닛을 제한한다
*/
XLegion* XAccount::CreateLegionByRandom(int numSquad, int byLevel)
{
	XBREAK(m_aryLegion[0] != NULL);
	XBREAK(m_listHero.size() > 0);
	XLegion *pLegion = new XLegion;
	m_aryLegion[0] = XSPLegion(pLegion);		// 어카운트에만 만드는 용도로 쓰기위해.
	// 장군 15명 생성
//	int maxHero = PROP_HERO->GetSize();
	int idx[XGAME::MAX_SQUAD] = { 2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14 };
	XBREAK(numSquad > XNUM_ARRAY(idx));
	// 이 부대가 랜덤선택가능한 유닛목록을 미리 뽑는다.
	XBREAK( byLevel == 0 );
	XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> aryUnits;
#ifdef _XSINGLE
	XLegion::sGetAvailableUnitByLevel(byLevel, &aryUnits);
#else
	// 더미 계정만들땐 실제 언락된 유닛들만으로 부대를 만든다.
	GetUnlockUnitsToAry( &aryUnits );
#endif
	//
	auto& dummyTbl = XGC->GetDummyTbl( byLevel );
	//
	for (int i = 0; i < numSquad; ++i) {
		int idxSquad = idx[i];
		if (pLegion->GetpSquadronByidxPos(idxSquad) == nullptr) {
			XSquadron *pSquad = nullptr;
#ifdef _XSINGLE
			auto unit = aryUnits.GetFromRandom();
			auto grade = XGAME::xGD_COMMON;
			int lvHero = XGAME::MAX_HERO_LEVEL;
			int lvSquad = PROP_SQUAD->GetMaxLevel();
			int lvSkill = XGAME::MAX_SKILL_LEVEL - 1;
			auto typeAtk = XGAME::GetAtkType( unit );
			auto pPropHero = PROP_HERO->GetPropRandomByAtkType( typeAtk, 50 );
			if (idxSquad == 2) {
				// 첫번째 부대는 지정되어있는 값으로 한다
				// 유닛이 지정되어있다면 그 유닛으로 한다.
				if( XGC->m_strHero1 == _T("random") || XGC->m_strHero1.empty() ) {
					// 영웅이 지정되어있지 않다면 지정유닛을 기준으로 랜덤영웅을 뽑음.
					if( XGC->m_unitLeft )
						unit = XGC->m_unitLeft;
					else
						// 지정유닛도 없다면 랜덤으로 유닛 뽑음.
						unit = aryUnits.GetFromRandom();
					// 유닛에 맞춰서 영웅 뽑음.
					typeAtk = XGAME::GetAtkType( unit );
					pPropHero = PROP_HERO->GetPropRandomByAtkType( typeAtk, 50 );
				} else {
					// 영웅이 직접 지정되어있다면
					pPropHero = PROP_HERO->GetpProp( XGC->m_strHero1 );
					XBREAK( pPropHero == nullptr );
					unit = XGC->m_unitLeft;
					if( pPropHero->typeAtk != XGAME::GetAtkType(unit) ) {
						CONSOLE("영웅(%s)의 공격타입(%d)과 지정한 유닛(%d)의 타입이 맞지 않아 자동변환됨.", pPropHero->GetstrName().c_str(), pPropHero->typeAtk, XGAME::GetAtkType(unit) );
						// 영웅에 맞춰서 유닛(소형)을 뽑음.
						unit = XGAME::GetUnitBySizeAndAtkType( XGAME::xSIZE_SMALL, pPropHero->typeAtk );
					}
				}
				if( XGC->m_lvHero1 )
					lvHero = XGC->m_lvHero1;
				if( XGC->m_lvSquadLeft )
					lvSquad = XGC->m_lvSquadLeft;
			} else {
				// 첫번째부대 이외 부대는 모두 디폴트값으로 한다.
			}
			XBREAK( pPropHero == nullptr );
			pSquad = CreateSquadron(pLegion, idxSquad
															, lvHero, lvSquad, lvSkill, lvSkill
															, pPropHero->strIdentifier.c_str(), grade, unit );
#else
			// 선택가능한 유닛목록중에 하나를 뽑음.
			auto unit = aryUnits.GetFromRandom();
			auto grade = (XGAME::xtGrade)(dummyTbl.m_gradeHero + xRandom(-1,1));
			if( grade <= XGAME::xGD_NONE )
				grade = XGAME::xGD_COMMON;
			if( grade >= XGAME::xGD_MAX )
				grade = XGAME::xGD_LEGENDARY;
			int lvHero = dummyTbl.m_lvHero + xRandom(-2, 2);
			if( lvHero <= 0 )
				lvHero = 1;
			if( lvHero > XGAME::MAX_HERO_LEVEL )
				lvHero = XGAME::MAX_HERO_LEVEL;
			int lvSquad = dummyTbl.m_lvSquad + xRandom(-1, 1 );
			if( lvSquad < 1 )
				lvSquad = 1;
			if( lvSquad > XGAME::MAX_SQUAD_LEVEL )
				lvSquad = XGAME::MAX_SQUAD_LEVEL;
			int lvSkill = dummyTbl.m_lvSkill + xRandom(-1, 1 );
			if( lvSkill < 1 )
				lvSkill = 1;
			if( lvSkill > XGAME::MAX_SKILL_LEVEL - 1 )
				lvSkill = XGAME::MAX_SKILL_LEVEL - 1;
			auto typeAtk = XGAME::GetAtkType( unit );
			auto pPropHero = PROP_HERO->GetPropRandomByAtkType( typeAtk, 0 );	// 유저더미 만들때는 lvLimit가 없게 한다.
			if( GetNumHeroes() == 0 ) {
				pPropHero = PROP_HERO->GetpProp(_T("unluny"));
				XVector<XGAME::xtUnit> aryAbleUnits;
				XARRAYLINEARN_LOOP_AUTO( aryUnits, unit ) {
					if( pPropHero->typeAtk == XGAME::GetAtkType( unit ) )
						aryAbleUnits.Add( unit );
				} END_LOOP;
				unit = aryAbleUnits.GetFromRandom();
			}
			XBREAK( pPropHero == nullptr );

			pSquad = CreateSquadron(pLegion, idxSquad
														, lvHero, lvSquad, lvSkill, lvSkill
														, pPropHero->strIdentifier.c_str(), grade, unit);
			if( i == 0 ) {
				// 첫영웅 언루니는 3성으로 
				auto pHero = GetpHeroByIdentifier(_T("unluny"));
				if( pHero ) {
					pHero->SetGrade( XGAME::xGD_RARE );
				}
			}
#endif // not single
			// 가장 첫번째 부대의 영웅이 리더.
			if (XASSERT(pSquad) && pLegion->GetpLeader() == nullptr)
				pLegion->SetpLeader(pSquad->GetpHero());
		}
	}
	// 원거리 유닛이 앞에 있으면 뒤로 빼주는등의 조정을 한다.
#if defined(_GAME_SERVER)
	pLegion->AdjustLegion();
#endif // not _XSINGLE
	return pLegion;
}

/**
 @brief 지정된 값으로 부대를 생성시킨다.
 랜덤요소는 없으면 반드시 외부에서 파라메터를 지정해줘야 한다.
 이 함수는 싱글모드 혹은 게임서버의 PC부대생성에 사용한다.
*/
XSquadron* XAccount::CreateSquadron(XLegion *pLegion,
																	int idxSquad,
																	int lvHero,
																	int lvSquad,
																	int lvActive,
																	int lvPassive,
																	LPCTSTR idsHero,
																	XGAME::xtGrade grade,
																	XGAME::xtUnit unit )
{
	XBREAK( XGAME::IsInvalidUnit(unit) );
	XASSERT( lvHero > 0 && lvHero <= XGAME::MAX_HERO_LEVEL );
	XASSERT( lvSquad > 0 && lvSquad <= PROP_SQUAD->GetMaxLevel() );
	XASSERT( lvActive > 0 && lvActive <= XGAME::MAX_SKILL_LEVEL - 1 );
	XASSERT( lvPassive > 0 && lvPassive <= XGAME::MAX_SKILL_LEVEL - 1 );
	XBREAK( XE::IsEmpty( idsHero ) );
	XBREAK( XGAME::IsInvalidGrade(grade) );
	XBREAK(pLegion->GetpSquadronByidxPos(idxSquad) != nullptr);
	auto pPropHero = PROP_HERO->GetpProp( idsHero );
	XBREAK( pPropHero == nullptr );
	auto pHero = XHero::sCreateHero(pPropHero, lvSquad, unit, GetThis() );
	XBREAK(pHero == nullptr);
	pHero->SetLevel( XGAME::xTR_LEVEL_UP, lvHero);
	pHero->SetGrade( grade );
	pHero->SetLevel( XGAME::xTR_SKILL_ACTIVE_UP, lvActive );
	pHero->SetLevel( XGAME::xTR_SKILL_PASSIVE_UP, lvPassive );
	AddHero(pHero);
	// NPC가 아니므로 sq가 블럭을 벗어날때 pHero를 파괴하진 않는다.
	// 일단은 놔두고 깔끔하게 해결해야함.
	XSquadron *pSq = new XSquadron(pHero);
	pLegion->AddSquadron(idxSquad, pSq, FALSE);
	return pSq;
}

#endif //#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))

/**
@brief 더미계정을 위한 등급 생성기
*/
XGAME::xtGrade XAccount::GetRandomGradeHeroByTable(int levelUser) const
{
	XGAME::xtGrade grade = XGAME::xGD_COMMON;
	if( levelUser <= 10 ) {
		grade = XGAME::xGD_COMMON;
	} else
	if (levelUser <= 15) {
		// 1,2등급
		XArrayLinearN<float, 2> aryChance;
		aryChance.Add(0.7f);
		aryChance.Add(0.3f);
		int idxSel = XE::GetDiceChance(aryChance);
		grade = (XGAME::xtGrade)(XGAME::xGD_COMMON + idxSel);
	} else 
	if (levelUser <= 25) {
		// 1,2,3등급
		XArrayLinearN<float, 3> aryChance;
		aryChance.Add(0.3f);
		aryChance.Add(0.3f);
		aryChance.Add(0.4f);
		int idxSel = XE::GetDiceChance(aryChance);
		grade = (XGAME::xtGrade)(XGAME::xGD_COMMON + idxSel);
	} else 
	if (levelUser <= 35) {
		// 2,3등급
		XArrayLinearN<float, 2> aryChance;
		aryChance.Add(0.6f);
		aryChance.Add(0.4f);
		int idxSel = XE::GetDiceChance(aryChance);
		grade = (XGAME::xtGrade)(XGAME::xGD_VETERAN + idxSel);
	} else 
	if (levelUser <= 45) {
		// 3등급만.
		grade = XGAME::xGD_RARE;
	} else {
		// 그 이상은 4등급만.
		grade = XGAME::xGD_EPIC;
	}
	return grade;
}
void XAccount::DestroyLegion(void)
{
//	m_aryLegion.Clear(LegionPtr());
	m_aryLegion.Fill( nullptr );
}

/**
영웅 시리얼 번호로 찾는다.
*/
XSPHero XAccount::GetHero(ID snHero)
{
	if (snHero == 0)
		return nullptr;
	for (auto pHero : m_listHero) {
		if (pHero->GetsnHero() == snHero)
			return pHero;
	}
	return nullptr;
}

XSPHeroConst XAccount::GetpcHeroBySN( ID snHero ) const 
{
	if( snHero == 0 )
		return nullptr;
	for( auto pHero : m_listHero ) {
		if( pHero->GetsnHero() == snHero )
			return pHero;
	}
	return nullptr;
}

/**
 @brief 영웅 아이디로 영웅을 찾음.
*/
XSPHero XAccount::GetHeroByidProp( ID idProp )
{
	if( XBREAK(idProp == 0) )
		return nullptr;
	for( auto pHero : m_listHero ) {
		if( pHero->GetidProp() == idProp )
			return pHero;
	}
	return nullptr;
}

XSPHero XAccount::GetpHeroByAtkType( XGAME::xtAttack typeAtk )
{
	for( auto pHero : m_listHero ) {
		if( pHero->GetTypeAtk() == typeAtk )
			return pHero;
	}
	return nullptr;
}

XSPHero XAccount::GetpHeroByUnit( XGAME::xtUnit unit )
{
	for( auto pHero : m_listHero ) {
		if( pHero->GetUnit() == unit )
			return pHero;
	}
	return nullptr;
}

int XAccount::GetNumHeroes()
{
	return m_listHero.size();
}

/**
 @brief 군단에 배속되지 않은 영웅의 수
*/
int XAccount::GetNumExtraHeroes()
{
	return GetNumHeroes() - GetCurrLegion()->GetNumSquadrons();
}
/**
 @brief 인벤의 idx번째있는 영웅을 얻는다.
*/
XSPHero XAccount::GetpHeroByIndex( int idx )
{
	int i = 0;
	for( auto pHero : m_listHero ) {
		if( i == idx )
			return pHero;
		++i;
	}
	return nullptr;
}
/**
 @brief szIdentifier의 영웅을 가지고 있는지 검사.
*/
bool XAccount::IsHaveHero( LPCTSTR szIdentifier )
{
	if( XBREAK( XE::IsEmpty(szIdentifier) ) )
		return false;
	for( auto pHero : m_listHero ) {
		if( pHero->GetpProp()->strIdentifier == szIdentifier )
			return true;
	}
	return false;
}

XSPHero XAccount::GetpHeroByIdentifier( LPCTSTR szIdentifier )
{
	if( XBREAK( XE::IsEmpty( szIdentifier ) ) )
		return nullptr;
	for( auto pHero : m_listHero ) {
		if( pHero->GetpProp()->strIdentifier == szIdentifier )
			return pHero;
	}
	return nullptr;
}

/**
스페셜 스팟이 나타나기까지 남은 일수
*/
// int XAccount::GetRemainDaySpecialSpot(void)
// {
// 	if (m_secSpecialSpotReleased == 0)
// 		SetSpecialSpotTimer();
// 	// 스팟이 없어진시간으로부터 현재까지 흐른 시간
// 	int dayPass = GetPassDaySpecialSpot();
// 	return m__dayWaitSpecialSpot - dayPass;
// }
// 
// /**
// 스페셜 스팟 타이머작동후부터 지난 일수
// */
// int XAccount::GetPassDaySpecialSpot(void)
// {
// 	auto secPass = XTimer2::sGetTime() - m_secSpecialSpotReleased;
// 	return (int)(secPass / 60 / 60 / 24);		// 일(day) 이하 버림.
// }
// 
/**
@brief 아이템을 인벤에 추가한다.
*/
XBaseItem* XAccount::AddItem(XBaseItem *pBaseItem)
{
#ifdef _DEV
	// 이미 같은 시리얼번호의 아이템이 있는지 검사.
	XBREAK(m_listItem.FindpByID(pBaseItem->GetsnItem()) != nullptr);
#endif
	m_listItem.Add(pBaseItem);
	//	XBREAK( m_listItem.size() > 100 );
	return pBaseItem;
}

/**
@brief 인벤에 넣어야 하는 아이템 목록을 받아서 인벤에 넣되 snItem이 같은게 발견되면
새로 생성하지 않고 그 객체에 갱신한다. 클라에서만 사용.
보통 서버에서 갱신된 아이템목록을 클라에 갱신시켜줄때 사용.
*/
XBaseItem* XAccount::DeserializeAddItem(XArchive& ar, int ver)
{
	XBaseItem *pItem = XBaseItem::sCreateDeSerialize(ar, ver);
	ID snItem = pItem->GetsnItem();
#ifdef _CLIENT
	// 편의상 기존 snItem아이템을 완전히 삭제해버리고 새로 add한다.
	DestroyItemBySN(pItem->GetsnItem(), 0);
#else
	XBREAK(IsExistItem(pItem->GetsnItem()));
	DestroyItemBySN(pItem->GetsnItem(), 0);
#endif
	auto pNewItem = AddItem(pItem);
	// 깨진포인터를 갖고 있을 수 있으므로 모든 영웅에게 아이템 포인터를 재갱신하라고 알림.
	UpdateNewItemForHeros(snItem, pNewItem);
	return pNewItem;
}

/**
@brief 아카이브를 deserialize해서 인벤에 업데이트 한다.
아이템은 인벤에 이미 있는 아이템일수도 아닐수도 있다.
인벤에 없는거라면 추가한다.
이미 있는것이라면 포인터를 삭제하지 않고 그대로 업데이트 한다.
*/
XBaseItem* XAccount::DeserializeUpdateItem(XArchive& ar, int verItem)
{
	ID idProp, snItem;
	XGAME::xtItem type;
	// ?? 개수템은?
	XBaseItem::sDeSerializeHeaderAndType(ar, &idProp, &snItem, &type, verItem);
	// 기존에 이미 있던것인지 검사.
	bool bCreate;
	auto pItem = GetItem(snItem);
	if (pItem == nullptr) {
		// 없는아이템이면 새로 생성한다.
		pItem = XBaseItem::sCreateItem(idProp, snItem);
		bCreate = true;
	}
	else
		bCreate = false;
	XBaseItem::sDeSerializeNoHeader(ar, pItem, idProp, snItem, verItem);
	// 새아이템이면 인벤에 넣는다.
	if (bCreate)
		pItem = AddItem(pItem);
	return pItem;
}


/**
@brief snItem이 재할당되어(pReallocItem(장착)) 영웅들이 갖고 있는 포인터를 갱신하라고 알려줌.
@param null이라면 클리어로 작동된다.
*/
void XAccount::UpdateNewItemForHeros(ID snItem, XBaseItem *pReallocItem)
{
	for (auto pHero : m_listHero)
	{
		pHero->UpdateEquipItem(snItem, pReallocItem);
	}
}

XSPHero XAccount::AddHero(XSPHero pHero)
{
	m_listHero.Add(pHero);
	return pHero;
}

/**
@brief 영웅을 삭제시키고 군단에서도 제거한다.
*/
void XAccount::DestroyHero(ID snHero)
{
	DeleteHeroInLegion(snHero);
	///< 
	auto pHero = m_listHero.FindByID(snHero);
//	XSPHero pHero = (*ppHero);
	pHero->SetUnequipAll();
	m_listHero.DelByID(snHero);
	//SAFE_DELETE(pHero);
}

/**
@brief 보유 군단에서 snHero의 부대를 모두 제거한다.
*/
void XAccount::DeleteHeroInLegion(ID snHero)
{
// 	XARRAYN_LOOP(m_aryLegion, LegionPtr&, spLegion)
	for( auto spLegion : m_aryLegion ) {
		if ( spLegion )
			spLegion->DestroySquadBysnHero(snHero);
	}// END_LOOP;
}
/**
@brief 군단에 배정된 장군을 제외한 장군의 리스트를 돌려준다.
@param idxLegion 군단 인덱스
*/
int XAccount::GetHerosListExceptLegion(XArrayLinearN<XSPHero, 1024> *pOutAry, XLegion *pLegion)
{
	XBREAK(pLegion == NULL);
	// 영웅들의 루프를 돌면서
	for( auto pHero : m_listHero ) {
		if( pLegion ) {
			XSquadron *pSquad = pLegion->GetSquadronByHeroSN( pHero->GetsnHero() );
			if( pSquad == NULL )
				pOutAry->Add( pHero );
		} else
			pOutAry->Add( pHero );
	}
	return pOutAry->size();
}

int XAccount::SerializePostInfo( XArchive& ar ) const
{
	ar << (int)m_listPost.size();
	for( auto pPostInfo : m_listPost ) {
		pPostInfo->Serialize( ar );
	}
	return 1;
}

int XAccount::DeSerializePostInfo( XArchive& ar, int verPost )
{
	int numPost;
	ar >> numPost;
	for( int i = 0; i < numPost; ++i ) {
		auto pInfo = new XPostInfo();
		pInfo->DeSerialize( ar, verPost );
		m_listPost.Add( pInfo );
	}
	return 1;
}


XPostInfo* XAccount::AddPostInfo(XPostInfo* pPostInfo)
{
	XBREAK( pPostInfo == nullptr );
	auto pExist = GetPostInfo( pPostInfo->GetsnPost() );
	if( pExist )
		return pExist;
	m_listPost.Add( pPostInfo );
	return pPostInfo;
}

void XAccount::DestroyPostInfo( ID snPost )
{
	for( auto itor = m_listPost.begin(); itor != m_listPost.end(); ) {
		auto pPostInfo = (*itor);
		if( pPostInfo->GetsnPost() == snPost ) {
			SAFE_DELETE( pPostInfo );
			m_listPost.erase( itor++ );
		} else
			++itor;
	}
}

XPostInfo* XAccount::GetPostInfo( ID snPost )
{
	for( auto pPostInfo : m_listPost ) {
		if( pPostInfo->GetsnPost() == snPost )
			return pPostInfo;
	}
	return nullptr;
}

void XAccount::DestroyPostInfoAll()
{
	for( auto pPostInfo : m_listPost ) {
		SAFE_DELETE( pPostInfo );
	}
	m_listPost.clear();
}

/**
 @brief snPost편지에 첨부되어있는 모든 아이템을 계정으로 수령한다.
 @return 해당 메일을 찾았으면 true
*/
bool XAccount::ReceivePostItemsAll( ID snPost )
{
	auto pPostInfo = GetPostInfo( snPost );
	if( pPostInfo ) {
		ReceivePostItemsAll( pPostInfo );
	}
	return pPostInfo != nullptr;
}

/**
 @brief pPostInfo편지에 첨부되어있는 모든 아이템들을 계정으로 수령한다.
*/
void XAccount::ReceivePostItemsAll( XPostInfo* pPostInfo )
{
	// 첨부되어있는 모든 아이템들의 리스트
	for( auto spItems : pPostInfo->GetlistPostItems() ) {
		auto type = spItems->GetItemType();
		const auto idItem = spItems->GetItemID();
		const auto numItem = spItems->GetnCount();
		switch( type ) {
		case XGAME::xPOSTRES_GOLD:
			AddGold( numItem );
			break;
		case XGAME::xPOSTRES_GEM:
			AddCashtem( numItem );
			break;
		case XGAME::xPOSTRES_ITEMS: {
			auto pPropItem = PROP_ITEM->GetpProp( idItem );
			if( pPropItem ) {
				CreateItemToInven( pPropItem, numItem );
			}
		} break;
		case XGAME::xPOSTRES_RESOURCE:
			AddResource( (xtResource)idItem, numItem );
			break;
		case XGAME::xPOSTRES_GUILD_COIN:
			AddGuildPoint( numItem );
			break;
		default:
			XBREAKF( 1, "메일내 알수없는 첨부아이템 타입:%d", type );
			break;
		}
	}
}

/**
 @brief 
*/
XSpot* XAccount::CreateNewSpot( ID idSpot, XDelegateSpot *pDelegate )
{
	auto pProp = PROP_WORLD->GetpProp( idSpot );
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	return CreateNewSpot( pProp, pDelegate );
}
/**
 @brief 스팟 생성 공통모듈(DB Load시에는 사용하지 않음)
 구름이 걷혀서 최초 월드에 생성될때
 퀘스트등으로 스팟이 생성될때
 툴에서...
*/
XSpot* XAccount::CreateNewSpot(XPropWorld::xBASESPOT *pBaseProp, XDelegateSpot *pDelegate)
{
	if( XBREAK( pBaseProp == NULL ) )
		return nullptr;
	XSpot *pSpot = nullptr;
	do {
		pSpot = m_pWorld->GetSpot( pBaseProp->idSpot );
		if( pSpot ) {
			if( pBaseProp->GetType() == XGAME::xSPOT_CASTLE
				|| pBaseProp->GetType() == XGAME::xSPOT_NPC ) {
				if( !pSpot->IsEventSpot() ) {
					// 별개수 맵에 없으면 새로 추가시킨다.
					int numStar = GetNumSpotStar( pSpot->GetidSpot() );
					if( numStar < 0 )
						AddSpotStar( pSpot->GetidSpot() );
				}
			}
			break;
		}
		pSpot = XSpot::sCreate( m_pWorld, pBaseProp, GetThis(), pDelegate );
		if( XBREAK( pSpot == nullptr ) )
			return nullptr;
		AddSpot( pSpot );
#ifdef _GAME_SERVER
		pSpot->OnCreateNewOnServer( GetThis() );	// 월드에 add된직후에 호출되어야 한다.
#endif // _GAME_SERVER
		// 새로운 스팟이 생성되면 계정정보에 별개수 정보를 만든다.
		if( pBaseProp->GetType() == XGAME::xSPOT_CASTLE 
			|| pBaseProp->GetType() == XGAME::xSPOT_NPC ) {
			if( !pSpot->IsEventSpot() )
				AddSpotStar( pSpot->GetidSpot() );
		}
	} while( 0 );
	return pSpot;
}

/**
 @brief 별을 얻을 수 있는 상태인가(별표시 포함)
*/
bool XAccount::IsAbleGetStar() const
{
	return ( GetLevel() >= 7 );
}

/**
 @brief idSpot의 현재별개수를 리턴
*/
int XAccount::GetNumSpotStar( ID idSpot )
{
	auto itor = m_mapSpotStar.find( idSpot );
	if( itor == m_mapSpotStar.end() )
		return -1;	// 못찾음
	return (*itor).second;
}
/**
 @brief 새로운 스팟을 등록함.
*/
int XAccount::AddSpotStar( ID idSpot )
{
	// 이미 있는데 부르면 에러
// 	if( XBREAK(GetNumSpotStar( idSpot ) >= 0) ) {
	if( GetNumSpotStar( idSpot ) >= 0 ) {
		// 맵이 바껴도 이런일이 생길 수 있음.
		return -1;
	}
	m_mapSpotStar[ idSpot ] = 0;
	return 1;
}
/**
 @brief idSpot의 별개수를 세팅한다.
 @return 정상적으로 세팅되었으면 별개수를 리턴한다. 3이 리턴되면 맵에서 삭제되었다. -1은 잘못된 호출이다.
*/
int XAccount::SetNumStarBySpot( ID idSpot, XGAME::xtSpot type, int numStar )
{
	if( type != XGAME::xSPOT_CASTLE && type != XGAME::xSPOT_NPC )
		return 0;
	XBREAK( numStar < 0 );
	XBREAK( numStar > 3 );
	auto itor = m_mapSpotStar.find( idSpot );
	if( XASSERT(itor != m_mapSpotStar.end()) ) {
		const int num = (*itor).second;
		// 세팅하려는 값이 이전값보다 높으면 차이만큼 누적별수를 증가시킨다.
		if( numStar > num )
			m_numStar += numStar - num;
		(*itor).second = numStar;
		return numStar;
	}
	// 에러
	return -1;
}
int XAccount::SetNumStarBySpot( XSpot *pBaseSpot, int numStar )
{
	return SetNumStarBySpot( pBaseSpot->GetidSpot(), pBaseSpot->GettypeSpot(), numStar );
}

/**
 @brief 스팟 애드 공통함수.
*/
void XAccount::AddSpot( XSpot *pBaseSpot )
{
	m_pWorld->_AddSpot( pBaseSpot );
#ifndef _CLIENT
	pBaseSpot->OnCreate( GetThis() );
#endif // not client
}

/**
pProp구름이 걷힐때 그 밑에 연결되어있던 스팟들을 생성한다.
겹쳐있는 스팟도 있으므로 중복해서 생성하지 않도록 한다.
*/
void XAccount::CreateSpotsByCloud(XPropCloud::xCloud *pPropArea,
																	XDelegateSpot *pDelegate,
																	XArrayLinearN<XSpot*, 1024> *pOutAry/*=NULL*/,
																	bool bDummyMode )
{
	XArrayLinearN<ID, 512> arySpots;
	// 구름밑에 깔린 스팟들의 리스트를 가져온다.
	pPropArea->GetSpotsToAry(arySpots);
	XARRAYLINEARN_LOOP(arySpots, const ID, idSpot) {
		auto pBaseProp = PROP_WORLD->GetpProp(idSpot);
		if (XBREAK(pBaseProp == NULL))	// 스팟이 있었는데 지운후 propCloud는 갱신안했을때도 이런일 생김 반드시 툴로 propCloud를 저장시킬것.
			return;
		bool bCreate = true;
		switch (pBaseProp->type) {
		case XGAME::xSPOT_NPC:
		case XGAME::xSPOT_CAMPAIGN:
		case XGAME::xSPOT_VISIT:
			if( pBaseProp->idWhen )	// 나타날시기가 지정되어있는것들은 제외
				bCreate = false;
			break;
		}
		//
		XSpot *pSpot = nullptr;
		if (bCreate) {
			pSpot = CreateNewSpot(pBaseProp, pDelegate);
			if( pSpot ) {
#ifdef _DUMMY_GENERATOR
				if( bDummyMode ) {
					if( pSpot->GettypeSpot() == XGAME::xSPOT_JEWEL ) {
						auto pSpotJewel = SafeCast<XSpotJewel*>( pSpot );
						if( pSpotJewel ) {
							pSpotJewel->ClearSpot();
							pSpotJewel->OnSpawn( GetThis() );
							// 더미모드에서 보석스팟이 생성되면 바로 자기것으로 만들어서 매칭이 되도록 한다.
							pSpotJewel->SetPlayerOwner( m_idAccount, m_strName.c_str() );
							pSpotJewel->SetidMatchEnemy( 0 );
							pSpotJewel->SetLevel( GetLevel() );
//							pSpotJewel->SetlevelMine( 1 );
							pSpotJewel->ResetDefense( GetLevel() );
						}
					} else // jewel
					if( pSpot->GettypeSpot() == XGAME::xSPOT_MANDRAKE ) {
						auto pSpotMandrake = SafeCast<XSpotMandrake*>( pSpot );
						if( pSpotMandrake ) {
							pSpotMandrake->ClearSpot();
							pSpotMandrake->OnSpawn( GetThis() );
							// 더미모드에서 보석스팟이 생성되면 바로 자기것으로 만들어서 매칭이 되도록 한다.
							pSpotMandrake->SetPlayerOwner( m_idAccount, m_strName.c_str() );
							pSpotMandrake->SetLevel( GetLevel() );
							pSpotMandrake->SetWin( xRandom(11) );
							int reward = 1000;
							for( int i = 0; i < pSpotMandrake->GetWin(); ++i )
								reward *= 2;
							pSpotMandrake->SetReward( reward );
						}
					}
				}
#endif // _DUMMY_GENERATOR
				if( pOutAry )
					pOutAry->Add( pSpot );
			}
		} // bCreate
	} END_LOOP;
}
// void XAccount::SetSpecialSpotTimer(int dayAdjustWait)
// {
// 	// 스팟 해제된 시간을 지금으로 설정한다.
// 	m_secSpecialSpotReleased = XTimer2::sGetTime();
// 
// 	// 	int y, m, d;
// 	// 	int daysWait;
// 	// 	//
// 	// 	XSYSTEM::GetYearMonthDay( &y, &m, &d );
// 	int daysWait = 1 + random(7);
// 	// 지난번 사이클에 기다려야 했던 날짜를 기반으로 적어도 1주일 간격으로 스폰되도록 일수를 보정한다.
// 	if (m__dayWaitSpecialSpot > 0)
// 		daysWait += 7 - m__dayWaitSpecialSpot;
// 	daysWait += dayAdjustWait;		// 추가 보정일
// 	m__dayWaitSpecialSpot = daysWait;	// 앞으로 기다려야 할 일수.
// 	// 	m_ymdSpecialSpotStart.year = (WORD)y;
// 	// 	m_ymdSpecialSpotStart.month = m;
// 	// 	m_ymdSpecialSpotStart.day = d;
// 	// 	m_dayWaitSpecialSpot = daysWait;
// }

/**
@brief 스페셜 스팟이 지금 활성화 되어야 하는지 확인하고 그러면 스팟을 활성화 시킨다.
*/
// XSpotSpecial* XAccount::DoCheckSpecialSpotActivateNow()
// {
// 	// 스페셜 스팟이 지금 활성화 되어야 하는지 확인한다.
// 	if (IsActivateNowSpecialSpot())	// || 1 테스트
// 	{
// 		// 스페셜 스팟중 한곳을 활성화 시킨다.
// 		return DoActiveSpecialSpot();
// 	}
// 	return nullptr;
// }
// 
// /**
// 스페셜 스팟이 지금 활성화 되어야 하는지 확인한다.
// */
// BOOL XAccount::IsActivateNowSpecialSpot(void)
// {
// 	// 스폰타이머 자체가 안켜져있으면 걍 리턴
// 	if (XBREAK(m_secSpecialSpotReleased == 0))
// 		return FALSE;
// 	// 	if( GetymdSpecialSpotStart().IsActive() == FALSE )
// 	// 		return FALSE;
// 	// 이미 활성화된 스페셜 스팟이 있는지 확인한다.
// 	if (m_pWorld->GetActivatedSpecialSpot())
// 		return FALSE;
// 	// 오늘이 스페셜 스팟이 생성되어야 하는 날짜인지 확인한다.
// 	if (IsActivateTodaySpecialSpot())
// 		return TRUE;
// 	return FALSE;
// }

/**
오늘 스팟을 활성화 해야 하는 날인지 검사한다.
*/
// BOOL XAccount::IsActivateTodaySpecialSpot(void)
// {
// 	/*
// 	int year, month, day;
// 	// 오늘 년/월/일을 알아냄
// 	XSYSTEM::GetYearMonthDay( &year, &month, &day );
// 	int dayStart
// 	= XSYSTEM::GetYMDToDays( m_pAccount->GetymdSpecialSpotStart().GetYear(),
// 	m_pAccount->GetymdSpecialSpotStart().GetMonth(),
// 	m_pAccount->GetymdSpecialSpotStart().GetDay() );
// 	int dayToday = XSYSTEM::GetYMDToDays( year, month, day );
// 	*/
// 	// 지난일수랑 기다려야 하는 일수랑 딱맞으면 오늘 활성화 해야 함.
// 	auto dayPass = GetPassDaySpecialSpot();		// 지나간 일수
// 	if (dayPass >= m__dayWaitSpecialSpot)
// 		return TRUE;
// 	return FALSE;
// }
// 
// /**
// 스페셜 스팟중 한곳을 활성화 시킨다.
// */
// XSpotSpecial* XAccount::DoActiveSpecialSpot(void)
// {
// 	//	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	// 자정으로부터 지나간시간(초)을 구한다.
// 	int hour, min, sec;
// 	XSYSTEM::GetHourMinSec(&hour, &min, &sec);
// 	int secTotal = (hour * 60 * 60) + (min * 60) + sec;
// 	XSpotSpecial *pSpot
// 		= m_pWorld->SetActiveSpecialSpotToRandom(secTotal, GetThis());
// 	return pSpot;
// }


// XSpotCampaign* XAccount::CreateSpotCampaign( XPropWorld::xCampaign* pProp, XDelegateSpot *pDelegate )
// {
// 	if( XBREAK( pProp == NULL ) )
// 		return NULL;
// 
// 	XSpotCampaign *pSpot = m_pWorld->_CreateCampaignSpot( pProp, this );
// 	return pSpot;
// }
// 
// XSpotVisit* XAccount::CreateSpotVisit( XPropWorld::xVisit* pProp, XDelegateSpot *pDelegate )
// {
// 	// 이미 월드에 생성되어있는 스팟이면 다시 생성하지 않는다.
// 	if( XBREAK( pProp == NULL ) )
// 		return NULL;
// 	XSpotVisit *pSpot = m_pWorld->_CreateVisitSpot( pProp, this );
// 	return pSpot;
// }
// 
// XSpotCash* XAccount::CreateSpotCash( XPropWorld::xCash* pProp, XDelegateSpot *pDelegate )
// {
// 	// 이미 월드에 생성되어있는 스팟이면 다시 생성하지 않는다.
// 	if( XBREAK( pProp == NULL ) )
// 		return NULL;
// 	XSpotCash *pSpot = m_pWorld->_CreateCashSpot( pProp, this );
// 	return pSpot;
// }



/**
@brief 현재 군단에 소속된 영웅들 모두에게 add만큼의 exp를 더한다.
더불어 렙업한 영웅이 있다면 pAryOut에 받아준다.
@param pOutAryLevelup 렙업한 영웅들의 아이디를 받는다.
*/
void XAccount::AddExpToHeros(int add, XLegion *pLegion, XVector<ID>* pOutAryLevelup )
{
	if (XBREAK(pLegion == nullptr))
		return;
	const auto typeTrain = XGAME::xTR_LEVEL_UP;
	// 군단내 영웅리스트를 얻는다.
	XVector<XSPHero> ary;
	pLegion->GetHerosToAry( &ary );
//	XARRAYLINEARN_LOOP(ary, XSPHero, pHero) {
	for( auto pHero : ary ) {
		if (!pHero->IsMaxLevelLevel()) {
			int lvMax = pHero->GetLvLimitByAccLv( GetLevel(), XGAME::xTR_LEVEL_UP );
			if( pHero->GetLevel() < lvMax ) {
				bool bLvUp = pHero->AddExp( typeTrain, add );
				if( bLvUp ) {
					if( pOutAryLevelup )
						pOutAryLevelup->Add( pHero->GetsnHero() );
					OnHeroLevelup( typeTrain, pHero );
				}
			}
		}
	}
}

/**
@brief clan징표 모두를 해당 클랜 책으로 교환한다.
*/
// ID XAccount::ChangeScalpToBook(XGAME::xtClan clan, XArrayLinearN<XBaseItem*, 256> *pOutAry)
// {
// 	// 해당클랜의 징표식별자를 얻는다.
// 	_tstring strIdentifier = XGAME::GetIdsClanScalp(clan);;
// 	XPropItem::xPROP *pPropScalp = PROP_ITEM->GetpProp(strIdentifier);
// 	if (pPropScalp == nullptr)
// 		return 0;
// 	// 해당 징표가 몇개나 있는지 검사
// 	int numTotal = GetNumItems(pPropScalp->idProp);
// 	// 최소 10개는 있어야 한다.
// 	if (numTotal >= 10)
// 	{
// 
// 		XPropItem::xPROP *pBookProp = PROP_ITEM->GetClanBookProp(clan);
// 		if (pBookProp == nullptr)	// recv에서 호출하는것이므로 만약 유저가 조작된 값을 보내서 계속 이함수를 호출할수도 있으므로 xbreak안씌움
// 			return 0;
// 		// 10개 단위로 징표 파괴시킴
// 		int numBook = (numTotal / 10);
// 		int numDel = numBook * 10;
// 		DestroyItem(pPropScalp->idProp, numDel);
// 		// 책 개수만큼 생성시킴
// 		//		ID idBook = pBookProp->idProp + ( clan - 1 );
// 		ID idBook = pBookProp->idProp;
// 		for (int i = 0; i < numBook; ++i)
// 		{
// 			XBaseItem *pBook = XBaseItem::sCreateItem(idBook);
// 			if (XASSERT(pBook))
// 			{
// 				//				XBREAK( pBookProp->param[0] <= 0 );
// 				//				pBook->SetBookExp( (XINT64)pBookProp->param[0] );
// 				pBook->SetBookExp(XGC->m_expPerBook);
// 				AddItem(pBook);
// 				if (pOutAry)
// 					pOutAry->Add(pBook);
// 			}
// 		}
// 		return pPropScalp->idProp;
// 	}
// 	return 0;
// }

/**
@brief szIdentifier아이템이 몇개나 있는지 알아낸다.
*/
int XAccount::GetNumItems(LPCTSTR szIdentifier)
{
	int num = 0;
	for (auto pBaseItem : m_listItem) {
		if (XE::IsSame(pBaseItem->GetszIdentifier(), szIdentifier))
			num += pBaseItem->GetNum();
	}
	return num;
}
int XAccount::GetNumItems(ID idProp)
{
	int num = 0;
	for (auto pBaseItem : m_listItem) {
		if (pBaseItem->GetidProp() == idProp)
			num += pBaseItem->GetNum();
	}
	return num;
}

/**
@brief szIdentifier아이템을 num개 삭제한다.
성공하면 1을 리턴. 실패하면 0을 리턴한다.
*/
int XAccount::DestroyItem(LPCTSTR szIdentifier, int num)
{
	auto pProp = PROP_ITEM->GetpProp(szIdentifier);
	if (XASSERT(pProp))
		return DestroyItem(pProp->idProp, num);
	return 0;
}

int XAccount::DestroyItem(ID idProp, int numDel)
{
	XBREAK(idProp == 0);
	// 템 삭제후 퀘스트에 이벤트 발생시킴
#ifndef _CLIENT
	if (idProp)
		m_pQuestMng->DispatchEventByDestroyItem(idProp, numDel);
#endif // not _CLIENT
	for( auto itor = m_listItem.begin(); itor != m_listItem.end(); ) {
		auto pItem = ( *itor );
		if( pItem->GetidProp() == idProp ) {
			if( numDel > 0 ) {
				int numStack = pItem->GetNum();
				// 쌓여있는 개수가 지워야하는 개수보다 작거나 같으면
				if( numStack <= numDel ) {
					// 슬롯을 통채로 지운다.
					m_listItem.erase( itor++ );
					// 만약 영웅이 갖고 있다면 장착해제시킨다.
					if( pItem->GetType() == XGAME::xIT_EQUIP )
						UpdateNewItemForHeros( pItem->GetsnItem(), nullptr );
					SAFE_DELETE( pItem );
					numDel -= numStack; //인벤토리 아이템 stack이 5개 밖에 안되기 때문에 이렇게 사용 하여도 무방.
					if( numDel == 0 )
						return 1;
					XBREAK( numDel < 0 );
					continue;
				} else {
					pItem->SetNum( numStack - numDel );
					numDel = 0;
				}
			}
			if( numDel <= 0 )
				return 1;
		}
		++itor;
	}
	XBREAKF(1, "XAccount::DestroyItem:failed destroy:idProp=%d, numDel=%d", idProp, numDel);
	return 0;
}

/**
@brief 아이템 시리얼번호로 삭제
겹치는 아이템도 삭제 가능하다.
@param num 겹치는 아이템의 경우 삭제할 개수(기본값은 1). 만약 num이 0이면 남아있는 개수에 상관없이 snItem만 맞으면 다 삭제시킨다.

*/
int XAccount::DestroyItemBySN(ID snItem, const int num)
{
	BOOL bFound = FALSE;

	for (auto itor = m_listItem.begin(); itor != m_listItem.end();) {
		auto pItem = (*itor);
		if (pItem->GetsnItem() == snItem) {
#ifdef _DEBUG
			XBREAK(pItem->GetpProp()->maxStack == 1 && bFound == TRUE);		// 이미 한번 지웠는데 또있다는 얘기임.
#endif // _DEBUG
			int numStack = pItem->GetNum();
			XBREAK(num > numStack);		// 삭제하려는 수량이 실제 있는 수량보다 클수는 없다.
			pItem->SetNum(numStack - num);
			bFound = TRUE;
#ifndef _CLIENT
			m_pQuestMng->DispatchEventByDestroyItem(pItem->GetidProp(), num);
#endif // not _CLIENT
			if (pItem->GetNum() == 0 || num == 0) {
				m_listItem.erase(itor++);
				// 만약 영웅이 갖고 있다면 장착해제시킨다.
				if (pItem->GetType() == XGAME::xIT_EQUIP)
					UpdateNewItemForHeros(pItem->GetsnItem(), nullptr);
				SAFE_DELETE(pItem);
			}
#ifdef _DEBUG
			else {
				// 디버깅 모드에서는 확인을 위해서 계속 루프를 돈다.
				++itor;
			}
			continue;
#else
			break;
#endif // not debug
		}
		++itor;
	}

	return (int)bFound;
}

/**
@brief 해당 클랜의 징표가 몇개나 있는지 알아낸다.
*/
// int XAccount::GetNumScalp(XGAME::xtClan clan, int grade)
// {
// 	_tstring strIdentifier = XGAME::GetIdsClanScalp(clan);
// 	int num = GetNumItems(strIdentifier.c_str());
// 	return num;
// }
// 
// /**
// @brief 해당클랜의 징표가 몇개나 있는지 검사.
// */
// int XAccount::GetNumClanBook(XGAME::xtClan clan, int grade)
// {
// 	_tstring strIdentifier = XGAME::GetIdsClanBook(clan);
// 	int num = GetNumItems(strIdentifier.c_str());
// 	return num;
// }
// 
// /**
// @brief clan의 클랜북의 경험치 총합
// */
// XINT64 XAccount::GetExpClanBooks(XGAME::xtClan clan, int grade)
// {
// 	XINT64 expSum = 0;
// 	for (auto pItem : m_listItem)
// 	{
// 		if (pItem && pItem->IsBook())
// 		{
// 			if (clan == pItem->GetBookClan())
// 				expSum += pItem->GetBookExp();
// 		}
// 	}
// 	return expSum;
// }



/**
@brief pProp아이템을 num개 만큼 생성해 인벤에 넣는다.
@param pAryOut null이 아니라면 생성한 아이템을 어레이에담아준다.
@return 생성한 아이템객체의 개수를 리턴한다.
*/
int XAccount::CreateItemToInven( const XPropItem::xPROP *pProp, int num,
																XArrayLinearN<XBaseItem*, 256> *pAryOut/* = nullptr*/)
{
	XBREAK(num <= 0);
	int numCreated = 0;
	// 겹치는 아이템인가
	if (pProp->maxStack > 1) {
		// 이미 같은 아이템이 있는지 검색해서 그곳에 우선으로 쌓는다.
		for( auto pItem : m_listItem ) {
			if( num == 0 )
				break;
			XBREAK( num < 0 );
			if( pItem->GetidProp() != pProp->idProp )
				continue;
			// 아직 더 쌓을수 있는 아이템이면
			int numStack = pItem->GetNum();
			if( numStack < pProp->maxStack ) {
				// num을 그대로 쌓았을때 넘치는가
				if( numStack + num > pProp->maxStack ) {
					pItem->SetNum( pProp->maxStack );	// 일단 최대치 만큼 쌓고
					num = ( numStack + num ) - pProp->maxStack;	// 오바되는 부분만 남은 개수로 남김
				} else
					// 안넘칠때는 그대로 더한다.
				{
					pItem->SetNum( numStack + num );
					num = 0;
				}
				// 변동이 생긴 아이템은 리스트에 넣는다.,
				if( pAryOut )
					pAryOut->Add( pItem );
			}
		}
		XBREAK(num < 0);
		// 그래도 개수가 남으면 새로 생성한다.
		if( num > 0 ) {
			int numObj = num / pProp->maxStack;	// 아이템을 몇개를 생성해야 하는가.
			int mod = num % pProp->maxStack;		// 그리고 나머지
			for( int i = 0; i < numObj; ++i ) {
				XBaseItem *pItem = XBaseItem::sCreateItem( pProp );
				if( XBREAK( pItem == nullptr ) )
					return -1;
				pItem->SetNum( pProp->maxStack );
				pItem = AddItem( pItem );
				if( XBREAK( pItem == nullptr ) )
					return -1;
				if( pAryOut )
					pAryOut->Add( pItem );
				++numCreated;
			}
			if( mod > 0 ) {
				XBaseItem *pItem = XBaseItem::sCreateItem( pProp );
				if( XBREAK( pItem == nullptr ) )
					return -1;
				pItem->SetNum( mod );
				pItem = AddItem( pItem );
				if( XBREAK( pItem == nullptr ) )
					return -1;
				if( pAryOut )
					pAryOut->Add( pItem );
				++numCreated;
			}
		}
	} else {
		// 안겹치는 아이템인가.
		// 그냥 개수만큼 객체를 생성하면 됨
		for (int i = 0; i < num; ++i) {
			auto pItem = CreateItemToInvenForNoStack(pProp);
			if (XBREAK(pItem == nullptr))
				return -1;
			if (pAryOut)
				pAryOut->Add(pItem);
			++numCreated;
		}
	}
	//	XBREAK( pAryOut && numCreated != pAryOut->size() );

	return numCreated;
}

int XAccount::CreateItemToInven( ID idProp, int num, 
								XArrayLinearN<XBaseItem*, 256> *pAryOut/* = nullptr*/ )
{
	auto pProp = PROP_ITEM->GetpProp( idProp );
	if( XBREAK( pProp == nullptr ) )
		return 0;
	return CreateItemToInven( pProp, num, pAryOut );
}

int XAccount::CreateItemToInven( const _tstring& idsItem, int num,
								XArrayLinearN<XBaseItem*, 256> *pAryOut/* = nullptr*/ )
{
	auto pProp = PROP_ITEM->GetpProp( idsItem );
	if( XBREAK( pProp == nullptr ) )
		return 0;
	return CreateItemToInven( pProp, num, pAryOut );
}

/**
@brief 안겹치는 아이템 1개 생성 전용 add함수
*/
XBaseItem* XAccount::CreateItemToInvenForNoStack( const XPropItem::xPROP *pProp)
{
	if (XBREAK(pProp->maxStack > 1))
		return nullptr;
	XBaseItem *pItem = XBaseItem::sCreateItem(pProp);
	if (XBREAK(pItem == nullptr))
		return nullptr;
	pItem = AddItem(pItem);
	if (XBREAK(pItem == nullptr))
	{
		SAFE_DELETE(pItem);
		return nullptr;
	}
	return pItem;
}

XBaseItem* XAccount::CreatePieceItemByidHero( ID idHero, int num )
{
	ID idPropPiece = GetidItemPieceByidHero( idHero );
	return CreatePieceItem( idPropPiece, num );
}

XBaseItem* XAccount::CreatePieceItem( ID idPropItem, int num )
{
	XArrayLinearN<XBaseItem*, 256> ary;
	CreateItemToInven( idPropItem, num, &ary );
	XBREAK( ary.size() == 0 );
	return ary[0];
}

ID XAccount::GetidItemPieceByidHero( ID idHero )
{
	return 60000 + idHero;
}

/**
@brief 인벤의 아이템을 얻는다
*/
XBaseItem* XAccount::GetItem(ID snItem)
{
	for (auto pItem : m_listItem) {
		if (pItem->GetsnItem() == snItem)
			return pItem;
	}
	return nullptr;
}

const XBaseItem* XAccount::GetpcItemBySN( ID snItem ) const
{
	for( auto pItem : m_listItem ) {
		if( pItem->GetsnItem() == snItem )
			return pItem;
	}
	return nullptr;
}

XBaseItem* XAccount::GetItem( LPCTSTR idsItem )
{
	for( auto pItem : m_listItem ) {
		if( pItem->GetstrIdentifier() == idsItem )
			return pItem;
	}
	return nullptr;
}

/**
@brief parts용 장착아이템 하나를 얻는다.
@param bExcludeEquiped 장착중인 템은 제외
*/
XBaseItem* XAccount::GetItemByEquip(XGAME::xtParts parts, bool bExcludeEquiped)
{
	if (XBREAK(parts == XGAME::xPARTS_NONE))	// invalid call
		return nullptr;
	for (auto pItem : m_listItem)
	{
		if (pItem->GetpProp()->parts == parts)
		{
			XASSERT(pItem->IsEquipable());
			// 장착중인템을 제외하는 옵션이면
			if (bExcludeEquiped)
			{
				// 장착된템인지 한번더 검사한다.
				if (!IsEquip(pItem->GetsnItem()))
					return pItem;	// 장착된템도 아니면 이걸로 리턴
			}
			else
				return pItem;
		}
	}
	return nullptr;
}


void XAccount::ChangeShopItemList()
{
	m_listShopSell.clear();
	//ShopProp 
	XList4<XPropItem::xPROP*> listItems;
	// 아직 5성은 안풀것임.
	for (int i = XGAME::xGD_EPIC; i >= 1; --i) {
		PROP_ITEM->GetpPropRandom((XGAME::xtGrade)i,
								XGAME::xIT_EQUIP,
								XGAME::MAX_SALE_GRADE_COUNT,
								&listItems);
	}
	for (auto pProp : listItems) {
		m_listShopSell.push_back( pProp->idProp );
	}
	std::vector<_tstring> aryIds = {_T( "hero_louis" ), _T( "hero_dorian" ),
									_T( "hero_fei" ), _T( "hero_enloque" ),
									_T( "hero_sinceridade" )};
	for( auto& idsSoul : aryIds ) {
		auto pPropItem = PROP_ITEM->GetpProp( idsSoul );
		if( XASSERT( pPropItem ) ) {
			m_listShopSell.push_back( pPropItem->idProp );
		}
	}
}
/**
 @brief idItemProp이 판매목록에 있는지 확인
*/
BOOL XAccount::IsSaleItemidProp(ID idItemProp)
{
// 	std::vector<ID>::iterator begin = m_listShopSell.begin();
// 	std::vector<ID>::iterator end = m_listShopSell.end();
// 
//	for( ; begin != end; ++begin) {
	for( auto idSell : m_listShopSell ) {
		if( idSell == idItemProp ) 
			return TRUE;
// 		if (*begin == idItemProp) return TRUE;
	}
	return FALSE;
}

#if defined(_CLIENT) || defined(_GAME_SERVER)
/**
@brief 현재 pHero영웅의 스킬 레벨업이 가능한 조건인가.
@param typeSkill 0:passive 1:active
@return 스킬레벨이 가능하면 XGAME::xES_OK를 리턴한다.
@see XGAME::xtSkillLevelUp
enum xtSkillLevelUp {
xES_OK = 1,					///< 레벨업 가능
xES_NO_HAVE_SKILL = -1,		///< 스킬이 없음
xES_FULL_LEVEL = -2,			///< 이미 만렙
xES_NOT_ENOUGH_ITEM = -3,	///< 아이템 부족
};
*/
XGAME::xtSkillLevelUp XAccount::GetAbleLevelupSkill(XSPHero pHero, XGAME::xtTrain type)
{
	if (pHero->GetSkillDat(type) == nullptr)
		return XGAME::xES_NO_HAVE_SKILL;
//	int level = pHero->GetLevelSkill(type);
	int level = pHero->GetLevel( type );
	if (level >= XGAME::GetMaxSkillLevel())		// 더이상 레벨업 할수 없음.
		return XGAME::xES_FULL_LEVEL;
	XBREAK(level <= 0 || level > XGAME::GetMaxSkillLevel());
	auto pProp = pHero->GetpPropSkillupNext(type);
	if (XBREAK(pProp == nullptr))
		return XGAME::xES_INVALID_CALL;
	if (pHero->GetLevel() < pProp->levelLimitByHero)
		return XGAME::xES_LIMITED_BY_HERO_LEVEL;
	return XGAME::xES_OK;
// 	ID idItem = SCROLL_GRAY + (pProp->gradeNeed - 1);
// 	int numHave = GetNumItems(idItem);
// 	if (numHave >= pProp->numItem)
// 		return XGAME::xES_OK;
// 	return XGAME::xES_NOT_ENOUGH_ITEM;
}

/**
@brief
*/
// BOOL XAccount::IsEmptyAbilMap()
// {
// 	if (m_aryUnitsAbil.size() == 0)
// 		return TRUE;
// 	return m_aryUnitsAbil[1].size() == 0;
// }

/**
@brief 현재 pHero에게 메달제공이 가능하냐
*/
// bool XAccount::IsAbleProvideSquad(XSPHero pHero, ID *pOutID, int *pOutNum)
// {
// 	if( IsLockHangout() ) {		// 병사집합소가 없으면 업글 안됨
// 		return false;
// 	}
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSquad = pHero->GetpPropSquadupNext();
// 	if (pPropSquad == nullptr)
// 		return false;
// 	if (pHero->IsAbleProvideSquad(pPropSquad) == false)
// 		return false;
// 	if (pHero->GetLevel() < pPropSquad->levelLimitByHero)
// 		return false;
// 	int numItem = 0;
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSquadLvupItem(pHero->GetType());
// 	if (XBREAK(idNeed == 0))
// 		return false;
// 	idNeed += (pPropSquad->gradeNeed - 1);
// 	if (pOutID)
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if (pOutNum)
// 		*pOutNum = pPropSquad->numItem;
// 	// 소지하고 있는 해당메달수를 센다.
// 	int num = GetNumItems(idNeed);
// 	// 메달 없으면 안됨.
// 	if (num <= 0)
// 		return false;
// 	return true;
// }
// 
// /**
// @brief 현재 pHero에게 메달제공이 가능하냐
// */
// bool XAccount::IsAbleProvideSkill(XSPHero pHero, XGAME::xtTrain type, ID *pOutID, int *pOutNum)
// {
// 	if( IsLockAcademy() ) {		// 병사집합소가 없으면 업글 안됨
// 		return false;
// 	}
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSkill = pHero->GetpPropSkillupNext(type);
// 	if (pPropSkill == nullptr)
// 		return false;
// 	// 먼저 영웅걸로 검사해봄
// 	if (pHero->IsAbleProvideSkill(pPropSkill, type) == false)
// 		return false;
// 	// 통과하면 계정에 필요아이템이 있는지 확인
// 	int numItem = 0;
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSkillLvupItem(pPropSkill->gradeNeed);
// 	if (XBREAK(idNeed == 0))
// 		return false;
// 	if (pOutID)
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if (pOutNum)
// 		*pOutNum = pPropSkill->numItem;
// 	// 소지하고 있는 해당메달수를 센다.
// 	int num = GetNumItems(idNeed);
// 	// 메달 없으면 안됨.
// 	if (num <= 0)
// 		return false;
// 	return true;
// }

/**
@brief pHero가 현재 업그레이드가 가능한 상태인가.
pHero가 렙업할수 있는 충분한양의 메달을 모은 상태
*/
// bool XAccount::IsAbleLevelUpSquad(XSPHero pHero, ID *pOutID, int *pOutNum)
// {
// 	if( IsLockHangout() ) {		// 병사집합소가 없으면 업글 안됨
// 		return false;
// 	}
// 	// 업글 아이템이 충분히 있는가.
// 	auto pPropSquad = pHero->GetpPropSquadupNext();
// 	if (pPropSquad == nullptr)
// 		return false;
// 	// 일단 최대 레벨이면 더이상 업글 안됨
// 	if (pHero->GetlevelSquad() >= PROP_SQUAD->GetMaxLevel())
// 		return false;
// 	// 영웅 렙제에 걸리면 더이상 안됨.
// 	if (pHero->GetLevel() < pPropSquad->levelLimitByHero)
// 		return false;
// 	int numItem = 0;
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSquadLvupItem(pHero->GetType(), pPropSquad->gradeNeed);
// 	XBREAK(idNeed == 0);
// 	// 	idNeed += (pPropSquad->gradeNeed - 1);
// 	// 소지한 업글템의 개수를 셈
// 	if (pOutID)
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if (pOutNum)
// 		*pOutNum = pPropSquad->numItem;
// 	// 소지하고 있는 해당메달수를 센다.
// 	int num = GetNumItems(idNeed);
// 	// 소지하고 있는걸로 남은 메달을 다 채울 수 있으면 true
// 	if (num < pPropSquad->numItem - pHero->GetNumProvidedMedal())
// 		return false;
// 	return true;
// }
bool XAccount::IsAbleLevelUpSquad( XSPHero pHero )
{
	if( IsLockHangout() ) {		// 병사집합소가 없으면 업글 안됨
		return false;
	}
	const auto typeTrain = XGAME::xTR_SQUAD_UP;
	// 업글 아이템이 충분히 있는가.
	auto pPropSquad = pHero->GetpPropSquadupNext();
	if( pPropSquad == nullptr )
		return false;
	// 일단 최대 레벨이면 더이상 업글 안됨
// 	if( pHero->Getlevel( typeTrain ) >= PROP_SQUAD->GetMaxLevel() )
// 		return false;
	if( pHero->GetLevel( typeTrain ) >= pHero->GetMaxLevel( typeTrain ) )
		return false;
	// 영웅 렙제에 걸리면 더이상 안됨.
	if( pHero->GetLevel() < pPropSquad->levelLimitByHero )
		return false;
	int numItem = 0;
	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSquadLvupItem( pHero->GetType(), pPropSquad->gradeNeed );
// 	XBREAK( idNeed == 0 );
// 	// 	idNeed += (pPropSquad->gradeNeed - 1);
// 	// 소지한 업글템의 개수를 셈
// 	if( pOutID )
// 		*pOutID = idNeed;	// UI등에 사용하기 위해 필요한 아이템의 아이디를 넣어줌.
// 	if( pOutNum )
// 		*pOutNum = pPropSquad->numItem;
// 	// 소지하고 있는 해당메달수를 센다.
// 	int num = GetNumItems( idNeed );
// 	// 소지하고 있는걸로 남은 메달을 다 채울 수 있으면 true
// 	if( num < pPropSquad->numItem - pHero->GetNumProvidedMedal() )
// 		return false;
	return true;
}


/**
@brief 영웅중에 부대업글이 가능한 영웅이 있는가.
*/
bool XAccount::IsAbleLevelUpSquad()
{
	for (auto pHero : m_listHero) {
		if (IsAbleLevelUpSquad(pHero))
			return true;
	}
	return false;
}

/**
@brief 영웅중에 레벨업할수 있는게 있는지 검사.
*/
bool XAccount::IsAbleLevelUpHero()
{
	for (auto pHero : m_listHero) {
		if (IsAbleLevelUpHero(pHero))
			return true;
	}
	return false;
}

/**
@brief pHero가 현재 레벨업할수 있는지 검사.
*/
bool XAccount::IsAbleLevelUpHero(XSPHero pHero)
{
	if (GetLevel() <= pHero->GetLevel())
		return false;
	const auto typeTrain = XGAME::xTR_LEVEL_UP;
	if( pHero->IsFullExp( typeTrain ) )		// exp가 가득차면 터치해서 렙옵시킬수 있다.
		return true;
// 	/// 현재소지한 이영웅의 클랜북의 exp총합.
// 	auto expTotal = GetExpClanBooks(pHero->GetClan());
// 	auto expRemain = pHero->GetXFLevelObj().GetExpRemain();
// 	if (expTotal > expRemain)
// 		return true;
	return false;
}

/**
@brief 렙업레디중인 영웅 목록 얻기
*/
// int XAccount::GetLevelupReadyHeroes(XArrayLinearN<XSPHero, 256> *pOutAry)
// {
// 	for (auto pHero : m_listHero) {
// 		if (pHero->IsAnyLevelupReady())
// 			pOutAry->Add(pHero);
// 	}
// 	return pOutAry->size();
// }

/**
@brief 영웅중 한명이 스킬렙업 가능한 상태가 되었다.
*/
bool XAccount::GetAbleLevelupSkill()
{
	for (auto pHero : m_listHero) {
		if (GetAbleLevelupSkill(pHero, XGAME::xTR_SKILL_PASSIVE_UP) == XGAME::xES_OK)
			return true;
		if (GetAbleLevelupSkill(pHero, XGAME::xTR_SKILL_ACTIVE_UP) == XGAME::xES_OK)
			return true;
	}
	return false;
}

/**
@brief 영웅중 한명이 부대렙업 가능한 상태가 되었다.
*/
bool XAccount::IsAbleUpgradeHero()
{
	if (IsAbleLevelUpHero())
		return true;
	if (IsAbleLevelUpSquad())
		return true;
	if (GetAbleLevelupSkill())
		return true;


	return false;
}

/**
@brief pHero영웅이 현재 업그레이드 가능한 상태인가
*/
bool XAccount::IsAbleUpgradeHeroAny(XSPHero pHero)
{
	// 레벨 업
	// 일단 렙업쪽은 렙업 훈련중이 아니어야 한다.
	if (IsAbleLevelUpHero(pHero)
		&& !IsTrainingHero(pHero->GetsnHero(), XGAME::xTR_LEVEL_UP))
		return true;
	// 부대업
	if (IsAbleLevelUpSquad(pHero)
		&& !IsTrainingHero(pHero->GetsnHero(), XGAME::xTR_SQUAD_UP))
		return true;
	if( !IsLockAcademy() ) {
		// 스킬업
		if( GetAbleLevelupSkill( pHero, XGAME::xTR_SKILL_PASSIVE_UP ) == XGAME::xES_OK
			&& !IsTrainingHero( pHero->GetsnHero(), XGAME::xTR_SKILL_PASSIVE_UP ) )
			return true;
		if( GetAbleLevelupSkill( pHero, XGAME::xTR_SKILL_ACTIVE_UP ) == XGAME::xES_OK
			&& !IsTrainingHero( pHero->GetsnHero(), XGAME::xTR_SKILL_ACTIVE_UP ) )
			return true;
	}
	return false;
}
/**
 @brief gold로 얻을수 있는 exp양
*/
int XAccount::GetTrainExpByGold( int lvHero, int gold, XGAME::xtTrain typeTrain )
{
	// 일단 1exp당 10골드로 계산
	switch( typeTrain ) {
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetExpTrainByGold( gold, lvHero );
// 		return gold / 10;
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetExpTrainByGold( gold );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetExpTrainByGold( gold );
// 		return gold / 100;
	default:
		XBREAK(1);
		break;
	}
	return 0;
}
/**
 @brief gold에 의한 exp를 계산하는데 영웅의 현재레벨과 오르게될 레벨을 반영한 비용으로 계산된다.
 @param pOutExp 금화에 의해 얻을수 있는 경험치 총량
 @param pOutSec 금화에 의해 훈련할수 있는 총 시간(초)
 @param pOutGold 보통은 0이지만 레벨제한에 걸리는등의 이유로 더이상 경험치를 주지못할때 환불될 금액
*/
void XAccount::GetTrainExpByGoldCurrLv( XSPHero pHero, 
										int goldBase, 
										XGAME::xtTrain typeTrain, 
										int *pOutExp, 
										int *pOutSec,
										int *pOutGold )
{
	int lvHero = pHero->GetLevel( typeTrain );
	int gold = goldBase;
	int expSum = 0;
	int secSum = 0;
	// 현재 루프레벨의 최대경험치를 구한다.
	int expCurr = pHero->GetExp( typeTrain );
	while( gold > 0 ) {
		// 최대레벨에선 경험치가 더이상 오르지 않음.
		if( lvHero >= XGAME::MAX_HERO_LEVEL )
			break;
		const int expByGold = GetTrainExpByGold( lvHero, gold, typeTrain );
		if( expByGold <= 0 ) {
			gold = 0;	// 짜투리 남는건 버림
			break;
		}
		int expMax = pHero->GetMaxExpWithLevel( typeTrain, lvHero );
		int expRemain = expMax - expCurr;
		// 더해야할 exp가 남은 exp보다 크면 렙업시키고 그만큼 깐다.
		if( expByGold > expRemain ) {
			// 현재레벨에서 남은 경험치를 gold로 환산
			int goldByExpRemain = GetGoldByExp( lvHero, expRemain, typeTrain );
			if( goldByExpRemain <= 0 )
				goldByExpRemain = 1;	// 최소 1이되도록
			gold -= goldByExpRemain;	// 처음 금액에서 그만큼 뺌
			expSum += expRemain;
			// 더한 경험치 분량의 시간을 누적시킨다.
			int secByAdded = GetsecTrainHero( lvHero, expRemain, typeTrain );
			if( secByAdded <= 0 )
				secByAdded = 1;
			secSum += secByAdded;
			int lvLimit = pHero->GetLvLimitByAccLv( GetLevel(), typeTrain );
			if( lvHero >= lvLimit )  {
				// 레벨 한계에 걸리면 더이상 경험치를 주면 안된다.
				// 돈도 그만큼 까야한다.
				//--expSum;	// 1을 빼줘야 렙업을 하지 않음.
				break;
			} else {
				++lvHero;	// 렙업
				expCurr = 0;
			}
		} else {
			expCurr += expByGold;
			expSum += expByGold;
			int secByAdded = GetsecTrainHero( lvHero, expByGold, typeTrain );
			if( secByAdded <= 0 )
				secByAdded = 1;
			secSum += secByAdded;
			// 더해진 경험치만큼의 가격을 gold에서 뺀다.
			int goldByExpAdded = GetGoldByExp( lvHero, expByGold, typeTrain );
			if( goldByExpAdded <= 0 )
				goldByExpAdded = 1;		// 최소 1원이 되도록
			gold -= goldByExpAdded;
		}
		// 돈이 남았으면 계속 루프돈다.
	}
	XBREAK( expSum <= 0 );
	XBREAK( secSum <= 0 );
	if( pOutExp )
		*pOutExp = expSum -	1;		// max로 경험치를 올릴때 마지막 1때문에 레벨업이 되버려서 1을 빼줌.
	if( pOutSec )
		*pOutSec = secSum;
	if( pOutGold )
		*pOutGold = gold;	// 환불 금액. 보통은 0
}


/**
 @brief exp를 금화로 환산한다.
*/
int XAccount::GetGoldByExp( int lvHero, int exp, XGAME::xtTrain typeTrain )
{
	switch( typeTrain )	{
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetGoldByExp( exp, lvHero );
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetGoldByExp( exp );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetGoldByExp( exp );
	default:
		XBREAK( 1 );
		break;
	}
	return 0;
}

/**
 @brief lvHero레벨의 최대exp를 금화로 환산한다.
*/
int XAccount::sGetGoldByMaxExp( int lvHero, XGAME::xtTrain typeTrain )
{
	const int expMax = XHero::sGetMaxExpWithLevel( typeTrain, lvHero );
	switch( typeTrain )	{
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetGoldByExp( expMax, lvHero );
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetGoldByExp( expMax );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetGoldByExp( expMax );
	default:
		XBREAK( 1 );
		break;
	}
	return 0;
}
/**
 @brief exp훈련을 하기위한 시간(초)를 돌려준다.
*/
int XAccount::GetsecTrainHero( int lvHero, int exp, XGAME::xtTrain typeTrain )
{
	switch( typeTrain ) {
	case XGAME::xTR_LEVEL_UP:
		return EXP_TABLE_HERO->GetSecByExp( exp, lvHero );
	case XGAME::xTR_SQUAD_UP:
		return PROP_SQUAD->GetSecByExp( exp );
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		return XPropUpgrade::sGet()->GetSecByExp( exp );
	default:
		XBREAK(1);
		break;
	}
	return 0;
}
int XAccount::GetAPPerBattle()
{
	return XPropUser::sGet()->GetTable( GetLevel() )->apPerBattle;
}

// xClient xGameServer
#endif  // CLIENT || GAMESERVER용맹
/**
@brief lvAcc레벨에 받을수 있는 특성포인트 최대값
*/
int XAccount::sGetMaxTechPoint(int lvAcc)
{
	if (XBREAK(lvAcc > XGAME::GetLevelMaxAccount()))
		lvAcc = XGAME::GetLevelMaxAccount();
	int point = (lvAcc - 1) * 3;    // 소형유닛 포인트
	if (point > 25 * 3)
		point = 25 * 3;     // 최대치 25를 넘어갈수 없다.
	if (lvAcc >= 12) {
		for (int i = 4; i < 10; ++i) {
			int p = ((lvAcc - XAccount::sGetLevelByUnlockUnit(i)) + 1);
			if (p > 0) {
				if (p > 25)
					p = 25;
				point += p;
			}
		}
	}
	return point;
}

/**
 @brief 빈슬롯이 있다면 그것은 전투력에 포함하지 않는 전투력을 얻는다.
*/
int XAccount::GetPowerExcludeEmpty()
{
	if( m_PowerExcludeEmpty == 0 )
		UpdatePower();
	return m_PowerExcludeEmpty;
}
/**
 @brief 빈슬롯이 있다면 가장쎈 영웅의 전투력으로 포함시켜 계산한 값으로 돌려준다.(이것은 오프라인시 매치당할때의 어뷰징을 방지하기 위함이다)
*/
int XAccount::GetPowerIncludeEmpty()
{
	if( m_PowerIncludeEmpty == 0 )
		UpdatePower();
	return m_PowerIncludeEmpty;
}

/**
@brief 총 군사력을 얻는다.
*/
int XAccount::GetMilitaryPower()
{
	float score = 0;
	auto spLegion = GetCurrLegion();
	XVector<XSPHero> aryHeroes;
	spLegion->GetHerosToAry( &aryHeroes );
	std::vector<XSPHero> aryHeroesEtc;   // 메인군단에 속하지 않은 나머지 영웅들
	for( auto pHero : m_listHero ) {
		bool bMain = false;
//		XARRAYLINEARN_LOOP_AUTO( aryHeroes, pHeroInLegion ) {
		for( auto pHeroInLegion : aryHeroes ) {
			if( pHeroInLegion->GetsnHero() == pHero->GetsnHero() ) {
				bMain = true;
				break;
			}
		}
		// 메인군단에 속하지 않은 영웅만 어레이에 넣는다.
		if( !bMain )
			aryHeroesEtc.push_back( pHero );
	}
	// 메인 군단에 속한 영웅들의 전투력 합산
//	XARRAYLINEARN_LOOP_AUTO( aryHeroes, pHero ) {
	for( auto pHero : aryHeroes ) {
		score += XLegion::sGetMilitaryPower( pHero );
	}
	for( auto pHero : aryHeroesEtc ) {
		// 메인군단에 속하지 않은 영웅들의 전투력은 10%만 취한다.
		score += ( XLegion::sGetMilitaryPower(pHero ) * 0.1f ); 
	}
	return (int)score;
}

int XAccount::GetMilitaryPowerCurrLegion() 
{
	return XLegion::sGetMilitaryPower( GetCurrLegion() );
}

void XAccount::UpdatePower() 
{
	auto spLegion = GetCurrLegion();
	if( XASSERT( spLegion != nullptr ) ) {
		m_PowerExcludeEmpty = XLegion::sGetMilitaryPower( spLegion );
		m_PowerIncludeEmpty = m_PowerExcludeEmpty;
		// 만약 부대슬롯을 다 채우지 않은 상태라면 가장쎈 영웅의 전투력으로 메운다.
		int powerMax = GetPowerMaxInHeroes();
		const int maxSquad = XAccount::sGetMaxSquadByLevelForPlayer( GetLevel() );
		int numRemain = maxSquad - spLegion->GetNumSquadrons();
		if( numRemain > 0 ) {
			m_PowerIncludeEmpty += ( powerMax * numRemain );
		}
		if( m_PowerExcludeEmpty < m_PowerIncludeEmpty ) {
			// 빈슬롯을 제외한 전투력이 포함한 전투력보다 작다. 이것은 슬롯을 비워놨다는뜻이다.
			// 케이스2가지. 영웅이 있는데도 고의로 안채운경우. 영웅이 부족해서 안채운경우.
			if( GetNumHeroes() < maxSquad ) {
				// 영웅이 부족해서 안채운 경우
				// 현재 보유한 모든 영웅들의 전투력을 합산해서 가진다.
				m_PowerExcludeEmpty = 0;
				for( auto pHero : m_listHero ) {
					const auto powerHero = XLegion::sGetMilitaryPower( pHero );
					m_PowerExcludeEmpty += powerHero;
				}
			} else {
				// 고의로 안채운경우
				m_PowerExcludeEmpty = m_PowerIncludeEmpty;
			}
		}
	}
}
/**
 @brief  내가 보유한 영웅중에 가장 전투력이 쎈 영웅의 전투력을 얻음.
*/
int XAccount::GetPowerMaxInHeroes()
{
	int powerMax = 0;
	for( auto pHero : m_listHero ) {
		int power = XLegion::sGetMilitaryPower( pHero );
		if( power > powerMax )
			powerMax = power;
	}
	return powerMax;
}

// void XAccount::InitAbilMap()
// {
// #if defined(_DB_SERVER) || defined(_LOGIN_SERVER)
// 	std::map<ID, xAbil> mapNew;
// 	for (int i = 0; i < XGAME::xUNIT_MAX; ++i) {
// 		m_aryUnitsAbil.Add(mapNew);	// 0번 인덱스
// 	}
// #else
// 	// 다른서버에서 serialize로 만들어 보내기때문에 0이면 안됨.
// 	XBREAK(m_aryUnitsAbil.size() == 0);
// 	XArrayLinearN<XPropTech::xNodeAbil*, 256> ary;
// 	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
// 		XGAME::xtUnit unit = (XGAME::xtUnit) i;
// 		ary.Clear();
// 		XPropTech::sGet()->GetNodesToAry(unit, &ary);
// 		std::map<ID, xAbil>& mapAbil = m_aryUnitsAbil[unit];
// 		XARRAYLINEARN_LOOP(ary, XPropTech::xNodeAbil*, pNodeAbil) {
// 			xAbil abil;
// // 			if (pNodeAbil->GetSizeUnit() == XGAME::xSIZE_SMALL
// // 				&& pNodeAbil->tier == 1)
// // 			if( pNodeAbil->tier == 1 )
// // 				abil.point = 0;			// 티어1 특성은 기본적으로 열려있음.
// // 			else
// // 				abil.point = -1;			// 그 외는 모두 잠김 상태.
// 			mapAbil[pNodeAbil->idNode] = abil;
// 		} END_LOOP;
// 	}
// #endif
// }

/**
@brief 특성맵 시리얼라이즈
*/
int XAccount::SerializeAbil(XArchive& ar)
{
#if defined(_CLIENT) || defined(_GAME_SERVER)
// 	if (XBREAK(m_aryUnitsAbil.size() == 0))
// 		return 0;
#else
// 	if (m_aryUnitsAbil.size() == 0)
// 		InitAbilMap();
#endif
	ar << VER_ABILITY_SERIALIZE;
	ar << m_Researching;
// 	m_timerResearch.Serialize(ar);
// 	ar << m_idAbilResearchNow;
// 	ar << m_secResearchComplete;
// 	for (int i = 1; i < XGAME::xUNIT_MAX; ++i)
// 	{
// 		XGAME::xtUnit unit = (XGAME::xtUnit) i;
// 		std::map<ID, xAbil>& mapAbil = m_aryUnitsAbil[unit];
// 		std::map<ID, xAbil>::iterator itor;
// 		ar << (int)mapAbil.size();
// 		for (itor = mapAbil.begin(); itor != mapAbil.end(); ++itor) {
// 			xAbil& abil = (*itor).second;
// 			ID idNode = (*itor).first;
// 			XBREAK(idNode >= 0xffff);
// 			ar << (char)abil.point;
// 			ar << (BYTE)0;
// 			ar << (WORD)idNode;
// 		}
// 	}
	return 1;
}

int XAccount::DeserializeAbil(XArchive& ar)
{
	int ver = 0;
	ar >> ver;
	ar >> m_Researching;
// 	m_timerResearch.DeSerialize(ar, ver);
// 	ar >> m_idAbilResearchNow;
// 	ar >> m_secResearchComplete;

// 	for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
// 		XGAME::xtUnit unit = (XGAME::xtUnit) i;
// 		std::map<ID, xAbil>& mapAbil = m_aryUnitsAbil[unit];
// //		std::map<ID, xAbil>::iterator itor;
// 		int size = 0;
// 		ar >> size;
// 		for (int i = 0; i < size; ++i) {
// 			char c0;
// 			WORD w0;
// 			xAbil abil;
// 			ID idNode;
// 			ar >> c0;	abil.point = c0;
// 			XBREAK(abil.point > 5);
// 			if (abil.point == 255) {
// 				abil.point = -1;
// 				XBREAK(abil.point != -1);
// 			}
// 			ar >> c0;
// 			ar >> w0;	idNode = w0;
// #ifdef _GAME_SERVER
// // 			auto pNodeProp = XPropTech::sGet()->GetpNode( idNode );
// // 			if( pNodeProp && pNodeProp->tier == 1 )
// // 				if( abil.point == -1 )
// // 					abil.point = 0;
// #endif
// 			mapAbil[idNode] = abil;
// 		}
// 	}
	return 1;
}

void XAccount::CreateQuestMng(void)
{
	XBREAK( m_pQuestMng != nullptr );
	m_pQuestMng = new XQuestMng;
	XBREAK(m_pQuestMng == nullptr);
}

void XAccount::SetQuestDelegate(XDelegateQuestMng *pDelegate)
{
	if (XBREAK(m_pQuestMng == nullptr))
		return;
	m_pQuestMng->SetpDelegate(pDelegate);
}
/**
 @brief 현재 type자원의 양을 얻는다.
*/
int XAccount::GetResource( XGAME::xtResource type ) const
{				
	switch( type )
	{
	case XGAME::xRES_WOOD:
	case XGAME::xRES_IRON:
	case XGAME::xRES_JEWEL:
	case XGAME::xRES_SULFUR:
	case XGAME::xRES_MANDRAKE:
		return m_aryResource[ type ];
	case XGAME::xRES_GOLD:
		return GetGold();
	case XGAME::xRES_CASH:
		return GetCashtem();
	case XGAME::xRES_GUILD_COIN:
		return GetGuildCoin();
	default:
		XBREAK(1);
		break;
	}
	return 0;
}

#if defined(_CLIENT) || defined(_DEV)
// 강제 동기화. 클라에서만 사용
void XAccount::SetResource( XGAME::xtResource type, int num ) 
{
	switch( type )
	{
	case XGAME::xRES_WOOD:
	case XGAME::xRES_IRON:
	case XGAME::xRES_JEWEL:
	case XGAME::xRES_SULFUR:
	case XGAME::xRES_MANDRAKE: {
	// XBREAK(type < 0 || type > XGAME::xRES_MAX);
		if( XBREAK( XGAME::IsInvalidResource( type ) ) )
			return;
		m_aryResource[ type ] = num;
	} break;
	case XGAME::xRES_GOLD:
		SetGold( (DWORD)num );
	case XGAME::xRES_CASH:
		SetCashtem( (DWORD)num );
	case XGAME::xRES_GUILD_COIN:
		SetptGuild( (DWORD)num );
	default:
		XBREAK( 1 );
		break;
	}
}
#endif // defined(_CLIENT) || defined(_DEV)

/**
@brief 자원을 증/감 시킨다.
*/
int XAccount::AddResource(XGAME::xtResource type, int amount)
{
	int curr;
	switch( type )
	{
	case XGAME::xRES_WOOD:
	case XGAME::xRES_IRON:
	case XGAME::xRES_JEWEL:
	case XGAME::xRES_SULFUR:
	case XGAME::xRES_MANDRAKE: {
		if( XGAME::IsInvalidResource(type) )
			return -1;
		if (amount < 0 && m_aryResource[type] < -amount)
			return -1;
		if (amount == 0)
			return m_aryResource[type];
		curr = m_aryResource[type] += (int)amount;
	} break;
	case XGAME::xRES_GOLD:
		curr = AddGold( amount );
	case XGAME::xRES_CASH:
		curr = AddCashtem( amount );
	case XGAME::xRES_GUILD_COIN:
		curr = AddGuildPoint( amount );
	default:
		curr = -1;
		XBREAK( 1 );
		break;
	}
#ifdef _GAME_SERVER
	if (amount > 0)	{
		xQuest::XEventInfo infoQuest;
		infoQuest.SettypeResource(type);
		infoQuest.SetAmount(amount);
		m_pQuestMng->DispatchEvent(XGAME::xQC_EVENT_GET_RESOURCE, infoQuest);
	}
#endif
	return curr;
}

// XAccount::xOption::xOption()
// {
// 	bSound = TRUE;
// 	//bInvite = TRUE;
// 	nLang = 1;
// }
// 
// void XAccount::xOption::Save()
// {
// 	FILE *fp = nullptr;
// 	LPCTSTR _szPath = XE::MakeDocFullPath(_T(""), _T("option.txt"));
// 	std::string strPath = SZ2C(_szPath);
// 	fopen_s(&fp, strPath.c_str(), "wt");
// 	fprintf_s(fp, "%d\n%d", bSound, nLang);
// 	fclose(fp);
// }
// 
// void XAccount::xOption::Load()
// {
// 	FILE *fp = nullptr;
// 	LPCTSTR _szPath = XE::MakeDocFullPath(_T(""), _T("option.txt"));
// 	std::string strPath = SZ2C(_szPath);
// 	fopen_s(&fp, strPath.c_str(), "rt");
// 	if (fp != NULL)
// 	{
// 		fscanf_s(fp, "%d %d", &bSound, &nLang);
// 		fclose(fp);
// 	}
// 	else
// 		Save();
// }

BOOL XAccount::AddGuildJoinReq(ID joinreqguildidx)
{
	if (GetGuildIndex() > 0)
		return FALSE;
	if (GetGuildgrade() > 0)
		SetGuildgrade(XGAME::xtGuildGrade::xGGL_NONE);

	if( IsReqJoinGuild( joinreqguildidx ) )
		return FALSE;
// 	std::list< ID  >::iterator itor = m_ReqJoinGuildList.begin();
// 	for (; itor != m_ReqJoinGuildList.end(); itor++)
// 	{
// 		if (joinreqguildidx == (*itor)) return FALSE;
// 	}
	m_listGuildJoinReq.push_back(joinreqguildidx);
	return TRUE;
}

BOOL XAccount::IsReqJoinGuild(ID joinreqguildidx) const
{
// 	auto itor = m_ReqJoinGuildList.begin();
// 	for (; itor != m_ReqJoinGuildList.end(); itor++)
	for( auto idGuild : m_listGuildJoinReq ) {
		if (joinreqguildidx == idGuild ) 
			return TRUE;
	}
	return FALSE;
}

/**
@brief 현재레벨에서 선택가능한 모든 유닛에게 테크포인트 1씩 지급한다.
*/
// void XAccount::AddTechPointAll(int add)
// {
// 	int size = m_aryTechPoint.size();
// 	for (int i = 1; i < size; ++i)
// 	{
// 		auto unit = (XGAME::xtUnit) i;
// 		if (!IsLockUnit(unit))
// 			m_aryTechPoint[i] += add;
// 	}
// }

/**
@brief level에서 unit를 선택가능하냐
*/
// bool XAccount::sIsSelectable( XGAME::xtUnit unit, int level )
// {
// 	auto pProp = PROP_UNIT->GetpProp( unit );
// 	if( XBREAK( pProp == nullptr ) )
// 		return false;
// 	return sIsSelectable( pProp->size, level );
// }
// bool XAccount::sIsSelectable( XGAME::xtSize size, int level )
// {
// 	if( size == XGAME::xSIZE_SMALL )
// 		return true;
// 	else
// 	if( size == XGAME::xSIZE_MIDDLE && level >= XGAME::LEVEL_MIDDLE_UNIT )
// 		return true;
// 	else
// 	if( size == XGAME::xSIZE_BIG && level >= XGAME::LEVEL_BIG_UNIT )
// 		return true;
// 
// 	return false;
// }

/**
@brief 계정레벨에 따른 부대수 제한
*/
int XAccount::sGetMaxSquadByLevelForPlayer(int level)
{
	XBREAK( level == 0 );
	int maxSquad = 0;
	if (level < 4)		maxSquad = 3;
	else if (level < 9)		maxSquad = 4;
	else if (level < 12)		maxSquad = 5;
	else if (level < 15)		maxSquad = 6;
	else if (level < 20)		maxSquad = 7;
	else if (level < 25)		maxSquad = 8;
	else if (level < 30)		maxSquad = 9;
	else if (level < 34)		maxSquad = 10;
	else if (level < 38)		maxSquad = 11;
	else if (level < 42)		maxSquad = 12;
	else if (level < 46)		maxSquad = 13;
	else if (level < 50)		maxSquad = 14;
	else
		maxSquad = 15;
	return maxSquad;
}

/**
@brief 현재 군단에 자리가 남는가.
*/
bool XAccount::IsRemainSquad()
{
	auto pLegion = GetCurrLegion();
	if (pLegion)
	{
		if (pLegion->GetNumSquadrons() < sGetMaxSquadByLevelForPlayer(GetLevel()))
			return true;
		return false;
	}
	return true;	// 이런경우는 없을거 같지만 생긴다면 알림으로 알려주는것도 갠차늠.
}

#if defined(_CLIENT) || defined(_GAME_SERVER)
/**
@brief pHero의 pProp특성이 현재 활성화 된 특성인지 검사.
*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
bool XAccount::IsEnableAbil( XSPHero pHero, XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp)
{
	// 소형 티어1 특성은 무조건 활성화
	if (pProp->GetSizeUnit() == XGAME::xSIZE_SMALL && pProp->tier == 1)
		return true;
	// 안되는 조건들부터 걸러낼것.
	// 유닛이 잠겨있으면 안됨
	if (IsLockUnit(unit))
		return false;
	// 레벨이 안되면 비활성
	if (pHero->GetLevel() < pProp->GetLvOpanable())
		return false;
	for (auto pParent : pProp->GetListParents()) {
		// 부모가 루트라면 pProp은 활성화된것.
		if (pParent->IsRoot())
			return true;
		// 이 노드의 부모중 하나라도 포인트 찍은게 있으면 이노드를 활성화시킴
		const auto abil = pHero->GetAbilNode(unit, pParent->idNode);
		if (abil.point > 0)
			return true;
	}
	return false;
}
#endif // defined(_CLIENT) || defined(_GAME_SERVER)

/**
@brief pProp특성이 현재 잠금을 해제할수 있는지 확인.
*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
bool XAccount::IsUnlockableAbil(XSPHero pHero
															, XGAME::xtUnit unit
															, XPropTech::xNodeAbil *pProp)
{
	const auto err = GetUnlockableAbil( pHero, unit, pProp );
	return err == XGAME::xE_OK;
}
#endif // defined(_CLIENT) || defined(_GAME_SERVER)
/**
 @brief pHero가 특성언락을 할수 있는지 에러코드를 얻는다.
*/
#if defined(_CLIENT) || defined(_GAME_SERVER)
XGAME::xtError XAccount::GetUnlockableAbil( XSPHero pHero
																					, XGAME::xtUnit unit
																					, XPropTech::xNodeAbil *pProp)
{
	if( pHero->GetTypeAtk() != XGAME::GetAtkType( unit ) )
		return XGAME::xE_NOT_ENOUGH_AUTHORITY;
	// 모든 유닛의 특성포인트 합을 구함.
	int numUnlock = pHero->GetNumUnlockAbil();
	// 남은 언락포인트
	const int numRemainUnlock = pHero->GetnumRemainAbilUnlock();
	// 언락포인트가 있으면 금화검사는 안하고 넘어감.
	if( numRemainUnlock == 0 ) {
		// 금화가 부족하면 해제 못함.
		if( m_Gold < (DWORD)XPropTech::sGet()->GetGoldUnlockWithNum( numUnlock ) )
			return XGAME::xE_NOT_ENOUGH_GOLD;
	}
	if( pHero->GetLevel() < pProp->GetLvOpanable() )
		return XGAME::xE_NOT_ENOUGH_LEVEL;
	if( IsLockUnit( unit ) )
		return XGAME::xE_MUST_UNLOCK_UNIT;
	bool bAllLock = true;
	if( pProp->GetListParents().size() == 0 )
		bAllLock = false;
	//
	for( auto pParent : pProp->GetListParents() ) {
		if( pParent->IsRoot() )		// 부모가 루트면 잠금해제할수 있음.
			return XGAME::xE_OK;
		// 이 노드의 부모가 모두 잠금상태면해제 못함.
		const auto abil = pHero->GetAbilNode( unit, pParent->idNode );
		if( abil.IsUnlock() ) {
			bAllLock = false;
			break;
		}
	}
	// 부모노드가 모두 잠금상태면 해제 못함.
	if( bAllLock )
		return XGAME::xE_MUST_PRECEDE_UNLOCK;
	return XGAME::xE_OK;
}
#endif // defined(_CLIENT) || defined(_GAME_SERVER)

#if defined(_CLIENT) || defined(_GAME_SERVER)
XGAME::xtError XAccount::GetUnlockableAbil( XSPHero pHero
																					, XGAME::xtUnit unit
																					, ID idAbilNode )
{
	auto pProp = XPropTech::sGet()->GetpNode( unit, idAbilNode );
	return GetUnlockableAbil( pHero, unit, pProp );
}
#endif // defined(_CLIENT) || defined(_GAME_SERVER)

/**
@brief 특성업그레이드를 위해 자원이 충분한가.
*/
bool XAccount::IsEnoughResourceForResearch( XSPHero pHero)
{
	int numPoint = pHero->GetNumSetAbilPoint();
	if (IsEnoughIdxResourceForResearchWithPoint(numPoint + 1, 0) == false)
		return false;
	if (IsEnoughIdxResourceForResearchWithPoint(numPoint + 1, 1) == false)
		return false;
	return true;
}
/**
 @brief unit특성을 현재 연구하는데 idxRes번째 자원이 충분한가.
 @param idxRes 필요자원의 인덱스( ex:[wood0,iron1] )
*/
bool XAccount::IsEnoughIdxResourceForResearch( XSPHero pHero
																					, int idxRes )
{
	int numPoint = pHero->GetNumSetAbilPoint();
	return IsEnoughIdxResourceForResearchWithPoint(numPoint + 1, idxRes );
}

/**
 @brief unit특성에서 numPoint개째 특성을 연구하는데 자원이 충분한가.
 @param numPoint 현재특성개수+1 이 와야 현재 특성찍을수 있는지 알수 있다.
 @param idxRes 필요자원의 인덱스( ex:[wood0,iron1] )
*/
bool XAccount::IsEnoughIdxResourceForResearchWithPoint( int numPoint
																											, int idxRes )
{
	auto& costAbil = XPropTech::sGet()->GetResearchWithNum( numPoint );
	int size = costAbil.aryResourceNeed.size();
	// 연구 필요자원이 한가지인데 두번째 필요자원의 체크를 요구받으면 그냥 true로 리턴
	if( size == 1 && idxRes == 1 )
		return true;
	// 
	XBREAK( size == 0 || size >= 3 );
	if( size > 0 ) {
		if( XASSERT( idxRes < size ) ) {
			auto resType = costAbil.aryResourceNeed[ idxRes ];
			if( GetResource( resType ) < costAbil.aryNumNeed[ idxRes ] )
				return false;
		}
	}
	return true;
}

/**
 @brief res의 자원이 충분한가.
*/
bool XAccount::IsEnoughResource( const xRES_NUM& res )
{
	return GetResource( res.type ) >= (int)res.num;
}

bool XAccount::IsEnoughResourceWithAry( const XVector<xRES_NUM>& aryRes )
{
	for( auto& res : aryRes ) {
		if( GetResource( res.type ) < (int)res.num )
			return false;
	}
	return true;
}
/**
 @brief 지금 연구를 할수 있는 영웅이 있는지 탐색
*/
bool XAccount::IsAbleResearch()
{
	for( auto pHero : m_listHero ) {
		if( IsAbleResearchUnit( pHero ) )
			return true;
	}
	return false;
}

/**
 @brief pHero가 현재 이끌고 있는 유닛의 특성을 업글할수 있는 상태인가.
*/
bool XAccount::IsAbleResearchUnit( XSPHero pHero )
{
//	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
//		auto unit = ( XGAME::xtUnit )i;
	auto unit = pHero->GetUnit();		// 현재 장착유닛만 검사함.
		if( pHero->IsResearbleUnit(unit) )
			if( IsUnlockUnit( unit ) ) {
				if( IsEnoughResourceForResearch( pHero/*, unit*/ ) )
					return true;
		}
//	}
	return false;
}

/**
 @brief numPoint개의 특성연구비용 구조체를 얻는다.
*/
XPropTech::xtResearch& XAccount::GetCostAbilWithNum( int numPoint )
{
	return XPropTech::sGet()->GetResearchWithNum( numPoint );
}
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
/**ㄷ
@brief 남은 연구시간의 캐쉬가격
*/
int XAccount::GetCashRemainResearch()
{
	int secRemain = m_Researching.GetsecRemain();
	if (secRemain <= 0)
		return 0;
	int cost = GetCashResearch(secRemain);
	return cost;
}

int XAccount::GetCashResearch(int sec)
{
	if (sec < 0)
		sec = 0;
//	int cost = (sec / 72) - 1;	// 클라와 서버간의 시간오차가 있을수 있어서 1캐쉬 빼줌.
	int cost = ( sec / 36 ) - 1;	// 클라와 서버간의 시간오차가 있을수 있어서 1캐쉬 빼줌.
	if (cost <= 0)
		cost = 1;
	return cost;
}

/**
@brief 남은 연구시간의 금화가격
*/
int XAccount::GetGoldRemainResearch()
{
	int secRemain = m_Researching.GetsecRemain();
	if( secRemain <= 0 )
		return 0;
	int cost = GetGoldResearch( secRemain );
	return cost;
}
/**
 @brief sec시간의 금화환산액
*/
int XAccount::GetGoldResearch( int sec )
{
	if( sec < 0 )
		sec = 0;
	// propResearchZero/영웅렙업Zero/AG19참조
	const float lerp = ((float)GetLevel() / XGAME::MAX_ACC_LEVEL);	// 초반렙에서 즉시완료 비용이 너무 비싸서 단계적으로 조절해봄.
	const float goldPerSec = 305.2174f * lerp;
	int cost = (int)((sec-1) * goldPerSec);	// 서버와 클라간 오차(1초가량)가 있을수 있어서 1초 빼줌.
	if( cost <= 0 )
		cost = 1;
	return cost;
}

/**
@brief snSlot을 즉시완료할경우 필요한 캐쉬양을 얻는다.
*/
int XAccount::GetCashRemainTrain(xTrainSlot *pSlot)
{
	return GetCashResearch(pSlot->GetsecRemain());

}
int XAccount::GetGoldRemainTrain( xTrainSlot *pSlot )
{
	// 남은 훈련시간을 gold로 환산
	return GetGoldResearch( pSlot->GetsecRemain() );

}
/**
@brief 각 캐쉬기능들의 캐쉬가격을 리턴한다.
*/
int XAccount::GetCostCashSkill(XGAME::xtCashSkill typeCashSkill)
{
	switch (typeCashSkill)
	{
	case XGAME::xCS_REGEN_SPOT:
		return 10;
	case XGAME::xCS_CHANGE_PLAYER:
		return 10;
	case XGAME::xCS_SULFUR_RETREAT:
		return 10;
	}
	return 0x7fffffff;
}

/**
 @brief 보유영웅수에 따른 소환금화가격
*/
int XAccount::GetGoldNormalSummon()
{
	int num = GetNumHeroes();
	return (int)(::pow(num,5) * 128);
//	return (int)( ::pow( num, 4 ) * 500 );
}

/**
 @brief 금화양에 해당하는 캐쉬개수를 얻는다.
*/
int XAccount::GetCashFromGold( int numGold )
{
	int cash = (numGold / 6000) + 1;
	if( cash <= 0 )
		cash = 1;
	return cash;
}

int XAccount::GetCashFromRes( XGAME::xtResource typeRes, int num )
{
	const int gold = XGlobalConst::sGetGoldByTradeRes( typeRes, num );
	return GetCashFromGold( gold );
}
/**
 @brief 현재 훈련소 슬롯 확장 비용
*/
int XAccount::GetCashUnlockTrainingSlot()
{
	return 300;
}

/**
 @brief 
*/
void XAccount::Process(float dt, XDelegateBuff *pDelegate)
{
#ifdef _CLIENT
	//	ProcessClient( dt );
#endif // _CLIENT
	for (auto itor = m_listBuff.begin(); itor != m_listBuff.end();)
	{
		auto& buff = (*itor);
		buff.timer.Update(dt);
#ifdef _GAME_SERVER
		///< 버프 시간이 다되면 델리게이트 발생시키고 리스트에서 삭제.
		if (buff.timer.IsOver())
		{
			if (pDelegate)
				pDelegate->DelegateFinishBuff(GetThis(), buff);
			m_listBuff.erase(itor++);
		}
		else
#endif // _GAME_SERVER
			++itor;
	}
	//
	if (m_pWorld)
		m_pWorld->Process(dt);
	if (m_pQuestMng)
		m_pQuestMng->Process(dt);
}

#ifdef _CLIENT
/**
@brief 1초에 한번씩 도는 프로세스. 클라에서만 돌아간다.
*/
void XAccount::ProcessClientPerSec(float dt)
{
}
#endif // _CLIENT

/**
@brief 버프를 하나 추가한다.
*/
XGAME::xBuff* XAccount::AddBuff(const XGAME::xBuff& buff)
{
	auto pBuff = m_listBuff.FindpIf([&buff](XGAME::xBuff& elem)->bool {
		return elem.idBuff == buff.idBuff;
	});
	if (pBuff)
	{
#ifdef _GAME_SERVER
		// 이미 같은 버프가 있다면 시간만 더해준다.
		pBuff->timer.AddWaitSec(buff.timer.GetsecWaitTime());
#elif defined(_CLIENT)
		// 클라에선 똑같은게 있으면 덮어씌운다.
		(*pBuff) = buff;
#endif
		return pBuff;
	}
	else
	{
		m_listBuff.Add(buff);
		return m_listBuff.GetpLast();
	}
	return nullptr;
}

/**
@brief idBuff를 수동으로 삭제시킨다.
*/
void XAccount::DelBuff(ID idBuff)
{

	m_listBuff.DelIf([idBuff](XGAME::xBuff& elem)->bool {
		return elem.idBuff == idBuff;
	});
}

XGAME::xBuff* XAccount::GetBuff(ID idBuff)
{
	auto pBuff = m_listBuff.FindpIf([idBuff](XGAME::xBuff& elem)->bool {
		return elem.idBuff == idBuff;
	});
	return pBuff;
}

XGAME::xBuff* XAccount::GetBuff(LPCTSTR szIdentifier)
{
	auto pBuff = m_listBuff.FindpIf([szIdentifier](XGAME::xBuff& elem)->bool {
		return elem.sid == szIdentifier;
	});
	return pBuff;
}

/**
@brief 전투기록 하나를 추가시킨다.
@param bAttack 공격기록이면 true. 방어기록이면 false
*/
XGAME::xBattleLog* XAccount::AddBattleLog(bool bAttack, XGAME::xBattleLog& log)
{
	XList4<XGAME::xBattleLog> *pList = nullptr;
	if (bAttack)
		pList = &m_listBattleLogAttack;
	else
		pList = &m_listBattleLogDefense;
	pList->Add(log);
	// 리스트가 10개가 넘어가면 앞에서부터 삭제한다.
	if (pList->size() > 10)	{
		auto itorFirst = pList->begin();
		pList->erase(itorFirst);
	}

	return nullptr;
}

int XAccount::SerializeAttackLog(XArchive& ar)
{
	ar << VER_BATTLE_LOG;
	int size = m_listBattleLogAttack.size();
	ar << m_listBattleLogAttack;
	MAKE_CHECKSUM(ar);
	return 1;
}
int XAccount::DeSerializeAttackLog(XArchive& ar)
{
	int ver;
	ar >> ver;
	ar.SetverArchiveInstant(ver);
	ar >> m_listBattleLogAttack;	// 내부에서 Deserialize가 호출되면 인스턴트버전이 같이 넘어간다.
	RESTORE_VERIFY_CHECKSUM(ar);
	return 1;
}

int XAccount::SerializeDefenseLog(XArchive& ar)
{
	ar << VER_BATTLE_LOG;
	int size = m_listBattleLogDefense.size();
	if (m_numAttaccked > 255)
		m_numAttaccked = 1;			// 휴면계정의 경우 이럴수 있음.
	ar << (BYTE)m_numAttaccked;		// 오프라인동안에 공격을 받았다면 0이 아닌값이 된다.
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << m_listBattleLogDefense;
	MAKE_CHECKSUM(ar);
	return 1;
}
int XAccount::DeSerializeDefenseLog(XArchive& ar)
{
	int ver;
	BYTE b0;
	ar >> ver;
	ar >> b0;	m_numAttaccked = b0;
	ar >> b0 >> b0 >> b0;
	ar.SetverArchiveInstant(ver);
	ar >> m_listBattleLogDefense;	// 내부에서 Deserialize가 호출되면 인스턴트버전이 같이 넘어간다.
	int cnt = 0;
	int numOver = m_listBattleLogDefense.size() - 10;
	for( auto itor = m_listBattleLogDefense.begin(); itor != m_listBattleLogDefense.end(); ) {
		if( numOver-- <= 0 )
			break;
		m_listBattleLogDefense.erase( itor++ );
	}
	RESTORE_VERIFY_CHECKSUM(ar);
	return 1;
}

int XAccount::AddGold( int add ) 
{
	if( add < 0 ) {
		if( m_Gold < (DWORD)( -add ) ) {
// 			XBREAK(1);		// 영웅영입하다가 이런상황 생기는듯하다.
			m_Gold = 0;
		} else
			m_Gold += add;
	} else {
		if( (DWORD)0x7fffffff - m_Gold < (DWORD)add )
			m_Gold = (DWORD)0x7fffffff;
		else
			m_Gold += add;
	}
	return m_Gold;
}

int XAccount::AddCashtem( int add ) 
{
	if( add < 0 ) {
		if( m_Cashtem < (DWORD)( -add ) )
			m_Cashtem = 0;
		else
			m_Cashtem += add;
	} else {
		if( (DWORD)0xffffffff - m_Cashtem < (DWORD)add )
			m_Cashtem = (DWORD)0xffffffff;
		else
			m_Cashtem += add;
	}
	return m_Cashtem;
}

int XAccount::AddGuildPoint( int add )
{
	if( add < 0 ) {
		if( m_ptGuild + add < 0 )
			m_ptGuild = 0;
		else
			m_ptGuild += add;
	} else {
		if( 0x7fffffff - m_ptGuild < add )
			m_ptGuild = 0x7fffffff;
		else
			m_ptGuild += add;
	}
	return m_ptGuild;
}

int XAccount::xTrainSlot::Serialize( XArchive& ar ) {
	ar << (BYTE)VER_ETC_SERIALIZE;
	ar << (BYTE)type;
	ar << (WORD)0;
	ar << snSlot;
	timerStart.Serialize( ar );
	ar << snHero << secTotal;
	ar << _param[ 0 ];
	ar << _param[ 1 ];
	return 1;
}
int XAccount::xTrainSlot::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	int ver;
	ar >> b0;	ver = b0;
	ar >> b0;	type = ( XGAME::xtTrain )b0;
	ar >> b0 >> b0;
	ar >> snSlot;
	timerStart.DeSerialize( ar, ver );
	ar >> snHero >> secTotal;
	ar >> _param[ 0 ];
	ar >> _param[ 1 ];
	return 1;
}

/**
 @brief pCloud지역을 지금 오픈할수 있는가.
*/
XGAME::xtError XAccount::IsOpenableCloud( XPropCloud::xCloud *pProp )
{
	return IsOpenableCloudWithHaveGold( pProp, (int)GetGold() );
}

/**
 @brief this가 가진 골드양을 외부에서 지정해주는 버전.
*/
XGAME::xtError XAccount::IsOpenableCloudWithHaveGold( XPropCloud::xCloud *pProp, int goldHave )
{
	xtError errCode = xE_OK;
	if( IsLockArea( pProp, &errCode ) ) {
		XBREAK( errCode == xE_OK );
		return errCode;
	}
	// 돈이 없으면 못염.
	if( goldHave < pProp->cost )
		return XGAME::xE_NOT_ENOUGH_GOLD;
	return xE_OK;
}

/**
 @brief 지역이 어떤조건(선행지역/레벨)에 의해 잠겨있는가.
*/
bool XAccount::IsLockArea( XPropCloud::xCloud *pProp, xtError *pOut )
{
	xtError errCode = XGAME::xE_OK;
	do {
#ifdef _CHEAT
			if( m_bDebugMode ) {
				errCode = XGAME::xE_OK;
				break;
			}
#endif // _CHEAT
			if( pProp == nullptr ) {
				errCode = XGAME::xE_ERROR_CRITICAL;
				break;
			}
			// 선행지역이 지정되어있을경우 안열려있으면 못염.
			// 선행지역이 안열렸으면 못염.
			if( !pProp->idsPrecedeArea.empty() ) {
				auto pPropPrecede = PROP_CLOUD->GetpProp( pProp->idsPrecedeArea );
				if( pPropPrecede ) {
					// 선행지역이 오픈안되었으면..
					if( !m_pWorld->IsOpenedArea( pPropPrecede->idCloud ) ) {
						errCode = XGAME::xE_MUST_PRECEDE_AREA_OPEN;
						break;
					}
				}
			}
			// 레벨이 안되면 못염
			if( !pProp->IsOpenable( GetLevel() ) ) {
				errCode = XGAME::xE_NOT_ENOUGH_LEVEL;
				break;
			}
			// 선행퀘스트가 있을때 
			if( !pProp->idsQuest.empty() ) {
				// 선행퀘를 못깼으면 못염
				if( !IsCompletedQuest( pProp->idsQuest ) ) {
					errCode = XGAME::xE_MUST_PRECEDE_QUEST;
					break;
				}
			}
			// 키 아이템이 없으면 못염.(가장마지막에 검사할것)
			if( !pProp->idsItem.empty() ) {
				auto pKeyItem = GetItem( pProp->idsItem );
				if( pKeyItem == nullptr ) {
					errCode = XGAME::xE_MUST_NEED_KEY_ITEM;
					break;
				}
			}
			// 키 아이템 검사를 가장 마지막에 해야함. 경우에 따라 키아이템 없는 에러는 무시할때도 있음.
	} while (0);
	if( pOut )
		*pOut = errCode;
	return errCode != xE_OK;
}
bool XAccount::IsLockArea( ID idArea, xtError *pOut )
{
	auto pProp = PROP_CLOUD->GetpProp( idArea );
	if( XASSERT(pProp) ) {
		return IsLockArea( pProp, pOut );
	}
	return true;
}
/**
 @brief 영웅리스트에있는 영웅 pHero를 시리얼라이즈 한다.
*/
int XAccount::SerializeHeroUpdate( XArchive& ar, XSPHero pHero )
{
	ar << pHero->GetsnHero();
	return XHero::sSerialize( ar, pHero );
}
/**
 @brief ar에 담겨있는 영웅을 영웅인벤에 업데이트ㅏ한다.
 만약 없는영웅이면 새로 만들고 있으면 값만 업데이트 한다.
*/
int XAccount::DeserializeHeroUpdate( XArchive& ar )
{
	ID snHero;
	int verHero;
	ar >> snHero;
	auto pHero = GetHero( snHero );
	if( pHero ) {
		ar >> verHero;
		pHero->DeSerialize( ar, /*GetThis(), */verHero );
	} else {
		// 인벤에 없으면 새로 생성하고 추가한다.
		auto pHero = XHero::sCreateDeSerialize2( ar, GetThis() );
		AddHero( pHero );
	}
	return 1;
}
/**
 @brief 시퀀스idsSeq가 이미 실행된건지 확인한다.
*/
bool XAccount::IsCompletedSeq( const std::string& idsSeq ) 
{
	auto itor = m_mapCompletedSeq.find( idsSeq );
	if( itor == m_mapCompletedSeq.end() ) {
		return false;
	}
	return true;
}
/**
 @brief 시퀀스 완료목록에서 idsSeq를 지운다.
*/
void XAccount::_ClearCompletedSeq( const std::string& idsSeq )
{
	auto itor = m_mapCompletedSeq.find( idsSeq );
	if( itor == m_mapCompletedSeq.end() ) {
		return;
	}
	m_mapCompletedSeq.erase( itor );
}
/**
 @brief 모든 시퀀스완료목록을 삭제한다.
*/
void XAccount::_ClearAllCompletedSeq()
{
	m_mapCompletedSeq.clear();
}

int XAccount::SerializeSeq( XArchive& ar )
{
	ar << (int)m_mapCompletedSeq.size();
	for( auto itor : m_mapCompletedSeq ) {
		std::string str = itor.first;
		ar << str;
	}
	ar << m_idsLastSeq;
	MAKE_CHECKSUM( ar );
	return 1;
}

int XAccount::DeserializeSeq( XArchive& ar, int verEtc )
{
	m_mapCompletedSeq.clear();
	int size;
	ar >> size;
	_tstring tidsSeq;
	std::string idsSeq;
	for( int i = 0; i < size; ++i ) {
		ar >> tidsSeq;
		idsSeq = SZ2C(tidsSeq);
		AddCompleteSeq( idsSeq );
	}
	_tstring tstr;
	ar >> tstr;
	m_idsLastSeq = SZ2C(tstr);
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

// xuzhu end
#ifdef _GAME_SERVER
/**
@brief 현재 연구를 완료시킨다.
*/
int XAccount::DoCompleteCurrResearch()
{
	auto pHero = GetHero( m_Researching.GetsnHero() );
	if( XBREAK(pHero == nullptr) )
		return 0;		// 연구중에 영웅 날리면 이렇게 될 수 있음. 연구중엔 해고못시키게 해야겠군.
	ID idAbil = m_Researching.GetidAbil();
	if (idAbil == 0)
		return 0;
	auto pProp = XPropTech::sGet()->GetpNode(idAbil);
	if (XBREAK(pProp == nullptr))
		return 0;
	const auto abil = pHero->GetAbilNode(pProp->unit, idAbil);
	if (XBREAK(abil.point >= pProp->maxPoint))
		return 0;
	// 특성포인트를 올림
	int point = pHero->AddAbilPoint( pProp->unit, idAbil );
	//SetCompleteResearch();
	// 연구 완료
	m_Researching.DoComplete();
	return point;
}
#endif // _GAME_SERVER

bool XAccount::IsEquip(ID snItem)
{
	if (snItem == 0)
		return false;
	for (auto pHero : m_listHero) {
		if (pHero->IsEquip(snItem))
			return true;
	}
	return false;
}

/**
@brief snItem을 장착중인 영웅을 얻는다.,
*/
XSPHero XAccount::GetHeroByEquip(ID snItem)
{
	if (snItem == 0)
		return nullptr;
	for (auto pHero : m_listHero)	{
		if (pHero->IsEquip(snItem))
			return pHero;
	}
	return nullptr;
}

/**
@brief 훈련영웅 하나를 훈련소에 넣는다.
@return 성공하면 슬롯 아이디를 돌려준다.
*/
ID XAccount::AddTrainSlot(const xTrainSlot& slot)
{
	if (XBREAK(m_listTrainSlot.size() >= XGAME::MAX_TRAIN_SLOT))
		return 0;
	m_listTrainSlot.Add(slot);
	return slot.snSlot;
}

/**
@brief snHero가 현재 type훈련중인지 알아본다.
*/
bool XAccount::IsTrainingHero(ID snHero, XGAME::xtTrain type)
{
	for (auto& slot : m_listTrainSlot)	{
		if (slot.snHero == snHero &&
			slot.type == type)
			return true;
	}
	return false;
}

/**
@brief 이 영웅이 뭔가 훈련중인가.
*/
bool XAccount::IsTrainingHeroByAny(ID snHero)
{
	for (auto& slot : m_listTrainSlot)	{
		if (slot.snHero == snHero)
			return true;
	}
	return false;
}

/**
@brief 아무영웅이든지 훈련중인가.
*/
bool XAccount::IsTrainingAnyHero()
{
	return m_listTrainSlot.size() > 0;
}

/**
@brief snHero의 훈련정보를 꺼낸다.
*/
XAccount::xTrainSlot* XAccount::GetTrainingHero(ID snHero, XGAME::xtTrain type)
{
	for (auto& slot : m_listTrainSlot)	{
		if (slot.snHero == snHero &&
			slot.type == type)
			return &slot;
	}
	return nullptr;
}

/**
@brief snSlot의 훈련이 끝났는지 확인
*/
bool XAccount::IsTrainingComplete(ID snSlot)
{
	auto pSlot = m_listTrainSlot.FindByIDNonPtr(snSlot);
	if (XASSERT(pSlot))
		return pSlot->IsComplete();
	return false;
}

/**
@brief 훈련소슬롯을 돌면서 훈련이 완료되었으면 훈련완료 처리를 하고 핸들러를 발생시킨다.
*/
void XAccount::IfCompleteThenDoCompleteAndInvokeHandler(void(*funcHandler)())
{
	XBREAK(1);
	for (auto itor = m_listTrainSlot.begin(); itor != m_listTrainSlot.end();)	{
		// 만들려다 말음.
		++itor;
	}

}

/**
@brief snSlot의 훈련을 완료시킨다.
@param snHero 확인용
@param expAdd 훈련이 완료되고 받는 경험치
영웅한명이 여러슬롯을 쓸수도 있음.
*/
int XAccount::DoCompleteTraining(ID snSlot, XSPHero pHero, int expAdd)
{
	auto pSlot = GetpTrainingSlot(snSlot);
	if (XBREAK(pSlot == nullptr))
		return 0;
	if (XBREAK(pSlot->snHero != pHero->GetsnHero()))
		// invalid call
		return 0;
	auto type = pSlot->type;
	if( XASSERT( expAdd ) ) {
// 		pHero->SetbAutoLevelup( type, false );
		bool bLvUp = pHero->AddExp( type, expAdd );
		if( bLvUp )
			OnHeroLevelup( type, pHero );
	}
	// 훈련슬롯 삭제.
	m_listTrainSlot.DelByIDNonPtr(snSlot);
	return 1;
}

/**
@brief snHero의 훈련슬롯을 모두 삭제 시킨다.
*/
void XAccount::DelTrainSlotByHero(ID snHero)
{
	for (auto itor = m_listTrainSlot.begin(); itor != m_listTrainSlot.end();) {
		auto& prop = (*itor);
		if (prop.snHero == snHero) {
			m_listTrainSlot.erase(itor++);
		}
		else
			++itor;
	}
}

/**
@brief 훈련완료된 슬롯의 아이디를 얻는다.
*/
int XAccount::GetTrainCompleteSlots(XArrayLinearN<ID, 64> *pOutAry)
{
	for (auto& propSlot : m_listTrainSlot) {
		if (propSlot.IsComplete())
			pOutAry->Add(propSlot.snSlot);
	}
	return pOutAry->size();
}
/**
 @brief 영웅의 레벨이 올랐을때 핸들러.
*/
void XAccount::OnHeroLevelup( XGAME::xtTrain type, XSPHero pHero )
{
	xQuest::XEventInfo infoQuest;		// 이벤트 정보.
	if( type == XGAME::xTR_LEVEL_UP ) {
		infoQuest.SetLevelHero( pHero->GetLevelHero() );
		m_pQuestMng->DispatchEvent( XGAME::xQC_EVENT_HERO_LEVEL, infoQuest );
	}
	if( type == XGAME::xTR_SQUAD_UP ) {
		infoQuest.SetLevelSquad( pHero->GetlevelSquad() );
		m_pQuestMng->DispatchEvent( XGAME::xQC_EVENT_HERO_LEVEL_SQUAD, infoQuest );
	}
	if( type == XGAME::xTR_SKILL_ACTIVE_UP ) {
		infoQuest.SetLevelSkill( pHero->GetlvActive() );
		m_pQuestMng->DispatchEvent( XGAME::xQC_EVENT_HERO_LEVEL_SKILL, infoQuest );
	}
	if( type == XGAME::xTR_SKILL_PASSIVE_UP ) {
		infoQuest.SetLevelSkill( pHero->GetlvPassive() );
		m_pQuestMng->DispatchEvent( XGAME::xQC_EVENT_HERO_LEVEL_SKILL, infoQuest );
	}
}
/**
 @brief 현재보유한 모든 퀘스트들의 완료조건을 현재 계정상태로 다시 평가한다.
*/
void XAccount::DispatchQuestEventCurrState( XGAME::xtQuestCond typeCond )
{
	// typeCond이벤트는 when으로 사용할수 없으므로 이 이벤트로 인하여 퀘가 발생하는일은 없게할것.
	XVector<XQuestObj*> aryQuestObj;
	m_pQuestMng->GetQuestsToAry( &aryQuestObj );
	// 보유한 모든 퀘스트의 루프
	for( auto pQuestObj : aryQuestObj ) {
		if( pQuestObj->IsQuestType( typeCond ) ) {
			// 해당 퀘스트 타입의 퀘일때만 평가.
			DispatchQuestEventCurrState( pQuestObj, typeCond );
		}
	}
}

void XAccount::DispatchQuestEventCurrState( XQuestObj *pQuestObj
																					, XGAME::xtQuestCond typeCond )
{
	bool bUpdate = false;
	XARRAYLINEAR_LOOP_AUTO( pQuestObj->GetaryConds(), pCondObj ) {
// 	for( XQuestCon* pCondObj : pQuestObj->GetaryConds() ) {
		if( pCondObj->GetQuestType() == typeCond )  {
			auto _bUpdate = DispatchQuestEventCurrState( pQuestObj, pCondObj, typeCond );
			if( _bUpdate )
				bUpdate = true;
		}
	} END_LOOP;
	if( bUpdate )
		m_pQuestMng->UpdatedQuestObj( pQuestObj );	// 이것때매 UpdatedQuestObj을 public으로 바꿀수밖에 없었음.
}


/**
 @brief 특정퀘의 특정 조건에 대해서만 재평가를 하는 버전.
*/
bool XAccount::DispatchQuestEventCurrState( XQuestObj *pQuestObj
																					, XQuestCon *pQuestCon
																					, XGAME::xtQuestCond typeCond )
{
	bool bRetUpdate = false;
	switch( typeCond ) {
	case XGAME::xQC_EVENT_HAVE_ITEM: {
		// 모든 아이템에 대해서 다시 평가한다.
		for( auto pItem : m_listItem ) {
			// 예를들어 어떤아이템을 입수하면 그것으로 인해 다른퀘가 발생한다고 하자. 그런경우 만약 그 아이템을
			// 플레이중에 얻은게 아니라 오프라인중에 영자가 넣어줬다거나 해서 갑자기 생긴 아이템이라고 치면
			// 접속시에 그 아이템이 있는지 체크해서 퀘를 발생시켜야 한다. 하지만 그러자면 매번 모든 아이템을 모든 퀘스트에
			// 대해서 평가를 해야하기때문에 부하가 상당히 걸릴듯 하다. 그래서 부하를 줄이기위해 아이템입수로 퀘가 생기는류를 없애고
			// 대신 그 아이템을 사용하는 순간 퀘가 발생하는걸로 바꿔 부하를 줄인다.
			// 스크립터의 실수를 줄이기위해 지금 when과 what을 xQC_EVENT로 함께쓰고 있는데 따로 분리할필요가 있다.
			// GET_ITEM의 경우 what에는 있지만 when에는 없도록. 이렇게 하면 예상치 못한 조합을 사용한 퀘를 만들어 예측하지 못하는 버그를 줄일 수 있다.
//			m_pQuestMng->OccurQuestByDispatchEvent( typeCond, )
			bool bUpdate = m_pQuestMng->DispatchEventFast( pQuestObj, pQuestCon, typeCond, pItem->GetidProp() );
			if( bUpdate )
				bRetUpdate = true;
		}
	} break;
	case XGAME::xQC_EVENT_OPEN_AREA: {
		// 오픈한 모든 지역을 한번씩 다 이벤트로 날린다.
		for( auto idOpened : m_pWorld->GetlistOpendArea() ) {
			bool bUpdate = m_pQuestMng->DispatchEventFast( pQuestObj, pQuestCon, typeCond, idOpened );
			if( bUpdate )
				bRetUpdate = true;
		}
	} break;
	case XGAME::xQC_EVENT_HAVE_RESOURCE: {
		xQuest::XEventInfo info;
		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
			const auto typeRes = (XGAME::xtResource)i;
			info.SettypeResource( typeRes );
			info.SetAmount( GetResource( typeRes ) );
			bool bUpdate = m_pQuestMng->DispatchEventFast( typeCond, info, pQuestObj, pQuestCon );
			if( bUpdate )
				bRetUpdate = true;
		}
// 		info.SettypeResource( XGAME::xRES_GOLD );
// 		info.SetAmount( (int)GetGold() );
// 		bool bUpdate = m_pQuestMng->DispatchEventFast( pQuestObj, pQuestCon, typeCond, info );
// 		if( bUpdate )
// 			bRetUpdate = true;
	} break;
	case XGAME::xQC_EVENT_BUY_CASH: {
		bool bUpdate = m_pQuestMng->DispatchEventFast( pQuestObj, pQuestCon, typeCond, m_numBuyCash );
		if( bUpdate )
			bRetUpdate = true;
	} break;
	default:
		switch( typeCond ) {
			// 다음 이벤트의 경우는 모든 영웅들에 대해서 퀘완료 평가를 다시한다.
		case XGAME::xQC_EVENT_HERO_LEVEL:
		case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
		case XGAME::xQC_EVENT_HERO_LEVEL_SKILL: {
			for( auto pHero : m_listHero ) {
				// 모든 영웅에 대해서 레벨 평가를 다시 한다.
				switch( typeCond ) {
				case XGAME::xQC_EVENT_HERO_LEVEL:
					m_pQuestMng->DispatchEvent( typeCond, pHero->GetLevelHero() );
					break;
				case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
					m_pQuestMng->DispatchEvent( typeCond, pHero->GetlevelSquad() );
					break;
				case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:
					m_pQuestMng->DispatchEvent( typeCond, pHero->GetlvActive() );
					m_pQuestMng->DispatchEvent( typeCond, pHero->GetlvPassive() );
					break;
				}
			}
		} break;
		default:
			XBREAK(1); // 이런경우 안생기게 할것.
		} // switch
		break;
	} // switch

	return bRetUpdate;
}
int XAccount::GetCostRemainFullAP()
{
	const int apRemain = m_maxAP - m_AP;
	return GetCashFromAP( apRemain );
}

int XAccount::GetCashFromAP( int ap )
{
	const int cash = (int)( XGC->m_cashPerAP * ap );
	return ( cash < 1 ) ? 1 : cash;
}

/**
 @brief 유닛을 잠금 해제 시킨다.
 호출되기전에 해당 유닛의 언락포인트가 있어야 한다.
*/
void XAccount::SetUnlockUnit( XGAME::xtUnit unit ) 
{
	if( XBREAK( XGAME::IsInvalidUnit( unit ) ) )
		return;
	if( unit == XGAME::xUNIT_PALADIN ) {
		// 기사의 경우 전용 포인트가 있어야 열수 있음
		if( XASSERT( m_bUnlockTicketForPaladin ) ) {
			m_bUnlockTicketForPaladin = false;
			m_aryUnlockedUnit[ unit ] = 1;
//			m_bitFlagTutorial.SetUnlockUnit( unit, true );
		}
	} else
	if( !XGAME::IsSmall( unit ) ) {
		// 중대형유닛의 경우 중대형언락 포인트가 있어야 열수 있음.
		if( XASSERT(m_numUnlockTicketForMiddleOrBig > 0) ) {
			--m_numUnlockTicketForMiddleOrBig;
			m_aryUnlockedUnit[ unit ] = 1;
//			m_bitFlagTutorial.SetUnlockUnit( unit, true );
		}
	}
}
/**
 @brief unlock된 유닛들을 어레이에 담는다.
*/
void XAccount::GetUnlockUnitsToAry( 
											XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> *pOutAry )
{
	XBREAK( pOutAry == nullptr );
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		if( m_aryUnlockedUnit[i] != 0 )
			pOutAry->Add( (XGAME::xtUnit)i );
	}
}

/**
 @brief 잠겨있는 유닛 리스트를 얻음.
*/
void XAccount::GetLockUnitsToAry( XVector<XGAME::xtUnit> *pOutAry )
{
	XBREAK( pOutAry == nullptr );
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		if( m_aryUnlockedUnit[ i ] == 0 )
			pOutAry->Add( ( XGAME::xtUnit )i );
	}
}

/**
 @brief 현재 unit을 언락시킬수 있는상태인가.
*/
bool XAccount::IsUnlockableUnit( XGAME::xtUnit unit ) 
{
	if( XBREAK( XGAME::IsInvalidUnit( unit ) ) )
		return false;
	if( unit == XGAME::xUNIT_PALADIN ) {
		// 기사의 경우 전용 포인트가 있어야 열수 있음
		return m_bUnlockTicketForPaladin;
	}
	else
		if( !XGAME::IsSmall( unit ) ) {
			// 중대형유닛의 경우 중대형언락 포인트가 있어야 열수 있음.
			return m_numUnlockTicketForMiddleOrBig > 0;
		}
	return false;
}
/**
 @brief unit을 언제 오픈할수 있는가 
 @return 0을 리턴하면 지금 당장 오픈가능. 0이외의 숫자는 해당레벨에서 오픈가능
*/
int XAccount::GetLevelUnlockableUnit( XGAME::xtUnit unit )
{
	// 이제 지역오픈 기반으로 유닛 언락이 바꼈기때문에 이 함수는 의미가 없어졌다.
	// 그러나 연구소 화면에서 다음 언락 레벨을 표시해 줘야할 필요가 아직 있어서 남겨둠.
	bool bUnlockable = false;
	int lvUnlock = 0;
	if( unit == XGAME::xUNIT_PALADIN ) {
		lvUnlock = XGAME::LEVEL_UNLOCK_PALADIN;
		if( GetbUnlockTicketForPaladin() )
			bUnlockable = true;
	} else
	// 중대형유닛의 경우 언락 포인트가 있으면 잠금해제할수 있는 상태로 표시.
	if( !XGAME::IsSmall( unit ) ) {
		// 다음 언락레벨을 찾음.
		XARRAYLINEARN_LOOP_AUTO( XGC->m_aryLevelsUnlockUnitForNotSmall, lv ) {
			if( GetLevel() < lv ) {
				lvUnlock = lv;
				break;
			}
		} END_LOOP;
		// 언락포인트가 있어야 언락할수 있다.
		if( GetnumUnlockTicketForMiddleOrBig() > 0 )
			bUnlockable = true;
	}
	if( bUnlockable )
		return 0;
	return lvUnlock;
}

/**
 @brief 아무유닛이나 현재 언락포인트가 있는가.
*/
bool XAccount::IsUnlockableAnyUnit() 
{
	if( m_bUnlockTicketForPaladin )
		return true;
	return m_numUnlockTicketForMiddleOrBig > 0;
}
/**
 @brief 언록은 했으나 아직 플레이어가 확인안한 유닛이 있는가.
*/
bool XAccount::IsNoCheckUnlockUnit()
{
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		auto unit = (XGAME::xtUnit)i;
		if( IsNoCheckUnlockUnitEach( unit ) ) {
			// unit과 같은 atktype의 영웅을 보유하고 있는지확인.
			if( IsHaveHeroWithAtkType( XGAME::GetAtkType(unit) ) )
				return true;
		}
	}
	return false;
}
bool XAccount::IsNoCheckUnlockUnitEach( XGAME::xtUnit unit )
{
	return m_aryUnlockedUnit[ unit ] == 1 ;
}
/**
 @brief typeAtk와 같은 타입의 영웅을 보유하고 있는가.
*/
bool XAccount::IsHaveHeroWithAtkType( XGAME::xtAttack typeAtk )
{
	for( auto pHero : m_listHero ) {
		if( pHero->GetType() == typeAtk )
			return true;
	}
	return false;
}
/**
 @brief 아직 언록확인을 안한 유닛이 있으며 pHero의 병과가 그 유닛의 병과라면 true
*/
bool XAccount::IsNoCheckUnlockUnitWithHero( XSPHero pHero )
{
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		auto unit = ( XGAME::xtUnit )i;
		if( IsNoCheckUnlockUnitEach( unit ) ) {
			if( pHero->GetTypeAtk() == XGAME::GetAtkType( unit ) )
				return true;
		}
	}
	return false;
}
/**
 @brief 잠금해제된 unit을 클라에서 플레이어가 확인함.
*/
void XAccount::SetCheckUnlockUnit( XGAME::xtUnit unit )
{
	XBREAK( m_aryUnlockedUnit[unit] == 0 );
	m_aryUnlockedUnit[ unit ] = 2;
}
/**
@brief 계정레벨 lvAcc레벨에 가질수 있는 총 유닛 종류
*/
int XAccount::sGetNumUnitByLevel(int lvAcc)
{
// 	int numUnits[9] = { 2, 2, 3, 4, 5, 6, 7, 8, 9 };
	int numUnits[ 6 ] = { 4, 5, 6, 7, 8, 9};
	if( lvAcc < XGAME::LEVEL_UNLOCK_PALADIN )
		return 2;
	if (lvAcc < XGAME::LEVEL_UNLOCK_UNIT)
		return 3;
	for( int i = 5; i >= 0; --i ) {
		int lvOpen = XGC->m_aryLevelsUnlockUnitForNotSmall[i];
		if( lvAcc >= lvOpen  )
			return numUnits[ i ];;
	}
	XBREAK(1);
	return 4;
}

/**
@brief 유닛 개수에 따른 최소 가능 레벨을 구한다.
numUnits개수만큼의 유닛종류를 보유하려면 최소한 xx레벨이어야 한다.
*/
int XAccount::sGetLevelByUnlockUnit(int numUnits)
{
// 	if (XBREAK(numUnits <= 0 || numUnits > XARRAY_LENGTH(levelUnlock)))
// 		return 0;
// 	int idx = numUnits - 1;
// 	return levelUnlock[idx];
	if( numUnits <= 2 )
		return 1;		// lv1
	if( numUnits == 3 )
		return XGAME::LEVEL_UNLOCK_PALADIN;
	int idx = numUnits - 4;
	return XGC->m_aryLevelsUnlockUnitForNotSmall[ idx ];
}

/**
@brief 아무 영웅이나 현재 아이템 장비가능한 영웅이 있는가.
장비가 있어도 더 좋은 장비가 있다면 true가 된다.
*/
bool XAccount::IsAbleEquipAnyHero()
{
	int aryNumItems[XGAME::xPARTS_MAX] = { 0, };
	// 각 파츠별 소지한 개수를 얻는다.
	for( auto pItem : m_listItem ) {
		if( pItem ) {
			auto parts = pItem->GetpProp()->parts;
			if( XASSERT( IsValidParts( parts ) ) ) {
				++aryNumItems[parts];
			}
		}
	}
	// 각 영웅별로 장비슬롯을 검사한다;.
	for( auto pHero : m_listHero ) {
		if( pHero ) {
			for( int i = 1; i < XGAME::xPARTS_MAX; ++i ) {
				auto parts = (XGAME::xtParts)i;
				if( XASSERT( IsValidParts( parts ) ) ) {
					auto snItem = pHero->GetsnEquipItem( parts );
					// 슬롯이 비었고 거기에 넣을수 있는 템을 가지고 있을때는 무조건 true
					if( snItem == 0 ) {
						if( aryNumItems[parts] > 0 )
							return true;
					} else {
						auto pItem = GetpcItemBySN( snItem );
						// 슬롯이 비어있지는 않지만 없지만 장착한 장비보다 더 좋은게 있다면 true
						if( IsHaveBetterEquipItem( pItem ) )
							return true;
					}
				}
			}
		}
	}
	return false;
}

/**
@brief pItem보다 더좋은 장착아이템이 있나.
*/
bool XAccount::IsHaveBetterEquipItem( const XBaseItem* pItemEquip)
{
	if( XBREAK( pItemEquip == nullptr ) )
		return false;
	if( XBREAK( !pItemEquip->IsEquipable() ) )	// invalid call
		return false;
	if( XBREAK( pItemEquip->GetpProp()->parts == XGAME::xPARTS_NONE ) ) // missing parts
		return false;
	for( auto pItem : m_listItem ) {
		// 장착아이템이고 비교템과 파츠가 같으면
		if( pItem
				&& !IsEquip( pItem->GetsnItem() )	// 아직 장착하지 않았고
				&& pItem->IsEquipable()
				&& pItem->GetpProp()->parts == pItemEquip->GetpProp()->parts ) {
			if( pItem->IsBetterThan( pItemEquip ) )
				return true;
		}
	}
	return false;
}

/**
@brief pHero영웅이 어떤 슬롯이든 더 좋은템을 끼울수 있다면 true
*/
bool XAccount::IsHaveBetterThanParts(XSPHero pHero)
{
	for( int i = 1; i < XGAME::xPARTS_MAX; ++i ) {
		auto parts = (XGAME::xtParts)i;
		if( XASSERT( IsValidParts( parts ) ) ) {
			auto snItem = pHero->GetsnEquipItem( parts );
			// 슬롯이 비었고 거기에 넣을수 있는 템을 가지고 있을때는 무조건 true
			if( snItem == 0 ) {
				// 슬롯이 비었을땐 해당파츠 아이템 아무거나 있어도 더 좋은걸로 한다
				const bool bExcludeEquiped = true;	// 장착중인건 제외시킨다.
				auto pItemHave = GetItemByEquip( parts, bExcludeEquiped );
				if( pItemHave )
					return true;
			} else {
				auto pItem = GetpcItemBySN( snItem );
				// 슬롯이 비어있지는 않지만 없지만 장착한 장비보다 더 좋은게 있다면 true
				if( IsHaveBetterEquipItem( pItem ) )
					return true;
			}
		}
	}
	return false;
}

/**
@brief 참전중인 영웅중에서 현재보다 더 좋은 장착템을 끼울수 있는가.
*/
bool XAccount::IsHaveBetterThanPartsEnteredHero()
{
	auto spLegion = GetCurrLegion();
	for( auto pHero : m_listHero ) {
		bool bEnter = (spLegion->GetpHeroBySN( pHero->GetsnHero() ) != nullptr);
		if( bEnter && IsHaveBetterThanParts( pHero ) )
			return true;
	}
	return false;
}

/**
 @brief 전투전 전투세션을 만든다.
*/
ID XAccount::SetBattleSession( ID snSession, const XSPLegion& spLegion, ID idAccEnemy, ID idSpot, DWORD param )
{
#ifdef _CLIENT
	XBREAK( snSession == 0 );
#endif // _CLIENT
	if( snSession == 0 )
		m_BattleSession.snSession = XE::GenerateID();
	else
		m_BattleSession.snSession = snSession;
	m_BattleSession.idAccEnemy = idAccEnemy;
	m_BattleSession.idSpot = idSpot;
	m_BattleSession.param = param;
	m_BattleSession.spEnemy = spLegion;
	m_BattleSession.timerStart.DoStart();
	return m_BattleSession.snSession;
}

int XAccount::xBattleSession::Serialize( XArchive& ar ) {
	int ver = 1;
	ar << (BYTE)ver;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << snSession << idAccEnemy << idSpot << param;
	timerStart.Serialize( ar );
	if( spEnemy != nullptr ) {
		ar << 11;
		spEnemy->SerializeFull( ar );
	}
	else {
		ar << 0;
	}
	return 1;
}
int XAccount::xBattleSession::DeSerialize( XArchive& ar, int verEtc ) {
	BYTE b0;
	ar >> b0;   int ver = b0;
	ar >> b0 >> b0 >> b0;
	ar >> snSession >> idAccEnemy >> idSpot >> param;
	timerStart.DeSerialize( ar, verEtc );
	DWORD bHave;
	ar >> bHave;
	if( bHave ) {
		XBREAK( bHave != 11 );
		auto pLegion = XLegion::sCreateDeserializeFull( ar );
		spEnemy = XSPLegion( pLegion );
	}
	else
		spEnemy.reset();
	return 1;
}

void XAccount::GetsnlistSoulStone(XList4<ID> &listSoulStone)
{
	for( auto pItem : m_listItem ) {
		if( !pItem->GetpProp()->strIdHero.empty() )
			listSoulStone.Add( pItem->GetsnItem() );
	}
}

/**
 @brief 영혼석리스트를 얻는다. 이미 보유한 영웅의 영혼석은 제외한다.
*/
int XAccount::GetListSoulStoneExcludeHaveHero( XList4<XBaseItem*> *pOutList )
{
	for( auto pItem : m_listItem ) {
		auto pProp = pItem->GetpProp();
		if( pProp->type == XGAME::xIT_SOUL ) {
			XBREAK( pProp->strIdHero.empty() );
			// 보유하지 않은 영웅의 영혼석만 리스트에 담음.
			if( !IsHaveHero( pProp->strIdHero.c_str() ) )
				pOutList->Add( pItem );
		}
	}
	return pOutList->size();
}

int XAccount::GetNumSoulStone(const _tstring& strId)
{
	for( auto pItem : m_listItem ) {
		if (pItem->GetpProp()->strIdHero == strId)
			return pItem->GetNum();
	}
	return 0;
}

int XAccount::GetNumSoulStoneWithidPropHero( ID idPropHero )
{
	for( auto pItem : m_listItem ) {
		if( pItem->GetidProp() == XPropItem::sGetidHeroToidItemSoul(idPropHero) )
			return pItem->GetNum();
	}
	return 0;
}

int XAccount::GetNumSoulStone( XSPHero pHero )
{
	for( auto pItem : m_listItem ) {
		if( pItem->GetpProp()->strIdHero == pHero->GetstrIdentifer() )
			return pItem->GetNum();
	}
	return 0;
}

ID XAccount::GetsnSoulStone( const _tstring& strId )
{
	for( auto pItem : m_listItem ) {
		if (pItem->GetpProp()->strIdHero == strId)
			return pItem->GetsnItem();
	}
	return 0;
}
/**
 @brief snHero영웅을 승급시킨다.
*/
XGAME::xtError XAccount::DoPromotionHero(ID snHero)
{
	XSPHero pHero = GetHero(snHero);
	if (XBREAK(pHero == nullptr))
		return XGAME::xE_NOT_FOUND;
	auto bOk = IsPromotionHero(pHero);
	if( bOk != XGAME::xE_OK )
		return bOk;
	// 해당영웅의 영혼석 아이템을 인벤에서 얻음.
	auto pItemPiece = GetSoulStoneByHero( pHero->GetstrIdentifer().c_str() );
	if ( XASSERT(pItemPiece) ) {
		// 승급에 필요한 개수만큼 삭제시킴,
		int numNeed = XGAME::GetNeedSoulPromotion(pHero->GetGrade());
		XBREAK( numNeed == 0 );
		auto result = DestroyItemBySN(pItemPiece->GetsnItem(), numNeed );
		XBREAK( result == 0 );
		return pHero->DoPromotion();
	}
	return XGAME::xE_OK;
}

/**
 @brief idsHero의 영혼석 아이템을 얻는다.
*/
XBaseItem* XAccount::GetSoulStoneByHero( LPCTSTR idsHero )
{
	for( auto pItem : m_listItem ) {
		if( pItem->GetpProp()->strIdHero == idsHero )
			return pItem;
	}
	return nullptr;
}

int XAccount::GetNeedSoulPromotion( XSPHero pHero )
{
	return XGAME::GetNeedSoulPromotion( pHero->GetGrade() );
}
/**
 @brief pHero가 승급이 가능한가.
*/
XGAME::xtError XAccount::IsPromotionHero( XSPHero pHero )
{
	auto bOk = pHero->_IsPromotionForXAccount();
	if( bOk != XGAME::xE_OK ) 
		return bOk;
	auto pProp = pHero->GetpProp();
	if( pProp ) {
		int numHave = GetNumSoulStone( pHero->GetstrIdentifer() );
		int numNeed = XGAME::GetNeedSoulPromotion( pHero->GetGrade() );
		if( numHave >= numNeed )
			return XGAME::xE_OK;
	}
	return XGAME::xE_NOT_ENOUGH_ITEM;
}
/**
 @brief 영혼석으로 소환할수 있는 상태인지 검사한다.
*/
XGAME::xtError XAccount::IsAbleSummonHeroBySoulStone( const _tstring& strIdHero)
{
	auto pProp = PROP_HERO->GetpProp(strIdHero);
	if (pProp) {
		int numHave = GetNumSoulStone( strIdHero );
		// 최초소환은 1성부터 시작한다.
		int numNeed = XGAME::GetNeedSoulSummon();
		if( numHave >= numNeed )
			return XGAME::xE_OK;
	}
	return XGAME::xE_NOT_ENOUGH_ITEM;
}
/**
 @brief 보요한 영혼석중에 현재 소환가능한 영웅이 있는가.
*/
bool XAccount::IsAbleSummonHero()
{
	bool bRet = false;
	XList4<XBaseItem*> listSoulStone;
	GetListSoulStoneExcludeHaveHero( &listSoulStone );
	for( auto pItem : listSoulStone ) {
		auto pPropItem = pItem->GetpProp();
		auto pPropHero = PROP_HERO->GetpProp( pPropItem->strIdHero );
		if( XASSERT( pPropHero ) ) {
			// 소환할수 있는게 한개라도 있으면 즉시 멈춤.
			auto bOk = IsAbleSummonHeroBySoulStone( pPropHero->strIdentifier );
			if( bOk == XGAME::xE_OK ) {
				return true;
			}
		}
	}
	return false;
}
/**
 @brief 보유한 영웅중에 승급가능한 영웅이 있는가.
*/
bool XAccount::IsAblePromotionHero()
{
	bool bRet = false;
	for( auto pHero : m_listHero ) {
		if( IsPromotionHero( pHero) == XGAME::xE_OK )
			return true;
	}
	return false;
}

/**
 @brief idPropHero를 영혼석으로 소환한다.
*/
#ifndef _CLIENT
XGAME::xtError XAccount::DoSummonHeroByPiece( ID idPropHero, XSPHero *ppOut )
{
	auto pPropHero = PROP_HERO->GetpProp( idPropHero );
	if( XBREAK( pPropHero == nullptr ) )
		return XGAME::xE_NOT_FOUND;
	auto bOk = IsAbleSummonHeroBySoulStone( pPropHero->strIdentifier );
	if( bOk != XGAME::xE_OK )
		return bOk;
	// 해당영웅의 영혼석 아이템을 인벤에서 얻음.
	auto pItemPiece = GetSoulStoneByHero( pPropHero->strIdentifier );
	if( XASSERT( pItemPiece ) ) {
		// 승급에 필요한 개수만큼 삭제시킴,
// 		int numNeed = XGAME::GetNeedSoulSummon( pPropHero->GetGrade() );
		// 모든 소환은 1성부터 시작하므로.
// 		int numNeed = XGAME::GetNeedSoulSummon( XGAME::xGD_COMMON );
		int numNeed = XGAME::GetNeedSoulSummon();
		XBREAK( numNeed == 0 );
		auto result = DestroyItemBySN( pItemPiece->GetsnItem(), numNeed );
		XBREAK( result == 0 );
		auto pHero = CreateAddHero( pPropHero->idProp  );
		XBREAK( pHero == nullptr );
		if( ppOut )
			*ppOut = pHero;
	}
	return XGAME::xE_OK;
}
#endif // not _CLIENT

/**
 @brief pProp영웅을 생성해서 인벤에 넣는다. 
 @param unit 특정 유닛을 가진상태로 생성하려면 지정한다.
*/
#if defined(_XSINGLE) || !defined(_CLIENT)
XSPHero XAccount::CreateAddHero( ID idHero, XGAME::xtUnit unitExtern )
{
	auto pProp = PROP_HERO->GetpProp( idHero );
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	auto unit = unitExtern;
	if( unit == XGAME::xUNIT_NONE ) {
		XGAME::xtUnit units[] = {
			XGAME::xUNIT_SPEARMAN,
			XGAME::xUNIT_ARCHER,
			XGAME::xUNIT_PALADIN,
		};
		//지휘 타입에 따른 기본 병력 세팅
		int idx = 0;
		if( pProp->IsRange() )
			idx = 1;
		else if( pProp->IsSpeed() )
			idx = 2;
		unit = units[ idx ];	// 최초 등장은 1레벨부터이므로 소형으로 선택됨.
	}
	const int lvSquad = 1;
	auto pHero = XHero::sCreateHero( pProp, lvSquad, unit, GetThis() );
	if( XASSERT(pHero) ) {
		AddHero( pHero );
	}
	return pHero;
}
XSPHero XAccount::CreateAddHero( const _tstring& idsHero, XGAME::xtUnit unitExtern )
{
	auto pProp = PROP_HERO->GetpProp( idsHero );
	if( XASSERT(pProp) ) {
		return CreateAddHero( pProp->idProp, unitExtern );
	}
	return nullptr;
}
#endif // defined(_XSINGLE) || !defined(_CLIENT)


/**
 @brief 이미 열린지역인데 아직 안생긴 스팟이 있으면 새로 생성시킨다.
 어떤 지역이 닫혀있는데 그지역의 스팟이 열려있으면 삭제시킨다.
*/
void XAccount::UpdateSpots( XDelegateSpot *pDelegate )
{
	// 아직 구름이 열리지 않았지만 hexa타일이 열려있어서 나타나야 하는 스팟을 생성
	XVector<XPropCloud::xCloud*> aryArea;
	// 프로퍼티내 모든 구름지역을 꺼냄.
	PROP_CLOUD->GetPropToAry( &aryArea );
	//
	for( auto pPropArea : aryArea ) {
		// 이미 오픈된 지역이면 기존처럼 하위스팟을 모두 생성한다.
		if( m_pWorld->IsOpenedArea( pPropArea->idCloud ) ) {
			CreateSpotsByCloud( pPropArea, pDelegate );
		} else {
			// 아직 오픈되지 않은 지역이면
			// 이 지역내의 스팟들을 루프돈다.
			for( auto idSpot : pPropArea->spots ) {
				auto pPropSpot = PROP_WORLD->GetpProp( idSpot );
				if( XASSERT(pPropSpot) ) {
					// 이 스팟이 뚫려있는 스팟인가.
					if( PROP_CLOUD->IsOpenedSpot( idSpot, pPropArea, pPropSpot->vWorld ) ) {
						// opened속성 스팟만..
						if( pPropSpot->IsOpened() ) {
							if( !m_pWorld->IsHaveSpot( idSpot ) ) {
								auto pSpot = CreateNewSpot( pPropSpot, pDelegate );
								if( XASSERT( pSpot ) ) {
									// 생성성공
								}
							}
						}
					}
				}
			}
		}
	}
	// 어떤 지역이 닫혀있는데 그 지역의 스팟이 열려있으면 삭제시킨다.
	m_pWorld->UpdateSpots();
}

/**
 @brief 구름지역 프로퍼티와 다시 비교해서 잘못열리거나 닫힌게 있으면 수정한다
 프로퍼티에 변경이 생겼을때를 위한 자동화 도구.
*/
void XAccount::UpdateCloud()
{
	int lvAreaMax = 0;
	// 현재 레벨로는 열수없는 지역이 있으면 다시 닫아준다.
	XVector<XPropCloud::xCloud*> aryAll;
	PROP_CLOUD->GetPropToAry( &aryAll );
	for( auto pProp : aryAll ) {
		// 이미 열린지역중에서.
		if( m_pWorld->IsOpenedArea( pProp->idCloud ) ) {
			if( pProp->lvArea > lvAreaMax )
				lvAreaMax = pProp->lvArea;
			xtError err = xE_OK;
			auto bLock = IsLockArea( pProp->idCloud, &err );
			if( bLock && err != xE_MUST_NEED_KEY_ITEM ) {		// 키 아이템 없어서 잠긴경우는 무시한다.
				m_pWorld->DoCloseArea( pProp->idCloud );
			}
		}
	}
	// 비동기나서 저장이 잘못된경우 다시 세팅함.
	if( GetlvLimit() < lvAreaMax + 1 )
		SetlvLimit( lvAreaMax + 1 );
}

#ifdef WIN32
bool XAccount::IsCompletedQuest( const std::string& idsQuest )
{
	_tstring tidsQuest = C2SZ( idsQuest );
	return IsCompletedQuest( tidsQuest );
}
#endif // WIN32
/**
 @brief 퀘스트가 보상까지 받고 완전히 종료되었는지 검사.
*/
bool XAccount::IsCompletedQuest( const _tstring& idsQuest )
{
	auto pProp = XQuestProp::sGet()->GetpProp( idsQuest );
	if( XASSERT(pProp) ) {
		return m_pQuestMng->IsCompleteQuest( pProp->idProp );
	}
	return false;
}
/**
 @brief 아직 퀘목록에는 있지만 목표를 달성하여 ?상태인지 검사.
*/
bool XAccount::IsFinishedQuest( const _tstring& idsQuest )
{
	auto pProp = XQuestProp::sGet()->GetpProp( idsQuest );
	if( XASSERT( pProp ) ) {
		return m_pQuestMng->IsFinishedQuest( pProp->idProp );
	}
	return false;
}
/**
 @brief pBaseSpot의 전투력이 this의 전투력 이상인가
*/
bool XAccount::IsSameOverThanMe( XSpot *pBaseSpot )
{
	if( pBaseSpot->GetPower() == 0 )
		return true;		// 상대 전투력을 모르면 일단 트루
	int level = XGAME::GetHardLevel( pBaseSpot->GetPower(), GetPowerExcludeEmpty() );
	return level >= 0;
}
/**
 @brief pBaseSpot의 전투력이 녹색이상인가.
*/
bool XAccount::IsGreenOver( XSpot *pBaseSpot )
{
	if( pBaseSpot->GetPower() == 0 )
		return true;		// 상대 전투력을 모르면 일단 트루
	int level = XGAME::GetHardLevel( pBaseSpot->GetPower(), GetPowerExcludeEmpty() );
	return level >= -1;
}

bool XAccount::IsGreenOver( int powerEnemy )
{
	if( powerEnemy == 0 )
		return true;		// 상대 전투력을 모르면 일단 트루
	int level = XGAME::GetHardLevel( powerEnemy, GetPowerExcludeEmpty() );
	return level >= -1;
}

int XAccount::GetGradeLevel( int powerEnemy )
{
	return XGAME::GetHardLevel( powerEnemy, GetPowerExcludeEmpty() );
}

#if defined(_CLIENT) || defined(_GAME_SERVER)
int XAccount::GetmaxAP( int level ) 
{
	return XPropUser::sGet()->GetTable( level )->apMax;
}
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

int XAccount::GetCostOpenFog( XSPLegion spLegion )
{
	int maxFog = spLegion->GetMaxFogs( XLegion::xMAX_NOT_FOG );
	int numFog = spLegion->GetNumFogs();
//	int goldCost = (int)(::powf( 100.f, (float)( maxFog - numFog + 1) ));
	int cnt = maxFog - numFog;
	int bitMul = 1;
	bitMul <<= cnt;
	int goldCost = 100 * bitMul;
	return goldCost;
}
/**
 @brief 다른 유저로부터 침공이 가능한 상태인가.
*/
bool XAccount::IsAblePvP()
{
	return GetpWorld()->IsOpenedArea(_T("area.1012"));
}
/**
 @brief 보유한 영웅들을 어레이에 담아준다. GetInvenHero와 기능이 같음.
*/
void XAccount::GetarypHeroByInven( XVector<XSPHero> *pAryOut )
{
	*pAryOut = m_listHero;
}
/**
 @brief 보유한 영웅들의 idProp을 어레이에 담아준다.
*/
void XAccount::GetaryidPropHeroByInven( XVector<ID> *pAryOut )
{
	for( auto pHero : m_listHero )
		pAryOut->Add( pHero->GetidProp() );
}

void XAccount::StartTimerByTrader() 
{
	const xSec secRecall = (xSec)XGC->GetsecTraderReturn( GetLevel() );
	m_timerTrader.ClearTimer();
	m_timerTrader.Set( secRecall );		// 무역상 떠나있는동안 레벨이 오를수 있으나 미리 돌아오는 시간을 넣어야 한다.
}

/**
 @brief 무역상 귀환 남은시간
*/
int XAccount::GetsecRemainByTraderRecall() const
{
	if( m_timerTrader.IsOff() )
		return 0;
	return m_timerTrader.GetsecRemain();
}

/**
 @brief 지금 무역상을 소환할 수 있는가.
*/
bool XAccount::IsCallableTraderByGem()
{
	// 이미 무역상이 와있다면 false
	if( IsReadyTrader() )
		return false;
	// 캐시가 충분하면 true;
	const auto cost = GetCashForTraderRecall();
	return IsEnoughCash( cost );
}

/**
 @brief 지금 무역상 와있는가.
*/
bool XAccount::IsReadyTrader() const
{
	return m_timerTrader.IsOff() || m_timerTrader.IsOver();
}


/**
 @brief 무기점 아이템 리스트 갱신시간이 지났는가.
*/
bool XAccount::IsOverArmoryItemReset() const
{
	const auto secReset = XGC->m_shopCallInerval;
	return 	m_timerShop.IsOver( secReset );

}

void XAccount::SerializeTimerByTrader( XArchive& ar )
{
	ar << m_timerTrader;
}

void XAccount::DeserializeTimerByTrader( XArchive& ar )
{
	m_timerTrader.DeSerialize( ar );
}
void XAccount::DeserializeTimerByArmory( XArchive& ar )
{
	m_timerShop.DeSerialize( ar );
}

/**
 @brief 무역상 호출에 필요한 캐시템 개수
*/
int XAccount::GetCashForTraderRecall() const
{
	return ((GetLevel() / 10) + 1) * 10;
}

/**
 @brief pHero영웅에게 expAdd가 더해졌을때 예상되는 레벨을 리턴한다.
 @param bAccLvLimit 군주레벨제한을 적용할지 
*/
int XAccount::GetLvHeroAfterAddExp( XSPHero pHero, XGAME::xtTrain type, int expAdd, bool bAccLvLimit, int *pOutExp )
{
	// 현재 레벨 객체를 복사받음.
	XFLevel lvObj = pHero->GetLevelObj( type );
	lvObj.AddExp( expAdd );
	if( bAccLvLimit ) {
		const int lvLimit = pHero->GetLvLimitByAccLv( GetLevel(), type );
		if( lvObj.GetLevel() > lvLimit ) {
			lvObj.SetLevel( lvLimit );
			lvObj.SetExp( lvObj.GetMaxExpCurrLevel() );
		}
	}
	if( pOutExp )
		*pOutExp = lvObj.GetExp();
	return lvObj.GetLevel();
}

bool XAccount::IsAPTimeOver() const 
{
	const auto secByAP = XGlobalConst::sGet()->m_secByAP;
	if( m_timerAP.IsOver( secByAP ) )
		return true;
	return false;
}

/**
 @brief pBaseSpot이 소탕가능한 상태인가.
 1.상대가 녹색이어야 한다.
 2.3별 클리어 상태여야 한다.
*/
XGAME::xtError XAccount::IsAbleKill( XSpot* pBaseSpot )
{
	const ID idSpot = pBaseSpot->GetidSpot();
	const int powerEnemy = pBaseSpot->GetPower();
	const bool bNpc = pBaseSpot->IsNpc();
	if( pBaseSpot->IsNoAttack() )
		return xE_CAN_NOT;
	if( XBREAK(powerEnemy == 0) )
		return xE_ERROR_CRITICAL;
	if( GetLevel() < LV_KILL )
		return xE_NOT_ENOUGH_LEVEL;
	// 3별로 깬 상태여야 하고
	const auto numStar = GetNumSpotStar( idSpot );
	if( numStar < 3 ) {
		return xE_NOT_ENOUGH_NUM_CLEAR;
	}
	// 소탕권이 있어야 함.
	const auto numItem = GetNumItems( _T( "item_kill" ) );
	if( numItem <= 0 ) {
		return xE_NOT_ENOUGH_ITEM;
	}
	if( !bNpc ) {
		// 상대가 유저일경우 녹색만 가능
		int powerGrade
			= XGAME::GetHardLevel( powerEnemy, GetPowerExcludeEmpty() );
		if( powerGrade > -1 ) {
			return xE_NOT_ENOUGH_AUTHORITY;
		}
	}
	return xE_OK;
}
XGAME::xtError XAccount::IsAbleKill( ID idSpot )
{
	auto pBaseSpot = m_pWorld->GetpSpot( idSpot );
	if( pBaseSpot ) {
		return IsAbleKill( pBaseSpot );
		return xE_OK;
	}
	return xE_ERROR_CRITICAL;
}
void XAccount::SerializeCheat( XArchive* pOutAr )
{
	auto& ar = (*pOutAr);
	ar << GetLevel();
	ar << GetExp();
	ar << m_GMLevel;
}

void XAccount::DeSerializeCheat( XArchive& ar )
{
	int i0;
	ar >> i0;		SetLevel( i0 );
	ar >> i0;		SetExp( i0 );
	ar >> i0;		m_GMLevel = i0;
}

/**
 @brief 
 @return 헤더에 XDefNetwork.h를 넣기가 부담스러워서 int로 리턴함.
*/
int XAccount::ProcessCheatCmd( const _tstring& strCmdLine )
{
	XGAME::xtParamSync param = xPS_NONE;
	CToken token;
	token.LoadStr( strCmdLine.c_str() );
	while( 1 ) {
		token.GetToken();
		if( token.IsEof() && token.m_Token[0] == 0 )
			break;
		const std::string strCmd = SZ2C( token.m_Token );
		if( strCmd == "gm_level" ) {
			int num = token.GetNumber( false );
			if( !token.IsError() ) {
				m_GMLevel = num;
				return (int)xPS_GM_LEVEL;
			}
		} else
		if( strCmd == "level" ) {
			int num = token.GetNumber( false );
			if( !token.IsError() ) {
				SetLevel( num );
				return (int)xPS_ACC_LEVEL;
			}
		} else
		if( strCmd == "exp" ) {
			int num = token.GetNumber( false );
			if( !token.IsError() ) {
				SetExp( (DWORD)num );
				return (int)xPS_ACC_LEVEL;
			}
		} else
		if( strCmd == "gold" ) {
			int num = token.GetNumber( false );
			if( !token.IsError() ) {
				SetGold( (DWORD)num );
				return (int)xPS_ACC_LEVEL;
			}
		}
		if( strCmd == "sec_simul" ) {
			int num = token.GetNumber( false );
			if( !token.IsError() ) {
				XAccount::s_secOfflineForSimul = num;
				return (int)xPS_NO_SYNC;
			}
		} else
		if( strCmd == "trader" ) {
			return (int)xPS_TRADER;
		}
	} // white
	return (int)param;
}

#ifdef _XSINGLE
void XAccount::SetspLegion( int idxLegion, XSPLegion spLegion ) 
{
	m_aryLegion[idxLegion] = spLegion;
	XBREAK( spLegion && spLegion->IsNpc() );
}
#endif // _XSINGLE

/**
 @brief infoLegion을 바탕으로 군단을 생성하고 영웅을 추가한다.
*/
#ifdef _XSINGLE
// XSPLegion XAccount::CreatespLegion( const XGAME::xLegion& infoLegion )
// {
// 	df
// }
#endif // _XSINGLE

const XBaseItem* XAccount::GetpEquipItemWithHero( XSPHero pHero, XGAME::xtParts parts ) const
{
	const ID snItem = pHero->GetsnEquipItem( parts );
	return GetpcItemBySN( snItem );

}
