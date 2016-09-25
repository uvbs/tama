
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "XDlgConsole.h"
#include "DlgPropGrid.h"
#include "_DirectX/XE_MFCFramework.h"


#ifdef _DEBUG
#ifdef WIN32
// 아이폰비율
//	#define PHY_WIDTH	960		// windows에서의 창크기
//	#define PHY_HEIGHT	640	  
// 4:3-아이패드 비율
// 	#define PHY_WIDTH	1024		// windows에서의 창크기
// 	#define PHY_HEIGHT	768	  
// 16:9
#define PHY_WIDTH		1280		// windows에서의 창크기
#define PHY_HEIGHT	720
//#define PHY_WIDTH	576		// 
//#define PHY_HEIGHT	1024	  
// 5:3- 갤럭시s2(480x800)
//	#define PHY_WIDTH		640		// windows에서의 창크기
//	#define PHY_HEIGHT		1066
// 8:5-(800x1280)
//	#define PHY_WIDTH		640		// windows에서의 창크기
//	#define PHY_HEIGHT		1024
// 16:10
// 	#define PHY_WIDTH 1152
// 	#define PHY_HEIGHT 720

//#define PHY_WIDTH	720		// windows에서의 창크기
//#define PHY_HEIGHT	1280
#endif // win32
#else // debug
	#define PHY_WIDTH	1280		// windows에서의 창크기
	#define PHY_HEIGHT	720
