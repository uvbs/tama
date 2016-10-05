#pragma once

#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XPropCloud.h"
#include "XPropWorld.h"
#include "XParticleMng.h"
#include "XPropBgObjH.h"
#include "_Wnd2/XWndEdit.h"
#include "_Wnd2/XWndScrollView.h"

class XAccount;
class XLegion;
class XGame;
class XSpotCastle;
class XSpotJewel;
class XSpotSulfur;
class XSpotMandrake;
class XSpot;
class XWndCloud;
class XWndAlert;
class XQuestObj;
class XBaseTool;
class XWndCircleMenu;
class XWndPopupSpotMenu;
class XWndBgObjLayer;
class XWndSpot;
namespace xHelp {
	class XPropCamera;
	class XOrderCamera;
}
namespace XGAME {
struct xSceneBattleParam;
}

// 월드씬
class XSceneWorld : public XSceneBase
									, public XDelegateWndEdit
									, public XParticleDelegate
									, public XDelegateScrollView
{
public:
	const XE::VEC2 c_vHome = XE::VEC2(997,889);
	struct xRankInfo {
		int m_nRank;
		ID m_idAccount;	//나중에 ID 강조 등 쓸일이 있을 수 있음
		_tstring m_strName;
		int m_nLadderPoint;
		xRankInfo(int nRank, ID idAccount, _tstring strName, int nLadderPoint)
		{
			m_nRank = nRank;
			m_idAccount = idAccount;
			m_strName = strName;
			m_nLadderPoint = nLadderPoint;
		}
	};
private:
	template<typename T>
	struct xCallback {
		std::function<void( T* )> fnCallback;
		T* pCallbackOwner = nullptr;
	};
private:
	struct Layout {

	};
	struct xLayout {
		XLayout *pShop = nullptr;
		XLayout *pRank = nullptr;
		XLayout *pSocial = nullptr;
		XLayout *pMail = nullptr;
		xLayout() {
			DestroyAll();
		}
		void DestroyAll();
	} m_LayoutGroup;
	XLayoutObj m_Layout;
	XWndScrollView *m_pScrollView;
	CTimer m_timerClickSpot;
	CTimer m_timerExitGame;
	BOOL m_bOption[4];					// 옵션창 4개 체크박스 - 정완
	int m_SelInputBox;

	//xRegister m_sInput;
	_tstring m_strPassword;		// 에딧 박스로 입력한 패스워드를 보관해둘 변수
	_tstring m_strVerify;		// 에딧 박스로 다시 입력한 패스워드를 보관해둘 변수
	XGAME::xFocusMng m_FocusMng;		// 카메라이동 관리자.
	CTimer m_timerBird;
	CTimer m_timerSec;
	XSPAcc m_spAcc;
	XVector<XSurface*> m_aryWorldSurface;
//////////////////////////////////////
// 툴
#ifdef _xIN_TOOL
//	XBaseTool *m_pTool = nullptr;
	XE::VEC2 m_vMouse;
	XE::VEC2 m_vMouseWorld;
//	ID m_idTouchDownCloud;		// 터치다운시 선택된 구름
	ID m_idSelected;			// 선택된 구름.
	int m_idxClickedHexa;		///< 클릭한 헥사타일의 인덱스.
	BOOL m_bAction;
public:
// 	inline bool IsGameMode() {
// 		return m_pTool == nullptr;
// 	}
// 	inline bool IsToolMode() {
// 		return m_pTool != nullptr;
// 	}
//	GET_ACCESSOR( XBaseTool*, pTool );
private:
// xIN_TOOL
///////////////////////////////////////////////////////////////
#else
	inline bool IsGameMode() {
		return true;
	}
// not xIN_TOOL
#endif

#ifdef _CHEAT
	BOOL m_bViewCloudLayer;		// 구름레이어 보이기/감추기
#endif

private:
	//
	void Init()  {
		m_pScrollView = nullptr;

#ifdef _xIN_TOOL
//		m_bModeTool = FALSE;
//		m_idxCloud = -1;
//		m_idTouchDownCloud = 0;
		m_idSelected = 0;
		m_idxClickedHexa = -1;
		m_bAction = FALSE;
#endif

		m_bOption[0] = m_bOption[1] = m_bOption[2] = m_bOption[3] = FALSE;
		m_bViewCloudLayer = TRUE;

		m_SelInputBox = 0;
	}
	void Destroy();

protected:
public:
	XSceneWorld( XGame *pGame, XSPSceneParam& spBaseParam );
	~XSceneWorld() { Destroy(); }
	
	GET_ACCESSOR( XLayoutObj&, Layout );
#ifdef _xIN_TOOL
//	GET_SET_ACCESSOR( BOOL, bModeTool );
	GET_ACCESSOR( ID, idSelected );
	GET_ACCESSOR( int, idxClickedHexa );
	GET_ACCESSOR( BOOL, bAction );
	GET_ACCESSOR( const XE::VEC2&, vMouseWorld );
#endif
	GET_ACCESSOR( XWndScrollView*, pScrollView );

#ifdef _CHEAT
	GET_SET_ACCESSOR( BOOL, bViewCloudLayer );
#endif

	// virtual
	virtual void Create(void);
	BOOL OnCreate() override;
	virtual void OnDestroy() override;
	virtual int Process(float dt);
	virtual void Draw(void);
	virtual void OnLButtonDown(float lx, float ly);
	virtual void OnLButtonUp(float lx, float ly);
	virtual void OnRButtonDown(float lx, float ly);
	virtual void OnRButtonUp(float lx, float ly);

private:
	virtual void Update(void);		// 3 ~ 5초마다 

	XE::VEC2 GetPosWorldToLocal(const XE::VEC2& vwPos) const {
		if (m_pScrollView)
			return m_pScrollView->GetvAdjScroll() + vwPos;
		return vwPos;
	}

public:

#ifdef _xIN_TOOL
	void OnModeTool(xtToolMode toolMode);
//	void UpdateUIForTool(void);
	void DrawTool(void);
	int OnTouchDown(XWnd* pWnd, DWORD p1, DWORD p2);
	virtual void OnMouseMove(float lx, float ly);
	BOOL IsCoveredSpotByCloud(XPropWorld::xBASESPOT *pSpotProp, XPropCloud::xCloud *pProp);
	bool UpdateCloudSpotList( bool bAllCloud );
	bool UpdateSpotListByCloud(XPropCloud::xCloud *pProp);
	void EditedCloud(void);
	void UpdateAutoSave() {
		EditedCloud();
	}
	/// 저장해야할 변화가 있으면 이것을 한번 호출한다. 호출되면 자동세이브타이머가 켜지며 자동세이브가 된다.
	void UpdateSpotForTool(void);
	void DelSpotT();
	void CopySpotT();
	void Undo();
#endif // xIN_TOOL
	void _UpdateCloud();

#ifdef _CHEAT
	int OnCheat(XWnd* pWnd, DWORD p1, DWORD p2);
//	int OnCheatRegen(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnCheatClearQuest(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnCheatDelQuest( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickKill( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickXuzhuMode( XWnd* pWnd, DWORD p1, DWORD p2 );
//	int OnClickDebugShowLog( XWnd* pWnd, DWORD p1, DWORD p2 );
#endif // _CHEAT

public:

	void UpdateUI(void);
	

	// 월드 클릭 & 스크롤
	int OnClickWorld(XWnd* pWnd, DWORD p1, DWORD p2);			// 월드 이미지 클릭
	int OnScrollWorldmap(XWnd* pWnd, DWORD p1, DWORD p2);		// 월드 스크롤

	// 스팟 생성
// 	void CreateMyCastle(XWnd *pRoot);
	BOOL CreateJewelSpot( XSpotJewel *pSpot );
	BOOL CreateSulfurSpot( XSpotSulfur *pSpot );
	BOOL CreateMandrakeSpot( XSpotMandrake *pSpot );
	
	// 스팟 업데이트
private:
// 	void UpdateCastleSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateJewelSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateSulfurSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateMandrakeSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateNpcSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateDailySpot( XSpot *pBaseSpot, XWnd *pRoot );
// //	void UpdateSpecialSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateCampaignSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateVisitSpot( XSpot *pBaseSpot, XWnd *pRoot );
// 	void UpdateCashSpot( XSpot *pBaseSpot, XWnd *pRoot );
//   void UpdateCommonSpot( XSpot *pBaseSpot, XWnd *pRoot );
	void UpdateSpots(void);
  void UpdateResIcon( XSpot *pSpot, XWnd *pRoot );
public:
	// 스팟 클릭
	int OnClickCastleSpot(XSpot *pBaseSpot);
	int OnClickJewelSpot(XSpot *pBaseSpot);
	int OnClickSulfurSpot(XSpot *pBaseSpot);
	int OnClickMandrakeSpot(XSpot *pBaseSpot);
	int OnClickNpcSpot(XSpot *pBaseSpot);
	int OnClickDailySpot(XSpot *pBaseSpot);
//	int OnClickSpecialSpot(XSpot *pBaseSpot);
	int OnClickCampaignSpot(XSpot *pBaseSpot);
	int OnClickVisitSpot(XSpot *pBaseSpot);
	int OnClickCashSpot( XSpot *pBaseSpot );
  int OnClickCommonSpot( XSpot *pBaseSpot );

	void OnProduceResAtSpot( XSpot* pBaseSpot, const XTimer2& timerCalc, const std::vector<XGAME::xRES_NUM>& aryRes );
	int OnClickSpot( XWnd* pWnd, DWORD p1, DWORD p2 );

	// 스팟 공격 & 정찰 & 수집
	int OnReconSpot(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnAttackSpot(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnCollectSpot(XWnd* pWnd, DWORD p1, DWORD p2);

	// 헬퍼 함수
/*
	XWndAlert* CreateSpotPopup(const char *cIdentifier, XSpot *pBaseSpot, BOOL bRecon = TRUE, BOOL bAttack = TRUE);
*/
	XWndPopupSpotMenu* CreateSpotPopup2(ID idSpot );
	void CloseMenuReconAttack(void);
	void UpdateResourceUI(const char *cKey, int amount);
//	void UpdateResourceUI2( XGAME::xtResource typeRes );
//	void UpdateResourceUI2( XGAME::xtIndexUI idxUI, bool bForce = false );
	void ArriveResObj(XGAME::xtResource typeRes, int start);
//	void OnEndTransitionIn(XTransition *pTrans);
	void SpotcollectLock(ID idSpot);
	int OnCloseCenter(XWnd* pWnd, DWORD p1, DWORD p2);
//	int OnClickAttackFail(XWnd* pWnd, DWORD p1, DWORD p2);

	// Recv
	void RecvClearStorageWithWndSpot(XSpot *pBaseSpot);
	void OnRecvReconSpot(ID idSpot, const LegionPtr& spLegion );
//	int OnRecvBattleResult(XGAME::xtSpot typeSpot, int bWin);
	void OnRecvBattleResult( XGAME::xBattleResult& result );
	void OnRecvBattleInfo( XSPSceneParam spParam );
	//	void DoEnterBattleScene( ID idSpot, ID idBattle, int level, LPCTSTR szName, LegionPtr& spLegion, ID snSession/*, XAccount *pEnemy*/, int defense, std::shared_ptr<XGAME::xSPM_BATTLE> spOut );
	//	void OnRecvAttackedSpotResult( BOOL bWin, XSpotResource *pSpot, int level );
	void OnRecvAttackedCastle(ID idSpot, ID idAccount, int level, LPCTSTR szName);
//	void OnRecvSpotCollect(XSpot *pBaseSpot, XGAME::xtResource typeRes, float _num);
	void OnRecvSpotCollect( XSpot *pBaseSpot, const std::vector<XGAME::xRES_NUM>& aryRes );

	// 아래 버튼들
	int OnClickUnitOrg(XWnd* pWnd, DWORD p1, DWORD p2);		// 부대편성 버튼
	int OnClickLaboratory(XWnd* pWnd, DWORD p1, DWORD p2);	// 기술연구 버튼
	int OnClickHero(XWnd* pWnd, DWORD p1, DWORD p2);		// 영웅고용 버튼
	int OnClickStorage(XWnd* pWnd, DWORD p1, DWORD p2);		// 창고 버튼
	int OnClickShop(XWnd* pWnd, DWORD p1, DWORD p2);		// 시장 버튼

	// 시장 버튼들
	int OnTrader(XWnd* pWnd, DWORD p1, DWORD p2);			// 교역상 버튼
	int OnArmory(XWnd* pWnd, DWORD p1, DWORD p2);			// 무기상 버튼
	int OnPremium(XWnd* pWnd, DWORD p1, DWORD p2);			// 잡화점 버튼

	// 왼쪽 버튼들
	int OnClickLeftMenuHide(XWnd* pWnd, DWORD p1, DWORD p2);		// 왼쪽 메뉴 버튼

	int OnClickRank(XWnd* pWnd, DWORD p1, DWORD p2);		// 랭크 버튼
	int OnClickSocial(XWnd* pWnd, DWORD p1, DWORD p2);		// 소셜 버튼
	int OnClickMailbox(XWnd* pWnd, DWORD p1, DWORD p2);		// 우편함 버튼
	int OnClickOption(XWnd* pWnd, DWORD p1, DWORD p2);		// 옵션 버튼

	// 랭크
	int OnClickRankPrev(XWnd* pWnd, DWORD p1, DWORD p2);		// 이전 버튼
	int OnClickRankNext(XWnd* pWnd, DWORD p1, DWORD p2);		// 다음 버튼

	// 우편함
	int OnClickMailboxElem(XWnd* pWnd, DWORD p1, DWORD p2);		// 우편함 리스트 클릭
	int OnClickMailGetItem(XWnd* pWnd, DWORD p1, DWORD p2);		// 획득 클릭
//	int OnClickMailOk(XWnd* pWnd, DWORD p1, DWORD p2);			// 확인 클릭
	void UpdateMailBoxAlert();

	void UpdateMailbox();		// 우편함 업데이트
	void RecvGetItem();

	// 옵션
//	int OnClickCheckSound(XWnd* pWnd, DWORD p1, DWORD p2);		// 소리끄기
//	int OnClickCheck2(XWnd* pWnd, DWORD p1, DWORD p2);		// 언어선택
//	int OnClickCheck3(XWnd* pWnd, DWORD p1, DWORD p2);		// 계정등록
	int OnClickCheckInvite(XWnd* pWnd, DWORD p1, DWORD p2);		// 초대거절

	int OnClickOptionClose(XWnd* pWnd, DWORD p1, DWORD p2);		// 옵션 닫기 버튼
private:
	int OnClosePopupBySideMenu(XWnd* pWnd, DWORD p1, DWORD p2);		// 팝업 닫기

public:
	void OnViewCloudLayer( void );
	int OnClickCloudLabel( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvOpenCloud( ID idCloud, DWORD dwGold, DWORD addCash );
	

	// 퀘스트
//	int OnBookQuest( XWnd* pWnd, DWORD p1, DWORD p2 );
	BOOL IsCompleteQuest( void );
	virtual void OnRecvPacket( ID idPacket );
	
	void UpdateQuestUI(void);
	void OnOccurQuest( XQuestObj *pObj );
	void RecvCompleteQuest( XQuestObj *pObj );
	void OnUpdateQuest( XQuestObj *pObj );
	void OnRecvQuestReward( XQuestObj *pObj );
	void OnClickStageInCampaign( ID snSession, ID idSpot, int idxStage, int level, LegionPtr& spLegion );
	int OnCompleteQuest( XWnd* pWnd, DWORD p1, DWORD p2 );

	// ??
	void OnSpotSync(ID idSpot);
	BOOL OnKeyUp( int keyCode );
//	void CreateDebugButtons(void);
	void UpdateDebugButtons();
	int OnClickFace( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvRankList(const XVector<xRankInfo*>& aryRank);

	// 계정등록
// 	int OnClickRegister(XWnd* pWnd, DWORD p1, DWORD p2);
// 	int OnClickEmailBox(XWnd* pWnd, DWORD p1, DWORD p2);
// 	int OnClickPasswordBox(XWnd* pWnd, DWORD p1, DWORD p2);
// 	int OnClickVerifyBox(XWnd* pWnd, DWORD p1, DWORD p2);
	void RecvRegistID(BOOL bSuccess);

	// 에딧 박스
	void OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld) override;

	int OnClickChangePlayerByGold( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickChangePlayerByCash( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickChangePlayerByCashLack( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickRegen( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnOkRegenSpot( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickBattleLog( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnMandrakeWithdraw( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnAutoUpdate() override;
	void DoPopupLevelup();
	void DoBlinkSpot( ID idSpot, float sec = 10.f );
	void DoBlinkSpotCode( ID code, float sec = 10.f );
	void DoBlinkSpotType( XGAME::xtSpot type, float sec = 10.f, ID idCloud = 0 );
	void DoMovePosInWorld( const XE::VEC2& vCenter, float secMove = 0 );
// 	template<typename T, typename F>
// 	void DoMovePosInWorld( const XE::VEC2& vCenter, float secMove, T* pOwner, F func ) {
// 		m_FocusMng.DoMove( m_pScrollView->GetvCenter(), vCenter, secMove, pOwner, func );
// 	}
	void DoMoveToArea( ID idCloud, float secMove = 0 );
	void DoMoveToSpot( ID idSpot, float secMove = 0 );
// 	template<typename T, typename F>
// 	void DoMoveToSpot( ID idSpot, float secMove, T* pOwner, F func ) {
// 		DoMoveToSpot( idSpot, secMove );
// 		m_FocusMng.callBack.pCallbackOwner = pOwner;
// 		m_FocusMng.callBack.fnCallback = std::bind( func, std::placeholders::_1 );
// 	}
	void DoMoveToCodeSpots( ID code, float sec = 0 );
	void DoDirectToPos( const XE::VEC2& vPos, float sec = 0 );
	int OnClickHome( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickAccLogin(XWnd* pWnd, DWORD p1, DWORD p2);
// 	int OnClickAccLoginOk(XWnd* pWnd, DWORD p1, DWORD p2);
// 	void RecvAccLogin(int result);
//	int OnClickFillAPByCash( XWnd* pWnd, DWORD p1, DWORD p2 );
	void RecvCampaignByGuildRaid( ID idSpot, xCampaign::CampObjPtr spCampObj );
	void UpdateUnitOrg();
	void UpdateTech();
	void UpdateSummonHero();
	void UpdateStorage();
	void UpdateMarket();
// 	void AddAlert( XWnd *pButt, ID idAct, const XE::VEC2& _vPos = XE::VEC2(-1) );
// 	void DelAlert( XWnd *pButt );
	void AddGear( XWnd *pButt, const XE::VEC2& _vPos );
	void DelGear( XWnd *pButt );
//	void DoCutScene();
//	void DoCutScene( const char* idsSeq );
	XWnd* GetpFixedUIRoot() {
		return Find("root.ui");	
	}
//	void SetActive( bool bFlag ) override;
	XWnd* GetpRootSpot() {
		return Find( "root.spot" );
	}
	XWnd* GetpRootWorldObj() {
		return Find("root.world.obj");
	}
	XWnd* GetpLayerUIParticle() {
		return Find("root.layer.ui.particle");
	}
	void OnReload();
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;		// 씬이 모두 밝아지고 호출됨
	void DoEmitBird();
	void OnRecvSpotTouch( XSpot *pBaseSpot, std::vector<XGAME::xDropItem>& aryDropItem, XArchive& arAdd );
	void ApplyAreaBannersByOpened();
	void ApplyAreaBannerByAry( const std::vector<ID>& aryArea, bool bShowOpened, bool bShowClosed, float secLife );
	void UpdateCostLabel();
	void OnAttackedLog();
 	void DoMakeResourceParticle( const XE::VEC2& vPos, XGAME::xtResource typeRes, float _num );
	void DoMakeResourceParticle( const XE::VEC2& _vPos, const std::vector<XGAME::xRES_NUM>& aryRes );
	XE::VEC2 OnDelegateProjection2( const XE::VEC3& vPos ) override;
	void OnRecvUnlockMenu( XGAME::xtMenus bitUnlock );
	bool IsQuestDirectionToArea( XPropCloud::xCloud* pPropArea );
	XE::VEC2 GetvwCamera();
	void SetvwCamera( const XE::VEC2& vwCamera );
	XHero* GetpHero( ID idHero ) override;
//	void UpdateOptionPopup();
//	void UpdateSpotProfilePicture( XSpotCastle *pSpot, const std::string& strFbUserId, const XE::xImage& imgInfo );
	/// 프로필사진 레이어를 얻는다.
	XWnd* GetpLayerByProfilePictures() {
		if( m_pScrollView == nullptr )
			return nullptr;
		return m_pScrollView->Find( "root.profile.picture" );
	}
	XWndBgObjLayer* GetpLayerBgObjUnder();
	XWnd* GetpLayerSpotForTool() {
		if( m_pScrollView )
			return m_pScrollView->Find( "wnd.root.tool.scrollview" );
		return nullptr;
	}
	XWnd* GetpLayerAreaCost() {
		return Find( "root.cost.label" );
	}
	XE::VEC2 GetvwCenterByAllWorld() const {
		return m_pScrollView->GetsizeScroll() * 0.5f;
	}
	XE::VEC2 GetvwSizeView() const {
		return m_pScrollView->GetsizeView();
	}
	void UpdateBgObjs();
	XWndSpot* GetpWndSpot( ID idSpot );
	void OnTraderArrive();
#ifdef _xIN_TOOL
	void UpdateBgObjEach( xnBgObj::xProp* pProp );
	void UpdateBgObjSelected();
#endif // _xIN_TOOL
private:
	int OnClickFacebook( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickProfile( XWnd* pWnd, DWORD p1, DWORD p2 );
	void LoadWorldMap( XWndScrollView* pScrlView );
	void UpdateAreaAttension( const XE::VEC2& vwPos );
	void DestroyAreaAttension();
//	void DestroyWndOption();
//	int OnClickSelectLanguage( XWnd* pWnd, DWORD p1, DWORD p2 );
//	int OnCloseOption( XWnd* pWnd, DWORD p1, DWORD p2 );
	void DelegateStopScroll( const XE::VEC2& vCurr ) override;

	//
#ifdef _xIN_TOOL
	void DelegateChangeToolMode( xtToolMode modeOld, xtToolMode modeCurr ) override;
	friend class XTool;
	friend class XToolCloud;
#endif // _xIN_TOOL
	void AddMenuKill( XWndPopupSpotMenu* pPopup, ID idSpot, int enemyPower, bool bNPC );
//	void OnRecvSyncAcc( XGAME::xtParamSync type );
};

extern XSceneWorld *SCENE_WORLD;

XE_NAMESPACE_START( XGAME )
//
bool UpdateProfileImage( XWnd *pRoot, const std::string& strIds, bool bShow, XSurface *psfcPicture );
//
XE_NAMESPACE_END; // XGAME

