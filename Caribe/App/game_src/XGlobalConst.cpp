#include "stdafx.h"
#include "XGlobalConst.h"
#include "XLegion.h"
#include "XAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
//////////////////////////////////////////////////////////////////////////
// 글로벌 상수 객체
XGlobalConst* XGlobalConst::s_pSingleton = nullptr;
XGlobalConst* _XGC = nullptr;		// 변수값 보기 힘들어서 추가함.

const XGlobalConst* XGlobalConst::sGet()
{
	if( s_pSingleton == nullptr ) {
		s_pSingleton = new XGlobalConst();
		_XGC = s_pSingleton;
	}
	return s_pSingleton;
}
XGlobalConst* XGlobalConst::sGetMutable()
{
	if( s_pSingleton == nullptr ) {
		s_pSingleton = new XGlobalConst();
		_XGC = s_pSingleton;
	}
	return s_pSingleton;
}
void XGlobalConst::sDestroySingleton( void ) 
{
	SAFE_DELETE( s_pSingleton );
}

int XGlobalConst::sGetGoldByTradeRes( XGAME::xtResource typeRes, int num ) 
{
	XBREAK( XGAME::IsInvalidResource( typeRes ) );
	return (int)( s_pSingleton->m_resourceRate[(int)typeRes] * num );
}

