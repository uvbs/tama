#include "stdafx.h"
#include "constGame.h"
//#include "XGameCommon.h"
//#include "XGame.h"
#include "XGameLua.h"
#include <iomanip>
#include "XExpTableUser.h"
//#include "XAccount.h"
#include "XLegion.h"
#include "XPropUnit.h"
#include "XSkillMng.h"
#include "XResObj.h"
#include "XSystem.h"
#ifdef _CLIENT
#include "XFramework/client/XLayout.h"
#include "XAccount.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndText.h"
#endif // _CLIENT


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif




namespace XGAME 
{
	const XE::VEC2 x_sizeHexa( 194 / 2, 144 / 2 );
};

/**
 @brief 최대 유저레벨
*/
int XGAME::GetLevelMaxAccount()
{
	return EXP_TABLE_USER->GetMaxLevel();
}

/**
 @brief str스트링에서 #szToken#부분을 szReplace로 바꾼다.
*/
void XGAME::sReplaceToken( _tstring& str, 
							LPCTSTR szFind, LPCTSTR szReplace )
{
	XBREAK( 16 < _tcslen(szFind) );
	XBREAK( 256 < _tcslen( szReplace ) );
	int idxSrc = 0;
	bool bOpen = false;
	TCHAR szToken[ 32 ];
	int idxToken = 0;
	TCHAR szSrc[ 0x8000 ];
	_tcscpy_s( szSrc, str.c_str() );	// lazy copy
	int lenSrc = XNUM_ARRAY(szSrc);
	while( 1 ) {
		XBREAK( idxSrc >= lenSrc );
		TCHAR c = szSrc[ idxSrc++ ];
		if( c == 0 )
			break;
		else
		if( c == '#' ) {
			if( bOpen == false ) {
				bOpen = true;
			} else {
				// close
				XBREAK( idxToken >= XNUM_ARRAY(szToken) );
				szToken[ idxToken++ ] = c;
				szToken[ idxToken ] = 0;
				if( _tcsicmp(szToken, szFind) == 0 ) {
					XE::ReplaceStr( str, szToken, szReplace );
				}
				idxToken = 0;
				bOpen = false;
			}
		}
		if( bOpen ) {
			szToken[ idxToken++ ] = c;
			szToken[ idxToken ] = 0;
		}
	}
}
/**
 @brief 각 유닛의 원형작은 아이콘파일명을 돌려준다.
*/
_tstring XGAME::GetResUnitSmall( XGAME::xtUnit unit )
{
	auto pProp = PROP_UNIT->GetpProp( unit );
	if( XASSERT(pProp) ) {
		// 원형유닛 초상화
		const _tstring strTitle = XE::GetFileTitle( pProp->strFace.c_str() );
		const _tstring strFace = XE::Format( _T( "%s02.png" ), strTitle.c_str() );
		return _tstring( XE::MakePath( DIR_IMG, strFace ) );
	}
	return _tstring();
}

#ifdef _CLIENT
void XGAME::UpdateSkillTooltip( ID idSkill, XWnd *pRoot )
{
	auto pSkillDat = SKILL_MNG->FindByID( idSkill );
	// 스킬 이미지
	if( pSkillDat == nullptr )
		return;
	// 스킬 이름
	xSET_TEXT( pRoot, "text.name", pSkillDat->GetstrName() );
	// 스킬 설명
	_tstring strDesc;
	pSkillDat->GetstrDesc( &strDesc, 1 );
	xSET_TEXT( pRoot, "text.desc", strDesc );
	// 스킬 이미지
	xSET_IMG( pRoot, "img.icon", pSkillDat->GetResIcon() );
}
#endif // _CLIENT

