// MainFrm.cpp : CMainFrame 클래스의 구현
//
#include "stdafx.h"
#include "Caribe.h"
#include "MainFrm.h"
#include "XConnector.h"
#include "XResObj.h"
#include "XGame.h"
#include "client/XAppMain.h"
#include "XSceneWorld.h"
#include "XSceneTech.h"
#include "DlgBattle11.h"
#include "DlgCreateItem.h"
#include "DlgCreateHero.h"
#include "XDlgPropCloud.h"
#include "DlgRestartSeq.h"
#include "DlgEditAbility.h"
#include "DlgCloseArea.h"
#include "DlgEnterName.h"
#include "DlgLayerTest.h"
//#include "DlgEditVal.h"
#include "DlgCheatCommand.h"
#include "DlgBattleOption.h"
#include "XSceneTech.h"
#include "XPropTech.h"
#include "DlgPropGrid.h"
#include "XSockGameSvr.h"
#include "XSockLoginSvr.h"
#include "XTool.h"
#include "XQuestProp.h"
#include "XQuestMng.h"
#include "XPropHelp.h"
#include "XSeq.h"
#include "XWorld.h"
#include "XFramework/XSoundTable.h"
#include "XSoundMng.h"
#include "XOption.h"
#include "_Wnd2/XWnd.h"
#include "Sprite/SprMng.h"
#include "etc/xUtil.h"
#ifdef WIN32
#include "XObjEtc.h"
#endif // WIN32
#ifdef _XPROFILE
#include "XSceneBattle.h"
#include "XFramework/XEProfile.h"
#endif // _XPROFILE


// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
// CMainFrame
CMainFrame *g_pMainFrm = nullptr;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_CHEAT_MODE, &CMainFrame::OnCheatMode)
	ON_UPDATE_COMMAND_UI(ID_CHEAT_MODE, &CMainFrame::OnUpdateCheatMode)
	ON_WM_SIZE()
	ON_COMMAND( IDM_RELOAD_SOUNTTBL, &CMainFrame::OnReloadSoundTbl )
	ON_COMMAND( ID_TOOL_MODE, &CMainFrame::OnToolMode )
	ON_UPDATE_COMMAND_UI( ID_TOOL_MODE, &CMainFrame::OnUpdateToolMode )
	ON_COMMAND( ID_NEXT_CLOUD, &CMainFrame::OnNextCloud )
	ON_COMMAND( ID_PREV_CLOUD, &CMainFrame::OnPrevCloud )
	ON_COMMAND( ID_SELECT_CLOUD, &CMainFrame::OnSelectCloud )
	ON_COMMAND( ID_VIEW_CLOUD_LAYER, &CMainFrame::OnViewCloudLayer )
	ON_UPDATE_COMMAND_UI( ID_VIEW_CLOUD_LAYER, &CMainFrame::OnUpdateViewCloudLayer )
	ON_COMMAND( ID_DEBUG_VIEW_SQUAD_RADIUS, &CMainFrame::OnDebugViewSquadRadius )
	ON_UPDATE_COMMAND_UI( ID_DEBUG_VIEW_SQUAD_RADIUS, &CMainFrame::OnUpdateDebugViewSquadRadius )
	ON_COMMAND( ID_DEBUG_1VS1, &CMainFrame::OnDebug1vs1 )
	ON_UPDATE_COMMAND_UI( ID_DEBUG_1VS1, &CMainFrame::OnUpdateDebug1vs1 )
	ON_COMMAND( ID_DEBUG_VIEW_REFCNT, &CMainFrame::OnDebugViewRefcnt )
	ON_UPDATE_COMMAND_UI( ID_DEBUG_VIEW_REFCNT, &CMainFrame::OnUpdateDebugViewRefcnt )
	ON_COMMAND( ID_DEBUG_VIEW_UNITSN, &CMainFrame::OnDebugViewUnitsn )
	ON_UPDATE_COMMAND_UI( ID_DEBUG_VIEW_UNITSN, &CMainFrame::OnUpdateDebugViewUnitsn )
	ON_COMMAND( ID_DEBUG_VIEW_TARGET, &CMainFrame::OnDebugViewTarget )
	ON_UPDATE_COMMAND_UI( ID_DEBUG_VIEW_TARGET, &CMainFrame::OnUpdateDebugViewTarget )
	ON_COMMAND( ID_NUM_UNIT_RANDOM, &CMainFrame::OnNumUnitRandom )
	ON_UPDATE_COMMAND_UI( ID_NUM_UNIT_RANDOM, &CMainFrame::OnUpdateNumUnitRandom )
	ON_COMMAND( ID_UNIT_RANDOM, &CMainFrame::OnUnitRandom )
	ON_UPDATE_COMMAND_UI( ID_UNIT_RANDOM, &CMainFrame::OnUpdateUnitRandom )
	ON_COMMAND( ID_UNIT_ALL_SMALL, &CMainFrame::OnUnitAllSmall )
	ON_UPDATE_COMMAND_UI( ID_UNIT_ALL_SMALL, &CMainFrame::OnUpdateUnitAllSmall )
	ON_COMMAND( ID_UNIT_ALL_MIDDLE, &CMainFrame::OnUnitAllMiddle )
	ON_UPDATE_COMMAND_UI( ID_UNIT_ALL_MIDDLE, &CMainFrame::OnUpdateUnitAllMiddle )
	ON_COMMAND( ID_UNIT_ALL_BIG, &CMainFrame::OnUnitAllBig )
	ON_UPDATE_COMMAND_UI( ID_UNIT_ALL_BIG, &CMainFrame::OnUpdateUnitAllBig )
	ON_COMMAND( ID_RELOAD_SKILL, &CMainFrame::OnReloadSkill )
	ON_COMMAND( ID_VIEW_BUFF, &CMainFrame::OnViewBuff )
	ON_UPDATE_COMMAND_UI( ID_VIEW_BUFF, &CMainFrame::OnUpdateViewBuff )
	ON_COMMAND( ID_SLOW_FRAME, &CMainFrame::OnSlowFrame )
	ON_UPDATE_COMMAND_UI( ID_SLOW_FRAME, &CMainFrame::OnUpdateSlowFrame )
	ON_COMMAND( ID_SET_FRAMESKIP, &CMainFrame::OnSetFrameskip )
	ON_UPDATE_COMMAND_UI( ID_SET_FRAMESKIP, &CMainFrame::OnUpdateSetFrameskip )
	ON_COMMAND( ID_BATTLE11, &CMainFrame::OnBattle11 )
	ON_COMMAND( ID_VIEW_BOUND_BOX, &CMainFrame::OnViewBoundBox )
	ON_UPDATE_COMMAND_UI( ID_VIEW_BOUND_BOX, &CMainFrame::OnUpdateViewBoundBox )
	ON_COMMAND( ID_VIEW_SQUAD_SN, &CMainFrame::OnViewSquadSn )
	ON_UPDATE_COMMAND_UI( ID_VIEW_SQUAD_SN, &CMainFrame::OnUpdateViewSquadSn )
	ON_COMMAND( ID_CREATE_ITEM, &CMainFrame::OnCreateItem )
	ON_COMMAND( ID_VIEW_HP, &CMainFrame::OnViewHp )
	ON_UPDATE_COMMAND_UI( ID_VIEW_HP, &CMainFrame::OnUpdateViewHp )
	ON_COMMAND( ID_SELECT_CANCEL, &CMainFrame::OnSelectCancel )
	ON_COMMAND( ID_TOOL_ALPHA_CLOUD, &CMainFrame::OnToolAlphaCloud )
	ON_UPDATE_COMMAND_UI( ID_TOOL_ALPHA_CLOUD, &CMainFrame::OnUpdateToolAlphaCloud )
	ON_COMMAND( ID_EDIT_CLOUD, &CMainFrame::OnEditCloud )
	ON_COMMAND( ID_VIEW_HEXA_LINE, &CMainFrame::OnViewHexaLine )
	ON_UPDATE_COMMAND_UI( ID_VIEW_HEXA_LINE, &CMainFrame::OnUpdateViewHexaLine )
	ON_COMMAND( ID_RANDOM_CLOUD, &CMainFrame::OnRandomCloud )
	ON_COMMAND( ID_MODE_TECH_TOOL, &CMainFrame::OnModeTechTool )
	ON_UPDATE_COMMAND_UI( ID_MODE_TECH_TOOL, &CMainFrame::OnUpdateModeTechTool )
//#ifdef _xIN_TOOL
	ON_COMMAND( IDM_CREATE_ABIL, &CMainFrame::OnCreateAbil )
	ON_COMMAND( IDM_ABIL_TOOL_MOVE_MODE, &CMainFrame::OnAbilToolMoveMode )
	ON_UPDATE_COMMAND_UI( IDM_ABIL_TOOL_MOVE_MODE, &CMainFrame::OnUpdateAbilToolMoveMode )
	ON_COMMAND( IDM_ABIL_TOOL_LINK_MODE, &CMainFrame::OnAbilToolLinkMode )
	ON_UPDATE_COMMAND_UI( IDM_ABIL_TOOL_LINK_MODE, &CMainFrame::OnUpdateAbilToolLinkMode )
