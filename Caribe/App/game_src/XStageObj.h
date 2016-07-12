/********************************************************************
	@date:	2015/05/08 18:57
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XStageObj.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XPropCamp.h"
#include "XPropItem.h"

XE_NAMESPACE_START( xCampaign )

// 도전자 정보
struct xTryer {
	_tstring strName;
	ID idAcc = 0;
	int level = 0;
	int power = 0;
	ID getid() {
		return idAcc;
	}
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar, int ver );
};
class XCampObj;
/****************************************************************
* @brief 캠페인 스테이지 인스턴스
* @author xuzhu
* @date	2015/05/08 18:54
*****************************************************************/
class XStageObj : public XLock
{
public:
	static int sSerialize( XArchive& ar, StageObjPtr spStageObj );
	static int sDeSerialize( XArchive& ar, CampObjPtr spCamp, StageObjPtr spStageObj );
private:
	XSPPropStage _m_spPropStage;		// 왜 언더바 해놨지?
	int m_numClear = 0;		// 클리어 횟수
	int m_numTry = 0;			// 도전 횟수
	LegionPtr m_spLegion;
	int m_numSquad = 0;   // 만들어야할 부대수(프로퍼티엔 0으로 들어있을수도 있어서 따로뺌)
	XList4<xTryer> m_listTryer;		// 이 스테이지에 도전했던 유저의 이름과 레벨들.
	int m_Power = 0;				// 군단 전투력(한번세팅되면 군단이 바뀌지 않는한 바뀌지 않는다. update속도땜에 이렇게 함.)
	int m_LevelLegion = 0;			// 군단 레벨
//	int m_lvLimit = 0;					// 입장 제한레벨
	std::vector<XGAME::xDropItem> m_aryDrops;	// 드랍가능한 아이템 품목.
	XGAME::xLegionParam m_paramLegion;	// 부대생성에 관한 외부파라메터
	int m_numStar = 0;		// 클리어하고 받은 별점
	bool m_bRecvReward = false;		// 보상을 이미 받았으면 true
	void Init() {}
	void Destroy();
public:
	XStageObj() { Init(); };
	XStageObj( XSPPropStage& spPropStage );
	virtual ~XStageObj() { Destroy(); }
	//
	XSPPropStage GetspPropStage() const {
		return _m_spPropStage;
	}
protected:
	void SetspPropStage( XSPPropStage spPropStage ) {
		XLOCK_OBJ;
		_m_spPropStage = spPropStage;
	}
	SET_LOCK_ACCESSOR( int, numClear );
public:
	GET_LOCK_ACCESSOR( int, numClear );
	GET_SET_LOCK_ACCESSOR( int, numTry );
	GET_SET_LOCK_ACCESSOR( LegionPtr, spLegion );	// spLegion은 thread safe하지 않음.
	GET_LOCK_ACCESSOR( int, numSquad );
	GET_SET_LOCK_ACCESSOR( int, Power );
	GET_SET_LOCK_ACCESSOR( int, LevelLegion );
	GET_LOCK_ACCESSOR( std::vector<XGAME::xDropItem>&, aryDrops );
	GET_LOCK_ACCESSOR( XGAME::xLegionParam&, paramLegion );
	GET_SET_LOCK_ACCESSOR( int, numStar );
	GET_SET_LOCK_ACCESSOR( bool, bRecvReward );
	void SetparamLegion( XGAME::xLegionParam& paramLegion ) {
		m_paramLegion = paramLegion;
	}
	virtual int AddnumClear() {
		XLOCK_OBJ;
		return ++m_numClear;
	}
	void ClearnumClear() {
		XLOCK_OBJ;
		m_numClear = 0;
	}
	// 한번이라도 클리어한적 있다면 별이 1개라도 있다.
	bool IsCleared() {
		return m_numStar != 0;
	}
	int GetidxStage() {
		XLOCK_OBJ;
		return GetspPropStage()->idxStage;
	}
	// 도전가능한 스테이지 인가.(XCampObj::IsAbleTry를 쓸것.)
// 	bool IsAbleTry( int idxStageLastUnlock ) {
// 		XLOCK_OBJ;
// 		if( GetspPropStage()->idxStage > idxStageLastUnlock )
// 			return false;
// 		return !IsAllNumClear();
// 	}
	virtual int Serialize( XArchive& ar );
//	virtual int DeSerialize( XPropCamp::xProp* pPropCamp, int idxStage, XArchive& ar, int verCamp );
	virtual int DeSerialize( const CampObjPtr spCampObj, int idxStage, XArchive& ar, int verCamp );
	bool CreateLegion( CampObjPtr spCampObj, int lvBase, int idxFloor );
#if defined(_XSINGLE) || !defined(_CLIENT)
	LegionPtr CreateLegion2( CampObjPtr spCampObj, int lvBase, int *OutLvLegion, int idxFloor ) const;
#endif
	void DestroyLegion();
	void AddTryer( ID idAcc, _tstring& strName, int level, int power ) {
		XLOCK_OBJ;
		if( !m_listTryer.FindByIDNonPtr( idAcc ) ) {
			xTryer tryer;
			tryer.idAcc = idAcc;
			tryer.strName = strName;
			tryer.level = level;
			tryer.power = power;
			m_listTryer.Add( tryer );
		}
	}
	bool FindTryer( ID idAcc ) {
		XLOCK_OBJ;
		return m_listTryer.FindByIDNonPtr( idAcc ) != nullptr;
	}
	void _ClearTryer() {
#ifdef _CHEAT
		XLOCK_OBJ;
		m_listTryer.clear();
#endif // _CHEAT
	}
	void InitStage() {
		XLOCK_OBJ;
		m_listTryer.clear();
		m_numClear = 0;
		m_spLegion.reset();
	}
	void AddItems( ID idItem, int num );
	void AddItems( ID idItem, int num, float dropChance );
	void AddItems( LPCTSTR szIds, int num );
	void AddItems( XPropItem::xPROP* pProp, int num );
	void ClearItems() {
		m_aryDrops.clear();
	}
	// 도전가능한 상태인가.
	bool IsAbleTry() const;
// 	bool IsAllNumClear() {
// 		XLOCK_OBJ;
// 		int maxTry = GetspPropStage()->maxTry;
// 		if( maxTry == 0 )
// 			return false;	// 이제 maxtry 0은 무한대로 도전
// 		return maxTry > 0 && m_numClear >= maxTry;
// 	}
	// 완전히 클리어했는가.
	bool IsAllNumClear() const;
	const XGAME::xDropItem& GetDrop( int idx ) {
		return m_aryDrops[ idx ];
	}
	virtual bool SetDropItemsFromProp( const CampObjPtr spCampObj );
	ID GetidProp() {
		XBREAK( _m_spPropStage == nullptr );
		return _m_spPropStage->idProp;
	}
	virtual void Update( CampObjPtr spCampObj, int idxFloor );
	void SetSquadParam( XSPCampObj spCampObj, int idxPos, int lvLegion, int idxFloor, const XGAME::xSquad& _propSquad, XGAME::xSquad* pOut ) const;
	// 스테이지가 "클리어"상태인지
//	bool IsClear();
private:
}; // class XStageObj

XE_NAMESPACE_END;   // namespace xCampiagn