const char* XGAME::GetstrEnumActionEvent( xtActionEvent event )
{
	switch( event )
	{
	case XGAME::xAE_NONE:	return "";
	case XGAME::xAE_UNLOCK_MENU:	return "xAE_UNLOCK_MENU";
	case XGAME::xAE_OPEN_AREA:		return "xAE_OPEN_AREA";
	case XGAME::xAE_UNLOCK_UNIT:	return "xAE_UNLOCK_UNIT";
	case XGAME::xAE_ENTER_SCENE:	return "xAE_ENTER_SCENE";
	case XGAME::xAE_POPUP:			return "xAE_POPUP";
	case XGAME::xAE_CLOSE_POPUP:	return "xAE_CLOSE_POPUP";
	case XGAME::xAE_END_SEQ:		return "xAE_END_SEQ";
	case XGAME::xAE_WHERE_QUEST:	return "xAE_WHERE_QUEST";
	case XGAME::xAE_CONFIRM_QUEST:	return "xAE_CONFIRM_QUEST";
	case XGAME::xAE_QUEST_COMPLETE:	return "xAE_QUEST_COMPLETE";
	case XGAME::xAE_QUEST_NEW:	return "xAE_QUEST_NEW";
	case XGAME::xAE_VISIT_SPOT:	return "xAE_VISIT_SPOT";
	case XGAME::xAE_QUEST_DESTROY: return "xAE_QUEST_DESTROY";
	default:
		XBREAK(1);
		break;
	}
	return "";
}

float XGAME::GetMultiplyPower( int levelHard ) 
{
	if( levelHard >= 3 )
		return 2.1f;	// levelHard+1로 검색할수 있으므로 이런경우를 허용함.
	else if( levelHard >= 2 )
		return 1.6f;
	else if( levelHard >= 1 )
		return 1.3f;
	else if( levelHard >= 0 )
		return 1.1f;
	else if( levelHard >= -1 )
		return 0.8f;
	else
		return 0.7f;
}

// 전투력 powerEnemy의 난이도등급을 리턴한다 0이면 동급이며 양수가 어려움, 음수가 쉬움이다. 기준은 powerBase가 된다.
int XGAME::GetHardLevel( int powerEnemy, int powerBase ) 

{
//	XBREAK( powerEnemy == 0 || powerBase == 0 );	// 영웅전장이 스테이지에 전투력을 담고 있지 않아서 잠시 삭제. 영웅전장의 방식을 바꿔야 할듯.
	float rate = powerEnemy / (float)powerBase;
	if( rate > GetMultiplyPower( 2 ) )
		return 2;
	else if( rate > GetMultiplyPower( 1 ) )
		return 1;
	else if( rate > GetMultiplyPower( 0 ) )
		return 0;
	else if( rate > GetMultiplyPower( -1 ) )
		return -1;
	else
		return -2;
}
/**
 @brief powerBase를 기준으로 lvColor의 하한선 전투력을 얻는다.
 @param lvColor -2~2
*/
int XGAME::GetPowerByColor( int powerBase, int lvColor )
{
	return (int)(powerBase * GetMultiplyPower( lvColor ));
}
// 전투력 powerEnemy가 무슨색인지 얻는다. 기준은 powerBase
XCOLOR XGAME::xGetColorPower( int powerEnemy, int powerBase ) 
{
	int grade = GetHardLevel( powerEnemy, powerBase );
	switch( grade ) {
	case 2:	return XCOLOR_RED;
	case 1:	return XCOLOR_ORANGE;
	case 0: return XCOLOR_YELLOW;
	case -1: return XCOLOR_GREEN;
	case -2: return XCOLOR_GRAY;
	default:
		XBREAK( 1 );
		break;
	}
	return 0;
}
int XGAME::xDropItem::Serialize( XArchive& ar ) {
	ar << idDropItem;
	ar << chance;
	XBREAK( num > 0x7f );
	ar << (char)num;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	return 1;
}
int XGAME::xDropItem::DeSerialize( XArchive& ar, int ) {
	ar >> idDropItem;
	ar >> chance;
	char c0;
	ar >> c0;	num = c0;
	ar >> c0 >> c0 >> c0;
	return 1;
}