//#endif // xIN_TOOL
	ON_COMMAND( IDM_TOOL_SHOW_CLOUD, &CMainFrame::OnToolShowCloud )
	ON_COMMAND( IDM_TOOL_HIDE_CLOUD, &CMainFrame::OnToolHideCloud )
	ON_UPDATE_COMMAND_UI( IDM_TOOL_SHOW_CLOUD, &CMainFrame::OnUpdateToolShowCloud )
	ON_UPDATE_COMMAND_UI( IDM_TOOL_HIDE_CLOUD, &CMainFrame::OnUpdateToolHideCloud )
	ON_COMMAND( ID_MODE_SPOT_TOOL, &CMainFrame::OnModeSpotTool )
	ON_UPDATE_COMMAND_UI( ID_MODE_SPOT_TOOL, &CMainFrame::OnUpdateModeSpotTool )
	ON_COMMAND( IDM_GAME_MODE, &CMainFrame::OnGameMode )
	ON_UPDATE_COMMAND_UI( IDM_GAME_MODE, &CMainFrame::OnUpdateGameMode )
	ON_COMMAND( IDM_SPOT_ALL_VIEW, &CMainFrame::OnSpotAllView )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_ALL_VIEW, &CMainFrame::OnUpdateSpotAllView )
	ON_COMMAND( IDM_SPOT_VIEW_CASTLE, &CMainFrame::OnSpotViewCastle )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_CASTLE, &CMainFrame::OnUpdateSpotViewCastle )
	ON_COMMAND( IDM_SPOT_VIEW_SULFUR, &CMainFrame::OnSpotViewSulfur )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_SULFUR, &CMainFrame::OnUpdateSpotViewSulfur )
	ON_COMMAND( IDM_SPOT_VIEW_MANDRAKE, &CMainFrame::OnSpotViewMandrake )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_MANDRAKE, &CMainFrame::OnUpdateSpotViewMandrake )
	ON_COMMAND( IDM_SPOT_VIEW_NPC, &CMainFrame::OnSpotViewNpc )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_NPC, &CMainFrame::OnUpdateSpotViewNpc )
	ON_COMMAND( IDM_SPOT_VIEW_EVENT, &CMainFrame::OnSpotViewEvent )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_EVENT, &CMainFrame::OnUpdateSpotViewEvent )
	ON_COMMAND( IDM_SPOT_VIEW_JEWEL, &CMainFrame::OnSpotViewJewel )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_JEWEL, &CMainFrame::OnUpdateSpotViewJewel )
	ON_COMMAND( IDM_DEL_SPOT, &CMainFrame::OnDelSpot )
	ON_COMMAND( IDM_DEL_NODE, &CMainFrame::OnDelNode )
	ON_UPDATE_COMMAND_UI( IDM_DEL_NODE, &CMainFrame::OnUpdateDelNode )
	ON_COMMAND( IDM_VIEW_QUEST, &CMainFrame::OnViewQuest )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_QUEST, &CMainFrame::OnUpdateViewQuest )
	ON_COMMAND( IDM_UPDATE, &CMainFrame::OnUpdate )
	ON_COMMAND( IDM_VIEW_DAMAGE, &CMainFrame::OnViewDamage )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_DAMAGE, &CMainFrame::OnUpdateViewDamage )
	ON_COMMAND( IDM_VIEW_ATTACKED_DAMAGE, &CMainFrame::OnViewAttackedDamage )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_ATTACKED_DAMAGE, &CMainFrame::OnUpdateViewAttackedDamage )
	ON_COMMAND( IDM_HERO_IMMORTAL, &CMainFrame::OnHeroImmortal )
	ON_UPDATE_COMMAND_UI( IDM_HERO_IMMORTAL, &CMainFrame::OnUpdateHeroImmortal )
	ON_COMMAND( IDM_VIEW_SQUAD_INFO, &CMainFrame::OnViewSquadInfo )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_SQUAD_INFO, &CMainFrame::OnUpdateViewSquadInfo )
	ON_COMMAND( IDM_VIEW_ENEMY_HERO_LEVEL, &CMainFrame::OnViewEnemyHeroLevel )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_ENEMY_HERO_LEVEL, &CMainFrame::OnUpdateViewEnemyHeroLevel )
	ON_COMMAND( IDM_UNIT_IMMOTAL, &CMainFrame::OnUnitImmotal )
	ON_UPDATE_COMMAND_UI( IDM_UNIT_IMMOTAL, &CMainFrame::OnUpdateUnitImmotal )
	ON_COMMAND( IDM_CREATE_HERO, &CMainFrame::OnCreateHero )
	ON_COMMAND( IDM_XUZHU_MODE, &CMainFrame::OnXuzhuMode )
	ON_UPDATE_COMMAND_UI( IDM_XUZHU_MODE, &CMainFrame::OnUpdateXuzhuMode )
	ON_COMMAND( IDM_DISCONNECT, &CMainFrame::OnDisconnect )
	ON_COMMAND( IDM_GOTO_HOME, &CMainFrame::OnGotoHome )
	ON_COMMAND( IDM_RETURN_FROM_HOME, &CMainFrame::OnReturnFromHome )
	ON_COMMAND( IDM_GOTO_START, &CMainFrame::OnGotoStart )
	ON_COMMAND( IDM_RELOAD, &CMainFrame::OnReload )
	ON_COMMAND( ID_RELOAD_CMD, &CMainFrame::OnReloadCmd )
	ON_COMMAND( IDM_SPOT_VIEW_BUILDING, &CMainFrame::OnSpotViewBuilding )
	ON_UPDATE_COMMAND_UI( IDM_SPOT_VIEW_BUILDING, &CMainFrame::OnUpdateSpotViewBuilding )
	ON_COMMAND( IDM_UPDATE_QUEST, &CMainFrame::OnUpdateQuest )
	ON_COMMAND( IDM_RESTART_SEQ, &CMainFrame::OnRestartSeq )
	ON_COMMAND( IDM_CLOSE_AREA, &CMainFrame::OnCloseArea )
	ON_COMMAND( IDM_DEL_SEQ, &CMainFrame::OnDelSeq )
	ON_COMMAND( IDM_RESET_QUEST, &CMainFrame::OnResetQuest )
	ON_COMMAND( IDM_TEST_MAIL, &CMainFrame::OnTestMail )
	ON_COMMAND( IDM_MODIFY_TEXT, &CMainFrame::OnModifyText )
	ON_COMMAND( IDM_MERGE_TEXT, &CMainFrame::OnMergeText )
	ON_COMMAND( IDM_EXTRACT_HANGUL, &CMainFrame::OnExtractHangul )
	ON_COMMAND( IDM_OMISSION_TEXT, &CMainFrame::OnOmissionText )
	ON_COMMAND( IDM_FILL_TEXT, &CMainFrame::OnFillEmtpyText )
	ON_COMMAND( IDM_RESAVE_PROP_HELP, &CMainFrame::OnResavePropHelp )
	ON_COMMAND( IDM_VIEW_BOUNDBOX_SPOT, &CMainFrame::OnViewBoundboxSpot )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_BOUNDBOX_SPOT, &CMainFrame::OnUpdateViewBoundboxSpot )
	ON_COMMAND( IDM_FINISH_SEQ, &CMainFrame::OnFinishSeq )
	ON_COMMAND( IDM_PLAYX1, &CMainFrame::OnPlayx1 )
	ON_COMMAND( IDM_PLAYX2, &CMainFrame::OnPlayx2 )
	ON_COMMAND( IDM_PLAYX4, &CMainFrame::OnPlayx4 )
	ON_COMMAND( IDM_PLAYX8, &CMainFrame::OnPlayx8 )
	ON_UPDATE_COMMAND_UI( IDM_PLAYX1, &CMainFrame::OnUpdatePlayx1 )
	ON_UPDATE_COMMAND_UI( IDM_PLAYX2, &CMainFrame::OnUpdatePlayx2 )
	ON_UPDATE_COMMAND_UI( IDM_PLAYX4, &CMainFrame::OnUpdatePlayx4 )
	ON_UPDATE_COMMAND_UI( IDM_PLAYX8, &CMainFrame::OnUpdatePlayx8 )
	ON_COMMAND( IDM_VIEW_AREA_LABEL, &CMainFrame::OnViewAreaLabel )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_AREA_LABEL, &CMainFrame::OnUpdateViewAreaLabel )
	ON_COMMAND( IDM_VIEW_SQUADS, &CMainFrame::OnViewSquads )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_SQUADS, &CMainFrame::OnUpdateViewSquads )
	ON_COMMAND( IDM_PLAY_PAUSE, &CMainFrame::OnPlayPause )
	ON_UPDATE_COMMAND_UI( IDM_PLAY_PAUSE, &CMainFrame::OnUpdatePlayPause )
	ON_COMMAND( IDM_PLAY, &CMainFrame::OnPlay )
	ON_UPDATE_COMMAND_UI( IDM_PLAY, &CMainFrame::OnUpdatePlay )
	ON_COMMAND( IDM_KEY_BACK, &CMainFrame::OnKeyBack )
	ON_COMMAND( IDM_STOP_PASSIVE, &CMainFrame::OnStopPassive )
	ON_UPDATE_COMMAND_UI( IDM_STOP_PASSIVE, &CMainFrame::OnUpdateStopPassive )
	ON_COMMAND( IDM_FIL_HERO, &CMainFrame::OnFilHero )
	ON_UPDATE_COMMAND_UI( IDM_FIL_HERO, &CMainFrame::OnUpdateFilHero )
	ON_COMMAND( IDM_FIL_UNIT, &CMainFrame::OnFilUnit )
	ON_UPDATE_COMMAND_UI( IDM_FIL_UNIT, &CMainFrame::OnUpdateFilUnit )
	ON_COMMAND( IDM_STOP_ACTIVE, &CMainFrame::OnStopActive )
	ON_UPDATE_COMMAND_UI( IDM_STOP_ACTIVE, &CMainFrame::OnUpdateStopActive )
	ON_COMMAND( IDM_FILTER_PLAYER, &CMainFrame::OnFilterPlayer )
	ON_UPDATE_COMMAND_UI( IDM_FILTER_PLAYER, &CMainFrame::OnUpdateFilterPlayer )
	ON_COMMAND( IDM_FILTER_ENEMY, &CMainFrame::OnFilterEnemy )
	ON_UPDATE_COMMAND_UI( IDM_FILTER_ENEMY, &CMainFrame::OnUpdateFilterEnemy )
	ON_COMMAND( IDM_VIEW_CUT_SCENE, &CMainFrame::OnViewCutScene )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_CUT_SCENE, &CMainFrame::OnUpdateViewCutScene )
	ON_COMMAND( IDM_VIEW_MEMORY_INFO, &CMainFrame::OnViewMemoryInfo )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_MEMORY_INFO, &CMainFrame::OnUpdateViewMemoryInfo )
	ON_COMMAND( IDM_VIEW_FRAME_RATE, &CMainFrame::OnViewFrameRate )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_FRAME_RATE, &CMainFrame::OnUpdateViewFrameRate )
	ON_COMMAND( IDM_LAYER_TEST, &CMainFrame::OnLayerTest )
	ON_COMMAND( IDM_RELOAD_LAYOUT, &CMainFrame::OnReloadLayout )
	ON_WM_KEYDOWN()
	ON_COMMAND( IDM_VIEW_HEXA_IDX, &CMainFrame::OnViewHexaIdx )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_HEXA_IDX, &CMainFrame::OnUpdateViewHexaIdx )
	ON_COMMAND( IDM_VIEW_AREA_COST, &CMainFrame::OnViewAreaCost )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_AREA_COST, &CMainFrame::OnUpdateViewAreaCost )
	ON_COMMAND( IDM_MODE_BGOBJ_TOOL, &CMainFrame::OnModeBgObjTool )
	ON_UPDATE_COMMAND_UI( IDM_MODE_BGOBJ_TOOL, &CMainFrame::OnUpdateModeBgObjTool )
	ON_COMMAND( IDM_VIEW_BGOBJ_BOUNDBOX, &CMainFrame::OnViewBgObjBoundBox )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_BGOBJ_BOUNDBOX, &CMainFrame::OnUpdateViewBgObjBoundBox )
	ON_COMMAND( IDM_VIEW_SPOT_INFO, &CMainFrame::OnViewSpotInfo )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_SPOT_INFO, &CMainFrame::OnUpdateViewSpotInfo )
	ON_COMMAND( IDM_PLAY_SOUND_EFFECT, &CMainFrame::OnPlaySoundEffect )
	ON_UPDATE_COMMAND_UI( IDM_PLAY_SOUND_EFFECT, &CMainFrame::OnUpdatePlaySoundEffect )
	ON_COMMAND( IDM_PLAY_BGM, &CMainFrame::OnPlayBGM )
	ON_UPDATE_COMMAND_UI( IDM_PLAY_BGM, &CMainFrame::OnUpdatePlayBGM )
	ON_COMMAND( IDM_VIEW_WIN_BOUND_BOX, &CMainFrame::OnViewWinBoundBox )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_WIN_BOUND_BOX, &CMainFrame::OnUpdateViewWinBoundBox )
	ON_COMMAND( IDM_VIEW_MOUSE_OVER_WINS, &CMainFrame::OnViewMouseOverWins )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_MOUSE_OVER_WINS, &CMainFrame::OnUpdateViewMouseOverWins )
	ON_COMMAND( IDM_FLUSH_CASH_IMG, &CMainFrame::OnFlushCashImg )
	ON_COMMAND( IDM_TEST_PARTICLE, &CMainFrame::OnTestParticle )
	ON_COMMAND( IDM_RELOAD_PARTICLE, &CMainFrame::OnReloadParticle )
	ON_COMMAND( IDM_MODE_TEST_PARTICLE, &CMainFrame::OnModeTestParticle )
	ON_UPDATE_COMMAND_UI( IDM_MODE_TEST_PARTICLE, &CMainFrame::OnUpdateModeTestParticle )
	ON_COMMAND( IDM_CHEAT_COMMAND, &CMainFrame::OnCheatCommand )
	ON_COMMAND( IDM_VIEW_DAMAGE_LOGGING, &CMainFrame::OnViewDamageLogging )
	ON_UPDATE_COMMAND_UI( IDM_VIEW_DAMAGE_LOGGING, &CMainFrame::OnUpdateOnViewDamageLogging )
	ON_COMMAND( IDC_BATTLE_OPTION, &CMainFrame::OnBattleOption )
	ON_COMMAND( IDM_RELOAD_CONSTANT_BY_RETRY, &CMainFrame::OnReloadWhenRetryConstant )
	ON_UPDATE_COMMAND_UI( IDM_RELOAD_CONSTANT_BY_RETRY, &CMainFrame::OnUpdateReloadWhenRetryConstant )
	ON_COMMAND( IDM_RELOAD_PROPUNIT_BY_RETRY, &CMainFrame::OnReloadWhenRetryPropUnit )
	ON_UPDATE_COMMAND_UI( IDM_RELOAD_PROPUNIT_BY_RETRY, &CMainFrame::OnUpdateReloadWhenRetryPropUnit )
	ON_COMMAND( IDM_RELOAD_PROPSKILL_BY_RETRY, &CMainFrame::OnReloadWhenRetryPropSkill )
	ON_UPDATE_COMMAND_UI( IDM_RELOAD_PROPSKILL_BY_RETRY, &CMainFrame::OnUpdateReloadWhenRetryPropSkill )
	ON_COMMAND( IDM_RELOAD_PROPLEGION_BY_RETRY, &CMainFrame::OnReloadWhenRetryPropLegion )
	ON_UPDATE_COMMAND_UI( IDM_RELOAD_PROPLEGION_BY_RETRY, &CMainFrame::OnUpdateReloadWhenRetryPropLegion )
	ON_COMMAND( ID_PROFILETEST_NODRAW, &CMainFrame::OnProfileTestNoDraw )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_NODRAW, &CMainFrame::OnUpdateProfileTestNoDraw )
	ON_COMMAND( ID_PROFILETEST_NODP, &CMainFrame::OnProfileTestNoDP )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_NODP, &CMainFrame::OnUpdateProfileTestNoDP )
	ON_COMMAND( ID_PROFILETEST_NOTEXTURE, &CMainFrame::OnProfileTestNoTexture )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_NOTEXTURE, &CMainFrame::OnUpdateProfileTestNoTexture )
	ON_COMMAND( ID_PROFILETEST_SMALLTEXTURE, &CMainFrame::OnProfileTestSmallTex )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_SMALLTEXTURE, &CMainFrame::OnUpdateProfileTestSmallTex )
	ON_COMMAND( ID_PROFILETEST_NOFONTDRAW, &CMainFrame::OnProfileTestNoFontDraw )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_NOFONTDRAW, &CMainFrame::OnUpdateProfileTestNoFontDraw )
	ON_COMMAND( ID_PROFILETEST_NODRAWBAR, &CMainFrame::OnProfileTestNoDrawBar )
	ON_UPDATE_COMMAND_UI( ID_PROFILETEST_NODRAWBAR, &CMainFrame::OnUpdateProfileTestNoDrawBar )
	ON_COMMAND( IDM_PROFILING, &CMainFrame::OnProfiling )
	ON_UPDATE_COMMAND_UI( IDM_PROFILING, &CMainFrame::OnUpdateProfiling )
	ON_COMMAND( IDM_TEST_SCENE, &CMainFrame::OnGotoTestScene )
