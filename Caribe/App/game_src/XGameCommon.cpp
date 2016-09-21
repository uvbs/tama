#include "stdafx.h"
#include "XGameCommon.h"
#include "XFramework/XGlobal.h"
#include "XFramework/XConstant.h"
#include "XGameLua.h"
#include "constGame.h"
#include "XPropHero.h"
#include "XPropWorld.h"
#include "XPropCloud.h"
#include "XPropUnit.h"
#include "XSkillMng.h"
#include "XExpTableHero.h"
#include "XExpTableUser.h"
#include "XPropItem.h"
#include "XPropSquad.h"
#include "XPropTech.h"
#include "XQuestProp.h"
#include "XPropCamp.h"
#include "XPropUpgrade.h"
#include "XPropHelp.h"
#include "XPropLegion.h"
#include "XGlobalConst.h"
#include "XResMng.h"
#include "XImageMng.h"
#if defined(_CLIENT) || defined(_GAME_SERVER)
#include "XPropUser.h"
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

#ifdef _CLIENT
#include "XFramework/client/XPropParticle.h"
#endif // _DEBUG

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xHelp;
namespace XGAME {
}

////////////////////////////////////////////////////////////////
XGameCommon::XGameCommon()
{
	Init();
	XE::LANG.SetSelectedKey( "english" );
}

void XGameCommon::Destroy()
{
	DestroyCommon();
}

void XGameCommon::Create()
{
}

BOOL XGameCommon::LoadTextTable()
{
	XBREAK( TEXT_TBL != nullptr );
	// 이것은 장차 엔진단으로 들어가게 된다. 
	// 일단은 패치클라 실행전에 로딩되어야 하므로 여기 잔류한다.
	{
		TCHAR szRes[ 64 ];
		TEXT_TBL = new XTextTable;
		LPCTSTR szFile = XE::Format( _T( "text_%s.txt" ), XE::LANG.GetszFolder() );
		_tcscpy_s( szRes, XE::MakePath( DIR_PROP, szFile ) );
		if( TEXT_TBL->Load( szRes ) == FALSE ) {
#ifdef _CLIENT
#ifdef _DEBUG
			XBREAK(1);
#else
			if( XE::GetLoadType() == XE::xLT_WORK_TO_PACKAGE_COPY ) {
				// 파일이 손상되었다고 판단하고 패키지폴더에서 다시 카피함
				XE::CopyPackageToWork( szRes );
				XALERT( "%s: The file is damaged. Take action, but if the same problem continues to occur, please delete the app and reinstall.", szRes );
				SAFE_DELETE( TEXT_TBL );
				TEXT_TBL = new XTextTable;
				TEXT_TBL->Load( szRes );
			}
#endif // not DEBUG
#endif	
		}
	}
	return TRUE;
}

void XGameCommon::LoadConstant()
{
	CONSOLE( "Load defineGame.h..." );
	CONSTANT = new XConstant;
	CONSTANT->Load( _T( "defineGame.h" ) );			// 상수값들 읽어서 메모리에 적재
// #ifdef _CLIENT
	CONSTANT->Load( _T( "defineAct.h" ) );			// 추가로 읽는다.
	AddConstant();
// #endif // _CLIENT
}

