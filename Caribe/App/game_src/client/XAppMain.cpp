#include "stdafx.h"
#ifdef WIN32
#include "DlgEnterName.h"
#include "XDlgConsole.h"
#endif
#include "XGame.h"
#include "XGlobalConst.h"
#include "XResMng.h"
#include "client/XCheatOption.h"
#include "client/XAppMain.h"
#include "XAccount.h"
#include "XAsyncMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif
XAppMain *XAPP = nullptr;

XAppMain* XAppMain::sCreate( XE::xtDevice device, int widthPhy, int heightPhy )
{
	XBREAK( XAPP != nullptr );
	XAPP = new XAppMain;
	XAPP->Create( device, 
					widthPhy,	// 하드웨어 물리적 해상도
					heightPhy,	// 
					(float)XRESO_WIDTH,		// 게임의 논리적 해상도
					(float)XRESO_HEIGHT );
// 	XAPP->m_bXuzhuMode = true;		// 이제 허저모드는 디폴트.
	return XAPP;
}
//////////////////////////////////////////////////////////////////////////
XAppMain::XAppMain()
	: m_dwFilter(XGAME::xBIT_SIDE_FILTER)
{
	XBREAK( XAPP != nullptr );
	XAPP = this;
	Init();
		
}

void XAppMain::Destroy() 
{
	XBREAK( XAPP == nullptr );
//	XAPP = nullptr;
}

void XAppMain::DidFinishCreate( void )
{
#if defined(_XSINGLE) && !defined(WIN32)
	m_bViewFrameRate = true;
#endif 
}

// 엔진 초기화 완료.
void XAppMain::DidFinishInitEngine( void )
{
}

// 매 루프 돌아가야 하는 게임객체가 있다면 생성을 하위에 맡긴다.
XEContent* XAppMain::CreateGame( void )
{
	XGame *pGame = new XGame;
//	pGame->Create();
	return pGame;
}

void XAppMain::ConsoleMessage( LPCTSTR szMsg )
{
#ifdef WIN32
	if( IsThreadMain() ) {
		if( GetDlgConsole() )
			GetDlgConsole()->MessageString( szMsg );
	}
#endif
}