END_MESSAGE_MAP()



static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	g_pMainFrm = this;
}

CMainFrame::~CMainFrame()
{
	g_pMainFrm = nullptr;
	TRACE("~CMainFrame()\n");
}

void CMainFrame::SetTitle( void )
{
	CString strHeader = _T("XESample - ");
	CString strTitle;
	strTitle += __DATE__;
	strTitle += _T(" ");
	strTitle += __TIME__;
	XGame::s_strVerBuild = strTitle;
	SetWindowText( (LPCTSTR)(strHeader + strTitle) ); 
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef _VER_OPENGL
#else
	XE_MFCFramework::Create( GetSafeHwnd(), AfxGetInstanceHandle() );		// d3d9 초기화
#endif // _VER_OPENGL

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// wndConsole
	if( !m_wndConsole.Create( this,
														&m_dlgConsole,
														CString( "Console" ),
														IDD_CONSOLE,
														IDM_VIEW_CONSOLE,
														WS_CHILD | WS_VISIBLE | CBRS_RIGHT ) ) {
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndConsole.SetBarStyle( m_wndConsole.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndConsole.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar(&m_wndConsole );
	//FloatControlBar( &m_wndConsole, CPoint(100,600) );

	// wndPropGrid
#ifdef _XPROP_GRID
	if( !m_wndPropGrid.Create( this, &m_dlgPropGrid, CString( "PropGrid" ), IDD_PROPGRID, IDM_VIEW_PROPGRID, WS_CHILD | WS_VISIBLE | CBRS_RIGHT ) )
	{
		TRACE0( "Failed to create dialogbar\n" );
		return -1;      // fail to create
	}
	m_wndPropGrid.SetBarStyle( m_wndPropGrid.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndPropGrid.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &m_wndPropGrid );
#endif // _XPROP_GRID

	SetTitle();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	int scrW = GetSystemMetrics( SM_CXSCREEN );
	int scrH = GetSystemMetrics( SM_CYSCREEN );

	// MainFrm의 초기위치를 지정
	float ratio = (float)PHY_HEIGHT / PHY_WIDTH;
	cs.cx = PHY_WIDTH;	// 창크기
	if( scrH <= 1050 )
		cs.cx = 500;
	else
	if( scrH <= PHY_HEIGHT )
		cs.cx = 600;
	cs.cy = (int)(cs.cx * ratio);// 430 * 2;
// 	cs.cx += 20;
// 	cs.cy += 217;		// 1280x720일 경우
	// 892,831
	cs.cx += 408;
	cs.cy += 106;		// 1280x720/콘솔창 오른쪽
	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기
void CMainFrame::OnCheatMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->OnCheatMode();
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->SetbUpdate( TRUE );
	GAME->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateCheatMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugMode )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

/**
 @brief 구름툴 모드 선택
*/
void CMainFrame::OnToolMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_CLOUD );
// 	if( SCENE_WORLD )
// 	{
// 		XAPP->m_ToolMode = xTM_CLOUD;
// 		SCENE_WORLD->OnModeTool( XAPP->m_ToolMode );
// 	}
#endif // _xIN_TOOL
}


void CMainFrame::OnUpdateToolMode( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( XBaseTool::sGetCurrMode() == xTM_CLOUD );
// 	if( SCENE_WORLD )
// 	{
// 		if( XAPP->m_ToolMode == xTM_CLOUD )
// 			pCmdUI->SetCheck( 1 );
// 		else
// 			pCmdUI->SetCheck( 0 );
// 	}
#endif // _xIN_TOOL
}


void CMainFrame::OnNextCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// #ifdef _xIN_TOOL
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->OnNextCloud();
// #endif // _xIN_TOOL
}


void CMainFrame::OnPrevCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// #ifdef _xIN_TOOL
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->OnPrevCloud();
// #endif // _xIN_TOOL
}


