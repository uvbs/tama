#pragma once
#include "XFramework/server/XEUser.h"
#include "server/XSAccount.h"
#include "XSpot.h"
#include "XPropCloud.h"
#include "XQuestMng.h"
#include "XPacketCG.h"
#include "XStruct.h"
#include "XPool.h"
#include "XFramework/Game/XFLevelH.h"
#include "XDefNetwork.h"

class XSAccount;

class XGameUser : public XEUser, public XDelegateSpot, 
								 public XDelegateLevel,
								 public XDelegateQuestMng,
								 public XDelegateBuff,
								public XMemPool<XGameUser>
{
public:
private:
private:
	XSPSAcc m_spAcc;
//	BOOL m_bDestroySave;
	CTimer m_timer1Min;		// 1분 타이머	
	XGAME::xtPlatform m_Platform = XGAME::xPL_NONE;
	std::string m_strKeyAds;			// 광고비디오 시청 식별키
	CTimer m_timerAds;			// 광고비디오 시청 요청 타이머
	//CTimer m_messagetimer2min;
	/*
	 전투상대 부대정보
	 세션이 끊길때(접속이 끊기는거 말고) m_pEnemyLegion이 남아있다는것은
	 전투중이었다는것을 의미한다. 이때는 플레이어 패배로 처리하고 m_pEnemyLegion
	 은 삭제시킨다. 이정보는 DB에 저장하지 않는다.
	*/
//	XLegion *m_pEnemyLegion;

	void Init() {
//		m_pEnemyLegion = NULL;
		m_spAcc = nullptr;
//		m_bDestroySave = FALSE;
	}
	void Destroy();
public:
	XGameUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect );
	virtual ~XGameUser() { Destroy(); } 

	void SuccessLoginBeforeSend( XSPSAcc spAcc, BOOL bReconnect = FALSE );
	void SuccessLoginAfterSend();	
	//
	GET_SET_ACCESSOR( XSPSAcc, spAcc );
	GET_SET_ACCESSOR_CONST( XGAME::xtPlatform, Platform );
	inline ID GetidAcc() const {
		if( !m_spAcc )
			return 0;
		return m_spAcc->GetidAccount();
	}
	inline ID GetidGuild() {
		return m_spAcc->GetidGuild();
	}
	inline LPCTSTR GetstrName( void ) {
		if( !m_spAcc )
			return _T("");
		return m_spAcc->GetstrName();
	}
	inline int GetLevel( void ) {
		if( !m_spAcc )
			return 0;
		return m_spAcc->GetLevel();
	}
	inline int GetScore( void ) {
		return m_spAcc->GetLadder();
	}
	inline XWorld* GetpWorld( void ) {
		return m_spAcc->GetpWorld();
	}
	inline XQuestMng* GetpQuestMng( void ) {
		return m_spAcc->GetpQuestMng();
	}
	void Send( XPacket& ar );
	void SendDuplicateConnect();

	void OnDestroy() override;
	virtual BOOL Save( void ) override;
  void OnFireConnectBefore();
	void Process( float dt ) override;
	void CalculateResource( void );
	BOOL DoBattle( ID idSpot, 
					ID idAccount, 
					int level, 
					LPCTSTR szName, 
					XLegion *pLegion );
//	XLegion* CreateNpcLegionByUserLevel( void );
	void CreateSpotsByCloud( XPropCloud::xCloud *pProp, BOOL bSyncClient/*, XArrayLinearN<XSpot*, 1024> *pOutAry = nullptr*/ );
	void DoDropProcess( const XSpot *pSpot, 
						XArrayLinearN<XBaseItem*,256> *pOutUpdatedAry,
						XArrayLinearN<ItemBox, 256>* pOutDrops );
	ItemBox DoDropScalp( void );
	ItemBox DoDropMedal( void );
	ItemBox DoDropScroll( void );
	BIT UnlockMenu( XPropCloud::xCloud *pArea, int lvArea );
	//