double FixDouble(double a, int n)
{
	return (floor(a* pow(100.0, n))) / pow(100.0, n);
}
void XGlobalConst::LoadConst()
{
	LoadParams();		// key/val 방식의 새로운 버전의 로더
//	LoadDynaParam( &m_Param );

	// 누락된 enum이 없는지 검사하기위해 한번씩 콜함.
	for( int i = 0; i < XGAME::xAE_MAX; ++i )
		XGAME::GetstrEnumActionEvent( (XGAME::xtActionEvent)i );
//	XLua *pLua = m_pGameCommon->GetpLua();

//  	m_rateMainStorageLoss = 0.2f;
// 	m_rateLocalStorageLoss = 0.5f;
	m_aryRateMainLoss = { 0.1f, 0.2f, 0.4f, 0.5f, 0.6f };
	m_aryRateLocalLoss = { 0.05f, 0.1f, 0.2f, 0.3f, 0.4f };
	m_rateJewelStorageLoss = 0.3f;
	//
// 	m_SampleInt = GetVal<int>( _T("sample_int") );
// 	m_SampleFloat = (float)GetVal<double>( _T("sample_float"), _T("group") );
// 	m_SampleLuaFloat = pLua->GetVar<float>( "SAMPLE01" );
// 	m_strSampleLua = pLua->GetVar<char*>( "SAMPLE_STR" );
	int shopsellrate = GetVal<int>(_T("shop_sell_rate"), _T("const"));;
	m_rateShopSell = float(shopsellrate / 100.f);
	XBREAK( m_rateShopSell > 1.f );
//	m_expBattle = GetVal<int>(_T("battle_exp"), _T("const"));
//	m_expBattlePerHero = GetVal<int>(_T("battle_exp_per_hero"), _T("const"));
// 	m_goldLowSummon = GetVal<int>(_T("gold_low_summon"), _T("const"));
// 	m_goldHighSummon = GetVal<int>(_T("gold_high_summon"), _T("const"));
	m_cashHighSummon = GetVal<int>(_T("cash_high_summon"), _T("const"));	
	m_cashShopReload = GetVal<int>(_T("cash_shop_refresh"), _T("const"));
	m_armoryCallInterval = GetVal<int>(_T("shop_call_interval"), _T("const"));

// 	m_aryGachaChanceNormal.resize(6);
// 	m_aryGachaChancePremium.resize(6);
	float 
	value = (float)GetVal<double>( _T( "grade1_normal" ), _T( "hero_gatcha" ) );
	m_aryGachaChanceNormal[ 1 ] = value;
	value = (float)GetVal<double>( _T( "grade2_normal" ), _T( "hero_gatcha" ) );
	m_aryGachaChanceNormal[ 2 ] = value;
// 	value = (float)GetVal<double>( _T( "grade3_piece_normal" ), _T( "hero_gatcha" ) );
// 	m_aryGachaChanceNormal[ 2 ] = value;
	value = (float)GetVal<double>( _T( "grade3_normal" ), _T( "hero_gatcha" ) );
	m_aryGachaChanceNormal[ 3 ] = value;
// 	value = (float)GetVal<double>( _T( "grade4_piece_normal" ), _T( "hero_gatcha" ) );
// 	m_aryGachaChanceNormal[ 4 ] = value;
	value = (float)GetVal<double>( _T( "grade4_normal" ), _T( "hero_gatcha" ) );
	m_aryGachaChanceNormal[ 4 ] = value;
	value = (float)GetVal<double>( _T( "grade5_normal" ), _T( "hero_gatcha" ) );
	m_aryGachaChanceNormal[ 5 ] = value;


	value = (float)GetVal<double>( _T( "grade1_premium" ), _T( "hero_gatcha" ) );
	m_aryGachaChancePremium[ 1 ] = value;
	value = (float)GetVal<double>( _T( "grade2_premium" ), _T( "hero_gatcha" ) );
	m_aryGachaChancePremium[ 2 ] = value;
	value = (float)GetVal<double>( _T( "grade3_premium" ), _T( "hero_gatcha" ) );
	m_aryGachaChancePremium[ 3 ] = value;
// 	value = (float)GetVal<double>( _T( "grade4_piece_premium" ), _T( "hero_gatcha" ) );
// 	m_aryGachaChancePremium[ 4 ] = value;
	value = (float)GetVal<double>( _T( "grade4_premium" ), _T( "hero_gatcha" ) );
	m_aryGachaChancePremium[ 4 ] = value;
	value = (float)GetVal<double>( _T( "grade5_premium" ), _T( "hero_gatcha" ) );
	m_aryGachaChancePremium[ 5 ] = value;

	m_resourceRate[XGAME::xRES_WOOD] = (float)GetVal<double>(_T("wood_rate"), _T("trade"));
	m_resourceRate[XGAME::xRES_IRON] = (float)GetVal<double>(_T("iron_rate"), _T("trade"));
	m_resourceRate[XGAME::xRES_JEWEL] = (float)GetVal<double>(_T("jewel_rate"), _T("trade"));
	m_resourceRate[XGAME::xRES_SULFUR] = (float)GetVal<double>(_T("sulfur_rate"), _T("trade"));
	m_resourceRate[XGAME::xRES_MANDRAKE] = (float)GetVal<double>(_T("mandrake_rete"), _T("trade"));

	// 유닛크기별 얻는 용맹포인트.
	{
		int bp = 0;
		m_aryAddBravePoint.Add( 0 );
		bp = GetVal<int>( _T( "brave_point_small" ), _T( "const" ) );
		m_aryAddBravePoint.Add( bp );
		bp = GetVal<int>( _T( "brave_point_middle" ), _T( "const" ) );
		m_aryAddBravePoint.Add( bp );
		bp = GetVal<int>( _T( "brave_point_big" ), _T( "const" ) );
		m_aryAddBravePoint.Add( bp );
	}

	// 무역상 & 무기상 & 창고(귀환시간은 XGlobalConst::GetsecTraderReturn())
	m_traderRecallItem = GetVal<int>(_T("trader_recall_item"), _T("const"));
//	m_traderRecallGem = GetVal<int>(_T("trader_recall_gem"), _T("const"));
	m_armoryRecallItem = GetVal<int>(_T("armory_recall_item"), _T("const"));
	m_armoryRecallGem = GetVal<int>(_T("armory_recall_gem"), _T("const"));
	m_storageLockItem = GetVal<int>(_T("storagy_lock_item"), _T("const"));
	m_storageLockGem = GetVal<int>(_T("storagy_lock_gem"), _T("const"));

// 	m_secSubscribeCycle = GetVal<int>(_T("subscribe_gem_interval"), _T("const"));
	m_secSubscribeCycle = GetInt( "sec_subscribe_cycle" );
	m_daysSubscribe = GetInt( "days_subscribe" );

//	m_tradeCallInterval = GetVal<int>(_T("trade_call_interval"), _T("const"));
	m_shopCallInerval = GetVal<int>(_T("shop_call_interval"), _T("const"));

	m_LogLevel = GetVal<int>(_T("log_level"), _T("const"));
	m_bravePointPerTech = GetVal<int>(_T("const_brave_point"), _T("const"));
	m_rateDropLastChance = GetFloat("drop_rate_last_chance", "const" ) / 100.f;
	m_apPerOnce = GetInt( "ap_per_once" );
	XBREAK( m_apPerOnce == 0 );
	m_secByAP = GetInt( "sec_by_ap" );
	XBREAK( m_secByAP == 0 );
//	m_secByAP = 60;
	m_expPerBook = GetInt( "exp_per_book" );
	m_goldChangePlayer = GetInt( "gold_change_player" );
	m_cashChangePlayer = GetInt( "cash_change_player" );
	m_secMaxBattleSession = GetInt( "sec_max_battle_session" );
	m_secMaxBattle = GetInt( "sec_max_battle" );
	m_secMaxBattleReady = GetInt( "sec_max_battle_ready" );
//	auto cFile = GetVal2<const char*>( "bg_normal" );
	{
		m_aryBgsBattle[ XGAME::xSPOT_CASTLE ] = GetTString( "bg_castle" );
		m_aryBgsBattle[ XGAME::xSPOT_JEWEL ] = GetTString( "bg_jewel" );
		m_aryBgsBattle[ XGAME::xSPOT_SULFUR ] = GetTString( "bg_sulfur" );
		m_aryBgsBattle[ XGAME::xSPOT_MANDRAKE ] = GetTString( "bg_mandrake" );
		m_aryBgsBattle[ XGAME::xSPOT_NPC ] = GetTString( "bg_npc" );
		m_aryBgsBattle[ XGAME::xSPOT_CAMPAIGN ] = GetTString( "bg_campaign" );
		m_aryBgsBattle[ XGAME::xSPOT_DAILY ] = GetTString( "bg_daily" );
		m_aryBgsBattle[ XGAME::xSPOT_SPECIAL ] = GetTString( "bg_special" );
		auto szDefault = GetTString( "bg_default" );
		for( int i = 0; i < XGAME::xSPOT_MAX; ++i ) {
			if( m_aryBgsBattle[i].empty() )
				m_aryBgsBattle[ i ] = szDefault;
		}
	}
	m_rateDropSoulByCastle = GetFloat( "rate_drop_soul_castle" ) / 100.f;
	XBREAK( m_rateDropSoulByCastle <= 0 || m_rateDropSoulByCastle > 100.f );
// 	m_aryLevelsUnlockUnitForNotSmall = { 12, 14, 16, 19, 22, 25 };
	m_aryLevelsUnlockUnitForNotSmall = { 
		XGAME::LEVEL_UNLOCK_UNIT, 
		XGAME::LEVEL_UNLOCK_UNIT + 3,		// 12
		XGAME::LEVEL_UNLOCK_UNIT + 6,		// 15
		XGAME::LEVEL_UNLOCK_UNIT + 9,		// 18
		XGAME::LEVEL_UNLOCK_UNIT + 12,	// 21
		XGAME::LEVEL_UNLOCK_UNIT + 15		// 24
	};	
	
	LoadCashItems( "cashitems", &m_aryCashitems );
	LoadCashItems( "cashitems_softnyx", &m_aryCashitemsBySoftnyx );
	LoadEtcItems();
//	GetAryTString( "guild_shop", &m_aryGuildShop );
	auto node = FindNode2( "guild_shop" );
	if( !node.IsEmpty() ) {
		auto nodeChild = node.GetFirst();
		while( !nodeChild.IsEmpty() ) {
			xItemGuildShop slot;
			slot.idsItem = nodeChild.GetTString( "item" );
			slot.cost = nodeChild.GetInt( "cost" );
			m_aryGuildShop.push_back( slot );
			nodeChild = nodeChild.GetNext();
		}
	}
	//
	m_rateKillDrop = GetFloat( "rate_drop_kill" ) / 100.f;
	XBREAK( m_rateKillDrop < 0 || m_rateKillDrop > 1.f );
	m_rateScalpDrop = GetFloat( "rate_drop_scalp") / 100.f;
	XBREAK( m_rateScalpDrop < 0 || m_rateScalpDrop > 1.f);
	m_cashPerAP = GetFloat( "gem_per_ap" );
	XBREAK( m_cashPerAP == 0 );

	m_gemFillDailyTry = GetInt( "gem_fill_daily" );
	m_rateFillDailyTry = GetFloat( "rate_fill_daily" );
	m_numEnterDaily = GetInt( "num_try_daily" );
	m_arySizeCost.Add(0);
	GetAryInt( "size_cost", &m_arySizeCost );
	XBREAK( m_arySizeCost.Size() != xSIZE_MAX );
} // XGlobalConst::LoadConst()