void CMainFrame::OnSelectCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// #ifdef _xIN_TOOL
// 	if( SCENE_WORLD )
// 	{
// //		SCENE_WORLD->OnSelectCloudMode();
// 		SCENE_WORLD->OnSelectCancel();
// 	}
// #endif // _xIN_TOOL
}


void CMainFrame::OnViewCloudLayer()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SCENE_WORLD )
		SCENE_WORLD->OnViewCloudLayer();
}


void CMainFrame::OnUpdateViewCloudLayer( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_WORLD )
	{
		if( SCENE_WORLD->GetbViewCloudLayer() )
			pCmdUI->SetCheck( 1 );
		else
			pCmdUI->SetCheck( 0 );
		
	}
}


void CMainFrame::OnDebugViewSquadRadius()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewSquadRadius = !XAPP->m_bDebugViewSquadRadius;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateDebugViewSquadRadius( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewSquadRadius )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );

}


void CMainFrame::OnDebug1vs1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugSquad = !XAPP->m_bDebugSquad;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateDebug1vs1( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugSquad )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnDebugViewRefcnt()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewRefCnt = !XAPP->m_bDebugViewRefCnt;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateDebugViewRefcnt( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewRefCnt )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnDebugViewUnitsn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewUnitSN = !XAPP->m_bDebugViewUnitSN;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateDebugViewUnitsn( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewUnitSN )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnDebugViewTarget()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewTarget = !XAPP->m_bDebugViewTarget;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateDebugViewTarget( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewTarget )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}

/**
 @brief 유닛수 랜덤 설정
*/
void CMainFrame::OnNumUnitRandom()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugRespawnNumRandom = !XAPP->m_bDebugRespawnNumRandom;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateNumUnitRandom( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugRespawnNumRandom )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUnitRandom()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nDebugRespawnUnit = 0;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateUnitRandom( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_nDebugRespawnUnit == 0 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUnitAllSmall()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nDebugRespawnUnit = 1;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateUnitAllSmall( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_nDebugRespawnUnit == 1 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUnitAllMiddle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nDebugRespawnUnit = 2;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateUnitAllMiddle( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_nDebugRespawnUnit == 2 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUnitAllBig()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nDebugRespawnUnit = 3;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateUnitAllBig( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_nDebugRespawnUnit == 3 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnReloadSkill()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnViewBuff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewBuff = !XAPP->m_bDebugViewBuff;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewBuff( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewBuff )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnSlowFrame()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugSlowFrame = !XAPP->m_bDebugSlowFrame;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateSlowFrame( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugSlowFrame )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnSetFrameskip()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->SetbFrameSkip( !XAPP->GetbFrameSkip() );
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateSetFrameskip( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->GetbFrameSkip() )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnBattle11()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgBattle11 dlg;
	dlg.m_strHero1 = XGC->m_strHero1.c_str();
	dlg.m_strHero2 = XGC->m_strHero2.c_str();
	dlg.m_radioPlayer = XGC->m_unitLeft -  1;
	dlg.m_radioAI = XGC->m_unitRight - 1;
	dlg.m_lvSquadLeft = XGC->m_lvSquadLeft;
	dlg.m_lvSquadRight = XGC->m_lvSquadRight;
	dlg.m_lvHero1 = XGC->m_lvHero1;
	dlg.m_lvHero2 = XGC->m_lvHero2;
	if( dlg.DoModal() == IDOK )
	{
		XBREAK( dlg.m_radioPlayer < -1 || dlg.m_radioPlayer > 8 );
		XBREAK( dlg.m_radioAI < -1 || dlg.m_radioAI > 8 );
// 		XBREAK( dlg.m_strHero1.IsEmpty() );
// 		XBREAK( dlg.m_strHero2.IsEmpty() );
		XGlobalConst::sGetMutable()->m_strHero1 = dlg.m_strHero1;
		XGlobalConst::sGetMutable()->m_strHero2 = dlg.m_strHero2;
		if( XGlobalConst::sGetMutable()->m_strHero1.empty() )
			XGlobalConst::sGetMutable()->m_strHero1 = _T("random");
		if( XGlobalConst::sGetMutable()->m_strHero2.empty() )
			XGlobalConst::sGetMutable()->m_strHero2 = _T( "random" );
		XGlobalConst::sGetMutable()->m_unitLeft = (XGAME::xtUnit)(dlg.m_radioPlayer + 1);
		XGlobalConst::sGetMutable()->m_unitRight = ( XGAME::xtUnit )( dlg.m_radioAI + 1 );
		XGlobalConst::sGetMutable()->m_lvSquadLeft = dlg.m_lvSquadLeft;
		XGlobalConst::sGetMutable()->m_lvSquadRight = dlg.m_lvSquadRight;
		XGlobalConst::sGetMutable()->m_lvHero1 = dlg.m_lvHero1;
		XGlobalConst::sGetMutable()->m_lvHero2 = dlg.m_lvHero2;
		XAPP->XClientMain::SaveCheat();
	}
}


void CMainFrame::OnViewBoundBox()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewBoundBox = !XAPP->m_bDebugViewBoundBox;
	XAPP->XClientMain::SaveCheat();
	//
//	CDlgPropGrid::sGet()->UpdateData( 1 );
}


void CMainFrame::OnUpdateViewBoundBox( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewBoundBox )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewSquadSn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewSquadSN = !XAPP->m_bDebugViewSquadSN;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewSquadSn( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewSquadSN )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnCreateItem()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgCreateItem dlg;
	if( dlg.DoModal() == IDOK )
	{
		XPropItem::xPROP *pProp = PROP_ITEM->GetpProp( dlg.m_strIdentifier );
		if( pProp == nullptr )
			pProp = PROP_ITEM->GetpPropFromName( dlg.m_strIdentifier );
		if( pProp )
		{
			GAMESVR_SOCKET->SendReqCheatCreateItem( GAME, pProp, dlg.m_numItem );
			//
		}
		else
			XALERT( "%s의 아이템을 찾을 수 없습니다.", dlg.m_strIdentifier );
	}
}


void CMainFrame::OnViewHp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewHp = !XAPP->m_bDebugViewHp;
	XAPP->XClientMain::SaveCheat();
	//
//	CDlgPropGrid::sGet()->UpdateData( 0 );

}


void CMainFrame::OnUpdateViewHp( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewHp )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnSelectCancel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// #ifdef _xIN_TOOL
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->OnSelectCancel();
// #endif // _xIN_TOOL
}


void CMainFrame::OnEditCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr )
		return;
	ID idSelected = SCENE_WORLD->GetidSelected();
	if( idSelected == 0 )
		return;
	XPropCloud::xCloud *pProp = PROP_CLOUD->GetpProp( idSelected );
	if( XASSERT(pProp) )
	{
		XDlgPropCloud dlg;
		dlg.m_idCloud = pProp->idCloud;
		dlg.m_idsArea = pProp->strIdentifier.c_str();
		dlg.m_idName = pProp->idName;
// 		dlg.m_xPos = (int)pProp->vwPos.x;
// 		dlg.m_yPos = (int)pProp->vwPos.y;
		dlg.m_lvOpenable = pProp->lvArea;
		dlg.m_Cost = pProp->cost;
		dlg.m_xPosCost = (int)pProp->vAdjust.x;
		dlg.m_yPosCost = (int)pProp->vAdjust.y;
		dlg.m_idsPrecede = pProp->idsPrecedeArea.c_str();
		dlg.m_idsItem = pProp->idsItem.c_str();
		if( dlg.DoModal() == IDOK )
		{
			pProp->idCloud = dlg.m_idCloud;
			pProp->strIdentifier = dlg.m_idsArea;
			if( pProp->strIdentifier.empty() )
				pProp->strIdentifier = XFORMAT("area.%d", pProp->idCloud );
			pProp->idName = dlg.m_idName;
// 			pProp->vwPos.x = (float)dlg.m_xPos;
// 			pProp->vwPos.y = (float)dlg.m_yPos;
			pProp->lvArea = dlg.m_lvOpenable;
			pProp->cost = dlg.m_Cost;
			pProp->vAdjust.x = (float)dlg.m_xPosCost;
			pProp->vAdjust.y = (float)dlg.m_yPosCost;
			pProp->idsPrecedeArea = dlg.m_idsPrecede;
			// 선행지역이 없는곳이면 경고.
			if( !pProp->idsPrecedeArea.empty() ) {
				auto pPrecede = PROP_CLOUD->GetpProp( pProp->idsPrecedeArea );
				if( pPrecede == nullptr ) {
					AfxMessageBox( XFORMAT( "%s라는 지역은 없습니다.", pProp->idsPrecedeArea.c_str() ) );
					pProp->idsPrecedeArea.clear();
				}
			}
			pProp->idsItem = dlg.m_idsItem;
			if( !pProp->idsItem.empty() ) {
				auto pPropItem = PROP_ITEM->GetpProp( pProp->idsItem );
				if( pProp == nullptr ) {
					AfxMessageBox( XFORMAT( "%s라는 아이템은 없습니다.", pProp->idsItem.c_str() ) );
					pProp->idsItem.clear();
				}
			}
			pProp->AddIdx( SCENE_WORLD->GetidxClickedHexa() );
			PROP_CLOUD->UpdateAreaIds( pProp );
			SCENE_WORLD->EditedCloud();		// auto save
			SCENE_WORLD->UpdateCloudSpotList( true );
		}
	}
#endif // _xIN_TOOL


}


void CMainFrame::OnViewHexaLine()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bToolViewHexaLine = !XAPP->m_bToolViewHexaLine;
	XAPP->XClientMain::SaveCheat();

}


void CMainFrame::OnUpdateViewHexaLine( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr /*|| XAPP->IsToolCloudMode() == FALSE*/ ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	if( XAPP->m_bToolViewHexaLine )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#endif // _xIN_TOOL
}


void CMainFrame::OnRandomCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	PROP_CLOUD->DoReArrangeCloudRandom();
}


void CMainFrame::OnModeTechTool()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_TECH );
// 	XAPP->m_ToolMode = xTM_TECH;
// 	if( SCENE_TECH )
// 		SCENE_TECH->OnModeTool( XAPP->m_ToolMode );
#endif // _xIN_TOOL
}


