#pragma once
#include "XSpot.h"
#include "XPropWorld.h"
//#include "XPropCamp.h"
#include "XArchive.h"
#include "etc/XTimer2.h"
//#include "XLegion.h"
//#include "XCampObj.h"
#include "XPropCloud.h"
#include "XSpotH.h"


#ifdef _CLIENT
class XMsgPusher;
#endif // _CLIENT
////////////////////////////////////////////////////////////////
/*
최초 오픈되어 아직 매치안되어 ?인 상태(클리어상태) idOwner = 0 Level = 0
유저가 매칭되어 있는 상태 idOwner != 0 Level != 0
NPC가 매칭되어 있는 상태 idOwner = 0; Level != 0
내꺼인 상태 idOwner != 0 Level != 0
뺏긴상태(클리어상태) idOWner = 0 Level = 0
*/
class XSpotCastle : public XSpot
{
public:
	static xSec s_secResetHeroSoul;		// 영혼석타이머 리셋된 시간.
private:
	//
	ID m_idOwner = 0;		// 위 주석 참조
	std::string m_strcFbUserId;			// 페북 유저의 경우 페북 아이디
	XTimer2 m_timerOwn;	// 소유타이머. 자원지를 소유한 순간부터 작동되기 시작해서 뺏길때까지 계속 흘러간다.
	XTimer2 m_timerCalc;	// 정산타이머. 자원지를 소유하고 정산하고나면 리셋된다. 클라이언트에서는 리젠타이머로 사용한다.
	std::vector<XSpot::xLOCAL_STORAGE> m_aryLocalStorage;	// 지역창고
	int m_numLose = 0;		// 패배횟수
	int m_powerUpperLimit = 0x80000000;	// 상한선
	ID m_idHeroDrop = 0;			// 이 스팟에서 드랍될 영웅(x시간마다 다른 영웅으로 교체된다)
	_tstring m_strHello;
#ifdef _CLIENT
//	XE::xImage m_imgProfile;		// 프로필 이미지
	XSurface* m_psfcProfile = nullptr;
	CTimer m_timerSec;
	bool m_bAttacked = false;		// 매치되면 첨에 true가 된다.
#endif // _CLIENT
	//
	void Init() {}
	void Destroy() {
#ifdef _CLIENT
		SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
#endif // _CLIENT
	}
	//
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotCastle( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotCastle( XWorld *pWorld, XPropWorld::xCASTLE* pProp, XDelegateSpot *pDelegate=nullptr );
	virtual ~XSpotCastle() { Destroy(); }
	//
//	GET_ACCESSOR( ID, idOwner );
	ID GetidOwner() const override {
		return m_idOwner;
	}
	GET_SET_ACCESSOR( int, numLose );
	GET_SET_ACCESSOR( int, powerUpperLimit );
	GET_SET_ACCESSOR_CONST( const std::string&, strcFbUserId );
//	GET_SET_ACCESSOR_CONST( const _tstring&, strHello );
	_tstring GetstrHello() const override {
		return m_strHello; 
	}
	void SetstrHello( const _tstring& strHello ) override {
		m_strHello = strHello;
	}
#ifdef _CLIENT
//	GET_ACCESSOR_CONST( const XE::xImage&, imgProfile );
	GET_ACCESSOR_CONST( XSurface*, psfcProfile );
	GET_SET_ACCESSOR_CONST( bool, bAttacked );
#endif // _CLIENT
	inline bool IsHavePowerUpperLimit() {
		return m_powerUpperLimit != 0x80000000;
	}
	inline bool IsHaveNotPowerUpperLimit() {
		return !IsHavePowerUpperLimit();
	}
	void AddLose( int add ) {
		m_numLose -= add;
		if( m_numLose < 0 )
			m_numLose = 0;
	}
	bool IsPC() const override { 
//		return m_idOwner != 0 && GetLevel() != 0;
		return true;		// 이제 성스팟은 npc가 들어올수 없음.
	}
	bool IsNpc() const override { 
		return false;
	}
	bool IsVsNPC() const override {
		return IsNpc();
	}
	bool IsQuestion() const override {
		return m_idOwner == 0 && GetLevel() == 0;
	}
	// 점유시간 타이머로 쓴다.
	XTimer2& GetTimerCalc() {
		return m_timerCalc;
	}
	void ResetTimerCalc( void ) {
		GetTimerCalc().Reset();
	}
	XTimer2& GetTimerOwn() {
		return m_timerOwn;
	}
	// 리젠시간이 지났는가.
	BOOL IsRegenOver() {
		return GetTimerCalc().IsOver();
	}
	// 스팟을 유저(플레이어 자신포함)의 소유로 만든다.
	void SetPlayerOwner( ID idAcc, LPCTSTR szName, BOOL isRespawn );
	/**
		현재까지의 타이머로 자원을 정산해서 그 양을 돌려준다.
	*/
	void CalculateResource( ID idAcc, xSec secAdjustOffline ) override;
	void CalculateResourceByPassTime( ID idAcc, xSec secPassTime, xSec secAdjustOffline );
	void Process( float dt );
	GET_ACCESSOR_CONST( const std::vector<XSpot::xLOCAL_STORAGE>&, aryLocalStorage );
	void LossLocalStorage( float rateLoss, std::vector<int> *pOutAry ) override {
		for( auto& res : m_aryLocalStorage ) {
			auto loss = res.Loss( rateLoss );
			if( pOutAry && XGAME::IsValidResource(res.m_Type) )
				(*pOutAry)[ res.m_Type ] += (int)loss;
		}
	}
	//
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xCASTLE*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
  XPropWorld::xCASTLE* GetpProp() {
    auto pBaseProp = GetpBaseProp();
    return static_cast<XPropWorld::xCASTLE*>( pBaseProp );
  }
	void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
// 	virtual void SerializeUpdate( XArchive& ar );
// 	virtual void DeSerializeUpdate( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
public:
	void SetMaxLocalStorage();
	void SerializeLocalStorage( XArchive& ar );
	void DeSerializeLocalStorage( XArchive& ar, int verWorld );
	//
	void AdjustTimer( xSec secAdjust ) override {
		XSpot::AdjustTimer( secAdjust );
		m_timerOwn.AddTime( secAdjust );
		m_timerCalc.AddTime( secAdjust );
	}
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pUser, ID idAcc, xSec secLastCalc ) override;
	// 지역창고에 자원을 쌓는다.
	void DoStackToLocal( float num, bool bFullCheck ) override {
		// 성스팟은 생산가능한 자원을 같은양으로 쌓는다.
		for( auto& res : m_aryLocalStorage )
			res.Add( num, bFullCheck );
	}
	void MoveResourceLocalToMain( XSPAcc spAccount, XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry=nullptr ) override;
	void ClearLocalStorage() override {
		for( auto& res : m_aryLocalStorage )
			res.numCurr = 0;
	}
	_tstring GetStrLocalStorage() const override {
		_tstring str = _T("local:\n");
		for( auto& res : m_aryLocalStorage ) {
			str += XE::Format( _T( "[%s]%.2f/%d\n" ), XGAME::GetStrResourceForDebug( res.m_Type ),
													res.numCurr, (int)res.maxSize );
		}
		return str;
	}
	void GetStrProduce( _tstring& strOut ) {
		for( auto& res : GetpProp()->m_aryProduce ) {
			int producePerHour = (int)(res.num * 60.f);
			// %s/시간
			strOut += XE::Format( XTEXT(80201), XE::NumberToMoneyString( producePerHour ) );
			strOut += _T("\n");
		}
	}
	float GetLocalStorageMax( XGAME::xtResource typeRes = XGAME::xRES_NONE ) const {
		for( auto& res : GetaryLocalStorage() ) {
			if( res.m_Type == typeRes ) {
				return res.maxSize;
			}
		}
		return 0;
	}
	float GetLocalStorageAmount( XGAME::xtResource typeRes ) const override {
		for( auto& res : GetaryLocalStorage() ) {
			if( res.m_Type == typeRes ) {
				return res.numCurr;
			}
		}
		return 0;
	}
	// 가장적게 쌓인걸 기준으로 계산함.
	float GetLocalStoragePercent() const override { 
		float percent = 100.f;
		XBREAK( m_aryLocalStorage.size() == 0 );
		for( auto& res : m_aryLocalStorage ) {
			if( XASSERT(res.maxSize > 0) ) {
				float p = res.numCurr / res.maxSize;
				if( p < percent)
					percent = p;
			}
		}
		return percent;
	}
	void Initialize( XSPAcc spAcc ) override;
	BOOL IsActive( void ) const override { 
		return TRUE;	// 성스팟은 비활성상태가 없는것 아닌가?
	}
	void ResetSpot() override {
		ResetSpawn();
	}
	void DoSpawnSpot() override {
		ResetSpawn();
		GettimerSpawnMutable().AddTime( 99999.f );
	}
	void ResetSpawn();
	void OnSpawn( XSPAcc spAcc ) override;
	void AddLocalStorageResources( xResourceAry *pOutAry ) override {
		XBREAK( pOutAry->size() == 0 );
		for( auto& res : m_aryLocalStorage ) {
			if( XASSERT(XGAME::IsValidResource(res.m_Type)) ) 
				(*pOutAry)[ res.m_Type ] += (int)res.numCurr;
		}
	}
	void ClearSpot() override;
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat ) override;
	bool IsEnemy() const override;