void XAppMain::SaveCheat( FILE *fp )
{
	fprintf( fp, "framerate = %d\r\n", GetbViewFrameRate() );
	fprintf( fp, "left_unit = %d\r\n", XGC->m_unitLeft );
	fprintf( fp, "right_unit = %d\r\n", XGC->m_unitRight );
	fprintf( fp, "left_unit_lv = %d\r\n", XGC->m_lvSquadLeft );
	fprintf( fp, "right_unit_lv = %d\r\n", XGC->m_lvSquadRight );
	fprintf( fp, "debug_squad = %d\r\n", m_bDebugSquad );
	fprintf( fp, "view_refcnt = %d\r\n", m_bDebugViewRefCnt );
	fprintf( fp, "view_squad_radius = %d\r\n", m_bDebugViewSquadRadius );
	fprintf( fp, "view_unitsn = %d\r\n", m_bDebugViewUnitSN );
	fprintf( fp, "view_targetsn = %d\r\n", m_bDebugViewTarget );
	fprintf( fp, "show_cloud = %d\r\n", m_nToolShowCloud );
	fprintf( fp, "view_buff = %d\r\n", m_bDebugViewBuff );
	fprintf( fp, "view_hp = %d\r\n", m_bDebugViewHp );
	fprintf( fp, "view_spot = %d\r\n", m_modeToolSpotView );
	fprintf( fp, "left_hero = \"%s\"\r\n", SZ2C(XGC->m_strHero1.c_str()) );
	fprintf( fp, "right_hero = \"%s\"\r\n", SZ2C(XGC->m_strHero2.c_str()) );
	fprintf( fp, "view_quest = %d\r\n", m_bDebugQuestList );
	fprintf( fp, "view_damage = %d\r\n", m_bDebugViewDamage );
	fprintf( fp, "view_me_damage = %d\r\n", m_bDebugViewAttackedDamage );
	fprintf( fp, "hero_immor = %d\r\n", m_bDebugHeroImmortal );
	fprintf( fp, "unit_immor = %d\r\n", m_bDebugUnitImmortal );
	fprintf( fp, "view_squad = %d\r\n", m_bDebugViewSquadInfo );
	fprintf( fp, "lv_hero1 = %d\r\n", XGC->m_lvHero1 );
	fprintf( fp, "lv_hero2 = %d\r\n", XGC->m_lvHero2 );
	fprintf( fp, "view_label = %d\r\n", m_bDebugViewAreaLabel );
	fprintf( fp, "stop_active = %d\r\n", m_bStopActive );
	fprintf( fp, "stop_passive = %d\r\n", m_bStopPassive );
	fprintf( fp, "cut_scene = %d\r\n", m_bViewCutScene );
	fprintf( fp, "mem_info = %d\r\n", m_bViewMemoryInfo );
	fprintf( fp, "view_hexa_idx = %d\r\n", m_bViewHexaIdx );
	fprintf( fp, "view_area_cost = %d\r\n", m_bViewAreaCost );
	
#ifdef WIN32
	fprintf( fp, "mouse_over_wins = %d\r\n", XWnd::s_bDrawMouseOverWins );
	fprintf( fp, "layout_xml = \"%s\"\r\n", m_strLayoutXml.c_str() );
	fprintf( fp, "layout_node = \"%s\"\r\n", m_strLayoutNode.c_str() );
// 	fprintf( fp, "fil_player = %d\r\n", m_bFilterPlayer );
// 	fprintf( fp, "fil_enemy = %d\r\n", m_bFilterEnemy );
// 	fprintf( fp, "fil_hero = %d\r\n", m_bFilterHero );
// 	fprintf( fp, "fil_unit = %d\r\n", m_bFilterUnit );
	fprintf( fp, "filter = %d\r\n", m_dwFilter );
	fprintf( fp, "option = %d\r\n", m_dwOption );
	fprintf( fp, "nodraw = %d\r\n", m_dwNoDraw );
	fprintf( fp, "battle_log = %d\r\n", m_bBattleLogging );
	fprintf( fp, "reload_cmd = \"%s\"\r\n", m_strReloadCmd.c_str() );
	fprintf( fp, "show_hp_squad = %d\r\n", m_bDebugViewSquadsHp );
	fprintf( fp, "wait_after_win = %d\r\n", m_bWaitAfterWin );
	fprintf( fp, "reload_const = %d\r\n", m_bReloadWhenRetryConstant );
	fprintf( fp, "reload_prop_unit = %d\r\n", m_bReloadWhenRetryPropUnit );
	fprintf( fp, "reload_prop_skill = %d\r\n", m_bReloadWhenRetryPropSkill );
	fprintf( fp, "reload_prop_legion = %d\r\n", m_bReloadWhenRetryPropLegion );
#endif // WIN32
	fprintf( fp, "show_face = %d\r\n", m_bShowFace );

	CONSOLE( "saved cheat data" );




}
void XAppMain::LoadCheat( CToken& token )
{
	if( token == _T("framerate") ) {
		token.GetToken();	// =
		bool bView = token.GetBool();
		SetbViewFrameRate( bView );
	} else
	if( token == _T("left_unit") ) {
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_unitLeft = (XGAME::xtUnit) token.GetNumber();
	} else
	if( token == _T( "right_unit" ) )	{
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_unitRight = ( XGAME::xtUnit ) token.GetNumber();
	}	else
	if( token == _T( "left_unit_lv" ) )	{
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_lvSquadLeft = token.GetNumber();
	}	else
	if( token == _T( "right_unit_lv" ) )	{
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_lvSquadRight = token.GetNumber();
	}	else
	if( token == _T("debug_squad") )	{
		token.GetToken();	// =
		m_bDebugSquad = token.GetNumber();
	} else
	if( token == _T("view_refcnt") )
	{
		token.GetToken();	// =
		m_bDebugViewRefCnt = token.GetNumber();
	} else
	if( token == _T("view_squad_radius") )
	{
		token.GetToken();	// =
		m_bDebugViewSquadRadius = token.GetNumber();
	} else
	if( token == _T("view_unitsn") )
	{
		token.GetToken();	// =
		m_bDebugViewUnitSN = token.GetNumber();
	} else
	if( token == _T( "view_targetsn" ) )
	{
		token.GetToken();	// =
		m_bDebugViewTarget = token.GetNumber();
	} else
	if( token == _T( "view_buff" ) )
	{
		token.GetToken();	// =
		m_bDebugViewBuff = token.GetNumber();
	} else
	if( token == _T( "show_cloud" ) )
	{
		token.GetToken();	// =
		m_nToolShowCloud = token.GetNumber();
	} else
	if( token == _T( "view_hp" ) )
	{
		token.GetToken();	// =
		m_bDebugViewHp = token.GetNumber();
	} else
	if( token == _T( "view_spot" ) )
	{
		token.GetToken();	// =
		m_modeToolSpotView = (XGAME::xtSpot) token.GetNumber();
	} else
	if( token == _T("left_hero"))
	{
		token.GetToken();	// =
		auto szToken = token.GetToken();
		if( XE::IsHave(szToken) )
			XGlobalConst::sGetMutable()->m_strHero1 = szToken;
	} else
	if( token == _T( "right_hero" ) )
	{
		token.GetToken();	// =
		auto szToken = token.GetToken();
		if( XE::IsHave( szToken ) )
			XGlobalConst::sGetMutable()->m_strHero2 = szToken;
	} else
	if( token == _T( "view_quest" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugQuestList = (n!=0);
	} else
	if( token == _T( "view_damage" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugViewDamage = (n!=0);
	} else
	if( token == _T( "view_me_damage" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugViewAttackedDamage = (n!=0);
	} else
	if( token == _T( "hero_immor" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugHeroImmortal = (n!=0);
	} else
	if( token == _T( "unit_immor" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugUnitImmortal = (n!=0);
	} else
// 	if( token == _T( "xuzhu_mode" ) )
// 	{
// 		token.GetToken();	// =
// 		int n = token.GetNumber();
// 		m_bXuzhuMode = (n!=0);		// 임시로 받아둠.
// 		CONSOLE( "cheat mode: xuzhu_mode = %d", n );
// 	} else
	if( token == _T( "view_squad" ) )
	{
		token.GetToken();	// =
		int n = token.GetNumber();
		m_bDebugViewSquadInfo = (n!=0);
	} else
	if( token == _T("lv_hero1") ) {
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_lvHero1 = token.GetNumber();
	} else
	if( token == _T("lv_hero2") ) {
		token.GetToken();	// =
		XGlobalConst::sGetMutable()->m_lvHero2 = token.GetNumber();
	} else
	if( token == _T("view_label") ) {
		token.GetToken(); // =
		m_bDebugViewAreaLabel = token.GetBool();
	} else
	if( token == _T("stop_passive") ) {
		token.GetToken(); // =
		m_bStopPassive = token.GetBool();
	} else
	if( token == _T("stop_active") ) {
		token.GetToken(); // =
		m_bStopActive = token.GetBool();
	} else
	if( token == _T("cut_scene") ) {
		token.GetToken(); // =
		m_bViewCutScene = token.GetBool();
	} else
	if( token == _T("mem_info") ) {
		token.GetToken(); // =
		m_bViewMemoryInfo = token.GetBool();
	} else
	if( token == _T("view_hexa_idx") ) {
		token.GetToken(); // =
		m_bViewHexaIdx = token.GetBool();
	} else
	if( token == _T( "view_area_cost" ) ) {
		token.GetToken(); // =
		m_bViewAreaCost = token.GetBool();
	} else
	if( token == _T("show_face") ) {
		token.GetToken(); // =
		m_bShowFace = token.GetNumber();
	}
#ifdef WIN32
	else
	if( token == _T("layout_xml") ) {
		token.GetToken(); // =
		m_strLayoutXml = SZ2C(token.GetToken());
	} else
	if( token == _T("layout_node") ) {
		token.GetToken(); // =
		m_strLayoutNode = SZ2C(token.GetToken());
	} else
	if( token == _T("mouse_over_wins")) {
		token.GetToken(); // =
		XWnd::s_bDrawMouseOverWins = token.GetBool();
	} else
	if( token == _T("filter") ) {
		token.GetToken(); // =
		m_dwFilter = (DWORD)token.GetNumber();
	} else
	if( token == _T("option") ) {
		token.GetToken(); // =
		m_dwOption = (DWORD)token.GetNumber();
	} else
	if( token == _T("nodraw") ) {
		token.GetToken(); // =
		m_dwNoDraw = (DWORD)token.GetNumber();
	} else
// 	if( token == _T("fil_player") ) {
// 		token.GetToken(); // =
// 		m_bFilterPlayer = token.GetBool();
// 	} else
// 	if( token == _T("fil_enemy") ) {
// 		token.GetToken(); // =
// 		m_bFilterEnemy = token.GetBool();
// 	} else
// 	if( token == _T("fil_hero") ) {
// 		token.GetToken(); // =
// 		m_bFilterHero = token.GetBool();
// 	} else
// 	if( token == _T("fil_unit") ) {
// 		token.GetToken(); // =
// 		m_bFilterUnit = token.GetBool();
// 	} else
	if( token == _T("battle_log") ) {
		token.GetToken(); // =
		m_bBattleLogging = token.GetBool();
	} else
	if( token == _T( "reload_cmd" ) ) {
		token.GetToken(); // =
		m_strReloadCmd = SZ2C(token.GetToken());
	} else
	if( token == _T( "show_hp_squad" ) ) {
		token.GetToken(); // =
		m_bDebugViewSquadsHp = token.GetBool();
	} else
	if( token == _T( "wait_after_win" ) ) {
		token.GetToken(); // =
		m_bWaitAfterWin = token.GetBool();
	} else
	if( token == _T( "reload_const" ) ) {
		token.GetToken(); // =
		m_bReloadWhenRetryConstant = token.GetBool();
	} else
	if( token == _T( "reload_prop_unit" ) ) {
		token.GetToken(); // =
		m_bReloadWhenRetryPropUnit = token.GetBool();
	} else
	if( token == _T( "reload_prop_skill" ) ) {
		token.GetToken(); // =
		m_bReloadWhenRetryPropSkill = token.GetBool();
	} else
	if( token == _T( "reload_prop_legion" ) ) {
		token.GetToken(); // =
		m_bReloadWhenRetryPropLegion = token.GetBool();
	}

#endif // WIN32





// 	m_bXuzhuMode = true;	// 이제 무조건 xuzhumode
	
	

}

void XAppMain::Draw( void )
{
	XClientMain::Draw();
#ifdef _VER_IOS
#ifdef _CHEAT
	/*
	if( XAPP->m_bDebugMode )
	{
		if( m_timerMem.IsOff() )
			m_timerMem.Set( 1.0f );
		if( m_timerMem.IsOver() )
		{
			IOS::GetFreeMem( &m_memInfo );
			m_timerMem.Reset();
		}
		float freeMB = m_memInfo.free / 1000.f / 1000.f;
		float totalMB = m_memInfo.total / 1000.f / 1000.f;
		XCOLOR colFree = XCOLOR_BLUE;
		if( freeMB < totalMB * 0.1f )
			colFree = XCOLOR_RED;
		PUT_STRINGF( 0, 50, colFree, "free:%.2fMB", freeMB );
		PUT_STRINGF( 0, 60, XCOLOR_BLUE, "total:%.2fMB", totalMB );
	}
	*/
#endif
#endif
}

void XAppMain::OnError( XE::xtError codeError, DWORD p1, DWORD p2 )
{
	switch( codeError )
	{
		// 기본 시스템 폰트가 없거나 손상되었다.
	case XE::xERR_FAIL_CREATE_ASIC_FONT:
		{
			if( XE::GetLoadType() == XE::xLT_WORK_TO_PACKAGE_COPY )
			{
				// 패키지에 있는 파일을 워크에 카피해 넣는다.
				LPCTSTR szFont = (LPCTSTR)p1;
				XE::CopyPackageToWork( XE::MakePath( DIR_FONT, szFont ) );
				XALERT("error! app restart please.");
				DoExit();
			}
		}
		break;
	}
}

BOOL XAppMain::DoEditBox( TCHAR *pOutText, int lenOut ) 
{ 
#ifdef WIN32
	XBREAK( pOutText == nullptr );
	CDlgEnterName dlg;		// 이거 CIV전용이니까 일반화 시킬것
	dlg.m_strName = pOutText;
	if( dlg.DoModal() == IDOK )
	{
		if( dlg.m_strName.GetLength() < lenOut )
		{
			_tcscpy_s( pOutText, lenOut, dlg.m_strName );
			return TRUE;
		} else
			XLOGXN("input editbox string too long: buffsize=%d, string size=%d", lenOut, dlg.m_strName.GetLength() );
	}
#endif

	return FALSE; 
}

/**
 @brief 플랫폼에 따라서 리소스 읽는방식을 달리한다.
*/
BOOL XAppMain::OnSelectLoadType( XE::xtLoadType typeLoad ) 
{
	if( XInputMng::s_Device == XE::DEVICE_WINDOWS )
	{
#ifdef _XPATCH
		if( typeLoad == XE::xLT_WORK_TO_PACKAGE_COPY )  //
			return TRUE;
#else
		if( typeLoad == XE::xLT_PACKAGE_ONLY )  //
			return TRUE;
#endif
	} else
	if( XInputMng::s_Device == XE::DEVICE_ANDROID ||
		XInputMng::s_Device == XE::DEVICE_IPAD ||
		XInputMng::s_Device == XE::DEVICE_IPOD )
	{
		if( typeLoad == XE::xLT_WORK_TO_PACKAGE_COPY )  //
			return TRUE;
	} else
		{
#ifdef _XPATCH
				// 나머지기종은 워크카피로 테스트
				if( typeLoad == XE::xLT_WORK_TO_PACKAGE_COPY )
						return TRUE;
#else
				if( typeLoad == XE::xLT_PACKAGE_ONLY )  //
						return TRUE;
#endif
		}
	return FALSE;
}

bool XAppMain::RequestCheatAuth()
{
	if( XAccount::sGetPlayer() && XAccount::sGetPlayer()->GetGMLevel() == 1 ) {
		return true;
	}
	return false;
}

void XAppMain::FrameMove()
{
	XClientMain::FrameMove();
	//
	XAsyncMng::sGet()->Process();
}