void CMainFrame::OnUpdateModeTechTool( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	// 테크신이 아니면 비활성
#ifdef _xIN_TOOL
	if( SCENE_TECH == nullptr ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( XBaseTool::sIsToolTechMode() );
// 	if( SCENE_TECH == nullptr )
// 	{
// 		pCmdUI->Enable( FALSE );
// 		return;
// 	}
// 	if( XAPP->m_ToolMode == xTM_TECH )
// 		pCmdUI->SetCheck( 1 );
// 	else
// 		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


#ifdef _xIN_TOOL
void CMainFrame::OnCreateAbil()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CaribeView에서 처리함.
}


void CMainFrame::OnAbilToolMoveMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SCENE_TECH == nullptr )
		return;
	SCENE_TECH->SetmodeTool( XSceneTech::xMODE_MOVE );
}


void CMainFrame::OnUpdateAbilToolMoveMode( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_TECH == nullptr || !XBaseTool::sIsToolTechMode() ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( SCENE_TECH->GetmodeTool() == XSceneTech::xMODE_MOVE );
// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH )
// 	{
// 		pCmdUI->Enable( FALSE );
// 		return;
// 	}
// 	if( SCENE_TECH->GetmodeTool() == XSceneTech::xMODE_MOVE )
// 		pCmdUI->SetCheck( 1 );
// 	else
// 		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnAbilToolLinkMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SCENE_TECH == nullptr )
		return;
	SCENE_TECH->SetmodeTool( XSceneTech::xMODE_LINK );
}


void CMainFrame::OnUpdateAbilToolLinkMode( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_TECH == nullptr || !XBaseTool::sIsToolTechMode() ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( SCENE_TECH->GetmodeTool() == XSceneTech::xMODE_LINK );
// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH )
// 	{
// 		pCmdUI->Enable( FALSE );
// 		return;
// 	}
// 	if( SCENE_TECH->GetmodeTool() == XSceneTech::xMODE_LINK )
// 		pCmdUI->SetCheck( 1 );
// 	else
// 		pCmdUI->SetCheck( 0 );
}
#endif // _xIN_TOOL


void CMainFrame::OnToolShowCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nToolShowCloud = 2;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnToolHideCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nToolShowCloud = 0;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateToolShowCloud( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_WORLD == nullptr /*||
		( SCENE_WORLD && SCENE_WORLD->GetbModeTool() == FALSE )*/ )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	if( XAPP->m_nToolShowCloud == 2 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUpdateToolHideCloud( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_WORLD == nullptr /*||
		( SCENE_WORLD && SCENE_WORLD->GetbModeTool() == FALSE )*/ )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	if( XAPP->m_nToolShowCloud == 0 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnToolAlphaCloud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_nToolShowCloud = 1;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateToolAlphaCloud( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SCENE_WORLD == nullptr /*||
		( SCENE_WORLD && SCENE_WORLD->GetbModeTool() == FALSE )*/ )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	if( XAPP->m_nToolShowCloud == 1 )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnModeSpotTool()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_SPOT );
// 	XAPP->m_ToolMode = xTM_SPOT;
//  	if( SCENE_WORLD )
//  		SCENE_WORLD->OnModeTool( XAPP->m_ToolMode );
#endif // _xIN_TOOL
}


void CMainFrame::OnUpdateModeSpotTool( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( XBaseTool::sIsToolSpotMode() );
// 	if( SCENE_WORLD == nullptr )
// 	{
// 		pCmdUI->Enable( FALSE );
// 		return;
// 	}
// 	if( XAPP->m_ToolMode == xTM_SPOT )
// 		pCmdUI->SetCheck( 1 );
// 	else
// 		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}

void CMainFrame::OnModeBgObjTool()
{
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_OBJ );
// 	XAPP->m_ToolMode = xTM_OBJ;
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->OnModeTool( XAPP->m_ToolMode );
#endif 
}

void CMainFrame::OnUpdateModeBgObjTool( CCmdUI *pCmdUI )
{
#ifdef _xIN_TOOL
	if( SCENE_WORLD == nullptr ) {
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->SetCheck( XBaseTool::sIsToolBgObjMode() );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL

		
}


void CMainFrame::OnGameMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_NONE );
// 	XAPP->m_ToolMode = xTM_NONE;
// 	if( SCENE_TECH )
// 		SCENE_TECH->OnModeTool( XAPP->m_ToolMode );
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->OnModeTool( XAPP->m_ToolMode );
#endif // _xIN_TOOL
}


void CMainFrame::OnUpdateGameMode( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	pCmdUI->SetCheck( !XBaseTool::sIsToolMode() );
// 	if( XAPP->m_ToolMode == xTM_NONE )
// 		pCmdUI->SetCheck( 1 );
// 	else
// 		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnSpotAllView()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_NONE;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotAllView( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnSpotViewCastle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_CASTLE;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewCastle( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_CASTLE )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}

void CMainFrame::OnSpotViewJewel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_JEWEL;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewJewel( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_JEWEL )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}

void CMainFrame::OnSpotViewSulfur()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_SULFUR;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewSulfur( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_SULFUR )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnSpotViewMandrake()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_MANDRAKE;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewMandrake( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_MANDRAKE )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnSpotViewNpc()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_NPC;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewNpc( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_NPC )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnSpotViewEvent()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_WHEN;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdateSpotViewEvent( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_WHEN )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}

void CMainFrame::OnSpotViewBuilding()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_modeToolSpotView = XGAME::xSPOT_COMMON;
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}

void CMainFrame::OnUpdateSpotViewBuilding( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	if( XAPP->m_modeToolSpotView == XGAME::xSPOT_COMMON )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
#else
	pCmdUI->Enable( FALSE );
#endif // _xIN_TOOL
}


void CMainFrame::OnDelSpot()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifdef _xIN_TOOL
	// 이거 사용하지 말고 OnDelNode로 통합
	if( SCENE_WORLD )
		SCENE_WORLD->DelSpotT();
#endif // _xIN_TOOL
}

void CMainFrame::OnDelNode()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH ||
// 		( SCENE_TECH && SCENE_TECH->m_pSelNodeT == nullptr ) )
// 	{
// 		return;
// 	}
	if( XBaseTool::sIsToolTechMode() && //XAPP->m_ToolMode == xTM_TECH &&
		(SCENE_TECH && SCENE_TECH->m_pSelNodeT ) )
		SCENE_TECH->DelNodeSelected();
	else
	if( XBaseTool::sIsToolSpotMode() && // XAPP->m_ToolMode == xTM_SPOT &&
		SCENE_WORLD && XWndSpotForTool::s_pSelected )
		SCENE_WORLD->DelSpotT();
#endif // _xIN_TOOL
}

void CMainFrame::OnUpdateDelNode( CCmdUI *pCmdUI )
{
	// TODO: ���⿡ ���� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
#ifdef _xIN_TOOL
	// 	if( SCENE_TECH == nullptr || XAPP->m_ToolMode != xTM_TECH ||
	// 		( SCENE_TECH && SCENE_TECH->m_pSelNodeT == nullptr ) )
	// 	{
	// 		pCmdUI->Enable( FALSE );
	// 		return;
	// 	}
	pCmdUI->Enable( TRUE );
#endif // _xIN_TOOL
}

void CMainFrame::OnViewQuest()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugQuestList = !XAPP->m_bDebugQuestList;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewQuest( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugQuestList )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}


void CMainFrame::OnUpdate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	if( SCENE_WORLD )
// 		SCENE_WORLD->SetbUpdate( TRUE );
	GAME->SetbUpdate( TRUE );
}


void CMainFrame::OnViewDamage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewDamage = !XAPP->m_bDebugViewDamage;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewDamage( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewDamage )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewAttackedDamage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewAttackedDamage = !XAPP->m_bDebugViewAttackedDamage;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewAttackedDamage( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewAttackedDamage )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnHeroImmortal()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugHeroImmortal = !XAPP->m_bDebugHeroImmortal;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateHeroImmortal( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugHeroImmortal )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewSquadInfo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewSquadInfo = !XAPP->m_bDebugViewSquadInfo;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewSquadInfo( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewSquadInfo )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewEnemyHeroLevel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewHeroLevel = !XAPP->m_bDebugViewHeroLevel;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewEnemyHeroLevel( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewHeroLevel )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnUnitImmotal()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugUnitImmortal = !XAPP->m_bDebugUnitImmortal;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateUnitImmotal( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugUnitImmortal )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnCreateHero()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgCreateHero dlg;
	if( dlg.DoModal() == IDOK )
	{
		auto pProp = PROP_HERO->GetpProp( dlg.m_strIdentifier );
		if( pProp == nullptr )
			pProp = PROP_HERO->GetpPropFromName( dlg.m_strIdentifier );
		if( pProp )
		{
			GAMESVR_SOCKET->SendCheat( GAME, 8, pProp->idProp, dlg.m_Num );
			//
		}
		else
			XALERT( "%s의 영웅을 찾을 수 없습니다.", dlg.m_strIdentifier );
	}
}


void CMainFrame::OnXuzhuMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GAME->ToggleXuzhuMode();

}


void CMainFrame::OnUpdateXuzhuMode( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if( ACCOUNT && ACCOUNT->m_bXuzhuMode )
// 		pCmdUI->SetCheck( 1 );
// 	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnDisconnect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( GAMESVR_SOCKET )
		GAMESVR_SOCKET->DoDisconnect();
	if( LOGINSVR_SOCKET )
		LOGINSVR_SOCKET->DoDisconnect();
	XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
}

/**
 @brief 홈으로 나가기
*/
void CMainFrame::OnGotoHome()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XE::OnPauseHandler();
}

/**
 @brief 홈에서 돌아오기
*/
void CMainFrame::OnReturnFromHome()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XE::OnResumeHandler();
}


void CMainFrame::OnGotoStart()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GAME->OnGotoStart( nullptr, 0, 0 );
//   if( GAMESVR_SOCKET )
//     GAMESVR_SOCKET->DoDisconnect();
// //  XConnector::sGet()->StopFSM();
//   GAME->GetpScene()->DoExit( XGAME::xSC_START );
}