/** //////////////////////////////////////////////////////////////////
 @brief key /val 방식으로 읽어들이고 검색하는 다이나믹 파라메터
*/
void XGlobalConst::LoadDynaParam( XParamObj2* pOut )
{
// 	XEXmlNode nodeRoot = FindNode( "global" );
// 	if( nodeRoot.is )
}

/**
 @brief global.txt
*/
bool XGlobalConst::LoadTable( LPCTSTR szTxt )
{
	CToken token;
	bool bRet = true;
	// 국가별폴더부터 읽어보고 없으면
	if( token.LoadFile( XE::MakePathLang( DIR_PROP, szTxt ), XE::TXT_UTF16 ) == xFAIL ) {
		// 유니버셜 폴더에서 읽음.
		if( token.LoadFile( XE::MakePath( DIR_PROP, szTxt ), XE::TXT_UTF16 ) == xFAIL ) {
			XERROR( "%s read failed", szTxt );
			return false;
		}
	}
	m_rateEquipDrop = token.GetNumberF() / 100.f;
	m_frEquipDropMax = token.GetNumber();
	// 레벨대별 루프
	for( int lvSec = 0; lvSec < m_aryDropRatePerLevel.GetMax(); ++lvSec ) {
		xEquipDrop drop;
		drop.lvStart = token.GetNumber();
		drop.lvEnd = token.GetNumber();
		drop.aryDropRate.Add( 0 );		// idx 0
		int sum = 0;
		for( int i = 1; i < XGAME::xGD_MAX; ++i ) {
			int fraction = token.GetNumber();
			drop.aryDropRate.Add( fraction );
			sum += fraction;
		}
		XBREAKF( sum != m_frEquipDropMax, "%s:%s", szTxt, _T("경고: 확률테이블의 합은 100%여야 합니다.") );
		m_aryDropRatePerLevel.Add( drop );
	}
	m_expBattle = token.GetNumber();
	XASSERT( m_expBattle > 0 && m_expBattle < 0xffff );
	m_expBattlePerHero = token.GetNumber();
	XASSERT( m_expBattlePerHero > 0 && m_expBattlePerHero < 0xffff );
	// 지역 오픈 비용 테이블
	bRet = LoadAreaCost( token );
	// npc군단 생성 테이블
	bRet = LoadLegionTable( token );
#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
	// 더미생성 테이블
	bRet = LoadDummyTable( token );
#endif // #if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
	
	return bRet;
}
/**
 @brief global.txt의 지역가격 테이블 읽기
*/
bool XGlobalConst::LoadAreaCost( CToken& token )
{
	int lvLast = 0;
	for( int i = 0; i < XGAME::MAX_AREA_LEVEL; ++i ) {
		int lv = token.GetNumber();
		if( XBREAK( lv != lvLast + 1 ) ) {
			CONSOLE("지역레벨이 순차적이지 않음.");
			return false;
		}
		int cost = token.GetNumber();
		m_aryAreaCost[ lv ] = cost;
		lvLast = lv;
	}
	return true;
}
/**
 @brief npc군단 생성용 테이블
*/
bool XGlobalConst::LoadLegionTable( CToken& token )
{
	int lvLast = 0;
	for( int i = 0; i < XGAME::MAX_AREA_LEVEL; ++i ) {
		int lv = token.GetNumber();
		if( XBREAK( lv != lvLast + 1 ) ) {
			CONSOLE( "군단레벨이 순차적이지 않음." );
			return false;
		}
		xLegionTable tbl;
		tbl.m_lvLegion = lv;
		tbl.m_numSquad = token.GetNumber();
		XBREAK( tbl.m_numSquad <= 0 || tbl.m_numSquad > XGAME::MAX_SQUAD );
		tbl.m_lvHero = token.GetNumber();
		tbl.m_gradeHero = (XGAME::xtGrade) token.GetNumber();
		XBREAK( XGAME::IsInvalidGrade( tbl.m_gradeHero ) );
		tbl.m_lvSquad = token.GetNumber();
		m_aryLegionTable[ lv ] = tbl;
		lvLast = lv;
	}
	return true;
}