//	void DoBattleResult( BOOL bWin, ID idSpot, ID idEnemyAccount, LPCTSTR szName );
	void DelegateOnSpawnTimer( XSpot *pSpot, xSec secAdjustOffline ) override;
	void DelegateReleaseSpotBefore( XSpot *pSpot ) override;
	void DelegateReleaseSpotAfter( XSpot *pSpot ) override;
	DWORD OnDelegateGetMaxExp( const XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) const override;
	int OnDelegateGetMaxLevel( const XFLevel *pLevel, DWORD param1, DWORD param2 ) const override;
	void OnDelegateLevelUp( XFLevel *pLevel ) override;
	//
	int RecvSpotRecon( XPacket& p );
	int RecvSpotAttack( XPacket& p );
//	int RecvReqFinishBattle( XPacket& p );
	int RecvReqFinishBattle2( XPacket& p );
	int RecvSpotCollect( XPacket& p );
	void RecvGetUserLegionByScore( XPacket& p );
	void RecvGetUserLegionByIdAcc( XPacket& p );
//	void RecvGetWoodIron( XPacket& p );
	void RecvGetUserByPower( XPacket& p );
	void RecvReqMatchJewelMine( XPacket& p );
	void RecvGetJewelLegionByIdAcc( XPacket& p );

	int RecvItemSellList(XPacket& p);				//아이템 상점 판매 목록
	int RecvBuyItem(XPacket& p);					//아이템 구매
	int	RecvSellItem( XPacket& p ); 					///< 아이템 상점에 판매
	int RecvItemSpent(XPacket& p);					///< 아이템 사용
	int RecvInventoryExpand(XPacket& p);			///< 인벤토리 확장

// 	int RecvCashItemBuyApple(XPacket& p);			//애플 인앱 결제
	int RecvBuyCashtemIAP(XPacket& p);			//구글 인앱 결제
	int RecvCashItemBuyPayload(XPacket& p);			//구글 payload 발급

	void cbCashItemBuyApple(XPacket& p);			//애플 인앱 결제 Callback
	void cbCashItemBuyGoogle(XPacket& p);			//구글 인앱 결제 Callback
	void cbCashItemBuyPayload(XPacket& p);			//구글 payload 발급
	void cbDeletePayload( XPacket& ar );

	//POST(우편함)
	void RecvPostInfoRequest(XPacket& p);
	void RecvPostReadRequest(XPacket& p);
	void RecvPostItemGetRequest(XPacket& p);
	void RecvPostItemGetRequestAll(XPacket& p);
	void RecvPostDeleteRequest(XPacket& p);
	void RecvAddPost(XPacket& p);
//	void RecvAccountPostUpdateResult(XPacket& p);

	void SendSyncPosts();	
	void DoSubscribeAsyncRecursive( XGAME::xtPlatform platform );
	void DoAddPostInfoVec(XGAME::xtPostType e, std::vector<XPostInfo::sPostItem>& Vec);
	void DoAddPostInforewardtable(XGAME::xtPostType e, ID idrewardtable );
//	void DoAddPostInfo(_tstring strSenderName, _tstring strTitle, _tstring strMessage, std::vector<XPostInfo::sPostItem>& Vec);
//	void cbDoSubscribeAsync(XPacket& p);	
		
	void cbDoAddPostInfo(XPacket& p);	
//	void cbDoAddPostInfoVec(XPacket& p);
//	void cbDoAddPostInforewardtable(XPacket& p);

	void cbSendPostInfoRequest(XPacket& p);
	void cbSendUpdateLogin(XPacket& p);
//	void OnRecvFinishBattleOnCastle( ID idSpot, ID idEnemy, int numWood, int numIron );
	void OnRecvJewelBattleInfo( BOOL isFound, XArchive& arPacket );