// 클라/서버 공통 데이타 로드부를 코딩합니다.(프로퍼티 같은거)
void XGameCommon::CreateCommon()
{
	// 텍스트 테이블
	SAFE_DELETE( TEXT_TBL );
	LoadTextTable();
	// 스크립트 상수
#if (!defined(_XPROP_SERIALIZE) && defined(_CLIENT)) || defined(_SERVER)
	XBREAK( CONSTANT != nullptr );
	LoadConstant();
	IMAGE_MNG->LoadMap( _T( "img_map.txt" ) );
	// 안드로이드는 여기 읽으면 안됨
#ifndef _XSINGLE
#ifdef _VER_ANDROID
#error "Do add _XPROP_SERIALIZE"
#endif
#endif
#endif // not _XPROP_SERIALIZE
	// 루아 생성
// 	CONSOLE( "Load game.lua..." );
// 	m_pLua = new XGameLua("game.lua");
// 	m_pLua->Initialize();
//
	CONSOLE( "Create global constant..." );
	// xml로딩
	XGlobalConst::sGetMutable()->Load( XE::MakePath( DIR_SCRIPTW, _T("global.xml") ) );
	if( XBREAK( XGlobalConst::sGet()->IsError() ) ) {
		XERROR( "load failed global.xml" );
		return;
	}
	bool bOk = XGlobalConst::sGetMutable()->LoadTable( _T("global.txt") );
	if( XBREAK(bOk == false) ) {
		XERROR( "load failed global.txt" );
		return;
	}
	XGlobalConst::sGetMutable()->LoadConst();
	// 프로퍼티 로딩
#if (!defined(_XPROP_SERIALIZE) && defined(_CLIENT)) || defined(_SERVER)
	PROP_UNIT = new XPropUnit;
	if( PROP_UNIT->Load( _T( "propUnit.txt" ) ) == FALSE )
		XERROR( "load error! %s", PROP_UNIT->GetstrFilename() );
	CONSOLE("Load PropHero...");
	PROP_HERO = new XPropHero;
	if( PROP_HERO->Load(_T("propHero.txt")) == FALSE )
		XERROR( "load error! %s", PROP_HERO->GetstrFilename() );
	//
	LoadPropLegion();
//	XPropLegion::sGet()->Load( _T( "propLegion.xml" ) );
	CONSOLE( "Load PropWorld..." );
	PROP_WORLD = new XPropWorld( _T("propWorld.xml") );
	if( PROP_WORLD->IsError() )
		XERROR( "load error! %s", PROP_WORLD->GetstrFilename().c_str() );
	CONSOLE( "Load PropCloud" );
	PROP_CLOUD = new XPropCloud( _T("propCloud2.xml") );
	if( PROP_CLOUD->IsError() )
		XERROR( "load error! %s", PROP_CLOUD->GetstrFilename().c_str() );
//	PROP_CLOUD->Save(_T("test.xml"));
	// 각 스팟들이 어느(구름)지역에 속해있는지를 세팅한다.
	PROP_WORLD->SetAreaToSpots();
#endif // not _XPROP_SERIALIZE
	//
	///< 
	CONSOLE( "Load exp_table_hero..." );
	EXP_TABLE_HERO = new XExpTableHero;
	if( EXP_TABLE_HERO->Load( _T( "exp_table_hero.txt" ) ) == FALSE )
		XERROR( "exp_table_hero.txt load error" );
	//
	CONSOLE( "Load exp_table_user..." );
	EXP_TABLE_USER = new XExpTableUser;
	if( EXP_TABLE_USER->Load( _T( "exp_table_user.txt" ) ) == FALSE )
		XERROR( "exp_table_user.txt load error" );
#if defined(_CLIENT) || defined(_GAME_SERVER)
	if( !XPropUser::sGet()->Load( _T("propUser.txt") ) )
		XERROR( "%s load error", XPropUser::sGet()->GetstrFilename() );
	CONSOLE( "Load propSkill..." );
#if (!defined(_XPROP_SERIALIZE) && defined(_CLIENT)) || defined(_SERVER)
	SKILL_MNG = new XSkillMng;
	if( SKILL_MNG->Load(_T("propSkill.xml")) == FALSE )
		XERROR("%s", _T("propSkill load error"));
	//
	CONSOLE( "Load propTech..." );
	if( XPropTech::sGet()->Load( _T( "propTech.xml" ) ) == FALSE )
#ifdef _xIN_TOOL
		XLOG( "%s load error", XPropTech::sGet()->XXMLDoc::GetstrFilename() );
#else
		XERROR( "%s load error", XPropTech::sGet()->XXMLDoc::GetstrFilename().c_str() );
#endif
#endif // not _XPROP_SERIALIZE
#endif // CLIENT or GAMESERVER
	///< 
#if (!defined(_XPROP_SERIALIZE) && defined(_CLIENT)) || defined(_SERVER)
	CONSOLE( "Load propItems..." );
	PROP_ITEM = new XPropItem;
	if( PROP_ITEM->Load(_T("propItem.txt")) == FALSE )
		XERROR("%s load error", PROP_ITEM->GetstrFilename() );
#if defined(_CLIENT) || defined(_GAME_SERVER)
	CONSOLE( "Load propResearch..." );
	if( XPropTech::sGet()->XEBaseProp::Load( _T( "propResearch.txt" ) ) == FALSE )
		XERROR( "%s load error", XPropTech::sGet()->XEBaseProp::GetstrFilename() );
	if( !XPropUpgrade::sGet()->Load( _T( "propUpgrade.txt" ) ) )
		XERROR( "%s load error", XPropUpgrade::sGet()->GetstrFilename() );
#endif // CLIENT or GAMESERVER
	///< 
	CONSOLE( "Load propSquad..." );
	PROP_SQUAD = new XPropSquad;
	if( PROP_SQUAD->Load( _T( "propSquad.txt" ) ) == FALSE )
		XERROR( "%s load error", PROP_SQUAD->GetstrFilename() );
	CONSOLE( "Load campaign..." );
	if( XPropCamp::sGet()->Load( _T( "propCamp.xml" ) ) == FALSE )
		XERROR( "%s load error", XPropCamp::sGet()->GetstrFilename().c_str() );
	//
	CONSOLE( "Load quests..." );
	if( XQuestProp::sGet()->Load( _T( "quest001.xml" ) ) == FALSE )
		XERROR( "%s load error", XQuestProp::sGet()->GetstrFilename().c_str() );
	if( XQuestProp::sGet()->Load( _T( "quest_main001.xml" ) ) == FALSE )
		XERROR( "%s load error", XQuestProp::sGet()->GetstrFilename().c_str() );
	if( XQuestProp::sGet()->Load( _T( "quest_repeat.xml" ) ) == FALSE )
		XERROR( "%s load error", XQuestProp::sGet()->GetstrFilename().c_str() );
	XQuestProp::sGet()->CheckValidQuest();
	//
	XPropHelp::sGet()->Load( _T( "propHelp.xml" ) );
#endif // not _XPROP_SERIALIZE
#ifdef _CLIENT
	XPropParticle::sGet()->Load( _T("particles/particles.xml") );
#endif // _CLIENT

#if (!defined(_XPROP_SERIALIZE) && defined(_CLIENT)) || defined(_SERVER)
	OnAfterPropSerialize();  // virtual
// #if defined(_CLIENT) || defined(_GAME_SERVER) 
//   // 레벨별 표준 전투력 테이블 생성
//   XGlobalConst::sGetMutable()->MakeTablePowerPerLevel();
// #  if defined(_XUZHU) && defined(_DEBUG) 
//   XGC->SaveMaxPowerTable();
// #  endif
// #endif 
#endif // not _XPROP_SERIALIZE

	CONSOLE( "finish CreateCommon..." );
}

