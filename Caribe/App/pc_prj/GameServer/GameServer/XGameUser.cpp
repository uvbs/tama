#include "stdafx.h"
#include "XGameUser.h"
#include "XClientConnection.h"
#include "XDBASvrConnection.h"
#include "XMain.h"
#include "XGameUserMng.h"
#include "Network/XPacket.h"
#include "XGame.h"
#include "XAccount.h"
#include "XWorld.h"
#include "XPacketGDB.h"
#include "XSpots.h"
#include "XSpotDaily.h"
#include "XExpTableHero.h"
#include "XExpTableUser.h"
#include "XPropHero.h"
#include "XPropSquad.h"
#include "../XGameUtil.h"
#include "XBaseItem.h"
#include "XQuestProp.h"
#include "XQuestCon.h"
#include "XPropTech.h"
#include "XRanking.h"
#include "server/XGuildMgr.h"
#include "XPropUpgrade.h"
#include "XGuild.h"
#include "XCampObj.h"
#include "XCampObjHero2.h"
#include "XStageObj.h"
#include "XSquadron.h"
#include "XLegion.h"
#include "rapidjson/document.h"
#include "XGlobalConst.h"
#include "Softnyx.h"
#include "server/XGoogle.h"
#include "XSystem.h"
#include "XDefNetwork.h"
#include "XHero.h"
#include "XPropLegionH.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
//#define BTRACE(F,...)		(0)

enum {
	// 졌을때 깎이는 점수양
	xLoseMin = 5,	
	xLoseMax = 10,
};

#define TAG_SOFTNYX		"iap_softnyx"
#define TAG_IAP				"iap"
#define TAG_LOGIN			"login"
#define TAG_BATTLE		"battle"
#define TAG_POST			"post"

// #define CONSOLE_ACC( TAG, F, ... ) \
// 	if( CONSOLE_MAIN->FindidAccLog( GetidAcc() ) == )
// this유저가 로깅리스트에 있을때만 출력. & TAG가 로깅태그에 있을때만 출력.
#define CONSOLE_ACC( TAG, F, ... ) \
	if( CONSOLE_MAIN->IsLogidAcc( GetidAcc() ) ) { \
	__xLogfTag( TAG, XLOGTYPE_LOG, XTSTR("%s:%s(%d)-", F), __TFUNC__, GetspAcc()->GetstrName(), GetidAcc(), ##__VA_ARGS__); \
	}

#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
LPCTSTR GetstrHelloByRandom()
{
	switch( xRandom( 9 ) ) {
	case 0: return _T( "Don't touch me!" ); break;
	case 1: return _T( "이름이 없는 자 사라지지 않으리" ); break;
	case 2: return _T( "젭알 들어오지 마세여 ㅠㅠ" ); break;
	case 3: return _T( "신상백 샀땅~~~~" ); break;
	case 4: return _T( "꽃은 꽃일뿐 뿌리는 될 수 없다." ); break;
	case 5: return _T( "Not today!" ); break;
	case 6: return _T( "ㄴㄴㄴ" ); break;
	case 7: return _T( "떼돈을 벌자!" ); break;
	case 8: return _T( "나 거지임. 오지마셈." ); break;
	}
	return _T( "" );
}
#endif // #if _DEV_LEVEL <= DLV_DEV_EXTERNAL

extern XRanking* RANKING_DB;
//extern XGuildMgr* GUILD_MGR;

using namespace xQuest;
using namespace XGAME;
using namespace xCampaign;
using namespace XGAME;

template<> XPool<XGameUser>* XMemPool<XGameUser>::s_pPool = NULL;

#define MAKE_PCONNECT( A ) \
	XBREAK( GetspConnect() == nullptr ); \
	auto A = SafeCast<XClientConnection*>( GetspConnect().get() )

// XVERIFY_BREAK를 통과 하지 못하면 그것은 해킹이거나 비동기 상태로 간주하고 재접속하게 한다.
#define XVERIFY_BREAK( EXP )	{	\
	if( !m_spAcc->m_bDebugMode && XBREAK( EXP ) )	{	\
		LogAccount(); \
		return 0;	\
	}	\
}

#define XVERIFY_BREAKF( EXP, F, ... )	{	\
	if( !m_spAcc->m_bDebugMode && XBREAKF( EXP, F, ##__VA_ARGS__ ) )	{	\
		LogAccount(); \
		return 0;	\
	}	\
}

#pragma message("패킷들의 값만 유효함을 검사하지 말고 패킷이 오는 시점의 유효함도 검사해야할듯 싶다. 예를들어 RecvUIAction이 올 상황이 아닌데 오는경우.")
//////////////////////////////////////////////////////////////////////////
XGameUser::XGameUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect ) 
	: XEUser( pUserMng, spConnect )
{ 
	Init(); 
	m_timer1Min.Set( 60.f );	
//	m_messagetimer2min.Set(5.f * 2);
}

void XGameUser::Destroy()
{
	m_spAcc.reset();
//	SAFE_DELETE( m_pEnemyLegion );
//	Save( );
}

/**
 @brief this유저의 기본정보를 로그로 출력한다.
*/
void XGameUser::LogAccount() 
{
	if( XBREAK( GetspConnect() == nullptr ) )
		return;
	CONSOLE( "idAcc=%d, uuid=%s, nick=%s, ip=%s",
																					m_spAcc->GetidAccount(),
																					m_spAcc->GetstrUUID().c_str(),
																					m_spAcc->GetstrName(),
																					GetspConnect()->GetszIP() );
}

/**
 @brief 서버측에서 명시적으로 유저를 잘라낼때 호출된다.
*/
void XGameUser::OnFireConnectBefore()
{
}

int XGameUser::RecvCheat( XPacket& p )
{
#ifdef _CHEAT
	int type;
	p >> type;
	// exp증가
	if( type == 1 )	{
		int exp;
		p >> exp;
		if( exp > 0 ) {
			auto lvLimit = m_spAcc->GetlvLimit();
			m_spAcc->SetlvLimit( 51 );
			m_spAcc->AddExp( exp );
			m_spAcc->SetlvLimit( lvLimit );
		} else
		if( exp < 0 ) {
			m_spAcc->GetXFLevelObj().SubExp( -exp );
		}
		// 치트로 올린건 편의상 렙업창이 안뜨게 하기 위해.
		m_spAcc->GetXFLevelObj().SetbLevelUp( FALSE );
		SendLevelSync();
	} else
	// 영웅레벨 증가.
	if( type == 2 )	{
		DWORD snHero;
		int level;
		DWORD dw0;
		p >> snHero >> level >> dw0;
		auto type = (XGAME::xtTrain)dw0;
		XHero *pHero = m_spAcc->GetHero(snHero);
		if( XASSERT(pHero) ) {
			pHero->SetLevel( type, level );
		}
	} else
	// 금화 증가
	if( type == 3 )	{
		DWORD gold;
		p >> gold;
		m_spAcc->AddGold( gold );
	} else
	// 캐쉬증가
	if( type == 4 ) {
		int add;
		p >> add;
		m_spAcc->AddCashtem( add );
	}
	// 스팟 리젠
	if( type == 5 )	{
		ID idSpot;
		p >> idSpot;
		auto pSpot = GetpWorld()->GetSpot( idSpot );
		// 리젠 타임을 강제로 돌린다.
		if( pSpot )
			pSpot->GettimerSpawnMutable().AddTime( 99999.f );
	} else
	// 퀘 강제 클리어
	if( type == 6 )	{
		ID idQuestProp;
		p >> idQuestProp;
		XQuestObj *pObj = m_spAcc->GetpQuestMng()->GetQuestObj( idQuestProp );
		if( pObj )
		{
			pObj->SetAllComplete();
			// 퀘완료를 시키기위해 
			m_spAcc->GetpQuestMng()->_UpdateQuest();
			DoRewardQuest( idQuestProp );
		}
	} else
	if( type == 7 )	{
		int add;
		p >> add;
		for( int i = 0; i < XGAME::xRES_MAX; ++i )
			m_spAcc->AddResource( (XGAME::xtResource)i, add );
	} else
	if( type == 8 )	{
		DWORD idProp;
		int num;
		p >> idProp;
		p >> num;
		for( int i = 0; i < num; ++i )
		{
			auto pProp = PROP_HERO->GetpProp( idProp );
			if( pProp )
			{
				XGAME::xtSize tierUnit = XGAME::xSIZE_SMALL;
				auto unit = XGAME::GetRandomUnit( pProp->typeAtk, tierUnit );
				auto pHero = XHero::sCreateHero( pProp, 1, unit );
				m_spAcc->AddHero( pHero );
				SendCreateHero( pHero );
			}
		}
	} else
	if( type == 9 )	{
		ID idSpot;
		p >> idSpot;
	} else
	if( type == 10 ) {
		int mode;
		p >> mode;
	} else
	if( type == 11 ) {
		int num;
		p >> num;
		m_spAcc->SetnumFreeSlot( num );
	} else
	if( type == 12 ) {
		int score;
		p >> score;
		m_spAcc->SetLadder( score );
	} else
	if( type == 13 ) {
		int ap;
		p >> ap;
		m_spAcc->SetAP( ap );
	} else
	if( type == 14 ) {
		int mode;
		p >> mode;
		m_spAcc->m_bDebugMode = (mode != 0);
	} else
	if( type == 15 ) {
		UpdateOccurQuestCheat();
	} else
	if( type == 16 ) {
		int i0;
		p >> i0 >> i0 >> i0 >> i0;
		_tstring strParam;
		p >> strParam;
		std::string idsSeq = SZ2C(strParam);
		m_spAcc->_ClearCompletedSeq( idsSeq );
	} else
	if( type == 17 ) {
		ID idArea;
		p >> idArea;
		m_spAcc->GetpWorld()->DoCloseArea( idArea );
	} else
	if( type == 18 ) {
		ID idQuest;
		p >> idQuest;
		m_spAcc->GetpQuestMng()->_DelQuest( idQuest );
	} else
	if( type == 19 ) {
		ID idQuest;
		p >> idQuest;
		m_spAcc->GetpQuestMng()->_DelQuestByCompleted( idQuest );
	} else
	if( type == 20 ) {
//		AddPostSubscribe();
		Cheat_AddPostInfoGuildCoin();
	} else
	if( type == 21 ) {
		ID idSpot;
		p >> idSpot;
		auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
		if( XASSERT(pBaseSpot) ) {
			auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
			if( pSpot ) {
				auto spCampObj = pSpot->GetspCampObj( m_spAcc->GetidGuild() );
				if( spCampObj ) {
					spCampObj->_DoClear();
				}
			}
		}
	} else
	if( type == 22 ) {
		ID idSpot;
		DWORD param2;
		p >> idSpot;
		p >> param2;
		auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
		if( XASSERT( pBaseSpot ) ) {
			auto pSpot = SafeCast<XSpotDaily*>( pBaseSpot );
			if( pSpot ) {
				switch( param2 ) {
				case 0:	pSpot->SetnumEnter( 0 ); break;
				case 1: pSpot->IncFloor(); break;
				case 2: pSpot->DecFloor(); break;
				case 3: pSpot->ClearCheat(); break;
				}
				SendSpotSync( pSpot );
			}
		}
	} else
	if( type == 99 ) {
#if _DEV_LEVEL >= DLV_OPEN_BETA
		if( m_spAcc->GetGMLevel() == 5 ) 
#endif
		{
			int i0;
			_tstring strCmdLine;
			p >> i0 >> i0 >> i0 >> i0;
			p >> strCmdLine;
			auto paramSync = (xtParamSync)( m_spAcc->ProcessCheatCmd( strCmdLine ) );
			XConsole( _T("%s:%s"), __TFUNC__, strCmdLine.c_str() );
			if( paramSync == xPS_NONE ) {
				XConsole( _T("치트명령 처리하지 못함.") );
			}
			SendSyncAcc( paramSync );
		}
	} else
	// 용맹포인트 증가.
	if( type == 200 ) {
		int add;
		p >> add;
//		m_spAcc->AddBrave( add );
		m_spAcc->AddGold( add );
	}
#endif
	return 1;
}

int XGameUser::RecvCheatCreateItem( XPacket& p )
{
#ifdef _CHEAT
	ID idProp;
	int num;
	p >> idProp;
	p >> num;
	XPropItem::xPROP *pProp = PROP_ITEM->GetpProp( idProp );
	XVERIFY_BREAK( pProp == nullptr );
	if ( num > 100 )
		num = 100;
	XArrayLinearN<XBaseItem*, 256> ary;
	m_spAcc->CreateItemToInven( pProp, num, &ary );
	if( pProp->strIdentifier.find(_T("book")) != _tstring::npos ) {
		XARRAYLINEARN_LOOP( ary, XBaseItem*, pItem ) {
			// 치트로 만든 책은 경험치를 넣어준다.
			pItem->SetBookExp( XGC->m_expPerBook );
		} END_LOOP;
	}
	// 클라로 결과 보내줌.
	XPacket ar( (ID)xCL2GS_CHEAT_CREATE_ITEM );
	ar << ary.size();
	ar << VER_ITEM_SERIALIZE;
	XARRAYLINEARN_LOOP( ary, XBaseItem*, pItem ) {
		XBaseItem::sSerialize( ar, pItem );
	} END_LOOP;
	Send( ar );
	///< 
//	for( int i = 0; i < num; ++i )
	{
		xQuest::XEventInfo infoQuest;		// 이벤트 정보.
		infoQuest.SetidItem( idProp );
		infoQuest.SetAmount( num );
		DispatchQuestEvent( XGAME::xQC_EVENT_GET_ITEM, infoQuest );
	}

#endif
	return 1;
}


// 로그인서버로 계정정보를 보낸다
BOOL XGameUser::Save( void )
{	
	// 자원지에서 생산된 자원 정산
	CalculateResource();
	m_spAcc->UpdatePower();      // 현재 전투력을 계산
	if( DBA_SVR ) {
		// 저장할때 게임서버 시간을 저장해둔다.
		m_spAcc->SetsecLastSave();
		DBA_SVR->SendSaveAccountInfo( m_spAcc, FALSE, 0 );
		AddLog(XGAME::xULog_Account_Save, _T("UserInfo Update"));
	}

	return TRUE;
}

XCrypto* XGameUser::GetpCryptObj() 
{
	MAKE_PCONNECT( pConnect );
	if( XASSERT( pConnect ) )
		return &( pConnect->GetCryptObj() );
	return nullptr;
}

// 현재 까지의 스팟 타이머로 자원을 정산한다.
void XGameUser::CalculateResource( void )
{
	XArrayLinearN<XSpot*, 1024> ary;
	GetpWorld()->GetSpotsToAry( &ary );
	XARRAYLINEARN_LOOP( ary, XSpot*, pSpot ) {
		// 각 스팟별로 자원을 정산시킨다.
		pSpot->CalculateResource( m_spAcc->GetidAccount(), 0 );
	} END_LOOP;
	GetpWorld()->UpdateLastCalc();
}

void XGameUser::SendDuplicateConnect()
{
	MAKE_PCONNECT( pConnect );	
	pConnect->SendAccountDuplicateLogout();
// 	XPacket ar((ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT);
//     pConnect->Send( ar );		// 클라이언트로 데이타를 보냄	
}

void XGameUser::Send( XPacket& ar )
{
	MAKE_PCONNECT( pConnect );	
	pConnect->__SendForXE( ar );		// 클라이언트로 데이타를 보냄
}

/**
@brief 클라로부터 로그인 요청을 받으면 게임서버에서 유저 객체를 생성한후
 계정데이타를 유저에게 보내기전에 할일을 처리한다. 
 아직 클라에는 m_spAcc를 보내기전이라는것에 유의.
*/
void XGameUser::SuccessLoginBeforeSend( XSPSAcc spAcc, BOOL bReconnect )
{	
	spAcc->SetQuestDelegate( this );
	m_spAcc = spAcc;
	// 접속하면 
	// 현재 레벨의 최대ap양을 산정해서 ap통 만듬.
	m_spAcc->SetmaxAP( m_spAcc->GetmaxAP(GetLevel()) );
	// 오프라인동안의 시간을 계산해서 ap채워줌.
	const xSec secOffline = XTimer2::sGetTime() - m_spAcc->GetsecLastSaved();
	CONSOLE_TAG("connect", "secOffline:%s", XGAME::GetstrResearchTime( secOffline ).c_str() );
//	const xSec secRegenAP = 10;	// ap는 n초마다 1씩 증가함.
	// 현재 ap가 풀로안차있을때만 오프라인재생값이 더해짐
	if( m_spAcc->GetAP() < m_spAcc->GetmaxAP() ) {
		int restAP = m_spAcc->GetmaxAP() - m_spAcc->GetAP();
		XBREAK( restAP < 0 );
//		int apAdd = secOffline / secRegenAP;
		float apPerSec = ( XGC->m_apPerOnce / (float)XGC->m_secByAP );
		int apAdd = (int)(secOffline * apPerSec);
// 		int apAdd = (int)((secOffline / 60.f) * XGC->m_apPerOnce);
		if( apAdd > restAP ) {
			apAdd = restAP;
		}
		m_spAcc->SetAP( m_spAcc->GetAP() + apAdd );
	}
	m_spAcc->ResetAPTimer();		// 로딩후엔 클라랑 동기화를 맞추기위해 타이머를 리셋시키는게 좋음. 59초에 로그아웃 한사람은 59초를 손해보는 부분은 있음.
	// 무기점 타이머가 꺼져있으면 타이머를 작동시킴
	if( m_spAcc->GettimerShop().IsOff() ) {
		m_spAcc->ResetTimerByArmory();
		m_spAcc->ChangeShopItemList();
	}
	// 무기점 아이템 갱신시간이 지나면 갱신하고 타이머 리셋
	if( m_spAcc->IsOverArmoryItemReset() || m_spAcc->GettimerShop().IsOff() ) {
		m_spAcc->ChangeShopItemList();
		m_spAcc->ResetTimerByArmory();
	}

	// 구름업데이트
	m_spAcc->UpdateCloud();
	//////////////////////////////////////////////////////////////////////////
	// 로그인 직후에 구름이 열린지역을 모두 조사해서 안생긴 스팟이 있는지 확인한다.
	// 이것은 구름이 걷히고 스팟이 이후에 추가되었을때를 위함이다.
	m_spAcc->UpdateSpots( this );
	// 퀘가 수정되었거나 해서 스팟퀘는 있는데 스팟이 없는경우 새로 생성시켜줌.
	UpdateSpotsByQuest();
	//////////////////////////////////////////////////////////////////////////
	// 접속직후 요일이벤트 스팟이 오늘자 타입으로 제대로 세팅되어있는지 확인한다.
	//////////////////////////////////////////////////////////////////////////
	// 현재 활성화된 요일스팟이 있는지 찾아온다.
// 	{
// 		// 요일스팟이 있을때.
// 		if( GetpWorld()->GetNumSpots( XGAME::xSPOT_DAILY ) > 0 ) {
// 			// 오늘의 요일을 알아낸다.
// 			XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 			// 오늘의 요일에 대한 타입을 알아낸다.
// //			XGAME::xtDailySpot type = XSpotDaily::sGetDowToType( dowToday );
// 			// 자정으로부터 지나간 시간을 세팅한다.
// 			int hour, min, sec;
// 			XSYSTEM::GetHourMinSec( &hour, &min, &sec );
// 			int secPass = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
// 			XSpotDaily *pSpot = GetpWorld()->GetActiveDailySpot();
// 			if( pSpot ) {
// 				// 읽어온 스팟의 타입과 오늘타입을 비교해서 다르면 바꿔준다.
// 				if( /*pSpot->GetType() != type ||*/ pSpot->GetdowToday() != dowToday ) {
// 					// 다르면 이 스팟은 해제시킨다.
// //					pSpot->ReleaseSpot();
// 					// 새 스팟을 랜덤위치에 활성화시킨다.
// 					GetpWorld()->SetActiveDailySpotToRandom( dowToday, secPass, spAcc );
// 				}
// 			} else {
// 				// 요일스팟은 있는데 활성화된 스팟이 없으므로 아무거나 하나 활성화시킴.
// 				GetpWorld()->SetActiveDailySpotToRandom( dowToday, secPass, spAcc );
// 			}
// 		}
// 	}
	// 로그인 직후에 각 스팟들의 처리이벤트
	spAcc->GetpWorld()->OnAfterDeSerialize( this, 
												spAcc->GetidAccount() );
	spAcc->SetpDelegateLevel( this );//		GetLevel().SetpDelegate( this );

	ProcessAttackedJewel( secOffline );		// 정식기능으로 추가
	ProcessAttackedHome( secOffline );
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	// 개발버전에서는 더미가 공격해온것처럼 시뮬레이션
	/*
	광산이 내소유가 된지 일정시간이 흐른뒤면
	적절한 점수대의 유저하나를 얻어온다. 얻어올때 이것은 시뮬레이션 용도라는 플래그도 같이 보내야 함.
	광산이 마치 그 유저에게 뺏긴것처럼 세팅한다.
	전투가 끝난후 시뮬레이션 상대였으면 상대의 DB에 아무런 처리를 하지 않는다.
	*/
//	ProcessAttackedJewel( secOffline );
	ProcessAttackedMandrake( secOffline );
	// 다른 유저로부터 침공당한것같은 시뮬레이션
	{
// 		ProcessAttackedHome( secOffline );
	}
#endif // _DEV
	// 유황 기습했던것 처리.
	m_spAcc->DoStackEncounter( secOffline );

	// 클라로 계정정보 보내줌
	if( bReconnect == FALSE )	{	// 재접상황이면 계정정보는 보내지 않음
		/*CString string1, string2;		string1.Format( _T("SuccessLogin "));		string2.Format( _T("%s"), szIP );		string1 += string2;		DBLog( USER_CONNECT, (TCHAR*)((LPCTSTR)string1));*/
	}	else	{
		/*CString string1, string2;		string1.Format( _T("SuccessLogin "));		string2.Format( _T("%s"), szIP );		string1 += string2;		DBLog( USER_RECONNECT, (TCHAR*)((LPCTSTR)string1));*/
	}
}

/**
 @brief 퀘가 바뀌거나 해서 스팟관련퀘는 있는데 스팟이 없는경우는 스팟을 생성시켜준다.
 이 함수는 SuccessLoginBeforeSend에서만 불리는걸 보장해야함.
*/
void XGameUser::UpdateSpotsByQuest()
{
	XASSERT( m_spAcc->IsLoginBefore() );
	// 현재 가진 퀘목록을 얻는다.
	XVector<XQuestObj*> ary;
	GetpQuestMng()->GetQuestsToAry( &ary );
	// 퀘로부터 발생하는 스팟이 있으면 그 스팟이 있는지 검사.
	for( auto pQuestObj : ary ) {
		UpdateSpotByQuest( pQuestObj, nullptr );
	}
}

/**
 @brief pQuestObj로부터 발생하는 스팟이 있는지 검사해서 생성한다.,
*/
void XGameUser::UpdateSpotByQuest( XQuestObj *pQuestObj, XVector<XSpot*> *pOutAry )
{
	auto pList = PROP_WORLD->GetpPropWhen( pQuestObj->GetstrIdentifer() );
	if( pList )	{
		for( auto pBaseSpotProp : *pList ) {
			if( !GetpWorld()->IsHaveSpot( pBaseSpotProp->idSpot ) ) {
				XSpot *pBaseSpot = m_spAcc->CreateNewSpot( pBaseSpotProp->idSpot, this );
				if( XASSERT(pBaseSpot) ) {
					if( pOutAry )
						pOutAry->Add( pBaseSpot );
					// 정찰퀘용 스팟은 공격금지로 한다.
					if( pQuestObj->IsReconQuest() )
						pBaseSpot->SetbitFlagByNoAttack( true );
	// 				SendSpotSync( pBaseSpot );
				}
			}
		}
	}
}

/**
 @brief 유저가 로그인에 성공한 후 유저에게 계정정보를 보내준 직후에 호출된다.
*/
void XGameUser::SuccessLoginAfterSend()
{
	if( XBREAK( m_spAcc == nullptr ) )
		return;
	m_spAcc->SetnumAttaccked( 0 );   // 클라에 보내줬으므로 클리어.
	// 현재레벨에서 혹시 못받은 퀘가 있으면 보내준다.
	xQuest::XEventInfo infoQuest;		// 이벤트 정보.
	infoQuest.SetLevel( m_spAcc->GetLevel() );
	DispatchQuestEvent( XGAME::xQC_EVENT_LEVEL, infoQuest );
	// 퀘스트 완료조건 재평가.(QC_EVENT_LEVEL이벤트도 이안으로 포함시킬것)
	UpdateQuestCondition();
	///< 접속할때 한번 연구가 끝났는지 확인해서 끝났으면 추가로 패킷보냄.
	auto& research = m_spAcc->GetResearching();
	ID idAbil = research.GetidAbil();
// 	ID idAbil = m_spAcc->GetidAbilResearchNow();
	if( idAbil != 0 && research.IsComplete() ) {
		m_spAcc->DoCompleteCurrResearch();
		auto pPropTech = XPropTech::sGet()->GetpNode( idAbil );
		if( XASSERT(pPropTech) ) {
			auto pHero = m_spAcc->GetHero( research.GetsnHero() );
			if( pHero ) {
				const auto abil = pHero->GetAbilNode( pPropTech->unit, idAbil );
				SendUpdateResearch( xEC_OK, pHero->GetsnHero(), idAbil, abil.point );
			}
		}
	}
	// 퍼블릭키를 보냄.
	if( m_Platform == xPL_GOOGLE_STORE )
		SendIAPPublicKey();
	// 훈련이 완료된 슬롯 아이디를 얻는다.
	XArrayLinearN<ID, 64> ary;
	m_spAcc->GetTrainCompleteSlots( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, snSlot ) {
		// 훈련하나가 끝났으면 보내주고 리스트에서 삭제 후 다시 루프를 돈다.
		auto ret = SendTrainingComplete( snSlot, xEC_OK );
		if( ret == 0 ) {
			auto pSlot = m_spAcc->GetpTrainingSlot( snSlot );
			if( pSlot )
				// 실패하면 슬롯 삭제.
				m_spAcc->DelTrainSlotByHero( pSlot->snHero );
		}
	} END_LOOP;
	{
		// 접속한 게임서버의 아이디를 DB에 저장한다.
		const ID idSvr = XEnv::sGet()->GetGSvrID();
		ID idKey = DBA_SVR->SendUpdateLogin(GetidAcc(), idSvr, GetspConnect()->GetszIP() );
		// 콜백이 되면 우편물을 요청한다.
		DBA_SVR->AddResponse(idKey, this, &XGameUser::cbSendUpdateLogin);
	}
	{
		XVector<XSpot*> ary;
		GetpWorld()->GetSpotsToAry( &ary, XGAME::xSPOT_CASTLE );
		GetpWorld()->GetSpotsToAry( &ary, XGAME::xSPOT_JEWEL );
		GetpWorld()->GetSpotsToAry( &ary, XGAME::xSPOT_MANDRAKE );
		for( auto pSpot : ary ) {
			if( pSpot->IsEnemy() ) {
				BTRACE( "G:%s: nick=%s, idAcc=%d", __TFUNC__, m_spAcc->GetstrName(), GetidAcc() );
				ID idKey
					= DBA_SVR->SendReqHelloMsgByidAcc( pSpot->GetidOwner(), pSpot->GetidSpot() );
				DBA_SVR->AddResponse( idKey, this, &XGameUser::cbReqHelloMsgByidAcc );
			}
		}
	}
}

/**
 @brief idAcc유저의 인사말메시지를 받음.
*/
void XGameUser::cbReqHelloMsgByidAcc( XPacket& p )
{
	ID idAcc, idSpot;
	_tstring strHello;
	p >> idAcc >> idSpot >> strHello;
	BTRACE( "G:%s: nick=%s, idAcc=%d", __TFUNC__, m_spAcc->GetstrName(), GetidAcc() );
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	if( strHello.empty() ) {
		int dice = xRandom(3);
		if( dice == 0 ) {		// 3번중 한번은 메시지를 가상으로 만들어준다.
			strHello = GetstrHelloByRandom();
		}
	}
#endif // #if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	auto pBaseSpot = GetpWorld()->GetpSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		if( pBaseSpot->GetidOwner() == idAcc ) {
			pBaseSpot->SetstrHello( strHello );
			SendSyncSpotWithHello( pBaseSpot );
		}
	}
}

/**
 @brief 스팟의 인사말 메시지를 갱신한다.
*/
void XGameUser::SendSyncSpotWithHello( XSpot *pSpot )
{
	if( XBREAK( pSpot == nullptr ) )
		return;
	const auto type = pSpot->GettypeSpot();
	if( type != XGAME::xSPOT_CASTLE 
		&& type != XGAME::xSPOT_JEWEL 
		&& type != XGAME::xSPOT_MANDRAKE ) {
		return;
	}
	XPacket ar( (ID)xCL2GS_SYNC_HELLO );
	ar << pSpot->GetidSpot();
	ar << pSpot->GetstrHello();
	Send( ar );
}


/**
 @brief 접속후 몇몇 정보를 DB에 써놓고 콜백한다.
 접속한 게임서버의 아이디.
*/
void XGameUser::cbSendUpdateLogin(XPacket& p)
{
	ID result;
	p >> result;
	if( result ) {
		// 메일함 전체를 요청한다.
		ID idKey = DBA_SVR->SendPostInfoRequest( m_spAcc );
		if( idKey )
			DBA_SVR->AddResponse( idKey, this, &XGameUser::cbSendPostInfoRequest );
	}
}

/**
 @brief 요청한 메일함 데이터가 도착함.
*/
void XGameUser::cbSendPostInfoRequest(XPacket& p)
{
	ID numPost;
	p >> numPost;
	//
	if( numPost > 0 ) {
		XArchive arPosts;
		p >> arPosts;
		m_spAcc->DeSerializePostInfo( arPosts, VER_POST );
	}
	// 우편함을 동기화시킨다.(일단 1차로 보내고)
	SendSyncPosts();
	// 접속하지 않아서 못받은 월정액상품이 있으면 다 보낼때까지 리커시브로 보낸다.
	DoSubscribeAsyncRecursive( m_Platform );
}

//Save
void XGameUser::OnDestroy()
{
	// 마지막 접속한 게임서버아이디를 0으로 클리어 한다.
	DBA_SVR->SendUpdateAccountLogout( 0, GetidAcc() );
// 	DBA_SVR->SendUpdateAccountLogout( 0
// 		, GetidAccount()
// 		, 0 );	// ?? 이건 왜 0으로 보내징?
	AddLog(XGAME::xULog_Account_Logout, _T("Logout"));
}

void XGameUser::Process( float dt ) 
{
	// 범용 1분타이머 
	if( m_timer1Min.IsOver() ) {
		// 스페셜스팟이 있고 아직 활성화된 스팟이 없다면
// 		if( m_spAcc->IsActiveSpecialSpotTimer() && 
// 			GetpWorld()->GetActivatedSpecialSpot() == nullptr ) {
// 			// 지금 활성화 해야하는지 검사.
// 			XSpotSpecial *pSpot = m_spAcc->DoCheckSpecialSpotActivateNow();
// 			if( pSpot )
// 				SendSpotSync( pSpot );
// 		}
		if( m_spAcc->GetnumSubscribe() > 0 ) {
			// 월정액 상품 지급중
			const xSec secCurr = XTimer2::sGetTime();
			const xSec secNext = m_spAcc->GetsecNextSubscribe();
#ifdef _DEBUG
			const xSec secRemain = secNext - secCurr;		// 디버깅용
			const int minRemain = secRemain / 60;				// 디버깅용
#endif // _DEBUG
			if( secCurr >= secNext ) {
				// 온라인중에 지급시간이 지남
				AddPostSubscribe( m_Platform );			// 지급 메일 보냄
				// 지급시간을 24시간 증가시킴
				const float secSubscribeCycle = (float)XGC->m_secSubscribeCycle;
				const int daySubscribe = XGC->m_daysSubscribe;		// 지급할 일수
				m_spAcc->SetsecNextSubscribe( secNext + (DWORD)secSubscribeCycle );	// 24시간 뒤를 다음 지급시간으로
				if( m_spAcc->AddnumSubscribe( 1 ) >= daySubscribe ) {	// 지급횟수를 증가.
					// 30개를 다 채웠으면 만료
					m_spAcc->SetnumSubscribe( 0 );
					m_spAcc->SetsecNextSubscribe( 0 );
				}
			}
		}
		// 무기점  갱신시간이 지났는가.
		if( m_spAcc->IsOverArmoryItemReset() ) {
			SendShoplistMerchant(); // SendToClient Shop 등장 알림.
			m_spAcc->ResetTimerByArmory();
		}
		//
		m_timer1Min.Reset();
	}
	/*
	스팟 타이머 업데이트 등의 일을 함.
	*/
	m_spAcc->Process( dt, this );

	// 무역상 귀환시간이 지났는가.(이제 클라측에서 요청하는걸로 바뀜)
// 	if( m_spAcc->IsOverTraderRecall() ) {
// 		SendTradeMerchant();
// 		m_spAcc->OffTimerByTrader();
// 	}
}

DWORD XGameUser::OnDelegateGetMaxExp( const XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) const
{
	return EXP_TABLE_USER->GetMaxExp( level );
}

int XGameUser::OnDelegateGetMaxLevel( const XFLevel *pLevel, DWORD param1, DWORD param2 ) const
{ 
	return EXP_TABLE_USER->GetMaxLevel();
}
void XGameUser::OnDelegateLevelUp( XFLevel *pLevel ) 
{
	// 렙업하면 강제로 전투력한번 업데이트 시킴
	m_spAcc->UpdatePower();
	xQuest::XEventInfo infoQuest;
	infoQuest.SetLevel( GetLevel() );
	DispatchQuestEvent( XGAME::xQC_EVENT_LEVEL, infoQuest );
	// 렙업하면 언락되어 있는 모든 유닛들에게 테크포인트 1씩 준다.
//	m_spAcc->AddTechPointAll( 1 );
	m_spAcc->SetmaxAP( m_spAcc->GetmaxAP(GetLevel()) );
	m_spAcc->SetAP( m_spAcc->GetmaxAP() );
// 	XARRAYLINEARN_LOOP_AUTO( XGC->m_aryLevelsUnlockUnitForNotSmall, lv ) {
// 		// 중/대형유닛 언락포인트 받아야 하는 레벨이 되면 포인트를 하나 올림.
// 		if( GetLevel() == lv ) {
// 			int point = m_spAcc->GetnumUnlockTicketForMiddleOrBig();
// 			m_spAcc->SetnumUnlockTicketForMiddleOrBig( point + 1 );
// 			break;
// 		}
// 	} END_LOOP;

	
	_tstring strLog = XE::Format(_T("LevelUp => (%d)"), GetLevel());
	AddLog(XGAME::xULog_Account_Level_Up, strLog);
//	SendLevelSync();  // 여기서 이거 하면 안됨. exp계산 아직 덜끝난 상태임

}


// /**
//  스페셜 스팟이 지금 활성화 되어야 하는지 확인한다.
// */
// BOOL XGameUser::DoCheckSpecialSpotActivateNow( void )
// {
// 	// 스폰타이머 자체가 안켜져있으면 걍 리턴
// 	if( m_spAcc->GetymdSpecialSpotStart().IsActive() == FALSE )
// 		return FALSE;
// 	// 이미 활성화된 스페셜 스팟이 있는지 확인한다.
// 	if( GetpWorld()->GetActivatedSpecialSpot() )
// 		return FALSE;
// 	// 오늘이 스페셜 스팟이 생성되어야 하는 날짜인지 확인한다.
// 	if( IsActivateToday() )
// 		return TRUE;
// 	return FALSE;
// }
// 
// /**
//  오늘 스팟을 활성화 해야 하는 날인지 검사한다.
// */
// BOOL XGameUser::IsActivateToday( void ) 
// {
// /*
// 	int year, month, day;
// 	// 오늘 년/월/일을 알아냄
// 	XSYSTEM::GetYearMonthDay( &year, &month, &day );
// 	int dayStart 
// 		= XSYSTEM::GetYMDToDays( m_spAcc->GetymdSpecialSpotStart().GetYear(),
// 								m_spAcc->GetymdSpecialSpotStart().GetMonth(),
// 								m_spAcc->GetymdSpecialSpotStart().GetDay() );
// 	int dayToday = XSYSTEM::GetYMDToDays( year, month, day );
// */
// 	// 지난일수랑 기다려야 하는 일수랑 딱맞으면 오늘 활성화 해야 함.
// 	if( m_spAcc->GetPassDaySpecialSpot() == m_spAcc->GetdayWaitSpecialSpot() )
// 		return TRUE;
// 	return FALSE;
// }

/**
 스페셜 스팟중 한곳을 활성화 시킨다.
*/
// XSpotSpecial* XGameUser::DoActiveSpecialSpot( void )
// {
// //	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	// 자정으로부터 지나간시간(초)을 구한다.
// 	int hour, min, sec;
// 	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
// 	int secTotal = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
// 	XSpotSpecial *pSpot 
// 		= GetpWorld()->SetActiveSpecialSpotToRandom( secTotal, m_spAcc );
// 	return pSpot;
// }


//////////////////////////////////////////////////////////////////////////
/**
 스팟 정찰(유저/자원지/npc 통합)
*/
int XGameUser::RecvSpotRecon( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XSpot* pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	// 정찰은 회색스팟이어도 가능
	xQuest::XEventInfo infoQuest;
	infoQuest.SetidSpot( pBaseSpot->GetidSpot() );
	infoQuest.SettypeSpot( pBaseSpot->GettypeSpot() );
	infoQuest.SetidCode( pBaseSpot->GetpBaseProp()->idCode );
	infoQuest.SetidArea( pBaseSpot->GetidArea() );
	DispatchQuestEvent( XGAME::xQC_EVENT_RECON_SPOT, infoQuest );
	DispatchQuestEvent( XGAME::xQC_EVENT_RECON_SPOT_TYPE, infoQuest );
	// 정찰됨.
	pBaseSpot->SetbitFlagByReconed( true );
	_tstring strSpotType;
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		XVERIFY_BREAK( pSpot->GetidOwner() == m_spAcc->GetidAccount() );
		if( pSpot->IsQuestion() ) {
			// 아직 매치 안된 스팟
			DoMatchUserSpot( pSpot, 0 );
		} else
		if( pSpot->IsNpc() ) {
			if( pSpot->GetspLegion() == nullptr )
				pSpot->CreateLegion( m_spAcc );
			// NPC가 점령중
			SendReconInfo( true, 0, pSpot );
		} else {
			_tstring strLog = XE::Format(_T("Spot Recon Req(XGAME::xSPOT_CASTLE) Level: %d, SpotID: %d"), GetLevel(), idSpot);
			AddLog(XGAME::xULog_User_Spot_Recon, strLog);

			ID idKey = 
				DBA_SVR->SendGetUserLegionByidAccount( idSpot,
													pSpot->GetidOwner(),
													GetidConnect(),
													xCL2GS_LOBBY_SPOT_RECON );	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
			DBA_SVR->AddResponse( idKey,
									this,
									&XGameUser::RecvGetUserLegionByIdAcc );
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_JEWEL ) {
		XSpotJewel *pSpot = SafeCast<XSpotJewel*,XSpot*>( pBaseSpot );
		XVERIFY_BREAK( pSpot->GetidOwner() == m_spAcc->GetidAccount() );
		XPropWorld::xJEWEL *pProp = pSpot->GetpProp();
		XVERIFY_BREAK( pProp == NULL );
		XVERIFY_BREAK( pProp->idx == 0 );
		// 아직 매치가 안된상태에서 정찰
		if( pSpot->GetidOwner() == 0 ) {
			// 광산이 아직 매치가 안된상태
			// 일단 내광산으로 해놓고 ...
			pSpot->SetPlayerOwner( GetidAcc(), GetstrName() );
			// 매치 요청
			DoMatchJewel( pSpot );
		} else {
		// 적군이 점령한상태에서 정찰
			int idxMine = pSpot->GetpProp()->idx;
			if( idxMine ) {
				_tstring strLog = XE::Format(_T("Spot_Recon_Req_Enemy(XGAME::xSPOT_JEWEL) User_Level: %d, idxMine: %d"), GetLevel(), idxMine);
				AddLog(XGAME::xULog_User_Spot_Recon, strLog);

				ID idKey = 
					DBA_SVR->SendGetJewelLegionByidAcc( idxMine,
																				idSpot,
																				pSpot->GetidOwner(),
																				GetidConnect(),
																				xCL2GS_LOBBY_JEWEL_SPOT_RECON );	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
				DBA_SVR->AddResponse( idKey, this,
										&XGameUser::RecvGetJewelLegionByIdAcc );
			}
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_SULFUR 
		|| pBaseSpot->GettypeSpot() == XGAME::xSPOT_NPC ) {
//		XSpotSulfur *pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		if( pBaseSpot ) {
			// 정찰받는 시점에서 군단이 없으면 생성한다.
			if( pBaseSpot->GetspLegion() == nullptr )
				pBaseSpot->CreateLegion( m_spAcc );
			SendReconInfo( true, 0, pBaseSpot );
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_MANDRAKE ) {
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		XVERIFY_BREAK( pSpot->GetidOwner() == GetidAcc() );
		if( pSpot ) {
			int idx = pSpot->GetpProp()->idx;
			if( idx ) {
				if( pSpot->GetidOwner() == 0 ) {
					// 아직 주인이 없는상태(?상태). 매칭이 이뤄져야 함.
					// DB서버에 만드레이크 자원지 하나 매칭을 요구한다.
					DoMatchMandrake( pSpot );
				} else {
					ID idKey 
						= DBA_SVR						
						->SendReqGetMandrakeLegionByIdAcc( idx
																						, pSpot->GetsnSpot()
																						, pSpot->GetidOwner()
																						, (ID)xCL2GS_LOBBY_MANDRAKE_SPOT_RECON );
					DBA_SVR->AddResponse( idKey, this,
												&XGameUser::RecvGetMandrakeLegionByIdAcc );					
				}
			}
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_DAILY )	{
		auto pSpot = SafeCast<XSpotDaily*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		// NPC군대의 정보를 보내준다.
		if( pSpot->GetspLegion() == nullptr )
			pSpot->CreateLegion( m_spAcc );
		SendReconInfo( true, 0, pSpot );
	} else {
	//////////////////////////////////////////////////////////////////////////
		XBREAK( 1 );
	}
	_tstring strLog = XFORMAT( "Spot_Recon(%s) SpotID: %d, Spot_Level: %d", 
														XGAME::GetStrSpotEnum(pBaseSpot->GettypeSpot()),
														idSpot, 
														pBaseSpot->GetLevel() );
	AddLog( XGAME::xULog_User_Spot_Recon, strLog );

	return 1;
}

/**
 스팟 공격
*/
int XGameUser::RecvSpotAttack( XPacket& p )
{
	int ret = 1;
	ID idSpot;
	XE::VEC2 vFocus;
	p >> idSpot;
	p >> vFocus;
	bool bOk = true;
	m_spAcc->SetvFocusWorld( vFocus );
	XSpot* pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	const int ap = pBaseSpot->GetNeedAP( m_spAcc );
//	XVERIFY_BREAK( m_spAcc->GetAP() < ap );
	if( m_spAcc->GetAP() < ap ) {
		SendSyncAcc( xPS_AP );
		return 1;
	}
	m_spAcc->AddAP( -ap );
// 	// 전투직전 핸들러.
// 	pBaseSpot->OnBeforeBattle( m_spAcc );
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		XVERIFY_BREAK( pSpot->GetidOwner() == m_spAcc->GetidAccount() );
		XVERIFY_BREAK( pSpot->IsQuestion() );
		XBREAK(pSpot->GetidOwner() == 0);
		ID idKey = DBA_SVR->SendReqLoginLockForBattle(GetidAcc(),
																									pSpot->GetidOwner(),
																									pSpot->GetidSpot(),
																									pSpot->GetsnSpot(),
																									pSpot->GettypeSpot());
		DBA_SVR->AddResponse(idKey,
													this,
													&XGameUser::RecvLoginLockForBattle);
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_JEWEL ) {
		auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		XVERIFY_BREAK( pSpot->GetidOwner() == m_spAcc->GetidAccount() );
		// 해당자원지 소유자의 부대정보를 요청해서 클라이언트에 배틀정보를 넘긴다.
		// 요청한 결과는 XDBASvrConnection::RecvGetUserLegionByIdAcc 로 온다.
		int idxMine = pSpot->GetpProp()->idx;
		if( idxMine ) {
			XBREAK( pSpot->GetidOwner() == 0 );
			ID idKey = DBA_SVR->SendReqLoginLockForBattle( GetidAcc(),
														pSpot->GetidOwner(),
														pSpot->GetidSpot(),
														pSpot->GetsnSpot(),
														pSpot->GettypeSpot(),
														(DWORD)idxMine );
			DBA_SVR->AddResponse( idKey,
								this,
								&XGameUser::RecvLoginLockForBattle );
		}
	}
	else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_SULFUR )
	{
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		if( pSpot->GetspLegion() == nullptr )
			pSpot->CreateLegion( m_spAcc );
		SendBattleInfoWithidAcc( pSpot, 0 );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_MANDRAKE )
	{
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		XVERIFY_BREAK( pSpot->GetidOwner() == GetidAcc() );
		int idx = pSpot->GetpProp()->idx;
		if( idx )		{
			XBREAK( pSpot->GetidOwner() == 0 );
			ID idKey = DBA_SVR->SendReqLoginLockForBattle( GetidAcc(), 
																						pSpot->GetidOwner(),
																						pSpot->GetidSpot(),
																						pSpot->GetsnSpot(),
																						pSpot->GettypeSpot(),
																						(DWORD)idx );
			DBA_SVR->AddResponse( idKey,
														this,
														&XGameUser::RecvLoginLockForBattle );
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_NPC )
	{
		auto pSpot = SafeCast<XSpotNpc*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		// 공격받는 시점에서 군단이 없으면 그때 만든다.
		if( pBaseSpot->GetspLegion() == nullptr )
			pSpot->CreateLegion( m_spAcc );
		SendBattleInfoWithidAcc( pBaseSpot, 0 );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_DAILY )
	{
		auto pSpot = SafeCast<XSpotDaily*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
#if _DEV_LEVEL <= DLV_DEV_PERSONAL
		// 개발자용 버전에서는 어느요일을 클릭했는지까지 보낸다.
		char c0;
		p >> c0;		auto dow = (XE::xtDOW)c0;
		p >> c0 >> c0 >> c0;
		pSpot->_SetdowToday( m_spAcc, dow );		// 강제로 요일을 바꿈.
#endif
		// 공격받는 시점에서 군단이 없으면 그때 만든다.
		auto spLegion = pBaseSpot->GetspLegion();
		if( spLegion == nullptr || (spLegion && spLegion->GetNumSquadrons() == 0) )
			pSpot->CreateLegion( m_spAcc );
		XArchive arParam;
		auto err = pSpot->DoEnter();
		if( err == XSpotDaily::xERR_OK ) {
			// 스팟 입장에 성공하면 클라로 전투정보를 보내준다.
			arParam << (int)err;
			arParam << pSpot->GetnumEnter();
			arParam << pSpot->GetRemainEnter();
		} else {
			arParam << (int)err;
			arParam << pSpot->GetnumEnter();
			arParam << pSpot->GetRemainEnter();
		}
		SendBattleInfoWithidAccParam( pSpot, 0, &arParam );
	} else
	if( pBaseSpot->GettypeSpot() == xSPOT_PRIVATE_RAID ) {

	} else
	//////////////////////////////////////////////////////////////////////////
	if( pBaseSpot->IsCampaignType() ) {
		char c0;
		p >> c0;		const int idxStage = c0;
		p >> c0;		const int idxFloor = c0;
		p >> c0 >> c0;
		XVERIFY_BREAK( idxStage < 0 );
		if( DoSpotAttackCampaign( pBaseSpot, idxStage, idxFloor ) == 0 ) {
			m_spAcc->AddAP( ap );		// 진입 실패했으면 깎았던 ap를 다시 채움.
			ret = 0;
		}
	}
	//
	const _tstring strLog = XFORMAT( "Spot_Atk_Legion(%s) SpotID: %d, SpotName: %s, SpotLevel: %d",
																	XGAME::GetStrSpotEnum( pBaseSpot->GettypeSpot() ),
																	idSpot, pBaseSpot->GetszName(), pBaseSpot->GetLevel() );
	AddLog( XGAME::xULog_User_Spot_Recon, strLog );
	return ret;
}

/**
 @brief 로그인락 요청의 결과가 돌아옴. 전투시작
*/
void XGameUser::RecvLoginLockForBattle( XPacket& p )
{
	ID idSpot, snSpot;
	ID idAttacker;
	ID idDefender;
	DWORD typeSpot;
	DWORD secStart, secTotal;
	DWORD param1, param2;
	XArchive arPack;

	p >> idAttacker >> idDefender >> secStart;
	bool bOnline = ( secStart == 0 );
	p >> arPack;
	arPack >> idSpot >> snSpot >> typeSpot >> secTotal >> param1 >> param2;
	//
	do	{
		auto pUser = XGAME_USERMNG->GetspUser( idAttacker );
		if( pUser == nullptr ) {
			// 다시 로그인락을 풀어준다.
			DBA_SVR->SendUnlockLoginForBattle( idDefender, idAttacker );
			break;
		}
		if( bOnline ) {
			// 온라인이면 클라로 에러 패킷 보냄
			XPacket ar( (ID)xCL2GS_IS_ONLINE );
			ar << idSpot;
			ar << idDefender;
			Send( ar );
		}
		else {
			switch( typeSpot )
			{
			//////////////////////////////////////////////////////////////////////////
			case XGAME::xSPOT_CASTLE: {
				// 온라인이 아니면 전투 정보 요청하는 프로세스로 전환
				// 해당자원지 소유자의 부대정보를 요청해서 클라이언트에 배틀정보를 넘긴다.
				// 요청한 결과는 XDBASvrConnection::RecvGetUserLegionByIdAcc 로 온다.
				ID idKey =
					DBA_SVR->SendGetUserLegionByidAccount( idSpot,
														idDefender,
														GetidConnect(),
														xCL2GS_LOBBY_SPOT_ATTACK );
				DBA_SVR->AddResponse( idKey, this,
									&XGameUser::RecvGetUserLegionByIdAcc );
			} break;
			/////////////////////////////////////////////////////////////////////////
			case XGAME::xSPOT_JEWEL: {
				// arParam.비어있는건 아닌지 검사
				int idxMine = (int)param1;
				ID idKey =
					DBA_SVR->SendGetJewelLegionByidAcc( idxMine,
													idSpot,
													idDefender,
													GetidConnect(),
													xCL2GS_LOBBY_JEWEL_BATTLE_INFO );
				DBA_SVR->AddResponse( idKey, this,
									&XGameUser::RecvGetJewelLegionByIdAcc );
			} break;
				//////////////////////////////////////////////////////////////////////////
			case XGAME::xSPOT_MANDRAKE: {
				int idx = (int)param1;
// 				XPacket ar( (ID)xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC );
// 				ar << idx;
// 				ar << snSpot;	// 오류검사용
// 				ar << idDefender;
// 				ar << (ID)xCL2GS_LOBBY_MANDRAKE_BATTLE_INFO;
//				DBA_SVR->Send( ar );
				ID idKey 
					= DBA_SVR						
					->SendReqGetMandrakeLegionByIdAcc( idx
																					, snSpot
																					, idDefender
																					, (ID)xCL2GS_LOBBY_MANDRAKE_BATTLE_INFO );
				DBA_SVR->AddResponse( idKey, this,
												&XGameUser::RecvGetMandrakeLegionByIdAcc );
			} break;
			default:
				XBREAK( 1 );
				break;
			}
			_tstring strLog = XE::Format( _T( "Spot_Atk_Req(%s) SpotID: %d" ),
				XGAME::GetStrSpotEnum( ( XGAME::xtSpot )typeSpot ),
				idSpot );
			AddLog( XGAME::xULog_User_Spot_Combat, strLog );
		}
	} while( 0 );
}

/**
 스팟이 지도상에서 사라지기 전에 발생한다.
 현재는 요일/스페셜 스팟이 사라질때만 발생함.
*/
void XGameUser::DelegateReleaseSpotBefore( XSpot *pSpot )
{
	if( pSpot->GettypeSpot() == XGAME::xSPOT_DAILY )
	{
	} 
//	else
// 	if( pSpot->GettypeSpot() == XGAME::xSPOT_SPECIAL )
// 	{
// 		// 다음 스폰을 위한 타이머를 작동
// // 		int y, m, d;
// // 		XSYSTEM::GetYearMonthDay( &y, &m, &d );
// 		m_spAcc->SetSpecialSpotTimer();
// 	}
}

/**
 스팟이 지도상에서 사라진 후에 발생한다.
 현재는 요일/스페셜 스팟이 사라질때만 발생함.
*/
void XGameUser::DelegateReleaseSpotAfter( XSpot *pSpot )
{
	if( pSpot->GettypeSpot() == XGAME::xSPOT_DAILY )
	{
		SendSpotSync( pSpot );
	}
// 	else
// 	if( pSpot->GettypeSpot() == XGAME::xSPOT_SPECIAL )
// 	{
// 		SendSpotSync( pSpot );
// 	}
}

/**
 보석광산 전투전 DB로부터 군단정보를 받아 유저에게 넘긴다.
*/
void XGameUser::OnRecvJewelBattleInfo( BOOL isFound, XArchive& arPacket )
{

}

/**
 광산에 주둔중인 부대의 정보등을 받음.
*/
void XGameUser::RecvGetJewelLegionByIdAcc( XPacket& p )
{
	ID idPacket;
	int isFound;
	int idxMine;
	XGAME::xJewelMatchEnemy infoMatch;
	//
	p >> isFound;
	p >> idPacket;
	p >> idxMine;
	if( isFound ) {
		XArchive arLegion;
		XArchive arAbil;
		int ver;
		{
			_tstring strName;
			ID idAcc;
			int levelMine, defense, level, lootJewel, power, score;
			p >> ver >> idAcc >> level >> strName >> levelMine >> defense;
			p >> power >> score >> arLegion >> arAbil >> lootJewel/* >> idxMine*/;
			infoMatch.m_idAcc = idAcc;
			infoMatch.m_Defense = defense;
			infoMatch.m_Ladder = score;
			infoMatch.m_lvAcc = level;
			infoMatch.m_LvMine = levelMine;
			infoMatch.m_Power = power;
			infoMatch.m_strName = strName;
			infoMatch.m_lootJewel = lootJewel;
		}
		if( idPacket == xCL2GS_LOBBY_JEWEL_BATTLE_INFO ||
			idPacket == xCL2GS_LOBBY_JEWEL_SPOT_RECON ) {
			// 적부대정보를 보관해둔다.
			XSpotJewel* pSpot = GetpWorld()->GetSpotJewelByIdx( idxMine );
			if( XBREAK( pSpot == NULL ) )
				return;
			if( ver ) {
				XLegion *pLegion = XLegion::sCreateDeserializeFull( arLegion );
				pSpot->SetpLegion( pLegion );
			} else {
				XBREAK( pSpot->GetspLegion() != NULL );
				pSpot->SetpLegion( NULL );
			}
			pSpot->SetMatch( infoMatch );
//			pSpot->SetlevelMine( levelMine );
// 			if( defense < 0 )
// 				defense = 0;
// 			pSpot->SetDefense( defense );
// 			pSpot->SetlootJewel( lootJewel );
// 			pSpot->SetPower( power );
			XBREAK( pSpot->GetidOwner() != infoMatch.m_idAcc );
			ID snSession = 0;
			float rateHp = 1.f;
			if( idPacket == xCL2GS_LOBBY_JEWEL_BATTLE_INFO ) {
				snSession = m_spAcc->SetBattleSession( 0,      // auto generate
															pSpot->GetspLegion(),
															infoMatch.m_idAcc,
															pSpot->GetidSpot() );
				rateHp = 1.f + (infoMatch.m_Defense * 0.1f);
			}
			_tstring strLog = XE::Format(_T("Spot Recon Result Enemy(XGAME::xSPOT_JEWEL) User_Level: %d, idxMine: %d, Spot_Level: %d, Mine_Level: %d, Name: %s")
																			, GetLevel(), idxMine, infoMatch.m_lvAcc, infoMatch.m_LvMine, infoMatch.m_strName.c_str() );
			AddLog(XGAME::xULog_User_Spot_Recon, strLog);
			// 클라에 패킷은 이미 앞에서 다 보냄
			// 클라에 보냄
			{
				XPacket ar( idPacket );
				ar << isFound << idxMine;
				ar << arLegion << arAbil;
				ar << infoMatch;
// 				ar << idAcc << level << strName << levelMine << defense;
// 				ar << power << score << arLegion << arAbil << lootJewel/* << idxMine*/;
				ar << rateHp << snSession;
				Send( ar );
			}
		} else {
			XBREAK(1);
		}
	} else {
		// not found
		auto pSpot = GetpWorld()->GetSpotJewelByIdx( idxMine );
		if( XBREAK( pSpot == NULL ) )
			return;
		pSpot->ClearSpot();
		pSpot->ClearMatchEnemy();
		pSpot->OnSpawn( m_spAcc );
		pSpot->SetPlayerOwner( GetidAcc(), GetstrName() );
		pSpot->ClearLevelMine();
//		pSpot->SetlevelMine( 1 );
//		pSpot->ResetDefense( GetLevel() );
		{
			XPacket ar( idPacket );
			ar << isFound << idxMine;
			Send( ar );
			SendSpotSync( pSpot );
		}
	}
}

/**
 @brief 징표 드랍을 위해 주사위를 굴린다.
*/
ItemBox XGameUser::DoDropScalp( void )
{
	ID idItem = 0;
	int dice = random(100);
	if( dice < 20 )
		idItem = 10000;		// crow
	else if( dice < 40 )
		idItem = 10001;		// ironlord
	else if( dice < 60 )
		idItem = 10002;		// freedom
	else if( dice < 80 )
		idItem = 10003;		// ancient
	else
		idItem = 10004;		// flame
	if( XBREAK( idItem == 0 ) )
	{
		auto nullItem = std::make_pair( nullptr, 0 );
		return nullItem;
	}
//	XBaseItem *pItem = XAccount::sCreateItem( idItem, 1 );
	XPropItem::xPROP *pProp = PROP_ITEM->GetpProp( idItem );
	XBREAK( pProp == nullptr );
	return std::make_pair(pProp, 1);
}

/**
 @brief 부대업글 전리품
*/
ItemBox XGameUser::DoDropMedal( void )
{
	ID idHead[3];
	idHead[ 0 ] = XGAME::GetSquadLvupItem( XGAME::xAT_TANKER );
	idHead[ 1 ] = XGAME::GetSquadLvupItem( XGAME::xAT_RANGE );
	idHead[ 2 ] = XGAME::GetSquadLvupItem( XGAME::xAT_SPEED );
	XArrayLinearN<float,4> aryChance;
	XArrayLinearN<XArrayLinearN<float,4>,5> aryTable;
	aryChance.Add( 0.99f );
	aryChance.Add( 0.01f );
	aryChance.Add( 0.f );
	aryChance.Add( 0.f );
	aryTable.Add(aryChance);	// 0
	aryChance.Clear();
	aryChance.Add( 0.88f );
	aryChance.Add( 0.11f );
	aryChance.Add( 0.01f );
	aryChance.Add( 0.f );
	aryTable.Add( aryChance );	// 1
	aryChance.Clear();
	aryChance.Add( 0.72f );
	aryChance.Add( 0.21f );
	aryChance.Add( 0.06f );
	aryChance.Add( 0.01f );
	aryTable.Add( aryChance );	// 2
	aryChance.Clear();
	aryChance.Add( 0.56f );
	aryChance.Add( 0.31f );
	aryChance.Add( 0.11f );
	aryChance.Add( 0.02f );
	aryTable.Add( aryChance );	// 3
	aryChance.Clear();
	aryChance.Add( 0.4f );
	aryChance.Add( 0.41f );
	aryChance.Add( 0.16f );
	aryChance.Add( 0.03f );
	aryTable.Add( aryChance );	// 4

	ID idItem = idHead[ random(3) ];
	int idxTable = 0;
	if( m_spAcc->GetLevel() < 5 )
		idxTable = 0;
	else if( m_spAcc->GetLevel() < 10 )
		idxTable = 1;
	else if( m_spAcc->GetLevel() < 15 )
		idxTable = 2;
	else if( m_spAcc->GetLevel() < 20 )
		idxTable = 3;
	else 
		idxTable = 4;
	int idxSel = XE::GetDiceChance( aryTable[ idxTable ] );
	if( XBREAK( idxSel < 0 ) )
		idxSel = 0;
	XBREAK( idxSel >= aryChance.size() );
	idItem += idxSel;
	auto pProp = PROP_ITEM->GetpProp( idItem );
	XBREAK( pProp == nullptr );
	return std::make_pair( pProp, 1 ) ;
}

/**
 @brief 스킬 두루마리
*/
ItemBox XGameUser::DoDropScroll( void )
{
	ID idHead = 10010;	// scroll01
	XArrayLinearN<float, 4> aryChance;
	XArrayLinearN<XArrayLinearN<float, 4>, 5> aryTable;
	aryChance.Add( 0.99f );
	aryChance.Add( 0.01f );
	aryChance.Add( 0.f );
	aryChance.Add( 0.f );
	aryTable.Add( aryChance );	// 0
	aryChance.Clear();
	aryChance.Add( 0.88f );
	aryChance.Add( 0.11f );
	aryChance.Add( 0.01f );
	aryChance.Add( 0.f );
	aryTable.Add( aryChance );	// 1
	aryChance.Clear();
	aryChance.Add( 0.72f );
	aryChance.Add( 0.21f );
	aryChance.Add( 0.06f );
	aryChance.Add( 0.01f );
	aryTable.Add( aryChance );	// 2
	aryChance.Clear();
	aryChance.Add( 0.56f );
	aryChance.Add( 0.31f );
	aryChance.Add( 0.11f );
	aryChance.Add( 0.02f );
	aryTable.Add( aryChance );	// 3
	aryChance.Clear();
	aryChance.Add( 0.4f );
	aryChance.Add( 0.41f );
	aryChance.Add( 0.16f );
	aryChance.Add( 0.03f );
	aryTable.Add( aryChance );	// 4

	ID idItem = idHead;
	int idxTable = 0;
	if( m_spAcc->GetLevel() < 5 )
		idxTable = 0;
	else if( m_spAcc->GetLevel() < 10 )
		idxTable = 1;
	else if( m_spAcc->GetLevel() < 15 )
		idxTable = 2;
	else if( m_spAcc->GetLevel() < 20 )
		idxTable = 3;
	else
		idxTable = 4;
	int idxSel = XE::GetDiceChance( aryTable[ idxTable ] );
	if( XBREAK(idxSel < 0) )
		idxSel = 0;
	XBREAK( idxSel >= aryChance.size() );
	idItem += idxSel;
	auto pProp = PROP_ITEM->GetpProp( idItem );
	XBREAK( pProp == nullptr );
	return std::make_pair( pProp, 1 );

}

/**
 @brief 전투후 드롭할 아이템을 생성시킨다.
*/
void XGameUser::DoDropProcess( const XSpot *pSpot, 
								XArrayLinearN<XBaseItem*,256> *pOutUpdatedAry,
								XArrayLinearN<ItemBox, 256>* pOutDrops )
{
	XBREAK( pSpot == nullptr );
	XBREAK( pSpot->IsInvalidSpot() );		// 스팟이 어떤 구름영역에도 속해있지 않다.
	float multiplyDropNum = 1.f;
	if( m_spAcc->GetBuff(_T("item_drop_up")) )
		multiplyDropNum *= 2.f;
	pSpot->DoDropItem( m_spAcc, pOutDrops, pSpot->GetLevel(), multiplyDropNum );
	// 퀘스트로인해 드랍되는 아이템이 있는지 검사.
	{
		xQuest::XEventInfo infoQuest;
		infoQuest.SetidArea( pSpot->GetidArea() );
		infoQuest.SetidSpot( pSpot->GetidSpot() );
		// 회색스팟은 해당사항 없도록 한다.
		if( m_spAcc->IsGreenOver( pSpot->GetPower() ) ) {
			// spot_type형태의 퀘만 회색스팟은 인정안되게 함. 다른형태의 퀘는 회색스팟이어도 클리어 됨.
			infoQuest.SettypeSpot( pSpot->GettypeSpot() );
		}
		GetpQuestMng()->DoDropFromQuest( *pOutDrops, infoQuest );
	}
	// 드랍할 품목을 인벤에 추가
	XARRAYLINEARN_LOOP_AUTO( *pOutDrops, &itemBox ) {
		auto pPropItem = std::get<0>( itemBox );
		auto num = std::get<1>( itemBox );
		m_spAcc->CreateItemToInven( pPropItem, num, pOutUpdatedAry );
		// 퀘스트시스템에 드랍한 물건의 정보를 보냄
		{
			xQuest::XEventInfo infoQuest;		// 이벤트 정보.
			infoQuest.SetidItem( pPropItem->idProp );
			infoQuest.SetAmount( num );
			infoQuest.SetidArea( pSpot->GetidArea() );
			infoQuest.SetidSpot( pSpot->GetidSpot() );
			infoQuest.SettypeSpot( pSpot->GettypeSpot() );
			infoQuest.SetidCode( pSpot->GetpBaseProp()->idCode );
			DispatchQuestEvent( XGAME::xQC_EVENT_GET_ITEM, infoQuest );
		}
	} END_LOOP;
	///< 
}


bool XGameUser::IsVerificationBattle( /*전투데이타*/) 
{
	return true;
}
/**
 @brief 클라에서 전투종료후 올라온다.
*/
int XGameUser::RecvReqFinishBattle2( XPacket& p )
{
//	bool bSend = true;
	XGAME::xBattleResult result;
	XGAME::xBattleFinish battle;
	XBREAK( m_spAcc->GetPowerExcludeEmpty() == 0 );
	int powerAccBeforeExcludeEmpty = m_spAcc->GetPowerExcludeEmpty();
	p >> battle;
	result.idSpot = battle.idSpot;
	result.idxStage = battle.idxStage;
	//
	XSpot* pBaseSpot = GetpWorld()->GetSpot( result.idSpot );
	//////////////////////////////////////////////////////////////////////////
	// 유효성 검증
	XVERIFY_BREAK( pBaseSpot == nullptr );
	XVERIFY_BREAK( pBaseSpot->GetbitFlag().noAttack );
	result.typeSpot = pBaseSpot->GettypeSpot();
	XVERIFY_BREAK( result.typeSpot == XGAME::xSPOT_VISIT );
	// 전투전 배치씬에서 포기하고 나감.
	if( battle.ebCode == XGAME::xEB_CANCEL ) {
		ID idEnemy = pBaseSpot->GetidOwner();
		XBREAK( m_spAcc->GetidAccount() == idEnemy );
		ProcCancelBattle( pBaseSpot, idEnemy );
		//
		XPacket ar( (ID)xCL2GS_INGAME_CANCEL_BATTLE );
		Send( ar );
		return 1;
	}
	XVERIFY_BREAK( battle.bitWinner == XGAME::xSIDE_NONE );
	result.SetWin( battle.bitWinner == XGAME::xSIDE_PLAYER );
	// 유황스팟이 아닌데 이게 true로 되어있으면 뭔가 잘못된거임
	XVERIFY_BREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_SULFUR && battle.bRunAwaySulfur );
	XVERIFY_BREAK( battle.bRunAwaySulfur && battle.ebCode != XGAME::xEB_FINISH );	// 유황도주는 승리로간주해야하기때문에 FINISH가 와야함.
	if( battle.bCheatKill ) {
		XVERIFY_BREAK( result.IsLose() );		// 소탕인데 이게 올라올수가 없음.
		const auto errCodeKill = m_spAcc->IsAbleKill( pBaseSpot );
		XVERIFY_BREAK( errCodeKill != xE_OK  );
		XVERIFY_BREAK( battle.ebCode != xEB_FINISH );
		if( battle.ebCode == xEB_FINISH ) {
			// 소탕의 경우 RecvSpotTouch를 거치지 않기때문에 소탕을 누르는 순간 ap검사를 해서 모자란상황이면
			// 조용히 다시 동기화 시켜야한다.
			const int ap = pBaseSpot->GetNeedAP( m_spAcc );
			if( m_spAcc->GetAP() < ap ) {
				m_spAcc->ClearBattleSession();
				SendCancelKill();
				return 1;
			}
		}
	}
	// 전투세션아이디가 다르면 
//#ifdef _CHEAT
	if( !battle.bCheatKill )
//#endif // _CHEAT
	{
		// 모든 전투는 전투세션이 있어야 한다. 상대하던 부대의 정보원본을 갖고 있어야 하기때문.
		XVERIFY_BREAK( battle.snSession == 0 );
		XVERIFY_BREAK( m_spAcc->GetsnSession() == 0 );
		XVERIFY_BREAK( battle.snSession != m_spAcc->GetsnSession() );
		auto& session = m_spAcc->GetBattleSession();
		if( session.idAccEnemy ) {
			// pvp에서 3분이 지난 전투는 인정안함.
			xSec secPassBattle = m_spAcc->GetsecPassBattle();
			XVERIFY_BREAK( m_spAcc->IsTimeOverBattle( XGC->m_secMaxBattleSession ) );
		}
	}
	int lvEnemy = pBaseSpot->GetLevel();
	XBREAK( lvEnemy == 0 );
	//////////////////////////////////////////////////////////////////////////
	{
//#ifdef _CHEAT
		// 스팟소탕했는데 군단이 없으면 급 만듬.
		if( battle.bCheatKill && pBaseSpot->GetspLegion() == nullptr ) {
			// 어차피 소탕이면 무조건 이기기때문에 상대의 군단정보가 필요없을듯 하다.
			// 가라로 군단 만들어도 상관없을듯. 만약 상대를 정찰해놓은 상대라면 군단이 있으므로 괜찮음.
			pBaseSpot->CreateLegion( m_spAcc );
		}
// 		// 정상적인 경우라면 이런경우는 생기지 않아야 함.
// 		if( XBREAK(battle.bCheatKill && pBaseSpot->GetspLegion() == nullptr) ) {
// 			m_spAcc->ClearBattleSession();
// 			SendCancelKill();
// 			return 1;
// 		}
//#endif // _CHEAT
		// 후퇴
		if( battle.ebCode == XGAME::xEB_RETREAT ) {
			// 그냥 패배로 간주하면 됨
			result.SetWin( false );
		} else
		if( battle.ebCode == XGAME::xEB_FINISH ) {     // 도망간경우는 안됨.
			// 플레이데이타를 서버에서 시뮬레이션해서 검증한다.
			// 플레이어의 승패와 관계없이 검증체크한다. 패배하더라도 얻는 포인트등이 있을수 있기때문.
			// 패배처리후 XVERIFY_BREAK처리
			if( XBREAK( IsVerificationBattle() == false ) ) {}
			// 계정경험치 증가(져도 경험치 먹음, 후퇴시엔 못먹음.)
			m_spAcc->AddExp( XGC->m_expBattle );
			// 성이 아닌경우는 도망간경우는 연패로 인정안함.
			if( pBaseSpot->GettypeSpot() != xSPOT_CASTLE && result.IsLose() )
				pBaseSpot->SetnumLose( pBaseSpot->GetnumLose() + 1 );   // 패배횟수를 올림
		}
		if( battle.bRunAwaySulfur ) {
			// 유황스팟에서 유료로 도주함.
			// 캐쉬 깜.
			int cost = m_spAcc->GetCostCashSkill( XGAME::xCS_SULFUR_RETREAT );
			XVERIFY_BREAK( m_spAcc->IsNotEnoughCash( cost ) );
			m_spAcc->AddCashtem( -cost );
		}
//		if( result.IsWin() && (!pBaseSpot->IsEventSpot() || pBaseSpot->IsCampaignType()) ) {
		if( result.IsWin() ) {	// 어떤 전투든 이기면 별점은 나와야 함.
			// 이긴게임에 한하여 별점 산출	// 15초이내 클리어 3별// 30초이내 클리어 2별// 그 외 1별
			if( battle.bCheatKill ) {
				result.numStar = 3;
			} else {
				if( battle.secPlay <= 30 && battle.m_rateHpAlly >= 0.5f )
					result.numStar = 3;
				else if( battle.secPlay <= 45 && battle.m_rateHpAlly >= 0.3f )
					result.numStar = 2;
				else
					result.numStar = 1;
			}
			// 별퀘를 갖고 있어야만 별을 모을수 있음.
//			if( m_spAcc->GetpQuestMng()->IsHaveGetStarQuest() ) {
			if( m_spAcc->IsAbleGetStar() ) {
				DoCollectingSpotStar( result.numStar, pBaseSpot );
			}
			// 80%이상 hp남음 3별			// 50%이상 2별			// 그 외 1별			// hp는 나중에 구현
		}
		// 성의 경우는 도망쳐도 연패로 인정
		if( pBaseSpot->GettypeSpot() == xSPOT_CASTLE )
			if( result.IsLose() )
				pBaseSpot->SetnumLose( pBaseSpot->GetnumLose() + 1 );   // 패배횟수를 올림
	}
	// 퀘스트이벤트에서 써야하므로 군단포인터를 받아둔다. 참조포인터이므로 밑에서 군단데이터를 삭제해도 지워지지 않는다.
	LegionPtr spLegion = pBaseSpot->GetspLegion();
	// 소탕치트 했는데 군단이 없으면 급 만듬.
	if( spLegion == nullptr ) {
//#ifdef _CHEAT
// 		if( battle.bCheatKill ) {
// 			pBaseSpot->CreateLegion( m_spAcc );
// 			spLegion = pBaseSpot->GetspLegion();
// 		}
//#endif // _CHEAT
		// npc류 스팟에서는 이런게 나와선 안된다. 현재 유저중엔 군단이없는 유저가 있는듯.
		XVERIFY_BREAK( spLegion == nullptr );	// 현재 유저데이타에서도 군단없는 유저가 있나?
	}
	int scorePrev = m_spAcc->GetLadder();
	// 스팟종류에 따라 처리를 달리 해준다.
	if( pBaseSpot->IsVsPC() && !battle.bCheatKill ) {
		result.bVsPVP = true;
		m_spAcc->SetnumBattleByPvp( m_spAcc->GetnumBattleByPvp() + 1 );	// pvp전 횟수
		if( result.IsWin() )
			m_spAcc->SetnumWinsByPvp( m_spAcc->GetnumWinsByPvp() + 1 );	// 그중 이긴횟수
	}
	result.logForAttacker.strName = pBaseSpot->GetszName();
	result.logForAttacker.m_lvAcc = pBaseSpot->GetLevel();
	result.logForAttacker.powerBattle = pBaseSpot->GetPower();
	const int powerEnemy = pBaseSpot->GetPower();	// 스팟의 군단이 없어지기전에 미리 받아둠.
	result.bClearSpot = result.IsWin();	// 대부분의 경우는 승리는 스팟클리어임
	//
	if( result.IsLose() ) {
		// 졌을때 패널티
		result.logForAttacker.addScore = -xRandom( xLoseMin, xLoseMax );
		m_spAcc->AddScore( result.logForAttacker.addScore );
		XASSERT( result.aryUpdated.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
		XASSERT( result.aryDrops.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
		XASSERT( result.aryHeroes.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
	}
	// 일반적인 보상처리
	// 지더라도 보상프로세스엔 들어가게 바뀜
	if( pBaseSpot->GettypeSpot() != XGAME::xSPOT_SULFUR ) {
		// 유황이외 스팟의 공통 처리부
		DoRewardProcess( pBaseSpot, &result );
	}
	result.logForAttacker.score = pBaseSpot->GetScore();
	//////////////////////////////////////////////////////////////////////////
	// 스팟별 처리
	int bVerify = 
	ProcSpotEach( pBaseSpot, &result, battle );
	XVERIFY_BREAK( bVerify == false );
	XBREAK( pBaseSpot->IsVsPC() && result.GetidEnemy() == 0 );	// pvp전인데 idEnemy가 세팅안되어있으면 에러
	bool bClearSpot = result.bClearSpot;	// DispatchQuestEventByBattle는 bClearSpot을 필요로 하는데 이 플래그는 ProcSpotEach를 거쳐야만 유효한 값이 되기때문에 실수방지를 위해 이렇게 함.
	//////////////////////////////////////////////////////////////////////////
	result.logForAttacker.bWin = result.IsWin();
	if( result.IsWin() )	{
		// 퀘스트 이벤트 처리(인카운터가 발생했더라도 처리한다. 어쨋든 NPC은 깬거니까.)
		DispatchQuestEventByBattle( pBaseSpot, result.IsWin(), bClearSpot, spLegion );
	}
	// 전투가 끝난 후 스팟별로 해야할일을 처리
	const auto idEnemy = result.GetidEnemy();
	const auto bWin = result.IsWin();
	const auto bRetreat = battle.IsRetreat();
	pBaseSpot->OnAfterBattle( m_spAcc
													, idEnemy
													, bWin
													, result.numStar
													, bRetreat );
	bool bSend = true;
	if( pBaseSpot->IsSulfur() ) {
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		// 유저 난입이 일어났으면 지금 정보 전송안함.
		if( result.bEncounter )
			bSend = false;
	}
	m_spAcc->UpdatePower();	// 져도 계정경험치는 오르므로 렙업이나 기타 다른이유로 전투력이 변했을수 있으니 한번 갱신함.
	// 유황 인카운터가 일어날수도 있으므로 send밖에서 다 채움.
	result.aryAccRes = m_spAcc->GetaryResource();
	XSpot::sSerialize( result.arSpot, pBaseSpot );
	result.ladder = m_spAcc->GetLadder();
	result.m_totalStar = m_spAcc->GetnumStar();
	result.m_powerAccIncludeEmpty = m_spAcc->GetPowerIncludeEmpty();
	result.m_powerAccExcludeEmpty = m_spAcc->GetPowerExcludeEmpty();
	result.m_numCashtem = m_spAcc->GetCashtem();
	result.logForAttacker.m_bKill = battle.bCheatKill;
	//
	if( bSend ) {
		if( battle.bCheatKill ) {
			const int ap = pBaseSpot->GetNeedAP( m_spAcc );
			XVERIFY_BREAK( m_spAcc->GetAP() < ap );
			m_spAcc->AddAP( -ap );
			m_spAcc->DestroyItem( _T("item_kill"), 1 );		// 소탕권 하나 소모
		}
		// 업데이트된 스팟정보를 갱신함.
		//
		XPacket ar( (ID)xCL2GS_INGAME_REQ_FINISH_BATTLE );
		ar << result;
		Send( ar );
		SendLevelSync();
		///< pvp대전이었으면 pvp대전상대에게 방어로그를 전달한다.  
		if( result.GetidEnemy() ) {
			result.logForAttacker.sec = XTimer2::sGetTime();
			// this플레이어에게 배틀로그 갱신
			m_spAcc->AddBattleLog( true, result.logForAttacker );
			SendAddBattleLog( true, result.logForAttacker );
			//
			if( !battle.bCheatKill ) {		// 소탕으로 이겼으면 상대에게 로그 안보냄.
				XGAME::xBattleLog logForDefender;		// 공격자에 대한 로그
				logForDefender.sec = XTimer2::sGetTime();
				logForDefender.idEnemy = GetidAcc();
				logForDefender.m_lvAcc = lvEnemy;
				logForDefender.addScore = -result.logForAttacker.addScore;
				logForDefender.aryLoot = result.logForAttacker.aryLoot;
				logForDefender.bRevenge = result.logForAttacker.bRevenge;
				logForDefender.bWin = !result.logForAttacker.bWin;
				logForDefender.m_bKill = battle.bCheatKill;
				// AddExp()과정에서 렙업으로 인해 전투력이 바뀔수 있으므로 바뀌기전 값을 보냄.
				auto powerIncludeEmpty = m_spAcc->GetPowerIncludeEmpty();
				// 승리시: 방어자입장에선 털린것이므로 공격자의 파워를 높게 쳐줘서 덜(?)기분상하도록 한다.
				// 패배시: 빈슬롯 뺀값으로 보냄. 방어자입장에서 막은것이므로 높은값을 보내줘서 기분좋게(?)해줌.
				if( result.logForAttacker.bWin )
					logForDefender.powerBattle = powerIncludeEmpty;
				//		logForDefender.powerBattle = powerAccBefore;	// 빈슬롯포함값
				logForDefender.score = scorePrev;
				logForDefender.strName = m_spAcc->GetstrName();
#pragma message("성 스팟의 경우 방어자가 승리했어도 자원손실에 관한 정보를 보내야한다. 방어로그도 실제 손실된 양으로 표시해야 한다.-----------------------------------")
				if( result.typeSpot != XGAME::xSPOT_SULFUR )		// encounter때는 보내지 않음.
					DBA_SVR->SendDefenseInfo( result.GetidEnemy(), logForDefender );
				// 공격자가 승리했으면 방어자에게 패배정보 보냄.
				if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE
					|| pBaseSpot->GettypeSpot() == XGAME::xSPOT_JEWEL
					|| pBaseSpot->GettypeSpot() == XGAME::xSPOT_MANDRAKE ) {
					ProcAfterBattleUserOwner( pBaseSpot, result.GetidEnemy(), powerEnemy, result.IsWin(), logForDefender );
				}
			}
		}
	}  else { // bSend
		if( result.bEncounter ) {
			auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
			if( pSpot ) {
				// 모든 결과를 result에 다 담고 메칭을 한다. 만약 매칭이 안되었을땐 result를 그대로 보내준다.
				DoMatchSulfur( pSpot, result );
			}
		}
	}
	// 삭제되어야 할 스팟이 있었으면 삭제시킴. bDestroy인채로 클라에 내려가면 클라의 스팟도 자동 삭제된다.
//	if( pBaseSpot->IsDestroy() )
//		GetpWorld()->DestroySpot( pBaseSpot->GetidSpot() );
	m_spAcc->ClearBattleSession();

#ifdef _DEV_SERVER
//	Save();		// 개발버전에선 전투끝나면 세이브 한번씩 하도록 함.
#endif // _DEV_SERVER

	return 1;
} // RecvReqFinishBattle2

/**
 @brief 전투후 스팟에 대한 처리를 한다.
 유저가 상대인 스팟만 해당한다.
*/
void XGameUser::ProcAfterBattleUserOwner( XSpot *pBaseSpot, 
										ID idEnemy, 
										int powerEnemy,
										bool bWin,
										XGAME::xBattleLog& logForDefender )
{
	XBREAK( pBaseSpot == nullptr );
	XBREAK( idEnemy == 0 );
	XBREAK( logForDefender.m_bKill );
	switch( pBaseSpot->GettypeSpot() )
	{
	case XGAME::xSPOT_CASTLE: {           
	}	break;
	case XGAME::xSPOT_JEWEL: {
		auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
		if( bWin ) {
			ID idDBAcc = pSpot->GetidMatchEnemy();
			XGAME::xJewelInfo info;
			info.m_idxJewel = pSpot->GetidxJewel();
			info.m_idOwner = GetidAcc();
			info.m_idMatchEnemy = GetidAcc();
			info.m_strName = GetstrName();
			info.m_LevelMine = pSpot->GetlevelMine();
			info.m_Defense = pSpot->GetDefense();
			DBA_SVR->SendChangeJewelOwn( idDBAcc, info );
		} else {
			DBA_SVR->SendAddDefenseByJewel( pSpot );
		}
	}	break;
	case XGAME::xSPOT_MANDRAKE: {
	}	break;
	default:
		break;
	}
	// 스팟 공통
	if( bWin ) {
		// 노랑이상에게 이긴것만 보낸다.공평하지 못하므로.
		int powerGrade 
			= XGAME::GetHardLevel( powerEnemy, m_spAcc->GetPowerExcludeEmpty() );
		if( powerGrade >= 0 ) {
			// idEnemy의 DB에 패배결과를 보낸다.
			ID idKey 
				= DBA_SVR->SendReqDefeatInfo( idEnemy, GetidAcc(), 
											pBaseSpot->GettypeSpot(), powerGrade );
			DBA_SVR->AddResponse( idKey,
								this,
								&XGameUser::RecvDefeatInfo );
		}
	}
	// 방어자의 로그인락을 풀어준다.
	DBA_SVR->SendUnlockLoginForBattle( idEnemy, GetidAcc() );
#pragma message("바로 로그인락을 풀면 안되고 전투결과를 상대에게 보내고 DB에 완전히 반영될때까지 잡아둬야 함.")
}

/**
 @brief 전투를 빠져나오며 해야할 필수동작들.
*/
int XGameUser::ProcCancelBattle( XSpot *pBaseSpot, ID idEnemy )
{
	// 방어자의 로그인락을 풀어준다.
	if( idEnemy )
		DBA_SVR->SendUnlockLoginForBattle( idEnemy, GetidAcc() );
	m_spAcc->ClearBattleSession();
	//////////////////////////////////////////////////////////////////////////
	// 길드레이드 처리
	if( pBaseSpot->IsGuildRaid() ) {
		auto pSpot = static_cast<XSpotCommon*>( pBaseSpot );
		auto spCampObj = pSpot->GetspCampObj( GetidGuild() );
		XVERIFY_BREAK( spCampObj == nullptr );      // 캠페인 스테이지 눌렀는데 캠페인오브젝트가 없으면 안됨.
		XArchive arDummy;
		DBA_SVR->SendReqLeaveGuildRaid( GetidAcc(), 
										GetidGuild(), 
										GetLevel(),
										spCampObj->GetidProp(), 
										0, 
										XGAME::xWT_NONE,	// 전투취소
										arDummy );
	}
	auto typeSpot = pBaseSpot->GettypeSpot();
	if( typeSpot == XGAME::xSPOT_COMMON ) {

	} else {
	}
	return 1;
}

/**
 @brief 전투후 마지막으로 스팟 업데이트 정보등을 보낼때 공통 루틴
*/
// void XGameUser::ArchivingSendUpdateSpotFinishBattle( XArchive& ar, XSpot *pBaseSpot )
// {
// 	MAKE_CHECKSUM( ar );
// 	m_spAcc->SerializeResource( ar );
// 	XSpot::sSerialize( ar, pBaseSpot );
// }

int XGameUser::DispatchQuestEventByBattle( XSpot *pBaseSpot, 
																					bool bWin, 
																					bool bClearSpot, 
																					LegionPtr& spLegion  )
{
	xQuest::XEventInfo infoQuest;
	infoQuest.SetidSpot( pBaseSpot->GetidSpot() );
	infoQuest.SettypeSpot( pBaseSpot->GettypeSpot() );
	infoQuest.SetidCode( pBaseSpot->GetpBaseProp()->idCode );
	infoQuest.SetidArea( pBaseSpot->GetidArea() );
	infoQuest.SetspLegion( spLegion );
	if( bWin ) {
		// 캠페인의 경우 마지막까지 깨야 클리어이므로 bWin=true여도 bClearSpot은 false일수 있다.
		if( bClearSpot ) {
			DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_SPOT, infoQuest );
			DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_SPOT_CODE, infoQuest );
//			if( m_spAcc->IsGreenOver( pBaseSpot->GetPower() ) ) {
				DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_SPOT_TYPE, infoQuest );
//			}
		}
	}
	// 영웅킬 퀘스트는 전투에서 져도 영웅만 죽이면 달성하는것이므로 bWin밖에 있어야 한다.
	// 스팟전투 승리후에 들어오는 곳이므로 군단정보가 반드시 있어야 함.
	XBREAK( spLegion == nullptr );
	if( spLegion != nullptr ) {
		XVector<XHero*> aryHeros;
		spLegion->GetHerosToAry( &aryHeros );
		for( auto pHero : aryHeros ) {
			infoQuest.SetidHero( pHero->GetidProp() );
			DispatchQuestEvent( XGAME::xQC_EVENT_KILL_HERO, infoQuest );
		}
	}
	return 1;
}

/**
 @brief 전투후 보상처리
*/
int XGameUser::DoRewardProcess( XSpot *pBaseSpot, XGAME::xBattleResult *pOutResult )
{
	if( pOutResult->IsWin() ) {
		// 레더점수 증가
		pOutResult->logForAttacker.addScore = random( 1, 3 );
		m_spAcc->AddScore( pOutResult->logForAttacker.addScore );
		DoDropProcess( pBaseSpot, &pOutResult->aryUpdated, 
									&pOutResult->aryDrops );
		DoRewardResource( pBaseSpot, pOutResult );
	}
	// 영웅들 경험치 증가(져도 받음)
	XLegion *pLegion = m_spAcc->GetCurrLegion().get();
	m_spAcc->AddExpToHeros( XGC->m_expBattlePerHero, m_spAcc->GetCurrLegion().get(), &pOutResult->m_aryLevelUpHeroes );
	// 영웅들정보 갱신(갱신된 부분만 보내도록 최적화 할것)
	pLegion->GetHerosToAry( &pOutResult->aryHeroes );
	return 1;
}

/**
 @brief 스팟에 자원드랍이 설정되어있으면 자원을 계정창고에 넣는다.
*/
int XGameUser::DoRewardResource( const XSpot *pBaseSpot, XGAME::xBattleResult *pOut )
{
	if( XBREAK(pOut->IsLose()) )	
		return -1;
	// 전투한스팟의 현재 별달성도
	int numStarBySpot = m_spAcc->GetNumSpotStar( pBaseSpot->GetidSpot() );
	// 자원 드랍이 설정되어있으면 자원을 먹게 함.
	for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
		auto type = (XGAME::xtResource)i;
		int amount = pBaseSpot->GetLootAmount( type );
		if( amount > 0 ) {
			if( m_spAcc->GetBuff( _T( "item_loot_up" ) ) )
				amount *= 2;
			float mulByStar = 1.f;
			switch( numStarBySpot ) {
			case 1:	mulByStar = 1.0f;	break;
			case 2: mulByStar = 1.1f;	break;		// 2별부터 보너스가 붙음
			case 3: mulByStar = 1.3f;	break;
			default:
				break;
			}
			// 스팟 별점상태에 따라 루팅양을 조절한다.
			amount = int(amount * mulByStar);
			pOut->m_mulByStar = (int)(mulByStar * 100.f);
			m_spAcc->AddResource( type, amount );
			pOut->logForAttacker.AddLootRes( type, amount );
//			pOut->logForAttacker.aryLoot.push_back( XGAME::xRES_NUM( type, amount ) );
		} // if( amount > 0 ) {
	}
	return 1;
}

/**
 @brief 성스팟 보상처리
*/
int XGameUser::DoRewardProcessForCastle( XSpotCastle *pSpot, XGAME::xBattleResult *pOut,
										XGAME::xBattleFinish& battle )
{
	// 패배시엔 방어자의 죽은 자원부대수를 얻어서 그에따른 루팅양을 공격자에게 준다.
	if( pOut->IsLose() ) {
		// 방어자의 군단.
		auto spLegion = pSpot->GetspLegion();
		if( XASSERT(spLegion) ) {
			spLegion->DeserializeForGuildRaid( battle.arLegion[ 1 ] );
			// 전멸한 자원부대수를 얻는다.
			int numDead = spLegion->GetNumDeadSquadByResourceSquad();
			if( numDead ) {
				XBREAK( pOut->logForAttacker.aryLoot.size() != 0 );
				// 스팟의 루팅양을 구해서 전멸한 자원부대수만큼의 비율로 루팅양을 얻는다.
				for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
					auto typeRes = (XGAME::xtResource)i;
					int num = pSpot->GetLootAmount( typeRes );
					if( num > 0 ) {
						// typeRes자원의 총루팅양의 25%를 /3해서 그것을 전멸한 부대수만큼 곱한게 약탈량
//						const auto rateLoot = XGC->m_rateLootResourceSquad;
//						int looted = (int)(((num * rateLoot) / 3.f) * numDead);
						int looted = (int)(XGC->GetLootAmountByEach( (float)num ) * numDead);
						if( m_spAcc->GetBuff( _T( "item_loot_up" ) ) )
							looted *= 2;
						if( looted == 0 )
							looted = 1;
						XBREAK( looted < 0 );
						// 계정정보에 더하고
						m_spAcc->AddResource( typeRes, looted );
						pOut->logForAttacker.AddLootRes( typeRes, looted );
						// 약탈한 만큼 스팟 루팅양에서 뺌.
						pSpot->AddLootRes( typeRes, -looted );
					}
				}
			}
		}
	}
	return 1;
}

/**
 @brief 전투패배한 상대의 DB에 쓰는 처리가 다끝나면 상대의 로그인락을 해제시킴.
*/
void XGameUser::RecvDefeatInfo( XPacket& p )
{
}

int XGameUser::DoRewardProcessForJewel( XSpotJewel *pSpot, XGAME::xBattleResult *pOut )
{
	if( pOut->IsWin() ) {
		int loot = pSpot->GetlootJewel();
		// 스팟을 점령했다. 스팟정보를 내 계정으로 가져온다. 상대스팟정보를 클리어 한다. 
		if( m_spAcc->GetBuff( _T( "item_loot_up" ) ) )
			loot *= 2;
		m_spAcc->AddResource( XGAME::xRES_JEWEL, loot );
		pOut->logForAttacker.aryLoot.push_back( XGAME::xRES_NUM( XGAME::xRES_JEWEL, loot ) );
	} else {
		// 공략에 실패했으므로 방어도만 다운시킨다.
		return -1;
	}
	return 1;
}

/**
 @brief NPC상대로 이기고 인카운터가 없을때나 졌을때 혹은 PC상대(Encounter)로 이겼을때.
*/
int XGameUser::DoRewardProcessForSulfur( XSpotSulfur *pSpot, XGAME::xBattleResult *pOut )
{
	// 유황스팟은 패배했을때 이쪽으로 들어오지 않는다.
	if( XBREAK(pOut->IsLose()) )
		return 0;
	DoRewardProcess( pSpot, pOut );
	return 1;
}

/**
 @brief 
*/
void XGameUser::ProcForMandrake( const XSpotMandrake *pSpot, XGAME::xBattleResult *pOut )
{
	// 
	int idx = -1;
	auto pProp = PROP_WORLD->GetpPropMandrake( pSpot->GetidSpot() );
	if( XASSERT(pProp) )
		idx = pProp->idx;
	if( XBREAK(idx < 0) )
		return;
	int reward = pSpot->GetReward();	// 
	if( pOut->IsWin() ) {
		ID idEnemy = pSpot->GetidOwner();
		// 공성전에서 이겼다.
		// 상대방 만드레이크 스팟의 대전정보를 클리어 시켜 ? 상태로 만든다.
		DBA_SVR->SendChangeMandrakeOwn( idx,
																		idEnemy,
																		0,
																		0,
																		_T( "" ),
																		pSpot->GetWin(),	// 현재 승수를 보내서 상대가 로그인했을때 최종 몇번 연승했는지 알수 있도록 한다.
																		reward,	// 현재 쌓여있던 보상값을 상대가 가져가도록 한다.
																		0,
																		0 );

		// 상대가 날 매칭해놓고 잠수 타버릴수가 있으므로 매칭이 된 후 일정시간동안 공격해 오지 않으면
		// 상대의 정보(idEnemy)를 클리어 시켜 다른 사람이 날 매칭할 수 있도록 하자.
		// 정찰을 통해 매칭된 상대의 정보를 보고 캐쉬로 클리어 시킬수 있는 수단도 필요.
		// 
	} else {
		// 우리가 상대를 뺏으러 갔지만 졌음.
		int win = pSpot->GetWin() + 1;	// 상대 스팟에 1승추가
		reward *= 2;
		// 공성전에서 졌다.
		// 스팟주인의 DB데이타를 갱신시키도록 한다.
		DBA_SVR->SendChangeMandrakeOwn( idx,		// idxSpot
								pSpot->GetidOwner(),		// 저장할 유저의 아이디
								pSpot->GetidOwner(),		// 스팟의 주인
								0,	// idEnemy. 나와의 링크를 끊는다.
								pSpot->GetszName(),
								win,
								reward,
								pSpot->GetidxLegion(),
								1 );		// 
	}
}

/**
 @brief NPC스팟 개별보상처리
*/
int XGameUser::DoRewardProcessForNpc( XSpotNpc *pSpot, XGAME::xBattleResult *pOut )
{
	if( XBREAK( pOut->IsLose() ) )	return -1;
	int numLoot = pSpot->GetnumResource();
	if( m_spAcc->GetBuff( _T( "item_loot_up" ) ) )
		numLoot *= 2;
	m_spAcc->AddResource( pSpot->GettypeResource(), numLoot );
	pOut->logForAttacker.aryLoot.push_back( XGAME::xRES_NUM( pSpot->GettypeResource(), numLoot ) );
	return 1;
}

/**
 @brief Daily스팟 개별보상처리
*/
int XGameUser::DoRewardProcessForDaily( XSpotDaily *pSpot, XGAME::xBattleResult *pOut )
{
	if( XBREAK( pOut->IsLose() ) )	return -1;
	return 1;
}

/**
 @brief 전투후 처리에 대한 스팟별 처리
*/
int XGameUser::ProcSpotEach( XSpot *pBaseSpot, 
														XGAME::xBattleResult *pOut, 
														XGAME::xBattleFinish& battle )
{
	// 스팟별 추가 보상 및 처리
	//////////////////////////////////////////////////////////////////////////
	switch( pBaseSpot->GettypeSpot() ) {
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_CASTLE: {
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		pOut->SetidEnemy( pSpot->GetidOwner() );
		DoRewardProcessForCastle( pSpot, pOut, battle );
		if( pOut->IsWin() ) {
			// 이겼을때 그 스팟에서 가장 낮았던 전투력을 기억함.
			int power = pSpot->GetPower();
			// 높은값을 저장하는 게 맞는거 같다.
			if( power > pSpot->GetpowerUpperLimit() )
				pSpot->SetpowerUpperLimit( power );
		}
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_JEWEL: {
		auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		pOut->SetidEnemy( pSpot->GetidOwner() );
		if( pOut->IsWin() )
			DoRewardProcessForJewel( pSpot, pOut );
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_SULFUR: {
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == nullptr );
		ProcSpotEachSulfur( pSpot, pOut, battle );
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_MANDRAKE: {
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		// 만드레이크는 최초 내가 뺏을때는 제외하고 모두 오프라인 방어전이다.
		// 이함수는 뺏을때만 호출되므로 스팟아이디와 내 아이디가 같을 수 없다.
		XVERIFY_BREAK( pSpot->GetidOwner() == GetidAcc() );
		pOut->SetidEnemy( pSpot->GetidOwner() );
		ProcForMandrake( pSpot, pOut );
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_NPC: {
		auto pSpot = SafeCast<XSpotNpc*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		if( pOut->IsWin() )
			DoRewardProcessForNpc( pSpot, pOut );
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_DAILY: {
		auto pSpot = SafeCast<XSpotDaily*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == NULL );
		if( pOut->IsWin() ) {
			DoRewardProcessForDaily( pSpot, pOut );

		}
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_COMMON:
	case XGAME::xSPOT_CAMPAIGN: {
		if( pBaseSpot->IsCampaignType() )
			ProcForCampaign( pBaseSpot, pOut, battle );
	} break;
	case XGAME::xSPOT_PRIVATE_RAID: {

	} break;
	default:
		XVERIFY_BREAK( 1 );
		break;
	}
	return 1;
}
/**
 @brief 유황스팟에 대한 전투종료처리
*/
int XGameUser::ProcSpotEachSulfur( XSpotSulfur *pSpot, XGAME::xBattleResult *pOut, const XGAME::xBattleFinish& battle )
{
	// 유황스팟의 경우
	XBREAK( pSpot == nullptr );
//	pSpot->SetbEncounter( false );	// 앞선전투에서 true한걸 클리어.
	if( pOut->bVsPVP ) {
		pOut->SetidEnemy( pSpot->GetidEncounterUser() );
		XBREAK( pOut->GetidEnemy() == 0 );
	}
	// 인카운터시 도주하면 상대유저에게 이긴것으로 처리한다.
	if( pOut->IsWin() ) {
		DoRewardProcessForSulfur( pSpot, pOut );
//		if( pSpot->IsVsNPC() ) {
		if( pOut->bVsPVP == false ) {
			// NPC에게 이긴상황
			int bEncounter = xRandom( 3 );
			if( bEncounter == 0 ) {
				// 인카운터 발생
				pOut->bEncounter = true;			// 인카운터가 발생함. 
				return 1;
			}
		}
	// Win
	} else {
	// Lose
		// vsPC전 이었는가.
		if( pSpot->IsVsPC() ) {
			// 인카운터(유저)전에서 졌다.
			// 인카운터 상대가 유황을 가져감.
			DBA_SVR->SendEncounterWin( GetidAcc()
																, pSpot->GetidEncounterUser()
																, pSpot->GetEncounterName()
																, pSpot->GetnumSulfur()
																, pOut->idSpot );
			// NPC하고는 이겼으므로 스팟은 리셋
		} else {
			// NPC에게 패배
		}
		XASSERT( pOut->aryUpdated.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
		XASSERT( pOut->aryDrops.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
		XASSERT( pOut->aryHeroes.size() == 0 );	// 이전에 null로 넘겼기때문에 지금은 0인걸로 생각해야한다.
	}
	return 1;
}

/**
 @brief 
*/
int XGameUser::ProcForCampaign( XSpot *pBaseSpot, XGAME::xBattleResult *pOut, const XGAME::xBattleFinish& battle ) 
{
	XASSERT( pBaseSpot->IsCampaignType() );
	XVERIFY_BREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_CAMPAIGN &&
					pBaseSpot->GettypeSpot() != XGAME::xSPOT_COMMON );
	xCampaign::CampObjPtr spCampObj;
	auto typeSpot = pBaseSpot->GettypeSpot();
	int idxFloor = 0;
	//////////////////////////////////////////////////////////////////////////
	if( typeSpot == XGAME::xSPOT_CAMPAIGN ) {
		auto pSpot = static_cast<XSpotCampaign*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == nullptr );
		spCampObj = pSpot->GetspCampObj();
	} else
	//////////////////////////////////////////////////////////////////////////
	if( typeSpot == XGAME::xSPOT_COMMON ) {
		auto pSpot = static_cast<XSpotCommon*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == nullptr );
		if( pSpot->IsGuildRaid() ) {
			spCampObj = pSpot->GetspCampObj( m_spAcc->GetGuildIndex() );
			XVERIFY_BREAK( spCampObj == nullptr );      // 캠페인 스테이지 눌렀는데 캠페인오브젝트가 없으면 안됨.
			XGAME::xtWin win = (pOut->IsWin())? XGAME::xWT_WIN : XGAME::xWT_LOSE;	// 혹시 승/패가 아닌 정보가 필요할수도 있을거 같아서 이형태로 보냄
			DBA_SVR->SendReqLeaveGuildRaid( GetidAcc(), 
											m_spAcc->GetGuildIndex(), 
											GetLevel(), 
											spCampObj->GetidProp(), 
											0, 
											win, 
											battle.arLegion[1] );
		} else
		if( pSpot->IsMedalCamp() ) {
			spCampObj = pSpot->GetspCampObj( nullptr );
		} else
		if( pSpot->IsHeroCamp() ) {
			spCampObj = pSpot->GetspCampObj( nullptr );
			auto spCampObjHero = std::static_pointer_cast<XCampObjHero2>( spCampObj );
			if( XASSERT(spCampObjHero) ) {
				idxFloor = spCampObjHero->GetidxFloorByBattle();		/// 서버에서 보관해둔값으로 써야함.
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 승리시 처리
	if( pOut->IsWin() ) {
		XVERIFY_BREAK( spCampObj == nullptr );      // 캠페인 스테이지 눌렀는데 캠페인오브젝트가 없으면 안됨.
		int idxStage = spCampObj->GetidxPlaying();	// 방금플레이했던 스테이지
		XBREAK( idxStage < 0 );
		auto spStageObj = spCampObj->GetspStage( idxStage, idxFloor );		// 방금 전투한 스테이지
		auto spLegion = spStageObj->GetspLegion();
		auto bClearLastStage = spCampObj->ClearStageLastPlay( idxFloor );
		auto spStageLastPlay = spCampObj->GetspStageLastPlay( idxFloor );
		if( spStageLastPlay ) {
			if( pOut->numStar > spStageLastPlay->GetnumStar() )
				spStageLastPlay->SetnumStar( pOut->numStar );
		}
//		if( spCampObj->IsClearCampaign() == true ) {
		if( bClearLastStage ) {
			// 마지막스테이지까지 다 깼다.
			pOut->bClearSpot = true;			// 퀘스트 조건용.
		} else {
			// 별점 누적
			pOut->bClearSpot = false;
		}
		if( idxStage >= 0 ) {
			xQuest::XEventInfo infoQuest;
			infoQuest.SetidSpot( pBaseSpot->GetidSpot() );
			infoQuest.SettypeSpot( typeSpot );
			infoQuest.SetidCode( pBaseSpot->GetpBaseProp()->idCode );
			infoQuest.SetidArea( pBaseSpot->GetidArea() );
			infoQuest.SetspLegion( spLegion );
			infoQuest.SetParam( XGAME::xQC_EVENT_CLEAR_STAGE, (DWORD)(idxStage + 1) );
			DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_STAGE, infoQuest );
		}
	}
	return 1;
}

/**
 스팟 동기화
 @param bDestroy pSpot을 삭제하라
*/
void XGameUser::SendSpotSync( XSpot *pSpot, bool bDestroy )
{
	XBREAK( pSpot == nullptr );
	XPacket ar( (ID)xCL2GS_SPOT_SYNC );
	ar << pSpot->GetidSpot();
	if( bDestroy == false ) {
		ar << pSpot->GetsnSpot();	// 검사용
		XSpot::sSerialize( ar, pSpot );
	} else
		ar << 0;	// 스팟 삭제
	Send( ar );
}
/**
 this의 군대와 pLegion의 군단이 전투를 해서 결과를 리턴한다.
*/
BOOL XGameUser::DoBattle( ID idSpot, 
						ID idAccount, 
						int level, 
						LPCTSTR szName, 
						XLegion *pLegion )
{
	return random(100) > 30 ? TRUE : FALSE;
}
/**
 @brief 플레이어의 전투력에 기반해서 범위를 얻는다
*/
void XGameUser::GetMatchRangePowerByPlayerPower( int *pOutMin, int *pOutMax )
{
	XBREAK( pOutMin == nullptr );
	XBREAK( pOutMax == nullptr );
	/*
	기준점수에 -0.2배 ~ +1.5배를 검색범위로 한다.
	내 전투력이 평균치보다 높으면 평균치와 믹스해서 그것을 기준점수로 한다.(현질러는 항상 강한상대와 붙지 않도록 하기 위해)
	내 전투력이 평균치보다 낮으면 그냥 내 점수를 기준점수로 한다.
	*/
	int myPower = m_spAcc->GetMilitaryPowerCurrLegion();
	int basePower = myPower;
	// 난이도가 계단식으로 나오도록 일정자리 이하는 버린다.
	if( basePower > 10000 ) {
		basePower = (basePower / 5000) * 5000;    // 만이하 절삭해서 난이도가 계산식으로 이뤄지게 한다.
	} else
	if( basePower > 5000 ) {
		basePower = (basePower / 1000) * 1000;  
	}
	// 10전 이상부터는 승률에 맞춰 전투력을 조절한다.
	if( m_spAcc->GetnumBattleByPvp() > 10 ) {
		basePower = (int)(basePower * ( m_spAcc->GetRatioWinsByPvp() + 0.5f ));
	}
	int powerMin;
	int powerMax;
	if( basePower > 10000 ) {
		powerMin = (int)( basePower * 0.9f );
		powerMax = (int)( basePower * 1.5f );
	} else
	if( basePower > 5000 ) {
		powerMin = (int)( basePower * 0.9f );
		powerMax = (int)( basePower * 1.5f );
	} else {
		powerMin = (int)( basePower * 0.5f );
		powerMax = (int)( basePower * 1.1f );
	}
	if( GetLevel() == 1 ) {
		powerMin = 5000;
		powerMax = 8000;
	}
	XBREAK( powerMin < 0 );
	XBREAK( powerMax > 10000000);   // 지나치게 높은 값.
	if( pOutMin )
		*pOutMin = powerMin;
	if( pOutMax )
		*pOutMax = powerMax;
}

/**
 @brief 매치될 상대의 색등급을 계산한다.
 현재 오픈된 지역중 가장높은레벨의 지역을 기준으로 매칭하는 방식
*/
int XGameUser::GetGradeLevelByMatch( XSpot *pBaseSpot )
{
	const int power = m_spAcc->GetPowerExcludeEmpty();
	const auto pWorld = m_spAcc->GetpWorld();
	const int numCastle = pWorld->GetNumSpots( XGAME::xSPOT_CASTLE );
	const int lvSub = (numCastle <= 6)? 2 : 3;
	const int lvBase = pWorld->GetMaxLevelOpenedArea() - lvSub;
	const int lvArea = pBaseSpot->GetAreaLevel();
	int gradeColor = 0;
	// 스팟지역레벨에 따라 스폰되어야 하는 색
	if( lvArea >= lvBase + 3 ) {
		// 빨강 스폰
		gradeColor = 2;
	} else
	if( lvArea >= lvBase + 2 ) {
		// 주황/빨강 스폰
		gradeColor = 1 + xRandom(2);
		// 랜덤으로 하지말고 곱하는 값을 직접 지정해서 단계적으로 조금씩 어려워지도록 하는게 낫다.
	} else
	if( lvArea >= lvBase ) {
		gradeColor = 1;
	} else 
	if( lvArea >= lvBase - 3 )	{
		gradeColor = 0;
	} else {
		gradeColor = -1;
	}
	return gradeColor;
}

/**
 @brief 매치될 상대의 색등급을 계산한다.
 지역고유레벨과 군주레벨의 비교로 계산된다.
*/
int XGameUser::GetGradeLevelByMatch2( XSpot* pBaseSpot )
{
	/* 계정렙+1지역은 주황스폰,+2는 주황/빨강, 그이상은 빨강
	계정렙-5까지는 노랑스폰, 그 이하는 초록
	단 그 성스팟의 gradeLowest값이 있다면 그 등급이상은 스폰시키지 않음.	*/
	const int lvAcc = m_spAcc->GetLevel();
	const int power = m_spAcc->GetPowerExcludeEmpty();	// 슬롯빈건 제외한 전투력이기땜에 고의로 슬롯을 비워서 치팅할수도 있음 잉여영웅수도 같이 봐야함.
	int lvArea = pBaseSpot->GetAreaLevel();
	int gradeColor = 0;
	// 스팟지역레벨에 따라 스폰되어야 하는 색
	if( lvArea >= lvAcc + 2 ) {
		// 빨강 스폰
		gradeColor = 2;
	} else
	if( lvArea >= lvAcc ) {
		// 주황/빨강 스폰
		gradeColor = 1;
		// 랜덤으로 하지말고 곱하는 값을 직접 지정해서 단계적으로 조금씩 어려워지도록 하는게 낫다.
	} else
	if( lvArea >= lvAcc - 2 ) {
		gradeColor = 0;
	} else 
	if( lvArea >= lvAcc - 5 )	{
		gradeColor = -1;
	} else {
		gradeColor = -2;
	}
	return gradeColor;
}

/**
 @brief 성스팟 pBaseSpot의 지역레벨에 맞춰 매치되어야 하는 적당한 전투력 범위를 구한다.
*/
void XGameUser::GetMatchRangePowerForCastle( XSpotCastle *pSpot, int *pOutMin, int *pOutMax )
{
	if( XBREAK( pSpot == nullptr ) )
		return;
	XBREAK( pOutMin == nullptr );
	XBREAK( pOutMax == nullptr );
	*pOutMin = 0;
	*pOutMax = 0;

	const int power = m_spAcc->GetPowerExcludeEmpty();
	int gradeColor = GetGradeLevelByMatch2( pSpot );
	CONSOLE_TAG( "match", "%s:grade=%d", __TFUNC__, gradeColor );
	if( m_spAcc->GetLevel() < 10 && pSpot->IsHaveNotPowerUpperLimit() ) {
		// 최초 도전으로 봐도 된다.
		gradeColor = 1;		// 빨강으로 매치되도록.
	}
	/*
	노랑지역의 경우 무조건 노랑범위로 매칭이 되는데
	지역레벨을 약간씩 반영하여 같은 노랑이라도 약간씩 차이가 나도록 하면 더 좋을듯.
	*/
	// 매칭으로 얻어온 power값과 실제 군대의 power값이 다를수 있다.(계산 알고리즘이 변한경우)
	// 그러므로 매칭때는 노랑이었는데 실제 군대는 초록일수 있으므로 매칭시 하한선을 정해줘야 한다.
	if( XBREAK( gradeColor >= 3 ) )
		gradeColor = 0;
	// 결정된 색등급의 전투력 범위를 구함.
	int powerMin = (int)( power * XGAME::GetMultiplyPower( gradeColor ) );
	int powerMax = (int)( power * XGAME::GetMultiplyPower( gradeColor + 1 ) ) - 1;
	CONSOLE_TAG( "match", "%s:accPow=%d, grade=%d, min=%d, max=%d", __TFUNC__, power, gradeColor, powerMin, powerMax );
	// 스팟에 전투력상한선이 지정되어있다면...
	if( pSpot->IsHavePowerUpperLimit() ) {
		// max값이 상한선을 넘어가면 min,max를 다시 구함.
		if( powerMax > pSpot->GetpowerUpperLimit() ) {
			int dist = powerMax - powerMin;
			powerMax = pSpot->GetpowerUpperLimit();
			powerMin = powerMax - dist;
			// 지역색 이하로는 떨어지지 않는다.
			int powerGreen = XGAME::GetPowerByColor( power, gradeColor );
			if( powerMin < powerGreen ) {
				powerMin = powerGreen;
				powerMax = powerMin + dist;
			}
			XBREAK( powerMax <= 0 || powerMin <= 0 );
		}
	}
	XBREAK( powerMax - powerMin < 100 );	// 혹시나 gradeColor가 3이 왔을때 이런경우가 생길수 있음.
	*pOutMin = powerMin;
	*pOutMax = powerMax;
}

/*
.원하는 전투력범위의 상대를 찾는다.
.상대가 보유한 자원정보는 100%로 가져온다.
.상대와의 전투력차이에 의해 몇%의 자원만 보일건지결정한다.(매칭당시)
.전투결과가 나왔을때 상대가 회색이었다면 패배(승리도..)정보를 전달하지 않는다.
.매칭된지 24시간이 지난 상대는 무료로 교체할수 있게 한다.
*/

/**
 적당한 유저를 매칭시켜 내 영지의 성을 점령하게 한다.
*/
void XGameUser::DoMatchUserSpot( XSpotCastle *pSpot, xSec secAdjustOffline )
{
	if( XBREAK( pSpot == nullptr ) )
		return;
	// 적당한 매치범위를 구한다.
	int powerMin, powerMax;
	GetMatchRangePowerForCastle( pSpot, &powerMin, &powerMax );
	CONSOLE_TAG("match", "%s:min=%d, max=%d", __TFUNC__, powerMin, powerMax );
// 	XBREAK( powerMax - powerMin < 100 );	// 혹시나 gradeColor가 3이 왔을때 이런경우가 생길수 있음.
	ID idKey =
		DBA_SVR->SendReqGetUserByPower( m_spAcc->GetidAccount(),
										pSpot->GetidSpot(),
										powerMin,
										powerMax,
										secAdjustOffline,
										GetidConnect(),
										xCL2GS_LOBBY_ATTACKED_CASTLE );
	DBA_SVR->AddResponse( idKey,
						this,
						&XGameUser::RecvGetUserByPower );
}

/**
 @brief 
*/
void XGameUser::DoMatchSulfur( XSpotSulfur *pSpot, const XGAME::xBattleResult& result )
{
	// 타유저 난입상황
	// DB서버에 유저 매치요청
	// 기본적으로 지역레벨에 기반해서 매치가되지만 하한선은 노랑색이 되도록 한다.
	int lvAcc = m_spAcc->GetLevel();
	int power = m_spAcc->GetPowerExcludeEmpty();
	int lvArea = pSpot->GetAreaLevel();
	int gradeColor = 0;
	// 스팟지역레벨에 따라 스폰되어야 하는 색
	if( lvArea >= lvAcc + 3 ) {
		// 빨강 스폰
		gradeColor = 2;
	} else
	if( lvArea >= lvAcc + 2 ) {
		// 주황/빨강 스폰
		gradeColor = 1 + xRandom(2);
		// 랜덤으로 하지말고 곱하는 값을 직접 지정해서 단계적으로 조금씩 어려워지도록 하는게 낫다.
	} else
	if( lvArea >= lvAcc ) {
		gradeColor = 1;
	} else {
		gradeColor = 0;
	}
	if( XBREAK( gradeColor >= 3 ) )
		gradeColor = 0;
	int powerMin = (int)( power * XGAME::GetMultiplyPower( gradeColor ) );
	int powerMax = (int)( power * XGAME::GetMultiplyPower( gradeColor + 1 ) ) - 1; // gradeColor안에서만 나오도록 하기위해 상위등급으로 올라가지 않도록 -1을 해줌
	//
//	XBREAK( pSpot->GetnumSulfur() == 0 );
	XArchive arRequest;
	arRequest << pSpot->GetidSpot();
	arRequest << (int)pSpot->GettypeSpot();
//	arRequest << (int)pSpot->GetnumSulfur();
	arRequest << result;

	ID idKey =
		DBA_SVR->SendGetUserLegionByScore( powerMin,
																			powerMax,
																			0,	// idxLegion
																			m_spAcc->GetidAccount(),
																			arRequest,
																			// 이거 유황스팟용이라는걸 명시적으로 알수있도록 이름바꿀것.
																			xCL2GS_INGAME_REQ_FINISH_BATTLE );	// idPacket
	DBA_SVR->AddResponse( idKey, this,
						&XGameUser::RecvGetUserLegionByScore );
}
/**
 @brief 광산상대를 찾아 매치시킨다.
*/
void XGameUser::DoMatchJewel( XSpotJewel *pSpot, bool bSimul, bool bSimulWin )
{
	auto pProp = pSpot->GetpProp();
	// 0번광산은 말뚝광산이므로 매치시키지 않는다.
	if( XBREAK(pProp->idx == 0) )
		return;
	// 매치 요청
	int power = m_spAcc->GetPowerIncludeEmpty();	// 광산은 의도적으로 전투력 낮춰놓고 매치할수도 있으니 포함된값으로 검색.
	// 노랑색 난이도범위에서 매치한다.
	int powerMin = (int)(power * XGAME::GetMultiplyPower(-1));
	int powerMax = (int)(power * XGAME::GetMultiplyPower(1));
	// 매치할 적당한 전투력 범위를 구한다.
// 	GetMatchRangePower( &powerMin, &powerMax );
	// DB서버에 0,1번 보석광산을 매치시켜 달라고 요청한다.
	// 이것도 나중에 ? 로 시작하는 방식으로 바꿀것.
	ID idKey;
	idKey = DBA_SVR->SendReqMatchJewelMine( pProp->idx,
																					GetidAcc(),
																					powerMin,
																					powerMax,
																					0,
																					bSimul,
																					bSimulWin,
																					GetidConnect() );
	DBA_SVR->AddResponse( idKey, this,
												&XGameUser::RecvReqMatchJewelMine );
}

/**
 @brief 보석광산 매칭결과를 받음.
*/
void XGameUser::RecvReqMatchJewelMine( XPacket& p )
{
	char c0;
	BYTE b0;
	p >> c0;	int idxMine = c0;
	p >> b0;	bool bSimul = (b0 != 0);
	p >> b0;	bool bSimulWin = (b0 != 0 );
	p >> b0;	bool bFound = (b0 != 0);
	XGAME::xJewelMatchEnemy infoMatch;
	//
	if( bFound ) {
		XArchive arInfoMatch;
		p >> arInfoMatch;
		infoMatch.DeSerialize( arInfoMatch );
	}
	//
	auto pSpot = GetpWorld()->GetSpotJewelByIdx( idxMine );
	if( XBREAK( pSpot == nullptr ) )
		return;
	//
	if( !bFound ) {
		// 적절한 상대를 못찾음.
		const auto bNoAttack = pSpot->IsNoAttack();
		pSpot->ClearSpot();
		pSpot->ClearMatchEnemy();
		pSpot->OnSpawn( m_spAcc );
		// 상대를 못찾음.
		pSpot->SetPlayerOwner( m_spAcc->GetidAccount(), m_spAcc->GetstrName() );
		pSpot->ClearLevelMine();
		pSpot->SetbitFlagByNoAttack( bNoAttack );
	} else {
		//////////////////////////////////////////////////////////////////////////
		// 상대를 찾음.
// #ifdef _DEV
// 		if( bSimul ) {
// 			// 더미가 
// 			SimulAttackedJewel( pSpot, bSimulWin, infoMatch );
// 		} else 
// #endif // _DEV
		{
			// bFound true && not simul
			pSpot->ClearSpot();
			pSpot->ClearMatchEnemy();
			pSpot->OnSpawn( m_spAcc );
			// 매치가 되어서 다른 유저가 스팟을 차지함.
			pSpot->SetPlayerOwner( infoMatch.m_idAcc, infoMatch.m_strName.c_str() );
			pSpot->SetidMatchEnemy( infoMatch.m_idAcc );
//#ifdef _DEV
			if( infoMatch.IsDummyUser() ) {
				// 더미유저가 매칭되었을때만...
				pSpot->SetstrNameByMatchEnemyForSimul( infoMatch.m_strName );
				pSpot->SetlvMatchEnemyForSimul( infoMatch.m_lvAcc );
			}
//#endif // _DEV
			pSpot->SetMatch( infoMatch );
			// 매칭해올때 매칭상대의 idEnemy에 this의 idAcc로 갱신되어 
			// 다른 유저가 매칭못하게 되어있다는걸 가정함.
		}
	}
	// 메모리상의 매치된 광산정보가 사라지기전에 저장시킴.
#if !defined(_XUZHU)
	Save();
#endif // not XUZHU
	//
// 	if( bSimul ) {
// 		SendSpotSync( pSpot );
// 	} else 
	{
		XPacket ar( (ID)xCL2GS_LOBBY_JEWEL_MATCH_RESULT );
		ar << idxMine;
		ar << infoMatch.m_idAcc;
		XSpot::sSerialize( ar, pSpot );	// 개발의 편의상 통째로 함. 나중에 패킷최적화
		Send( ar );
	}
}

/**
 @brief 가상상대(더미)가 내 스팟이었던 pSpot을 침공한것을 시뮬레이션.
*/
#ifdef _DEV
// void XGameUser::SimulAttackedJewel( XSpotJewel *pSpot
// 																	, bool bDefenseOk
// 																	, const XGAME::xJewelMatchEnemy& info )
// {
// 	int lvMineOld = pSpot->GetlevelMine();
// 	// this가 승리(방어에 성공함)
// 	if( bDefenseOk ) {
// 		// 침공시뮬레이션에서 매치상대가 없었을때 this가 방어에 성공한 상황.
// 		// 주인이 안바꼈으므로 기존데이타는 그대로 두고 새로찾은 상대의 정보만 추가한다.
// 		pSpot->SetstrNameByMatchEnemyForSimul( info.m_strName );
// 		pSpot->SetlvMatchEnemyForSimul( info.m_lvAcc );
// 		pSpot->SetidMatchEnemy( info.m_idAcc );
// //		pSpot->SetDefense( info.m_Defense );
// 	} else {
// 		// 방어에 실패함.
// 		pSpot->ClearSpot();
// 		pSpot->ClearMatchEnemy();
// 		pSpot->OnSpawn( m_spAcc );
// 		// 소유주는 더미에게 넘어가고 더미의 상대를 this로 정한다.
// 		pSpot->SetPlayerOwner( info.m_idAcc, info.m_strName.c_str() );
// 		pSpot->SetstrNameByMatchEnemyForSimul( info.m_strName );
// 		pSpot->SetlvMatchEnemyForSimul( info.m_lvAcc );
// 		pSpot->SetidMatchEnemy( info.m_idAcc );
// 		auto infoCopy = info;
// 		infoCopy.m_LvMine = lvMineOld + 1;
// 		pSpot->SetMatch( infoCopy );
// 	}
// 	// 가상상대의 DB에 광산정보를 갱신함.
// 	DBA_SVR->SendUpdateJewelMineInfo( info.m_idAcc, GetidAcc(), pSpot );
// }
#endif // _DEV

/**
 @brief 
*/
void XGameUser::DoMatchMandrake( XSpotMandrake *pSpot )
{
	// 기본적으로 지역레벨에 기반해서 매치가되지만 하한선은 노랑색이 되도록 한다.
	int lvAcc = m_spAcc->GetLevel();
	int power = m_spAcc->GetPowerIncludeEmpty();
	int lvArea = pSpot->GetAreaLevel();
	int gradeColor = 0;
	// 스팟지역레벨에 따라 스폰되어야 하는 색
	if( lvArea >= lvAcc + 3 ) {
		// 빨강 스폰
		gradeColor = 2;
	} else
	if( lvArea >= lvAcc + 2 ) {
		// 주황/빨강 스폰
		gradeColor = 1 + xRandom(2);
		// 랜덤으로 하지말고 곱하는 값을 직접 지정해서 단계적으로 조금씩 어려워지도록 하는게 낫다.
	} else
	if( lvArea >= lvAcc ) {
		gradeColor = 1;
	} else {
		gradeColor = 0;
	}
	if( XBREAK( gradeColor >= 3 ) )
		gradeColor = 0;
	int powerMin = (int)( power * XGAME::GetMultiplyPower( gradeColor ) );
	int powerMax = (int)( power * XGAME::GetMultiplyPower( gradeColor + 1 ) ) - 1; // gradeColor안에서만 나오도록 하기위해 상위등급으로 올라가지 않도록 -1을 해줌
	int idx = pSpot->GetpProp()->idx;
	_tstring strLog 
		= XE::Format( _T( "Spot_Recon_Req_Enemy(XGAME::xSPOT_MANDRAKE) User_Level: %d, idx: %d" ), GetLevel(), idx );
	AddLog( XGAME::xULog_User_Spot_Recon, strLog );

	ID idKey 
		= DBA_SVR->SendReqMatchMandrake( GetidAcc()
																	, idx
																	, powerMin
																	, powerMax
																	, pSpot->GetsnSpot() );
	DBA_SVR->AddResponse( idKey, this,
										&XGameUser::RecvReqMatchMandrake );
}

/**
 DB서버로부터 min,max점수로 찾은 유저의 군단정보가 도착했다.
 유황매치에 사용
*/
void XGameUser::RecvGetUserLegionByScore( XPacket& p )
{
	ID idPacket, idSpot;
	XArchive arResult, arRequest;
	DWORD dw0;
	int ret = 1;
//	int numSulfur;
	p >> idPacket;
	p >> arRequest;
	p >> arResult;
	arRequest >> idSpot;
	arRequest >> dw0;		// dummy(typespot)
//	arRequest >> numSulfur;		// 획득유황 개수.
//	XBREAK( numSulfur == 0 );
	XGAME::xBattleResult resultBattle;
	arRequest >> resultBattle;
	do  {
	if( idPacket == xCL2GS_INGAME_REQ_FINISH_BATTLE ) {
		// 유황스팟에서만 호출되는것으로 가정함.
		ID idAcc;
		arResult >> idAcc;
		XSpot *pBaseSpot = GetpWorld()->GetSpot( idSpot );
		if( XBREAK( pBaseSpot == nullptr ) ) {
			ret = 0; break;
		}
		if( XBREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_SULFUR ) ) {
			ret = 0; break;
		}
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		if( XBREAK( pSpot == nullptr ) ) {
			ret = 0; break;
		}
		if( idAcc ) {
			_tstring strName;
			int score, power, level/*, numSulfur*/;
			arResult >> strName;
			arResult >> power;
			arResult >> score;
			arResult >> level;
			XArchive arLegion;
			arResult >> arLegion;
			XLegion *pLegion = XLegion::sCreateDeserializeFull( arLegion );
			// 이경우 클라는 전투가 끝나지 않으니 전투를 끝낼수 있도록 하는 처리 추가할것.
			if( XBREAK( pLegion == nullptr ) )	// 매치상대의 데이타가 깨져서 역직렬이 실패할수 있다.
				return;
			XBREAK( pSpot->GetspLegion() != nullptr );
			pSpot->ClearSpot();
			pSpot->SetpLegion( pLegion );
			auto spLegion = pSpot->GetspLegion();
			XBREAK( spLegion == nullptr );
			pSpot->SetPower( power );		// 상대가 부대슬롯을 안채워놨다면 실제 군단전투력과 다를수 있음. 그러나 그대로 사용하게 함.
			pSpot->SetidEncounterUser( idAcc );
			pSpot->SetEncounterName( strName.c_str() );
			pSpot->SetLevel( level );
			pSpot->SetScore( score );
			for( auto& res : resultBattle.logForAttacker.aryLoot ) {
				if( res.type == xRES_SULFUR )
					pSpot->SetnumSulfur( (int)res.num );
			}
			// 이부분 나중엔 필요없어짐
#ifdef _CHEAT
// 			if( strName == DUMMY_NAME )
// 				pLegion->DummyDataRechange( level, m_spAcc );
#endif // _CHEAT
			// 인카운터 유저를 찾아 클라에 전투시작을 보내줌.
			XASSERT( pSpot->GetnumSulfur() > 0 );
// 			XGAME::xBattleStartInfo info( false, pSpot );
// 			info.m_idEnemy = idAcc;
// 			info.m_strName = strName;
			SendBattleInfoWithidAcc( pSpot, idAcc, strName );
		} else {
			// 인카운터 유저를 못찾음.
			// 그냥 보상처리하고 전투종료
			XPacket ar( (ID)xCL2GS_INGAME_REQ_FINISH_BATTLE );
			// 스팟 리셋
			pSpot->ClearSpot();
			pSpot->ResetSpot();
			// 리셋까지 끝난 스팟정보를 클라에 보냄.
			ar << resultBattle;
			Send( ar );
			SendLevelSync();
		}
	} else {
		XBREAK(1);		// 이런경우 있으면 안됨. 뭐가됐건 클라에 응답패킷 보내야함.
	}
	} while( 0 );
}

/**
 전투력으로 매칭로 유저를 매칭해 유저정보를 받아왔다.
*/
void XGameUser::RecvGetUserByPower( XPacket& p )
{
	ID idSpot;
	ID idPacket;
	ID idAcc;
	int level;
	_tstring strName, strFbUserId, strHello;
	xSec secAdjustOffline;
	int ladder, power, gold;
	p >> idPacket;
	p >> idSpot;
	p >> secAdjustOffline;
	p >> idAcc;		// 매칭으로 찾은 상대편 idAccount, 0이면 NPC로 생성하라.
	if( idAcc ) {
		xResourceAry aryLocals;
		xResourceAry aryMain;
		p >> aryLocals;		// 이 값은 무시됨
		p >> aryMain;			// 이 값은 무시됨.
		p >> gold;
		p >> level;
		p >> strName;
		p >> power;
		p >> ladder;
		p >> strFbUserId;
		p >> strHello;
	} else {
		gold = 0;
		level = 0;
		power = 0;
		ladder = 0;
	}
	/*
	정찰전까지는 레벨,이름,전투력,루팅정보,루팅자원,드랍아이템,군단정보를 알수가 없다.
	정찰을 하지 않고 전투를 할수 없다.
	치트로 깨는경우 드랍아이템정보를 즉석에서 만들어내야 한다.
	*/
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	// 유저를 얻어온후 그다음 하는일은 패킷종류에 따라 다르다.
//#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	if( idPacket == xCL2GS_LOBBY_SIMUL_ATTACKED_HOME ) {
		// 더미에게 가상으로 공격받음을 시뮬레이션하기위해 더미정보 하나를 얻어옴.
		if( idAcc ) {
			SimulAttackedHome( idAcc, level, strName, power, ladder );
		}
	} else
//#endif // _DEV
	if( idPacket == xCL2GS_LOBBY_ATTACKED_CASTLE ) {
		// 성점령 당했을때.
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		XBREAK( pSpot == NULL );
		BOOL isRespawn = FALSE;
		XBREAK(idAcc == m_spAcc->GetidAccount());	// 이런경우가 생기는지 검사해봄
		if( idAcc == m_spAcc->GetidAccount() )
			isRespawn = TRUE;	// 이런경우가 있음?
		// 이미 맵상에 있는 유저라면 다시 검색함.
		if (m_spAcc->IsExistUserSpot(idAcc) && idAcc ) {
			if (m_spAcc->CheckSpotSearchAble()) {
				DoMatchUserSpot(pSpot, 0);
				_tstring strLog = XE::Format(_T("Spot_Recon_RecvGetUserByScore_Result(XGAME::xSPOT_CASTLE) AlreadyExistUser IdAccount: %d, Name: %s")
					, idAcc, strName.c_str());
				AddLog(XGAME::xULog_User_Spot_Recon, strLog);
// #if _DEV_LEVEL <= DLV_DEV_EXTERNAL
				CONSOLE_ACC( TAG_BATTLE, "중복 검색되어 다시 찾음.");
// #endif
				return;
			} else
				idAcc = 0;
		}
		// 새 정보로 쓰기전 그간 점령한 동안의 자원을 먼저 정산한다.
		pSpot->CalculateResource( m_spAcc->GetidAccount(), 0 );	// 앞서 timerCalc로 보정했으므로 여기서 다시 할필요는 없음.
		// 오프라인동안 매칭된상황도 있을수 있으니 그 시간을 보정해준다.
		if( secAdjustOffline > 0.f )
			pSpot->AdjustTimer( secAdjustOffline );
#pragma message("정산하고 지역창고 클리어 시켜버리면 안됨")
		pSpot->ClearSpot();
		if( idAcc ) {
			bool bFirst = false;
			if( bFirst ) {
				idAcc = 1;
				pSpot->SetPlayerOwner( idAcc, _T("Lord Gemtree"), isRespawn );
				pSpot->SetLevel( 8 );
				pSpot->SetPower( power );
				// 이제 유저로부터 받아온 aryLocal, aryMain은 무시하고 시스템에 내장된 계산식에 의해 루팅양을 결정한다.
				pSpot->SetDropRes2( m_spAcc, power, level );
				pSpot->SetScore( ladder );
				pSpot->SetstrcFbUserId( "" );
				strHello = _T("hello world!");
				pSpot->SetstrHello( strHello );
				pSpot->OnSpawn( m_spAcc );	// 모든 기본정보가입력되었으면 스폰핸들러를 호출한다.
			} else {
				pSpot->SetPlayerOwner( idAcc, strName.c_str(), isRespawn );
				pSpot->SetLevel( level );
				pSpot->SetPower( power );
				// 이제 유저로부터 받아온 aryLocal, aryMain은 무시하고 시스템에 내장된 계산식에 의해 루팅양을 결정한다.
				pSpot->SetDropRes2( m_spAcc, power, level );
				pSpot->SetScore( ladder );
				pSpot->SetstrcFbUserId( SZ2C( strFbUserId ) );
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
				if( strHello.empty() )		// 내부개발섭에선 인삿말이 보통 없으니 더미유저의 경우 랜덤으로 인삿말을 넣어준다.
					strHello = GetstrHelloByRandom();
#endif // #if _DEV_LEVEL <= DLV_DEV_EXTERNAL
				pSpot->SetstrHello( strHello );
				pSpot->OnSpawn( m_spAcc );	// 모든 기본정보가입력되었으면 스폰핸들러를 호출한다.
			} // not first
		} else {
			// 적절한 유저를 못찾음. 그냥 자기성으로 놔둠.
			CONSOLE_ACC( TAG_BATTLE, "적절한 유저 못찾음.");
		
			pSpot->ClearSpot();
			pSpot->ResetSpot();		// 다시 일정시간후에 검색할수 있도록 타이머를 리셋한다.
			pSpot->SetPlayerOwner( GetidAcc(), GetstrName(), TRUE );
			pSpot->SetLevel( GetLevel() );
		}
		//
		XPacket ar( idPacket );
		ar << idSpot;
		XSpot::sSerialize( ar, pSpot );
		Send( ar );
		
		_tstring strLog = XE::Format(_T("Spot_Recon_RecvGetUserByScore_Result(XGAME::xSPOT_CASTLE) User_Level: %d, Spot_Level: %d, idSpot: %d, Score: %d, Name: %s")
			, GetLevel(), pSpot->GetLevel(), pSpot->GetidSpot(), pSpot->GetScore(), pSpot->GetszName());
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);
	} else {
		XBREAK(1);	// 이런경우가 아직 있나?
	}
}

/**
 idAcc유저의 군단정보를 읽어옴.
 정찰과 전투 두 경우가 있다.
*/
void XGameUser::RecvGetUserLegionByIdAcc( XPacket& p )
{
	// Score버전이랑 코드가 같네....나중에 합치자
	ID idSpot;
	ID idAccount;
	ID idPacket=0;
	XArchive arResult;
	XArchive arLegion;
	int ret = 1;
//	XAccount accTarget;
	bool bDummyUser = true;

	do {
		LegionPtr spLegion;
		p >> idSpot;
		p >> idPacket;
		p >> idAccount;		// 매칭으로 찾은 상대편 idAccount;
		XSpot *pBaseSpot = GetpWorld()->GetSpot( idSpot );
		if( XBREAK( pBaseSpot == NULL ) )
		{ ret = 0; break;	}
//		accTarget.SetLevel( pBaseSpot->GetLevel() );
		if( idAccount ) {
			p >> arResult;
			arResult >> arLegion;
			// 이미 정찰된 군대리스트가 있으면 백업 받음.
			std::vector<ID> aryUnfogHeroesSN;	
			if( pBaseSpot->GetspLegion() ) {
				pBaseSpot->GetspLegion()->GetUnFogList( &aryUnfogHeroesSN );
			}
			// 새 정보로 풀음.
			auto pLegion = XLegion::sCreateDeserializeFull( arLegion );
			if( pLegion ) {
#ifdef _CHEAT
//				if( IsDummyUser() )
					// 병과에 따라 앞뒤열로 바꿔준다.(더미유저만 이거 해야함)
					pLegion->AdjustLegion();
#endif // _CHEAT
				// 자원부대를 설정한다.
				SetResourceSquad( pBaseSpot, pLegion );
				// 안개부대를 설정한다.
				SetFogSquad( pLegion, aryUnfogHeroesSN );
// 				std::vector<ID> aryResourceHero;
// 				// 재도전시엔 자원부대를 설정하지 않음.
// 				if( pBaseSpot->GetnumLose() > 0 )
// 					pLegion->ClearResourceSquad();
// 				else {
// 					// 이미 자원부대가 세팅되어있으면 다시 하지 않음.
// 					auto spExistLegion = pBaseSpot->GetspLegion();
// 					if( spExistLegion ) {
// 						aryResourceHero = spExistLegion->GetaryResourceHero();
// 					}
// 					if( aryResourceHero.size() == 0 )
// 						pLegion->SetResourceSquad( 3 );	// 랜덤으로 자원부대를 설정한다.
// 				}
// 				if( aryResourceHero.size() > 0 )
// 					pLegion->SetAryResource( aryResourceHero );
// 				새 군단정보로 바꾸기전에 기존정보의 안개정보를 꺼내서 백업받고
// 				새 군단정보의 안개리스트를 갱신시킨다.
// 				타이밍에 따라 이전 군단리스트와 다를수도 있으므로
// 				안개리스트의 영웅이 없을수 있다. 그경우는 그냥 무시하도록 한다.
// 				if( aryUnfogHeroesSN.size() == 0 ) {
// 					// 정찰된(unfog된)부대가 없었다면 안개를 새로 만들어 준다.
// 					XGAME::xLegionParam legionInfo;
// 					pLegion->MakeFogs( legionInfo.m_numVisible );
// 				} else {
// 					// 이미 정찰된 부대가 있다면 그 정보를 복구시킨다.
// 					pLegion->SetUnFogList( aryUnfogHeroesSN );
// 				}
				spLegion = LegionPtr( pLegion );
				pBaseSpot->SetspLegion( spLegion );
// 				if( aryResourceHero.size() > 0 )
// 					pLegion->SetAryResource( aryResourceHero );

				// 임시코드. DB클리어하면 삭제
#ifdef _DEV
				// 현재로선 더미유저인지 진짜 유저인지 알방법이 없어서 모두 더미라고 함.
// 				if( bDummyUser )
// 					pLegion->DummyDataRechange( pBaseSpot->GetLevel(), m_spAcc );
#endif // _DEV
			}
		} // if( idAccount ) {
		if( spLegion == nullptr ) {
			// 상대 계정이 사라지거나 에러임. 스팟 초기화 해야 함.
			pBaseSpot->Initialize( m_spAcc );
			XGAME::xBattleStartInfo info( false, pBaseSpot );
			info.m_idEnemy = idAccount;
			info.m_bInitSpot = true;		// 스팟 초기화
			SendBattleInfo( pBaseSpot, &info );
		} else {
			if( idPacket == xCL2GS_LOBBY_SPOT_ATTACK ) {
				// 클라에 적부대 정보를 보내줌(전투)
				SendBattleInfoWithidAcc( pBaseSpot, idAccount );
				_tstring strLog = XE::Format( _T( "Spot_Attack_Result(XGAME::xSPOT_CASTLE) Level: %d, SpotID: %d Level: %d, idSpot: %d, Score: %d, Name: %s" )
					, GetLevel(), idSpot, pBaseSpot->GetLevel(), pBaseSpot->GetidSpot(), pBaseSpot->GetScore(), pBaseSpot->GetszName() );
				AddLog( XGAME::xULog_User_Spot_Combat, strLog );
			} else
			if( idPacket == xCL2GS_LOBBY_SPOT_RECON ) {
				// 클라에 적부대 정보를 보내줌(정찰)
				SendReconInfo( true, idAccount, pBaseSpot );
				_tstring strLog = XE::Format( _T( "Spot_Recon_Result(XGAME::xSPOT_CASTLE) Level: %d, SpotID: %d Level: %d, idSpot: %d, Score: %d, Name: %s" )
					, GetLevel(), idSpot, pBaseSpot->GetLevel(), pBaseSpot->GetidSpot(), pBaseSpot->GetScore(), pBaseSpot->GetszName() );
				AddLog( XGAME::xULog_User_Spot_Recon, strLog );
			}
		}
	} while(0);
//	accTarget.Release();
}

/**
 @brief pLegion군단에 자원부대를 설정한다.
*/
void XGameUser::SetResourceSquad( XSpot *pBaseSpot, XLegion *pLegion )
{
	std::vector<ID> aryResourceHero;
	// 재도전시엔 자원부대를 설정하지 않음.
	if( pBaseSpot->GetnumLose() > 0 )
		pLegion->ClearResourceSquad();
	else {
		// 이미 자원부대가 세팅되어있으면 다시 하지 않음.
		auto spExistLegion = pBaseSpot->GetspLegion();
		if( spExistLegion ) {
			aryResourceHero = spExistLegion->GetaryResourceHero();
		}
		if( aryResourceHero.size() == 0 )
			pLegion->SetResourceSquad( 3 );	// 랜덤으로 자원부대를 설정한다.
	}
	if( aryResourceHero.size() > 0 )
		pLegion->SetAryResource( aryResourceHero );
}
/**
 @brief pLegion군단에 안개부대를 설정한다.
 @param aryUnfogHeroesSN 기존에 안개부대정보
*/
void XGameUser::SetFogSquad( XLegion *pLegion, const std::vector<ID>& aryUnfogHeroesSN )
{
// 새 군단정보로 바꾸기전에 기존정보의 안개정보를 꺼내서 백업받고
// 새 군단정보의 안개리스트를 갱신시킨다.
// 타이밍에 따라 이전 군단리스트와 다를수도 있으므로
// 안개리스트의 영웅이 없을수 있다. 그경우는 그냥 무시하도록 한다.
	if( aryUnfogHeroesSN.size() == 0 ) {
		// 정찰된(unfog된)부대가 없었다면 안개를 새로 만들어 준다.
		XGAME::xLegionParam legionInfo;
		pLegion->MakeFogs( legionInfo.m_numVisible );
	} else {
		// 이미 정찰된 부대가 있다면 그 정보를 복구시킨다.
		pLegion->SetUnFogList( aryUnfogHeroesSN );
	}
}

/**
 스폰타이머 발생
*/
void XGameUser::DelegateOnSpawnTimer( XSpot *pBaseSpot, xSec secAdjustOffline )
{
	// 
// 	if( pBaseSpot->GettypeSpot() != XGAME::xSPOT_CASTLE ) {
		bool bNoAttack = pBaseSpot->IsNoAttack();
		pBaseSpot->ClearSpot();
		// 노어택 속성은 스폰이 새로 되더라도 지워지지 않음.
//		pBaseSpot->GetbitFlag().noAttack = bNoAttack;
		pBaseSpot->SetbitFlagByNoAttack( bNoAttack );
// 	}
	//
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		if( XBREAK( pSpot == NULL ) )
			return;
		SendSpotSync( pSpot );
	//	pBaseSpot->OnSpawn( m_spAcc );		// 유저상대스팟의 경우는 실제 매치가 된후 호출되게 바뀜.
		if( pSpot->GetidOwner() == m_spAcc->GetidAccount() ) {
			_tstring strLog = XE::Format(_T("Spot_Recon_DelegateOnSpawnTimer_Req(XGAME::xSPOT_CASTLE) User_Level: %d,idSpot: %d"), GetLevel(), pSpot->GetidSpot());
			AddLog(XGAME::xULog_User_Spot_Recon, strLog);
//			DoMatchUserSpot(pBaseSpot, secAdjustOffline);
		}
			
	} else
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_SULFUR ) {
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		if( XBREAK( pSpot == NULL ) )
			return;
		pBaseSpot->OnSpawn( m_spAcc );
		//
		XPacket ar( (ID)xCL2GS_LOBBY_SULFUR_SPAWN );
		ar << pSpot->GetidSpot();
		XSpot::sSerialize( ar, pSpot );
		Send( ar );

		_tstring strLog = XE::Format(_T("Spot_Recon_DelegateOnSpawnTimer(XGAME::xSPOT_SULFUR) idSpot: %d"), pSpot->GetidSpot());
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);

	} else
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_NPC )
	{
		auto pSpot = SafeCast<XSpotNpc*>( pBaseSpot );
		if( XBREAK( pSpot == NULL ) )
			return;
		pBaseSpot->OnSpawn( m_spAcc );
		//
		XPacket ar( (ID)xCL2GS_LOBBY_NPC_SPAWN );
		ar << pSpot->GetidSpot();
		XSpot::sSerialize( ar, pSpot );
		Send( ar );

		_tstring strLog = XE::Format(_T("Spot_Recon_DelegateOnSpawnTimer(XGAME::xSPOT_NPC) idSpot: %d"), pSpot->GetidSpot());
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);
	} else
// 	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_SPECIAL )
// 	{
// 		auto pSpot = SafeCast<XSpotSpecial*>( pBaseSpot );
// 		if( XBREAK(pSpot == NULL) )
// 			return;
// 		pBaseSpot->OnSpawn( m_spAcc );
// 		// 입장횟수가 1회 충전되면 발생한다.
// 		XPacket ar( (ID)xCL2GS_UPDATE_SPECIAL_SPOT );
// 		ar << pBaseSpot->GetidSpot();
// 		ar << pBaseSpot->GetsnSpot();
// 		ar << pSpot->GetnumEnterTicket();
// 		Send( ar );
// 
// 		_tstring strLog = XE::Format(_T("Spot_Recon_DelegateOnSpawnTimer(XGAME::xSPOT_SPECIAL) idSpot: %d"), pSpot->GetidSpot());
// 		AddLog(XGAME::xULog_User_Spot_Recon, strLog);
// 	} else
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASH )
	{
		auto pSpot = SafeCast<XSpotCash*>( pBaseSpot );
		if( XBREAK( pSpot == NULL ) )
			return;
		pBaseSpot->OnSpawn( m_spAcc );
		SendSpotSync( pSpot );
		//
		_tstring strLog = XE::Format( _T( "Spot_Recon_DelegateOnSpawnTimer(XGAME::xSPOT_CASH) idSpot: %d" ), pSpot->GetidSpot() );
		AddLog( XGAME::xULog_User_Spot_Recon, strLog );
	} else {
		XBREAK(1);	// 그외에 스팟은 타이머가 있을필요 없다.
	}

}

/**
 클라로부터 어떤 스팟의 지역창고를 수거하겠다는 요청이 옴.
*/
int XGameUser::RecvSpotCollect( XPacket& p )
{
	ID idSpot;
	DWORD dw0;
	XGAME::xtSpot typeSpot;

	p >> idSpot;
	p >> dw0;	typeSpot = (XGAME::xtSpot)dw0;
	XSpot *pSpot = GetpWorld()->GetSpot(idSpot);
	XVERIFY_BREAK( pSpot == NULL );
	XVERIFY_BREAK( pSpot->GettypeSpot() != typeSpot );
	// 일단 수거누른 시점까지 자원을 정산시켜서 지역창고에 쌓는다.
	pSpot->CalculateResource( m_spAcc->GetidAccount(), 0 );
	// 지역창고의 자원을 중앙창고로 옮긴다.
	XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> ary;
	pSpot->MoveResourceLocalToMain( m_spAcc, &ary );
	//
	XPacket ar( (ID)xCL2GS_LOBBY_SPOT_COLLECT );
	ar << idSpot;
	ar << (DWORD)typeSpot;
	ar << ary.size();
	for( int i = 0; i < ary.size(); ++i )
	{
		ar << (DWORD)ary[i].type;
		ar << ary[i].num;
	}
	m_spAcc->SerializeResource( ar );
	Send( ar );

	return 1;
}

/**
 @brief 전투전 적부대 정보를 보내줌
 모든 전투 공통으로 사용해야 한다.
*/
void XGameUser::SendReconInfo( bool bRecon, 
																ID idAccEnemy, 
																XSpot *pSpot, 
																XArchive *pParam )
{
	XASSERT( bRecon == true );		// 이제 이 함수는 정찰 전용임. 전투정보를 보내려면 SendBattleInfo를 사용
	// 전투직전 핸들러.
	pSpot->OnBeforeBattle( m_spAcc );
	// 클라에 보내주기전 전투세션을 저장한다.
//	ID snSession = 0;
	//
	XBREAK( pSpot->GetstrName().empty() );
	XBREAK( pSpot->GetLevel() == 0 );
// 	XBREAK( !bRecon && snSession == 0 );		// // 전투는 상대가 npc든 플레이어든 세션아이디는 무조건 있어야 함.
	XGAME::xBattleStartInfo info( true, pSpot );
	info.m_idEnemy = idAccEnemy;
// 	info.m_snSession = snSession;
	info.m_snSession = 0;		// 정찰이므로 0임.
	info._SetAPs( m_spAcc->GetAP(), m_spAcc->GetmaxAP() );
	if( pParam )
		info.m_arParam = *pParam;
	//
	XPacket ar( (ID)xCL2GS_SPOT_INFO );		// 정찰결과 패킷일수도 있고 전투정보 패킷일수도 있다.
	info.Serialize( ar, pSpot );
	Send( ar );
}

/**
 @brief 정찰에 사용하지 않음. 전투에만 사용.
*/
void XGameUser::SendBattleInfo( XSpot *pSpot
															, XGAME::xBattleStartInfo *pInfo )
{
	XBREAK( pSpot == nullptr );
	XBREAK( pInfo == nullptr );
	// 전투직전 핸들러.
	pSpot->OnBeforeBattle( m_spAcc );
	// 클라에 보내주기전 전투세션을 저장한다.
	ID snSession = 0;
	// 
	if( XASSERT( pSpot->GetspLegion() != nullptr && !pInfo->m_bRecon) ) {
		snSession = m_spAcc->SetBattleSession( 0,      // auto generate
																						pSpot->GetspLegion(),
																						pInfo->m_idEnemy,
																						pSpot->GetidSpot() );
	}
	//
	XBREAK( snSession == 0 );
	pInfo->m_snSession = snSession;
	if( pInfo->m_Level == 0 )
		pInfo->m_Level = pSpot->GetLevel();
	if( pInfo->m_strName.empty() )
		pInfo->m_strName = pSpot->GetstrName();
	if( pInfo->m_Power == 0 )
		pInfo->m_Power = pSpot->GetPower();
	if( pInfo->m_Ladder == 0 )
		pInfo->m_Ladder = pSpot->GetScore();
	if( pInfo->m_idSpot == 0 )
		pInfo->m_idSpot = pSpot->GetidSpot();
	if( !pInfo->m_typeSpot )
		pInfo->m_typeSpot = pSpot->GettypeSpot();
	pInfo->_SetAPs( m_spAcc->GetAP(), m_spAcc->GetmaxAP() );
	//
	XPacket ar( (ID)xCL2GS_SPOT_INFO );		// 정찰결과 패킷일수도 있고 전투정보 패킷일수도 있다.
	pInfo->Serialize( ar, pSpot );
	Send( ar );
}

/**
 @brief 전투정보 보내기(약식)
 vs 유저용
*/
void XGameUser::SendBattleInfoWithidAcc( XSpot *pSpot, ID idAcc, const _tstring& strName )
{
	XGAME::xBattleStartInfo info( false, pSpot );
	info.m_idEnemy = idAcc;
	info.m_strName = strName;
	SendBattleInfo( pSpot, &info );
}

void XGameUser::SendBattleInfoWithidAccParam( XSpot *pSpot, ID idAcc, XArchive* pArParam )
{
	XGAME::xBattleStartInfo info( false, pSpot );
	info.m_idEnemy = idAcc;
	if( pArParam )
		info.m_arParam = *pArParam;
	SendBattleInfo( pSpot, &info );
}

/**
 레벨,경험치,레벨업정보등을 동기화 한다.
*/
void XGameUser::SendLevelSync( void )
{
	XPacket ar( (ID)xCL2GS_LEVEL_SYNC );
	m_spAcc->SerializeLevel( ar );
//	m_spAcc->SerializeTechPoint( ar );
	ar << (WORD)m_spAcc->GetAP();
	ar << (WORD)m_spAcc->GetmaxAP();
	ar << m_spAcc->GetPowerIncludeEmpty();
	ar << m_spAcc->GetPowerExcludeEmpty();
	m_spAcc->SerializeUnlockPoint( ar );
	Send( ar );
}

void XGameUser::SendBaseInfo( void )
{
	XPacket ar( (ID)xCL2GS_SYNC_BASE_INFO );
	m_spAcc->SerializeLevel( ar );
//	m_spAcc->SerializeTechPoint( ar );
	ar << m_spAcc->GetGold();
	ar << m_spAcc->GetCashtem();
//	ar << m_spAcc->GetptBrave();
	ar << m_spAcc->GetPowerIncludeEmpty();
	ar << m_spAcc->GetPowerExcludeEmpty();
	ar << m_spAcc->GetLadder();
	m_spAcc->SerializeResource( ar );
	m_spAcc->SerializeUnlockPoint( ar );
	Send( ar );
}
void XGameUser::SendSyncResource( void )
{
	XPacket ar( (ID)xCL2GS_SYNC_RESOURCE );
	m_spAcc->SerializeResource( ar );
	Send( ar );
}

void XGameUser::SendCancelKill()
{
	XPacket ar( (ID)xCL2GS_CANCEL_KILL );
	ar << (short)m_spAcc->GetAP();
	ar << (short)m_spAcc->GetmaxAP();
	Send( ar );
}

/**
 DB서버로부터 매칭된 결과가 도착함.
*/
void XGameUser::RecvReqMatchMandrake( XPacket& p )
{
	int idxMandrake, level, win, reward, power, score, idxLegion;
	ID snSpot, idEnemy;
	_tstring strName, strFbUserId, strHello;
	XArchive arDB;
	p >> idxMandrake;
	p >> snSpot;
	p >> arDB;
	arDB >> idEnemy;
	if( idEnemy ) {
		arDB >> strName;
		arDB >> level;
		arDB >> win;
		arDB >> reward;
		arDB >> idxLegion;
		arDB >> power;
		arDB >> score;
		arDB >> strFbUserId;
		arDB >> strHello;
	}
	//
	auto pSpot = GetpWorld()->GetSpotMandrakeByIdx( idxMandrake );
	if( XBREAK( pSpot == NULL ) )
		return;
	if( XBREAK( pSpot->GetsnSpot() != snSpot ) )
		return;
	pSpot->ClearSpot();
	pSpot->OnSpawn( m_spAcc );
	if( idEnemy ) {
		pSpot->SetPlayerOwner( idEnemy, strName.c_str() );
		pSpot->SetidMatchEnemy( GetidAcc() );		// 적은 내가 된다.(이거 사용하나)
		pSpot->SetLevel( level );
		pSpot->SetWin( win );
		pSpot->SetReward( reward );
		pSpot->SetScore( score );
		pSpot->SetPower( power );
		pSpot->SetstrcFbUserId( SZ2C(strFbUserId) );
		pSpot->SetstrHello( strHello );
// 		pSpot->SetPlayerOwner( idEnemy, 
// 							strName.c_str(), 
// 							GetidAccount(),		// idEnemy, 적은 내가 된다.
// 							level, 
// 							win, 
// 							reward, 
// 							score );
// 		pSpot->SetPower( power );
		// 장비템 하나를 드랍시키고 그것을 스팟에 설치한다.
// 		auto pPropItem = XSpot::sDoDropEquip( m_spAcc->GetLevel() );
// 		if( pPropItem )
// 			pSpot->SetidDropItem( pPropItem->idProp );
// 		else
// 			pSpot->SetidDropItem( 0 );
		_tstring strLog = XE::Format(_T("Spot_Recon_Result_Enemy(XGAME::xSPOT_MANDRAKE) Name: %s, Level: %d, Win:%d, Score: %d"), strName.c_str(), level, win, score );
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);

	} else
	{
		// 매칭상대를 못찾음.
		pSpot->SetPlayerOwner( GetidAcc(), GetstrName() );
		pSpot->SetidMatchEnemy( 0 );
		pSpot->SetLevel( m_spAcc->GetLevel() );
		pSpot->SetWin( 0 );
		pSpot->SetScore( m_spAcc->GetLadder() );
		pSpot->SetPower( m_spAcc->GetPowerIncludeEmpty() );
		pSpot->SetReward( 1000 );
// 		pSpot->SetPlayerOwner( GetidAccount(), 
// 								GetstrName(), 
// 								0,	// idEnemey
// 								m_spAcc->GetLevel(), 
// 								0,	// win
// 								0,	// reward
// 								m_spAcc->GetLadder() );
	
		_tstring strLog = XE::Format(_T("Spot_Recon_Result_Enemy(XGAME::xSPOT_MANDRAKE) None, User_Score: %d"), m_spAcc->GetLadder());
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);
	}
	Save();
	// 클라로 결과 보내줌.
	XPacket ar( (ID)xCL2GS_LOBBY_MANDRAKE_MATCH_RESULT );
	ar << idxMandrake;
	XSpot::sSerialize( ar, pSpot );
//	ar << arDB;
	Send( ar );
}

/**
 DBA서버로부터 만드레이크 스팟 군대정보가 도착함.
*/
void XGameUser::RecvGetMandrakeLegionByIdAcc( XPacket& p )
{
	int idxMandrake;
	ID snSpot, idEnemy, idPacket;
	XArchive arDB;
	p >> idxMandrake;
	p >> snSpot;
	p >> idEnemy;
	auto pSpot = GetpWorld()->GetSpotMandrakeByIdx( idxMandrake );
	if( XBREAK( pSpot == NULL ) )
		return;
	if( XBREAK( pSpot->GetsnSpot() != snSpot ) )
		return;
	if( idEnemy )	{
		p >> idPacket;
		p >> arDB;
		int level, win, reward, legion;
		_tstring strName;
		arDB >> level;
		arDB >> strName;
		arDB >> win >> reward >> legion;
		XArchive arLegion, arAbil;
		arDB >> arLegion;
		arDB >> arAbil;
		auto pLegion = XLegion::sCreateDeserializeFull( arLegion );
		pSpot->SetpLegion( pLegion );
		bool bBattle = ( idPacket != xCL2GS_LOBBY_MANDRAKE_SPOT_RECON );
		ID snSession = 0;
		if( bBattle ) {
			snSession = m_spAcc->SetBattleSession( 0
																							, pSpot->GetspLegion()
																							, idEnemy
																							, pSpot->GetidSpot() );
		}
		_tstring strLog = 
		XE::Format(_T("Spot_Recon_Result_Legion(XGAME::xSPOT_MANDRAKE) Name: %s, Level: %d, Win: %d, reward: %d, legionidx: %d")
																																, strName.c_str(), level, win, reward, legion);
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);

		// 군단정보 클라에 보내줌.
		{
			XPacket ar( (ID)idPacket );
			ar << idEnemy;
			ar << idxMandrake;
			ar << snSpot;
			ar << pSpot->GetidSpot();
			ar << snSession;
// 			if( idPacket == xCL2GS_LOBBY_MANDRAKE_SPOT_RECON )
// 				ar << 0;
// 			else
// 				ar << 1;	// xCL2GS_LOBBY_MANDRAKE_BATTLE_INFO
			ar << arDB;
			Send( ar );
		}
	} else
	{
		// idEnemy가 에러나서 스팟을 내껄로 복구함.
		pSpot->SetPlayerOwner( GetidAcc(), GetstrName() );
		pSpot->SetidMatchEnemy( 0 );
		pSpot->SetLevel( m_spAcc->GetLevel() );
		pSpot->SetWin( 0 );
		pSpot->SetScore( m_spAcc->GetLadder() );
		pSpot->SetPower( m_spAcc->GetPowerIncludeEmpty() );
		pSpot->SetReward( 1000 );
// 		pSpot->SetPlayerOwner( GetidAccount(), 
// 								GetstrName(), 
// 								0, 
// 								m_spAcc->GetLevel(), 
// 								0, 
// 								0, 
// 								m_spAcc->GetLadder() );
		_tstring strLog = 
		XE::Format(_T("Spot_Recon_Result_Enemy(XGAME::xSPOT_MANDRAKE) IdEnemy_Error, User_Level: %d, User_Score: %d")
																					, m_spAcc->GetLevel(), m_spAcc->GetLadder());
		AddLog(XGAME::xULog_User_Spot_Recon, strLog);

		// 클라이언트 동기화
	}
}

/**
 클라가 구름 오픈을 요청했다.
*/
int XGameUser::RecvOpenCloud( XPacket& p )
{
	char c0;
	ID idCloud;
	p >> idCloud;
	p >> c0;		auto termsPay = (xtTermsPayment)c0;
	p >> c0 >> c0 >> c0;
	XVERIFY_BREAK( idCloud == 0 );
	auto pProp = PROP_CLOUD->GetpProp( idCloud );
	XVERIFY_BREAK( pProp == NULL );
	//
	xtError err = xE_UNKNOWN;
	do {
		int costGold = pProp->cost;
		int costCash = 0;
		// 비동기 상황을 고려하여 에러처리
		err = m_spAcc->IsOpenableCloud( pProp );
		// 돈이 모자라는 상황이면 필요한 비용을 얻어낸다.
		if( err == xE_NOT_ENOUGH_GOLD ) {
			err = GetPaymentCost( costGold, err, termsPay, &costGold, &costCash );
			if( err == xE_NOT_ENOUGH_GOLD ) {
				// 지불방법에 따라 금화 부족한게 해결되었을수 있으니 다시한번 오픈가능여부를 검사해봐야 함.
				int goldHave = pProp->cost;		// 골드부족에러가 안나도록 해서 다시한번 검사.
				err = m_spAcc->IsOpenableCloudWithHaveGold( pProp, goldHave );
				XVERIFY_BREAK( err == xE_NOT_ENOUGH_GOLD );	// 이게 다시 나올순 없다.
				// 이래도 성공을 못했다면 에러를 클라에 반송함.
				if( err != xE_OK )
					break;
			}
		}
		if( err != xE_OK )		// 골드/캐시 비동기로 인해 안맞을수 있어서 XVERIFY를 사용하지 않음.
			break;
		if( !pProp->idsItem.empty() ) {
			auto bOk = m_spAcc->DestroyItem( pProp->idsItem.c_str(), 1 );
			XVERIFY_BREAK( !bOk );	// 위에서 검사했으므로 에러가 나면 안됨.
		}
		GetpWorld()->DoOpenCloud( idCloud );
		m_spAcc->SetlvLimit( pProp->lvArea + 1 );		// 오픈한 지역레벨을 한계레벨로 설정
	
		// 지역레벨에 따라 기능의 잠금해제를 한다. 
		BIT bitUnlock = UnlockMenu( pProp, pProp->lvArea );
		//
		if( costGold > 0 )
			m_spAcc->AddGold( -costGold );
		if( costCash > 0 )
			m_spAcc->AddCashtem( -costCash );
		// 지역 오픈 보너스
		m_spAcc->AddCashtem( 5 );
	
		// 구름이 걷힘에 따라 그 밑에 있던 스팟이 나타나게 함.
		CreateSpotsByCloud( pProp, TRUE );
		// 구름이 걷힐때 그 밑에 스팟들에게 이벤트를 보냄.
		GetpWorld()->DispatchSpotEvent( pProp );
		// 퀘스트시스템에 이벤트 보냄.
		xQuest::XEventInfo infoQuest;
		infoQuest.SetidArea( idCloud );
		DispatchQuestEvent( XGAME::xQC_EVENT_OPEN_AREA, infoQuest );
	
		SendUnlockMenu( (XGAME::xtMenus)bitUnlock );
		// 유닛언락 포인트 받아야 하는 레벨의 지역이 열리면 포인트를 지급한다.
		if( pProp->unitUnlock ) {
			if( pProp->unitUnlock == XGAME::xUNIT_ANY ) {
				int point = m_spAcc->GetnumUnlockTicketForMiddleOrBig();
				m_spAcc->SetnumUnlockTicketForMiddleOrBig( point + 1 );
			} else 
			if( pProp->unitUnlock == XGAME::xUNIT_PALADIN )	{
				m_spAcc->SetbUnlockTicketForPaladin( true );
			} else {
				XBREAK(1);
			}
		}
		err = xE_OK;
	} while (0);
	// 클라에결과 보냄
	SendResultOpenCloud( idCloud, err, termsPay );
//	{
// 		XBREAK( m_spAcc->GetlvLimit() > 0xff );
// 		XPacket ar( (ID)xCL2GS_LOBBY_OPEN_CLOUD );
// 		ar << idCloud;
// 		ar << (char)err;
// 		ar << (char)m_spAcc->GetlvLimit();
// 		ar << (char)0;
// 		ar << (char)0;
// 		ar << m_spAcc->GetGold();
// 		ar << (int)m_spAcc->GetCashtem();
// 		m_spAcc->SerializeUnlockPoint( ar );
// // 		ar << m_spAcc->GetlvLimit();
// 		Send( ar );
//	}
	
	_tstring strLog = XE::Format(_T("Cloud Open Level: %d, CloudID: %d"), GetLevel(), idCloud);
	AddLog(XGAME::xULog_Cloud_Open, strLog);
	return 1;
}

xtError XGameUser::IsAblePayment( int goldCostOpen
																, xtError err
																, xtTermsPayment termsPay
																, int* pOutGold, int* pOutCash )
{
	int costGold = *pOutGold;
	int costCash = *pOutCash;
	XBREAK( err == xE_UNKNOWN );
	do 
	{
		// 골드를 지불하고 모자르는 골드는 캐시로지불
		if( termsPay == xTP_GOLD_AND_CASH ) {
			costGold = (int)( goldCostOpen - m_spAcc->GetGold() );
			if( costGold <= 0 )
				break;
			// 모자르는 금화를 캐시로 지불할수 있는지
			costCash = m_spAcc->GetCashFromGold( costGold );
			if( !m_spAcc->IsEnoughCash( costCash ) ) {
				err = xE_NOT_ENOUGH_CASH;
				break;
			}
		} else
		if( termsPay == xTP_CASH ) {
			// 100%캐시로 지불할수 있는지.
			costCash = m_spAcc->GetCashFromGold( goldCostOpen );
			if( !m_spAcc->IsEnoughCash( costCash ) ) {
				err = xE_NOT_ENOUGH_CASH;
				break;
			}
		} else {
			XBREAKF( 1, "unknown termsPay:%d", termsPay );
			err = xE_ERROR_CRITICAL;
		}
	} while (0);
	*pOutGold = costGold;
	*pOutCash = costCash;
	return err;
}

/**
 @brief 지불방법에 따라 소모될 gold와 cash의 양을 돌려준다.
*/
xtError XGameUser::GetPaymentCost( int goldCost
																, xtError err
																, xtTermsPayment termsPay
																, int* pOutGold, int* pOutCash )
{
	if( XASSERT( err == xE_NOT_ENOUGH_GOLD ) ) {
		if( termsPay == xTP_GOLD ) {	// 이건그냥 돈부족로 인한 실패.
		} else {
			err = IsAblePayment( goldCost, err, termsPay, pOutGold, pOutCash );
		}
	}
	return err;
}

void XGameUser::SendResultOpenCloud( ID idCloud, xtError err, XGAME::xtTermsPayment termsPay )
{
	XBREAK( m_spAcc->GetlvLimit() > 0xff );
	XPacket ar( (ID)xCL2GS_LOBBY_OPEN_CLOUD );
	ar << idCloud;
	ar << (char)err;
	ar << (char)m_spAcc->GetlvLimit();
	ar << (char)termsPay;
	ar << (char)0;
	ar << m_spAcc->GetGold();
	ar << (int)m_spAcc->GetCashtem();
	m_spAcc->SerializeUnlockPoint( ar );
	Send( ar );
}

/**
 @brief 지역레벨(lvArea)에 따라 기능을 잠금해제 한다.
*/
BIT XGameUser::UnlockMenu( XPropCloud::xCloud *pArea, int lvArea )
{
	BIT bitUnlock = 0;
	if( pArea->strIdentifier == _T("area.labor") )
		bitUnlock = XGAME::xBM_LABORATORY;
	else if( pArea->strIdentifier == _T( "area.001" ) ) {
		bitUnlock = XGAME::xBM_CATHEDRAL;
//		m_spAcc->SetbUnlockTicketForPaladin( true );	// 기사잠금해제 할수 있는 포인트 줌.
	} else if( pArea->strIdentifier == _T( "area.market" ) )
		bitUnlock = XGAME::xBM_MARKET;
	else if( pArea->strIdentifier == _T( "area.tavern" ) )
		bitUnlock = XGAME::xBM_TAVERN;
	else if( pArea->strIdentifier == _T( "area.002" ) )
		bitUnlock = XGAME::xBM_ACADEMY;
	else if( pArea->strIdentifier == _T( "area.1004" ) )
		bitUnlock = XGAME::XBM_UNIT_HANGOUT;
	else if( pArea->strIdentifier == _T( "area.1013" ) )
		bitUnlock = XGAME::xBM_EMBASSY;
// 	else if( pArea->strIdentifier == _T( "area.1012" ) )
// 		bitUnlock = XGAME::xBM_TRAING_CENTER;
	if( bitUnlock )
		m_spAcc->GetbitUnlockMenu().SetBit( bitUnlock );
	return bitUnlock;
}

void XGameUser::SendUnlockMenu( XGAME::xtMenus bitUnlock )
{
	XPacket ar( (ID)xCL2GS_UNLOCK_MENU );
	ar << static_cast<BIT>( bitUnlock );
	m_spAcc->GetbitUnlockMenu().Serialize( ar );
	m_spAcc->SerializeUnlockPoint( ar );
	Send( ar );
}
/**
 @brief 영웅에게 책 제공
*/
// int XGameUser::RecvAddHeroExp(XPacket& p)
// {
// 	ID snHero;
// //	ID idWindow = 0;
// 	ID dw0;
// 	XArrayN<ID,5> aryItems;
// 	BOOL Flag = FALSE;
// 	p >> dw0;	//idWindow;
// 	p >> snHero;
// 	XVERIFY_BREAK(snHero == 0);
// 	p >> aryItems;
// 	XHero *pHero = m_spAcc->GetHero( snHero );
// 	XVERIFY_BREAK( pHero == NULL );
// 	XVERIFY_BREAK( pHero->IsMaxLevel(XGAME::xTR_LEVEL_UP) );
// 	XVERIFY_BREAK( pHero->GetLevel() >= m_spAcc->GetLevel() );
// 	XINT64 sumExp = 0;
// 	XARRAYN_LOOP( aryItems, ID, snItem ) {
// 		if( snItem ) {
// 			XBaseItem *pItem = m_spAcc->GetItem( snItem );
// 			XVERIFY_BREAK( pItem == nullptr );
// 			XBREAK( pItem->GetBookExp() <= 0 );
// 			sumExp += pItem->GetBookExp();
// 		}
// 	} END_LOOP;
// 	// 수동모드에서는 exp가 만땅이되어도 바로 렙업되지 않는다.
// 	pHero->GetXFLevelObj().SetbAutoLevelup( false );	// 수동모드로 바꾼다.
// 	XARRAYN_LOOP( aryItems, ID, snItem ) {
// 		if( snItem ) {
// 			XBaseItem *pItem = m_spAcc->GetItem( snItem );
// 			XVERIFY_BREAK( pItem == nullptr );
// 			XBREAK( pItem->GetBookExp() <= 0 );
// 			pHero->AddExp( pItem->GetBookExp() );
// 			m_spAcc->DestroyItemBySN( pItem->GetsnItem() );
// 		}
// 	} END_LOOP;
// 	if( !pHero->IsMaxLevel(XGAME::xTR_LEVEL_UP) 
// 		&& pHero->GetXFLevelObj().IsFullExp() )
// 		pHero->SetbLevelupReady( XGAME::xTR_LEVEL_UP, true );
// 	//결과를 클라에 보냄
// 	XPacket ar((ID)xCL2GS_LOBBY_ADD_HERO_EXP);
// 	ar << Flag;
// 	ar << 0;	//idWindow;
// 	ar << snHero;	
// 	ar << aryItems;
// 	pHero->SerializeLevel( ar );
// 	pHero->SerializeLevelupReady( ar );
// 	Send(ar);
// 	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_PROVIDE_BOOK );
// 	SendBaseInfo();		// 편의상 만든 함수. 계정의 기본값들을 보내준다.
// 
// 	return 1;
// }

int XGameUser::RecvChageHeroLegion(XPacket& p)
{
	ID snHero;
	ID idUnit;
	ID idWindow;

	p >> idWindow;
	p >> snHero;
	XVERIFY_BREAK(snHero == 0);
	p >> idUnit;
	XVERIFY_BREAK(idUnit == 0);

	XHero *pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == NULL );
	XGAME::xtUnit unit = (XGAME::xtUnit) idUnit;
	pHero->SetUnit( unit );
// 	auto& propSquad = PROP_SQUAD->GetTable( pHero->GetlevelSquad() );
// 	int numUnit = 0;// = XLegion::sGetNumUnitByLevel( unit, m_spAcc->GetLevel() );
// 	switch( XGAME::GetSizeUnit( pHero->GetUnit() ) )
// 	{
// 	case XGAME::xSIZE_SMALL:
// 		numUnit = propSquad.maxSmall;
// 		break;
// 	case XGAME::xSIZE_MIDDLE:
// 		numUnit = propSquad.maxMiddle;
// 		break;
// 	case XGAME::xSIZE_BIG:
// 		numUnit = propSquad.maxBig;
// 		break;
// 	}
//	pHero->SetnumUnit( numUnit );

	//결과를 클라에 보냄
	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_HERO_LEGION);
	ar << idWindow;
	ar << snHero;
	ar << idUnit;
	ar << pHero->GetnumUnit();
	Send(ar);

	return 1;
}

int XGameUser::RecvChageHeroEquip(XPacket& p)
{
	ID snHero = 0;
	ID snItem = 0;
	ID typeAction = 0;
	p >> snHero;
	p >> snItem;
	p >> typeAction;
	BOOL bSuccess = TRUE;
	XVERIFY_BREAK(snHero == 0);
	XVERIFY_BREAK(snItem == 0);
	XHero* pHero = m_spAcc->GetHero(snHero);
	if (pHero) {			
		XBaseItem* pItem = m_spAcc->GetItem(snItem);
		if (pItem) {
			if (typeAction > 0) {
				pHero->SetEquip(pItem);
				DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_EQUIP );
			} else
				pHero->SetUnequip(pItem);
		} else
			bSuccess = FALSE;
	} else
		bSuccess = FALSE;
	//
	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_HERO_EQUIPMENT);	
	ar << snHero;
	if( bSuccess )
		ar << snItem;
	else
		ar << 0;
	ar << typeAction;
	Send(ar);

	return 1;
}
/**
 pProp구름이 걷힐때 그 밑에 연결되어있던 스팟들을 생성한다.
 겹쳐있는 스팟도 있으므로 중복해서 생성하지 않도록 한다.
*/
void XGameUser::CreateSpotsByCloud( XPropCloud::xCloud *pProp, 
									BOOL bSyncClient )
{
	XArrayLinearN<XSpot*, 1024> arySpots;
	m_spAcc->CreateSpotsByCloud( pProp, this, &arySpots );
	//
	XARRAYLINEARN_LOOP_AUTO( arySpots, pBaseSpot ) {
		if( pBaseSpot && bSyncClient )
			SendSpotSync( pBaseSpot );
	} END_LOOP;
}



/**
 @brief 클라이언트의 요청으로 영웅 1명을 소환한다.
 현재는 임시로 아무거나 하나 소환시킨다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSummonHero()
*/
int XGameUser::RecvSummonHero( XPacket& p )
{
	int i0;
	XGAME::xtGatha type;
	p >> i0;	type = (XGAME::xtGatha) i0;		// 저급/고급(금화)/고급(캐쉬)
	XVERIFY_BREAK( type <= XGAME::xGA_NONE || type >= XGAME::xGA_MAX );
	// 소환가격 검사.
	int gold = m_spAcc->GetGoldNormalSummon();
	if( type == XGAME::xGA_LOW_GOLD ) {
		if( m_spAcc->GetFlagTutorial().bSummonHero ) {
			// 첫번째 소환을 한 이후에만 가격을 매김.
			if( m_spAcc->IsNotEnoughGold( gold ) ) {
				_tstring strLog = XE::Format( _T( "Sommon_Gatcha_Failed:Type: %d, Require: %d, Have: %d" ), type, gold, m_spAcc->GetGold() );
				AddLog( XGAME::xULog_Hero_Gatcha_Normal, strLog );
				XVERIFY_BREAK( m_spAcc->IsNotEnoughGold( gold ) );
			}
		}
	} else
	if( type == XGAME::xGA_HIGH_CASH ) {
		const int cash = XGC->GetcashHighSummon();
		if( m_spAcc->IsNotEnoughCash( cash ) ) {
			_tstring strLog = XE::Format( _T( "Sommon_Gatcha_Failed:Type:%d, Require:%d, HaveGold:%d, HaveCash:%d" ), type, cash, m_spAcc->GetGold(), m_spAcc->GetCashtem() );
			AddLog( XGAME::xULog_Hero_Gatcha_Royal, strLog );
			XVERIFY_BREAK( m_spAcc->IsNotEnoughCash( cash ) );
		}
	}
/*
	가챠로 뽑을수 있는 모든 영웅리스트중에서 랜덤으로 하나 고름.
	1~3등급까지의 가챠테이블을 준비
	주사위를 굴려서 등급을 결정
	if( 그 영웅을 아직 가지고 있지 않다면 )
		영웅생성
		주사위로 결정한 등급을 입력함.
	else
		영웅의 영혼석을 생성
		개수는 주사위로 결정한 등급의 누적 영혼석 개수.
*/
	XPropHero::xPROP *pProp = nullptr;
	// 영웅수 5명까지는 영혼석이 나오지 않게 한다.
	const int numHeroes = m_spAcc->GetNumHeroes();
	const bool bNoPiece = (numHeroes <= 5);
	if( bNoPiece ) {
		// 영혼석 나오지 않음.
		if( m_spAcc->GetFlagTutorial().bSummonHero == 0 ) {
			// 최초소환시에는 무조건 스피드영웅을 뽑도록 한다.
			pProp = PROP_HERO->GetpPropRandomByGetType( XGAME::xGET_GATHA, XGAME::xAT_SPEED );
		} else {
			// 가챠로 얻을수 있는 영웅중에서 현재 보유하지 않은 영웅중에서 하나를 랜덤으로 얻는다.
			XVector<ID> aryExclude;
			m_spAcc->GetaryidPropHeroByInven( &aryExclude );
			XVector<XPropHero::xPROP*> ary;
			PROP_HERO->GetpPropByGetToAryWithExclude( &ary, XGAME::xGET_GATHA, aryExclude );
			pProp = ary.GetFromRandom();
// 			pProp = PROP_HERO->GetpPropRandomByGetType( XGAME::xGET_GATHA );
		}
	} else {
		// 영혼석도 나옴
		// 가챠로 얻을수 있는 영웅중에서 하나를 랜덤으로 얻는다.
		pProp = PROP_HERO->GetpPropRandomByGetType( XGAME::xGET_GATHA );
	}
	if( XBREAK(pProp == nullptr) )
		return 0;
	// 확률테이블에서 주사위를 굴려 등급을 결정한다.
	int idx = -1;
	if( type == XGAME::xGA_LOW_GOLD ) {
		idx = XE::GetDiceChance( XGC->m_aryGachaChanceNormal );
	} else 
// 	if( type == XGAME::xGA_HIGH_GOLD ) {
// 		// 금화로 고급뽑기 
// 		idx = XE::GetDiceChance( XGC->m_aryGachaChancePremium );
// 	} else 
	if( type == XGAME::xGA_HIGH_CASH ) {
		// 캐쉬로 고급뽑기.
		idx = XE::GetDiceChance( XGC->m_aryGachaChancePremium );
	} else {
		XBREAK(1);
	}
	auto grade = (XGAME::xtGrade) idx;
	if( XBREAK( IsInvalidGrade( grade ) ) )
		return 0;
	// 이미 pProp영웅을 가지고 있는지 확인한다.
	bool bPiece = false;
	//결과를 클라에 보냄
	XPacket ar( (ID)xCL2GS_LOBBY_SUMMON_HERO );
	ar << pProp->idProp;
	auto pHeroExist = m_spAcc->GetHeroByidProp( pProp->idProp );
	if( pHeroExist ) {
		XBREAK( bNoPiece == true );
		// 이미있으므로 영혼석으로 지급
		// grade를 소환하기 위한 총 영혼석 수.
		const int num = XGAME::GetNumTotalSoulSummon( grade );
		m_spAcc->CreatePieceItemByidHero( pProp->idProp, num );
		bPiece = true;
		XBREAK( num > 0xffff );
		ar << xboolToByte( bPiece );
		ar << (BYTE)type;
		ar << (WORD)num;
	} else {
		bPiece = false;
		ar << xboolToByte( bPiece );
		ar << (BYTE)type;
		ar << (WORD)0;
		// 없는 영웅이므로 영웅생성후 인벤으로.
		int idx = 0;
		XGAME::xtUnit units[] = {
			XGAME::xUNIT_SPEARMAN,
			XGAME::xUNIT_ARCHER,
			XGAME::xUNIT_PALADIN,
		};
		//지휘 타입에 따른 기본 병력 세팅
		if( pProp->IsRange() )
			idx = 1;
		else if( pProp->IsSpeed() )
			idx = 2;
		auto unit = units[ idx ];	// 최초 등장은 1레벨부터이므로 소형으로 선택됨.
//		int numUnit = 5; //1레벨이니까 기본 숫자제공.
//		XHero *pHero = XHero::sCreateHero( pPropHero, unit, numUnit );
		int lvSuqad = 1;
		XHero *pHero = XHero::sCreateHero( pProp, lvSuqad, unit );
		XVERIFY_BREAK( pHero == nullptr );
		pHero->SetGrade( grade );
		m_spAcc->AddHero( pHero );
		XHero::sSerialize( ar, pHero );
		//
		xQuest::XEventInfo infoQuest;
		infoQuest.SetidHero( pHero->GetidProp() );
		DispatchQuestEvent( XGAME::xQC_EVENT_GET_HERO, infoQuest );
	}
	DispatchQuestEvent( XGAME::xQC_EVENT_HIRE_HERO );
	// 최초 소환시엔 무료
	if( type == XGAME::xGA_LOW_GOLD ) {
		// 최초 소환은 무료
		if( m_spAcc->GetFlagTutorial().bSummonHero == 0 ) {
			// 무료소환
			gold = 0;
		} else {
			m_spAcc->AddGold( -gold );
		}
		m_spAcc->GetFlagTutorial().bSummonHero = 1;
		DispatchQuestEvent( XGAME::xQC_EVENT_HIRE_HERO_NORMAL );
		_tstring strLog = XE::Format( _T( "Sommon_Gatcha_Gold:Type:%d, grade:%d, Require:%d, RemainGold:%d, RemainCash:%d" ), type, grade, gold, m_spAcc->GetGold(), m_spAcc->GetCashtem() );
		AddLog( XGAME::xULog_Hero_Gatcha_Normal, strLog );
	} else
	if( type == XGAME::xGA_HIGH_CASH ) {
		m_spAcc->AddnumHirePremium( 1 );
		const int cash = XGC->GetcashHighSummon();
		m_spAcc->AddCashtem( -cash );
		DispatchQuestEvent( XGAME::xQC_EVENT_HIRE_HERO_PREMIUM );
		//
		_tstring strLog = XE::Format( _T( "Sommon_Gatcha_Cash:Type:%d, grade:%d, Require:%d, RemainGold:%d, RemainCash:%d" ), type, grade, cash, m_spAcc->GetGold(), m_spAcc->GetCashtem() );
		AddLog( XGAME::xULog_Hero_Gatcha_Royal, strLog );
	} else { 
		XBREAK(1); 
	}
// 	if( m_spAcc->GetFlagTutorial().bSummonHero == 0 )
// 		cost = 0;
// 	if( type == XGAME::xGA_LOW_GOLD )
// 		m_spAcc->AddGold( -((int)cost) );
// 	else if (type == XGAME::xGA_HIGH_GOLD)
// 		m_spAcc->AddGold(-((int)cost));
// 	else if ( type == XGAME::xGA_HIGH_CASH ) //이건 캐쉬 가챠
// 		m_spAcc->SetCashtem(m_spAcc->GetCashtem() - cost);
	// 컷씬이벤트
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_HIRE_HERO );
	// 

	ar << m_spAcc->GetGold();
	ar << m_spAcc->GetCashtem();
	Send( ar );
	///< 소환후엔 무조건 한번 저장(좋은거 뽑혔는데 섭다되는경우를 방지, 릴리즈에서만)
#ifndef _DEBUG
	Save();
#endif
	return 1;
}

/**
 @brief 군단편성에서 새 부대를 만들었다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqNewSquad()
*/
int XGameUser::RecvNewSquad( XPacket& p )
{
	ID snHero;
	int idxSlot, idxLegion;
	p >> snHero >> idxSlot >> idxLegion;
	XHero *pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == NULL );
	XLegion *pLegion = m_spAcc->GetLegionByIdx( idxLegion ).get();
	XVERIFY_BREAK( pLegion == NULL );
	XSquadron *pSq = new XSquadron( pHero );
	XVERIFY_BREAK( pSq == NULL );
	pLegion->AddSquadron( idxSlot, pSq, FALSE );
	///< 
	//결과를 클라에 보냄
	XPacket ar( (ID)xCL2GS_LOBBY_NEW_SQUAD );
	ar << snHero << idxSlot << idxLegion;
	Send( ar );


	return 1;
}

/** 
 @brief 군단편성시 부대의 위치를 서로 바꾼다.
 만약 idxDst == -1일경우 idxSrc부대를 삭제시킨다.
 클라이언트의 SendReqMoveSquad()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqMoveSquad()
*/
int XGameUser::RecvMoveSquad( XPacket& p )
{
	TRACE("RecvMoveSquad");
	int idxSrc, idxDst, idxLegion;
	ID snHeroSrc, snHeroDst;
	p >> idxSrc >> idxDst >> idxLegion >> snHeroSrc >> snHeroDst;
	XLegion *pLegion = m_spAcc->GetLegionByIdx( idxLegion ).get();
	XVERIFY_BREAK( pLegion == NULL );
	XVERIFY_BREAK( idxDst < -1 );
	int idx = pLegion->_GetIdxSquadByHeroSN( snHeroSrc );
	XVERIFY_BREAK( idx != idxSrc );
	if( snHeroDst > 0 )
	{
		idx = pLegion->_GetIdxSquadByHeroSN( snHeroDst );
		XVERIFY_BREAK( idx != idxDst );
	}
	if( idxDst == -1 )	{
		// remove squad
		pLegion->DestroySquadBysnHero( idxSrc );
	} else {
		pLegion->SwapSlotSquad( idxSrc, idxDst );
	}
	//결과를 클라에 보냄
	XPacket ar( (ID)xCL2GS_LOBBY_MOVE_SQUAD );
	ar << idxSrc << idxDst << idxLegion << snHeroSrc << snHeroDst;
	Send( ar );

	return 1;
}

/**
 @brief 부대 배치 바꿈.
*/
int XGameUser::RecvChangeSquad(XPacket& p)
{
	int idxlegion, legionherosize;
	ID snLeader;
	
	p >> idxlegion;
	p >> legionherosize;
	p >> (ID)snLeader;
	XVERIFY_BREAK(idxlegion < 0 || idxlegion >= 5 );
	XVERIFY_BREAK(legionherosize < 0 );
	XVERIFY_BREAK(legionherosize > XGAME::MAX_SQUAD);
	XVERIFY_BREAK(idxlegion != m_spAcc->GetCurrLegionIdx());
	int pos = 0;
	int shHero  =0;
	struct data{
		int spos;
		ID ssnhero;
	};
	XList4< data>	squadlist;
	squadlist.clear();
	//CHECK
	for (int n = 0; n < legionherosize; n++) {
		data tmp;
		p >> tmp.spos;
		p >> (ID)tmp.ssnhero;
		XHero* pHero = m_spAcc->GetHero(tmp.ssnhero);
		// 이런경우는 정상적이라면 일어나선 안되는 경우이므로 더이상 처리할필요 없습니다. 
		// 이 경우 클라는 해킹이거나 비동기상태이므로 XVERYFY_BREAK로 리턴하여 재접속하도록 하고 있습니다.
		XVERIFY_BREAK(pHero == nullptr);
		squadlist.push_back(tmp);
	}
	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_SQUAD);
	ar << (int)1;
	ar << idxlegion;
	ar << legionherosize;
	ar << (ID)snLeader;
	for (int n = 0; n < XGAME::MAX_SQUAD; n++) {
		m_spAcc->GetCurrLegion()->DestroySquadByIdxPos(n);
	}
	auto itor = squadlist.begin();		
	for (; itor != squadlist.end(); itor++) {	
		XSquadron *pSq = new XSquadron(m_spAcc->GetHero(itor->ssnhero));
		m_spAcc->GetCurrLegion()->AddSquadron(itor->spos, pSq, false);
		if (pSq->GetpHero()->GetsnHero() == snLeader)
			m_spAcc->GetCurrLegion()->SetpLeader(pSq->GetpHero());
		ar << (int)itor->spos;
		ar << (ID)itor->ssnhero;
	}
	m_spAcc->UpdatePower();
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_HERO_ORGANIZE );
	Send(ar);
	return 1;
}

void XGameUser::RecvPostInfoRequest(XPacket& p)
{
//	if (DBA_SVR)
//	{	
//		DBA_SVR->SendPostInfoRequest(m_spAcc, GetidConnect());
//	}
}

void XGameUser::RecvPostReadRequest(XPacket& p)
{
	////우편물 읽음.
	//XINT64 idx;
	//p >> idx;	
	//XBREAK(idx < 0);
	//m_spAcc->ChangePostStatus(idx, XPostInfo::_POST_STATUS_READ);
}

// void XGameUser::RecvPostItemGetRequest(XPacket& p)
// {
// 	//우편물 읽음.
// 	int idx;
// 	int itemposidx; //prop의 item idx 가 아니다. 1~10 까지 인벤토리 번호.
// 	int itemidx; //prop의 item idx 가 아니다. 1~10 까지 인벤토리 번호.
// 	p >> idx;
// 	p >> itemposidx;
// 	p >> itemidx;
// 
// 	XBREAK(idx < 0);
// 	XBREAK(itemposidx > 10 || itemposidx < 0);
// 
// 	m_spAcc->PostItemReceive(idx, itemposidx, itemidx);
// }

/**
 @brief snPost의 첨부아이템들을 모두 수령하고 메일을 삭제한다.
*/
void XGameUser::RecvPostItemGetRequestAll(XPacket& p)
{	
	ID snPost = 0;
	p >> snPost;
#pragma message("첨부템을 받더라도 메일은 지워선 안됨.")
	auto pPost = m_spAcc->GetPostInfo(snPost);
	if (pPost == nullptr)	{
		// 여기서 없으면 바로 에러로 응답
		XPacket ar((ID)xCL2GS_LOBBY_POST_GETITEM_ALL);
		ar << 0;
		Send(ar);
	}	else {
//		if (pPost->GetPostType() == XGAME::xPOSTTYPE_NOTIFY)
//		{
//			if (DBA_SVR)
//			{
//				ID idKey = DBA_SVR->SendPOSTInfoOneUpdate( m_spAcc, postindex, xCL2GS_LOBBY_POST_GETITEM_ALL);	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
//				if (idKey == 0) return;
//				DBA_SVR->AddResponse(idKey, this, &XGameUser::RecvAccountPostUpdateResult);
//			}
//		}
	//	else
		{
			// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
			// snPost메일을 삭제시킨다.
			ID idKey = DBA_SVR->SendPostDestroy(GetidAcc(), snPost );	
			DBA_SVR->AddResponse(idKey, this, &XGameUser::cbPostDelete);
		}		
	}	
}

/**
 @brief DB에서 메일삭제가 완료됨. 
 게임서버의 메일도 삭제와 동시에 아이템 수령
*/

void XGameUser::cbPostDelete( XPacket& p )
{
	int result = 0;
	ID snPost;
	ID idAcc;
//	ID idPacket;
	p >> idAcc;
	p >> snPost;
	p >> result;
//	p >> idPacket;
	// 클라측에서 메일을 삭제하라고 요청이 온것이기때문에 result에 관계없이 삭제하는게 맞는듯.
	auto bFind = m_spAcc->ReceivePostItemsAll( snPost );
	// 계정정보 동기화
	XPacket ar( (ID)xCL2GS_LOBBY_POST_GETITEM_ALL );
	ar << snPost;
	ar << result;
	ar << m_spAcc->GetCashtem();		// 바뀐값으로 동기화
	ar << m_spAcc->GetGold();
	ar << m_spAcc->GetptGuild();
	m_spAcc->SerializeItems( ar );
	Send( ar );
	if( bFind ) {
		// 메일 파괴
		m_spAcc->DestroyPostInfo( snPost );
	}
// 	if( result > 0 ) {
// 	} else {
// 		XPacket ar( (ID)xCL2GS_LOBBY_POST_GETITEM_ALL );
// 		ar << snPost;
// 		ar << result;
// 		Send( ar );
// 	}
}

// void XGameUser::RecvAccountPostUpdateResult(XPacket& p)
// {
// 	int result = 0;
// 	ID postindex;
// 	p >> postindex;
// 	p >> result;
// 	//모두 지급 해주는 거 만들어야 함.
// 	if (result > 0)
// 	{
// 		if (m_spAcc->PostItemReceiveAll(postindex))
// 		{
// 			XPacket ar((ID)xCL2GS_LOBBY_POST_GETITEM_ALL);
// 			ar << postindex;
// 			ar << result;
// 			ar << m_spAcc->GetCashtem();
// 			ar << m_spAcc->GetGold();
// 			m_spAcc->SerializeItems(ar);
// 			Send(ar);
// 			return;
// 		}
// 	}
// 	XPacket ar((ID)xCL2GS_LOBBY_POST_GETITEM_ALL);
// 	ar << postindex;
// 	ar << result;
// 	Send(ar);
// }


void XGameUser::RecvPostDeleteRequest(XPacket& p)
{
	////우편물 지우기.(지우는 기준을 만들어서 자동으로 삭제 하는 것도 고려 할것.)
	//XINT64 idx;

	////아이템을 수령 하지 않은 경우 삭제 하는것은 어떻게 할까?	
	//p >> idx;
	//
	//XBREAK(idx < 0);	

	//m_spAcc->ChangePostStatus(idx, XPostInfo::_POST_STATUS_DELETE);
}

/**
 @brief 계정의 우편함 데이터를 동기화시킨다.
*/
void XGameUser::SendSyncPosts()
{
	XPacket ar( (ID)xCL2GS_LOBBY_POST_INFO );
	m_spAcc->SerializePostInfo( ar );
	Send( ar );		// 클라이언트로 데이타를 보냄
//	MAKE_PCONNECT(pConnect);
//	pConnect->SendPostInfo( m_spAcc );	// 이걸 왜 커넥트에 넣어놨지?
}

/**
 @brief 무역상을 통해 자원을 교환했다.
 클라이언트의 SendReqTrade()에 대한 서버측의 Receive함수
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqTrade()
*/
int XGameUser::RecvTrade( XPacket& p )
{
// 	if (m_spAcc->GetTradeTimer()->IsOn()) {
// 		XPacket ar((ID)xCL2GS_LOBBY_TRADE);
// 		ar << (int)0;
// 		ar << DWORD(m_spAcc->GetTradeTimer()->GetRemainSec());	// 남은 시간을 보내준다.(초)		
// 		Send(ar);
// 		return 1;
// 	}
	int bSuccess;
	if( m_spAcc->GettimerTrader().IsOn()  && !m_spAcc->IsOverTraderRecall() ) {
		bSuccess = 0;
		// 상인이 떠나있는 상태인데 거래가 들어옴. 타이머를 다시 동기화시켜줌.
// 		XPacket ar((ID)xCL2GS_LOBBY_TRADE);
// 		ar << (int)0;			// 실패패킷
// //		ar << DWORD(m_spAcc->GetTradeTimer()->GetRemainSec());	// 남은 시간을 보내준다.(초)		
// 		ar << m_spAcc->GettimerTrader();
// 		Send(ar);
// 		return 1;
	} else {
		bSuccess = 1;
		// 각 자원의 환율에 따라 금화로 환산
		int resource[XGAME::xRES_MAX] = { 0 };
		int totalGold = 0;
		_tstring strLog = XE::Format(_T("Trade "));
		for (int i = 0; i < XGAME::xRES_MAX; i++) {
			int tmpgold = 0;
			auto typeRes = (XGAME::xtResource)i;
			p >> resource[i];
			XVERIFY_BREAK( resource[i] > m_spAcc->GetResource( typeRes )  );
			int result = m_spAcc->AddResource(typeRes, -resource[i]);
			XVERIFY_BREAK(result < 0);
			float rate = XGC->m_resourceRate[i];
			tmpgold = (int)(resource[i] * rate);
			strLog += XE::Format(_T("[%d]:%d => %d"), i, resource[i], tmpgold);
			totalGold += tmpgold;
		}
		strLog += XE::Format(_T(" = TotalGold: %d )"), totalGold);
		AddLog(XGAME::xULog_Use_Trade, strLog);
		m_spAcc->AddGold(totalGold);	
	//	m_spAcc->GetTradeTimer()->Set(float(XGC->m_tradeCallInterval));// 60 * 60 * 24 * 3); //3일 후에 다시 만나요.
	// 	float secRecall = (float)XGC->GetsecTraderReturn(GetLevel());
	// 	m_spAcc->GetTradeTimer()->Set( secRecall );
	// 	m_spAcc->SetsecTrade((DWORD)(m_spAcc->GetTradeTimer()->GetRemainSec()));
		m_spAcc->StartTimerByTrader();
		//
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, XGAME::xUA_TRADE );
	}
	///< 
	//결과를 클라에 보냄
	XPacket ar( (ID)xCL2GS_LOBBY_TRADE );
	ar << (int)bSuccess;
//	ar << (DWORD)(m_spAcc->GetTradeTimer()->GetRemainSec());
	ar << m_spAcc->GettimerTrader();
	if( bSuccess ) {
		m_spAcc->SerializeResource( ar );	// 통째로 동기화
		ar << m_spAcc->GetGold();
	}
	Send( ar );	

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqChangeScalpToBook()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqChangeScalpToBook()
*/
int XGameUser::RecvChangeScalpToBook( XPacket& p )
{
// 	int i0;
// 	XGAME::xtClan clan;
// 	p >> i0;	clan = (XGAME::xtClan)i0;
// 	XVERIFY_BREAK( clan <= XGAME::xCL_NONE || clan >= XGAME::xCL_MAX );
// 	XArrayLinearN<XBaseItem*, 256> ary;
// 	ID idScalp = m_spAcc->ChangeScalpToBook( clan, &ary );
// 	// 클라에서 이미 징표개수 확인해서 보낸것이므로 0이나와선 안됨
// 	XVERIFY_BREAK( idScalp == 0 );
// 	//
// 	//결과를 클라에 보냄
// 	XPacket ar( (ID)xCL2GS_LOBBY_CHANGE_SCALP_TO_BOOK );
// 	ar << (BYTE)clan;
// 	ar << (BYTE)ary.size();
// 	ar << (BYTE)VER_ITEM_SERIALIZE;
// 	ar << (BYTE)(ary.size() * 10);
// 	XARRAYLINEARN_LOOP( ary, XBaseItem*, pItem )
// 	{
// 		XBaseItem::sSerialize( ar, pItem );
// 	} END_LOOP;
// 	ar << idScalp;
// 	Send( ar );
	return 1;
}

/**
 클라이언트의 SendReqUpgradeSquad()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqUpgradeSquad()
*/
// int XGameUser::RecvUpgradeSquad( XPacket& p )
// {
// 	ID snHero;
// 	p >> snHero;
// 	XVERIFY_BREAK( snHero == 0 );
// 	XHero *pHero = m_spAcc->GetHero( snHero );
// 	XVERIFY_BREAK( pHero == nullptr );
// 	XVERIFY_BREAK( pHero->GetlevelSquad() >= PROP_SQUAD->GetMaxLevel() );
// 	int levelNext = pHero->GetlevelSquad() + 1;
// 	auto& propSquad = PROP_SQUAD->GetTable( levelNext );
// 	XVERIFY_BREAK( m_spAcc->GetJewel() < propSquad.numRes );
// 	XVERIFY_BREAK( m_spAcc->IsTrainingSquadupHero( snHero ) );
// 	XVERIFY_BREAK( m_spAcc->GetNumRemainFreeSlot() <= 0 );
// 	// 유닛의 공격타입에 따라 필요한 업글템의 아이디를 얻음.
// 	ID idNeed = XGAME::GetSquadLvupItem( XGAME::GetTypeUnit( pHero->GetUnit() ) );
// 	XBREAK( idNeed == 0 );
// 	XBREAK( propSquad.gradeNeed <= XGAME::xGD_NONE || propSquad.gradeNeed >= XGAME::xGD_MAX );
// 	idNeed += ( propSquad.gradeNeed - 1 );
// 	// 소지한 업글템의 개수를 셈
// 	int num = m_spAcc->GetNumItems( idNeed );
// 	XVERIFY_BREAK( num < propSquad.numItem );
// 	m_spAcc->DestroyItem( idNeed, propSquad.numItem );
// 	m_spAcc->AddResource( XGAME::xRES_JEWEL, -propSquad.numRes );
// 	// 훈련시작
// 	XAccount::xTrainSlot slot;
// 	slot.DoStartSquadup( snHero, (float)propSquad.secTrain );
// 	m_spAcc->AddTrainSlot( slot );
// 	//결과를 클라에 보냄
// 	XPacket ar( (ID)xCL2GS_LOOBY_UPGRADE_SQUAD );
// 	ar << snHero;
// 	ar << idNeed;
// 	ar << propSquad.numItem;
// 	ar << 0;	// levelsquad
// 	ar << (BYTE)VER_ETC_SERIALIZE;
// 	ar << (BYTE)0;
// 	ar << (WORD)0;
// 	m_spAcc->SerializeTrainSlot( ar );
// 	m_spAcc->SerializeResource( ar );
// 	Send( ar );
// 
// 
// 	return 1;
// }

/**
 클라이언트의 SendReqReleaseHero()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqReleaseHero()
*/
int XGameUser::RecvReleaseHero( XPacket& p )
{
	ID snHero;
	p >> snHero;
	XHero *pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	// 영웅의 토탈 exp를 얻는다.
	XINT64 expTotal = pHero->GetExpSum();
	expTotal += pHero->GetMaxExpWithLevel( XGAME::xTR_LEVEL_UP, 1 );
	// 책을 만든다.
//	XPropItem::xPROP *pPropBook = PROP_ITEM->GetClanBookProp( pHero->GetClan() );
	auto pPropBook = PROP_ITEM->GetpProp( _T("book01_freedom") );
	// 영웅을 해제시킨다.
	m_spAcc->DestroyHero( snHero );
	pHero = nullptr;	// 파괴되었으므로
	XBREAK( pPropBook == nullptr );
//	XBaseItem *pItem = XBaseItem::sCreateItem( pPropBook );
	auto pItem = m_spAcc->CreateItemToInvenForNoStack( pPropBook );
	XVERIFY_BREAK( pItem == nullptr );
	pItem->SetBookExp( expTotal );
//	m_spAcc->AddItem( pItem );
	///<
	XPacket ar( (ID)xCL2GS_LOBBY_RELEASE_HERO );
	ar << (BYTE)VER_ITEM_SERIALIZE;
	ar << (BYTE)1;
	ar << (WORD)0;
	XBaseItem::sSerialize( ar, pItem );
	ar << snHero;
	Send( ar );
	//
	xQuest::XEventInfo infoQuest;		// 이벤트 정보.
	infoQuest.SetidItem( pItem->GetidProp() );
	DispatchQuestEvent( XGAME::xQC_EVENT_GET_ITEM, infoQuest );

	return 1;
}

/**
 @brief 아이템 하나를 생성하게 한다.
*/
void XGameUser::SendAddItem( XBaseItem *pItem )
{
	XPacket ar( (ID)xCL2GS_ADD_ITEM );
	ar << (BYTE)VER_ITEM_SERIALIZE;
	ar << (BYTE)1;
	ar << (WORD)0;
	XBaseItem::sSerialize( ar, pItem );
	Send( ar );
}

/**
 @brief 여러개의 아이템을 추가하도록 클라로 보낸다.
*/
void XGameUser::SendItemsList( const XArrayLinearN<XBaseItem*, 256>& ary )
{
	XPacket ar( (ID)xCL2GS_ADD_ITEM_LIST );
	ar << (BYTE)VER_ITEM_SERIALIZE;
	ar << (BYTE)0;
	ar << (WORD)0;
	ar << ary.size();
	XARRAYLINEARN_LOOP_AUTO( ary, pItem )
	{
		XBaseItem::sSerialize( ar, pItem );
	} END_LOOP;
	Send( ar );
}

/**
 @brief 아이템 인벤토리 전체를 동기화 시킨다.
*/
void XGameUser::SendItemInvenSync()
{
	XPacket ar( (ID)xCL2GS_ITEM_INVEN_SYNC );
	m_spAcc->SerializeItems( ar );
	Send( ar );

}

int XGameUser::RecvItemSellList(XPacket& p)				//아이템 상점 판매 목록
{
	int i0;
	p >> i0;	auto type = ( XGAME::xtSpentCall )i0;
	int bSuccess;
	if ( m_spAcc->IsOverArmoryItemReset() ) {
		// 갱신시간이 이미 지났으면 그냥 갱신시켜줌.
		m_spAcc->ResetTimerByArmory();
		m_spAcc->ChangeShopItemList();
		bSuccess = 1;
	}	else {
		//
		if (type == XGAME::xSC_SPENT_ITEM) { //여긴 카드
			const ID idItemForReset = XGC->m_armoryRecallItem;
			int nCount = m_spAcc->GetNumItems( idItemForReset );
			if (nCount > 0) {				
				m_spAcc->ResetTimerByArmory();
				m_spAcc->ChangeShopItemList();
				m_spAcc->DestroyItem( idItemForReset, 1 );
				bSuccess = 1;
				int tmpCount = m_spAcc->GetNumItems( XGC->m_armoryRecallItem );
				//
				const _tstring strLog = XE::Format(_T("ShopMerchant_Call_Item: Before[%d] => After[%d])"), nCount, tmpCount);
				AddLog(XGAME::xULog_ShopList_Call_Gem, strLog);
			} else {
				// 아이템이 없음.
				bSuccess = 0;
				const _tstring strLog = XE::Format(_T("ShopMerchant_Call_Item: Have_not_enough_Item)"));
				AddLog(XGAME::xULog_ShopList_Call_Gem, strLog);
			}
		// by item
		}	else 
		if (type == XGAME::xSC_SPENT_GEM) {//젬
			int remaincash = m_spAcc->GetCashtem();
			remaincash -= XGC->m_armoryRecallGem;
			if (remaincash > 0) {
				bSuccess = 1;
				m_spAcc->ResetTimerByArmory();
				m_spAcc->ChangeShopItemList();
				m_spAcc->SetCashtem(remaincash);
// 				ID idKey = DBA_SVR->SendShopGemCall(GetidAccount(), remaincash, GetidConnect(), xCL2GS_LOBBY_ITEM_SHOP_LIST_CASH);	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
// 				DBA_SVR->AddResponse(idKey, this, &XGameUser::RecvShopCashCallResult);
			} else {
				bSuccess = 0;
				const _tstring strLog = XE::Format(_T("ShopMerchant_Call_Gem:Have_not_enough_Gem)"));
				AddLog(XGAME::xULog_ShopList_Call_Gem, strLog);
			}
		} // by gem
	}
	XPacket ar( (ID)xCL2GS_LOBBY_ITEM_SHOP_LIST_CASH );
	ar << (int)type;
	ar << (int)bSuccess;
	ar << m_spAcc->GetCashtem();
	if( bSuccess )
		m_spAcc->SerializeShopList( ar );
	Send( ar );
	return 1;
}
/**
 @brief 
*/
int XGameUser::RecvCashItemBuyPayload(XPacket& p)			//구글 payload 발급
{
	BYTE b0;
	_tstring productID;
//	int ProductType = (int)XGAME::xtCashType::xCT_NONE;
	p >> b0;	auto platform = (XGAME::xtPlatform)b0;
	p >> b0 >> b0 >> b0;
	XVERIFY_BREAK( platform <= xPL_NONE || platform >= xPL_MAX );
	m_Platform = platform;
	p >> productID;			//상품 코드
	XVERIFY_BREAK( productID.empty() );

	const auto pGoodsInfo = XGC->GetCashItem( productID, platform );
	XVERIFY_BREAK( pGoodsInfo == nullptr );
	if( m_spAcc->IsSubscribing() && pGoodsInfo->IsScribeType() ) {
		SendCashItemBuyPayload( xIAPP_CANT_BUY_ANYMORE, platform, pGoodsInfo->m_type, productID, _T("") );
		return 1;
	}
#ifdef _INAPP_GOOGLE
	ID idKey = DBA_SVR->SendGooglePayload(GetidAcc()
																			, platform
																			, pGoodsInfo->m_type
																			, productID
																			, GetidConnect()
																			, xCL2GS_LOBBY_CASH_ITEM_BUY_PAYLOAD);
	DBA_SVR->AddResponse(idKey, this, &XGameUser::cbCashItemBuyPayload);
#endif
	return 1;
}
/**
 @brief 페이로드가 생성되어 돌아옴.
*/
void XGameUser::cbCashItemBuyPayload(XPacket& p)
{	
#ifdef _INAPP_GOOGLE
	BYTE b0;
	_tstring strProductID;
	_tstring strPayloadkey;
// 	int result = 0;
// 	int ProductType = (int)XGAME::xtCashType::xCT_NONE;
	ID idaccount = 0;
	ID idPacket = 0;	
	ID idConnect = 0;
	p >> b0;	auto errCode = (XGAME::xtErrorIAP)b0;
	p >> b0;	auto platform = (XGAME::xtPlatform)b0;
	p >> b0;	auto typeProduct = ( XGAME::xtCashType )b0;
	p >> b0;
	p >> idaccount;
	p >> strProductID;
	p >> strPayloadkey;
	p >> idConnect;
	p >> idPacket;	

	m_spAcc->m_strPayload = strPayloadkey;		// 일단 보관

	// 페이로드 전송
	SendCashItemBuyPayload( errCode, platform, typeProduct, strProductID, strPayloadkey );
#endif
}

void XGameUser::SendCashItemBuyPayload( xtErrorIAP err
																			, xtPlatform platform
																			, xtCashType typeProduct
																			, const _tstring& strProductID
																			, const _tstring& strPayloadKey )
{
	XPacket ar( (ID)xCL2GS_LOBBY_CASH_ITEM_BUY_PAYLOAD );
	ar << (BYTE)err;
	ar << (BYTE)platform;
	ar << (BYTE)typeProduct;
	ar << (BYTE)0;
	ar << strProductID;
	GetpCryptObj()->SerializeEncryptWithString( strPayloadKey, ar, 20 );
	Send( ar );
}
/**
 @brief 클라에서 구매를 한 후 서버로 확인요청을 받음.
 인앱 결제
*/
int XGameUser::RecvBuyCashtemIAP(XPacket& p)
{	
	BYTE b0;
	xtErrorIAP errCode = xIAPP_NONE;
	const ID idaccount = GetidAcc();
	_tstring idsProduct;
	do {
		_tstring _strJson, _strSignature;
		p >> b0;		auto platForm = (xtPlatform)b0;
		p >> b0 >> b0 >> b0;
		XVERIFY_BREAK( platForm <= xPL_NONE || platForm >= xPL_MAX );
		p >> _strJson;
		p >> _strSignature;
		// Json분석
		const std::string strcJsonReceiptFromClient = SZ2C( _strJson );
		const std::string strcSignature = SZ2C( _strSignature );
		XGAME::xInApp inapp;
		//////////////////////////////////////////////////////////////////////////
		if( platForm == xPL_SOFTNYX ) {
			const _tstring strPayload = _strSignature;
			const std::string strcPayload = strcSignature;
			p >> idsProduct;
			errCode = XSoftnyx::sDoVerifyAfterPurchase( strcJsonReceiptFromClient
																								, strcPayload
																								, idsProduct
																								, &inapp );
			CONSOLE_ACC( TAG_SOFTNYX, "platform=%d, strJson=%s, strSignature=%s, idsProduct=%s, errCode=%d"
									 , platForm
									 , _strJson.c_str()
									 , _strSignature.c_str()
									 , idsProduct.c_str()
									 , errCode );
		} else
		//////////////////////////////////////////////////////////////////////////
		if( platForm == xPL_GOOGLE_STORE ) {
			errCode = XGoogle::sDoVerifyAfterPurchase( strcJsonReceiptFromClient
																								, strcSignature
																								, &inapp );
			// 영수증 검증
			/**
				여기서 반드시 과거 영수증들과 비교해서 이미 처리한 결제인지 확인해야 함.
				클라이언트에서 같은 영수증이 두번 날아올수도 있음.
				중복되는 영수증이 두번날아오면 그냥 씹고 세번째부터는 해킹의심유저로 봐야함.
			*/
			if( errCode != xIAPP_NONE ) {
				CONSOLE_ACC( TAG_IAP, "platform=%d, strJson=%s\r\n, strSignature=%s\r\n, product=%s, devPayload=%s, errCode=%d"
										 , platForm
										 , _strJson.c_str()
										 , _strSignature.c_str()
										 , inapp.m_idsProduct.c_str()
										 , C2SZ( inapp.m_strcPayload )
										 , errCode );
			}
			XBREAK( inapp.AssertValid() == false );
			XBREAK( XEnv::sGet()->GetstrPublicKey().empty() );
		} // if( platForm == xPL_ANDROID )
		//////////////////////////////////////////////////////////////////////////
		XBREAK( errCode == xIAPP_SUCCESS );		// 이건 지금 결정되선 안됨
		if( errCode != xIAPP_NONE )
			break;		// 에러발생
		//상품 내용이 서버가 알고 있는지만 확인 하고 나머지 검증은 DBA 가 하자.
		ID idKey = DBA_SVR->SendGoogleBuyCashVerify( idaccount
																								, inapp
																								, GetidConnect()
																								, xCL2GS_LOBBY_CASH_ITEM_BUY_IAP );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbCashItemBuyGoogle );
	} while (0);
	// 에러가 났을경우 클라로 결과보냄
	XBREAK( errCode == xIAPP_SUCCESS );		// success는 아직 판단하지 않음.
	if( errCode != xIAPP_NONE ) {
		CONSOLE_ACC( TAG_IAP, "IAP error: product=%s, err=%d"
								 , idsProduct.c_str(), errCode );
		SendCashItemBuyGoogle( errCode, idsProduct );
	}
	return 1;
}

// DBA서버로부터 인앱 구매결과가 도착함.
void XGameUser::cbCashItemBuyGoogle( XPacket& p )
{
	BYTE b0;	
	XGAME::xInApp inapp;
	ID idAcc, idConnect, idPacket;

	p >> b0;		auto errCode = (XGAME::xtErrorIAP)b0;
	p >> b0 >> b0 >> b0;
	p >> idAcc;
	p >> inapp;
	p >> idConnect;
	p >> idPacket;
	XBREAK( inapp.m_Platform == xPL_NONE );
	XBREAK( inapp.m_idsProduct.empty() );
	// 검증에 성공하면 계정에 적용.
	auto pGoodsInfo = XGlobalConst::sGet()->GetCashItem( inapp.m_idsProduct, inapp.m_Platform );
	if( XASSERT(pGoodsInfo) ) {
		// 결제 성공
		if( errCode == XGAME::xIAPP_SUCCESS ) {
			if (inapp.IsSubscribe() ) {
				// 구입한 시점에 월정액 상품의 지급메일을 보낸다.
				AddPostSubscribe( inapp.m_Platform );
				const DWORD subscribeGemInterval = (DWORD)XGC->m_secSubscribeCycle;
				m_spAcc->SetsecNextSubscribe( XTimer2::sGetTime() + subscribeGemInterval );	// 구매시간 + 24시간을 다음 지급시간으로.
				m_spAcc->SetnumSubscribe( 1 );		// 최초 한번 지급
// 				const float secRemainTimer = m_spAcc->GetSubscribeTimer()->GetRemainSec();
// 				const float secEnd = (float)(pGoodsInfo->m_day * 60 * 60 * 24);
// 				const float secRemainEndTimer = m_spAcc->GetSubscribeEndTimer()->GetRemainSec();
// 				m_spAcc->GetSubscribeTimer()->Set( subscribeGemInterval );	//24시간마다 젬 배송
// 				m_spAcc->SetsecSubscribe( (DWORD)secRemainTimer );
// 				m_spAcc->GetSubscribeEndTimer()->Set( secEnd ); //1달동안 유효함.
// 				m_spAcc->SetsecSubscribeEnd( (DWORD)secRemainEndTimer );
			} else {
				m_spAcc->AddCashtem( pGoodsInfo->m_gem );	// 구매한 캐시 넣어줌.
			}
			DispatchQuestEvent( XGAME::xQC_EVENT_BUY_CASH, pGoodsInfo->m_gem );
			m_spAcc->AddnumBuyCash( 1 );		// 구입횟수 증가
			m_spAcc->AddamountBuyCash( pGoodsInfo->m_gem );	// 구입한 캐시개수 증가.
			const _tstring strPayload = C2SZ(inapp.m_strcPayload);
			ID idKey = DBA_SVR->SendDeletePayload( idAcc
																						, inapp.m_idsProduct
																						, strPayload );
			DBA_SVR->AddResponse(idKey, this, &XGameUser::cbDeletePayload );
		} else
		if( errCode == XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD ) {
			// 클라로 구매 결과를 보냄.
			SendCashItemBuyGoogle( errCode, inapp.m_idsProduct );
		} else {
			SendCashItemBuyGoogle( errCode, inapp.m_idsProduct );
		}
	}
	// 클라로 구매 결과를 보냄.
//	SendCashItemBuyGoogle( errCode, inapp.m_idsProduct );

}
/**
 @brief 페이로드 삭제 완료
*/
void XGameUser::cbDeletePayload( XPacket& p )
{
	BYTE b0;
	ID idAcc;
	_tstring idsProduct;
	p >> b0;		auto errCode = ( XGAME::xtErrorIAP )b0;
	p >> b0 >> b0 >> b0;
	p >> idAcc;
	p >> idsProduct;
	XBREAK( idsProduct.empty() );
	// 클라로 구매 결과를 보냄.
	SendCashItemBuyGoogle( errCode, idsProduct );
}

/**
 @brief 인앱구매 결과를 클라에 보내준다.
*/
void XGameUser::SendCashItemBuyGoogle( XGAME::xtErrorIAP errCode
																			, const _tstring& idsProduct )
{
	XPacket ar( (ID)xCL2GS_LOBBY_CASH_ITEM_BUY_IAP );
	ar << (BYTE)errCode;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	XBREAK( idsProduct.empty() );
	ar << idsProduct;
	ar << m_spAcc->GetCashtem();  
	Send( ar );
}

int XGameUser::RecvBuyItem(XPacket& p)					//아이템 구매
{
	ID idProp;
// 	int shoptype; 
	int num = 1;
	int  i0;
	p >> i0;	auto shoptype = (XGAME::xtShopType)i0;
	p >> idProp;
	XVERIFY_BREAK( shoptype <= XGAME::xSHOP_NONE || shoptype >= XGAME::xSHOP_MAX );
	auto pProp = PROP_ITEM->GetpProp(idProp);
	XVERIFY_BREAK(pProp == nullptr);
	XVERIFY_BREAK( m_spAcc->IsSaleItemidProp( idProp ) == FALSE );
	XPacket ar((ID)xCL2GS_LOBBY_ITEM_BUY);
	p >> i0;	auto costType = (XGAME::xtCoin)i0;
	XVERIFY_BREAK( costType <= xCOIN_NONE || costType >= xCOIN_MAX );
	int cost = 0;
	int HaveGold = m_spAcc->GetGold();
	int Havecash = m_spAcc->GetCashtem();
	//무기상.
	if (shoptype == XGAME::xSHOP_ARMORY) {
// 		if (m_spAcc->IsSaleItemidProp(idProp)) {	
// 			p >> costType;
			if( costType == XGAME::xCOIN_GOLD ) {
				// 장착템은 템 가격을 직접 계산한다.
				cost = (int)pProp->GetBuyCost( GetLevel() );
				XVERIFY_BREAK(cost < 0 );
				m_spAcc->AddGold(-cost);		// Cost 비용 삭제 해주고.			
			} else
			if( costType == XGAME::xCOIN_CASH ) {
				cost = (int)pProp->cashCost;
				XVERIFY_BREAK( cost < 0 );
				m_spAcc->AddCashtem( -cost );
			} else
			if( costType == XGAME::xCOIN_MEDAL ) {
				int costMedal = XGC->m_costMedalForArmoryHero;
				XVERIFY_BREAK( costMedal < 0 );
				const _tstring idsMedal( _T("medal_tanker01") );
				int numMedal = m_spAcc->GetNumItems( idsMedal.c_str() );
				XVERIFY_BREAK( numMedal < costMedal );
				m_spAcc->DestroyItem( idsMedal.c_str(), costMedal );
			}
			// 상점 리스트에서 상품 목록도 지워주고.
			m_spAcc->RemoveListShopSell(idProp);
			m_spAcc->CreateItemToInven(pProp, num);
			DispatchQuestEvent( XGAME::xQC_EVENT_BUY_ITEM, pProp->idProp );
// 		}		
		ar << num;  //(성공의 경우 1 반환)
		ar << m_spAcc->GetGold();
		ar << m_spAcc->GetCashtem();
		m_spAcc->SerializeItems(ar);
		Send(ar);
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_BUY_ARMOR );
		_tstring strLog = XE::Format(_T("Buy_Item(XGAME::xSHOP_ARMORY): BuyType:%d, BuyIdProp:%d, Cost:%d, HaveGold:%d, RemainGold:%d, HaveCash:%d, RemainCash:%d"), costType, idProp, cost, HaveGold, m_spAcc->GetGold(), Havecash, m_spAcc->GetCashtem());
		AddLog(XGAME::xULog_Buy_Items, strLog);
	} else 
	// 귀중품상점
	if( shoptype == XGAME::xSHOP_CASHSHOP ) {		
		DWORD goldCost = pProp->GetBuyCost( GetLevel() );
		XVERIFY_BREAK((goldCost == 0) && (pProp->cashCost == 0));
//		XVERIFY_BREAK((pProp->GetCost(ACCOUNT->GetLevel()) != 0) && (pProp->cashCost != 0));
		if (goldCost > 0) {
			cost = (int)goldCost;
			if (goldCost > (DWORD)HaveGold) {
				ar << 0;  //(성공의 경우 1 반환)
				Send(ar);
				_tstring strLog = XE::Format(_T("Buy_Item_gold_Failed(XGAME::xSHOP_CASHSHOP): shoptype:%d, BuyIdProp:%d, Cost:%d, HaveCash:%d"), shoptype, idProp, cost, HaveGold);
				AddLog(XGAME::xULog_Buy_Items, strLog);
				return 1;
			} else {
				m_spAcc->AddGold(-cost);		// Cost 비용 삭제 해주고.
			}
		} else if (pProp->cashCost > 0) {
			cost = pProp->cashCost;
			if (pProp->cashCost  >(DWORD)Havecash) {
				ar << 0;  //(성공의 경우 1 반환)
				Send(ar);
				_tstring strLog = XE::Format(_T("Buy_Item_cash_Failed(XGAME::xSHOP_CASHSHOP): shoptype:%d, BuyIdProp:%d, Cost:%d, HaveCash:%d"), shoptype, idProp, cost, Havecash);
				AddLog(XGAME::xULog_Buy_Items, strLog);
				return 1;
			} else {
				m_spAcc->AddCashtem(-cost);		// Cost 비용 삭제 해주고.
			}
			//뺄꺼 빼주고
		}
		m_spAcc->CreateItemToInven(pProp, num);
		ar << num;  //(성공의 경우 1 반환)
		ar << m_spAcc->GetGold();
		ar << m_spAcc->GetCashtem();
		m_spAcc->SerializeItems(ar);
		Send(ar);
		_tstring strLog = XE::Format(_T("Buy_Item(XGAME::xSHOP_CASHSHOP): BuyIdProp:%d, Cost:%d, HaveGold:%d, RemainGold:%d, HaveCash:%d, RemainCash:%d"), idProp, cost, HaveGold, m_spAcc->GetGold(), Havecash, m_spAcc->GetCashtem());
		AddLog(XGAME::xULog_Buy_Items, strLog);
		//Cash 사용 정보 Update 해주자.	
		if (DBA_SVR) {
			DBA_SVR->SendCashInfoUpdate(GetidAcc(), m_spAcc->GetCashtem());
		}
	} else {
		// 알수없는 구입메시지
		XVERIFY_BREAK(1);
	}
	
	return 1;
}

/**
 @brief 아이템 상점에 판매
*/
int	XGameUser::RecvSellItem(XPacket& p) 					
{
	ID snItems = 0;
	p >> snItems;
	auto pItem = m_spAcc->GetItem(snItems);
	XVERIFY_BREAK(pItem == nullptr);
	int success = 1;
	DWORD HaveGold = m_spAcc->GetGold();
	auto pProp = pItem->GetpProp();
	auto cost = pProp->GetSellCost( GetLevel() );
	ID idProp = pItem->GetidProp();
	success = m_spAcc->DestroyItemBySN(snItems);
	pItem = nullptr;
	XVERIFY_BREAK( success == 0 );
	if (success) {
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_SELL_ITEM );
		//
		XPacket ar((ID)xCL2GS_LOBBY_ITEM_SELL);
		p << success;		
// 		if( XBREAK(XGC->m_rateShopSell >= 1.f) )
// 			XGC->m_rateShopSell = 1.f;
//		DWORD sellGold = DWORD(cost * XGC->m_rateShopSell);	// 판매가는 상점가의 n%
		int goldSell = (int)cost;
//		int GetSellGold
		XBREAK( goldSell < 0 );		// 비정상적인 상황
		m_spAcc->AddGold( goldSell );
		ar << success;
		ar << snItems;
		ar << m_spAcc->GetGold();
		Send(ar);
		_tstring strLog = XE::Format(_T("Buy_Sell(XGAME::xSHOP_ARMORY):[SellRate:%f] IdProp:%d, Cost:%d, HaveGold:%d => RemainGold:%d"), XGC->m_rateShopSell, idProp, goldSell, HaveGold, m_spAcc->GetGold());
		AddLog(XGAME::xULog_Buy_Items, strLog);
	}
	return 1;
}

int XGameUser::RecvItemSpent(XPacket& p)					///< 아이템 사용
{
	ID snItems = 0;
	p >> snItems;

	XVERIFY_BREAK( snItems == 0 );
	auto pItem = m_spAcc->GetItem( snItems );
	XVERIFY_BREAK( pItem == nullptr );
	//기타 아이템 효과 여부에 따라 여기에 코드를 추가 해줘야 한다.	
	// 드랍확률 버프템.
	if( pItem->GetstrIdentifier() == _T("item_drop_up") ||
		pItem->GetstrIdentifier() == _T("item_loot_up") )
	{
		XGAME::xBuff buff;
		buff.idBuff = pItem->GetidProp();
		buff.sid = pItem->GetstrIdentifier();
		buff.idName = pItem->GetpProp()->idName;
		buff.idDesc = pItem->GetpProp()->idDesc;
		buff.strIcon = pItem->GetpProp()->strIcon;
		float sec = (float)pItem->GetpProp()->aryAdjParam[0].param;
		// 버프가 이미 있으면 현재 버프시간에 더한다.
		buff.timer.Set( sec );
		auto pAddBuff = m_spAcc->AddBuff( buff );
		// 버프발생을 클라로 알린다.
		SendOccurBuff( pAddBuff );
	}

	int success = 1;
	//겹치는 아이템도 삭제 됨? ==> ㅇㅇ -xuzhu-
	success = m_spAcc->DestroyItemBySN(snItems);

	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SPENT);	
	ar << success;
	ar << snItems;
	Send(ar);
	return 1;
}
int XGameUser::RecvInventoryExpand(XPacket& p)			///< 인벤토리 확장
{	
	int type = 0;

	p >> type;

	// 최대 창고 갯수 체크
	if (m_spAcc->GetmaxItems() + 5 > XGAME::ITEM_MAX_COUNT)		//인벤토리를 더이상 확장 할 수 없다.
	{
		XPacket ar((ID)xCL2GS_LOBBY_INVENTORY_EXPAND);

		ar << int(0);
		ar << int(3);

		Send(ar);

		_tstring strLog = XE::Format(_T("Inventory_Expanded_Failed:Over_Max User InvenCount[%d]"), m_spAcc->GetmaxItems());
		AddLog(XGAME::xULog_Use_Gem, strLog);
	}

	if (type == XGAME::xSC_SPENT_ITEM) // 여긴 키
	{
		int nCount = m_spAcc->GetNumItems(XGC->m_storageLockItem);
		if (nCount > 0)
		{			
			m_spAcc->DestroyItem(XGC->m_storageLockItem, 1);
			m_spAcc->SetmaxItems(m_spAcc->GetmaxItems() + 5);

			int tmpCount = m_spAcc->GetNumItems(XGC->m_storageLockItem);

			XPacket ar((ID)xCL2GS_LOBBY_INVENTORY_EXPAND);
			ar << int(1);
			ar << type;
			ar << XGC->m_storageLockItem;
			ar << m_spAcc->GetmaxItems();			
			Send(ar);

			_tstring strLog = XE::Format(_T("Inventory_Expanded_Item:Before[%d] After[%d]"), nCount, tmpCount);
			AddLog(XGAME::xULog_Use_Gem, strLog);
		}
		else
		{
			XPacket ar((ID)xCL2GS_LOBBY_INVENTORY_EXPAND); //소모 리소스 부족
			
			ar << (int)0;
			ar << type;		// 호출 타입: 아이템			
			Send(ar);

			_tstring strLog = XE::Format(_T("Inventory_Expanded_Item:Have_not_enough[%d]"), nCount);
			AddLog(XGAME::xULog_Use_Gem, strLog);
			return 1;
		}
	}
	else if (type == XGAME::xSC_SPENT_GEM) //젬
	{
		int Havecash = m_spAcc->GetCashtem();
		XVERIFY_BREAK( Havecash < XGC->m_storageLockGem );
		m_spAcc->AddCashtem( -XGC->m_storageLockGem );
		m_spAcc->SetmaxItems(m_spAcc->GetmaxItems() + XGAME::INCREASE_INVEN_SIZE);

		XPacket ar((ID)xCL2GS_LOBBY_INVENTORY_EXPAND); //소모 리소스 부족
		ar << int(1);
		ar << type;		// 호출 타입: 아이템
		ar << XGC->m_storageLockGem;
		ar << m_spAcc->GetmaxItems();
		Send(ar);

		_tstring strLog = XE::Format(_T("Inventory_Expanded_Gem:Before[%d] After[%d]"), Havecash, m_spAcc->GetCashtem());
		AddLog(XGAME::xULog_Use_Gem, strLog);
	}

	return 1;
}

/**
 @brief 이떤이벤트(렙업,로그인,퇴치등)후에 새로운 퀘스트가 발생할 조건이되면 발생시킨다.
*/
void XGameUser::DispatchQuestEvent( XGAME::xtQuestCond eventQuest, const xQuest::XEventInfo& infoQuest )
{
	if( XBREAK( m_spAcc == nullptr ) )
		return;
	// 퀘스트리스트를 순회하면서 해당이벤트로 발생하는 퀘스트가 있는지 검사하고
	// 있다면 퀘스트를 생성시킨다.
	m_spAcc->GetpQuestMng()->DispatchEvent( eventQuest, infoQuest );
}

void XGameUser::DispatchQuestEvent( XGAME::xtQuestCond eventQuest )
{
	if( XBREAK( m_spAcc == nullptr ) )
		return;
	// 퀘스트리스트를 순회하면서 해당이벤트로 발생하는 퀘스트가 있는지 검사하고
	// 있다면 퀘스트를 생성시킨다.
	XEventInfo info;
	m_spAcc->GetpQuestMng()->DispatchEvent( eventQuest, info );
}

void XGameUser::DispatchQuestEvent( XGAME::xtQuestCond eventQuest, DWORD dwParam, DWORD dwParam2 )
{
	if( XBREAK( m_spAcc == nullptr ) )
		return;
	// 퀘스트리스트를 순회하면서 해당이벤트로 발생하는 퀘스트가 있는지 검사하고
	// 있다면 퀘스트를 생성시킨다.
	XEventInfo info;
	info.SetParam( eventQuest, dwParam );
	if( dwParam2 )
		info.SetParam2( dwParam2 );
	m_spAcc->GetpQuestMng()->DispatchEvent( eventQuest, info );
}

void XGameUser::DispatchQuestEvent( XGAME::xtQuestCond eventQuest, const _tstring& strParam )
{
	if( XBREAK( m_spAcc == nullptr ) )
		return;
	// 퀘스트리스트를 순회하면서 해당이벤트로 발생하는 퀘스트가 있는지 검사하고
	// 있다면 퀘스트를 생성시킨다.
	XEventInfo info;
	info.SetstrParam( strParam );
	m_spAcc->GetpQuestMng()->DispatchEvent( eventQuest, info );
}

/**
 @brief 개발용. 못받은 퀘를 모두 받게 한다.
*/
void XGameUser::UpdateOccurQuestCheat()
{
#ifdef _CHEAT
	// 레벨 이벤트.
	{
		xQuest::XEventInfo infoQuest;		// 이벤트 정보.
		infoQuest.SetLevel( m_spAcc->GetLevel() );
		infoQuest.SetbLevelUnder( true );	// 지정한 레벨이하의 퀘도 모두 발생되게
		DispatchQuestEvent( XGAME::xQC_EVENT_LEVEL, infoQuest );
	}
	// 지역오픈 이벤트
	{
		XArrayLinearN<XPropCloud::xCloud*, 512> aryOpened;
		GetpWorld()->GetOpenCloudsToAry( &aryOpened );
		XARRAYLINEARN_LOOP_AUTO( aryOpened, pProp ) {
			xQuest::XEventInfo infoQuest;		// 이벤트 정보.
			infoQuest.SetidArea( pProp->idCloud );
			DispatchQuestEvent( XGAME::xQC_EVENT_OPEN_AREA, infoQuest );
		} END_LOOP;
	}
	// 클리어 퀘스트 이벤트.
	{
		std::vector<ID> ary;
		m_spAcc->GetpQuestMng()->GetQuestsCompletedToAry( &ary );
		for( auto idQuest : ary ) {
			xQuest::XEventInfo infoQuest;		// 이벤트 정보.
			infoQuest.SetidQuest( idQuest );
			DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_QUEST, infoQuest );
		}
	}
#endif // _CHEAT
}

/**
 @brief 새 퀘스트 발생
 XQuestMng::DispatchEvent로부터 발생한다.
*/
void XGameUser::DelegateOccurQuest( XQuestObj *pQuestObj )
{
	XPacket ar( (ID)xCL2GS_QUEST_OCCUR );
	ar << pQuestObj->GetpProp()->idProp;
	ar << pQuestObj->GetsecUpdate();
	Send( ar );
	// 지역오픈류 퀘의경우
	if( pQuestObj->IsAreaQuest() ) {
		// 퀘가 원하는 지역이 이미 오픈되어있는지 검사하고,
		ID idArea = pQuestObj->GetidOpenAreaCondition();
		if( XASSERT( idArea ) ) {
			if( GetpWorld()->IsOpenedArea( idArea ) ) {
				XEventInfo info;
				info.SetidArea( idArea );
				// 그렇다면 그 지역을 파라메터로 해서 DispatchQuestEvent를 발생시킨다.
				DispatchQuestEvent( xQC_EVENT_OPEN_AREA, info );
			}
		}
	}
	// 이 퀘스트로부터 발생하는 스팟이 있는지 검사해서 생성한다.
	XVector<XSpot*> aryUpdatedSpot;
	UpdateSpotByQuest( pQuestObj, &aryUpdatedSpot );
	for( auto pSpot : aryUpdatedSpot ) {
		SendSpotSync( pSpot );
	}
	// 새 퀘스트를 받았을때 그퀘가 특정 스팟의 정찰이라면
	// 해당스팟을 얻어 공격금지로 설정해야 한다.
	if( pQuestObj->IsReconQuest() ) {
		ID idSpot = pQuestObj->GetidSpotOfCond();
		if( idSpot ) {
			auto pBaseSpot = m_spAcc->GetpWorld()->GetSpot( idSpot );
			if( XASSERT(pBaseSpot) ) {
				pBaseSpot->SetbitFlagByNoAttack( true );
// 				pBaseSpot->GetbitFlag().noAttack = 1;
				SendSpotSync( pBaseSpot );
			}
		}
	}
	// 스팟 클리어/정찰/방문퀘의 경우 만약 스팟이 사라졌다면 다시 나타나게 할것.
	// A스팟 정찰 퀘(a) 이후 A스팟 클리어 퀘(b)로 넘어갈때 a에서 해당 스팟을 파괴해버리면 퀘스트가 불가능해 진다.
	// 정찰퀘의 스팟일경우 공격이 안되게 만들어야 할듯.
}

/**
 @brief 이곳은 퀘조건 개수에 따라 여러번 호출될수도 있음.
*/
void XGameUser::DelegateOccurQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond )
{
	if( pCond->IsSpotClearType() ) {
		ID idSpot = pCond->GetpProp()->paramWhat.dwParam;
		if( XASSERT(idSpot) ) {
			auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
			if( XBREAK( pBaseSpot == nullptr ) )
				return;
			// 스팟클리어퀘의 경우 해당 스팟의 공격금지를 풀어준다.
			bool bNoAttack = pBaseSpot->IsNoAttack();
			pBaseSpot->SetbitFlagByNoAttack( false );
// 			pBaseSpot->GetbitFlag().noAttack = 0;
			if( bNoAttack )
				SendSpotSync( pBaseSpot );
		}
	}
	if( pCond->IsQuestType( XGAME::xQC_EVENT_GET_ITEM ) ) {
	} else
	if( pCond->IsQuestType( XGAME::xQC_EVENT_GET_STAR ) ) {
	} else {
		// 영웅레벨퀘의경우 퀘를 받는즉시 이미 조건완료된 영웅이 있는지 모든영웅들에게 이벤트를 한번씩 다 돌린다.
		const auto type = pCond->GetQuestType();
		UpdateQuestCondition( type );
	}
}

void XGameUser::UpdateQuestConditionEach( XGAME::xtQuestCond eventQuest, XGAME::xtQuestCond eventExcute )
{
	if( eventQuest == eventExcute || !eventQuest )
		m_spAcc->DispatchQuestEventCurrState( eventExcute );
}
/**
 @brief 모든 퀘스트에 대해서 현재상태를 기준으로 다시한번 조건 평가를 한다.
 퀘도 이미 받았고 이미 퀘스트 완료조건도 완수된상태라면 이벤트가 발생하지 않기때문에 그 퀘스트는 영원히 깰수 없다.
 그래서 접속시에 몇몇 이벤트들만 재평가를 한다.(버그 아니고서야 이런일이 생기나? 새 퀘를 추가해도 어차피 접속할때 퀘는 발생하는거 아닌가)
 @param eventQuest 불필요한 검색을 피하기 위해서 특정 이벤트만 재평가를 원할때 사용. xQC_NONE이면 모든 대상이벤트들을 재평가.
*/
void XGameUser::UpdateQuestCondition( XGAME::xtQuestCond eventQuest )
{
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_HERO_LEVEL );
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_HERO_LEVEL_SQUAD );
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_HERO_LEVEL_SKILL );
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_HAVE_ITEM );
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_HAVE_RESOURCE );
	UpdateQuestConditionEach( eventQuest, XGAME::xQC_EVENT_BUY_CASH );
// 	m_spAcc->DispatchQuestEventCurrState( XGAME::xQC_EVENT_HERO_LEVEL );
// 	m_spAcc->DispatchQuestEventCurrState( XGAME::xQC_EVENT_HERO_LEVEL_SQUAD );
// 	m_spAcc->DispatchQuestEventCurrState( XGAME::xQC_EVENT_HERO_LEVEL_SKILL );
// 	m_spAcc->DispatchQuestEventCurrState( XGAME::xQC_EVENT_HAVE_ITEM );
// 	m_spAcc->DispatchQuestEventCurrState( XGAME::xQC_EVENT_HAVE_RESOURCE );
}

/**
 @brief 퀘스트 조건 완수
*/
void XGameUser::DelegateCompleteQuest( XQuestObj *pQuestObj )
{
	// 퀘 완료 보내줌.
	XPacket ar( (ID)xCL2GS_QUEST_COMPLETE );
	ar << pQuestObj->GetidProp();
	XQuestObj::sSerialize( pQuestObj, ar );
	Send( ar );
// 	//
// 	xQuest::XEventInfo infoQuest;		// 이벤트 정보.
// 	infoQuest.SetidQuest( pQuestObj->GetpProp()->idProp );
// 	DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_QUEST, infoQuest );
// 	// 이 퀘스트의 완료로부터 삭제해야 하는 스팟이 있는지 검사해서 삭제
// 	DestroySpotByQuest( pQuestObj );
}

/**
 @brief 퀘스트 종료(보상까지 끝나서 퀘 삭제되기 직전)
*/
void XGameUser::DelegateFinishQuest( XQuestObj *pQuestObj, bool bItemSync )
{
	if( bItemSync )	{
		SendItemInvenSync();
	}
	if( pQuestObj->IsReconQuest() ) {
		ID idSpot = pQuestObj->GetidSpotOfCond();
		if( idSpot ) {
			auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
			if( pBaseSpot ) {
				pBaseSpot->SetbitFlagByNoAttack( false );
// 				pBaseSpot->GetbitFlag().noAttack = 0;
				SendSpotSync( pBaseSpot );
			}
		}
	}
	if( pQuestObj->GetstrIdentifer() == _T("scout_attack") ) {
		// 지역레벨에 따라 기능의 잠금해제를 한다. 
		auto bitUnlock = XGAME::xBM_BARRACK;
		m_spAcc->GetbitUnlockMenu().SetBit( static_cast<BIT>(bitUnlock) );
		SendUnlockMenu( bitUnlock );
	}
// 	else
// 	if( pQuestObj->GetstrIdentifer() == _T( "x.tutor.organize" ) ) {
// 		// 동키호테 배치퀘끝나면 영웅고용을 오픈한다.
// 		auto bitUnlock = XGAME::xBM_TAVERN;
// 		m_spAcc->GetbitUnlockMenu().SetBit( static_cast<BIT>( bitUnlock ) );
// 		SendUnlockMenu( bitUnlock );
// 	}

}
/**
 @brief 퀘스트의 완료로 인해 삭제되어야 하는 스팟들을 삭제시킨다.
*/
int XGameUser::DestroySpotByQuest( XQuestObj *pQuestObj )
{
	// 나중에 아이디를 키로 하는 방식으로 바꿀것.
	auto pList = PROP_WORLD->GetpPropWhenDel( pQuestObj->GetstrIdentifer() );
	if( pList )
	{
		for( auto pProp : *pList )
		{
			XSpot *pSpot = GetpWorld()->GetSpot( pProp->idSpot );
			if( pSpot )
			{
				pSpot->SetbDestroy( true );
				SendSpotSync( pSpot );
			}
//			GetpWorld()->DestroySpot( pProp->idSpot );
		}
	}
	return (pList != nullptr)? pList->size() : 0;
}

/**
 @brief 퀘스트 업데이트
*/
void XGameUser::DelegateUpdateQuest( XQuestObj *pQuestObj )
{
	XPacket ar( (ID)xCL2GS_QUEST_UPDATE );
	ar << pQuestObj->GetidProp();
	XQuestObj::sSerialize( pQuestObj, ar );
	Send( ar );
}

/**
 @brief 퀘스트로인해 드랍아이템이 발생되어야 함.
*/
void XGameUser::DelegateDropItemFromQuest( XQuestObj *pQuestObj, 
											ID idItem,
											XArrayLinearN<ItemBox,256> *pOut )
{
// 	int dice = xRandom(1000);
// 	if( dice < drop.prob * 1000 ) {
		auto pProp = PROP_ITEM->GetpProp( idItem );
		if( XASSERT(pProp) )
			pOut->Add( std::make_pair(pProp, 1) );
// 	}
}

/**
 @brief 퀘스트가 삭제되기전 발생.
*/
void XGameUser::DelegateDestroyQuest( XQuestObj *pQuestObj )
{
	//
	xQuest::XEventInfo infoQuest;		// 이벤트 정보.
	infoQuest.SetidQuest( pQuestObj->GetpProp()->idProp );
	DispatchQuestEvent( XGAME::xQC_EVENT_CLEAR_QUEST, infoQuest );
	// 이 퀘스트의 완료로부터 삭제해야 하는 스팟이 있는지 검사해서 삭제
	DestroySpotByQuest( pQuestObj );
}

/**
 완료된 퀘스트의 보상을 요청했다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqReqQuestReward()
*/
int XGameUser::RecvReqQuestReward( XPacket& p )
{
	ID idQuest;
	p >> idQuest;
	///< 
	if( DoRewardQuest( idQuest ) == 0 )
		return 0;

	_tstring strLog = XE::Format(_T("Quest Reward: %d"), idQuest);
	AddLog(XGAME::xULog_Quest_Reward, strLog);
	return 1;
}
/**
 @brief 
*/
int XGameUser::DoRewardQuest( ID idQuest )
{
	XQuestObj *pQuest = m_spAcc->GetpQuestMng()->GetQuestObj( idQuest );
	XVERIFY_BREAK( pQuest == nullptr );
	XVERIFY_BREAK( pQuest->IsAllComplete() == false );
	// 클라로 보내줄 패킷
	XPacket ar( (ID)xCL2GS_QUEST_REQUEST_REWARD );
	ar << idQuest;
	// 보상생성
	XArrayLinearN<XBaseItem*, 256> aryItems;
	auto& aryReward = pQuest->GetpProp()->aryReward;
	ar << (BYTE)aryReward.size();
	ar << (BYTE)VER_ITEM_SERIALIZE;
	ar << (WORD)0;
	int exp = pQuest->GetpProp()->GetExpReward( m_spAcc->GetLevel() );

	XVERIFY_BREAK( exp > 1000000 );		// 지나치게 높은값.
	m_spAcc->AddExp( exp );
	SendLevelSync();
//	m_spAcc->SerializeLevel( ar );
//	XARRAYLINEAR_LOOP_AUTO( aryReward, pReward ) {
	for( auto pReward : aryReward ) {
		ar << (int)pReward->rewardType;
		switch( pReward->rewardType ) {
		case XGAME::xtReward::xRW_ITEM: {
			auto pProp = PROP_ITEM->GetpProp( pReward->idReward );
			if( XASSERT(pProp) ) {
				aryItems.Clear();
				XBREAK( pReward->num <= 0 );
				m_spAcc->CreateItemToInven( pProp, pReward->num, &aryItems );
				ar << aryItems.size();
				XARRAYLINEARN_LOOP_AUTO( aryItems, pItem ) {
					if( pItem->GetType() == XGAME::xIT_BOOK ) {
						int expBook = XGC->m_expPerBook;
						if( pReward->dwParam > 0 )
							expBook = (int)pReward->dwParam;
						pItem->SetBookExp( expBook );
					}
					XBaseItem::sSerialize( ar, pItem );
				} END_LOOP;
			}
		} break;
		case XGAME::xtReward::xRW_RESOURCE: {
			ar << pReward->idReward;
			auto typeRes = (XGAME::xtResource)pReward->idReward;
			if( typeRes == XGAME::xRES_GOLD )
			{
				m_spAcc->AddGold( pReward->num );
				ar << m_spAcc->GetGold();
			} else
			{
				m_spAcc->AddResource( (XGAME::xtResource)pReward->idReward, pReward->num );
				// 더한값을 보내준다.
				ar << m_spAcc->GetResource( ( XGAME::xtResource )pReward->idReward );
			}
		} break;
		case XGAME::xtReward::xRW_CASH: {
			m_spAcc->AddCashtem( pReward->num );
			ar << m_spAcc->GetCashtem();
		} break;
		case XGAME::xtReward::xRW_HERO: {
			auto pPropHero = PROP_HERO->GetpProp( pReward->idReward );
			if( XASSERT(pPropHero) ) {
				auto unit = XGAME::GetUnitBySizeAndAtkType( xSIZE_SMALL, pPropHero->typeAtk );
				auto pHero = XHero::sCreateHero( pPropHero, 1, unit );
				if( XASSERT(pHero) ) {
					m_spAcc->AddHero( pHero );
					XHero::sSerialize( ar, pHero );
				}
			}
		} break;
		default:
			XBREAKF(1, "unknown reward type:%d", pReward->rewardType );
		}
	}
	//
	///< 
	Send( ar );

	// 퀘 종료 
	m_spAcc->GetpQuestMng()->DoExitQuest( pQuest );
	pQuest = nullptr;
	return 1;
}

/**
 @brief XQuestMng::DoExitQuest로부터 호출된다.
*/
void XGameUser::DelegateDestroyQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond )
{
	// 퀘완료후 퀘템을 삭제하는 조건인가.
	if( pCond->IsRewardAfterDelItem() )	
	{
		// 퀘템 아이디
		ID idItem = pCond->GetCondItem();
		auto pProp = PROP_ITEM->GetpProp( idItem );
		if( pProp )
		{
			// 퀘템이 전리품이 아니거나, 전리품이지만 반복퀘의 경우는 템 삭제
			if( !pProp->IsBooty() || ( pProp->IsBooty() && pQuestObj->IsRepeat()) )
			{
				XBREAK( pCond->GetpProp()->num == 0 );
				// 퀘템을 모두 삭제하면 안됨. 징표의 경우는 10개이상을 소지하고 있을수도 있음.
				// 그외 퀘템전용템은 퀘완료가 되면 아예 드랍이 되지 않아야 한다.
	#ifdef _DEBUG
				int numOld = m_spAcc->GetNumItems( idItem );
	#endif
				m_spAcc->DestroyItem( idItem, pCond->GetpProp()->num );
	#ifdef _DEBUG
				int numAfter = m_spAcc->GetNumItems( idItem );
				int a = 0;
	#endif
	//			SendItemInvenSync();
			}
		}
	}
	// 별획득퀘의 경우 퀘 완료시 별 차감함.
	if( pCond->IsQuestType( XGAME::xQC_EVENT_GET_STAR ) ) {
		int numCurr = m_spAcc->GetnumStar();
		numCurr -= pCond->GetpProp()->num;
		XASSERT( numCurr >= 0 );			// 치트로 퀘를 깨다보면 이런경우 생김.
		if( numCurr < 0 )
			numCurr = 0;
		m_spAcc->SetnumStar( numCurr );

	}
}


/**
 클라에서 스팟을 터치(방문)했다. 
 주의: visit스팟만 오는게 아님.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendSpotVisit()
*/
int XGameUser::RecvSpotTouch( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	///< 
	XSpot* pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	pBaseSpot->OnTouch( m_spAcc );
	pBaseSpot->Update( m_spAcc );
	// 
	XArchive arAdd;	// 추가요소
	std::vector<xDropItem> aryDropItem;
	switch( pBaseSpot->GettypeSpot() )
	{
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_CASTLE: {
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_MANDRAKE: {
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		if( pSpot ) {
			pSpot->CalculateResource( m_spAcc->GetidAccount(), 0 );
		}
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_NPC: {
		auto pSpot = SafeCast<XSpotNpc*>( pBaseSpot );
		// 스팟에 등록된 드랍리스트를 얻는다.
		pSpot->GetDropItems( &aryDropItem );
		xQuest::XEventInfo infoQuest;
		infoQuest.SetidArea( pSpot->GetidArea() );
		infoQuest.SetidSpot( pSpot->GetidSpot() );
		// 회색스팟은 해당사항 없도록 한다.
		if( m_spAcc->IsGreenOver( pSpot->GetPower() ) ) {
			// spot_type형태의 퀘만 회색스팟은 인정안되게 함. 다른형태의 퀘는 회색스팟이어도 클리어 됨.
			infoQuest.SettypeSpot( pSpot->GettypeSpot() );
		}
		// 추가로 퀘로 인해 떨어질 템목록을 추가한다.
		GetpQuestMng()->GetDropableItems( &aryDropItem, infoQuest );
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_CASH: {
		XSpotCash *pSpot = SafeCast<XSpotCash*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == nullptr );
		int numGet = pSpot->GetnumCash();
		m_spAcc->AddCashtem( numGet );
		arAdd << numGet;
		pSpot->Initialize( m_spAcc );
		SendBaseInfo();		// 패킷 최적화 대상.
		//
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_DAILY: {
		if( pBaseSpot->GetspLegion() == nullptr )
			pBaseSpot->CreateLegion( m_spAcc );
//		SendSpotSync( pBaseSpot );
	} break;
	case XGAME::xSPOT_PRIVATE_RAID: {
	} break;
	//////////////////////////////////////////////////////////////////////////
	case XGAME::xSPOT_COMMON: {
		auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
		XVERIFY_BREAK( pSpot == nullptr );
		if( pSpot->IsGuildRaid() ) {
			auto pPropCamp = pSpot->GetpPropCamp(); 
			auto spCampObj = pSpot->GetspCampObj( GetidGuild() );
			if( spCampObj == nullptr ) {
				// 아직 캠페인이 안만들어졌다면 저장해야할지도 모르니 생성해서 보냄.
				spCampObj = xCampaign::XCampObj::sCreateCampObj( pPropCamp, 0 );
			}
			// DB서버로 길드레이드 캠페인데이타 요청
			ID idKey = DBA_SVR->SendReqCampaignByGuildRaid( GetidAcc(),
													m_spAcc->GetidGuild(),
													pPropCamp->idProp,
													idSpot, spCampObj );
			DBA_SVR->AddResponse( idKey, this, &XGameUser::RecvReqCampaignByGuildRaid );
		} else
		if( pSpot->IsMedalCamp() ) {
			auto pPropCamp = pSpot->GetpPropCamp(); 
			XBREAKF( pPropCamp == nullptr, "propCamp == null: %s", pSpot->GetpProp()->strParam.c_str() );
			auto spCampObj = pSpot->GetspCampObj( nullptr );
			if( spCampObj ) {
				spCampObj->Update( m_spAcc );
			}
		} else
		if( pSpot->IsHeroCamp() ) {
			auto spCampObj = pSpot->GetspCampObj( nullptr );
			if( spCampObj )
				spCampObj->Update( m_spAcc );
		}
	} break;
	default:
		break;
	}
	// 퀘스트 처리
	xQuest::XEventInfo infoQuest;
	infoQuest.SetidSpot( pBaseSpot->GetidSpot() );
	infoQuest.SettypeSpot( pBaseSpot->GettypeSpot() );
	infoQuest.SetidCode( pBaseSpot->GetpBaseProp()->idCode );
	infoQuest.SetidArea( pBaseSpot->GetidArea() );
	DispatchQuestEvent( XGAME::xQC_EVENT_VISIT_SPOT, infoQuest );
	// 클라로 보내줄 패킷
	XPacket ar( (ID)xCL2GS_LOBBY_SPOT_TOUCH );
	ar << idSpot;
	ar << (int)0;   // subtype
// 	ar << m_spAcc->GetCashtem();
	XSpot::sSerialize( ar, pBaseSpot );
	ar << aryDropItem;
	ar << arAdd;
	Send( ar );

	return 1;
}

/**
 @brief DB서버로부터 캠페인정보가 도착함.
*/
void XGameUser::RecvReqCampaignByGuildRaid( XPacket& p )
{
	ID idCamp, idSpot;
	int bFound;
	XArchive arCamp;
	p >> idCamp >> idSpot;
	p >> bFound;
	if( bFound ) {
		p >> arCamp;
		// 길드캠페인객체는 스팟에 저장하지 않고 길드데이타에 저장함.
		ID idGuild = m_spAcc->GetidGuild();
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( XASSERT(pGuild) ) {
			auto spCampObj = pGuild->FindspRaidCampaign( idCamp );
			// DB서버에서 캠페인을 보내기전에 모든게임서버로 UpdateCampaign을 한상태이므로 캠페인데이타는 반드시 있어야 한다.
			if( XASSERT(spCampObj) ) {
				// 정책상 길드캠페인 객체는 스팟이 갖고 있지 않으나 여러가지 문제가 있어서 구조적 변경이 있기전까진 스팟에도 갖고 있는걸로 함.
				auto pSpot = SafeCast2<XSpotCommon*>( GetpWorld()->GetpSpot(idSpot) );
				if( XASSERT(pSpot) ) {
					pSpot->SetspCampObj( spCampObj );
				}
				XPacket ar( (ID)xCL2GS_LOBBY_REQ_CAMPAIGN_GUILD_RAID );
				ar << idSpot;
				ar << idCamp;
				ar << bFound;      // 
				if( bFound )
					ar << arCamp;
				Send( ar );
			}
		}
	}
	//
}

void XGameUser::DoAddPostInforewardtable(XGAME::xtPostType e, ID idrewardtable)
{
	/*if (DBA_SVR)
	{
		XArchive ar;

		ar << (ID)e;
		ar << idrewardtable;

		if (DBA_SVR)
		{
			ID idKey = DBA_SVR->SendPOSTInfoAdd(GetidAccount(), m_spAcc->GenerateSN(), (ID)(XGAME::xtPostType::xPOSTTYPE_TABLE), ar, xCL2GS_LOBBY_POST_ADD);
			DBA_SVR->AddResponse(idKey, this, &XGameUser::cbDoAddPostInforewardtable);
		}
	}*/
}

// void XGameUser::DoAddPostInfoVec(XGAME::xtPostType e, std::vector<XPostInfo::sPostItem>& Vec)
// {
// 	/*if (DBA_SVR)
// 	{
// 		XArchive ar;
// 
// 		ar << (ID)e;
// 
// 		int count = (int)Vec.size();
// 		ar << count;
// 
// 		for (int n = 0; n < count; n++)
// 		{
// 			if (Vec[n].sidprop > 0)
// 			{
// 				if (Vec[n].sCount == 0 || Vec[n].sCount > 10)
// 					Vec[n].sCount = 1;
// 			}
// 			ar << Vec[n].sidprop;
// 			ar << Vec[n].sCount;
// 		}
// 		if (DBA_SVR)
// 		{
// 			ID idKey = DBA_SVR->SendPOSTInfoAdd(GetidAccount(), m_spAcc->GenerateSN(), (ID)(XGAME::xtPostType::xPOSTTYPE_VECTOR), ar, xCL2GS_LOBBY_POST_ADD);
// 			DBA_SVR->AddResponse(idKey, this, &XGameUser::cbDoAddPostInfoVec);
// 		}
// 	}*/
// }

// void XGameUser::DoAddPostInfo(_tstring strSenderName, _tstring strTitle, _tstring strMessage, std::vector<XPostInfo::sPostItem>& Vec)
// {
// 	if (DBA_SVR)
// 	{
// 		XArchive ar;
// 		
// 		ar << (ID)XGAME::xtPostType::xPOSTTYPE_NORMAL;
// 
// 		ar.WriteString(strSenderName.c_str());	//Sender
// 		ar.WriteString(m_spAcc->GetstrName());		//Recv			
// 		ar.WriteString(strTitle.c_str());		//Title
// 		ar.WriteString(strMessage.c_str());		//Message
// 
// 		int count = (int)Vec.size();
// 		ar << count;
// 
// 		for (int n = 0; n < count; n++)
// 		{
// 			if (Vec[n].sidprop > 0)
// 			{
// 				if (Vec[n].sCount == 0 || Vec[n].sCount > 10)
// 					Vec[n].sCount = 1;
// 			}
// 			ar << Vec[n].sidprop;
// 			ar << Vec[n].sCount;
// 		}
// 		ID idKey = DBA_SVR->SendPOSTInfoAdd( GetidAcc()
// 																			, m_spAcc->GenerateSN()
// 																			, xPOSTTYPE_NORMAL
// 																			, ar
// 																			, xCL2GS_LOBBY_POST_ADD);
// 		DBA_SVR->AddResponse(idKey, this, &XGameUser::cbDoAddPostInfo);
// 	}
// }

// void XGameUser::cbDoAddPostInfoVec(XPacket& p)
// {
// 	int success, nCount;
// 	ID idaccount, postsn, cbidpacket, posttype;
// 
// 	p >> idaccount;
// 	p >> postsn;	
// 	p >> posttype;
// 	p >> success;
// 	p >> nCount;
// 	p >> cbidpacket;
// 
// 	XPacket ar((ID)cbidpacket);
// 	ar << success;
// 
// 	if( success ) {
// 		XPostInfo* pPost = new XPostInfo;
// 		pPost->SetIndex( postsn );
// 		pPost->SetPostType( ( XGAME::xtPostType )posttype );
// 
// 		for( int n = 0; n < nCount; n++ ) {
// 			XPostItem* pItemData = new XPostItem;
// 			pItemData->DeSerialize( p );
// 			pPost->AddlistPostItems( pItemData );
// 		}
// 		m_spAcc->GetPostInfo()->Add( pPost );
// 
// 		pPost->Serialize( ar );
// 		Send( ar );
// 		return;
// 	}
// 	Send(ar);
// 
// }

// void XGameUser::cbDoAddPostInforewardtable(XPacket& p)
// {
// 	int success;
// 	ID idaccount, postsn, cbidpacket, posttype, rewardid;
// 	
// 	p >> idaccount;
// 	p >> postsn;	
// 	p >> posttype;
// 	p >> success;
// 	p >> rewardid;
// 	p >> cbidpacket;
// 	
// 	XPacket ar((ID)cbidpacket);
// 	ar << success;
// 
// 	if (success)
// 	{
// 		XPostInfo* pPost = new XPostInfo;
// 		pPost->SetIndex(postsn);
// 		pPost->SetRewardTableid(rewardid);
// 		
// 		//Rewardtable 같은것이 있으면 보상 품목을 채워주던가....찾을떄 채워주자.
// 		m_spAcc->GetPostInfo()->Add(pPost);
// 
// 		pPost->Serialize(ar);
// 		Send(ar);
// 		return;
// 	}
// 	Send(ar);
// }

/**
 @brief 월정액상품의 지급메일을 보낸다. 
*/
void XGameUser::AddPostSubscribe( XGAME::xtPlatform platform )
{
	// 월정액 상품은 하나밖에 없어서 이렇게 함.
	auto pGoodsInfo = XGlobalConst::sGet()->GetCashItem( _T("inapp_cash00500"), platform );
	if( XASSERT( pGoodsInfo ) ) {
		XPostInfo post( m_spAcc->GenerateSN() );
		post.SetidTextSender( 2305 );
		post.SetstrRecvName( GetstrName() );
		post.SetidTextTitle( 2306 );
		post.SetidTextMsg( 2307 );
		post.AddPostGem( pGoodsInfo->m_gem );	// 매일 지급되어야 할 젬 개수
		// 우편을 보냄
		DoSendPost( &post );
// #ifdef _DEBUG
		CONSOLE_ACC( TAG_POST, "Add post subscribe");
// #endif // _DEBUG
	}
}

/**
 @brief 메일을 한통 보낸다.
*/
void XGameUser::DoSendPost( const XPostInfo* pPostInfo )
{
	ID idKey = DBA_SVR->SendPOSTInfoAdd( GetidAcc()
																		, xPOSTTYPE_NORMAL
																		, (*pPostInfo) );
	DBA_SVR->AddResponse( idKey, this, &XGameUser::cbDoAddPostInfo );
}


/**
 @brief	DB서버로부터 검증받은 메일을 계정내 리스트에 추가하고 클라로 보낸다.
*/
void XGameUser::cbDoAddPostInfo( XPacket& p )
{
	BYTE b0;
	p >> b0;		auto type = (xtPostType)b0;
	p >> b0;		int result = b0;
	p >> b0 >> b0;
	XPostInfo* pPostInfo = nullptr;
	if( result ) {
		pPostInfo = new XPostInfo();
//		p >> ( *pPostInfo );
		pPostInfo->DeSerialize( p, VER_POST );
//		m_spAcc->GetPostInfo()->Add( pPostInfo );
		m_spAcc->AddPostInfo( pPostInfo );
	}
	//
	XPacket ar( (ID)xCL2GS_LOBBY_POST_ADD );
	ar << (BYTE)type;
	ar << (BYTE)result;
	ar << (BYTE)0;
	ar << (BYTE)0;
	if( pPostInfo )
		ar << (*pPostInfo);
	Send( ar );
	// 접속하지 못해서 못받은 월정액상품이 더이상 없을때까지 리커시브
	DoSubscribeAsyncRecursive( m_Platform );
}
// void XGameUser::cbDoAddPostInfo( XPacket& p )
// {
// 	int success, nCount;
// 	ID postsn, cbidpacket, posttype;
// 	_tstring strSendername;
// 	_tstring strReceivename;
// 	_tstring strTitle;
// 	_tstring strMessage;
// 	XArchive postitemdata;
// 	p >> postsn;
// 	p >> posttype;
// 	p >> success;
// 	p >> strSendername;
// 	p >> strReceivename;
// 	p >> strTitle;
// 	p >> strMessage;
// 	p >> nCount;
// 	p >> cbidpacket;
// 	//p >> postitemdata;	
// 
// 	XPacket ar( (ID)cbidpacket );
// 	ar << success;
// 
// 	if( success ) {
// 		XPostInfo* pPost = new XPostInfo;
// 		pPost->SetIndex( postsn );
// 		pPost->SetPostType( ( XGAME::xtPostType )posttype );
// 		pPost->SetstrSenderName( strSendername.c_str() );
// 		pPost->SetstrRecvName( strReceivename.c_str() );
// 		pPost->SetstrTitle( strTitle.c_str() );
// 		pPost->SetstrMessage( strMessage.c_str() );
// 
// 		for( int n = 0; n < nCount; n++ ) {
// 			XPostItem* pItemData = new XPostItem;
// 			pItemData->DeSerialize( p );
// 			pPost->AddlistPostItems( pItemData );
// 		}
// 		m_spAcc->GetPostInfo()->Add( pPost );
// 
// 		pPost->Serialize( ar );
// 		Send( ar );
// 		return;
// 	}
// 	Send( ar );
// }

// void XGameUser::cbDoSubscribeAsync( XPacket& p )
// {
// 	// DB서버로부터 검증받은 메일을 계정내 리스트에 추가하고 클라로 보낸다.
// 	cbDoAddPostInfo( p );
// 	// 접속하지 않아서 못받은 월정액상품을 보낸다.
// 	DoSubscribeAsync();
// }


/**
 @brief 접속하지 않아서 못받은 월정액상품을 보낸다.
 한번에 한개의 메일을 보내므로 offline일수가 없어질때까지 리커시브로 불린다.
 AddPostSubscribe의 응답함수에서 다시 이것이 불린다.
*/
void XGameUser::DoSubscribeAsyncRecursive( XGAME::xtPlatform platform )
{
	// 못받은 일수가 남아있다면 계속 메일을 보냄.
	const auto cntOffline = m_spAcc->GetcntSubscribeOffline();
	if( cntOffline > 0 ) {
		if( XBREAK( cntOffline > 100 ) ) {
			// 못받은 유료상품이 100개이상인것은 뭔가 이상한 계정임. 더이상 요청하지 말고 로그남겨야 함.
		} else {
			AddPostSubscribe( platform );
			m_spAcc->AddnumSubscribe( 1 );		// 한개씩 증가
			m_spAcc->AddcntSubscribeOffline( -1 );
			// secNext는 offline cnt계산할때 다 적용함.
		}
	}
}

void XGameUser::Cheat_AddPostInfo()
{
#ifdef _CHEAT
	XPostInfo post( m_spAcc->GenerateSN() );
	post.SetstrSenderName( _T("tester") );
	post.SetstrRecvName( m_spAcc->GetstrName() );
	post.SetstrTitle( _T("This is title") );
	post.SetstrMessage( _T("This is message.") );
	post.AddPostGem( 100 );
	// 우편을 보냄
	DoSendPost( &post );
//	if( DBA_SVR ) {
// 		XArchive ar;
// 
// 		ar.WriteString( m_spAcc->GetstrName() ); //Sender
// 		ar.WriteString( m_spAcc->GetstrName() ); //Recv			
// 		ar.WriteString( _T( "This is title" ) );		 //Title
// 		ar.WriteString( _T( "This is message" ) ); //Message
// 
// 		int count = 2;// ::rand() % 4; //첨부 아이템 개수
// 		ar << count;
// 
// 		for( int n = 0; n < count; n++ ) {
// 			int ncount = ( ::xRand() % 9 ) + 1;
// 			XPropItem::xPROP* pData = PROP_ITEM->GetpPropRandom();
// 			XBREAK( pData == nullptr );
// 
// 			ar << (ID)XGAME::xtPOSTResource::xPOSTRES_ITEMS;
// 			ar << (ID)pData->idProp;
// 			ar << (ID)ncount;
// 		}
// 		ID idKey = DBA_SVR->SendPOSTInfoAdd( GetidAcc()
// 																				, m_spAcc->GenerateSN()
// 																				, (ID)( XGAME::xtPostType::xPOSTTYPE_NORMAL )
// 																				, ar
// 																				, xCL2GS_LOBBY_POST_ADD );
//		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbDoAddPostInfo );

		//DBA_SVR->SendPOSTInfoAdd(ar);
//	}
#endif // _CHEAT
}
void XGameUser::Cheat_AddPostInfoGuildCoin()
{
#ifdef _CHEAT
	XPostInfo post( m_spAcc->GenerateSN() );
	post.SetstrSenderName( _T( "tester" ) );
	post.SetstrRecvName( m_spAcc->GetstrName() );
	post.SetstrTitle( _T( "This is title" ) );
	post.SetstrMessage( _T( "This is message." ) );
	post.AddPostGuildCoin( 50 );
	// 우편을 보냄
	DoSendPost( &post );
// 	if( DBA_SVR ) {
// 		XArchive ar;
// 		ar << m_spAcc->GetstrName(); //Sender
// 		ar << m_spAcc->GetstrName(); //Recv			
// 		ar << _T( "This is title." );		 //Title
// 		ar << _T( "This is message." ); //Message
// 		int count = 1;// ::rand() % 4; //첨부 아이템 개수
// 		ar << count;
// 		XPostItem postItem( XGAME::xPOSTRES_GUILD_COIN, 100 );
// 		postItem.Serialize( ar );
// 		ID idKey = DBA_SVR->SendPOSTInfoAdd( GetidAcc(), m_spAcc->GenerateSN(), 
// 											(ID)( XGAME::xtPostType::xPOSTTYPE_NORMAL ), 
// 											ar, xCL2GS_LOBBY_POST_ADD );
// 		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbDoAddPostInfo );
// 	}
#endif // _CHEAT
}
/**
 @brief 캠페인 UI에서 스테이지를 선택했다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqClickStageInCampaign()
*/
int XGameUser::RecvClickStageInCampaign( XPacket& p )
{
	ID idSpot, snSpot, idPropStage, idWnd;
	char c0;
	//
	p >> idSpot;
	p >> snSpot;
	p >> c0;		const int idxStage = c0;
	p >> c0;		const int idxFloor = c0;
	p >> c0 >> c0;
	p >> idPropStage;
	p >> idWnd;
	//
	XSpot *pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	XVERIFY_BREAK( pBaseSpot->GetsnSpot() != snSpot );
	XVERIFY_BREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_COMMON &&
					 pBaseSpot->GettypeSpot() != XGAME::xSPOT_CAMPAIGN );
	auto spCampObj = pBaseSpot->GetspCampObj();
	XVERIFY_BREAK( spCampObj == nullptr );
	auto spStageObj = spCampObj->GetspStage( idxStage, idxFloor );
	XVERIFY_BREAK( spStageObj == nullptr );
	auto spPropStage = spStageObj->GetspPropStage();
	XVERIFY_BREAK( spPropStage == nullptr );
	XVERIFY_BREAK( spPropStage->idProp != idPropStage );
	XGAME::xtError errCode = XGAME::xE_OK;
	do  {
		const int lvAcc = m_spAcc->GetLevel();
		const int lvLimit = spCampObj->GetlvLimit( idxStage, idxFloor );
		if( lvLimit != 0 && lvAcc < lvLimit ) {
			errCode = XGAME::xE_NOT_ENOUGH_LEVEL;
			break;
		}
		if( m_spAcc->GetAP() < m_spAcc->GetAPPerBattle() ) {
			errCode = XGAME::xE_NOT_ENOUGH_AP;
			break;
		}
		errCode = spCampObj->IsAbleTry( spStageObj, idxFloor );
	} while (0);
	//
	bool bUpdate = false;
	if( errCode == xE_OK ) {
		if( spStageObj->GetspLegion() == nullptr ) {
			spStageObj->CreateLegion( spCampObj, m_spAcc->GetLevel(), idxFloor );
			bUpdate = true;
		}
		// 스테이지에서 드랍할수 있는 아이템을 스팟에 등록한다.
		if( spStageObj->SetDropItemsFromProp( spCampObj ) )
			bUpdate = true;
		pBaseSpot->SetDropItems( spStageObj->GetaryDrops() );
	}
		

	//
	XPacket ar( (ID)xCL2GS_CAMP_CLICK_STAGE );
	ar << idSpot;
	ar << snSpot;
//	ar << idxStage;
	ar << idPropStage;
	ar << idWnd;
	ar << (BYTE)errCode;
	ar << (BYTE)xboolToByte( bUpdate );
	ar << (char)idxStage;
	ar << (char)idxFloor;
	if( bUpdate ) {	// 클라에서 캠페인객체가 계속 삭제되었다 재생성 되어서 이것도 계속 보내야할듯 싶다.
		XArchive arStage;
		XStageObj::sSerialize( arStage, spStageObj );
		ar << arStage;
	}
	Send( ar );
	

// 	return DoSpotAttackCampaign( pBaseSpot, idxStage );
	return 1;
}

/**
 @brief 캠페인 공격의 경우 처리
*/
int XGameUser::DoSpotAttackCampaign( XSpot *pBaseSpot, int idxStage, int idxFloor )
{
	int bOk = 1;
	do {
		XBREAK( pBaseSpot == nullptr );
		ID idSpot = pBaseSpot->GetidSpot();
		XVERIFY_BREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_CAMPAIGN && 
									pBaseSpot->GettypeSpot() != XGAME::xSPOT_COMMON );
		xCampaign::CampObjPtr spCampObj;
		//////////////////////////////////////////////////////////////////////////
		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CAMPAIGN ) {
			auto pSpot = static_cast<XSpotCampaign*>( pBaseSpot );
			XVERIFY_BREAK( pSpot == nullptr );
			spCampObj = pSpot->GetspCampObj();
		}
		else
		//////////////////////////////////////////////////////////////////////////
		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_COMMON ) {
			auto pSpot = static_cast<XSpotCommon*>( pBaseSpot );
			XVERIFY_BREAK( pSpot == nullptr );
			spCampObj = pSpot->GetspCampObj( GetidGuild() );
			XVERIFY_BREAK( spCampObj == nullptr );      // 캠페인 스테이지 눌렀는데 캠페인오브젝트가 없으면 안됨.
			if( pSpot->IsGuildRaid() ) {
				XVERIFY_BREAK( spCampObj->FindTryer( GetidAcc() ) );	// 이미 한번 도전했으면 더이상 안됨.
			} else 
			if( pSpot->IsMedalCamp() ) {
			
			}
		}
		XVERIFY_BREAK( spCampObj == nullptr );      // 캠페인 스테이지 눌렀는데 캠페인오브젝트가 없으면 안됨.
		XVERIFY_BREAK( idxStage < 0 || idxStage >= spCampObj->GetNumStages( idxFloor ) );
		// 각 스테이지의 전투력을 알아야 해서 군단을 모두 만드는것으로 한다.
	//	spCampObj->CreateLegionAll( m_spAcc->GetLevel() );	// 스팟터치시 만드는걸로 변경
		//
		auto spStageObj = spCampObj->GetspStage( idxStage, idxFloor );
		XVERIFY_BREAK( spStageObj == nullptr );
		auto spPropStage = spStageObj->GetspPropStage();
		XVERIFY_BREAK( spPropStage == nullptr );
		const int lvLimit = spCampObj->GetlvLimit( idxStage, idxFloor );
		XVERIFY_BREAK( lvLimit && m_spAcc->GetLevel() < lvLimit );
// 		XVERIFY_BREAK( spPropStage->levelLimit != 0 &&
// 			m_spAcc->GetLevel() < spPropStage->levelLimit );
		XVERIFY_BREAK( spCampObj->IsAbleTry( spStageObj, idxFloor ) != XGAME::xE_OK );
		XBREAK( spStageObj->GetspLegion() == nullptr );	// 이제 각 스테이지의 전투력을 알수 있어야 하므로 스팟을 터치하는순간에 이미 군단이 만들어지므로 여기서 군단이 없어선 안된다.
		//////////////////////////////////////////////////////////////////////////
		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CAMPAIGN ) {
			auto pSpot = static_cast<XSpotCampaign*>( pBaseSpot );
//			XVERIFY_BREAK( m_spAcc->GetAP() < m_spAcc->GetAPPerBattle() );
			// 스팟에 군단이 지정되어 있지 않으면 바인딩 시킴
			if( pSpot->GetspLegion() == nullptr )
				pSpot->SetspLegion( spStageObj->GetspLegion() );
			spCampObj->DoEnterStage( idxStage, idxFloor );
			//
			XArchive arParam;
			arParam << idxStage;
// 			arParam << spPropStage->legion.lvLegion;
			arParam << spPropStage->m_spxLegion->lvLegion;
			arParam << idxFloor;
			SendBattleInfoWithidAccParam( pBaseSpot, 0, &arParam );
		}	else
		//////////////////////////////////////////////////////////////////////////
		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_COMMON ) {
			auto pSpot = static_cast<XSpotCommon*>( pBaseSpot );
			auto spLegion = spStageObj->GetspLegion();
			XVERIFY_BREAK( spLegion == nullptr );
			pSpot->SetspLegion( spLegion );
			int power = XLegion::sGetMilitaryPower( spLegion );  // 적군단의 전투력
			pSpot->SetPower( power );
			if( pSpot->IsGuildRaid() ) {
				// 누가 전투중이고 아직 시간이 안됬다면.
				if( spCampObj->GetidAccTrying() != GetidAcc()
					&& !spCampObj->IsTimeOverTry() ) {
					SendBattleInfoByGuildRaid( pBaseSpot,
												idxStage,
												spPropStage->m_spxLegion->lvLegion,
												XGAME::xGE_ERROR_STILL_TRYING_RAID );
					bOk = 0;	// 진입 실패
				}	else {
					// 진입요청
					ID idKey = DBA_SVR->SendReqEnterGuildRaid( GetidAcc(),
															GetidGuild(),
															spCampObj->GetidProp(),
															idSpot,
															GetstrName(),
															m_spAcc->GetLevel(),
															m_spAcc->GetPowerExcludeEmpty() );
					DBA_SVR->AddResponse( idKey, this, &XGameUser::RecvReqEnterGuildRaid );
				}
			} else
			if( pSpot->IsMedalCamp() ) {
				spCampObj->DoEnterStage( idxStage );
				//
				XArchive arParam;
				arParam << idxStage;
				arParam << spPropStage->m_spxLegion->lvLegion;
				arParam << idxFloor;
				SendBattleInfoWithidAccParam( pBaseSpot, 0, &arParam );
			} else
			if( pSpot->IsHeroCamp() ) {
				auto spCampObjHero = std::static_pointer_cast<XCampObjHero2>( spCampObj );
				if( XASSERT(spCampObjHero) ) {
					spCampObj->DoEnterStage( idxStage, idxFloor );
					spCampObjHero->SetidxFloorByBattle( idxFloor );		// 어떤 층의 전투를 시작했는지 서버에서 받아둬야 함.
					//
					XArchive arParam;
					arParam << idxStage;
					arParam << spStageObj->GetLevelLegion();
					arParam << idxFloor;
					SendBattleInfoWithidAccParam( pBaseSpot, 0, &arParam );
				}
			}
		}
		spCampObj->SetidxLastPlay( spStageObj->GetidxStage() );
	} while (0);
	return bOk;
}
/**
 @brief DB서버로부터 길드레이드 진입요청결과가 돌아옴.
*/
void XGameUser::RecvReqEnterGuildRaid( XPacket& p )
{
	ID idCamp, idSpot;
	int i0;
	int idxStage;

	XArchive arCamp;
	p >> idCamp >> idxStage >> idSpot;
	p >> i0;	auto result = (XGAME::xtGuildError)i0;
	p >> arCamp;		// 최신 캠페인 정보
	//
	auto pGuild = XGuildMgr::sGet()->FindGuild( GetidGuild() );
	if( XBREAK( pGuild == nullptr ) ) 
		return;
	pGuild->DeSerializeUpdateGuildRaidCamp( arCamp, idCamp );
	auto spCampObj = pGuild->FindspRaidCampaign( idCamp );
	if( XBREAK(spCampObj == nullptr) )
		return;
	if( result == XGAME::xGE_SUCCESS ) {
		spCampObj->DoEnterStage( idxStage );
	} else 
	if( result == XGAME::xGE_ERROR_STILL_TRYING_RAID ) {

	} else {
		XBREAKF(1,"raid enter result=%d", result );
	}
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		auto spStageObj = spCampObj->GetspStage( idxStage );
		pBaseSpot->SetspLegion( spStageObj->GetspLegion() );
		SendBattleInfoByGuildRaid( pBaseSpot, 
									spCampObj->GetidxLastUnlock(),
									spCampObj->GetspStageLastUnlock()->GetspPropStage()->m_spxLegion->lvLegion,
									result );
	}
}

void XGameUser::SendBattleInfoByGuildRaid( XSpot *pBaseSpot, int idxStage, int levelLegion, XGAME::xtGuildError err )
{
	XArchive arParam;
	arParam << idxStage;
	arParam << levelLegion;
	arParam << (int)err;
	SendBattleInfoWithidAccParam( pBaseSpot, 0, &arParam );
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqSendReqGuildRaidOpen()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSendReqGuildRaidOpen()
*/
int XGameUser::RecvSendReqGuildRaidOpen( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	auto spCampObj = pSpot->GetspCampObj( GetidGuild() );
	XVERIFY_BREAK( spCampObj == nullptr );
	spCampObj->CreateLegionAll(m_spAcc->GetLevel());	// 레이드 시작과 동시에 군단을 생성시킨다.
	XArchive arCampLegion;
	XCampObj::sSerializeLegion( spCampObj, arCampLegion );	// 군단정보만 묶어 보낸다.
	ID idKey =
		DBA_SVR->SendReqGuildRaidOpen( GetidAcc(), GetidGuild(), spCampObj->GetidProp(), idSpot, arCampLegion );
	DBA_SVR->AddResponse( idKey, this, &XGameUser::RecvGuildRaidOpenFromDB );
	return 1;
}


void XGameUser::RecvGuildRaidOpenFromDB( XPacket& p )
{
	ID idCamp, idSpot;
	int i0;
	XArchive arCamp;
	p >> idCamp >> idSpot;
	p >> i0;
	p >> arCamp;
	//
	XPacket ar( (ID)xCL2GS_OPEN_GUILD_RAID );
	ar << idCamp << idSpot;
	ar << arCamp;
	Send( ar );
}

/**
 클라이언트의 SendReqTrainComplete()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqTrainComplete()
*/
int XGameUser::RecvCheckTrainComplete( XPacket& p )
{
	ID snSlot;
	p >> snSlot;
	// idxSlot이 훈련이 끝난것 같다고 클라에서 알려옴.
//	bool bNotYet = true;		// 아직 안끝남.
	ID snHero = 0;
	// 어차피 렉에의해 이런경우가 발생해도 클라쪽에선 값들을 제대로 복구 못하므로 그냥 재 로딩하는게 낫다.
	XVERIFY_BREAK( !m_spAcc->IsTrainingSlot(snSlot) );
	xErrorCode result;
	auto pPropSlot = m_spAcc->GetpTrainingSlot( snSlot );
	if( pPropSlot ) {
		// 
		if( pPropSlot->IsComplete() )
			result = xEC_OK;
		else
			result = xEC_NOT_YET_FINISH;
	} else {
		// idxSlot은 훈련중이 아님
		result = xEC_IS_NOT_TRAINING;
	}
	SendTrainingComplete( snSlot, result );
	return 1;
}

int XGameUser::SendSyncTrainSlot()
{
	XPacket ar( (ID)xCL2GS_SYNC_TRAIN_SLOT );
	m_spAcc->SerializeTrainSlot( ar );
	Send( ar );
	return 1;
}

/**
 @brief 훈련을 끝냄
*/
int XGameUser::SendTrainingComplete(ID snSlot, xErrorCode result, XGAME::xtQuickComplete typeComplete )
{
// 	auto pSlot = m_spAcc->GetTrainingSlot( idxSlot );
	auto pSlot = m_spAcc->GetpTrainingSlot( snSlot );
	if( XBREAK(pSlot == nullptr) )
		return 0;
	auto pHero = m_spAcc->GetHero( pSlot->snHero );
	if( XBREAK( pHero == nullptr ) )
		return 0;
	auto type = pSlot->type;
	XPacket ar( (ID)xCL2GS_CHECK_TRAIN_COMPLETE );
	XBREAK( result > 0xffff );
	ar << (BYTE)VER_ETC_SERIALIZE;
	ar << (BYTE)type;
	ar << (WORD)result;
// 	ar << (BYTE)0;
	ar << snSlot;
	ar << pSlot->snHero;
	ar << m_spAcc->GetCashtem();
	ar << m_spAcc->GetGold();
	ar << (BYTE)typeComplete;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
// 
// 	if (quick)		// 즉시완료로 끝냄 여부
// 		ar << 1;
// 	else
// 		ar << 0;

	if( result == xEC_OK ) {
		int expAdd = 0;
//		if( type == XGAME::xTR_LEVEL_UP )
			expAdd = pSlot->GetAddExp();
		int bOk = m_spAcc->DoCompleteTraining( snSlot, pHero, expAdd );
		if( XBREAK(bOk == 0) )	// 이런경우가 생겨선 안됨.
			return 0;
		pSlot = nullptr;
// 		ar << (BYTE)VER_ETC_SERIALIZE;
// 		ar << (BYTE)type;
// 		ar << (BYTE)0;
// 		ar << (BYTE)0;
		m_spAcc->SerializeTrainSlot( ar );
// 		pHero->SerializeLevel( ar );
// 		pHero->SerializeLevelupReady( ar );
// 		pHero->SerializeProvided( ar );
		pHero->SerializeUpgrade( ar );
		pHero->GetbLevelUpAndClear( type );
		_tstring strLog = XE::Format( _T( "Training_Hero_Complete snSlot: %d, RemainCash: %d" ), snSlot, (int)m_spAcc->GetCashtem() );
		AddLog( XGAME::xULog_Hero_Train, strLog );
	} else
	{
		_tstring strLog = XE::Format(_T("Training_Hero_Failed snSlot: %d type: %d result=%d"), snSlot, pSlot->type, (int)result );
		AddLog(XGAME::xULog_Hero_Train, strLog);
	}

	Send( ar );

	return 1;
}


/**
 클라이언트의 SendReqResearch()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqResearch()
*/
int XGameUser::RecvResearch( XPacket& p )
{
	ID idNode, snHero;
	BYTE b0;
	WORD w0;
	p >> snHero;
	p >> b0;		auto unit = (XGAME::xtUnit)b0;
	p >> b0;		bool bByRemainPoint = xbyteToBool(b0);
	p >> w0;		idNode = w0;
	XVERIFY_BREAK( idNode == 0 );
	XVERIFY_BREAK( XVALID_UNIT(unit) == false );
	auto pProp = XPropTech::sGet()->GetpNode( unit, idNode );
	XVERIFY_BREAK( pProp == nullptr );
	XVERIFY_BREAK( unit != pProp->unit );
	auto pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	const auto abil = pHero->GetAbilNode( unit, idNode );
	XVERIFY_BREAK( abil.IsLock() );
	XVERIFY_BREAK( abil.point >= pProp->maxPoint );
	XVERIFY_BREAK( m_spAcc->IsEnableAbil( pHero, unit, pProp ) == false );
	int point = abil.point;
	if( bByRemainPoint ) {
		// 특성포인트로 연구.
		const int numRemainPoint = pHero->GetnumRemainAbilPoint();
		XVERIFY_BREAK( numRemainPoint <= 0 );
		// 여분 특성포인트 하나 뺌
		pHero->AddRemainAbilPoint( -1 );
		// 해당 특성에 포인트를 하나 올림.
		point = pHero->AddAbilPoint( unit, idNode );
		const _tstring strLog = XE::Format( _T( "Research_Tech_Start Unit: %d, Point: %d, idNode: %d, RemainGold: %d, numRemainPoint: %d" ), (int)unit, (int)abil.point, (int)idNode, (int)m_spAcc->GetGold(), numRemainPoint );
		AddLog( XGAME::xULog_Skill_Tree_Research, strLog );
	} else {
		// 자원과 시간으로 연구.
		XVERIFY_BREAK( m_spAcc->IsEnoughResourceForResearch( pHero/*, unit*/ ) == false );
		const auto& costAbil = pHero->GetCostAbilCurr( /*unit*/ );
		const int sec = costAbil.sec;
		// 필요한 자원을 뺌.
		const int sizeAry = costAbil.aryResourceNeed.size();
		for( int i = 0; i < sizeAry; ++i ) {
			auto resType = costAbil.aryResourceNeed[i];
			m_spAcc->AddResource(resType, -(costAbil.aryNumNeed[i]));
		}
		// 연구가 끝나면 +시켜야함.
		// 연구 시작.
		m_spAcc->GetResearching().Start( snHero, unit, idNode, sec );
		_tstring strLog = XE::Format( _T( "Research_Tech_Start Unit: %d, Point: %d, idNode: %d, RemainGold: %d, Sec: %d" ), (int)unit, (int)abil.point, (int)idNode, (int)m_spAcc->GetGold(), (int)sec );
		AddLog( XGAME::xULog_Skill_Tree_Research, strLog );
	}
	//
	XBREAK( idNode > 0xffff );
	XPacket ar( (ID)xCL2GS_TECH_RESEARCH );
	ar << (BYTE)xboolToByte( bByRemainPoint );
	ar << (char)point;
	if( bByRemainPoint ) {
		XBREAK( pHero->GetnumRemainAbilPoint() > 0xff );
		ar << (WORD)idNode;
		ar << snHero;
		ar << (BYTE)pHero->GetnumRemainAbilPoint();
		ar << (BYTE)unit;
		ar << (WORD)0;
	} else {
		ar << (BYTE)0;
		ar << (BYTE)0;
		m_spAcc->GetResearching().Serialize( ar );
		ar << m_spAcc->GetGold();
		m_spAcc->SerializeResource( ar );
	}
	Send( ar );
	// 
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_RESEARCH );
	DispatchQuestEvent( xQC_EVENT_RESEARCH_ABIL, idNode );
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqAbilLockFree()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqAbilLockFree()
*/
int XGameUser::RecvAbilLockFree( XPacket& p )
{
	ID idAbil, snHero;
	int i0;
	p >> snHero;
	p >> i0;		const auto unit = (XGAME::xtUnit) i0;
	p >> idAbil;
	auto pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	XVERIFY_BREAK( idAbil == 0 );
	XVERIFY_BREAK( XVALID_UNIT(unit) == false );
	auto pProp = XPropTech::sGet()->GetpNode( idAbil );
	XVERIFY_BREAK( pProp == nullptr );
	const auto err = m_spAcc->GetUnlockableAbil( pHero, unit, pProp );
	XVERIFY_BREAKF( err != xE_OK, "err = %d", err );
// 	XVERIFY_BREAK( m_spAcc->IsLockUnit(unit) );
// 	XVERIFY_BREAK( pProp->GetLvOpanable() > pHero->GetLevel() );
// 	XVERIFY_BREAK( m_spAcc->IsUnlockableAbil( pHero, unit, pProp) == false );
// 	XVERIFY_BREAK( goldNeed < 0 );
// 	XVERIFY_BREAK( m_spAcc->IsNotEnoughGold( goldNeed ) );
	const int goldNeed = pHero->GetGoldUnlockAbilCurr();	// 언락하기전에 해야 계산이 정확하게 나옴.
	// 특성을 잠금해제함.
	const bool bUnlock = pHero->SetUnlockAbil( unit, idAbil );
	XVERIFY_BREAK( bUnlock == false );

	const int numRemainUnlock = pHero->GetnumRemainAbilUnlock();
	if( numRemainUnlock > 0 ) {
		XBREAK( numRemainUnlock > 0xff );
		pHero->AddRemainAbilUnlock( -1 );
	} else {
		m_spAcc->AddGold( -( (int)goldNeed ) );
	}

	XPacket ar( (ID)xCL2GS_TECH_LOCK_FREE );
	ar << snHero;
	ar << (BYTE)unit;
	ar << (BYTE)pHero->GetnumRemainAbilUnlock();
	ar << (WORD)0;
	ar << idAbil;
	ar << m_spAcc->GetGold();
	Send( ar );
	_tstring strLog = XE::Format(_T("Research_Tech_LockFree: Unit[%d], AvilityID[%d], gold[%d], point[%d]")
		, (int)unit, (int)idAbil, (int)m_spAcc->GetGold(), numRemainUnlock);
	AddLog(XGAME::xULog_Skill_Tree_Research, strLog);	
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqResearchComplete()에 대한 서버측의 Receive함수
 클라측으로부터 "연구가 다 끝난거 같다. 확인해달라" 패킷이 날아옴.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqResearchComplete()
*/
int XGameUser::RecvCheckResearchComplete( XPacket& p )
{
	ID idAbil, snHero;
	p >> snHero;			// 업데이트를 요청한 영웅
	p >> idAbil;			// 업데이트를 요청한 현재 연구중인 특성아이디.
	XVERIFY_BREAK( idAbil == 0 );	// 
	XVERIFY_BREAK( snHero == 0 );
	auto& research = m_spAcc->GetResearching();
	do {
		auto pHero = m_spAcc->GetHero( research.GetsnHero() );
		if( pHero == nullptr ) {
			pHero = m_spAcc->GetHero( snHero );
			XVERIFY_BREAK( pHero == nullptr );
		} else
			snHero = pHero->GetsnHero();
		auto pPropTech = XPropTech::sGet()->GetpNode( idAbil );
		XVERIFY_BREAK( pPropTech == nullptr );
		xErrorCode codeError = xEC_OK;
		//	XGAME::xAbil *pAbil = nullptr;
		const auto abil = pHero->GetAbilNode( pPropTech->unit, idAbil );
		XVERIFY_BREAK( abil.IsLock() );
	//	XVERIFY_BREAK( pHero == nullptr );
		// 요청한 idAbil특성이 현재 연구중이 아니면 갱신정보를 보낸다.
		if( idAbil != research.GetidAbil() || !research.IsResearching() ) {
			// 타이밍문제로 비동기가 일어남. 올바른값을 클라에 갱신해줌.
			codeError = xEC_IS_NOT_RESEARCHING;
			SendUpdateResearch( codeError, snHero, idAbil, abil.point );
			break;
		}
		int point = abil.point;
//		const auto abil = pHero->GetAbilNode( pPropTech->unit, idAbil );
		if( m_spAcc->GetResearching().IsComplete() ) {
			// 연구가 완료됨
			// 현재 연구중인 특성을 완료시킨다.
			point = m_spAcc->DoCompleteCurrResearch();
			codeError = xEC_OK;
		} else {
			codeError = xEC_NOT_YET_FINISH;
			break;
		}
		SendUpdateResearch( codeError, snHero, idAbil, point );
	} while (0);
	return 1;
}

/**
 @brief 현재 특성연구 상태를 업데이트 한다.
*/
int XGameUser::SendUpdateResearch( xErrorCode codeError
																, ID snHero
																, ID idAbil
																, int point )
{
	XBREAK( snHero == 0 );
	XBREAK( idAbil == 0 );
	XPacket ar( (ID)xCL2GS_RESEARCH_COMPLETE );
	ar << (DWORD)codeError;
	ar << snHero;
	ar << idAbil;					// 업데이트해야할 특성아이디
	ar << point;
	ar << m_spAcc->GetCashtem();
	Send( ar );
	return 1;
}


// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqResearchCompleteNow()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqResearchComplete()
*/
int XGameUser::RecvResearchCompleteNow( XPacket& p )
{
	ID idAbil, snHero;
	p >> snHero;			// 업데이트를 요청한 영웅
	p >> idAbil;			// 업데이트를 요청한 현재 연구중인 특성아이디.
	XVERIFY_BREAK( idAbil == 0 );	// 
	XVERIFY_BREAK( snHero == 0 );
	auto& research = m_spAcc->GetResearching();
	auto pHero = m_spAcc->GetHero( research.GetsnHero() );
	XVERIFY_BREAK( pHero == nullptr );
	xErrorCode codeError = xEC_OK;
//	XGAME::xAbil *pAbil = nullptr;
	do {
		XVERIFY_BREAK( idAbil != research.GetidAbil() );
		XVERIFY_BREAK( snHero != research.GetsnHero() );
		auto pPropTech = XPropTech::sGet()->GetpNode( idAbil );
		XVERIFY_BREAK( pPropTech == nullptr );
		const auto abil = pHero->GetAbilNode( pPropTech->unit, idAbil );
		XVERIFY_BREAK( abil.IsLock() );
		// 연구중이 아니면 그냥 나감.
		if( research.GetidAbil() == 0 ) {
			codeError = xEC_IS_NOT_RESEARCHING;
			pHero = m_spAcc->GetHero( snHero );
			SendUpdateResearch( codeError, pHero->GetsnHero(), idAbil, abil.point );
			break;
		}
		int point = abil.point;
		if( research.IsComplete() ) {
			// 이미 연구가 완료됨
			m_spAcc->DoCompleteCurrResearch();
			codeError = xEC_OK;
		} else {
			int costRemain = m_spAcc->GetCashRemainResearch();
			if( m_spAcc->IsEnoughCash(costRemain) ) {
				m_spAcc->AddCashtem( -costRemain );
				point = m_spAcc->DoCompleteCurrResearch();
				codeError = xEC_QUICK_OK;
			} else {
				codeError = xEC_NOT_ENOUGH_CASH;
			}
		}
		SendUpdateResearch( codeError, pHero->GetsnHero(), idAbil, point );
	} while (0);
	//
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqCheckAPTimeOver()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqCheckAPTimeOver()
*/
int XGameUser::RecvCheckAPTimeOver( XPacket& p )
{
	XBREAK( m_spAcc->GetmaxAP() > 0xffff );
	WORD w0;
	p >> w0;	const int apClient = w0;	// client ap
	p >> w0;	const int apMaxClient = w0;	// client max ap

	WORD wSyncOnly = 0;
	bool bRefresh = false;
	// 클라이언트랑 값차이가 너무 많이 나면 한번 갱신시킴.
	if( abs(m_spAcc->GetAP() - apClient) > 5 
		|| abs(m_spAcc->GetmaxAP() - apMaxClient) > 5 ) {
		bRefresh = true;
		wSyncOnly = 0x8000;	// 리프레쉬만 한 경우.
	}
	int apCurr = m_spAcc->GetAP();
	XPacket ar( (ID)xCL2GS_AP_CHECK_TIMEOVER );
	// 실제 계산은 10초마다 한번씩 차지만 트래픽 문제도 있고하니 갱신은 1분마다 하는걸로.
	int secOver = 0;
	XBREAK( m_spAcc->GettimerAP().IsOff() );
	const auto secPass = m_spAcc->GetsecPassAPTime();
	const auto secByAp = XGlobalConst::sGet()->m_secByAP;
	if( secPass >= secByAp ) {
		float apPerSec = ( XGC->m_apPerOnce / (float)secByAp );
		float addAp = secPass * apPerSec;
		apCurr = m_spAcc->AddAP( (int)addAp );
// 		apCurr = m_spAcc->AddAP( (int)((secPass / (float)secByAp) * XGC->m_apPerOnce) );
		secOver = secPass - secByAp;
		m_spAcc->ResetAPTimer( secOver % secByAp );		// 오버된 수치만큼 더 더해서 리셋해야함.
		XBREAK( apCurr > 0xffff );
		XBREAK( apCurr == 0 );		// 적어도 1은 더했으므로 0이 나올수는 없다.
		bRefresh = true;
		wSyncOnly = 0;

	}
	// case. 1.1분이 지난게 확인되어 add되어 갱신된경우. 2.1분이 아직 안지난경우. 3.시간과 관계없이 오차가 많이나 한번 갱신한 경우.
	if( bRefresh ) {
		// 바뀐값을 보내줌.
		XBREAK( apCurr > 0xffff );
		XBREAK( secOver > 0xffff );
		ar << (WORD)m_spAcc->GetmaxAP();
		ar << (WORD)(apCurr | wSyncOnly);	// 리프레쉬만 한경우는 최상위 비트에 1을 넣어 보냄.
	} else {
		ar << (WORD)m_spAcc->GetmaxAP();
		ar << (WORD)0;	// 0이면 타이머가 아직 안지났음.
	}
	ar << (WORD)(secOver % 60);	// 60초에서 오버된만큼을 보정해줘야 함.
	Send( ar );

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqUnlockUnit()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqUnlockUnit()
*/
int XGameUser::RecvUnlockUnit( XPacket& p )
{
	DWORD dw0;
	XGAME::xtUnit unit;
	p >> dw0;	unit = (XGAME::xtUnit)dw0;
	XVERIFY_BREAK( XGAME::IsInvalidUnit( unit ) );
	XVERIFY_BREAK( m_spAcc->IsLockUnit( unit ) == false );
//	XVERIFY_BREAK( GetLevel() < m_spAcc->GetLevelUnlockUnit() );
	XVERIFY_BREAK( m_spAcc->IsUnlockableUnit( unit) == false );
	m_spAcc->SetUnlockUnit( unit );
// 	if( unit == XGAME::xUNIT_PALADIN )
// 		// 기사의 경우 몇레벨에 오픈되던 다른 소형유닛과 같은 포인트를 받도록
// 		m_spAcc->AddTechPoint( unit, XGAME::LEVEL_UNLOCK_PALADIN - 1 );
// 	else 
// 		// 중/대형유닛의 경우 언락한순간부터 1포인트씩 받도록.
// 		m_spAcc->AddTechPoint( unit, 1 );
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_UNLOCK_UNIT );
	DispatchQuestEvent( xQC_EVENT_UNLOCK_UNIT, (DWORD)unit );
	//
	XPacket ar( (ID)p.GetidPacket() );
	ar << (DWORD)unit;
	Send( ar );
	SendBaseInfo();

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqSendReqFillAP()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSendReqFillAP()
*/
int XGameUser::RecvSendReqFillAP( XPacket& p )
{
	int type;
	p >> type;	// 0캐쉬 1:아이템

	int cost = m_spAcc->GetCostRemainFullAP();
	int apMax = 0;
	if( !m_spAcc->IsEnoughCash( cost ) )
		apMax = 0;
	else
	{
		m_spAcc->AddCashtem( -cost );
		apMax = m_spAcc->GetmaxAP();
		m_spAcc->SetAP( apMax );
	}

	//
	XPacket ar( (ID)p.GetidPacket() );
	ar << apMax;
	ar << m_spAcc->GetCashtem();
	XBREAK( type != 0 );
	// 타입이 아이템이면 아이템 정보도 갱신해야 함.
	Send( ar );


	return 1;
}

/**
 클라이언트의 SendReqPaymentAssetByGem()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqPaymentAssetByGem()
*/
int XGameUser::RecvPaymentAssetByGem( XPacket& p )
{
	XGAME::xtError errCode = xE_SUCCESS;
	char c0;
	p >> c0;		const XGAME::xtPaymentRes typeAsset = (xtPaymentRes)c0;
	p >> c0;		const bool bByItem = (c0 != 0);
	p >> c0 >> c0;

	switch( typeAsset ) {
	case xPR_AP:
	case xPR_GOLD:
	case xPR_TIME:
	case xPR_RES:
		break;
	case xPR_TRY_DAILY: {
		// 횟수 모두 채우는 젬가격 얻음
		const int numGem = XGlobalConst::sGet()->m_gemFillDailyTry;
		XBREAK( numGem == 0 );
		if( m_spAcc->IsNotEnoughCash( numGem ) ) {
			errCode = xE_NOT_ENOUGH_CASH;
		} else {
			// 요일스팟 찾음
			auto pSpotDaily = GetpWorld()->GetpSpotDaily();
			XVERIFY_BREAK( pSpotDaily == nullptr );
			// 스팟에 업데이트
			pSpotDaily->ClearnumEnter();
			// 젬 소모
			m_spAcc->AddCashtem( -numGem );
			// 스팟 동기화 전송
			SendSpotSync( pSpotDaily );
		}
	} break;
	default:
		XVERIFY_BREAKF( 1, "unknown typeAsset(%d)", typeAsset );
		break;
	}

	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << (char)errCode;
	ar << (char)typeAsset;
	ar << xboolToByte(bByItem);
	ar << (char)0;
	ar << m_spAcc->GetCashtem();
	Send(ar);
	return 1;
}


/**
 클라이언트의 SendReqHeroUpgradeCompleteQuick()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqHeroUpgradeCompleteQuick()
*/
int XGameUser::RecvTrainCompleteQuick( XPacket& p )
{
	ID snSlot, snHero;
	char c0;
	XGAME::xtTrain typeTrain;
	p >> snSlot;
	p >> snHero;
	p >> c0;		typeTrain = (XGAME::xtTrain)c0;
	p >> c0;		bool bCash = xbyteToBool(c0);
	XVERIFY_BREAK( snSlot == 0 );
	XVERIFY_BREAK( snHero == 0 );
	xErrorCode result;
	auto pSlot = m_spAcc->GetpTrainingSlot( snSlot );
	XVERIFY_BREAK( pSlot == nullptr );		// 클라에서 훈련완료 처리를 제대로 못한것이므로 그냥 재로딩 시키는게 낫다.
	auto pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	XVERIFY_BREAK( pSlot->snHero != snHero );
	XVERIFY_BREAK( XGAME::IsInvalidTrainType( typeTrain ) );
	XVERIFY_BREAK( pSlot->type != typeTrain );
	do {
		result = xEC_OK;
		if( pSlot->IsComplete() )
			// 타이밍에 의해 서버에선 이미 훈련이 완료되었을 수 있음.
			break;
		if( typeTrain == XGAME::xTR_SQUAD_UP ) {
			auto pPropNext = pHero->GetpPropSquadupNext();
			XVERIFY_BREAK( pPropNext == nullptr );
		} else
		if( typeTrain == XGAME::xTR_SKILL_ACTIVE_UP ||
			typeTrain == XGAME::xTR_SKILL_PASSIVE_UP ) {
			auto pPropNext = pHero->GetpPropSkillupNext(typeTrain);
			XVERIFY_BREAK( pPropNext == nullptr );
		}
	} while (0);
	// 캐쉬 깜.
	int costUse = 0;
	XGAME::xtQuickComplete typeComplete;
	if( result == xEC_OK ) {
		if( bCash ) {
			auto cost = m_spAcc->GetCashRemainTrain( pSlot );
			if( m_spAcc->IsEnoughCash( cost ) ) {
				m_spAcc->AddCashtem( -cost );
				costUse = cost;
			} else {
				result = xEC_NOT_ENOUGH_CASH;
			}
			typeComplete = XGAME::xQCT_CASH;
		} else {
			auto cost = m_spAcc->GetGoldRemainTrain( pSlot );
			if( m_spAcc->IsEnoughGold( cost ) ) {
				m_spAcc->AddGold( -cost );
				costUse = cost;
			} else {
				result = xEC_NOT_ENOUGH_GOLD;
			}
			typeComplete = XGAME::xQCT_GOLD;
		}
		if( result == xEC_OK ) {
			DispatchQuestEvent( XGAME::xQC_EVENT_TRAIN_QUICK_COMPLETE );
		}
	}
	// 완료처리와 함께 전송함.
	int bOk = SendTrainingComplete( snSlot, result, typeComplete );
	if( bOk == 0 && costUse > 0 ) {
		if( bCash )
			m_spAcc->AddCashtem( costUse );	// 에러나서 다시 복구
		else
			m_spAcc->AddGold( costUse );	// 에러나서 다시 복구
	}
	XVERIFY_BREAKF( bOk == 0, "훈련실패:type=%d", typeTrain );

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqProvideBooty()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqProvideBooty()
*/
// int XGameUser::RecvProvideBooty( XPacket& p )
// {
// 	BYTE b0;
// 	ID snHero;
// 	p >> snHero;
// 	p >> b0;	auto type = (XGAME::xtTrain)b0;
// //	p >> b0;	int num = b0;
// 	p >> b0 >> b0 >> b0;
// 	XVERIFY_BREAK( type != XGAME::xTR_SQUAD_UP 
// 				&& type != XGAME::xTR_SKILL_ACTIVE_UP 
// 				&& type != XGAME::xTR_SKILL_PASSIVE_UP );
// 	auto pHero = m_spAcc->GetHero( snHero );
// 	XVERIFY_BREAK( pHero == nullptr );
// 	bool bLevelup = false;
// 	XPacket ar( p.GetidPacket() );
// 	ar << snHero;
// 	ar << (BYTE)type;
// 	ar << (BYTE)VER_ETC_SERIALIZE;
// 	ar << (BYTE)0;
// 	ar << (BYTE)0;
// 	xErrorCode result = xEC_OK;
// 	ID idNeed;		// 업글시 필요 템
// 	int numNeed;	// 필요 개수
// 	switch( type )
// 	{
// 	case XGAME::xTR_SQUAD_UP: {
// 		auto pPropSquadNext = pHero->GetpPropSquadupNext();
// 		XVERIFY_BREAK( pPropSquadNext == nullptr );
// 		int num = 1;	// 먹일 메달 개수
// 		XASSERT( num == 1 );	// 1개씩만 먹여야 함.
// 		// 현재 메달제공이 가능한 상태인가.
// 		XVERIFY_BREAK( !m_spAcc->IsAbleProvideSquad( pHero, &idNeed, &numNeed ) );
// 		// 훈련중이냐
// 		int numRefund1 = 0;
// 		int numRefund2 = 0;
// 		bool bAbleProvide = false;
// 		if( m_spAcc->IsTrainingSquadupHero( snHero ) ) {
// 			// 훈련 경과 시간
// 			auto pSlot = m_spAcc->GetTrainingHero( snHero, type );
// 			XVERIFY_BREAK( pSlot == nullptr );
// 			// 채워야 하는 남은 템수
// 			int numRemain = pPropSquadNext->numItem - pHero->GetNumProvided( type );
// 			// 훈련중인 메달이 이미 꽉찬상태면
// 			if( pSlot->GetnumTrainingItem() > (numRemain-1) ) {// 지금 1개를 채울것이므로-1
// 				int numOver = pSlot->GetnumTrainingItem() - (numRemain-1);	
// 				XASSERT( numOver > 0 );
// 				bool bAbleForward = false;
// 				// 이월이 되는가.
// 				auto pPropNextNext = pHero->GetpPropSquadupNextNext();
// 				if( pPropNextNext && pPropNextNext->gradeNeed == pSlot->GetgradeTrainingItem() )
// 					bAbleForward = true;	// 이월 됨
// 				// 이월이 되는가
// 				if( bAbleForward )
// 					// 그대로 더함.
// 					bAbleProvide = true;
// 				else {
// 					// 이월 안됨
// 					// 제공할 여유가 있는가
// 					if( pSlot->GetnumTrainingItem() > 1 ) {
// 						// 제공할수 있음
// 						// 보석1개환불, 시간감소
// 						numRefund1 = pPropSquadNext->GetNeedRes(0);
// 						numRefund2 = pPropSquadNext->GetNeedRes(1);
// 						// 훈련해야 하는거 하나 빼줌
// 						pSlot->SetAddItem( pSlot->GetgradeTrainingItem(), 
// 											pSlot->GetnumTrainingItem() - 1);
// 						// 훈련시간 감소
// 						pSlot->DoDecreaseTrainingSec( pPropSquadNext->needTrain.secTrainPerItem );
// 						// 메달 제공
// 						bAbleProvide = true;
// 					} else {
// 						// 이월이 안되는 마지막 한개는 제공불가가 됨.
// 						result = xEC_NO_MORE_PROVIDE;
// 						idNeed = 0;		// 메달 소모 안됨
// 					}
// 				}
// 				// 훈련으로 얻는것과 합치면 오버됨
// 			} else
// 				// 안넘으니 그냥 더함
// 				bAbleProvide = true;
// 		} else {
// 			// 훈련중이 아닐때
// 			bAbleProvide = true;
// 		}
// 		// 전리품 제공
// 		if( bAbleProvide ) {
// 			if( ( pHero->AddProvide( type, 1, nullptr ) ) ) {
// 				// 렙업
// 				bLevelup = true;
// 				pHero->SetNumProvide( type, 0 );	// 제공했던 아이템 클리어
// 			}
// 			XBREAK( m_spAcc->DestroyItem( idNeed, 1 ) == 0 );
// 			DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_PROVIDE_SQUAD );
// 		}
// 		ar << numRefund1;		// 환불될 보석
// 		ar << numRefund2;		// 환불될 보석
// 	} break;
// 	case XGAME::xTR_SKILL_ACTIVE_UP:
// 	case XGAME::xTR_SKILL_PASSIVE_UP: {
// 		auto pPropSkillNext = pHero->GetpPropSkillupNext(type);
// 		XVERIFY_BREAK( pPropSkillNext == nullptr );
// 		int num = 1;	// 먹일 보옥 개수
// 		XASSERT( num == 1 );	// 1개씩만 먹여야 함.
// 		// 현재 보옥제공이 가능한 상태인가.
// 		XVERIFY_BREAK( !m_spAcc->IsAbleProvideSkill( pHero, type, &idNeed, &numNeed ) );
// 		// 훈련중이냐
// 		int numRefund = 0;
// 		bool bAbleProvide = false;
// 		if( m_spAcc->IsTrainingSkillupHero( snHero, type ) ) {
// 			// 훈련 경과 시간
// 			auto pSlot = m_spAcc->GetTrainingHero( snHero, type );
// 			XVERIFY_BREAK( pSlot == nullptr );
// 			// 채워야 하는 남은 템수
// 			int numRemain = pPropSkillNext->numItem - pHero->GetNumProvided( type );
// 			// 훈련중인 보옥이 이미 꽉찬상태면
// 			if( pSlot->GetnumTrainingItem() > (numRemain-1) ) {	// 지금 1개를 채울것이므로-1
// 				int numOver = pSlot->GetnumTrainingItem() - (numRemain-1);	
// 				XASSERT( numOver > 0 );
// 				bool bAbleForward = false;
// 				// 이월이 되는가.
// 				auto pPropNextNext = pHero->GetpPropSkillupNextNext(type);
// 				if( pPropNextNext && pPropNextNext->gradeNeed == pSlot->GetgradeTrainingItem() )
// 					bAbleForward = true;	// 이월 됨
// 				// 이월이 되는가
// 				if( bAbleForward )
// 					// 그대로 더함.
// 					bAbleProvide = true;
// 				else {
// 					// 이월 안됨
// 					// 제공할 여유가 있는가
// 					if( pSlot->GetnumTrainingItem() > 1 ) {
// 						// 제공할수 있음
// 						// 자원1개환불, 시간감소
// 						numRefund = pPropSkillNext->GetNeedRes();
// 						// 훈련해야 하는거 하나 빼줌
// 						pSlot->SetAddItem( pSlot->GetgradeTrainingItem(), 
// 											pSlot->GetnumTrainingItem() - 1);
// 						// 훈련시간 감소
// 						pSlot->DoDecreaseTrainingSec( pPropSkillNext->needTrain.secTrainPerItem );
// 						// 보옥 제공
// 						bAbleProvide = true;
// 					} else {
// 						// 이월이 안되는 마지막 한개는 제공불가가 됨.
// 						result = xEC_NO_MORE_PROVIDE;
// 						idNeed = 0;		// 보옥 소모 안됨
// 					}
// 				}
// 				// 훈련으로 얻는것과 합치면 오버됨
// 			} else
// 				// 안넘으니 그냥 더함
// 				bAbleProvide = true;
// 		} else {
// 			// 훈련중이 아닐때
// 			bAbleProvide = true;
// 		}
// 		// 전리품 제공
// 		if( bAbleProvide ) {
// 			if( ( pHero->AddProvide( type, 1, nullptr ) ) ) {
// 				// 렙업
// 				bLevelup = true;
// 				pHero->SetNumProvide( type, 0 );	// 제공했던 아이템 클리어
// 			}
// 			XBREAK( m_spAcc->DestroyItem( idNeed, 1 ) == 0 );
// 			DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_PROVIDE_SKILL );
// 		}
// 		ar << numRefund;		// 환불될 자원
// 	} break;
// 	} // switch
// 	//
// 	m_spAcc->SerializeTrainSlot( ar );
// 	pHero->SerializeLevelupReady( ar );
// 	ar << idNeed;		// 소모된 메달. 0이면 소모안되었음.
// 	ar << xboolToByte(bLevelup);	// LevelupReady가 가므로 굳이 보내줄 필요 없는듯.
// 	XBREAK( pHero->GetNumProvided(type) > 0xff );
// 	ar << (BYTE)pHero->GetNumProvided(type);
// 	XBREAK( (DWORD)result >= 0xffff );
// 	ar << (WORD)result;
// 	Send( ar );
// 
// 	return 1;
// }
/**
 클라이언트의 SendReqPromotionHero()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqPromotionHero()
*/
int XGameUser::RecvPromotionHero( XPacket& p )
{
	ID snHero;
	p >> snHero;
	XVERIFY_BREAK( snHero == 0 );
	auto pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	// 승급을 함.
	auto bOk = m_spAcc->DoPromotionHero( snHero );
	XVERIFY_BREAK( bOk != XGAME::xE_OK );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << (int)bOk;
	ar << snHero;
	if( bOk == XGAME::xE_OK ) {
//		DispatchQuestEvent( XGAME::xQC_EVENT_GET_HERO, pHero->GetidProp() );
		DispatchQuestEvent( XGAME::xQC_EVENT_PROMOTION_HERO, (DWORD)pHero->GetGrade() );
		m_spAcc->SerializeHeroUpdate( ar, pHero );
	}
	Send(ar);
	return 1;
}
// 다음은 서버 코드입니다.
/**
 @brief 조각을 다 보아서 영웅을 소환시킨다.
 클라이언트의 SendReqSummonHeroByPiece()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSummonHeroByPiece()
*/
int XGameUser::RecvSummonHeroByPiece( XPacket& p )
{
	ID idPropHero;
	p >> idPropHero;
	auto pPropHero = PROP_HERO->GetpProp( idPropHero );
	XVERIFY_BREAK( pPropHero == nullptr );
	auto pItemPiece = m_spAcc->GetSoulStoneByHero( pPropHero->strIdentifier );
	XVERIFY_BREAK( pItemPiece == nullptr );
	XHero *pHero = nullptr;
	auto bOk = m_spAcc->DoSummonHeroByPiece( idPropHero, &pHero );
	XVERIFY_BREAK( bOk != XGAME::xE_OK );
	{
		DispatchQuestEvent( XGAME::xQC_EVENT_GET_HERO, pHero->GetidProp() );
		DispatchQuestEvent( XGAME::xQC_EVENT_SUMMON_HERO, pHero->GetidProp() );
	}

	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << (int)bOk;
	ar << idPropHero;
	m_spAcc->SerializeHeroUpdate( ar, pHero );
	Send(ar);
	return 1;
}
// 다음은 서버 코드입니다.
/**
 @brief 컷씬 시퀀스가 끝남.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqEndSeq()
*/
int XGameUser::RecvEndSeq( XPacket& p )
{
	_tstring str;
	p >> str;
	std::string idsSeq = SZ2C(str.c_str());
	XVERIFY_BREAK( m_spAcc->IsCompletedSeq( idsSeq ) );
	m_spAcc->AddCompleteSeq( idsSeq );
	// 튜토기사 편입컷씬이 끝나면 튜토리얼 기사를 제공한다.
	_tstring idsHero = _T("donkeyxote");
// 	XHero *pHero = nullptr;
	if( idsSeq == "visit.tavern" ) {
		if( !m_spAcc->IsHaveHero( idsHero ) ) {
			auto pHero = m_spAcc->CreateAddHero( idsHero );
			SendCreateHero( pHero );
		}
	}
	_tstring tidsSeq = C2SZ(idsSeq);
	DispatchQuestEvent( xQC_EVENT_END_SEQ, tidsSeq );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << str;
	ar << (int)m_spAcc->GetNumCompletedSeq();
// 	if( pHero )
// 		ar << pHero->GetidProp();
// 	else
// 		ar << 0;
	Send(ar);
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqSendReqBuyGuildItem()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSendReqBuyGuildItem()
*/
int XGameUser::RecvBuyGuildItem( XPacket& p )
{
	int idxSlot;
	p >> idxSlot;
	XVERIFY_BREAK( (size_t)idxSlot >= XGC->m_aryGuildShop.size());
	auto& slot = XGC->m_aryGuildShop[ idxSlot ];
	XVERIFY_BREAK( m_spAcc->GetptGuild() < slot.cost );
	m_spAcc->AddGuildPoint( -slot.cost );
	m_spAcc->CreateItemToInven( slot.idsItem, 1 );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << m_spAcc->GetptGuild();
	ar << slot.idsItem;
	Send(ar);
	return 1;
}



// xuzhu end
//Ranking
int XGameUser::RecvRankingInfo(XPacket& p)
{
	XPacket ar((ID)xCL2GS_LOBBY_RANKING_INFO);
	XAUTO_LOCK2( RANKING_DB );
	RANKING_DB->Serialize(ar);
	Send(ar);
	return 1;
}

//Guild
/**
 @brief 길드 정보를 동기화시킴
 this유저가 길드가 있을경우 소속된 길드정보를 보내주고
 없을경우 전체 길드 리스트를 보냄.
*/
int XGameUser::RecvGuildInfo( XPacket& p )
{
	const ID idGuild = m_spAcc->GetGuildIndex();
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_INFO );
	if( idGuild == 0 ) { //길드 없음
		ar << 1;		// 리스트 동기화 식별자
		// 가입신청한 길드와 그 길드에 실제로 내 명단이 있는지 비교해서 동기화
		XList4<ID> listCopy = m_spAcc->GetlistGuildJoinReq();
		for( auto  idGuildReq : listCopy ) {
			auto pGuild = XGuildMgr::sGet()->FindGuild( idGuildReq );
			if( pGuild ) {
				if( !pGuild->IsJoinReqerUser( GetidAcc() ) )
					m_spAcc->DelGuildJoinReq( pGuild->GetidGuild() );
			}
		}
		// 길드리스트 전체의 요약본을 보낸다.
		XGuildMgr::sGet()->SerializeGuildInfo( ar );

	} else {
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( pGuild ) {
			ar << 0;		// this유저의 길드 동기화 식별자
			XGuild::sSerialize( pGuild, ar );
		} else {
			// 계정에 길드인덱스는 있는데 실제 길드가 없는경우는 길드 리스트를 보내야함.
			m_spAcc->ClearGuild();
			ar << 1;		// 리스트 동기화
			// 길드리스트 전체의 요약본을 보낸다.
			XGuildMgr::sGet()->SerializeGuildInfo( ar );
		}
	}
	// 유저의 길드정보도 다시 동기화시키는게 좋음.
	XAccount::sSerializeGuildInfo( m_spAcc, &ar );
	Send( ar );
	return 1;
}
/**
 @brief 플레이어 길드정보만 보냄.
*/
// int XGameUser::RecvGuildInfo(XPacket& p)
// {
// 	XPacket ar((ID)xCL2GS_LOBBY_GUILD_INFO);
// 	auto pGuild = XGuildMgr::sGet()->FindGuild( m_spAcc->GetGuildIndex() );
// 	XGuild::sSerialize( pGuild );
// //		XGuildMgr::sGet()->SerializeGuildAll(m_spAcc->GetGuildIndex(), ar);
// 	Send(ar);
// 	return 1;
// }

/**
 @brief 길드리스트를 보냄
*/
// int XGameUser::RecvAllGuildInfo( XPacket& p )
// {
// 	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_INFO );
// 	XGuildMgr::sGet()->SerializeGuildInfo( ar );
// 	Send( ar );
// 	return 1;
// }
int XGameUser::RecvGuildChangeOption(XPacket& p)
{
	DWORD bAutoAccept;
	DWORD bBlockAccept;
	p >> bAutoAccept;
	p >> bBlockAccept;

	const ID idGuild = m_spAcc->GetGuildIndex();
	auto errCode = xGE_NO_SEND;
	do {
		if( idGuild == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( !pGuild ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		if( m_spAcc->GetGuildgrade() < xGGL_LEVEL4 ) {
			errCode = xGE_ERROR_NO_AUTHORITY;
			break;
		}
		ID idKey = DBA_SVR->SendGuildUpdateOption( m_spAcc, bAutoAccept, bBlockAccept, xCL2GS_LOBBY_GUILD_CHANEGE_OPTION );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildChangeOption );
	} while( 0 );
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANEGE_OPTION );
		ar << (DWORD)errCode;
		Send( ar );
	}
	return 1;
}

void XGameUser::cbGuildChangeOption( XPacket& p )
{
	DWORD result;
	p >> result;

	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANEGE_OPTION );
	ar << (DWORD)result;		
	Send( ar );
}

/**
 @brief 길드원의 정보를 변경
 this는 길마가 된다.
*/
int XGameUser::RecvGuildChangeMemberInfo(XPacket& p)
{
	auto errCode = xGE_NO_SEND;
	do {
		ID idAccTarget;
		BYTE b0;
		p >> idAccTarget;			// 변경하려는 멤버의 아이디
		p >> b0;		auto grade = (xtGuildGrade)b0;
		p >> b0 >> b0 >> b0;
		if( idAccTarget == 0 
				|| (grade <= xGGL_NONE || grade >= xGGL_MAX) ) {
			errCode = xGE_ERROR_INVALID_PARAMETER;
			break;
		}
		const ID idGuild = m_spAcc->GetGuildIndex();
		if( XGuildMgr::sGet()->GetGuildSize() == 0 || idGuild == 0 ) {//길드 없음
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		ID idKey = DBA_SVR->SendGuildUpdateMember( m_spAcc, idAccTarget, grade );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildChangeMember );
	} while(0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANGE_MEMBER );
		ar << (BYTE)errCode;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		Send( ar );
	}
	return 1;
}

void XGameUser::cbGuildChangeMember( XPacket& p )
{
	BYTE b0;
	ID idAcc;

	p >> idAcc;
	p >> b0;		auto result = (xtGuildError)b0;
	p >> b0 >> b0 >> b0;

	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANGE_MEMBER );
	ar << idAcc;
	ar << (BYTE)result;		//XGAME::xtGuildErrorCode 참고할것.
	ar << (BYTE)0;
	ar << (WORD)0;
	Send( ar );
}

int XGameUser::RecvGuildChangeContext(XPacket& p)
{
	_tstring strGuildContext;
	p >> strGuildContext;

	const ID idGuild = m_spAcc->GetGuildIndex();
	auto errCode = xGE_NO_SEND;
	do {
		if( strGuildContext.empty() ) {
			break;
		}
		if( idGuild == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( !pGuild ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		if( m_spAcc->GetGuildgrade() < xGGL_LEVEL4 ) {
			errCode = xGE_ERROR_NO_AUTHORITY;
			break;
		}
		ID idKey = DBA_SVR->SendGuildUpdateContext( m_spAcc, strGuildContext, xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildChangeContext );
	} while (0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT );
		ar << (DWORD)errCode;
		Send( ar );
	}
	return 1;
}
int XGameUser::RecvGuildUpdate(XPacket& p)
{
	return 1;
}
int XGameUser::RecvGuildCreate(XPacket& p)
{
	_tstring strGuildName;
	_tstring strGuildContext;
	int costtype;
	DWORD costvalue;
	p >> strGuildName;
	p >> strGuildContext;
	p >> costtype;
	p >> costvalue;

	auto errCode = xGE_NO_SEND;
	do {
		// 이미 길드가 있음.
		if( m_spAcc->GetGuildIndex() > 0 ) {
			errCode = xGE_ERROR_ALREADY_HAVE_GUILD;
			break;
		}
		if( strGuildName.length() > 16 ) {
			errCode = xGE_ERROR_NAME_IS_TOO_LONG;
			break;
		}
		if( strGuildContext.length() > 256 ) {
			errCode = xGE_ERROR_CONTEXT_IS_TOO_LONG;
			break;
		}
		// 이미 있는 길드인지 게임서버에서 일차검색.
		auto pGuild = XGuildMgr::sGet()->FindGuild( strGuildName );
		if( pGuild ) {
			errCode = xGE_ERROR_NAME_DUPLICATE;
			break;
		}
		// 	DWORD HaveGold = m_spAcc->GetGold();		// 잉? 길드만드는데 돈이 들었었나?
// 	if( costvalue > HaveGold ) {
// 		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CREATE );
// 		ar << (ID)XGAME::xtGuildError::xGE_ERROR_COST_NOT_ENOUGH;
// 		Send( ar );
// 		return 1;
// 	}
//
		ID idKey = DBA_SVR->SendGuildCreate( m_spAcc, strGuildName, strGuildContext, xCL2GS_LOBBY_GUILD_CREATE );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildCreate );
	} while (0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CREATE );
		ar << (DWORD)errCode;
		Send( ar );
	}
	return 1;
}
void XGameUser::cbGuildCreate(XPacket& p)
{	
	DWORD dw0;
//	ID idPacket;
	ID idGuild;
	_tstring strGuildName;
	_tstring strUserName;
	_tstring strGuildContext;
	p >> dw0;	auto result = (XGAME::xtGuildError)dw0;
	p >> idGuild;
	p >> strGuildName;
	p >> strGuildContext;

// 	if( result == XGAME::xGE_ERROR_NAME_DUPLICATE ) {
// 		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CREATE );
// 		ar << (ID)XGAME::xtGuildError::xGE_ERROR_NAME_DUPLICATE;
// 		Send( ar );
// 		return;
// 	} else
	if( result == XGAME::xGE_SUCCESS ) {
		m_spAcc->SetGuildgrade( XGAME::xtGuildGrade::xGGL_LEVEL5 );
		m_spAcc->SetGuildIndex( idGuild );
		m_spAcc->ClearlistGuildJoinReq();
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_JOIN_GUILD );
	}
	//
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CREATE );		
	ar << (DWORD)result;	
	ar << idGuild;
	ar << strGuildName;
	ar << strGuildContext;
	ar << (DWORD)(xGGL_LEVEL5);
	Send(ar);
}

void XGameUser::cbGuildChangeContext(XPacket& p)
{
	BYTE b0;
//	ID idAcc;
	_tstring strContext;

	p >> b0;		auto result = (xtGuildError)b0;
	p >> b0 >> b0 >> b0;
	p >> strContext;

	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT);
	ar << (BYTE)result;		
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << strContext;
	Send(ar);
}

/**
 @brief 길드 가입요청
*/
int XGameUser::RecvGuildJoinReq(XPacket& p)
{	
	auto errCode = xGE_NO_SEND;
	ID idGuild;
	p >> idGuild;
	do {
		// 요청한 길드의 정보를 꺼냄
		auto pTargetGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		// 요청한 길드가 없음.
		if( pTargetGuild == nullptr || pTargetGuild->GetnumMembers() == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		const ID idGuildEntered = m_spAcc->GetGuildIndex();		// 이미 가입된 길드
		// 이미 idGuild길드에 가입되어 있음.
		if( idGuildEntered == idGuild ) {
			errCode = xGE_ERROR_ALREADY_MEMBER;
			break;
		}
		// 이미 다른 길드에 가입되어 있음.
		{
			auto pGuild = XGuildMgr::sGet()->FindGuild( idGuildEntered );
			if( pGuild ) {
				errCode = xGE_ERROR_ALREADY_HAVE_GUILD;
				break;
			} else {
				// 다른길드에 가입된걸로 기록되어 있으나 그 길드가 없어져서 무효화함.
			}
		}
		// 요청한 길드에 빈자리가 없음.
		if( pTargetGuild->IsFullMember() ) {
			errCode = xGE_ERROR_MAX_MEMBER_OVER;
			break;
		}
		// 요청한 길드가 가입거부상태임.
		if( pTargetGuild->GetGuildOption().s_bBlockReqJoin ) {
			errCode = xGE_ERROR_BLOCK_JOIN_REQ;
			break;
		}
		// 나머지는 DBA서버에서 제대로 확인함.
		ID idKey = DBA_SVR->SendGuildJoinReq( m_spAcc, idGuild, xCL2GS_LOBBY_GUILD_JOIN_REQ );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildJoinReq );
	} while(0);
	//
	if( errCode != xGE_NO_SEND ) {
		// DBA로 보낼것도 없이 여기서 에러걸림.
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_JOIN_REQ );
		ar << (DWORD)errCode;
		Send( ar );
	}
	
	return 1;
}

/**
 @brief 길드 가입 요청 결과
 DB서버 XGameSvrConnection::SendGuildJoinReq에 대응
*/
void XGameUser::cbGuildJoinReq( XPacket& p )
{
	DWORD dw0;
//	ID idPacket;
	ID idGuild;
	ID idAcc;

	p >> idAcc;
	p >> idGuild;
//	p >> dw0;	auto event = (xtGuildEvent)dw0;
	p >> dw0;	auto result = (xtGuildError)dw0;
//	p >> idPacket;		// 사용안함.

	if( result == xGE_SUCCESS ) {
		m_spAcc->AddGuildJoinReq( idGuild );
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_JOIN_GUILD );
	} else
	if( result == xGE_ERROR_ALREADY_MEMBER ) {
		//이미 가입된 멤버면 계정정보 비동기화 이므로 갱신해서 동기화 시킴.
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( pGuild ) {
			auto pMember = pGuild->FindpMemberByidAcc( idAcc );
			if( pMember ) {
				m_spAcc->SetGuildIndex( idGuild );
				m_spAcc->SetGuildgrade( pMember->m_Grade );
				m_spAcc->ClearlistGuildJoinReq();
				// 여기서 갱신된걸 보내야 하지만 일반적인 상황이 아니라 그냥 함.
			}
		}
	} else
	if( result == xGE_ERROR_ALREADY_JOIN_REQ ) {
		// 길드db 엔 이 유저가 가입신청한걸로 되어있는데 이 유저의 가입신청목록엔 없는경우. 동기화시켜야함.
		m_spAcc->ClearGuild();
		m_spAcc->AddGuildJoinReq( idGuild );
		result = xGE_SUCCESS;
	}

	// 가입요청 성공
//	XBREAK( idPacket != xCL2GS_LOBBY_GUILD_JOIN_REQ );
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_JOIN_REQ );
	ar << (DWORD)result;		//XGAME::xtGuildErrorCode 참고할것.
	ar << idGuild;
	Send( ar );
}

/**
 @brief 길드 가입 수락
*/
int XGameUser::RecvGuildJoinAccept(XPacket& p)
{
	int i0;
	ID idAccReqer;
	p >> idAccReqer;		// 가입신청자
	p >> i0;		auto flagAccept = (xtGuildAcceptFlag)i0;		// 수락/거절

	auto errCode = xGE_NO_SEND;
	do {
		if( m_spAcc->GetGuildIndex() == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		auto pGuild = XGuildMgr::sGet()->FindGuild( m_spAcc->GetGuildIndex() );
		if( !pGuild ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			m_spAcc->ClearGuild();
			break;
		}
		//4까지는 허용 불가? 이거 조절 하는거 만들어야 함.
		if( m_spAcc->GetGuildgrade() < XGAME::xtGuildGrade::xGGL_LEVEL4 ) {
			errCode = xGE_ERROR_NO_AUTHORITY;
			break;
		}
		// 가입신청한 유저가 아님
		if( !pGuild->IsJoinReqerUser( idAccReqer ) ) {
			errCode = XGE_ERROR_MEMBER_NOT_FOUND;
			break;
		}
		ID idKey = DBA_SVR->SendGuildJoinAccept( GetidAcc()
																						 , idAccReqer
																						 , flagAccept
																						 , pGuild->GetGuildIndex()
																						 , xCL2GS_LOBBY_GUILD_JOIN_ACCEPT );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildJoinAccept );
	} while(0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_JOIN_ACCEPT );
		ar << (DWORD)errCode;
		Send( ar );
	}

	return 1;
}

void XGameUser::cbGuildJoinAccept( XPacket& p )
{
	DWORD dw0;
	ID idPacket = 0;
	ID idGuild = 0;
	ID idAccReqer = 0;
	
	p >> dw0;		auto errCode = (xtGuildError)dw0;
	p >> idAccReqer;
	p >> idGuild;
	p >> dw0;		auto flagAccept = (xtGuildAcceptFlag)dw0;
	p >> idPacket;

	if( errCode == xGE_SUCCESS ) {
		// this유저가 idAccReqer유저의 가입을 승인함.
	}
	XBREAK( idPacket != xCL2GS_LOBBY_GUILD_JOIN_ACCEPT );
	XPacket ar( idPacket );
	ar << (DWORD)errCode;		
	ar << idGuild;
	Send( ar );
}

/**
 @brief 길드 탈퇴
*/
int XGameUser::RecvGuildOut(XPacket& p)
{
	auto errCode = xGE_NO_SEND;
	do {
		const ID idGuild = m_spAcc->GetGuildIndex();
		// 길드에 가입되어있지 않음.
		if( idGuild == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		// 길드에 가입되어있으나 실제 그런길드가 없음.
		auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( pGuild == nullptr ) {
			m_spAcc->ClearGuild();
			m_spAcc->ClearlistGuildJoinReq();
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		ID idKey = DBA_SVR->SendGuildOut( GetidAcc(), idGuild, xCL2GS_LOBBY_GUILD_OUT );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildOut );
	} while (0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_OUT );
		ar << (DWORD)errCode;
		Send( ar );
	}
	return 1;
}

/**
@brief 길드 탈퇴
*/
void XGameUser::cbGuildOut( XPacket& p )
{
	DWORD dw0;
	p >> dw0;		auto result = (xtGuildError)dw0;
	if( result == xGE_SUCCESS ) {
		m_spAcc->ClearGuild();
		m_spAcc->ClearlistGuildJoinReq();
	}
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_OUT );
	ar << (DWORD)result;
	Send( ar );
}

/**
 @brief idAccTarget유저를 길드에서 추방시킴
*/
int XGameUser::RecvGuildKick(XPacket& p)
{
	ID idAccTarget;
	p >> idAccTarget;
	auto errCode = xGE_NO_SEND;
	do {
		if( idAccTarget == 0 ) {
			errCode = XGE_ERROR_MEMBER_NOT_FOUND;
			break;
		}
		if( m_spAcc->GetidGuild() == 0 ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		// 추방 권한이 없음.
		const auto grade = m_spAcc->GetGuildgrade();
		if( grade < xGGL_LEVEL4 ) {
			errCode = xGE_ERROR_NO_AUTHORITY;
			break;
		}
		// 자기자신을 추방할순 없음.
		if( idAccTarget == GetidAcc() ) {
			errCode = xGE_ERROR_NO_AUTHORITY;
			break;
		}
		auto pGuild = XGuildMgr::sGet()->FindGuild( m_spAcc->GetidGuild() );
		if( !pGuild ) {
			errCode = XGE_ERROR_GUILD_NOT_FOUND;
			break;
		}
		auto pMember = pGuild->FindpMemberByidAcc( idAccTarget );
		if( !pMember ) {
			errCode = XGE_ERROR_MEMBER_NOT_FOUND;
			break;
		}
		ID idKey = DBA_SVR->SendGuildKick( GetidAcc()
																			, idAccTarget
																			, m_spAcc->GetidGuild()
																			, xCL2GS_LOBBY_GUILD_KICK );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbGuildKick );
	} while (0);
	//
	if( errCode != xGE_NO_SEND ) {
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_KICK );
		ar << (DWORD)errCode;
		ar << idAccTarget;
		Send( ar );
	}
	return 1;
}

/**
@brief idAccKick유저를 길드에서 추방시킨 결과
*/
void XGameUser::cbGuildKick( XPacket& p )
{
	DWORD dw0;
	ID idAccKick;
	ID idGuild;
	p >> idAccKick;		// 추방시킨 유저의 아이디
	p >> idGuild;
	p >> dw0;		auto errCode = (xtGuildError)dw0;

	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_KICK );
	ar << (DWORD)errCode;
	ar << idAccKick;
	Send( ar );
}


void XGameUser::SendGuildEvent( xtGuildEvent event, XArchive& arParam )
{	
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_EVENT );	
	ar << (BYTE)event;
	ar << (BYTE)0;
	ar << (WORD)0;
	ar << arParam;
	Send(ar);
}

//////////////////////////////////////////////////////////////////////////

/**
 @brief 무역상을 호출했다.
*/
int	XGameUser::RecvTradeCashCall(XPacket& p)
{
//	int type = 0;
	int i0;
	p >> i0;		auto type = (XGAME::xtSpentCall)i0;
	XVERIFY_BREAK( !(type == XGAME::xSC_SPENT_ITEM || type == XGAME::xSC_SPENT_GEM) );
	//
	ID idRecallItem = XGC->m_traderRecallItem;
	int numRemainSpent = m_spAcc->GetNumItems( idRecallItem );
	const int numRemainSpentOrig = numRemainSpent;
	if( m_spAcc->GettimerTrader().IsOff() || m_spAcc->IsOverTraderRecall() ) {
		// 상인이 떠나지 않은상태이거나 귀환시간이 이미 지났으면 갱신만 시켜준다.
	} else {
		if( type == XGAME::xSC_SPENT_ITEM ) {
			XVERIFY_BREAK( numRemainSpent <= 0 );
			m_spAcc->DestroyItem( XGC->m_traderRecallItem, 1 );	// 호루라기 한개 파괴
			--numRemainSpent;		// 남은 아이템 개수갱신.
			const _tstring strLog = XE::Format( _T( "TradeMerchant_Call_Item:Before[%d] => After[%d] )" ), numRemainSpent, numRemainSpentOrig );
			AddLog( XGAME::xULog_Trade_Call_Gem, strLog );
		} else
		if( type == XGAME::xSC_SPENT_GEM ) {
			const int cashCost = m_spAcc->GetCashForTraderRecall();
			XVERIFY_BREAK( m_spAcc->IsNotEnoughCash( cashCost) );
			m_spAcc->AddCashtem( -cashCost );
// 			ID idKey =			젬사용된거 저장하는거 같은데 백섭되버리면 젬만 날아가는거 아닌가?
// 			DBA_SVR->SendTradeGemCall( GetidAccount()
// 																, m_spAcc->GetCashtem()
// 																, GetidConnect()
// 																, xCL2GS_LOBBY_CASH_TRADE_CALL );	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
// 			DBA_SVR->AddResponse( idKey, this, &XGameUser::RecvTradeCashCallResult );
		}
		m_spAcc->OffTimerByTrader();											// 귀환 타이머 끔.
	}
	XPacket ar( (ID)xCL2GS_LOBBY_CASH_TRADE_CALL );
	ar << (int)type;
	ar << m_spAcc->GettimerTrader();
//	ar << (int)1;
	if( type == XGAME::xSC_SPENT_ITEM ) {
		ar << idRecallItem;
		ar << numRemainSpent;			//남은 아이템 갯수를 보내준다.
	} else {
		ar << m_spAcc->GetCashtem();
	}
	Send( ar );

	return 1;
}

int XGameUser::RecvAccountNickNameDuplicate(XPacket& p)
{
	return 1;
}

int XGameUser::RecvAccountNameDuplicate(XPacket& p)
{
	return 1;
}
int XGameUser::RecvAccountNameRegist(XPacket& p)
{
	_tstring strAccountName;
	_tstring strPassWord;
	p >> strAccountName;
	p >> strPassWord;

	int nNameSize = (int)strAccountName.length();
	int nPassSize = (int)strPassWord.length();
	if (nNameSize == 0 || nNameSize > XGAME::MAX_ID_LEN || nPassSize == 0 || nPassSize > XGAME::MAX_ID_LEN )
	{
		XPacket ar((ID)xCL2GS_ACCOUNT_NAME_REGIST);
		ar << 0;				//Exist nickname	 0:실패 1:성공.
		ar << strAccountName;
		ar << strPassWord;
		Send(ar);
	} else {
		BTRACE( "G:%s:id=%s", __TFUNC__, strAccountName.c_str() );
		ID idKey = DBA_SVR->SendAccountNameRegist(GetidAcc(),
			strAccountName,
			strPassWord,
			xCL2GS_ACCOUNT_NAME_REGIST);	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
		DBA_SVR->AddResponse(idKey, this, &XGameUser::RecvAccountNameRegistResult);
	}	
	return 1;
}
// int XGameUser::RecvAccountNameLogin(XPacket& p)
// {
// 	_tstring strAccountName;
// 	_tstring strPassWord;
// 	p >> strAccountName;
// 	p >> strPassWord;
// 
// 	int nNameSize = (int)strAccountName.length();
// 	int nPassSize = (int)strPassWord.length();
// 	if (nNameSize == 0 || nNameSize > XGAME::MAX_ID_LEN || nPassSize == 0 || nPassSize > XGAME::MAX_ID_LEN)
// 	{
// 		XPacket ar((ID)xCL2GS_ACCOUNT_LOGIN_FROM_ID);
// 		ar << 0;				//Exist nickname	 0:실패 1:성공.
// 		ar << (ID)1;			// 1: ID 나 PW 에 길이가 0인것이 있음.
// 		ar << strAccountName;
// 		ar << strPassWord;
// 		Send(ar);
// 	}
// 	else
// 	{
// 		if (DBA_SVR)
// 		{
// 			ID idKey = DBA_SVR->SendAccountNameRegistCheck(GetidAccount(),
// 															strAccountName,
// 															strPassWord,
// 															xCL2GS_ACCOUNT_LOGIN_FROM_ID);	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
// 			DBA_SVR->AddResponse(idKey, this, &XGameUser::RecvAccountNameRegistCheckResult);
// 		}
// 	}
// 	return 1;	
// }

int XGameUser::RecvAccountNickNameChange(XPacket& p)
{
	_tstring strAccountNickName;
	p >> strAccountNickName;

	int nSize = int(strAccountNickName.length());
	if (nSize == 0 || nSize > XGAME::MAX_ID_LEN)
	{
		XPacket ar((ID)xCL2GS_ACCOUNT_NAME_DUPLICATE);
		ar << 0;
		Send(ar);
	}

	if (DBA_SVR)
	{
		ID idKey = DBA_SVR->SendAccountNameDuplicateCheck(	GetidAcc(),
															strAccountNickName,
															xCL2GS_ACCOUNT_NAME_DUPLICATE);	// DB서버로부터 결과를 받은다음 클라에 보내야 할 패킷
		DBA_SVR->AddResponse(idKey, this, &XGameUser::RecvAccountNameDuplicateResult);
	}
	return 1;
}


void XGameUser::RecvAccountNickNameDuplicateResult(XPacket& p)
{	
//	ID idaccount;
	ID result;
	ID idpacket;
	
	//p >> idaccount;
	p >> result;
	p >> idpacket;

	XPacket ar(idpacket);
	ar << result;
	Send(ar);
}
void XGameUser::RecvAccountNameDuplicateResult(XPacket& p)
{
//	ID idaccount;
	ID result;
	ID idpacket;
	
	//p >> idaccount;
	p >> result;
	p >> idpacket;

	XPacket ar(idpacket);
	ar << result;
	Send(ar);
}
void XGameUser::RecvAccountNameRegistCheckResult(XPacket& p)
{
	//	ID idaccount;
	ID result;
	ID idPacket;
	_tstring strAccountName;
	_tstring strPassword;

	//p >> idaccount;
	p >> result;
	p >> strAccountName;
	p >> strPassword;
	p >> idPacket;

	XPacket ar(idPacket);
	ar << result;		//Exist nickname	 0:실패 1:성공.
	ar << strAccountName;
	ar << strPassword;
	Send(ar);
}
void XGameUser::RecvAccountNameRegistResult(XPacket& p)
{
	ID idaccount;
	ID result;
	ID idPacket;
	_tstring strAccountName;
	_tstring strPassword;
	
	p >> idaccount;
	p >> result;
	p >> strAccountName;
	p >> strPassword;
	p >> idPacket;

	XPacket ar(idPacket);
	ar << result;		//Exist nickname	 0:실패 1:성공.
	ar << strAccountName;
	ar << strPassword;
	Send(ar);
	BTRACE( "G0:%s:id=%s, bOk=%d", __TFUNC__, strAccountName.c_str(), result );
}
void XGameUser::RecvAccountNickNameChangeResult(XPacket& p)
{
	ID idaccount;
	ID result;
	ID idPacket;
	_tstring strAccountNickName;
	
	p >> idaccount;
	p >> result;
	p >> strAccountNickName;
	p >> idPacket;

	XPacket ar(idPacket);
	ar << result;		//Exist nickname	
	ar << strAccountNickName;
	Send(ar);
}


/**
 @brief 무역상 귀환함.
*/
void XGameUser::SendTradeMerchant()
{
	XPacket ar((ID)xCL2GS_LOBBY_TIME_TRADE_CALL);
//	ar << (int)bFlag;
	Send(ar);
	_tstring strLog = XE::Format(_T("TradeMerchant_Call_TimeOver"));
	AddLog(XGAME::xULog_Trade_Call_Time, strLog);
}
/**
 @brief 무기상 물품이 갱신됨.
*/
void XGameUser::SendShoplistMerchant()
{
	m_spAcc->ChangeShopItemList();
	//
	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SHOP_LIST_TIME);
//	ar << (bFlag); //성공
	m_spAcc->SerializeShopList(ar);
	Send(ar);

	_tstring strLog = XE::Format(_T("ShopMerchant_ChangeList"));
	AddLog(XGAME::xULog_ShopList_Call_Time, strLog);
}
void XGameUser::DoAddPostNotify(XPacket& p)
{
	//_tstring strMessage;
	//int UpdateVersion;
	//p >> UpdateVersion;
	//p >> strMessage;
	//if (strMessage.length() == 0) return;
	//if (m_spAcc->GetUpdateVersion() >= UpdateVersion) return;
	//if (DBA_SVR)
	//{		
	//	XArchive ar;

	//	ar.WriteString(_T("GM")); //Sender
	//	ar.WriteString(m_spAcc->GetstrName()); //Recv			
	//	ar.WriteString(_T("Update"));		 //Title
	//	ar << strMessage;

	//	int count = 1; //첨부 아이템 종류
	//	ar << count;

	//	ar << (ID)XGAME::xtPOSTResource::xPOSTRES_GEM; //이부분도 global 에서 손볼수 있게 고치자.
	//	ar << (ID)20;
	//	
	//	m_spAcc->SetUpdateVersion(UpdateVersion);

	//	ID idKey = DBA_SVR->SendPOSTInfoAdd(GetidAccount(), m_spAcc->GenerateSN(), (ID)(XGAME::xtPostType::xPOSTTYPE_NORMAL), ar, xCL2GS_LOBBY_POST_ADD);
	//	DBA_SVR->AddResponse(idKey, this, &XGameUser::cbDoAddPostInfo);
	//}
}
// void XGameUser::SendSubscribePackage()
// {	
// 	AddPostSubscribe();
// }

//void XGameUser::SendGuildUpdate(XArchive& p)
//{	
//	XPacket ar((ID)xCL2GS_LOBBY_GUILD_UPDATE);	
//	ar << p;
//	Send(ar);
//}

void XGameUser::SendUserNotify( _tstring strMessage )
{
	XPacket ar((ID)xCL2GS_SYSTEM_GAME_NOTIFY);
	ar << strMessage;
	Send(ar);
}

void XGameUser::SendUserNotifyTest()
{
	//_tstring strNotifymessage;
	//int r = rand() % 3;
	//if ( r == 0 )
	//	strNotifymessage = _T(" 공지 메세지 테스트 중입니다. 1");
	//else if (r == 1)
	//	strNotifymessage = _T("공지 메세지 테스트 중입니다. 2");
	//else if (r == 2)
	//	strNotifymessage = _T("공지 메세지 테스트 중입니다. 3");
	//XPacket ar((ID)xCL2GS_SYSTEM_GAME_NOTIFY);
	//ar.WriteString(strNotifymessage.c_str());
	//Send(ar);
}
// 다음은 서버 코드입니다.
/**
 아직 스폰이 안되어 있을때 강제로 스폰을 시킨다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqRegenSpot()
*/
int XGameUser::RecvRegenSpot( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XVERIFY_BREAK( idSpot == 0 );
	auto pSpot = GetpWorld()->GetSpot( idSpot );
	// 리젠 타임을 강제로 돌린다.
	XVERIFY_BREAK( pSpot == nullptr );
	pSpot->ClearSpot();
//	pSpot->ResetSpot();
	pSpot->DoSpawnSpot();
	// 캐쉬 깜.
	int cost = m_spAcc->GetCostCashSkill( XGAME::xCS_REGEN_SPOT);
	m_spAcc->AddCashtem( -cost );

	//
	XPacket ar( (ID)xCL2GS_LOBBY_REGEN_SPOT );
	ar << idSpot;
	ar << m_spAcc->GetCashtem();
	Send( ar );
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqReMatchSpot()에 대한 서버측의 Receive함수
 이미 스폰이 되어있는상태에서 다시 상대를 바꾼다.
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqReMatchSpot()
*/
int XGameUser::RecvReMatchSpot( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XVERIFY_BREAK( idSpot == 0 );
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	pBaseSpot->ClearSpot();		// 일단 초기화 시키고.
	bool bOk = false;
	switch( pBaseSpot->GettypeSpot() )
	{
	case XGAME::xSPOT_CASTLE: {
		int cost = XGC->GetGoldChangePlayer( GetLevel() );
		if( pBaseSpot->IsSuccessiveDefeat() ) {
			cost = 0;
			pBaseSpot->SetnumLose( 0 );
		}
		auto pSpot = SafeCast2<XSpotCastle*>( pBaseSpot );
		if( pSpot ) {
			if( m_spAcc->IsEnoughGold( cost ) ) {
				DoMatchUserSpot( pSpot );
				m_spAcc->AddGold( -cost );
				bOk = true;
			} else {
//       금화가 부족하면 부족한만큼 캐쉬로 깐다.
//       금화량은 0으로 만든다.
				int goldNeed = cost - (int)m_spAcc->GetGold();
				int cash = m_spAcc->GetCashFromGold( goldNeed );
				if( m_spAcc->IsEnoughCash( cash ) ) {
					m_spAcc->AddCashtem( -cash );   // 부족한 양을 캐쉬로 결제
					m_spAcc->ClearGold();     // 금화는 모두 소모
					// DelegateOnSpawn을 사용하지 않는 이유는,이제 스폰타이머가 지나면자동매치되지 않고 ?상태로 변하기만 하는걸로 바껴서 그럼.
					DoMatchUserSpot( pSpot );
					bOk = true;
				}
			}
		}
	} break;
	case XGAME::xSPOT_JEWEL: {
		int cost = XGC->m_cashChangePlayer;
		if( m_spAcc->IsEnoughCash( cost ) ) {
			auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
			DoMatchJewel( pSpot );
			m_spAcc->AddCashtem( -cost );
			bOk = true;
		}
	} break;
	case XGAME::xSPOT_SULFUR:
	case XGAME::xSPOT_NPC: {
		int cost = XGC->m_cashChangePlayer;
		if( m_spAcc->IsEnoughCash( cost ) ) {
			pBaseSpot->ClearSpot();		// 클리어 시키고
// 			pBaseSpot->ResetSpot();		// 스폰타이머 다시 리셋시키고
// 			pBaseSpot->CreateLegion( m_spAcc );
			pBaseSpot->DoSpawnSpot();	// 강제로 스폰을 시킴.
// 			pBaseSpot->OnSpawn( m_spAcc );
			m_spAcc->AddCashtem( -cost );
			bOk = true;
		}
	}	break;
	case XGAME::xSPOT_MANDRAKE: {
		const int cost = XGC->m_cashChangePlayer;
		if( m_spAcc->IsEnoughCash( cost ) ) {
			auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
			DoMatchMandrake( pSpot );
			m_spAcc->AddCashtem( -cost );
			bOk = true;
		}
	} break;
	default:
		XBREAK( 1 );
		break;
	}
	// 캐쉬 깜.
	//	int cost = m_spAcc->GetCostCashSkill( XGAME::xCS_CHANGE_PLAYER );
	//  m_spAcc->AddCashtem( -cost );
	//
	XPacket ar( (ID)xCL2GS_LOBBY_REMATCH_SPOT );
	ar << idSpot;
	ar << m_spAcc->GetCashtem();
	ar << m_spAcc->GetGold();
	ar << (BYTE)pBaseSpot->GettypeSpot();
	ar << (BYTE)xboolToByte( bOk );
	ar << (BYTE)0;
	ar << (BYTE)0;
	if( bOk ) {
		switch( pBaseSpot->GettypeSpot() ) {
		case XGAME::xSPOT_SULFUR:
		case XGAME::xSPOT_NPC: {
			XSpot::sSerialize( ar, pBaseSpot );
		}	break;
		}
	}
	Send( ar );

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqSulfurRetreat()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSulfurRetreat()
*/
int XGameUser::RecvSulfurRetreat( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XVERIFY_BREAK( idSpot == 0 );
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	//XVERIFY_BREAK( m_idBattleSpot != idSpot );	// 전투하던 스팟이 아니면 실패.
	// 캐쉬 깜.
	int cost = m_spAcc->GetCostCashSkill( XGAME::xCS_SULFUR_RETREAT );
	XVERIFY_BREAK( m_spAcc->IsEnoughCash( cost ) == false );
	m_spAcc->AddCashtem( -cost );
	//
	auto pSpot = SafeCast<XSpotSulfur*, XSpot*>( pBaseSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	//
	XPacket ar( (ID)xCL2GS_INGAME_SULFUR_RETREAT );
	ar << idSpot;
	ar << m_spAcc->GetCashtem();
	XGAME::xBattleResult result;
	result.SetWin( true );
	int clearSpot = DoRewardProcessForSulfur( pSpot, &result );
	m_spAcc->AddBattleLog( true, result.logForAttacker );
	Send( ar );

	return 1;
}

/**
 @brief 계정 버프의 시간이 끝남.
*/
void XGameUser::DelegateFinishBuff( XSPAcc spAcc, const XGAME::xBuff& buff )
{
	// 버프가 끝났음을 클라에 알림.
	XPacket ar( (ID)xCL2GS_FINISH_BUFF );
	ar << buff.idBuff;
	Send( ar );
}

/**
 @brief 서버로부터 buff발생.
*/
void XGameUser::SendOccurBuff( XGAME::xBuff *pBuff )
{
	XPacket ar( (ID)xCL2GS_OCCUR_BUFF );
	pBuff->Serialize( ar );
	Send( ar );
}

// 다음은 서버 코드입니다.
/**
x전투시작
 클라이언트의 SendReqBattleStart()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqBattleStart()
*/
int XGameUser::RecvBattleStart( XPacket& p )
{
	// 그냥 바로 답장줌.
	XPacket ar( (ID)xCL2GS_INGAME_BATTLE_START );
	Send( ar );
	return 1;
}

/**
 @brief 클라로부터 스팟의 전투정보를 요청받음.
 클라이언트의 SendReqUpdateSpotForBattle()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqUpdateSpotForBattle()
*/
int XGameUser::RecvUpdateSpotForBattle( XPacket& p )
{
	ID idSpot;
	XArchive arAdd;		// 추가정보
	p >> idSpot;
	p >> arAdd;
	auto pSpot = GetpWorld()->GetpSpot( idSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	const xtSpot type = pSpot->GettypeSpot();
	// 전투에 필요한 최소한의 정보만 팩킹해서 보낸다.
	XArchive arLegion;
	XParamObj2 param;
	pSpot->SerializeForBattle( &arLegion, param );

	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << idSpot;
	ar << arLegion;
	ar << arAdd;
	Send(ar);
	return 1;
}

void XGameUser::SendAddBattleLog( bool bAttack, XGAME::xBattleLog& log )
{
	XPacket ar( (ID)xCL2GS_INGAME_ADD_BATTLE_LOG );
	ar << (BYTE)xboolToByte(bAttack);
	ar << (BYTE)VER_BATTLE_LOG;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << log;
	Send( ar );
}
/**
 @brief 생성한 영웅을 클라에 동기화시킨다.
*/
void XGameUser::SendCreateHero( XHero *pHero )
{
	XPacket ar( (ID)xCL2GS_CREATE_HERO );
	XHero::sSerialize( ar, pHero );
	Send( ar );
}


// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqWithdrawMandrake()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqWithdrawMandrake()
*/
int XGameUser::RecvWithdrawMandrake( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XVERIFY_BREAK( idSpot == 0 );
	auto pSpot = GetpWorld()->GetpSpotMandrake( idSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	// 만드레이크 밭의 보상을 수거
	m_spAcc->AddResource( XGAME::xRES_MANDRAKE, pSpot->GetReward() );
	// 스팟의 초기화
	pSpot->Initialize( m_spAcc );
	Save();
	XPacket ar( (ID)xCL2GS_LOBBY_WITHDRAW_MANDRAKE );
	ar << m_spAcc->GetResource( XGAME::xRES_MANDRAKE );
	ar << pSpot->GetidSpot();
	XSpot::sSerialize( ar, pSpot );
	Send( ar );



	return 1;
}

void XGameUser::RecvPushMsgRegist(XPacket& p)
{
	int type1 = 0;
	ID type2 = 0;
	ID idacc = 0;
	_tstring strBuff;
	int recvtime =0;
	p >> idacc;
	p >> type1;
	p >> type2;
	p >> recvtime;
	p >> strBuff;

// 	CONSOLE( "%s: idacc=%d, type1=%d, type2=%d recvtime=%d, str=%s"
// 					, __TFUNC__
// 					, idacc
// 					, type1, type2
// 					, recvtime
// 					, strBuff.c_str() );
	DBA_SVR->SendPushMsgRegist(idacc, type1, type2, recvtime, strBuff);
}
void XGameUser::RecvPushMsgUnRegist(XPacket& p)
{
	ID idacc = 0;
	int type1 = 0;
	ID type2 = 0;
	

	p >> idacc;
	p >> type1;
	p >> type2;
	
	DBA_SVR->SendPushMsgUnRegist(idacc, type1, type2);
}
void XGameUser::AddLog(int logtype, _tstring strLog)
{
	if (int(XGC->m_LogLevel) > logtype) {
		DBA_SVR->SendUserLog(GetidAcc(), GetstrName(), logtype, strLog);
	}	
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqGCMResistId()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqGCMResistId()
*/
int XGameUser::RecvGCMResistId( XPacket& p )
{
	_tstring strRegid, strPlatform;
	p >> strRegid;
	p >> strPlatform;

	auto codeErr = ( XGAME::xtError )xE_OK;
	do {
		if( strRegid.empty() ) {
			codeErr = xE_INVALID_GCM_REGIST_ID;
			break;
		}
		ID idKey = DBA_SVR->SendReqGCMRegistId( GetidAcc(), strRegid, strPlatform );
		DBA_SVR->AddResponse( idKey, this, &XGameUser::cbReqGCMRegistId );
	} while (0);

	if( codeErr != xE_OK ) {
		// 클라로 전송
		XPacket ar( p.GetidPacket() );
		ar << (DWORD)codeErr;
		Send( ar );
	}
	return 1;
}

/**
 @brief DBA서버로부터 결과가 돌아옴.
*/
void XGameUser::cbReqGCMRegistId( XPacket& p )
{
	// 클라로 전송
	XPacket ar( (ID)xCL2GS_PUSH_GCM_REGIST_ID );
	ar << (DWORD)XGAME::xE_OK;
	Send( ar );
}




// 다음은 서버 코드입니다.
/**
 클라이언트의 SendAccepLevelUp()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendAccepLevelUp()
*/
int XGameUser::RecvAccepLevelUp( XPacket& p )
{
	m_spAcc->GetXFLevelObj().SetbLevelUp( FALSE );
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendFlagTutorial()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendFlagTutorial()
*/
int XGameUser::RecvFlagTutorial( XPacket& p )
{
	DWORD dw0;
	p >> dw0;	
	auto& bitFlag = m_spAcc->GetFlagTutorial();
	*((DWORD*)(&bitFlag)) = dw0;
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendEnterScene()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendEnterScene()
*/
int XGameUser::RecvUIAction( XPacket& p )
{
	DWORD dw0, dwParam2;
	p >> dw0;		auto uiAction = ( XGAME::xtUIAction )dw0;
	p >> dwParam2;
//	XVERIFY_BREAK( idScene <= 0 || idScene >= XGAME::xSCENE_MAX );
	XVERIFY_BREAK( uiAction <= 0 || uiAction >= XGAME::xUA_MAX );
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, uiAction, dwParam2 );
	return 1;
}

// 다음은 서버 코드입니다.
/**
 @brief 캠페인 별점보상을 클릭
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqCampaignReward()
*/
int XGameUser::RecvCampaignReward( XPacket& p )
{
	ID idSpot;
	ID idCamp;
	int idxStage;
	p >> idSpot;
	p >> idCamp;
	p >> idxStage;
	auto pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	XVERIFY_BREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_CAMPAIGN );
	auto pSpot = SafeCast<XSpotCampaign*>( pBaseSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	auto spCamp = pSpot->GetspCampObj();
	XVERIFY_BREAK( spCamp == nullptr );
	XVERIFY_BREAK( spCamp->GetidProp() != idCamp );
	// idxStage의 보상을 주려면 별이 몇개가 필요한지.
	int needStar = 3 + idxStage * 3;	
	XVERIFY_BREAK( spCamp->GetnumStar() < needStar );
	auto spStage = spCamp->GetspStage( idxStage );
	XVERIFY_BREAK( spStage == nullptr );
	XArrayLinearN<XBaseItem*,256> aryItems;
	bool bRecvRes = false;
	bool bRecvGold = false;
	bool bRecvCash = false;
	bool bRecvGuildCoin = false;
	XGAME::xtError errCode = XGAME::xE_OK;
	do {
		if( spStage->GetbRecvReward() ) {
			// 이미 보상받음
			bRecvRes = bRecvGold = bRecvCash = bRecvGuildCoin = true;
			errCode = xE_ALREADY_RECV_REWARD;
			break;
		}
		for( auto& reward : spStage->GetspPropStage()->aryReward ) {
			int num = reward.num;
			XBREAK( num < 0 );
			if( num == 0 )
				num = 1;
			xQuest::XEventInfo infoQuest;		// 이벤트 정보.
			infoQuest.SetidArea( pSpot->GetidArea() );
			infoQuest.SetidSpot( pSpot->GetidSpot() );
			infoQuest.SettypeSpot( pSpot->GettypeSpot() );
			infoQuest.SetidCode( pSpot->GetpBaseProp()->idCode );
			switch( reward.rewardType )
			{
			case XGAME::xtReward::xRW_ITEM: {
				m_spAcc->CreateItemToInven( reward.idReward, num, &aryItems );
				// 퀘스트시스템에 드랍한 물건의 정보를 보냄
				infoQuest.SetidItem( reward.idReward );
				infoQuest.SetAmount( num );
				DispatchQuestEvent( XGAME::xQC_EVENT_GET_ITEM, infoQuest );
			} break;
			case XGAME::xtReward::xRW_GOLD: {
				m_spAcc->AddGold( num );
				bRecvGold = true;
			} break;
			case XGAME::xtReward::xRW_CASH: {
				m_spAcc->AddCashtem( num );
				bRecvCash = true;
			} break;
			case XGAME::xtReward::xRW_RESOURCE: {
				m_spAcc->AddResource( ( XGAME::xtResource )reward.idReward, num );
				bRecvRes = true;
			} break;
			case XGAME::xtReward::xRW_HERO: {
				auto pPropHero = PROP_HERO->GetpProp( reward.idReward );
				if( XASSERT( pPropHero ) ) {
					auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, XGAME::xSIZE_SMALL );
					auto pHero = XHero::sCreateHero( pPropHero, 1, unit );
					XVERIFY_BREAK( pHero == nullptr );
					m_spAcc->AddHero( pHero );
					infoQuest.SetidHero( reward.idReward );
					infoQuest.SetAmount( num );
					DispatchQuestEvent( XGAME::xQC_EVENT_GET_HERO, infoQuest );
					SendCreateHero( pHero );
				}
			} break;
			case XGAME::xtReward::xRW_GUILD_POINT: {
				m_spAcc->AddGuildPoint( num );
				bRecvGuildCoin = true;
			} break;
			default:
				XBREAK( 1 );
				break;
			}
		} // for
		spStage->SetbRecvReward( true );
	} while(0);
	if( aryItems.size() )
		SendItemsList( aryItems );
	if( bRecvRes )
		SendSyncResource();
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << (DWORD)errCode;
	ar << idSpot;
	ar << idCamp;
	ar << idxStage;
	if( bRecvGold )
		ar << m_spAcc->GetGold();
	else
		ar << 0;
	if( bRecvCash )
		ar << m_spAcc->GetCashtem();
	else
		ar << 0;
	if( bRecvRes )
		m_spAcc->SerializeResource2( ar );
	else
		ar << 0 ;
	xCampaign::XStageObj::sSerialize( ar, spStage );
	Send(ar);
	// 모든 스테이지의 보상을 다 받았으면 스팟 삭제한다.
	if( spCamp->IsRecvAllReward() ) {
		pSpot->SetbDestroy( true );
		SendSpotSync( pSpot, true );
	}
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqTrainHeroByGold()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqTrainHeroByGold()
 Training
*/
int XGameUser::RecvTrainHeroByGold( XPacket& p )
{
	ID snHero;
	int gold;
	BYTE b0;
	p >> snHero;
	p >> gold;
	p >> b0;	auto typeTrain = (XGAME::xtTrain)b0;
	p >> b0 >> b0 >> b0;
	XVERIFY_BREAK( typeTrain <= XGAME::xTR_NONE || typeTrain >= XGAME::xTR_MAX );
	XVERIFY_BREAK( gold > (int)m_spAcc->GetGold() );
	XVERIFY_BREAK( gold <= 0 );
	XVERIFY_BREAK( m_spAcc->GetNumRemainFreeSlot() <= 0 );
	auto pHero = m_spAcc->GetHero( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	XVERIFY_BREAK( pHero->IsMaxLevel( typeTrain ) );
	int expByGold = 0;
	int secByGold = 0;
	int goldRemain = 0;
	m_spAcc->GetTrainExpByGoldCurrLv( pHero, gold, typeTrain, &expByGold, &secByGold, &goldRemain );
	if( goldRemain > 0 )
		gold += goldRemain;
	m_spAcc->AddGold( -gold );
	// 훈련시작
	XAccount::xTrainSlot slot;
	slot.SetAddExp( expByGold );
	slot.DoStartTrain( typeTrain, pHero->GetsnHero(), (float)secByGold );
//	slot.DoStartLevelup( pHero->GetsnHero(), (float)secTrain );
//	int secTrain = infoTrain.secTotal;
	ID snSlot = m_spAcc->AddTrainSlot( slot );
	switch( typeTrain )
	{
	case XGAME::xTR_LEVEL_UP:
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_TRAINING_LEVEL );
		break;
	case XGAME::xTR_SQUAD_UP:
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_TRAINING_SQUAD );
		break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_TRAINING_SKILL );
		break;
	default:
		XBREAK(1);
		break;
	}

	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << snHero;
	ar << (BYTE)VER_ETC_SERIALIZE;
	ar << (BYTE)typeTrain;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << expByGold;
	ar << m_spAcc->GetGold();
	ar << snSlot;
	m_spAcc->SerializeTrainSlot( ar );
	Send(ar);
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqSendReqClickFogSquad()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSendReqClickFogSquad()
*/
int XGameUser::RecvClickFogSquad( XPacket& p )
{
	ID idSpot, snHero;
	int idxStage;
	p >> idSpot >> snHero;
	p >> idxStage;
	XSpot* pBaseSpot = GetpWorld()->GetSpot( idSpot );
	XVERIFY_BREAK( pBaseSpot == nullptr );
	auto spLegion = pBaseSpot->GetspLegion();
	XVERIFY_BREAK( spLegion == nullptr );
	auto pSquad = spLegion->GetSquadronByHeroSN( snHero );
	XVERIFY_BREAK( pSquad == nullptr );
	auto pHero = pSquad->GetpHero();
	XVERIFY_BREAK( pHero == nullptr );
	bool bFog = spLegion->IsFog( snHero );
	XVERIFY_BREAK( bFog == false );
	int goldCost = m_spAcc->GetCostOpenFog( spLegion );
	XVERIFY_BREAK( !m_spAcc->IsEnoughGold( goldCost ) );
	// 정찰비용 깜.
	m_spAcc->AddGold( -goldCost );
	// snHero부대의 안개 제거
	spLegion->DelFogSquad( snHero );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << idSpot << snHero;
	ar << idxStage;
	ar << m_spAcc->GetGold();
	Send(ar);
	return 1;
}
/**
 클라이언트의 SendCheckUnlockUnit()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendCheckUnlockUnit()
*/
int XGameUser::RecvCheckUnlockUnit( XPacket& p )
{
	int i0;
	p >> i0;		auto unit = (XGAME::xtUnit)i0;
	XVERIFY_BREAK( m_spAcc->IsLockUnit( unit ) );
	m_spAcc->SetCheckUnlockUnit( unit );
	return 1;
}
/**
 @brief 튜토리얼용. 전투준비씬에서 아군 부대를 선택함.
 클라이언트의 SendTouchSquadInReadyScene()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendTouchSquadInReadyScene()
*/
int XGameUser::RecvTouchSquadInReadyScene( XPacket& p )
{
	auto& bitFlag = m_spAcc->GetFlagTutorial();
	XVERIFY_BREAK( bitFlag.bTouchSquadInReady  );		// 이미 한번 한거는 클라에서 보내면 안됨.
	int idxStage;
	p >> idxStage;
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_TOUCH_SQUAD_IN_READY );
	bitFlag.bTouchSquadInReady = 1;
	return 1;
}

/**
 @brief 튜토리얼용. 전투씬에서 부대를 수동조작함. 
 클라이언트의 SendControlSquadInBattle()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendControlSquadInBattle()
*/
int XGameUser::RecvControlSquadInBattle( XPacket& p )
{
	auto& bitFlag = m_spAcc->GetFlagTutorial();
	XVERIFY_BREAK( bitFlag.bControlSquadInBattle );		// 이미 한번 한거는 클라에서 보내면 안됨.
	ID snHero;
	p >> snHero;
	DispatchQuestEvent( xQC_EVENT_UI_ACTION, xUA_CONTROL_SQUAD );
	bitFlag.bControlSquadInBattle = 1;
	return 1;
}
/**
 @brief 내 본진 가상 침공 시뮬레이션.
 주사위를 굴려 침공당했으면 강제로 더미를 하나 매칭해와서 SimulAttackedHome을 돌린다.
*/
void XGameUser::ProcessAttackedHome( xSec secPass )
{
	if( secPass == 0 )
		return;
	// 진짜 침공이 없었을때만 시뮬레이션 함.
	if( m_spAcc->GetnumAttaccked() != 0 )
		return;
	if( secPass > (xSec)XAccount::s_secOfflineForSimul ) {
		bool bOk = ( xRandom( 3 ) == 0 );	// 침공
		if( bOk ) {
			int powerMin = (int)( m_spAcc->GetPowerIncludeEmpty() * 1.2 );
			int powerMax = (int)( m_spAcc->GetPowerIncludeEmpty() * 0.8 );
			ID idKey =
				DBA_SVR->SendReqGetUserByPower( m_spAcc->GetidAccount(),
																				0,		// idSpot
																				powerMin,
																				powerMax,
																				0,		// secAdjustOffline
																				GetidConnect(),
																				xCL2GS_LOBBY_SIMUL_ATTACKED_HOME );
			DBA_SVR->AddResponse( idKey,
														this,
														&XGameUser::RecvGetUserByPower );
		}
	}
}
/**
 @brief 본성이 침공당한것처럼 시뮬레이션 한다.
*/
void XGameUser::SimulAttackedHome( ID idAcc, int lvAcc, const _tstring& strName, const int power, int ladder )
{
	XBREAK( m_spAcc->GetPowerIncludeEmpty() == 0 );
	XBREAK( power == 0 );
	const bool bWin = ( xRandom(2)==0 );		// 방어성공
	m_spAcc->SetnumAttaccked( m_spAcc->GetnumAttaccked() + 1 );
	XGAME::xBattleLog log;
	log.idEnemy = idAcc;
	log.strName = strName;
	log.m_lvAcc = lvAcc;
	log.score = (WORD)ladder;
	log.powerBattle = power;
	log.bRevenge = false;
	log.bWin = bWin;
	log.sec = XTimer2::sGetTime() - xRandom( xHOUR( 1 ) ) - 60;
	// 상대가 가져갈수 있는 최대 루팅양 계산.
	// 중앙창고 최대 가능 약탈량 계산.
	std::vector<int> aryLossMain( XGAME::xRES_MAX );
	const int powerGradeDefender = XGAME::GetHardLevel( power, m_spAcc->GetPowerIncludeEmpty() );
	const float rateLossMain = XGC->GetRateLossMain( powerGradeDefender );
	for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
		auto resType = ( XGAME::xtResource )i;
		auto amount = m_spAcc->GetResource( resType );
		int amountLoss = (int)( amount * rateLossMain );
		if( amountLoss > 0 )
			aryLossMain[ resType ] += amountLoss;
	}
	//
	if( bWin ) {
		// 공격자(가상더미)로부터 방어를 성공함.
		log.addScore = xRandom( 5, 10 );
		// 자원부대 손실만 적용함.
		const auto rateLootSquad = XGC->m_rateLootResourceSquad;
		for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
			auto resType = ( XGAME::xtResource )i;
			int amountLoss = (int)(aryLossMain[ resType ] * rateLootSquad);
			if( amountLoss > 0 ) {
				amountLoss = xRandom( amountLoss );
				if( amountLoss > 0 ) {
					log.AddLootRes( resType, amountLoss );
					m_spAcc->AddResource( resType, -amountLoss );
				}
			}
		}

		std::vector<int> aryLoss(XGAME::xRES_MAX);
// 		int powerGradeDefender = XGAME::GetHardLevel( power, m_spAcc->GetPowerIncludeEmpty() );
// 		const float rateLossMain = XGC->GetRateLossMain( powerGradeDefender );
		for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
			auto resType = (XGAME::xtResource)i;
			auto amount = m_spAcc->GetResource( resType );
			int amountLoss = (int)(amount * rateLossMain);
			if( amountLoss > 0 ) {
				aryLoss[ i ] += amountLoss;
				if( XASSERT(amount - amountLoss >= 0) )
					m_spAcc->SetResource( resType, amount - amountLoss );
			}
		}
		// 지역창고에 손실을 입힌다.
		const float rateLossLocal = XGC->GetRateLossLocal( powerGradeDefender );
		GetpWorld()->LossLocalStorageAll( rateLossLocal, &aryLoss );
		// 중앙/지역 손실량을 합산해서 로그에 넣는다.
		for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
			auto resType = ( XGAME::xtResource )i;
			if( aryLoss[i] > 0 )
				log.AddLootRes( resType, aryLoss[i] );	// 약탈량합산
		}
	} else {
		// 공격자(가상더미)로부터 방어에 실패함.
		log.addScore = -xRandom( 5, 10 );
		// 지역창고에 손실을 입힌다.
		std::vector<int> aryLossLocal( XGAME::xRES_MAX );
		const float rateLossLocal = XGC->GetRateLossLocal( powerGradeDefender );
		GetpWorld()->LossLocalStorageAll( rateLossLocal, &aryLossLocal );
		// 중앙/지역 손실량을 합산해서 로그에 넣는다.
		for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
			auto resType = ( XGAME::xtResource )i;
			int amountLoot = aryLossMain[ i ] + aryLossLocal[ i ];
			if( amountLoot > 0 )
				log.AddLootRes( resType, amountLoot );	// 약탈량합산
		}
	}
	auto& listLog = m_spAcc->GetlistBattleLogDefense();
	listLog.Add( log );

	// 클라로 결과 보내줌.
	XPacket ar( (ID)xCL2GS_LOBBY_SIMUL_ATTACKED_HOME );
	ar << (BYTE)m_spAcc->GetnumAttaccked();
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	m_spAcc->SerializeResource( ar );
	ar << log;
	Send( ar );
	///<
	m_spAcc->SetnumAttaccked( 0 );
}
/**
 @brief 내 소유상태인 보석광산중에 매치된상대가 더미면 그 더미가 날 공격한것처럼 시뮬레이션 한다.
 .최초 매치시 맞는 상대가 없어서 매치가 안됐다면 시뮬레이션 하지않고 놔둔다. 이것은 실제유저가 날 매치하게 하기 위해서다.
 .혹은 실제유저가 날 포기했을때도 그냥 놔두기로 한다.
 .실제유저간 배틀활성화를 위해서 실제유저로만 매칭을 시도해보고 없으면 더미를 매칭하도록 하자.
*/
void XGameUser::ProcessAttackedJewel( xSec secOffline )
{
	// 	if( 진짜침공이 있었는가)
	// 		return;
	if( secOffline < (xSec)XAccount::s_secOfflineForSimul )
		return;
	XVector<XSpotJewel*> aryJewels;
	m_spAcc->GetpWorld()->GetSpotsToAry( &aryJewels, XGAME::xSPOT_JEWEL );
	//
	for( auto pSpot : aryJewels ) {
		// 현재 광산이 내가 점령중일때만.
		if( pSpot->IsMySpot( GetidAcc() ) ) {
			// 첫 번째 광산을 매치해보고 매치가 안됐다면 실제 유저가 나를 매치할수 있게 그냥 놔둔다.(그러나 어지간해선 더미라도 매치가 될것이다.)
			// 만약 더미가 매치되었다면 그 더미가 날 공격한것처럼 시뮬레이션 한다.
			if( pSpot->IsMatchEnemyIsDummy() ) {
				bool bInvaded = ( xRandom( 3 ) == 0 );
				if( bInvaded ) {
					int diceMax = pSpot->GetDefense();
					if( diceMax < 1 )
						diceMax = 99;
					const auto dice = xRandom( diceMax );
					bool bWin = ( dice < 1 );
					if( bWin ) {
						// 방어성공
						pSpot->DoDefenseDamage();		// 방어도 하락 시킴.
						CONSOLE_ACC( "jewel_simul", "보석광산: 더미로부터 침공당함/방어성공" );
						// 내꺼였으며 매치상대가 있었던 경우.
						// 매칭상대에게 광산의 정보 업데이트 시킴.
						const ID idDBAcc = pSpot->GetidMatchEnemy();
						DBA_SVR->SendUpdateJewelMineInfo( idDBAcc, GetidAcc(), pSpot );
					} else {
						// 방어실패(스팟 뺏김)
						CONSOLE_ACC( "jewel_simul", "보석광산: 더미로부터 침공당함/방어실패" );
						// 내꺼였으며 매치상대가 있었던 경우.
						// 상대에게 뺏긴걸로 처리한다.
						ID idDBAcc = pSpot->GetidMatchEnemy();
						pSpot->ChangeOwnerByWinForDummy();
						// 매치상대의 DB에 정보를 업데이트 한다.
						DBA_SVR->SendUpdateJewelMineInfo( idDBAcc, GetidAcc(), pSpot );
					}
				}
			}
		}
	}
}
// void XGameUser::ProcessAttackedJewel( xSec secOffline )
// {
// // 	if( 진짜침공이 있었는가)
// // 		return;
// 	if( secOffline < XAccount::s_secOfflineForSimul )
// 		return;
// 	XVector<XSpotJewel*> aryJewels;
// 	m_spAcc->GetpWorld()->GetSpotsToAry( &aryJewels, XGAME::xSPOT_JEWEL );
// 	//
// 	for( auto pSpot : aryJewels ) {
// 		// 현재 광산이 내가 점령중일때만.
// 		if( pSpot->IsMySpot( GetidAcc() ) ) {
// 			// 매치상대가 없는상태이거나 매치된상대가 더미면 시뮬레이션함.
// 			if( pSpot->GetidMatchEnemy() == 0 || pSpot->IsMatchEnemyIsDummy() ) {
// 				bool bInvaded = (xRandom(3) == 0);
// 				if( bInvaded /*|| 1*/ ) {
// 					int diceMax = pSpot->GetDefense();
// 					if( diceMax < 1 )
// 						diceMax = 99;
// 					const auto dice = xRandom(diceMax);
// 					bool bWin =	( dice < 1 );
// 					if( bWin ) {
// 						pSpot->DoDefenseDamage();		// 방어도 하락 시킴.
// 						// 내꺼였으며 아직 매치상대가 없었던 경우.
// 						if( pSpot->GetidMatchEnemy() == 0 ) {
// 							CONSOLE_ACC( "jewel_simul", "보석광산: 첫 침공당함/방어성공" );
// 							const bool bSimul = true;
// 							DoMatchJewel( pSpot, bSimul, bWin );
// 						} else {
// 							CONSOLE_ACC( "jewel_simul", "보석광산: 재 침공당함/방어성공" );
// 							// 내꺼였으며 매치상대가 있었던 경우.
// 							// 매칭상대에게 광산의 정보 업데이트 시킴.
// 							const ID idDBAcc = pSpot->GetidMatchEnemy();
// 							DBA_SVR->SendUpdateJewelMineInfo( idDBAcc, GetidAcc(), pSpot );
// 						}
// 					} else {
// 						// 패배(스팟 뺏김)
// 						if( pSpot->GetidMatchEnemy() == 0 ) {
// 							CONSOLE_ACC( "jewel_simul", "보석광산: 첫 침공당함/방어실패" );
// 							// 내꺼였으며 매치상대가 없었던 경우.
// 							const bool bSimul = true;
// 							DoMatchJewel( pSpot, bSimul, bWin );
// 						} else {
// 							CONSOLE_ACC( "jewel_simul", "보석광산: 재 침공당함/방어실패" );
// 							// 내꺼였으며 매치상대가 있었던 경우.
// 							// 상대에게 뺏긴걸로 처리한다.
// 							ID idDBAcc = pSpot->GetidMatchEnemy();
// 							pSpot->ChangeOwnerByWin();
// 							// 매치상대의 DB에 정보를 업데이트 한다.
// 							DBA_SVR->SendUpdateJewelMineInfo( idDBAcc, GetidAcc(), pSpot );
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// }
#ifdef _DEV
void XGameUser::ProcessAttackedMandrake( xSec secOffline )
{
	if( secOffline < xHOUR(1) )
		return;
	XVector<XSpotMandrake*> arySpot;
	m_spAcc->GetpWorld()->GetSpotsToAry( &arySpot, XGAME::xSPOT_MANDRAKE );
	//
	for( auto pSpot : arySpot ) {
		const auto state = pSpot->GetState( GetidAcc() );
		// 현재 내가 점령중일때.
		if( state == xSpot::xSM_DEFENSE ) {
			// 오프라인시간당 1회의 기회로 랜덤을 돌려 침공시도 횟수를 얻음.
			const bool bInvaded = (xRandom(3) == 0);		// 다시 1/3확률로 침공당할지 말지 결정함.
			if( bInvaded ) {
				const bool bWin = ( xRandom(2) == 0 );
				if( bWin ) {
					// 오프라인동안 몇번이나 연승했는지.
					const int numWin = xRandom( (secOffline / xHOUR( 1 )) ) + 1 ;
					for( int i = 0; i < numWin; ++i ) {
						// 방어성공
//	#ifdef _XUZHU
						CONSOLE_ACC( "mandrake", "만드레이크: 침공당함/방어성공" );
//	#endif // _XUZHU
						pSpot->AddWin( 1 );
						pSpot->SetReward( pSpot->GetReward() * 2 );
					} // for
				} // if( bWin ) {
				else {
					// 방어실패
					const int numWin = pSpot->GetWin();
					const int reward = pSpot->GetReward();
					pSpot->Initialize( m_spAcc );
					pSpot->SetWin( numWin );
					pSpot->SetReward( reward );
					break;
				}
			} // if( bInvaded ) {
		} // if( state == xSpot::xSM_DEFENSE ) {
	}
}
#endif // _DEV

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendCheckEncounterLog()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendCheckEncounterLog()
*/
int XGameUser::RecvCheckEncounterLog( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	XVERIFY_BREAK( idSpot == 0 );
	auto pSpot = GetpWorld()->GetpSpotSulfur( idSpot );
	XVERIFY_BREAK( pSpot == nullptr );
	XVERIFY_BREAK( pSpot->GetbCheckEncounterLog() == true );
	pSpot->SetbCheckEncounterLog( true );
	return 1;
}

/**
 @brief 앱이 홈으로 나감.
 클라이언트의 SendGotoHome()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendGotoHome()
*/
int XGameUser::RecvGotoHome( XPacket& p )
{
	int secTimeout;
	p >> secTimeout;
	auto spConnect = GetspConnect();
	// 허트비트 타이머를 리셋한다. 
	// "클라: 나 잠시 딴거할테니 secTimeout시간동안 내가 안오면 다시 HB를 보내도 좋다"
	spConnect->ResetHeartBeatTimer( secTimeout );
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqRegisterFacebook()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqRegisterFacebook()
*/
int XGameUser::RecvRegisterFacebook( XPacket& p )
{
	_tstring strFbUserId, strFbUsername;
	p >> strFbUserId;
	p >> strFbUsername;
	ID idKey = DBA_SVR->SendReqRegisterFacebook( GetidAcc()
																						, strFbUserId
																						, strFbUsername );
	DBA_SVR->AddResponse( idKey, this, &XGameUser::cbRegisterFacebook );
	return 1;
}

void XGameUser::cbRegisterFacebook(XPacket& p)
{
	ID idAcc;
	_tstring strFbUserId, strFbUsername;
	p >> idAcc;
	p >> strFbUserId >> strFbUsername;
	// 정찰결과 패킷일수도 있고 전투정보 패킷일수도 있다.
	XPacket ar( (ID)xCL2GS_ACCOUNT_REGISTER_FACEBOOK );		
	ar << idAcc;
	ar << strFbUserId;
	ar << strFbUsername;
	Send( ar );

}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqChangeHelloMsg()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqChangeHelloMsg()
*/
int XGameUser::RecvChangeHelloMsg( XPacket& p )
{
	_tstring strHello;
	p >> strHello;
	m_spAcc->SetstrHello( strHello );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << 1;
	Send(ar);
	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqInitAbil()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqInitAbil()
*/
int XGameUser::RecvInitAbil( XPacket& p )
{
	ID snHero;
	p >> snHero;
	auto pHero = m_spAcc->GetpHeroBySN( snHero );
	XVERIFY_BREAK( pHero == nullptr );
	// pHero의 특성초기화 비용을 얻는다.
	int cost = pHero->GetCostInitAbil();
	XVERIFY_BREAK( m_spAcc->IsNotEnoughGold(cost) );
	const auto bOk = pHero->DoInitAbil();
	XVERIFY_BREAK( bOk == false );
	m_spAcc->AddGold( -cost );
	const int numRemainPoint = pHero->GetnumRemainAbilPoint();
	const int numRemainUnlock = pHero->GetnumRemainAbilUnlock();
	XBREAK( numRemainPoint > 0xff ||  numRemainPoint < 0 );
	XBREAK( numRemainUnlock > 0xff || numRemainUnlock < 0 );
	const int cntInitAbil = pHero->GetnumInitAbil();
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << snHero;
	ar << (BYTE)numRemainPoint;
	ar << (BYTE)numRemainUnlock;
	ar << (BYTE)cntInitAbil;
	ar << (BYTE)0;
	ar << m_spAcc->GetGold();
	Send( ar );

	return 1;
}

// 다음은 서버 코드입니다.
/**
 클라이언트의 SendReqUnlockTrainingSlot()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqUnlockTrainingSlot()
*/
int XGameUser::RecvUnlockTrainingSlot( XPacket& p )
{
	int numFreeSlot;
	p >> numFreeSlot;
	XVERIFY_BREAK( m_spAcc->GetnumFreeSlot() != numFreeSlot );	// 그냥 동기검증용.
	const int cashCost = m_spAcc->GetCashUnlockTrainingSlot();
	XVERIFY_BREAK( m_spAcc->IsNotEnoughCash(cashCost) );
	XVERIFY_BREAK( numFreeSlot >= m_spAcc->GetMaxTrainSlot() );
	///< 
	m_spAcc->SetnumFreeSlot( numFreeSlot + 1 );
	m_spAcc->AddCashtem( -cashCost );
	const int numFreeSlotAfter = m_spAcc->GetnumFreeSlot();
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << numFreeSlotAfter;
	ar << m_spAcc->GetCashtem();
	Send(ar);
	return 1;
}

/**
 @brief 암호화된 퍼블릭키를 보낸다.
*/
void XGameUser::SendIAPPublicKey()
{
	MAKE_PCONNECT( pConnect );
	auto& cryptObj = pConnect->GetCryptObj();
	const int sizeDummy = 496;
	XPacket ar( (ID)xCL2GS_ACCOUNT_PUBLIC_KEY );
	cryptObj.SerializeEncryptWithString( XEnv::sGet()->GetstrPublicKey(), ar, sizeDummy );
	Send( ar );
}

/**
 @brief 스팟클리어 후 별점을 얻음.
 @param clearStar 클리어한 별점
*/
void XGameUser::DoCollectingSpotStar( int clearStar, XSpot* pBaseSpot )
{
	auto type = pBaseSpot->GettypeSpot();
	if( type == XGAME::xSPOT_CASTLE || type == XGAME::xSPOT_NPC ) {
		int numOld = m_spAcc->GetNumSpotStar( pBaseSpot->GetidSpot() );
		// 별스팟이 아닌경우는 numOld는 -1이 나옴.
		if( clearStar > numOld && numOld >= 0 ) {
			// 별 갱신해야 하는 상황
			if( XASSERT( numOld >= 0 ) ) {
				if( XASSERT( numOld <= 3 ) ) {
					int add = clearStar - numOld;
					XASSERT( add > 0 );
					xQuest::XEventInfo infoQuest;
					infoQuest.SetAmount( add );
					DispatchQuestEvent( XGAME::xQC_EVENT_GET_STAR, infoQuest );
					m_spAcc->SetNumStarBySpot( pBaseSpot, clearStar );
				}
			}
		}
	}
}

/**
 @brief 각종값 동기화 통합판.
*/
void XGameUser::SendSyncAcc( xtParamSync type )
{
	if( type == xPS_NO_SYNC || type == xPS_NONE )
		return;
	XPacket ar( (ID)xCL2GS_SYNC );
	ar << (int)type;
	switch( type ) {
	case xPS_ACC_LEVEL:
		m_spAcc->SerializeLevel( ar );
		break;
	case xPS_AP: {
		XBREAK( m_spAcc->GetmaxAP() > 0xffff );
		ar << (short)m_spAcc->GetAP();
		ar << (short)m_spAcc->GetmaxAP();
	} break;
	case xPS_RESROUCE: {
		m_spAcc->SerializeResource( ar );
	} break;
	case xPS_TRADER: {
		if( m_spAcc->IsOverTraderRecall() ) {
			ar << 1;
		} else
			ar << 0;
		m_spAcc->SerializeTimerByTrader( ar );
	} break;
	default:
		break;
	}
	Send( ar );
}

/**
 클라이언트의 SendReqSync()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqSync()
*/
int XGameUser::RecvSync( XPacket& p )
{
	int i0, param;
	p >> i0;		const auto type = (xtParamSync)i0;
	p >> param;

	// 클라로 전송
	SendSyncAcc( type );
	return 1;
}

/**
 클라이언트의 SendReqPrivateRaidEnterList()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqPrivateRaidEnterList()
*/
int XGameUser::RecvPrivateRaidEnterList( XPacket& p )
{
	ID idSpot;
	int size;
	p >> idSpot;
	p >> size;
	XVERIFY_BREAK( size > XSpotPrivateRaid::c_maxSquad );
	auto pSpot = SafeCast<XSpotPrivateRaid*>( GetpWorld()->GetpSpot( idSpot ) );
	XVERIFY_BREAK( pSpot == nullptr );
	XList4<XHero*> listEnterPlayer;
	for( int i = 0; i < size; ++i ) {
		ID snHero;
		p >> snHero;
		auto pHero = m_spAcc->GetpHeroBySN( snHero );
		XVERIFY_BREAK( pHero == nullptr );
		listEnterPlayer.push_back( pHero );
	}
	pSpot->UpdatePlayerEnterList( listEnterPlayer );

	// 클라로 전송(echo)
	XPacket ar( p.GetidPacket() );
	Send(ar);

// 	XGAME::xBattleStartInfo info( false, pSpot );
// 	info.m_idEnemy = 0;
// 	info.m_strName = _T("legion empire");
// 	SendBattleInfo( pSpot, &info );

	return 1;
}


/**
 클라이언트의 SendReqEnterReadyScene()에 대한 서버측의 Receive함수
 @param p 패킷이 들어있는 아카이브
 @return 오류없이 완료되었다면 1을 리턴한다.
 @see SendReqEnterReadyScene()
*/
int XGameUser::RecvEnterReadyScene( XPacket& p )
{
	ID idSpot;
	p >> idSpot;
	auto pSpot = SafeCast<XSpotPrivateRaid*>( GetpWorld()->GetpSpot( idSpot ) );
	XVERIFY_BREAK( pSpot == nullptr );
	// 클라로 전송
	XPacket ar( p.GetidPacket() );
	ar << idSpot;
	Send(ar);
	return 1;
}