LPCTSTR XGAME::GetIdsMedal( xtAttack atkType, int grade ) 
{
	if( XBREAK( atkType <= xAT_NONE || atkType >= xAT_MAX ) )
		return _T( "" );
	if( XBREAK( grade >= 4 ) )
		return _T( "" );
	LPCTSTR aryIds[ 3 ][ 4 ] = {
		{_T( "medal_tanker01" ), _T( "medal_tanker02" ), _T( "medal_tanker03" ), _T( "medal_tanker04" )},
		{_T( "medal_range01" ), _T( "medal_range02" ), _T( "medal_range03" ), _T( "medal_range04" )},
		{_T( "medal_speed01" ), _T( "medal_speed02" ), _T( "medal_speed03" ), _T( "medal_speed04" )},
	};
	return aryIds[ atkType - 1 ][ grade ];
}

XGAME::xtUnit XGAME::GetUnitBySizeAndAtkType( XGAME::xtSize size, XGAME::xtAttack atkType )
{
	switch( atkType )
	{
	case XGAME::xAT_TANKER: {
		switch( size ) {
		case XGAME::xSIZE_SMALL:	return XGAME::xUNIT_SPEARMAN;
		case XGAME::xSIZE_MIDDLE:	return XGAME::xUNIT_MINOTAUR;
		case XGAME::xSIZE_BIG:		return XGAME::xUNIT_GOLEM;
		default:
			XBREAK(1);
			break;
		}
	} break;
	case XGAME::xAT_RANGE: {
		switch( size ) {
		case XGAME::xSIZE_SMALL:	return XGAME::xUNIT_ARCHER;
		case XGAME::xSIZE_MIDDLE:	return XGAME::xUNIT_CYCLOPS;
		case XGAME::xSIZE_BIG:		return XGAME::xUNIT_TREANT;
		default:
			XBREAK( 1 );
			break;
		}
	} break;
	case XGAME::xAT_SPEED: {
		switch( size ) {
		case XGAME::xSIZE_SMALL:	return XGAME::xUNIT_PALADIN;
		case XGAME::xSIZE_MIDDLE:	return XGAME::xUNIT_LYCAN;
		case XGAME::xSIZE_BIG:		return XGAME::xUNIT_FALLEN_ANGEL;
		default:
			XBREAK( 1 );
			break;
		}
	} break;
	default:
		XBREAK(1);
		break;
	}
	return XGAME::xUNIT_NONE;
}

XE_NAMESPACE_START( XGAME )

/**
 @brief 같은계열끼리는 or로 다른계열끼리는 and로 연산된다.
 ex:) small | middle | tanker
 소형이거나 중형이면서 탱커인 유닛
*/
xtUnit GetUnitRandomByFilter( xtUnitFilter filter )
{
	if( !filter || filter == XGAME::xUF_ALL )
		return (XGAME::xtUnit)( 1 + xRandom( XGAME::xUNIT_MAX - 1 ) );

	std::vector<XGAME::xtUnit> aryUnits;
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		auto unit = (XGAME::xtUnit) i;
		// 
		if( IsUnitFilter( unit, filter ) )
			aryUnits.push_back( unit );
	}
	if( aryUnits.size() == 0 )
		return XGAME::xUNIT_NONE;
	int idx = xRandom( aryUnits.size() );
	return aryUnits[idx];
}

