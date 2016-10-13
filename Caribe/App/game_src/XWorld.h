#pragma once
//#include "XSpots.h"
//#include "XTimeEventTimer.h"
//#include "XPropWorld.h"
#include "XPropCloud.h"
/**
 월드 시리얼라이즈 포맷버전
 ver13
  전투력
 ver14
  연패횟수
 ver16 
  스팟 드랍리스트
 ver17
  스팟랜덤시드값
 ver18
  루팅어레이
 ver19
  스팟 이름 삭제
 ver20
  일부스팟만 이름 저장
 ver22
  m_powerHighest추가
 ver23
  유황스팟 encounter리스트
 ver25
  광산침공시뮬을 위해 데이타 추가
 ver26
  성스팟 영혼석드랍
 ver27
  성스팟 페북 아이디 추가.
 ver28
  성스팟 szHello추가
 ver29
  스팟바이너리 압축.
 ver30
  요일스팟 리뉴얼.
 ver31
  요일스팟 시리얼라이즈 변경 
 ver32
  개인레이드 스팟
*/
#define VER_WORLD_SERIALIZE		32

//class XTimeEventTimer;
class XDelegateSpot;
class XAccount;
class XPropCloud;
class XSpot;
class XSpotCastle;
class XSpotJewel;
class XSpotSulfur;
class XSpotMandrake;
class XSpotNpc;
class XSpotVisit;
class XSpotCampaign;
class XSpotCash;
class XSpotDaily;
//class XSpotSpecial;
/**
 월드맵의 동적객체. DB에 저장되는 데이타
*/
////////////////////////////////////////////////////////////////
class XWorld
{
public:
	// 월드의 크기
	static XE::VEC2 sGetSize() {
		return XE::VEC2(2048, 2048);
	}
private:
	// 이제 PropWorld의 어레이 크기와 일치하기때문에 널이 들어가있는일은 없다.
	// 자원지+유저+NPC 스팟 통합 어레이
	XList4<XSpot*> m_listSpots;		// 이제 스팟 열리는 순간에만 스팟객체를 추가함.(나중에 최적화할때 어레이 형태로 바꿈)
	xSec m_secLastCalc;			// 월드 마지막 정산(업데이트)시간.
	XList4<ID> m_listOpendArea;	// 오픈시킨 지역의 아이디리스트

//	XList4<XTimeEventTimer*> m_listEvents;		// 시간 이벤트 성질의 것들도 모두 월드에서 관리 하도록 변경 시키자.

	BOOL m_bUpdate;			// 월드맵씬을 전체 업데이트 해야하면 TRUE

	void Init() {
		m_secLastCalc = 0;
		m_bUpdate = FALSE;

	}
	void Destroy();
public:
	XWorld();
	virtual ~XWorld() { Destroy(); }

#ifdef _DUMMY_GENERATOR
	GET_SET_ACCESSOR( xSec, secLastCalc );
#else
	GET_ACCESSOR( xSec, secLastCalc );
#endif // _DUMMY_GENERATOR
	GET_ACCESSOR( BOOL, bUpdate );
	SET_ACCESSOR( BOOL, bUpdate );
	void Release();
	void UpdateLastCalc() {
		m_secLastCalc = XTimer2::sGetTime();
	}
	GET_ACCESSOR_CONST( const XList4<ID>&, listOpendArea );
	void InitCloudList( int level=0 );
	void _AddSpot( XSpot *pBaseSpot );
	void OnCreateSpots( XSPAcc pAcc );

//	XSpotDaily* SetActiveDailySpotToRandom( XE::xtDOW dow, int secPass, XSPAcc spAcc );
	XSpotDaily* GetActiveDailySpot();
	inline XSpotDaily* GetpSpotDaily() {
		return GetActiveDailySpot();
	}
// 	XSpotSpecial* SetActiveSpecialSpotToRandom( int secPass, XSPAcc spAcc );
// 	XSpotSpecial* GetActivatedSpecialSpot();

//	void CreateEmptySpots();
	int GetSpotInSight( XArrayLinearN<XSpot*, 512> *pOutAry,
						XGAME::xtSpot typeSpot,
						const XE::VEC2& vCenter,
						float radius );
	template<typename T>
	T GetSpot( XGAME::xtSpot typeSpot, LPCTSTR szIdentifier );
	template<typename T>
	T GetSpot( XGAME::xtSpot typeSpot, ID idSpot );
	template<typename T, int N>
	int GetSpotsToAry( XArrayLinearN<T,N> *pOut, 
						XGAME::xtSpot typeSpot = XGAME::xSPOT_NONE );
	template<typename T>
	int GetSpotsToAry( XVector<T> *pOut, XGAME::xtSpot typeSpot );
	XSpot* GetSpot( ID idSpot );
	inline XSpot* GetpSpot( ID idSpot ) {
		return GetSpot( idSpot );
	}
	inline bool IsHaveSpot( ID idSpot ) {
		return GetSpot( idSpot ) != nullptr;
	}