void XGameCommon::LoadPropLegion()
{
	CONSOLE( "Load propLegion.xml..." );
	XPropLegion::sGet()->DestroyAll();
#ifdef _XSINGLE
	XPropLegion::sGet()->Load( _T( "propLegion_s.xml" ) );
#else
	XPropLegion::sGet()->Load( _T( "propLegion.xml" ) );
#endif // not _XSINGLE
	CONSOLE( "Load propLegion.xml...complete" );
}

/**
 @brief 프로퍼티 시리얼라이즈 받은 후 처리
*/
void XGameCommon::OnAfterPropSerialize()
{
#if defined(_CLIENT) || defined(_GAME_SERVER)
	// 레벨별 표준 전투력 테이블 생성
	XGlobalConst::sGetMutable()->MakeTablePowerPerLevel();
#  if defined(_XUZHU) && defined(_DEBUG)
	XGC->SaveMaxPowerTable();
#  endif
#endif 
}

void XGameCommon::DestroyCommon()
{
	XTRACE("XGameCommon::DestroyCommon()");
	XQuestProp::sDestroyInstance();
  XPropCamp::sDestroyInstance();
	SAFE_DELETE( PROP_SQUAD );
#if defined(_CLIENT) || defined(_GAME_SERVER)
  XPropUpgrade::sDestroy();
  XPropTech::sDestroy();
	SAFE_DELETE( SKILL_MNG );
#endif
  SAFE_DELETE( PROP_ITEM );
  SAFE_DELETE( EXP_TABLE_USER );
  SAFE_DELETE( EXP_TABLE_HERO );
	SAFE_DELETE( PROP_CLOUD );
	SAFE_DELETE( PROP_WORLD );
	SAFE_DELETE( PROP_HERO );
	SAFE_DELETE( PROP_UNIT );
	// 전역상수 객체 삭제
	XGlobalConst::sDestroySingleton();
	//
	SAFE_DELETE( m_pLua );
	SAFE_DELETE( CONSTANT );
  SAFE_DELETE( TEXT_TBL );
	XTRACE( "--XGameCommon::DestroyCommon()" );
}