/**
 @brief unit이 filter의 조건에 만족하는지 검사한다.
*/
bool IsUnitFilter( XGAME::xtUnit unit, xtUnitFilter filter )
{
	// 조건이 없으면 무조건 true;
	if( !filter )
		return true;
	bool bSizeCond = false;	// 크기조건
	if( filter & XGAME::xUF_SMALL )	// 사이즈 조건이 있고
		if( GetSizeUnit(unit) == XGAME::xSIZE_SMALL )	// 사이즈 조건이 맞으면 true
			bSizeCond = true;
	if( filter & XGAME::xUF_MIDDLE )
		if( GetSizeUnit( unit ) == XGAME::xSIZE_MIDDLE )
			bSizeCond = true;
	if( filter & XGAME::xUF_BIG )
		if( GetSizeUnit( unit ) == XGAME::xSIZE_BIG )
			bSizeCond = true;
	if( !(filter & xUF_SIZE_ALL) )	// 혹은 사이즈중 아무것도 선택안했으면 이 필터는 무시하고 true
		bSizeCond = true;
	// 병과조건
	bool bAtkTypeCond = false;
	if( filter & XGAME::xUF_TANKER )	// 병과 조건이 있고
		if( GetAtkType( unit ) == XGAME::xAT_TANKER )
			bAtkTypeCond = true;
	if( filter & XGAME::xUF_RANGE )
		if( GetAtkType( unit ) == XGAME::xAT_RANGE )
			bAtkTypeCond = true;
	if( filter & XGAME::xUF_SPEED )
		if( GetAtkType( unit ) == XGAME::xAT_SPEED )
			bAtkTypeCond = true;
	if( !(filter & xUF_ATKTYPE_ALL) )
		bAtkTypeCond = true;
	// 
	if( bSizeCond && bAtkTypeCond )
		return true;
	return false;
}

xtUnitFilter AddUnitFilter( xtAttack atkType, xtUnitFilter filterSrc ) 
{
	BIT bitFilter = (BIT)filterSrc;
	BIT bitOr = 0;
	switch( atkType ) {
	case XGAME::xAT_TANKER:	bitOr = XGAME::xUF_TANKER;	break;
	case XGAME::xAT_RANGE:	bitOr = XGAME::xUF_RANGE;	break;
	case XGAME::xAT_SPEED:	bitOr = XGAME::xUF_SPEED;		break;
	default:
		XBREAK( 1 );
		break;
	}
	bitFilter |= bitOr;
	return (XGAME::xtUnitFilter)bitFilter;
}

/**
 @brief size에 해당하는 필터를 filterSrc에 or해서 돌려준다.
*/
xtUnitFilter AddUnitFilter( xtSize size, xtUnitFilter filterSrc )
{
	BIT bitFilter = (BIT)filterSrc;
	BIT bitOr = 0;
	switch( size ) {
	case XGAME::xSIZE_SMALL:	bitOr = XGAME::xUF_SMALL;	break;
	case XGAME::xSIZE_MIDDLE:	bitOr = XGAME::xUF_MIDDLE;	break;
	case XGAME::xSIZE_BIG:		bitOr = XGAME::xUF_BIG;		break;
	default:
		XBREAK( 1 );
		break;
	}
	bitFilter |= bitOr;
	return (xtUnitFilter)bitFilter;
}
xtUnitFilter xLegionParam::SetUnitFilter( xtSize size ) {
	x_filterUnit = AddUnitFilter( size, x_filterUnit );
	return x_filterUnit;
// 	BIT bitFilter = (BIT)x_filterUnit;
// 	BIT bitOr = 0;
// 	switch( size )
// 	{
// 	case XGAME::xSIZE_SMALL:	bitOr = XGAME::xUF_SMALL;	break;
// 	case XGAME::xSIZE_MIDDLE:	bitOr = XGAME::xUF_MIDDLE;	break;
// 	case XGAME::xSIZE_BIG:		bitOr = XGAME::xUF_BIG;		break;
// 	default:
// 		XBREAK(1);
// 		break;
// 	}
// 	bitFilter |= bitOr;
// 	x_filterUnit = (XGAME::xtUnitFilter)bitFilter;
// 	return x_filterUnit;
}
xtUnitFilter xLegionParam::SetUnitFilter( xtAttack atkType ) {
	x_filterUnit = AddUnitFilter( atkType, x_filterUnit );
	return x_filterUnit;
// 	BIT bitFilter = (BIT)x_filterUnit;
// 	BIT bitOr = 0;
// 	switch( atkType )
// 	{
// 	case XGAME::xAT_TANKER:	bitOr = XGAME::xUF_TANKER;	break;
// 	case XGAME::xAT_RANGE:	bitOr = XGAME::xUF_RANGE;	break;
// 	case XGAME::xAT_SPEED:	bitOr = XGAME::xUF_SPEED;		break;
// 	default:
// 		XBREAK( 1 );
// 		break;
// 	}
// 	bitFilter |= bitOr;
// 	x_filterUnit = ( XGAME::xtUnitFilter )bitFilter;
// 	return x_filterUnit;
}