//	void SendBattleInfo( ID idPacket, ID idSpot, ID idAcc, XLegion *pLegion, XArchive *pParam=NULL );
	void SendReconInfo( bool bRecon/*, ID snSession*/, ID idAccEnemy, XSpot *pSpot, XArchive *pParam = nullptr);
	void SendBattleInfo( XSpot *pSpot, XGAME::xBattleStartInfo *pInfo );
	void SendBattleInfoWithidAcc( XSpot *pSpot, ID idAcc, const _tstring& strName );
	inline void SendBattleInfoWithidAcc( XSpot *pSpot, ID idAcc ) {
		SendBattleInfoWithidAcc( pSpot, idAcc, _tstring() );
	}
	void SendBattleInfoWithidAccParam( XSpot *pSpot, ID idAcc, XArchive* pArParam = nullptr );
	void SendBattleInfoByGuildRaid( XSpot *pBaseSpot, int idxStage, int levelLegion, XGAME::xtGuildError err );
	void GetMatchRangePowerForCastle( XSpotCastle *pSpot, int *pOutMin, int *pOutMax );
	void GetMatchRangePowerByPlayerPower( int *pOutMin, int *pOutMax );
	void DoMatchUserSpot( XSpotCastle *pBaseSpot, xSec secAdjustOffline = 0 );
	void SendSpotSync( XSpot *pSpot, bool bDestroy = false );
	void SendLevelSync( void );
	void RecvReqMatchMandrake( XPacket& p );
	void RecvGetMandrakeLegionByIdAcc( XPacket& p );
//	void SendChangeMandrakeOwn( int idxMandrake, ID idAcc, ID idNewOwner, ID idEnemy, LPCTSTR szName, int win, int reward, int idxLegion, int addOffWin );
	int RecvOpenCloud( XPacket& p );
	void SendUnlockMenu( XGAME::xtMenus bitUnlock );
	int RecvAddHeroExp(XPacket& p);
	int RecvChageHeroLegion(XPacket& p);
	int RecvChageHeroEquip(XPacket& p);
	int RecvSummonHero( XPacket& p );
	int RecvNewSquad( XPacket& p );
	int RecvMoveSquad( XPacket& p );
	int RecvChangeSquad(XPacket& p);
	int RecvCheat( XPacket& p );
	void UpdateOccurQuestCheat();
	int RecvTrade( XPacket& p );	
	int RecvChangeScalpToBook( XPacket& p );
	int RecvCheatCreateItem( XPacket& p );
// 	int RecvUpgradeSquad( XPacket& p );
	int RecvReleaseHero( XPacket& p );
	void SendAddItem( XBaseItem *pItem );
// 	int RecvLevelUpSkill( XPacket& p );
	void DispatchQuestEvent( XGAME::xtQuestCond eventQuest, const xQuest::XEventInfo& infoQuest );
	void DispatchQuestEvent( XGAME::xtQuestCond eventQuest );
	void DispatchQuestEvent( XGAME::xtQuestCond eventQuest, DWORD dwParam, DWORD dwParam2 = 0 );
	void DispatchQuestEvent( XGAME::xtQuestCond eventQuest, const _tstring& strParam );
	// quest delegate
	void DelegateOccurQuest( XQuestObj *pQuestObj ) override;
	void DelegateOccurQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond ) override;
	void DelegateCompleteQuest( XQuestObj *pQuestObj ) override;
	void DelegateUpdateQuest( XQuestObj *pQuestObj ) override;
	void DelegateDropItemFromQuest( XQuestObj *pQuestObj, ID idItem, XArrayLinearN<ItemBox,256> *pOut ) override;
	void DelegateDestroyQuest( XQuestObj *pQuestObj ) override;
	void DelegateDestroyQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond ) override;
	void DelegateFinishQuest( XQuestObj *pQuestObj, bool bItemSync ) override;
	int RecvReqQuestReward( XPacket& p );
	int RecvSpotTouch( XPacket& p );
	void SendItemsList( const XArrayLinearN<XBaseItem*, 256>& ary );
	int RecvClickStageInCampaign( XPacket& p );
	int DoSpotAttackCampaign( XSpot *pBaseSpot, int idxStage, int idxFloor );
	int DestroySpotByQuest( XQuestObj *pQuestObj );
	bool IsVerificationBattle( /*전투데이타*/ );
	int DispatchQuestEventByBattle( XSpot *pBaseSpot, bool bWin, bool bClearSpot, XSPLegion& spLegion );
	int DoRewardProcess( XSpot *pSpot, XGAME::xBattleResult *pOutResult );
	int DoRewardProcessForCastle( XSpotCastle *pSpot, XGAME::xBattleResult *pOut, XGAME::xBattleFinish& pBattle );
	int DoRewardProcessForJewel( XSpotJewel *pSpot, XGAME::xBattleResult *pOut );
	void ProcForMandrake( const XSpotMandrake *pSpot, XGAME::xBattleResult *pOut );
	int DoRewardProcessForNpc( XSpotNpc *pSpot, XGAME::xBattleResult *pOut );
	int DoRewardProcessForDaily( XSpotDaily *pSpot, XGAME::xBattleResult *pOut );