#if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))
/**
 @brief 더미계정생성용 테이블
*/
bool XGlobalConst::LoadDummyTable( CToken& token )
{
	CONSOLE("더미계정 테이블 loading.....");
	XBREAK( m_aryDummy.size() == 0 );
	int lvLast = 0;
	for( int i = 0; i < XGAME::MAX_ACC_LEVEL; ++i ) {
		int lv = token.GetNumber();
		if( XBREAK( lv != lvLast + 1 ) ) {
			CONSOLE( "레벨이 순차적이지 않음." );
			return false;
		}
		xDummy tbl;
		tbl.m_lvAcc = lv;
		tbl.m_numSquad = token.GetNumber();
		XBREAK( tbl.m_numSquad <= 0 || tbl.m_numSquad > XGAME::MAX_SQUAD );
		tbl.m_lvHero = token.GetNumber();
		tbl.m_gradeHero = (XGAME::xtGrade)token.GetNumber();
		XBREAK( XGAME::IsInvalidGrade(tbl.m_gradeHero) );
		tbl.m_lvSquad = token.GetNumber();
		tbl.m_lvSkill = token.GetNumber();
		tbl.m_numAbilPerUnit = token.GetNumber();
		tbl.m_numEquip = token.GetNumber();
		tbl.m_gradeEquip = (XGAME::xtGrade)token.GetNumber();
		XBREAK( XGAME::IsInvalidGrade( tbl.m_gradeEquip ) );
		//
		m_aryDummy[ lv ] = tbl;
		lvLast = lv;
	}
	CONSOLE( "더미계정 테이블 finish" );
	return true;
}
#endif // #if defined(_XSINGLE) || (defined(_DEV) && defined(_GAME_SERVER))