/**
 @brief sec초를 xx일 xx시간 xx분 xx초 형태의 문자열로 만들어준다.
*/
_tstring GetstrResearchTime( int sec )
{
	XBREAK( sec < 0 );
	int d, h, m, s;
	XSYSTEM::GetDayHourMinSec( (xSec)sec, &d, &h, &m, &s );
	_tstring str;// = _T( "연구시간:" );
	if( d > 0 )
		str += XFORMAT( "%d%s", d, XTEXT(2230) );	// 일
	if( h > 0 )
		str += XFORMAT( " %d%s", h, XTEXT(2231) );	// 시간
	// '일'이 없을때만 분/초을 표시한다.
	if( d <= 0 ) {
		if( m > 0 )
			str += XFORMAT( " %d%s", m, XTEXT(2232) );	// 분
		// 시간이 없을때만 초를 표시한다.
		if( s >= 0 && h <= 0 )
			str += XFORMAT( " %d%s", s, XTEXT(2233) );	// 초
	}
	return str;
}

LPCTSTR GetEnumResource( XGAME::xtResource res )
{
	switch( res )
	{
	case XGAME::xRES_WOOD:	return _T("xRES_WOOD");
	case XGAME::xRES_IRON:	return _T("xRES_IRON");
	case XGAME::xRES_JEWEL:	return _T("xRES_JEWEL");
	case XGAME::xRES_SULFUR:	return _T("xRES_SULFUR");
	case XGAME::xRES_MANDRAKE:	return _T("xRES_MANDRAKE");
	case xRES_GOLD: return _T("xRES_GOLD");
	case xRES_CASH: return _T("xRES_CASH");
	case xRES_GUILD_COIN: return _T("xRES_GUILD_COIN");
	default:
		XBREAK(1);
		break;
	}
	return _T("");
}

/**
 @brief 현재 gradeCurr등급이고 gradeCurr의 다음등급으로 올라가기 위해 필요한 영혼석수.
*/
int GetNeedSoulPromotion( XGAME::xtGrade gradeCurr ) 
{
	switch( gradeCurr ) {
	case XGAME::xGD_NONE:	return 10; break;
	case XGAME::xGD_COMMON: return 15; break;
	case XGAME::xGD_VETERAN: return 30; break;
	case XGAME::xGD_RARE: return 100; break;
	case XGAME::xGD_EPIC: return 200; break;
	case XGAME::xGD_LEGENDARY: return 0;	break;
// 		case XGAME::xGD_RARE: return 40; break;
// 		case XGAME::xGD_EPIC: return 50; break;
	}
	return 0;
}

/**
 @brief grade등급을 소환하기 위해 필요한 총 영혼석 수.
*/
int GetNumTotalSoulSummon( XGAME::xtGrade grade )
{
	int sum = 0;
	for( int i = 0; i < grade; ++i )
		sum += GetNeedSoulPromotion( (XGAME::xtGrade)i );
	return sum;
}

/**
 @brief attacker에게 병과 패널티가 적용되는지 검사
*/
bool IsPenaltyMOS( XGAME::xtAttack typeAttacker, XGAME::xtAttack typeDefender )
{
	if( typeAttacker == XGAME::xAT_TANKER && typeDefender == XGAME::xAT_RANGE )
		return true;
	if( typeAttacker == XGAME::xAT_RANGE && typeDefender == XGAME::xAT_SPEED )
		return true;
	if( typeAttacker == XGAME::xAT_SPEED && typeDefender == XGAME::xAT_TANKER )
		return true;
	return false;
}