// 	int DoRewardProcessForSpecial( XSpotSpecial *pSpot, XGAME::xBattleResult *pOut );
	void DoMatchSulfur( XSpotSulfur *pSpot, const XGAME::xBattleResult& result );
	int DoRewardProcessForSulfur( XSpotSulfur *pSpot, XGAME::xBattleResult *pOutResult );
	int DoRewardResource( const XSpot *pBaseSpot, XGAME::xBattleResult *pOut );
	int DoRewardQuest( ID idQuestProp );
	int RecvResearch( XPacket& p );
	void SendItemInvenSync();
	void DoMatchMandrake( XSpotMandrake *pSpot );

	int RecvRankingInfo(XPacket& p);

	int RecvGuildCreate(XPacket& p);
	void cbGuildCreate(XPacket& p);

	int RecvGuildInfo(XPacket& p);
//	int RecvAllGuildInfo( XPacket& p );
	int RecvGuildUpdate(XPacket& p);
	int RecvGuildJoinReq(XPacket& p);
	void cbGuildJoinReq(XPacket& p);
	int RecvGuildJoinAccept(XPacket& p);
	void cbGuildJoinAccept(XPacket& p);
	int RecvGuildOut(XPacket& p);
	void cbGuildOut(XPacket& p);
	int RecvGuildKick(XPacket& p);
	void cbGuildKick(XPacket& p);

	int RecvGuildChangeOption(XPacket& p);
	int RecvGuildChangeContext(XPacket& p);
	int RecvGuildChangeMemberInfo(XPacket& p);
	void cbGuildChangeOption(XPacket& p);
	void cbGuildChangeContext(XPacket& p);
	void cbGuildChangeMember(XPacket& p);

	int RecvTradeCashCall(XPacket& p);
	int RecvShopCashCall(XPacket& p);

//	int RecvTradeTimeCall(XPacket& p);
	//int RecvShopCashCall(XPacket& p);
	
	int RecvAccountNickNameDuplicate(XPacket& p);
	int RecvAccountNameDuplicate(XPacket& p);
	int RecvAccountNameRegist(XPacket& p);
	int RecvAccountNameLogin(XPacket& p);
	int RecvAccountNickNameChange(XPacket& p);

//	void RecvTradeCashCallResult(XPacket& p);
//	void RecvShopCashCallResult(XPacket& p);

	void cbPostDelete(XPacket& p);

	void RecvAccountNickNameDuplicateResult(XPacket& p);
	void RecvAccountNameDuplicateResult(XPacket& p);
	void RecvAccountNameRegistResult(XPacket& p);
	void RecvAccountNameRegistCheckResult(XPacket& p);
	void RecvAccountNickNameChangeResult(XPacket& p);

	void SendTradeMerchant();
	void SendShoplistMerchant();
	int RecvAbilLockFree( XPacket& p );
	int RecvCheckResearchComplete( XPacket& p );
//	int SendResearchComplete( ID idPacket, xErrorCode codeError );
	int SendUpdateResearch( xErrorCode codeError, ID snHero, ID idAbil, int point );
	
	void DoAddPostNotify(XPacket& p);

	void AddPostSubscribe( XGAME::xtPlatform platform );
	void SendUserNotifyTest();
	void SendUserNotify(_tstring strMessage);
