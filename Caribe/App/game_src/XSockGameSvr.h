#pragma once

#include "XPacketCG.h"
#include "Network/XSocket.h"
#include "XGameWnd.h"
#include "XGame.h"
#include "XGuildH.h"
#include "XGlobalConst.h"

namespace XGAME {
enum xtParamSync;
};

class XWnd;

/**
@brief 클라이언트에서 게임서버쪽으로의 연결을 하게될 소켓객체
*/
class XSockGameSvr : public XSOCKET
										, public XDelegateGuild
{
public:
// 	static std::shared_ptr<XSockGameSvr>& sGet();
private:
// 	static std::shared_ptr<XSockGameSvr> s_spInstance;
	struct xCALLBACK_PARAM {
		ID idParam;
	};
	struct xPARAM_RECON_SPOT : public xCALLBACK_PARAM {
		int sample;
	};
	struct xCALLBACK;
	struct xCALLBACK {
		void (XSOCKET::*pCallback)(XPacket& p, const xCALLBACK& c);
		ID idPacket;
		ID idCallback;
		xCALLBACK() {
			idPacket = 0;
			idCallback = 0;
			pCallback = nullptr;
		}
	};
	std::map<ID, xCALLBACK> m_mapCallback;
	struct xCALLBACK_WIN {
		void (XWnd::*pCallback)(xCALLBACK_PARAM *pParam);
		xCALLBACK_WIN() {
			pCallback = nullptr;
		}
	};
	std::map<std::string, xCALLBACK_WIN> m_mapCallback_Win;
private:
	BOOL m_bReconnect;
	int m_numRetryConnectGameSvr;		///< 게임서버재접속 시도 횟수.
	XCrypto m_CryptoObj;
	void Init() {
		m_bReconnect = FALSE;
		m_numRetryConnectGameSvr = 0;
	}
	void Destroy() {}
public:
	XSockGameSvr(XNetworkDelegate *pDelegate );
	virtual ~XSockGameSvr() { Destroy(); }
	//
	GET_SET_ACCESSOR(BOOL, bReconnect);
	BOOL ProcessPacket(DWORD idPacket, ID idKey, XPacket& p) override;
	/**
	각 리시브 패킷이 호출해야할 콜백함수를 등록함.
	클라의 요청없이 날아오는 패킷류를 이것으로 등록한다.
	*/
	template<typename T>
	void AddRecv(ID idPacket, T func) {
		typedef void (XSOCKET::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		xCALLBACK c;
		c.idPacket = idPacket;
		c.pCallback = static_cast<CALLBACK_FUNC>(func);
		m_mapCallback[idPacket] = c;
	}
	/**
	Request에 응답할 Response 콜백함수들을 등록한다.
	콜백함수를 초기화함수에서 미리 등록하지 않는이유는 보내고 받는 함수를
	한자리에 몰아놓음으로써 작업자가 따라기기 쉽게 하기 위해서다.
	AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
private:
	template<typename T>
	ID _AddResponse(ID idKey, ID idPacket, T func, XWnd *pTimeoutCallback, float secTimeout) {
		typedef void (XSOCKET::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		auto itor = m_mapCallback.find(idPacket);
		if (itor == m_mapCallback.end())	{
			// 찾아보고 없으면 등록시키고 있으면 재등록 시키지 않는다.
			xCALLBACK c;
			c.idPacket = idPacket;
			if (pTimeoutCallback)
				c.idCallback = pTimeoutCallback->getid();	// << 뭐에 쓰는거지?
			c.pCallback = static_cast<CALLBACK_FUNC>(func);
			m_mapCallback[idPacket] = c;
		}
		if (pTimeoutCallback)	{
			//			XTimeoutMng::sGet()->AddByKey( pTimeoutCallback->GetID(), 
			XTimeoutMng::sGet()->AddByKey(GAME->getid(),
				idKey,
				idPacket,
				TRUE,		// 응답올때까지 입력 blocking
				secTimeout );		// 타임아웃 시간
		}

		return idKey;
	}
public:
	template<typename T>
	ID AddResponse(ID idPacket, T func, XWnd *pTimeoutCallback, float secTimeout = 10.f) {
		ID idKey = XE::GenerateID();
		_AddResponse(idKey, idPacket, func, pTimeoutCallback, secTimeout);
		return idKey;
	}
	template<typename T>
	ID AddResponse(ID idKey, ID idPacket, T func, XWnd *pTimeoutCallback, float secTimeout = 10.f ) {
		XBREAK(pTimeoutCallback == nullptr);	// idKey와 함게 호출되는건 타임아웃객체가 있어야 한다.
		return _AddResponse(idKey, idPacket, func, pTimeoutCallback, secTimeout );
	}
	// 요청/응답류 send함수. 보낸 패킷아이디를 식별자로 타임아웃이 처리된다.
	/**
	AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
	template<typename T>
	BOOL SendRequest(XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout) {
		ID idKey = XE::GenerateID();
		return SendRequest(idKey, ar, func, pTimeoutCallback, idTextTimeout);
	}
	template<typename T>
	BOOL SendRequest(ID idKey, XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout) {
		typedef int (XSOCKET::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		_XCHECK_CONNECT(idTextTimeout);	// 커넥션이 끊어져있다면 알림창을 띄우게 하고 FALSE를 리턴한다.
		DWORD idPacket = ar.GetPacketHeader();
		AddResponse(idKey, idPacket, func, pTimeoutCallback, 10.f );
		Send(ar);
		return TRUE;
	}
	BOOL CallRecvCallback(ID idPacket, XPacket& p);

	//
	// Cheat
	//
#ifdef _CHEAT
	BOOL SendCheat(XWnd *pTimeoutCallback, int type, DWORD param1 = 0, DWORD param2 = 0, DWORD param3 = 0, DWORD param4 = 0, const _tstring& strParam = _tstring() );
	BOOL SendReqCheatCreateItem(XWnd *pTimeoutCallback, XPropItem::xPROP *pProp, int num);
	void RecvCheatCreateItem(XPacket& p, const xCALLBACK& c);
	//	BOOL SendCheatCreateItem( XWnd *pTimeoutCallback, XPropItem::xPROP *pProp, int num );
#endif

	//
	//	BOOL SendRequest( XPacket& ar, DWORD idTextTimeout=0 );
	BOOL SendNormal(XPacket& ar, ID idTextDisconnect = 0);
	BOOL SendPacketHeaderOnly( /*XWnd *pTimeoutCallback,*/ xtPacket idPacket/*, BOOL bTimeout=TRUE*/);
	void SendRequestPacketHeaderOnly(XWnd *pCallback, xtPacket idPacket);
	//
//	void SendRequestAccount(ID idAccount);
	BOOL SendReqLogin(XWnd *pCallback, ID idAccount);
	void RecvSuccessLogin(XPacket& p, const xCALLBACK& c);
	void RecvNoAccount(XPacket& p, const xCALLBACK& c) {}
	void RecvNewAccount(XPacket& p, const xCALLBACK& c) {}
	//
	void RecvMsgBox(XPacket& p, const xCALLBACK& c);
	void RecvInvalidVerify(XPacket& p, const xCALLBACK& c);
	void RecvDuplicateLogout(XPacket& ar, const xCALLBACK& c);
	void RecvReconnectTry( XPacket& ar, const xCALLBACK& c );
#ifdef _NEW_INAPP
	BOOL SendBuyCashItemByIAP( XWnd *pTimeoutCallback, const std::string& strcJsonReceipt, const std::string& strcSignature, const std::string& strcIdsProduct );
#else
	BOOL SendBuyCashItemByIAP( XWnd *pTimeoutCallback, XGAME::xtPlatform platform, LPCTSTR szJson, LPCTSTR szIdentifier, LPCTSTR szToken, LPCTSTR szOrderId, LPCTSTR szTransactDate );
#endif // not _NEW_INAPP
	void RecvBuyCashItemByIAP( XPacket& p, const xCALLBACK& c );
	
	BOOL SendReqReconSpot( XWnd *pTimeoutCallback, ID idSpot );
//	void RecvSpotRecon( XPacket& p, const xCALLBACK& c );

	BOOL SendReqSpotAttack( XWnd *pTimeoutCallback, ID idSpot, int idxStage = 0, int idxFloor = 0 );
	void RecvSpotInfo( XPacket& p, const xCALLBACK& c );
//  void RecvSpotInfoBattle( XPacket& p, XSpot *pBaseSpot, ID snSession, ID idAccEnemy );
	void ProcSpotInfoBattle( const XGAME::xBattleStartInfo& info, XSpot *pBaseSpot, XArchive& arParam );
//	void RecvBattleInfo( XPacket& p, const xCALLBACK& c );
	BOOL SendReqFinishBattle( XWnd *pTimeoutCallback, XGAME::xBattleFinish& battle );
//	BOOL SendReqFinishBattle( XWnd *pTimeoutCallback, ID idSpot, XGAME::xtExitBattle ebCode, XArchive& arParam, bool bCheat = false );
	void RecvFinishBattle( XPacket& p, const xCALLBACK& c );
	void RecvAttackedSpotResult( XPacket& p, const xCALLBACK& c );
	void RecvAttackedCastle( XPacket& p, const xCALLBACK& c );
	void RecvJewelMatchResult( XPacket& p, const xCALLBACK& c );
	void RecvJewelBattleInfo( XPacket& p, const xCALLBACK& c );
	void RecvJewelReconResult( XPacket& p, const xCALLBACK& c );
	void RecvJewelAttackInfo( XPacket& p, const xCALLBACK& c );
	void RecvSulfurSpawn( XPacket& p, const xCALLBACK& c );
	BOOL SendReqSpotCollect( XWnd *pTimeoutCallback, ID idSpot );
	void RecvSpotCollect( XPacket& p, const xCALLBACK& c );
	void RecvSpotSync( XPacket& p, const xCALLBACK& c );
	void RecvLevelSync( XPacket& p, const xCALLBACK& c );
	void RecvMandrakeMatchResult( XPacket& p, const xCALLBACK& c );
	void RecvMandrakeLegionResult( XPacket& p, const xCALLBACK& c );
	void RecvNpcSpawn( XPacket& p, const xCALLBACK& c );
//	void RecvUpdateSpecialSpot( XPacket& p, const xCALLBACK& c );
	BOOL SendReqOpenCloud( XWnd *pTimeoutCallback, ID idCloud, XGAME::xtTermsPayment termsPay );
	void RecvOpenCloud( XPacket& p, const xCALLBACK& c );
// 	BOOL SendReqAddHeroExp(XWnd *pTimeoutCallback, ID snHero, XArrayN<XBaseItem*,5>& ary, ID idWnd );
// 	void RecvAddHeroExp( XPacket& p, const xCALLBACK& c );
	BOOL SendReqChangeHeroLegion(XWnd *pTimeoutCallback, ID idHero, XGAME::xtUnit unit, ID idWnd);
	void RecvChangeHeroLegion(XPacket& p, const xCALLBACK& c);
	BOOL SendReqSummonHero(XWnd *pTimeoutCallback, XGAME::xtGatha typeSummon);
	void RecvSummonHero(XPacket& p, const xCALLBACK& c);
	BOOL SendReqNewSquad(XWnd *pTimeoutCallback, ID snHero, int idxSlot, int idxLegion);
	void RecvNewSquad(XPacket& p, const xCALLBACK& c);
	BOOL SendReqMoveSquad(XWnd *pTimeoutCallback, int idxSrc, int idxDst, int idxLegion, ID snHeroSrc, ID snHeroDst);
	void RecvMoveSquad(XPacket& p, const xCALLBACK& c);

//	BOOL SendReqChangeScalpToBook(XWnd *pTimeoutCallback, XGAME::xtClan clan);
	void RecvChangeScalpToBook(XPacket& p, const xCALLBACK& c);
	// 	BOOL SendReqUpgradeSquad( XWnd *pTimeoutCallback, ID snHero );
	// 	void RecvUpgradeSquad( XPacket& p, const xCALLBACK& c );
	BOOL SendReqReleaseHero(XWnd *pTimeoutCallback, ID idHero);
	void RecvReleaseHero(XPacket& p, const xCALLBACK& c);
	void RecvAddItem(XPacket& p, const xCALLBACK& c);
	// 	BOOL SendReqLevelUpSkill( XWnd *pTimeoutCallback, ID snHero, int typeSkill );
	// 	void RecvLevelUpSkill( XPacket& p, const xCALLBACK& c );
	BOOL SendReqAbilLockFree(XWnd *pTimeoutCallback, ID snHero, XGAME::xtUnit unit, ID idAbil);
	void RecvAbilLockFree(XPacket& p, const xCALLBACK& c);
	BOOL SendReqResearchComplete(XWnd *pTimeoutCallback, ID snHero, ID idResearchAbil);
	void RecvResearchComplete(XPacket& p, const xCALLBACK& c);

	// 창고
	BOOL SendItemSell(XWnd *pTimeoutCallback, ID snItem);
	void RecvItemSell(XPacket& p, const xCALLBACK& c);
	BOOL SendItemSpent(XWnd *pTimeoutCallback, ID snItem);
	void RecvItemSpent(XPacket& p, const xCALLBACK& c);
	BOOL SendInventoryExpand(XWnd *pTimeoutCallback, XGAME::xtSpentCall type);
	void RecvInventoryExpand(XPacket& p, const xCALLBACK& c);

	// 공통
	BOOL SendItemBuy( XWnd *pTimeoutCallback, ID snItem, XGAME::xtShopType shopType, XGAME::xtCoin coinType );
// 	BOOL SendItemBuy(XWnd *pTimeoutCallback, ID snItem, int shopType, int costType = 1);
	void RecvItemBuy(XPacket& p, const xCALLBACK& c);

	// 무기상
	//BOOL SendArmoryListTimeChange(XWnd *pTimeoutCallback);
	void RecvArmoryListTimeChange(XPacket& p, const xCALLBACK& c);
	BOOL SendArmoryListCashChange(XWnd *pTimeoutCallback, XGAME::xtSpentCall type);
	void RecvArmoryListCashChange(XPacket& p, const xCALLBACK& c);

	// 귀중품
	BOOL SendCashItemBuyAndroid(XWnd *pTimeoutCallback, XGlobalConst::cashItem* pCashItem);
	void RecvCashItemBuyAndroid(XPacket& p, const xCALLBACK& c);
// 	BOOL SendCashItemBuyIOS(XWnd *pTimeoutCallback, XGlobalConst::cashItem* pCashItem);
// 	void RecvCashItemBuyIOS(XPacket& p, const xCALLBACK& c);

	// ??
	BOOL SendReqEqiupItem(XWnd *pTimeoutCallback, ID snHero, ID snItem, ID typeAction);
	void RecvEqiupItem(XPacket& p, const xCALLBACK& c);
	void RecvQuestOccur(XPacket& p, const xCALLBACK& c);

	// 무역상
	BOOL SendReqTrade(XWnd *pTimeoutCallback, int wood, int iron, int jewel, int sulfur, int mandrake);
	void RecvTrade(XPacket& p, const xCALLBACK& c);
	//BOOL SendTimeTradeCall(XWnd *pTimeoutCallback);
//	void RecvTimeTradeCall(XPacket& p, const xCALLBACK& c);
	BOOL SendCashTradeCall(XWnd *pTimeoutCallback, XGAME::xtSpentCall type);
	void RecvCashTradeCall(XPacket& p, const xCALLBACK& c);

	// 우편함
	//BOOL SendPostAdd(XWnd *pTimeoutCallback);
	void RecvPostAdd(XPacket& p, const xCALLBACK& c);
	BOOL SendPostInfo(XWnd *pTimeoutCallback);
	void RecvPostInfo(XPacket& p, const xCALLBACK& c);
	//BOOL SendPostRead(XWnd *pTimeoutCallback, ID postID);
	//void RecvPostRead(XPacket& p, const xCALLBACK& c);
	BOOL SendPostGetItemAll(XWnd *pTimeoutCallback, ID postID);
	void RecvPostGetItemAll(XPacket& p, const xCALLBACK& c);

	BOOL SendPostTest(XWnd *pTimeoutCallback);		// 우편물 테스트

	// 퀘스트
	void RecvQuestComplete( XPacket& p, const xCALLBACK& c );
	void RecvQuestUpdate( XPacket& p, const xCALLBACK& c );
	BOOL SendReqReqQuestReward( XWnd *pTimeoutCallback, ID idQuest );
	void RecvReqQuestReward( XPacket& p, const xCALLBACK& c );
	BOOL SendSpotTouch( ID idSpot );
	void RecvSpotTouch( XPacket& p, const xCALLBACK& c );
	void RecvItemsList( XPacket& p, const xCALLBACK& c );
	BOOL SendReqClickStageInCampaign( XWnd *pTimeoutCallback, ID idSpot, ID snSpot, int idxStage, int idxFloor, ID idPropStage, ID idWndPopup );
	void RecvClickStageInCampaign( XPacket& p, const xCALLBACK& c );
//	void RecvOccurEncounter( XPacket& p, const xCALLBACK& c );
	BOOL SendReqResearch( XWnd *pTimeoutCallback, ID snHero, XGAME::xtUnit unit, ID idNode, bool bByRemainPoint );
	void RecvResearch( XPacket& p, const xCALLBACK& c );
	void RecvItemItemsSync( XPacket& p, const xCALLBACK& c );

	// 랭킹
	BOOL SendReqRankingList(XWnd *pTimeoutCallback);
	void RecvRankingList(XPacket& p, const xCALLBACK& c);
	//BOOL SendOptionSound( void );
	//BOOL SendOptionInvite( void );

	// 군단
	BOOL SendReqChangeSquad(XWnd *pTimeoutCallback, int idx, XLegion *pLegion);
	void RecvChangeSquad(XPacket& p, const xCALLBACK& c);
	BOOL SendReqRegistName(XWnd *pTimeoutCallback, _tstring strID, _tstring strPassword);
	void RecvRegistID(XPacket& p, const xCALLBACK& c);

	// 운영자 메세지
	void RecvSystemGameNotify(XPacket& p, const xCALLBACK& c);

	//
	BOOL SendReqRegenSpot(XWnd *pTimeoutCallback, ID idSpot);
	void RecvRegenSpot(XPacket& p, const xCALLBACK& c);
	BOOL SendReqReMatchSpot(XWnd *pTimeoutCallback, ID idSpot);
	void RecvReMatchSpot(XPacket& p, const xCALLBACK& c);
	BOOL SendReqSulfurRetreat(XWnd *pTimeoutCallback, ID idSpot);
	void RecvSulfurRetreat(XPacket& p, const xCALLBACK& c);
	void RecvOccurBuff(XPacket& p, const xCALLBACK& c);
	void RecvFinishBuff(XPacket& p, const xCALLBACK& c);
	BOOL SendReqBattleStart(XWnd *pTimeoutCallback);
	void RecvBattleStart(XPacket& p, const xCALLBACK& c);
	void RecvAddBattleLog(XPacket& p, const xCALLBACK& c);
	BOOL SendReqWithdrawMandrake(XWnd *pTimeoutCallback, ID idSpot);
	void RecvWithdrawMandrake(XPacket& p, const xCALLBACK& c);
	BOOL SendReqCheckAPTimeOver(XWnd *pTimeoutCallback);
	void RecvCheckAPTimeOver(XPacket& p, const xCALLBACK& c);
// 	BOOL SendReqProvideBooty(XWnd *pTimeoutCallback, ID snHero, XGAME::xtTrain type, int num);
// 	void RecvProvideBooty(XPacket& p, const xCALLBACK& c);
	BOOL SendReqPromotionHero( XWnd *pTimeoutCallback, XHero *pHero );
	void RecvPromotionHero( XPacket& p, const xCALLBACK& c );
	BOOL SendReqSummonHeroByPiece( XWnd *pTimeoutCallback, XPropHero::xPROP* pPropHero );
	void RecvSummonHeroByPiece( XPacket& p, const xCALLBACK& c );
	BOOL SendReqEndSeq( XWnd *pTimeoutCallback, const std::string& idsSeq );
	void RecvEndSeq( XPacket& p, const xCALLBACK& c );
	void RecvUnlockMenu( XPacket& p, const xCALLBACK& c );
	BOOL SendUIAction( XGAME::xtUIAction uiAction, DWORD dwParam2 );
	// xuzhu end
	//// 닉네임
	//BOOL SendReqChangeNickName(XWnd *pTimeoutCallback, _tstring strName);
	//void RecvChangeNickName(XPacket& p, const xCALLBACK& c);

	// 길드
	BOOL SendReqGuildInfo(XWnd *pTimeoutCallback);	//길드 정보 요청
	void RecvGuildInfo(XPacket& p, const xCALLBACK& c);
	BOOL SendReqCreateGuild(XWnd *pTimeoutCallback, _tstring strName, _tstring strDesc);	//길드 설립 요청
	void RecvCreateGuild(XPacket& p, const xCALLBACK& c);
	BOOL SendReqJoinGuild(XWnd *pTimeoutCallback, ID idIndex);	//길드 가입 요청
	void RecvJoinGuild(XPacket& p, const xCALLBACK& c);	//길드 정보 갱신
//	void RecvUpdateGuild(XPacket& p, const xCALLBACK& c);
	bool SendReqJoinGuildAccept(XWnd *pTimeoutCallback, ID idAcc, ID idFlag);	//길드 가입 수락/거부
	void RecvReqJoinGuildAccept(XPacket& p, const xCALLBACK& c);
	void RecvJoinResult(XPacket& p, const xCALLBACK& c);	//가입 신청한 길드의 결과
//	void RecvKickGuild(XPacket& p, const xCALLBACK& c); //추방 당했을때
	//탈퇴
	bool SendReqOutGuild(XWnd *pTimeoutCallback);
	void RecvReqOutGuild(XPacket& p, const xCALLBACK& c);
	//추방
	bool SendReqKickGuild(XWnd *pTimeoutCallback, ID idAcc);
	void RecvReqKickGuild(XPacket& p, const xCALLBACK& c);
	//옵션, 설명, 등급 변경
	bool SendReqChangeDesc(XWnd *pTimeoutCallback, _tstring strDesc);
	void RecvReqChangeDesc(XPacket& p, const xCALLBACK& c);
	bool SendReqChangeOption(XWnd *pTimeoutCallback, BOOL bAutoAccept, BOOL bBlockJoin);
	void RecvReqChangeOption(XPacket& p, const xCALLBACK& c);
	bool SendReqChangeMemberGrade(XWnd* pTimeoutCallback, ID idAccount, ID idAccLevel, _tstring strName, XGAME::xtGuildGrade eGrade);
	void RecvChangeMemberGrade(XPacket& p, const xCALLBACK& c);
	//
	void RecvCreateHero(XPacket& p, const xCALLBACK& c);
	BOOL SendReqResearchCompleteNow(XWnd *pTimeoutCallback, XHero *pHero, ID idAbil);
	void RecvSyncBaseInfo(XPacket& p, const xCALLBACK& c);
// 	BOOL SendReqTrainHero(XWnd *pTimeoutCallback, XHero *pHero, XGAME::xtTrain type);
// 	void RecvTrainHero(XPacket& p, const xCALLBACK& c);
	BOOL SendReqCheckTrainComplete(XWnd *pTimeoutCallback, ID snSlot);
	void RecvCheckTrainComplete(XPacket& p, const xCALLBACK& c);
	BOOL SendReqTrainCompleteTouch(XWnd *pTimeoutCallback, XGAME::xtTrain typeTrain, XHero *pHero);
	void RecvTrainCompleteTouch(XPacket& p, const xCALLBACK& c);
	BOOL SendAccepLevelUp();
	void RecvSyncTrainSlot(XPacket& p, const xCALLBACK& c);
	BOOL SendFlagTutorial();
// 	BOOL SendReqLevelupConfirm( XWnd *pTimeoutCallback, XGAME::xtTrain typeTrain, XHero *pHero );
// 	void RecvLevelupConfirm( XPacket& p, const xCALLBACK& c );
	void RecvIsOnline( XPacket& p, const xCALLBACK& c );
	void RecvUnlockLoginForBattle( XPacket& p, const xCALLBACK& c );
	void RecvReqCampaignByGuildRaid( XPacket& p, const xCALLBACK& c );
	BOOL SendReqSendReqGuildRaidOpen( XWnd *pTimeoutCallback, ID idSpot );
	void RecvSendReqGuildRaidOpen( XPacket& p, const xCALLBACK& c );
// 	void RecvResultEnterGuildRaid( XPacket& p, const xCALLBACK& c );
	//계정 연동
	//	BOOL SendReqLoginIDPW(XWnd *pTimeoutCallback, const _tstring& strID, const _tstring& strPW);
//	void RecvLoginIDPW(XPacket& p, const xCALLBACK& c);
	BOOL SendReqUnlockUnit( XWnd *pTimeoutCallback, XGAME::xtUnit unit );
	void RecvLockLoginForBattle( XPacket& p, const xCALLBACK& c );
	void RecvUnlockUnit( XPacket& p, const xCALLBACK& c );
	BOOL SendReqFillAP( XWnd *pTimeoutCallback );
	void RecvFillAP( XPacket& p, const xCALLBACK& c );
	BOOL SendReqTrainCompleteQuick( XWnd *pTimeoutCallback, ID snSlot, XGAME::xtTrain typeTrain, ID snHero, bool bCash );
	BOOL SendReqBuyGuildItem( XWnd *pTimeoutCallback, int idxSlot );
	void RecvBuyGuildItem( XPacket& p, const xCALLBACK& c );
	BOOL SendReqCampaignReward( XWnd *pTimeoutCallback, ID idSpot, ID idCamp, int idxStage );
	void RecvCampaignReward( XPacket& p, const xCALLBACK& c );
	void RecvSyncResource( XPacket& p, const xCALLBACK& c );
	BOOL SendReqTrainHeroByGold( XWnd *pTimeoutCallback, XHero *pHero, int gold, XGAME::xtTrain typeTrain );
	void RecvTrainHeroByGold( XPacket& p, const xCALLBACK& c );
	BOOL SendReqClickFogSquad( XWnd *pTimeoutCallback, ID idSpot, ID snHero, int idxStage );
	void RecvSendReqClickFogSquad( XPacket& p, const xCALLBACK& c );
	BOOL SendRegistPushMsg(XWnd *pTimeoutCallback, int idacc, int type1, int type2, const _tstring& msg, int time);
	BOOL SendUnregistPushMsg(XWnd *pTimeoutCallback, int idacc, int type1, int type2);
	BOOL SendCheckUnlockUnit( XGAME::xtUnit unit );
	BOOL SendTouchSquadInReadyScene( int idxSquad );
	BOOL SendControlSquadInBattle( const XHero *pHero );
	BOOL SendCheckEncounterLog( ID idSpot );
	BOOL SendGotoHome( int secTimeout );
	BOOL SendReqRegisterFacebook( XWnd *pTimeoutCallback, const std::string& strcFbUserId, const std::string& strFbUsername );
	BOOL SendReqChangeHelloMsg( XWnd *pTimeoutCallback, const _tstring& strHello );
	BOOL SendReqInitAbil( XWnd *pTimeoutCallback, ID snHero );
	BOOL SendReqUnlockTrainingSlot( XWnd *pTimeoutCallback );
	BOOL SendReqGCMResistId( XWnd *pTimeoutCallback, const std::string& strRegid );
	BOOL SendReqCreatePayload( XWnd *pTimeoutCallback, const _tstring& idsProduct );
	BOOL SendReqSync( XWnd *pTimeoutCallback, XGAME::xtParamSync type, int param = 0 );
	//	BOOL SendReqAllGuildInfo( XWnd *pTimeoutCallback );
	BOOL SendReqPaymentAssetByGem( XWnd *pTimeoutCallback, XGAME::xtPaymentRes typeAsset, bool bByItem );
	BOOL SendReqPrivateRaidEnterList( XWnd *pTimeoutCallback, const XList4<XHero*>& listHero, ID idSpot );
private:
	void RecvAttackedSimulByHome( XPacket& p, const xCALLBACK& c );
	void cbOnDisconnectByExternal() override;
	void RecvRegisterFacebook( XPacket& p, const xCALLBACK& c );
	void RecvSyncHello( XPacket& p, const xCALLBACK& c );
	void RecvChangeHelloMsg( XPacket& p, const xCALLBACK& c );
	void RecvInitAbil( XPacket& p, const xCALLBACK& c );
	void RecvUnlockTrainingSlot( XPacket& p, const xCALLBACK& c );
	void RecvGCMResistId( XPacket& p, const xCALLBACK& c );
	void RecvCreatePayload( XPacket& p, const xCALLBACK& c );
	void RecvPublicKey( XPacket& p, const xCALLBACK& c );
//	void RecvAllGuildInfo( XPacket& p, const xCALLBACK& c );
	void DelegateGuildAccept( XGuild* pGuild, const xnGuild::xMember& memberJoin ) override;
	void DelegateGuildReject( XGuild* pGuild, const xnGuild::xMember& memberReject ) override;
	void DelegateGuildChangeMemberInfo( XGuild* pGuild, const xnGuild::xMember& memberTarget, const xnGuild::xMember& memberConfirm ) override;
	void DelegateGuildKick( XGuild* pGuild, const xnGuild::xMember& memberKick ) override;
	void DelegateGuildOut( XGuild* pGuild, const xnGuild::xMember& memberOut ) override;
	void DelegateGuildUpdate( XGuild* pGuild, const xnGuild::xMember& member ) override;
	void RecvGuildEvent( XPacket& p, const xCALLBACK& c );
	void RecvProp( XPacket& ar, const xCALLBACK& c );
	void RecvCancelKill( XPacket& p, const xCALLBACK& c );
	void RecvSyncAcc( XPacket& p, const xCALLBACK& c );
	void RecvPaymentAssetByGem( XPacket& p, const xCALLBACK& c );
	void RecvPrivateRaidEnterList( XPacket& p, const xCALLBACK& c );
};

extern XSockGameSvr *GAMESVR_SOCKET;			// 월드서버와의 커넥션