/**
 @brief attacker에게 크기 패널티가 적용되는지 검사
*/
bool IsPenaltySize( XGAME::xtSize sizeAttacker, XGAME::xtSize sizeDefender )
{
	if( sizeAttacker == XGAME::xSIZE_SMALL && sizeDefender == XGAME::xSIZE_MIDDLE )
		return true;
	if( sizeAttacker == XGAME::xSIZE_MIDDLE && sizeDefender == XGAME::xSIZE_BIG )
		return true;
	if( sizeAttacker == XGAME::xSIZE_BIG && sizeDefender == XGAME::xSIZE_SMALL )
		return true;
	return false;
}
/**
 @brief 공격자가 방어자에게 병과로 우세한가.
*/
bool IsSuperiorMOS( XGAME::xtAttack typeAttacker, XGAME::xtAttack typeDefender )
{
	if( typeAttacker == XGAME::xAT_TANKER && typeDefender == XGAME::xAT_SPEED )
		return true;
	if( typeAttacker == XGAME::xAT_RANGE && typeDefender == XGAME::xAT_TANKER )
		return true;
	if( typeAttacker == XGAME::xAT_SPEED && typeDefender == XGAME::xAT_RANGE )
		return true;
	return false;
}
/**
 @brief 공격자가 방어자에게 크기로 우세한가.
*/
bool IsSuperiorSize( XGAME::xtSize sizeAttacker, XGAME::xtSize sizeDefender )
{
	if( sizeAttacker == XGAME::xSIZE_SMALL && sizeDefender == XGAME::xSIZE_BIG )
		return true;
	if( sizeAttacker == XGAME::xSIZE_MIDDLE && sizeDefender == XGAME::xSIZE_SMALL )
		return true;
	if( sizeAttacker == XGAME::xSIZE_BIG && sizeDefender == XGAME::xSIZE_MIDDLE )
		return true;
	return false;
}

LPCTSTR GetIconAdjParam( XGAME::xtParameter adjParam )
{
	switch( adjParam ) {
	case XGAME::xADJ_ATTACK:	return PATH_UI( "common_etc_att.png" );
	case XGAME::xADJ_DEFENSE:	return PATH_UI( "common_etc_def.png" );
	case XGAME::xADJ_MAX_HP:	return PATH_UI( "common_etc_hp.png" );
	case XGAME::xADJ_MOVE_SPEED:	return PATH_UI( "common_etc_mpd.png" );
	case XGAME::xADJ_ATTACK_SPEED:	return PATH_UI( "common_etc_apd.png" );
	}
	return _T( "" );
}

LPCTSTR GetStrAdjParam( XGAME::xtParameter adjParam )
{
	switch( adjParam ) {
	case XGAME::xADJ_ATTACK:	return XTEXT( 80037 );
	case XGAME::xADJ_DEFENSE:	return XTEXT( 80039 );
	case XGAME::xADJ_MAX_HP:	return XTEXT( 80040 );
	case XGAME::xADJ_MOVE_SPEED:	return XTEXT( 80042 );
	case XGAME::xADJ_ATTACK_SPEED:	return XTEXT( 80041 );
	}
	return _T( "" );
}