void XGlobalConst::LoadCashItems( const char* cKey, XVector<cashItem>* pOutAry ) const
{
	XEXmlNode root = FindNode("global");
	if (!root.IsEmpty()) {
		// 일반젬 목록
		XEXmlNode cashItemsNode = root.FindNode( cKey );
		XBREAK( cashItemsNode.IsEmpty() );
		//
		if (!cashItemsNode.IsEmpty()) {
			XEXmlNode itemNode = cashItemsNode.GetFirst();
			//
			if (!itemNode.IsEmpty()) {
				//
				while (!itemNode.IsEmpty()) {
					_tstring m_productID = itemNode.GetTString("productID");
					_tstring type = itemNode.GetTString("type");
					//int typeInt = 0;
					XGAME::xtCashType typeGoods;
					if (type == _T("nomal"))
						typeGoods = XGAME::xCT_NORMAL;
					else if (type == _T("subscribe"))
						typeGoods = XGAME::xCT_SUBSCRIBE;
					int gem = itemNode.GetInt("gem");
	//				int day = itemNode.GetInt("day");
	//				int daygem = itemNode.GetInt("daygem");
					_tstring contry = itemNode.GetTString("contry");
					float price = itemNode.GetFloat("price");
					int text = itemNode.GetInt("text");
					_tstring img = itemNode.GetTString("img");
					int x = itemNode.GetInt("posx");
					int y = itemNode.GetInt("posx");
					cashItem tempItem{ m_productID
															, typeGoods
															, gem
//															, day
//															, daygem
															, contry
															, price
															, text
															, img
															, XE::VEC2(x, y) };
					pOutAry->push_back(tempItem);
					itemNode = itemNode.GetNext();
				}
			}
		}
	}
}

const std::vector<XGlobalConst::cashItem>& 
XGlobalConst::GetAryCashItems( XGAME::xtPlatform platform ) const 
{
	XBREAK( platform == xPL_NONE );
	if( platform == xPL_SOFTNYX )
		return m_aryCashitemsBySoftnyx;
	return m_aryCashitems;
}

void XGlobalConst::LoadEtcItems()
{
	XEXmlNode root = FindNode( "global" );
	if( !root.IsEmpty() ) {
		XEXmlNode etcItemsNode = root.FindNode( "etcitems" );
		if( !etcItemsNode.IsEmpty() ) {
			XEXmlNode itemNode = etcItemsNode.GetFirst();
			if( !itemNode.IsEmpty() ) {
				while( !itemNode.IsEmpty() ) {
					int ID = itemNode.GetInt( "ID" );

					m_arrEtcItems.push_back( ID );
					itemNode = itemNode.GetNext();
				}
			}
		}
	}
}

const XGlobalConst::cashItem* 
XGlobalConst::GetCashItem(const _tstring& productID, XGAME::xtPlatform platform ) const
{
	for ( const auto& item : GetAryCashItems(platform) ) {
		if (item.m_productID == productID)
			return &item;
	}
	return nullptr;
}