/**
 @brief 추가 상수
*/
void XGameCommon::AddConstant()
{
	XBREAK( CONSTANT == nullptr );
	CONSTANT->Add( XTEXT( 1000 ), XGAME::xUNIT_SPEARMAN );
	CONSTANT->Add( XTEXT( 1001 ), XGAME::xUNIT_ARCHER );
	CONSTANT->Add( XTEXT( 1002 ), XGAME::xUNIT_PALADIN );
	CONSTANT->Add( XTEXT( 1003 ), XGAME::xUNIT_MINOTAUR );
	CONSTANT->Add( XTEXT( 1004 ), XGAME::xUNIT_CYCLOPS );
	CONSTANT->Add( XTEXT( 1005 ), XGAME::xUNIT_LYCAN );
	CONSTANT->Add( XTEXT( 1006 ), XGAME::xUNIT_GOLEM );
	CONSTANT->Add( XTEXT( 1007 ), XGAME::xUNIT_TREANT );
	CONSTANT->Add( XTEXT( 1008 ), XGAME::xUNIT_FALLEN_ANGEL );
	CONSTANT->Add( XTEXT( 1011 ), XGAME::xST_SLEEP );
	CONSTANT->Add( XTEXT( 1012 ), XGAME::xST_STUN );
	CONSTANT->Add( XTEXT( 1013 ), XGAME::xST_STUN );
	CONSTANT->Add( XTEXT( 1014 ), XGAME::xST_HOLD );
	CONSTANT->Add( XTEXT( 1015 ), XGAME::xST_BLEEDING );
	CONSTANT->Add( XTEXT( 1016 ), XGAME::xST_POISON );
	CONSTANT->Add( XTEXT( 1017 ), XGAME::xST_BLIND );
	CONSTANT->Add( XTEXT( 1018 ), XGAME::xST_SLOW );
	CONSTANT->Add( XTEXT( 1019 ), XGAME::xST_CHAOS );
	CONSTANT->Add( XTEXT( 1020 ), XGAME::xST_FEAR );
	CONSTANT->Add( XTEXT( 1021 ), XGAME::xST_SILENCE );
	CONSTANT->Add( XTEXT( 1038 ), XGAME::xST_FROZEN );
	CONSTANT->Add( XTEXT( 1059 ), XGAME::xST_ICE );
	CONSTANT->Add( XTEXT( 1069 ), XGAME::xST_BURN );
	CONSTANT->Add( XTEXT( 1022 ), XGAME::xST_INVISIBLE );
	CONSTANT->Add( XTEXT( 1042 ), XGAME::xST_TAUNT );
	CONSTANT->Add( XTEXT( 1070 ), XGAME::xST_FURY );
	CONSTANT->Add( XTEXT( 1074 ), XGAME::xST_PARALYSIS );

	CONSTANT->Add( XTEXT( 1025 ), XGAME::xUNIT_PALADIN );
	CONSTANT->Add( XTEXT( 1026 ), XGAME::xUNIT_MINOTAUR );
	CONSTANT->Add( XTEXT( 1027 ), XGAME::xUNIT_CYCLOPS );
	CONSTANT->Add( XTEXT( 1028 ), XGAME::xUNIT_LYCAN );
	CONSTANT->Add( XTEXT( 1029 ), XGAME::xUNIT_FALLEN_ANGEL );
	CONSTANT->Add( XTEXT( 1030 ), XGAME::xTB_HUMAN );
	CONSTANT->Add( XTEXT( 1031 ), XGAME::xTB_BEAST );
	CONSTANT->Add( XTEXT( 1032 ), XGAME::xTB_MONSTER );
	CONSTANT->Add( XTEXT( 1033 ), XGAME::xTB_DEVIL );
	// 크기
	CONSTANT->Add( XTEXT( 1034 ), XGAME::xSIZE_SMALL );
	CONSTANT->Add( XTEXT( 1035 ), XGAME::xSIZE_MIDDLE );
	CONSTANT->Add( XTEXT( 1036 ), XGAME::xSIZE_BIG );
	// 파라메터
	CONSTANT->Add( XTEXT( 1047 ), XGAME::xAT_TANKER );	// 탱커
	CONSTANT->Add( XTEXT( 1048 ), XGAME::xAT_RANGE );	// 원거리
	CONSTANT->Add( XTEXT( 1049 ), XGAME::xAT_SPEED );	// 스피드
	CONSTANT->Add( XTEXT( 1051 ), XGAME::xUR_SOLDIER );	// 병사
	CONSTANT->Add( XTEXT( 1052 ), XGAME::xUR_HERO );	// 영웅
}