int SerializeCrashDump( XArchive& ar, ID idAcc )
{
	XResFile res;
	LPCTSTR szFullpath = XE::MakeDocFullPath( _T( "" ), _T( "dump.bin" ) );
	if( res.Open( szFullpath, XBaseRes::xREAD ) ) {
//		XLOG( "open ok:[%s]", szFullpath );
		int size;
		char cDump[ 8192 ] = {0,}; // 덤프파일은 ansi이므로 이렇게 해도 됨.
		size = res.Size();
		if( size >= sizeof( cDump ) )
			size = sizeof( cDump ) - 1;
		if( XASSERT(size > 0 ) )
			res.Read( cDump, size );
		cDump[ size ] = 0;
		XLOG("send dump:sizeFile=%d,len=%d,%s", size, strlen(cDump), cDump );
		//
		ar << idAcc;
		ar << cDump;		
		res.ForceClose();
		return size;
	} else {
		// CONSOLE로 해서 파일에 안쓰도록 해야하는데 디;버깅땜에 임시로 XLOG로 함.
//		XLOG("dump not found:[%s]", szFullpath);
//		CONSOLE( "dump not found:[%s]", szFullpath );
		ar << 0;
	}
	return 0;
}

ID DeSerializeCrashDump( XArchive& ar, ID idAccOrig )
{
	ID idAcc;
	ar >> idAcc;
	if( idAcc ) {
		if( XBREAK(idAcc != idAccOrig) ) {
			return 0;
		}
		TCHAR szDump[ 0x10000 ];
		ar.ReadString( szDump );	
		char cDay[ 64 ];
		char cTemp[ 64 ];
		std::string strFullpath;
		char prefix[ 32 ];
		XE::MakeFilenameFromDay( cTemp, 64, "" );
		strcpy_s( cDay, cTemp );
		cDay[ 6 ] = 0;
		if( XE::IsHave( XE::GetPathWork() ) )	{
			strFullpath = XE::GetPathWorkA();
			strFullpath += cDay;
			XSYSTEM::MakeDir( strFullpath.c_str() );
			strFullpath = XE::GetPathWorkA();
			strFullpath += cDay;
			strFullpath += "/";
			sprintf_s( prefix, "dmp_%08lu", idAcc );
			strFullpath += prefix;
			strFullpath += ".txt";
			_tcscat_s( szDump, _T( "\r\n\r\n" ) );
			XE::WriteLogToFile( strFullpath.c_str(), szDump );
		}	
	}
	return idAcc != 0;
}

const char* GetstrEnumGrade( XGAME::xtGrade grade )
{
	switch( grade ) {
	case XGAME::xGD_COMMON:	return "xGD_COMMON";
	case XGAME::xGD_VETERAN: return "xGD_VETERAN";
	case XGAME::xGD_RARE:		return "xGD_RARE";
	case XGAME::xGD_EPIC:		return "xGD_EPIC";
	case XGAME::xGD_LEGENDARY:	return "xGD_LEGENDARY";
	case XGAME::xGD_NONE:	return "xGD_NONE";
	default:
		XBREAK(1);
		break;
	}
	return "";
}

const char* GetstrEnumGradeLegion( XGAME::xtGradeLegion grade )
{
	switch( grade ) {
	case XGAME::xGL_NORMAL:	return "xGL_NORMAL";
	case XGAME::xGL_ELITE:	return "xGL_ELITE";
	case XGAME::xGL_RAID:		return "xGL_RAID";
	case XGAME::xGL_NONE:	return "xGL_NONE";
	default:
		XBREAK(1);
		break;
	}
	return "";
}

const char* GetstrEnumTypeAtk( XGAME::xtAttack typeAtk )
{
	switch( typeAtk ) {
	case XGAME::xAT_TANKER:		return "xAT_TANKER";
		break;
	case XGAME::xAT_RANGE:	return "xAT_RANGE";
		break;
	case XGAME::xAT_SPEED:	return "xAT_SPEED";
		break;
	case XGAME::xAT_NONE:	return "xAT_NONE";
	default:
		XBREAK(1);
		break;
	}
	return "";
}

const char* GetstrEnumSizeUnit( XGAME::xtSize sizeUnit )
{
	switch( sizeUnit )
	{
	case XGAME::xSIZE_SMALL:		return "xSIZE_SMALL";
	case XGAME::xSIZE_MIDDLE:		return "xSIZE_MIDDLE";
	case XGAME::xSIZE_BIG:			return "xSIZE_BIG";
	case XGAME::xSIZE_NONE:	return "xSIZE_NONE";
	default:
		XBREAK(1);
		break;
	}
	return "";
}