void CMainFrame::OnReload()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GAME->OnReload();
}

void CMainFrame::OnReloadSoundTbl()
{
	SOUND_TBL->Reload();	
}

void CMainFrame::OnReloadCmd()
{
//	static std::string s_strCmd;
	CDlgCheatCommand dlg;
	dlg.m_strCmd = C2SZ(XAPP->m_strReloadCmd); //C2SZ(s_strCmd);
	dlg.m_strDesc = _T("명령어:\n");
	dlg.m_strDesc += _T( "prop_legion" );
	if( dlg.DoModal() == IDOK ) {
		XAPP->m_strReloadCmd = SZ2C((LPCTSTR)dlg.m_strCmd);
		XAPP->XClientMain::SaveCheat();
		if( !XAPP->m_strReloadCmd.empty() ) {
			if( XAPP->m_strReloadCmd == "prop_legion" ) {
				GAME->LoadPropLegion();
			}
		}
	}
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


/**
 @brief 새로 업데이트되어 추가된 퀘가 있을때 현재 퀘목록에 반영시킨다.
*/
void CMainFrame::OnUpdateQuest()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GAMESVR_SOCKET->SendCheat( GAME, 15 );
}


 void CMainFrame::OnRestartSeq()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 CDlgRestartSeq dlg;
	 if( dlg.DoModal() == IDOK ) {
		 if( !dlg.m_strIds.IsEmpty() ) {
			 _tstring idsSeq = dlg.m_strIds;
			 std::string cidsSeq = SZ2C(idsSeq);
			 ACCOUNT->_ClearCompletedSeq( cidsSeq );
			 GAME->_DoCutScene( cidsSeq.c_str() );
			 GAMESVR_SOCKET->SendCheat( GAME, 16, 0, 0, 0, 0, idsSeq );
		 }
	 }
 }


 void CMainFrame::OnCloseArea()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 CDlgCloseArea dlg;
	 if( dlg.DoModal() == IDOK ) {
		 if( !dlg.m_strArea.IsEmpty() ) {
			 auto pProp = PROP_CLOUD->GetpProp( dlg.m_strArea );
			 if( pProp ) {
				 sGetpWorld()->DoCloseArea( pProp->idCloud );
				 GAMESVR_SOCKET->SendCheat( GAME, 17, pProp->idCloud );
			 } else {
				 XALERT("%s:not found area", dlg.m_strArea );
			 }
		 }
	 }
 }
 
 void CMainFrame::OnDelSeq()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 CDlgRestartSeq dlg;
	 if( dlg.DoModal() == IDOK ) {
		 if( !dlg.m_strIds.IsEmpty() ) {
			 _tstring idsSeq = dlg.m_strIds;
			 std::string cidsSeq = SZ2C( idsSeq );
			 ACCOUNT->_ClearCompletedSeq( cidsSeq );
			 GAMESVR_SOCKET->SendCheat( GAME, 16, 0, 0, 0, 0, idsSeq );
		 }
	 }
 }


 void CMainFrame::OnFinishSeq()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 if( GAME->m_pCurrSeq ) {
		 GAME->OnEndSeq( GAME->m_pCurrSeq );
		 GAME->m_pCurrSeq->SetbDestroy( true );
	 }
 }

 void CMainFrame::OnResetQuest()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 CDlgEnterName dlg;
	 if( dlg.DoModal() == IDOK ) {
		auto pProp = XQuestProp::sGet()->GetpProp( dlg.m_strName );
		if( pProp ) {
			ACCOUNT->GetpQuestMng()->_DelQuestByCompleted( pProp->idProp );
			GAMESVR_SOCKET->SendCheat( GAME, 19, pProp->idProp );
		} else  {
			XALERT("%s:없는 퀘스트", dlg.m_strName );
		}

	 }
 }


 void CMainFrame::OnTestMail()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 GAMESVR_SOCKET->SendCheat( GAME, 20 );
 }

 /**
	@brief 현재 text_ko와 text_en을 비교해서 ko엔 있는데 en에 없는 누락된 텍스트들을 가려낸다.
 */
//  void CMainFrame::ExtraceOmission( XTextTable* pTableKo, XTextTable* pTableEn
// 																, XList4<XTextTable::xText>* pOutList )
//  {
// 
//  }

 /**
	@brief text_en등의 외국어 파일에서 번역이 안되어 한글로 채워져있는 텍스트를 골라낸다.
 */
void CMainFrame::ExtractHangulText( XVector<XTextTable::xText>* pOutAry )
{
	XTextTable textTbl;
	_tstring resText = PATH_PROP("text_en.txt");
	if( XASSERT(textTbl.Load( resText )) ) {
		textTbl.GetHangulTextToAry( pOutAry, 2000 );
	}
}

void CMainFrame::OnExtractHangul()
{
	XVector<XTextTable::xText> aryHangul;
	ExtractHangulText( &aryHangul );
	XList4<XTextTable::xText> listHangul;
	for( auto& text : aryHangul ) {
		listHangul.Add( text );
	}
	listHangul.Sort( []( XTextTable::xText& text1, XTextTable::xText& text2 )->bool {
		return text1.idText < text2.idText;
	} );
	SaveTextList( _T("text_hangul.txt"), listHangul );
}

/**
 @brief ko엔 있으나 en엔 없는 텍스트들만 추려낸다.
*/
void CMainFrame::OnOmissionText()
{
	XList4<XTextTable::xText> listMiss;
	XTextTable textTblEn;
	_tstring resText = PATH_PROP( "text_en.txt" );
	if( XASSERT( textTblEn.Load( resText ) ) ) {
		XTextTable textTblKo;
		if( XASSERT( textTblKo.Load( PATH_PROP("text_ko.txt") ) ) ) {
			textTblKo.GetOmissionTextToList( &textTblEn, &listMiss );
		}
	}
	SaveTextList( _T( "text_missed.txt" ), listMiss );
}

/**
 @brief text_ko엔 있으나 en엔 없는 텍스트를 ko로부터 en에 채워넣는다.
*/
void CMainFrame::OnFillEmtpyText()
{
	XTextTable textTblEn;
	if( XASSERT( textTblEn.Load( PATH_PROP( "text_en.txt" ) ) ) ) {
		XTextTable textTblKo;
		if( XASSERT( textTblKo.Load( PATH_PROP( "text_ko.txt" ) ) ) ) {
			const int numFill = textTblEn.FillEmptyTextFromSrc( &textTblKo );
			CONSOLE("%d개의 텍스트를 채워넣었습니다.", numFill );
		}
	}
	textTblEn.Save();
}
/**
@brief 과거버전 번역문서와 비교해서 달라지거나 추가된 문장들만 따로 빼주는 툴
과거 번역당시의 구버전 텍스트파일명을 수정해주고 저장은 text_changed로 저장된다.
*/
void CMainFrame::OnModifyText()
{
	// 번역당시 텍스트와 현재 텍스트를 비교해서 변경되거나 추가된 텍스트만 골라내어 파일에 저장한다.
	// 번역당시의 옛날 text_ko를 읽음.
	static XList4<XTextTable::xText> listChanged;
	static std::unordered_map<ID, _tstring> mapChanged;
	XTextTable *pTableOld = new XTextTable();
	// 과거 번역시 사용했던 한글파일
	_tstring resText = PATH_PROP("text_ko_prev.txt");
	CONSOLE("과거 마지막 번역본 한글파일:%s", resText.c_str() );
	if( !pTableOld->Load( resText.c_str() ) ) {
		XALERT(" file not found: %s", resText.c_str() );
		return;
	}
	XTextTable textTblKo;
	if( XBREAK( !textTblKo.Load( PATH_PROP("text_ko.txt")) ) )
		return;
	// 현재의 한글테이블을 루프돈다.
	for( auto& elemSrc : textTblKo.GetmapText() ) {
		ID idText = elemSrc.first;
		auto& strTextCurr = elemSrc.second;
		if( idText >= 2000 || idText < 1000 ) {
			// 구버전파일에서 해당번호의 텍스트를 얻어온다.
			auto& mapOld = pTableOld->GetmapText();
//			 LPCTSTR pTextOld = mapOld[ idText ];
			auto itor = mapOld.find( idText );
			if( itor != mapOld.end() ) {
				const _tstring& strOld = itor->second;
				// 현재 텍스트와 번역텍스트를 비교해서 다르면 따로 추린다.
//			 if( !XE::IsSame( pTextCurr, pTextOld ) ) {
				if( strTextCurr != strOld ) {
					listChanged.push_back( XTextTable::xText(idText, strTextCurr) );
					mapChanged[ idText ] = strTextCurr;
				}
			} else {
				// 새로생긴 텍스트
				listChanged.push_back( XTextTable::xText(idText, strTextCurr) );
				mapChanged[ idText ] = strTextCurr;
			}
		}
	}
	// text_en에서 한글로된 텍스트(아직 번역안된)만 골라내어 추가한다. 중복된건 넣지 않는다.
	{
		XVector<XTextTable::xText> aryHangul;
		ExtractHangulText( &aryHangul );
		for( auto& text : aryHangul ) {
			// 중복검사
			auto itor = mapChanged.find( text.idText );
			if( itor == mapChanged.end() ) {
				// 리스트에 없는것만 추가로 넣는다.
				listChanged.push_back( text );
				mapChanged[ text.idText ] = text.strText;
			}
		}
	}

	listChanged.Sort( [](XTextTable::xText& text1, XTextTable::xText& text2)->bool {
		return text1.idText < text2.idText;
	});
	//
	SaveTextList( _T("text_changed.txt"), listChanged );
// 	{
// 		XResFile resFile;
// 		LPCTSTR szFull = XE::MakePackageFullPath( DIR_PROP, _T("text_changed.txt") );
// 		resFile.Open( szFull, XResFile::xWRITE );
// 		if( resFile.GetLastError() == XE::ERR_READONLY ||
// 			resFile.GetLastError() == XE::ERR_PERMISSION_DENIED )
// 		{
// 			XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( szFull ) );
// 			return;
// 		}
// 		TCHAR bom = 0xFEFF;
// 		resFile.Write( &bom, 2 );
// 		for( auto& text : listChanged ) {
// 			LPCTSTR szStr = XE::Format( _T( "%d\t\"%s\"\r\n" ), text.idText, text.strText.c_str() );
// 			int size = ( _tcslen( szStr ) ) * sizeof( TCHAR );	// 바이너리로 저장하므로 스트링뒤에 null은 저장하면 안된다.
// 			resFile.Write( (void*)szStr, size );
// 		}
// 	}
	// 뽑힌 텍스트를 엑셀로 읽어서 전달한다.
	// 번역이 완료된 엑셀은 다시 텍스트로 변환한후
	// 번역된 텍스트를 text_en.txt에 바꿔 넣는다.
	SAFE_DELETE( pTableOld );
	XALERT("complete");
	//////////////////////////////////////////////////////////////////////////
	// 번역된문서를 합칠때는 TEXT_TBL로 그대로 다시 읽어서 원본text_en를 읽은 TEXT_TBL에 덮어씌우고 재저장한다.
}
 