	// 아직 오픈하지 않은 지역 아이디 리스트를 얻는다.
	int GetClosedCloudsToAry( XArrayLinearN<ID, 512> *pOut );
	XSpotJewel* GetSpotJewelByIdx( int idxJewel );
	XSpotMandrake* GetSpotMandrakeByIdx( int idxMandrake );
	XSpotCastle* GetpSpotCastle( ID idSpot );
	XSpotJewel* GetpSpotJewel( ID idSpot );
	XSpotSulfur* GetpSpotSulfur( ID idSpot );
	XSpotMandrake* GetpSpotMandrake( ID idSpot );
	XSpotNpc* GetpSpotNpc( ID idSpot );
	XSpotDaily* GetpSpotDaily( ID idSpot );
// 	XSpotSpecial* GetpSpotSpecial( ID idSpot );
	XSpotCampaign* GetpSpotCampaign( ID idSpot );
	XSpotVisit* GetpSpotVisit( ID idSpot );
	XSpotCash* GetpSpotCash( ID idSpot );
	//
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar );
	int DeSerializeSpotUpdate( XArchive& ar, ID idSpot );
	//
	void Process( float dt );
	void OnAfterDeSerialize( XDelegateSpot *pDelegator, ID idAcc );
//	void GetLootingAmount( int *pOutWood, int *pOutIron );
	void GetNumResLocalStorageByAllSpot( xResourceAry *pOutAry );
	void LossLocalStorageAll( float rateLoss, std::vector<int> *pOutAry );
	void DoOpenCloud( ID idCloud );
	void DoCloseArea( ID idArea );
	bool IsClosedArea( ID idCloud ) {
		return !IsOpenedArea( idCloud );
	}
	// idArea지역을 오픈했냐
	bool IsOpenedArea( ID idArea );
	bool IsOpenedArea( const _tstring& idsArea );
	bool IsExistUserSpot(ID idacc);
	int GetOpenCloudsToAry( XArrayLinearN<XPropCloud::xCloud*, 512> *pOutAry );
	int GetOpenCloudsToAry( XVector<XPropCloud::xCloud*> *pOutAry );
	int GetNumSpots( XGAME::xtSpot spotType ) const;
	int GetAvgLevelBySpot( XGAME::xtSpot type );
	int GetAvgScoreBySpot( XGAME::xtSpot type );
	void UpdateSpots();
	int GetMaxLevelOpenedArea();
	bool IsOpenPrecedeAreaWithDepth( XPropCloud::xCloud *pPropCurr, int depth );
	void DispatchSpotEvent( XPropCloud::xCloud* pPropArea );
	/// 앱이 Restore될때 프로퍼티 같은 잃어버린 포인터가 있다면 재로딩해야한다.
	void Restore();
	//
private:
	void DestroySpot( XSpot *pSpot );
	void DestroySpot( ID idSpot );
friend class XAccount;
};

#include "XWorld.inl"

#ifdef _CLIENT
XWorld* sGetpWorld();
#endif // _CLIENT