//	void SendGuildUpdate(XArchive& ar);
	//Templete Cheat Test Mode
	void Cheat_AddPostInfo();
	void Cheat_AddPostInfoGuildCoin();
	//
	int RecvRegenSpot( XPacket& p );
	int RecvReMatchSpot( XPacket& p );
	void DoMatchJewel( XSpotJewel *pSpot, bool bSimul = false, bool bSimulWin = false );
	int RecvSulfurRetreat( XPacket& p );
	void DelegateFinishBuff( XSPAcc spAcc, const XGAME::xBuff& buff ) override;
	void SendOccurBuff( XGAME::xBuff *pBuff );
	int RecvBattleStart( XPacket& p );
	int RecvUpdateSpotForBattle( XPacket& p );
	void SendAddBattleLog( bool bAttack, XGAME::xBattleLog& log );
	int RecvWithdrawMandrake( XPacket& p );
	void SendCreateHero( XSPHero pHero );
	int RecvResearchCompleteNow( XPacket& p );
	void SendBaseInfo( void );
//	XPropItem::xPROP* DoDropEquip();
	void LogAccount();
	void RecvLoginLockForBattle( XPacket& p );
	void RecvDefeatInfo( XPacket& p );
	void RecvReqCampaignByGuildRaid( XPacket& ar );
	void RecvReqEnterGuildRaid( XPacket& p );
	void RecvGuildRaidOpenFromDB( XPacket& p );
	int RecvSendReqGuildRaidOpen( XPacket& p );
	
	void AddLog(int logtype, _tstring strLog );
// 	int RecvTrainHero( XPacket& p );
// 	int RecvTrainHeroLevel( XSPHero pHero, int& time );
// 	int RecvTrainHeroSquad( XSPHero pHero, int& time );
// 	int RecvTrainHeroSkill( XSPHero pHero, XGAME::xtTrain type );
	int RecvCheckTrainComplete( XPacket& p );
	int SendTrainingComplete( ID snSlot, xErrorCode result, XGAME::xtQuickComplete typeComplete = XGAME::xQCT_NONE );
//	int RecvTrainCompleteTouch( XPacket& p );
	int RecvCheckAPTimeOver( XPacket& p );
	int RecvAccepLevelUp( XPacket& p );
	int SendSyncTrainSlot();
	int RecvFlagTutorial( XPacket& p );
	int RecvUnlockUnit( XPacket& p );
	int RecvSendReqFillAP( XPacket& p );
	int RecvTrainCompleteQuick( XPacket& p );
	int RecvProvideBooty( XPacket& p );
//	int RecvLevelupConfirm( XPacket& p );
	int ProcCancelBattle( XSpot *pBaseSpot, ID idEnemy );
	void ProcAfterBattleUserOwner( XSpot *pBaseSpot, ID idEnemy, int powerEnemy, bool bWin, XGAME::xBattleLog& logForDefender );
	int RecvPromotionHero( XPacket& p );
	int RecvSummonHeroByPiece( XPacket& p );
	int RecvEndSeq( XPacket& p );
	int RecvUIAction( XPacket& p );
	int RecvBuyGuildItem( XPacket& p );
	int RecvCampaignReward( XPacket& p );

	void RecvPushMsgRegist(XPacket& p);
	void RecvPushMsgUnRegist(XPacket& p);
	int RecvTrainHeroByGold( XPacket& p );
	int RecvClickFogSquad( XPacket& p );
	int RecvCheckUnlockUnit( XPacket& p );
	int RecvTouchSquadInReadyScene( XPacket& p );
	int RecvControlSquadInBattle( XPacket& p );
	int RecvCheckEncounterLog( XPacket& p );
	int RecvGotoHome( XPacket& p );
	int RecvRegisterFacebook( XPacket& p );
	int RecvChangeHelloMsg( XPacket& p );
	int RecvInitAbil( XPacket& p );
	int RecvUnlockTrainingSlot( XPacket& p );
	int RecvGCMResistId( XPacket& p );
	void SendGuildEvent( XGAME::xtGuildEvent event, XArchive& arParam );
	void DoSendPost( const XPostInfo* pPostInfo );
	int RecvSync( XPacket& p );
	int RecvPaymentAssetByGem( XPacket& p );
	int RecvPrivateRaidEnterList( XPacket& p );
	int RecvEnterReadyScene( XPacket& p );
	int RecvShowAdsVideo( XPacket& p );
	int RecvDidFinishShowAdsVideo( XPacket& p );