#endif // not debug
class CMainFrame : public CFrameWnd, public XE_MFCFramework
{
// 	struct xText {
// 		ID idText = 0;
// 		_tstring strText;
// 		xText( ID _idText, const _tstring& _str )
// 			: idText( _idText ), strText( _str ) {}
// 	};
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	XDlgConsole m_dlgConsole;
	CDlgPropGrid m_dlgPropGrid;

// 작업입니다.
public:
	void SetTitle( void );

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void ExtractHangulText( XVector<XTextTable::xText>* pOutAry );
	bool SaveTextList( LPCTSTR szFile, const XList4<XTextTable::xText>& aryText );

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CCoolDialogBar m_wndConsole;
#ifdef _XPROP_GRID
	CCoolDialogBar m_wndPropGrid;
#endif // _XPROP_GRID

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheatMode();
	afx_msg void OnUpdateCheatMode(CCmdUI *pCmdUI);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolMode();
	afx_msg void OnUpdateToolMode( CCmdUI *pCmdUI );
	afx_msg void OnNextCloud();
	afx_msg void OnPrevCloud();
	afx_msg void OnSelectCloud();
	afx_msg void OnViewCloudLayer();
	afx_msg void OnUpdateViewCloudLayer( CCmdUI *pCmdUI );
	afx_msg void OnDebugViewSquadRadius();
	afx_msg void OnUpdateDebugViewSquadRadius( CCmdUI *pCmdUI );
	afx_msg void OnDebug1vs1();
	afx_msg void OnUpdateDebug1vs1( CCmdUI *pCmdUI );
	afx_msg void OnDebugViewRefcnt();
	afx_msg void OnUpdateDebugViewRefcnt( CCmdUI *pCmdUI );
	afx_msg void OnDebugViewUnitsn();
	afx_msg void OnUpdateDebugViewUnitsn( CCmdUI *pCmdUI );
	afx_msg void OnDebugViewTarget();
	afx_msg void OnUpdateDebugViewTarget( CCmdUI *pCmdUI );
	afx_msg void OnNumUnitRandom();
	afx_msg void OnUpdateNumUnitRandom( CCmdUI *pCmdUI );
	afx_msg void OnUnitRandom();
	afx_msg void OnUpdateUnitRandom( CCmdUI *pCmdUI );
	afx_msg void OnUnitAllSmall();
	afx_msg void OnUpdateUnitAllSmall( CCmdUI *pCmdUI );
	afx_msg void OnUnitAllMiddle();
	afx_msg void OnUpdateUnitAllMiddle( CCmdUI *pCmdUI );
	afx_msg void OnUnitAllBig();
	afx_msg void OnUpdateUnitAllBig( CCmdUI *pCmdUI );
	afx_msg void OnReloadSkill();
	afx_msg void OnViewBuff();
	afx_msg void OnUpdateViewBuff( CCmdUI *pCmdUI );
	afx_msg void OnSlowFrame();
	afx_msg void OnUpdateSlowFrame( CCmdUI *pCmdUI );
	afx_msg void OnSetFrameskip();
	afx_msg void OnUpdateSetFrameskip( CCmdUI *pCmdUI );
	afx_msg void OnBattle11();
	afx_msg void OnViewBoundBox();
	afx_msg void OnUpdateViewBoundBox( CCmdUI *pCmdUI );
	afx_msg void OnViewSquadSn();
	afx_msg void OnUpdateViewSquadSn( CCmdUI *pCmdUI );
	afx_msg void OnCreateItem();
	afx_msg void OnViewHp();
	afx_msg void OnUpdateViewHp( CCmdUI *pCmdUI );
	afx_msg void OnSelectCancel();
	afx_msg void OnToolAlphaCloud();
	afx_msg void OnUpdateToolAlphaCloud( CCmdUI *pCmdUI );
	afx_msg void OnEditCloud();
	afx_msg void OnViewHexaLine();
	afx_msg void OnUpdateViewHexaLine( CCmdUI *pCmdUI );
	afx_msg void OnRandomCloud();
	afx_msg void OnModeTechTool();
	afx_msg void OnUpdateModeTechTool( CCmdUI *pCmdUI );
	afx_msg void OnCreateAbil();
	afx_msg void OnAbilToolMoveMode();
	afx_msg void OnUpdateAbilToolMoveMode( CCmdUI *pCmdUI );
	afx_msg void OnAbilToolLinkMode();
	afx_msg void OnUpdateAbilToolLinkMode( CCmdUI *pCmdUI );
	afx_msg void OnToolShowCloud();
	afx_msg void OnToolHideCloud();
	afx_msg void OnUpdateToolShowCloud( CCmdUI *pCmdUI );
	afx_msg void OnUpdateToolHideCloud( CCmdUI *pCmdUI );
	afx_msg void OnModeSpotTool();
	afx_msg void OnUpdateModeSpotTool( CCmdUI *pCmdUI );
	afx_msg void OnGameMode();
	afx_msg void OnUpdateGameMode( CCmdUI *pCmdUI );
	afx_msg void OnSpotAllView();
	afx_msg void OnUpdateSpotAllView( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewCastle();
	afx_msg void OnUpdateSpotViewCastle( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewSulfur();
	afx_msg void OnUpdateSpotViewSulfur( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewMandrake();
	afx_msg void OnUpdateSpotViewMandrake( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewNpc();
	afx_msg void OnUpdateSpotViewNpc( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewEvent();
	afx_msg void OnUpdateSpotViewEvent( CCmdUI *pCmdUI );
	afx_msg void OnSpotViewJewel();
	afx_msg void OnUpdateSpotViewJewel( CCmdUI *pCmdUI );
	afx_msg void OnDelSpot();
	afx_msg void OnViewQuest();
	afx_msg void OnUpdateViewQuest( CCmdUI *pCmdUI );
	afx_msg void OnUpdate();
	afx_msg void OnViewDamage();
	afx_msg void OnUpdateViewDamage( CCmdUI *pCmdUI );
	afx_msg void OnViewAttackedDamage();
	afx_msg void OnUpdateViewAttackedDamage( CCmdUI *pCmdUI );
	afx_msg void OnHeroImmortal();
	afx_msg void OnUpdateHeroImmortal( CCmdUI *pCmdUI );
	afx_msg void OnViewSquadInfo();
	afx_msg void OnUpdateViewSquadInfo( CCmdUI *pCmdUI );
	afx_msg void OnViewEnemyHeroLevel();
	afx_msg void OnUpdateViewEnemyHeroLevel( CCmdUI *pCmdUI );
	afx_msg void OnUnitImmotal();
	afx_msg void OnUpdateUnitImmotal( CCmdUI *pCmdUI );
	afx_msg void OnCreateHero();
	afx_msg void OnXuzhuMode();
	afx_msg void OnUpdateXuzhuMode( CCmdUI *pCmdUI );
	afx_msg void OnDisconnect();
  afx_msg void OnGotoHome();
  afx_msg void OnReturnFromHome();
  afx_msg void OnGotoStart();
  afx_msg void OnReload();
  afx_msg void OnSpotViewBuilding();
  afx_msg void OnUpdateSpotViewBuilding( CCmdUI *pCmdUI );
  afx_msg void OnUpdateQuest();
  afx_msg void OnRestartSeq();
  afx_msg void OnCloseArea();
  afx_msg void OnDelSeq();
  afx_msg void OnResetQuest();
  afx_msg void OnTestMail();
  afx_msg void OnModifyText();
	afx_msg void OnMergeText();
	afx_msg void OnExtractHangul();
	afx_msg void OnOmissionText();
	afx_msg void OnFillEmtpyText();
  afx_msg void OnResavePropHelp();
  afx_msg void OnViewBoundboxSpot();
  afx_msg void OnUpdateViewBoundboxSpot( CCmdUI *pCmdUI );
  afx_msg void OnFinishSeq();
	afx_msg void OnPlayx1();
	afx_msg void OnPlayx2();
	afx_msg void OnPlayx4();
	afx_msg void OnPlayx8();
	afx_msg void OnUpdatePlayx1( CCmdUI *pCmdUI );
	afx_msg void OnUpdatePlayx2( CCmdUI *pCmdUI );
	afx_msg void OnUpdatePlayx4( CCmdUI *pCmdUI );
	afx_msg void OnUpdatePlayx8( CCmdUI *pCmdUI );
	afx_msg void OnViewAreaLabel();
	afx_msg void OnUpdateViewAreaLabel( CCmdUI *pCmdUI );
	afx_msg void OnViewSquads();
	afx_msg void OnUpdateViewSquads( CCmdUI *pCmdUI );
	afx_msg void OnPlayPause();
	afx_msg void OnUpdatePlayPause( CCmdUI *pCmdUI );
	afx_msg void OnPlay();
	afx_msg void OnUpdatePlay( CCmdUI *pCmdUI );
	afx_msg void OnKeyBack();
	afx_msg void OnStopPassive();
	afx_msg void OnUpdateStopPassive( CCmdUI *pCmdUI );
	afx_msg void OnFilHero();
	afx_msg void OnUpdateFilHero( CCmdUI *pCmdUI );
	afx_msg void OnFilUnit();
	afx_msg void OnUpdateFilUnit( CCmdUI *pCmdUI );
	afx_msg void OnStopActive();
	afx_msg void OnUpdateStopActive( CCmdUI *pCmdUI );
	afx_msg void OnFilterPlayer();
	afx_msg void OnUpdateFilterPlayer( CCmdUI *pCmdUI );
	afx_msg void OnFilterEnemy();
	afx_msg void OnUpdateFilterEnemy( CCmdUI *pCmdUI );
	afx_msg void OnViewCutScene();
	afx_msg void OnUpdateViewCutScene( CCmdUI *pCmdUI );
	afx_msg void OnViewMemoryInfo();
	afx_msg void OnUpdateViewMemoryInfo( CCmdUI *pCmdUI );
	afx_msg void OnViewFrameRate();
	afx_msg void OnUpdateViewFrameRate( CCmdUI *pCmdUI );
	afx_msg void OnLayerTest();
	afx_msg void OnUpdateLayerTest( CCmdUI *pCmdUI );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnViewHexaIdx();
	afx_msg void OnUpdateViewHexaIdx( CCmdUI *pCmdUI );
	afx_msg void OnViewAreaCost();
	afx_msg void OnUpdateViewAreaCost( CCmdUI *pCmdUI );
	afx_msg void OnModeBgObjTool();
	afx_msg void OnUpdateModeBgObjTool( CCmdUI *pCmdUI );
	afx_msg void OnViewBgObjBoundBox();
	afx_msg void OnUpdateViewBgObjBoundBox( CCmdUI *pCmdUI );
	afx_msg void OnViewSpotInfo();
	afx_msg void OnUpdateViewSpotInfo( CCmdUI *pCmdUI );
	afx_msg void OnDelNode();
	afx_msg void OnUpdateDelNode( CCmdUI *pCmdUI );
	afx_msg void OnPlaySoundEffect();
	afx_msg void OnUpdatePlaySoundEffect( CCmdUI *pCmdUI );
	afx_msg void OnPlayBGM();
	afx_msg void OnUpdatePlayBGM( CCmdUI *pCmdUI );
	afx_msg void OnReloadSoundTbl();
	afx_msg void OnReloadLayout();
	afx_msg void OnTestParticle();
	afx_msg void OnReloadParticle();
	void OnModeTestParticle();
	void OnUpdateModeTestParticle( CCmdUI *pCmdUI );
	afx_msg void OnViewWinBoundBox();
	afx_msg void OnUpdateViewWinBoundBox( CCmdUI *pCmdUI );
	afx_msg void OnViewMouseOverWins();
	afx_msg void OnUpdateViewMouseOverWins( CCmdUI *pCmdUI );
	afx_msg void OnFlushCashImg();
	afx_msg void OnCheatCommand();
	afx_msg void OnViewDamageLogging();
	afx_msg void OnUpdateOnViewDamageLogging( CCmdUI *pCmdUI );
	afx_msg void OnReloadCmd();
	afx_msg void OnBattleOption();
	afx_msg void OnReloadWhenRetryConstant();
	afx_msg void OnReloadWhenRetryPropUnit();
	afx_msg void OnReloadWhenRetryPropSkill();
	afx_msg void OnReloadWhenRetryPropLegion();
	afx_msg void OnUpdateReloadWhenRetryConstant( CCmdUI *pCmdUI );
	afx_msg void OnUpdateReloadWhenRetryPropUnit( CCmdUI *pCmdUI );
	afx_msg void OnUpdateReloadWhenRetryPropSkill( CCmdUI *pCmdUI );
	afx_msg void OnUpdateReloadWhenRetryPropLegion( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoDraw();
	afx_msg void OnUpdateProfileTestNoDraw( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoDP();
	afx_msg void OnUpdateProfileTestNoDP( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoTexture();
	afx_msg void OnUpdateProfileTestNoTexture( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestSmallTex();
	afx_msg void OnUpdateProfileTestSmallTex( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoDrawBar();
	afx_msg void OnUpdateProfileTestNoDrawBar( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoFontDraw();
	afx_msg void OnUpdateProfileTestNoFontDraw( CCmdUI *pCmdUI );
	afx_msg void OnProfileTestNoProcess();
	afx_msg void OnUpdateProfileTestNoProcess( CCmdUI *pCmdUI );
	afx_msg void OnProfiling();
	afx_msg void OnUpdateProfiling( CCmdUI *pCmdUI );
	afx_msg void OnGotoTestScene();
	afx_msg void OnHideHpbar();
	afx_msg void OnHideName();
	afx_msg void OnUpdateHideHpbar( CCmdUI *pCmdUI );
	afx_msg void OnUpdateHideName( CCmdUI *pCmdUI );
};

extern CMainFrame *g_pMainFrm;
#define MAINFRAME	g_pMainFrm
#define D3DDEVICE		g_pMainFrm->GetDevice()