bool CMainFrame::SaveTextList( LPCTSTR szFile, const XList4<XTextTable::xText>& listText )
{
	XResFile resFile;
	LPCTSTR szFull = XE::MakePackageFullPath( DIR_PROP, szFile );
	auto ok = resFile.Open( szFull, XResFile::xWRITE );
	XBREAK( ok == 0 );
	if( resFile.GetLastError() == XE::ERR_READONLY ||
		resFile.GetLastError() == XE::ERR_PERMISSION_DENIED ) {
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( szFull ) );
		return false;
	}
	TCHAR bom = 0xFEFF;
	resFile.Write( &bom, 2 );
	for( auto& text : listText ) {
		LPCTSTR szStr = XE::Format( _T( "%d\t\"%s\"\r\n" ), text.idText, text.strText.c_str() );
		int size = ( _tcslen( szStr ) ) * sizeof( TCHAR );	// 바이너리로 저장하므로 스트링뒤에 null은 저장하면 안된다.
		resFile.Write( (void*)szStr, size );
	}
	return true;
}

/**
@brief 번역된 텍스트 파일을 현재 파일과 합친다.
text_en_new.txt파일을 현재의 text_en.txt파일에 갱신한다.
*/
void CMainFrame::OnMergeText()
{
	// text_en_new.txt를 읽어서 메모리에 올린다.
	XTextTable tableNew;
	const _tstring strNew = PATH_PROP( "text_en_new.txt" );
	auto bOk = tableNew.Load( strNew.c_str() );
	if( XASSERT(bOk) ) {
		XTextTable tableEnCurr;
		if( XASSERT( tableEnCurr.Load( PATH_PROP("text_en.txt")) ) ) {
			tableEnCurr.DoMerge( &tableNew );
			tableEnCurr.Save();
		}
	}
}

void CMainFrame::OnResavePropHelp()
 {
	 // TODO: 여기에 명령 처리기 코드를 추가합니다.
	 /*
	 저장할때
	 text항목들은 TEXT_TBL에 넣은후 그 아이디를 id_text에 기록한다.
	 코멘트가 된다면 원래 한글은 코멘트에 저장한다.
	 읽을때 id_text가 있으면 그것만 읽고 strText는 비워둔다
	 */
	 xHelp::XPropHelp::sGet()->Save( _T("propHelp2.xml") );
	 XALERT( "complete" );
 }


void CMainFrame::OnViewBoundboxSpot()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewBoundBoxSpot = !XAPP->m_bDebugViewBoundBoxSpot;
// 	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewBoundboxSpot( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewBoundBoxSpot )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}



void CMainFrame::OnPlayx1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 1.f;
}
void CMainFrame::OnPlayx2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 2.f;
}
void CMainFrame::OnPlayx4()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 4.f;
}
void CMainFrame::OnPlayx8()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 8.f;
}


void CMainFrame::OnUpdatePlayx1( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 1.f )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}
void CMainFrame::OnUpdatePlayx2( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 2.f )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}
void CMainFrame::OnUpdatePlayx4( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 4.f )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}
void CMainFrame::OnUpdatePlayx8( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 8.f )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewAreaLabel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewAreaLabel = !XAPP->m_bDebugViewAreaLabel;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewAreaLabel( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewAreaLabel )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnViewSquads()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bDebugViewSquadsHp = !XAPP->m_bDebugViewSquadsHp;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewSquads( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugViewSquadsHp )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}


void CMainFrame::OnPlayPause()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 0.f;
}


void CMainFrame::OnUpdatePlayPause( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 0.f ) {
		pCmdUI->SetCheck( 1 );
		pCmdUI->Enable( FALSE );
	} else {
		pCmdUI->SetCheck( 0 );
		pCmdUI->Enable( TRUE );
	}
}


void CMainFrame::OnPlay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_fAccel = 1.f;
}


void CMainFrame::OnUpdatePlay( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_fAccel == 0.f ) {
		pCmdUI->SetCheck( 0 );
		pCmdUI->Enable( TRUE );
	}
	else {
		pCmdUI->SetCheck( 1 );
		pCmdUI->Enable( FALSE );
	}
}


void CMainFrame::OnKeyBack()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->OnKeyUp( XE::KEY_BACK );
}


void CMainFrame::OnStopPassive()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bStopPassive = !XAPP->m_bStopPassive;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateStopPassive( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bStopPassive != false );
}


void CMainFrame::OnFilHero()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	XAPP->m_bFilterHero = !XAPP->m_bFilterHero;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateFilHero( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	pCmdUI->SetCheck( XAPP->m_bFilterHero != false );
}


void CMainFrame::OnFilUnit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	XAPP->m_bFilterUnit = !XAPP->m_bFilterUnit;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateFilUnit( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	pCmdUI->SetCheck( XAPP->m_bFilterUnit != false );
}


void CMainFrame::OnStopActive()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bStopActive = !XAPP->m_bStopActive;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateStopActive( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bStopActive );
}


void CMainFrame::OnFilterPlayer()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	XAPP->m_bFilterPlayer = !XAPP->m_bFilterPlayer;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateFilterPlayer( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	pCmdUI->SetCheck( XAPP->m_bFilterPlayer );
}


void CMainFrame::OnFilterEnemy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	XAPP->m_bFilterEnemy = !XAPP->m_bFilterEnemy;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateFilterEnemy( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	pCmdUI->SetCheck( XAPP->m_bFilterEnemy );
}


void CMainFrame::OnViewCutScene()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewCutScene = !XAPP->m_bViewCutScene;
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewCutScene( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewCutScene );
}


void CMainFrame::OnViewMemoryInfo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewMemoryInfo = !XAPP->m_bViewMemoryInfo;
}


void CMainFrame::OnUpdateViewMemoryInfo( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewMemoryInfo );
}


void CMainFrame::OnViewFrameRate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->SetbViewFrameRate( !XAPP->GetbViewFrameRate() );
	XAPP->XClientMain::SaveCheat();
}


void CMainFrame::OnUpdateViewFrameRate( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->GetbViewFrameRate() );
}

void CMainFrame::OnViewHexaIdx()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewHexaIdx = !XAPP->m_bViewHexaIdx;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateViewHexaIdx( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewHexaIdx );
}

void CMainFrame::OnViewAreaCost()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewAreaCost = !XAPP->m_bViewAreaCost;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateViewAreaCost( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewAreaCost );
}

void CMainFrame::OnViewBgObjBoundBox()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewBgObjBoundBox = !XAPP->m_bViewBgObjBoundBox;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateViewBgObjBoundBox( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewBgObjBoundBox );
}

void CMainFrame::OnViewSpotInfo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->m_bViewSpotInfo = !XAPP->m_bViewSpotInfo;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateViewSpotInfo( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( XAPP->m_bViewSpotInfo );
}

void CMainFrame::OnPlaySoundEffect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//SOUNDMNG->SetSoundMute( !SOUNDMNG->GetSoundMute() );
	//	XAPP->XClientMain::SaveCheat();
// 	SOUNDMNG->TogglebMuteSound();
// 	if( SOUNDMNG->IsbMuteSound() )
// 		SOUNDMNG->SetSoundMasterVolume( 0.f );
// 	else
// 		SOUNDMNG->SetSoundMasterVolume( 1.f );
// 	GAME->GetpOption()->TogglebSound();
// 	GAME->GetpOption()->Save();
	GAME->ToggleSound();
}
void CMainFrame::OnUpdatePlaySoundEffect( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( (GAME->GetpOption()->IsbSound())? 1 : 0 );
}

void CMainFrame::OnPlayBGM()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GAME->ToggleBGM();
// 	static float s_backup = 1.f;
//  	const auto vol = SOUNDMNG->GetBGMMasterVolume();
//  	if( vol != 0 )
//  		s_backup = vol;
// // 	SOUNDMNG->SetBGMMasterVolume( (vol)? 0 : s_backup );
// 	SOUNDMNG->TogglebMuteBGM();
// 	if( SOUNDMNG->IsbMuteBGM() )
// 		SOUNDMNG->SetBGMMasterVolume( 0 );
// 	else
// 		SOUNDMNG->SetBGMMasterVolume( s_backup );
// 	GAME->GetpOption()->TogglebMusic();
// 	GAME->GetpOption()->Save();
//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdatePlayBGM( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( (GAME->GetpOption()->IsbMusic())? 1 : 0 );
}

void CMainFrame::OnLayerTest()
{
// 	static CString s_strXml;
// 	static std::string s_strNode;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgLayerTest dlg;
	dlg.m_strXml = C2SZ(XAPP->m_strLayoutXml);
	dlg.m_strNode = C2SZ( XAPP->m_strLayoutNode );
	if( dlg.DoModal() == IDOK ) {
		XAPP->m_strLayoutXml = SZ2C(dlg.m_strXml);
		XAPP->m_strLayoutNode = SZ2C( (LPCTSTR)dlg.m_strNode );
		XAPP->XClientMain::SaveCheat();
//		std::string strNode = s_strNode;
		auto pPopup = new XWndLayerTest( dlg.m_strXml, XAPP->m_strLayoutNode.c_str() );
		pPopup->SetstrIdentifier( "__wnd.layout.test" );
		GAME->Add( pPopup );
	}
}