// 	ID GetidEnemy() override {
// 		return m_idOwner;
// 	}
	void DoDropRegisterItem( int level ) override;
#ifdef _CHEAT
	void CreateLegion( XSPAcc spAccount ) override;
#endif // _CHEAT
#if defined(_CLIENT) || defined(_GAME_SERVER)
	void GetResLootStandard( xResourceAry *pOutAry, int powerEnemy, int lvEnemy );
//	void SetDropRes( XSPAcc spAcc, int power, int level );
	void SetDropRes2( XSPAcc spAcc, int power, int level );
	void AdjustLootByLucky( xResourceAry *pAryMutable );
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

#ifdef _CLIENT
	float RegenResource( float sec, ID idPlayer, std::vector<XGAME::xRES_NUM>* pOut );
	float RegenResource2( float secCycle, ID idPlayer, XMsgPusher* pMsgPusher );
	void cbOnRecvProfileImage( const XE::xImage& imgInfo );
	void DoRequestProfileImage();
#endif // _CLIENT
	xSpot::xtStateCastle GetState( ID idAccPlayer ) const {
		XBREAK( idAccPlayer == 0 );
		if( m_idOwner == 0 )
			return xSpot::xSC_NO_MATCH;
		else
		if( m_idOwner != idAccPlayer )
			return xSpot::xSC_ENEMY;
		return xSpot::xSC_MY;
	}
}; // spot castle