const char* GetIdsRes( xtResource res )
{
	switch( res )	{
	case XGAME::xRES_WOOD:	return "wood";
	case XGAME::xRES_IRON:	return "iron";
	case XGAME::xRES_JEWEL: return "jewel";
	case XGAME::xRES_SULFUR: return "sulfur";
	case XGAME::xRES_MANDRAKE: return "mandrake";
	case XGAME::xRES_GOLD: return "gold";
	case XGAME::xRES_CASH: return "cash";
	case XGAME::xRES_GUILD_COIN: return "guildcoin";
	default:
		XBREAKF( 1, "unknown resource: idxUI=%d", res );
		break;
	}
	return "";
}

const char* GetIdsRes( xtIndexUI idxUI )
{
	switch( idxUI )	{
	case XGAME::xRES_WOOD:	return "wood";
	case XGAME::xRES_IRON:	return "iron";
	case XGAME::xRES_JEWEL: return "jewel";
	case XGAME::xRES_SULFUR: return "sulfur";
	case XGAME::xRES_MANDRAKE: return "mandrake";
	case XGAME::xIU_GOLD: return "gold";
	case XGAME::xIU_CASH: return "cash";
	case XGAME::xIU_GUILD_COIN: return "guildcoin";
	default:
		XBREAKF( 1, "unknown resource: idxUI=%d", idxUI );
		break;
	}
	return "";
}

/**
 @brief /// 각 자원들의 리소스 파일명을 돌려준다.(png/spr)
*/
LPCTSTR GetResourceSpr( XGAME::xtResource res ) 
{
// 	return _T("");
	switch( res )	{
	case XGAME::xRES_WOOD:	return _T( "ui_res_wood.spr" );	break;
	case XGAME::xRES_IRON:	return _T( "ui_res_iron.spr" );	break;
	case XGAME::xRES_JEWEL:	return _T( "ui_res_jewel.spr" );	break;
	case XGAME::xRES_SULFUR:	return _T( "ui_res_sulfur.spr" ); break;
	case XGAME::xRES_MANDRAKE:	return _T( "ui_res_mandrake.spr" );	break;
	case XGAME::xRES_GOLD:	return _T( "ui_res_gold.spr" );	break;
	case XGAME::xRES_CASH:	return _T( "ui_cash.spr" );	break;
	case XGAME::xRES_GUILD_COIN: return _T( "ui_guildcoin.spr" ); break;
	default:
		XBREAKF( 1, "unknown resource: res=%d", res );
		return _T("ui_res_question.spr");
		break;
	}
	return _T( "" );
}

#ifdef _CLIENT
xtPlatform GetPlatform() 
{
	return XGAME::PLATFORM_STORE;
// #if defined(_SOFTNYX)
// 	return xPL_SOFTNYX;
// #elif defined(_VER_ANDROID)
// 	return xPL_GOOGLE_STORE;
// #elif defined(_VER_IOS)
// 	return xPL_APPLE_STORE;
// #elif defined(WIN32)
// 	return xPL_GOOGLE_STORE;		// 안드로이드로 시뮬레이션.
// #else
// #error "unknown platform"
// #endif 
}
#endif // _CLIENT

LPCTSTR GetstrDEV_LEVEL()
{
#if _DEV_LEVEL == DLV_LOCAL
	return _T("DLV_LOCAL");
#elif	_DEV_LEVEL == DLV_DEV_CREW
	return _T("DLV_DEV_CREW");
#elif	_DEV_LEVEL == DLV_DEV_EXTERNAL
	return _T("DLV_DEV_EXTERNAL");
#elif	_DEV_LEVEL == DLV_OPEN_BETA
	return _T("DLV_OPEN_BETA");
#else
	return _T("DLV_UNKNOWN");
#endif 
}

// namespace XGAME
XE_NAMESPACE_END;