/**
 @brief 계정레벨별 표준전투력을 미리 구한다.
*/
void XGlobalConst::MakeTablePowerPerLevel()
{
#if defined(_CLIENT) || defined(_GAME_SERVER)
	int maxLevel = XGAME::GetLevelMaxAccount();
	XBREAK( maxLevel == 0 );
	m_aryMaxPowerPerLevel.resize( maxLevel + 1 );
	m_aryMaxPowerPerLevel[0] = 0;
	for( int i = 1; i <= maxLevel; ++i ) {
		int max = XLegion::sGetMilitaryPowerMax( i );
		m_aryMaxPowerPerLevel[i] = max;    // 디버깅용으로 맥스값만 저장
//		m_aryAvgPowerPerLevel[ i ] = (int)( max * 0.75f );    // 최대값과 최대값의 50%를 최하로 보고 그 중간인 75%를 평균치로 산정함.
	}
#endif // 
#ifdef _GAME_SERVER
	// npc전투력 테이블 미리 생성
	for( int lv = 1; lv <= XGAME::MAX_NPC_LEVEL; ++lv ) {
		XGAME::xLegionParam info;
		auto pLegion = XLegion::sCreateLegionForNPC( lv, 0, info );
		auto spLegion = XSPLegion( pLegion );
		int power = XLegion::sGetMilitaryPower( spLegion );
		m_aryNpcPowerPerLevel[ lv ] = power;
	}
#endif

}

/**
 @brief powerBase를 기준으로 지정한 난이도(색등급)에 해당하는 가장 높은 레벨을 얻는다.
 @param diffLv 색등급. -2~2
*/
int XGlobalConst::GetNpcLevelByColor( int powerBase, int diffLv ) const
{
#if defined(_GAME_SERVER)
	if( powerBase == 0 )
		return 1;
	int lvNpc = 0;
	// 지정된 색등급의 전투력을 계산한다.
	int powerSpeci = (int)(powerBase * XGAME::GetMultiplyPower( diffLv ));
	//
	for( int i = 1; i <= GetMaxNpcLevel(); ++i ) {
		// 각 npc레벨별로 전투력을 얻어서 powerSpeci를 넘지않는 가장 큰레벨을 구한다.
		int power = GetNpcPowerWithLevel( i );
		if( power > powerSpeci )
			return i - 1;
	}
#endif 
	XBREAK(1);
	return 0;
}

void XGlobalConst::SaveMaxPowerTable() const
{
#if defined(_CLIENT)
	FILE *fp = nullptr;
	fopen_s( &fp, "maxPower.txt", "wt" );
	if( fp )
	{
		for( int i = 1; i <= 50; i ++)
			fprintf_s( fp, "%d\t%d\t%d\n", i, 
									XAccount::sGetMaxTechPoint(i), 
									m_aryMaxPowerPerLevel[i] );
		fclose(fp);
	}
#endif // 
}
/**
 @brief 무역상이 다시 돌아오는 시간 계산
 행동력.xlsx/무역상귀환시간
*/
int XGlobalConst::GetsecTraderReturn( int lvAcc ) const
{
	float sec = (::pow( lvAcc / 3.f, 2 ) * 300.f) + 250;
	int secRet = (int)(sec / 4.f);
	if( lvAcc <= 8 )
		secRet /= 2;
	return secRet;
//	return (int)( sec ) / 4;
// 	return (int)(sec) / 2;
}

const XGlobalConst::xLegionTable& XGlobalConst::GetLegionTable( int lvLegion ) const
{
	if( XASSERT(lvLegion > 0) ) {
		int lv = lvLegion;
		if( lv >= (int)m_aryLegionTable.size() )
			lv = (int)m_aryLegionTable.size() - 1;
		return m_aryLegionTable[ lv ];
	}
// 		if( XASSERT(lvLegion > 0 && lvLegion <= (int)(m_aryLegionTable.size()-1)) ) {
// 			return m_aryLegionTable[ lvLegion ];
// 		}
	XBREAK( m_aryLegionTable.size() == 0 );
	return m_aryLegionTable[0];
}

void XGlobalConst::Serialize( XArchive& ar ) const
{
	ar << (WORD)m_gemFillDailyTry;
	ar << (char)m_numEnterDaily;
	ar << (char)0;
	ar << (float)m_rateFillDailyTry;
	// 압축할때 용량이 너무 작으니까 문제가 생겨서 더미를 넣음.
	for( int i = 0; i < 8; ++i ) {
		ar << 0;
	}
}
void XGlobalConst::DeSerialize( XArchive& ar, int )
{
	WORD w0;
	char c0;
	ar >> w0;		m_gemFillDailyTry = w0;
	ar >> c0;		m_numEnterDaily = c0;
	ar >> c0;
	ar >> m_rateFillDailyTry;
	for( int i = 0; i < 8; ++i ) {
		int i0;
		ar >> i0;
	}
}