#ifdef _XPROP_SERIALIZE
void XGameCommon::ArchivingProp( XArchive& ar )
{
	int sizeMax = 0;
	{
		XArchive arProp;
		XGlobalConst::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "globalxml:%d, ", sizeComp );
		ar << arProp;
	}	{
		XArchive arProp;
		CONSTANT->Serialize( arProp );
		XArchive arGlobal;
		XGlobalConst::sGet()->Serialize( arGlobal );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "XConstant:%d, ", sizeComp );
		ar << arProp;
	}	{
		XArchive arProp;
		PROP_UNIT->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propUnit:%d, ", sizeComp);
		ar << arProp;
	}	{
		XArchive arProp;
		PROP_HERO->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propHero: %d, ", sizeComp);
		ar << arProp;
	}
	{
		XArchive arProp;
		PROP_ITEM->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propitem: %d, ", sizeComp);
		ar << arProp;
	}
	{
		XArchive arProp;
		XPropLegion::sGet()->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propLegion: %d, ", sizeComp);
		ar << arProp;
	}
	{
		XArchive arProp;
		PROP_WORLD->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propworld: %d, ", sizeComp);
		ar << arProp;
	}
	{
		XArchive arProp;
		PROP_CLOUD->Serialize( arProp );
		const int sizeComp = 
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE("propCloud: %d, ", sizeComp);
		ar << arProp;
	}
#if defined(_CLIENT) || defined(_GAME_SERVER)
	{
		XArchive arProp;
		SKILL_MNG->Serialize( arProp );
		const int sizeComp =
			arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propSkill: %d, ", sizeComp );
		ar << arProp;
	}
	{
		XArchive arProp;
		XPropTech::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propTech: %d, ", sizeComp );
		ar << arProp;
	}
	{
		XArchive arProp;
		XPropUpgrade::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propUpgrade: %d, ", sizeComp );
		ar << arProp;
	}
#endif
	{
		XArchive arProp;
		PROP_SQUAD->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propSquad: %d, ", sizeComp );
		ar << arProp;
	}
	{
		XArchive arProp;
		XPropCamp::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propCamp: %d, ", sizeComp );
		ar << arProp;
	}
	{
		XArchive arProp;
		XQuestProp::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propQuest: %d, ", sizeComp );
		ar << arProp;
	}
	{
		XArchive arProp;
		XPropHelp::sGet()->Serialize( arProp );
		const int sizeComp =
		arProp.DoCompress();
		sizeMax += sizeComp;
		XTRACE( "propHelp: %d, ", sizeComp );
		ar << arProp;
	}
	XTRACE("sizeMax=%d", sizeMax);
}
#endif // _XPROP_SERIALIZE