void CMainFrame::OnReloadLayout()
{
	GAME->DestroyWndByIdentifier( "__wnd.layout.test" );
	const _tstring strXml = C2SZ( XAPP->m_strLayoutXml );
	auto pPopup = new XWndLayerTest( strXml.c_str(), XAPP->m_strLayoutNode.c_str() );
	pPopup->SetstrIdentifier( "__wnd.layout.test" );
	GAME->Add( pPopup );
}

void CMainFrame::OnTestParticle()
{
	CDlgLayerTest dlg;
	dlg.m_strXml = _T("particles.xml");
	dlg.m_strNode = C2SZ( GAME->m_strcNodeParticle );
	if( dlg.DoModal() == IDOK ) {
		GAME->m_strcNodeParticle = SZ2C( (LPCTSTR)dlg.m_strNode );
		XPropParticle::sGet()->Load( _T( "particles/particles.xml" ) );
	}
}
void CMainFrame::OnReloadParticle()
{
	XPropParticle::sGet()->Load( _T( "particles/particles.xml" ) );
}

void CMainFrame::OnModeTestParticle()
{
	if( XAPP->m_ModeTest != XAppMain::xTEST_PARTICLE  )
		XAPP->m_ModeTest = XAppMain::xTEST_PARTICLE;
	else
		XAPP->m_ModeTest = XAppMain::xTEST_NONE;
}
void CMainFrame::OnUpdateModeTestParticle( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL(XAPP->m_ModeTest == XAppMain::xTEST_PARTICLE) );
}


void CMainFrame::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	__super::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CMainFrame::OnViewWinBoundBox()
{
	XWnd::s_bDrawOutline = !XWnd::s_bDrawOutline;
}
void CMainFrame::OnUpdateViewWinBoundBox( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL(XWnd::s_bDrawOutline) );
}

void CMainFrame::OnViewMouseOverWins()
{
	XWnd::s_bDrawMouseOverWins = !XWnd::s_bDrawMouseOverWins;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateViewMouseOverWins( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XWnd::s_bDrawMouseOverWins ) );
}

void CMainFrame::OnFlushCashImg()
{
	IMAGE_MNG->DoFlushCache();
	SPRMNG->DoFlushCache();
}

void CMainFrame::OnCheatCommand()
{
	static _tstring s_strCmd;
	CDlgCheatCommand dlg;
	dlg.m_strCmd = s_strCmd.c_str();
	if( dlg.DoModal() == IDOK ) {
		s_strCmd = (LPCTSTR)dlg.m_strCmd;
		if( !s_strCmd.empty() ) {
			if( s_strCmd == _T( "show atlas" ) ) {

			}
			GAMESVR_SOCKET->SendCheat( GAME, 99, 0, 0, 0, 0, s_strCmd );
		}
	}
// 	CDlgEditVal dlg;
// 	dlg.m_nLevel = ACCOUNT->GetLevel();
// 	dlg.m_dwEXP = ACCOUNT->GetExp();
// #if _DEV_LEVEL <= DLV_DEV_PERSONAL
// 	dlg.m_GMLevel = ACCOUNT->GetGMLevel();
// #endif
// 	if( dlg.DoModal() == IDOK ) {
// 		ACCOUNT->SetLevel( dlg.m_nLevel );
// 		ACCOUNT->SetExp( dlg.m_dwEXP );
// #if _DEV_LEVEL <= DLV_DEV_PERSONAL
// 		ACCOUNT->SetGMLevel( dlg.m_GMLevel );
// #endif
// 	}
}


void CMainFrame::OnViewDamageLogging()
{
	XAPP->m_bBattleLogging = !XAPP->m_bBattleLogging;
}
void CMainFrame::OnUpdateOnViewDamageLogging( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XAPP->m_bBattleLogging ) );
}

void CMainFrame::OnBattleOption()
{
	CDlgBattleOption dlg;
	if( XAPP->m_dwFilter & xBIT_PLAYER_HERO )
		dlg.m_bCheckPlayerHero = TRUE;
	if( XAPP->m_dwFilter & xBIT_PLAYER_UNIT )
		dlg.m_bCheckPlayerUnit = TRUE;
	if( XAPP->m_dwFilter & xBIT_ENEMY_HERO )
		dlg.m_bCheckEnemyHero = TRUE;
	if( XAPP->m_dwFilter & xBIT_ENEMY_UNIT )
		dlg.m_bCheckEnemyUnit = TRUE;
	if( XAPP->m_dwFilter & xBIT_HERO_INFO_CONSOLE )
		dlg.m_bHeroInfoToConsole = TRUE;
	if( XAPP->m_dwFilter & xBIT_FLUSH_IMG )
		dlg.m_bCheckFlushImg = TRUE;
	if( XAPP->m_dwFilter & xBIT_FLUSH_SPR )
		dlg.m_bCheckFlushSpr = TRUE;
	{
		// 0x00010000 부터 0x80000000까지 16비트의 각 비트를 dlg에 셋
		DWORD bit = xBIT_NO_DRAW_DMG_NUM;
		for( int i = 0; i < 16; ++i ) {
			if( XAPP->m_dwFilter & bit )
				dlg.m_bNoDraw[i] = TRUE;
			bit <<= 1;
		}
	}
	dlg.m_strFontDmg = XObjDmgNum::s_strFont.c_str();
	//
	if( dlg.DoModal() ) {
		if( dlg.m_bCheckPlayerHero )	XAPP->m_dwFilter |= xBIT_PLAYER_HERO;
		else													XAPP->m_dwFilter &= ~xBIT_PLAYER_HERO;
		if( dlg.m_bCheckPlayerUnit )	XAPP->m_dwFilter |= xBIT_PLAYER_UNIT;
		else													XAPP->m_dwFilter &= ~xBIT_PLAYER_UNIT;
		if( dlg.m_bCheckEnemyHero )		XAPP->m_dwFilter |= xBIT_ENEMY_HERO;
		else													XAPP->m_dwFilter &= ~xBIT_ENEMY_HERO;
		if( dlg.m_bCheckEnemyUnit )		XAPP->m_dwFilter |= xBIT_ENEMY_UNIT;
		else													XAPP->m_dwFilter &= ~xBIT_ENEMY_UNIT;
		if( dlg.m_bHeroInfoToConsole )	XAPP->m_dwFilter |= xBIT_HERO_INFO_CONSOLE;
		else														XAPP->m_dwFilter &= ~xBIT_HERO_INFO_CONSOLE;
		if( dlg.m_bCheckFlushImg )	XAPP->m_dwFilter |= xBIT_FLUSH_IMG;
		else												XAPP->m_dwFilter &= ~xBIT_FLUSH_IMG;
		if( dlg.m_bCheckFlushSpr )	XAPP->m_dwFilter |= xBIT_FLUSH_SPR;
		else												XAPP->m_dwFilter &= ~xBIT_FLUSH_SPR;
		DWORD bit = xBIT_NO_DRAW_DMG_NUM;
		for( int i = 0; i < 16; ++i ) {
			if( dlg.m_bNoDraw[i] )
				XAPP->m_dwFilter |= bit;
			else
				XAPP->m_dwFilter &= ~bit;
			bit <<= 1;
		}
		XObjDmgNum::s_strFont = (LPCTSTR)dlg.m_strFontDmg;
		XAPP->XClientMain::SaveCheat();
	}
}

void CMainFrame::OnReloadWhenRetryConstant()
{
	XAPP->m_bReloadWhenRetryConstant = !XAPP->m_bReloadWhenRetryConstant;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateReloadWhenRetryConstant( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XAPP->m_bReloadWhenRetryConstant ) );
}

void CMainFrame::OnReloadWhenRetryPropUnit()
{
	XAPP->m_bReloadWhenRetryPropUnit = !XAPP->m_bReloadWhenRetryPropUnit;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateReloadWhenRetryPropUnit( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XAPP->m_bReloadWhenRetryPropUnit ) );
}


void CMainFrame::OnReloadWhenRetryPropSkill()
{
	XAPP->m_bReloadWhenRetryPropSkill = !XAPP->m_bReloadWhenRetryPropSkill;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateReloadWhenRetryPropSkill( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XAPP->m_bReloadWhenRetryPropSkill ) );
}


void CMainFrame::OnReloadWhenRetryPropLegion()
{
	XAPP->m_bReloadWhenRetryPropLegion = !XAPP->m_bReloadWhenRetryPropLegion;
	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateReloadWhenRetryPropLegion( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xboolToBOOL( XAPP->m_bReloadWhenRetryPropLegion ) );
}

void CMainFrame::OnProfileTestNoDraw()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoDraw );
//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoDraw( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoDraw) );
}

void CMainFrame::OnProfileTestNoDP()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoDP );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoDP( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoDP ) );
}

void CMainFrame::OnProfileTestNoTexture()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoTexture );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoTexture( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoTexture ) );
}

void CMainFrame::OnProfileTestSmallTex()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitSmallTex );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestSmallTex( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitSmallTex ) );
}

void CMainFrame::OnProfileTestNoDrawBar()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoDrawBar );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoDrawBar( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoDrawBar ) );
}

void CMainFrame::OnProfileTestNoFontDraw()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoFont );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoFontDraw( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoFont ) );
}

void CMainFrame::OnProfileTestNoProcess()
{
	xToggleBit( XGraphics::s_dwDraw, XE::xeBitNoProcess );
	//	XAPP->XClientMain::SaveCheat();
}
void CMainFrame::OnUpdateProfileTestNoProcess( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( xIsBitOn2( XGraphics::s_dwDraw, XE::xeBitNoProcess ) );
}

void CMainFrame::OnProfiling()
{
	if( SCENE_BATTLE )
		SCENE_BATTLE->OnDebugProfile( nullptr, 0, 0 );
}
void CMainFrame::OnUpdateProfiling( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( XEProfile::sIsActive() );
}

void CMainFrame::OnGotoTestScene()
{
	if( XGame::sGet()->GetCurrScene() )
		XGame::sGet()->GetCurrScene()->DoExit( XGAME::xSC_TEST );
}