private:
	XCrypto* GetpCryptObj();
	int ProcSpotEach( XSpot *pBaseSpot, XGAME::xBattleResult *pOut, XGAME::xBattleFinish& battle );
	int ProcSpotEachSulfur( XSpotSulfur *pSpot, XGAME::xBattleResult *pOut, const XGAME::xBattleFinish& battle );
	int ProcForCampaign( XSpot *pBaseSpot, XGAME::xBattleResult *pOut, const XGAME::xBattleFinish& battle );
	void SendSyncResource( void );
	void SetResourceSquad( XSpot *pBaseSpot, XLegion *pLegion );
	void SetFogSquad( XLegion *pLegion, const std::vector<ID>& aryUnfogHeroesSN );
	void UpdateQuestConditionEach( XGAME::xtQuestCond eventQuest, XGAME::xtQuestCond eventExcute );
	void UpdateQuestCondition( XGAME::xtQuestCond eventQuest = XGAME::xQC_NONE );
	void cbRegisterFacebook( XPacket& p );
	void cbReqHelloMsgByidAcc( XPacket& p );
	void SendSyncSpotWithHello( XSpot *pSpot );
	int GetGradeLevelByMatch( XSpot *pBaseSpot );
	int GetGradeLevelByMatch2( XSpot *pBaseSpot );
	void UpdateSpotsByQuest();
	void UpdateSpotByQuest( XQuestObj *pQuestObj, XVector<XSpot*> *pOutAry = nullptr );
	void cbReqGCMRegistId( XPacket& p );
	void SendCashItemBuyGoogle( XGAME::xtErrorIAP errCode, const _tstring& idsProduct );
	void SendIAPPublicKey();
	XSPGameUser GetThis() {
		return std::static_pointer_cast<XGameUser>( GetThis() );
	}
	void SendCashItemBuyPayload( XGAME::xtErrorIAP err, XGAME::xtPlatform platform, XGAME::xtCashType typeProduct, const _tstring& strProductID, const _tstring& strPayloadKey );
	void SendCancelKill();
#ifdef _DEV
	void ProcessAttackedHome( xSec secPass );
	void ProcessAttackedJewel( xSec secOffline );
	void ProcessAttackedMandrake( xSec secOffline );
	void SimulAttackedHome( ID idAcc, int lvAcc, const _tstring& strName, const int power, int ladder );
// 	void SimulAttackedJewel( XSpotJewel *pSpot, bool bDefenseOk, const XGAME::xJewelMatchEnemy& info );
#endif // _DEV
	void DoCollectingSpotStar( int clearStar, XSpot* pBaseSpot );
	void SendResultOpenCloud( ID idCloud, XGAME::xtError err, XGAME::xtTermsPayment termsPay );
	XGAME::xtError IsAblePayment( int goldCostOpen, XGAME::xtError err, XGAME::xtTermsPayment termsPay, int* pOutGold, int* pOutCash );
	XGAME::xtError GetPaymentCost( int goldCost, XGAME::xtError err, XGAME::xtTermsPayment termsPay, int* pOutGold, int* pOutCash );
	void SendSyncAcc( XGAME::xtParamSync type, int param );
	XSpot* GetpSpot( ID idSpot );
	int ProcBuyItemAtArmory( ID idProp, XGAME::xtCoin costType );
	int ProcBuyItemAtCashShop( ID idProp );
};


