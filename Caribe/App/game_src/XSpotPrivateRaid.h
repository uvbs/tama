/********************************************************************
	@date:	2016/09/30 14:24
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XSpotPrivateRaid.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XSpot.h"
#include "constGame.h"
#include "XLegionH.h"


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/30 14:24
*****************************************************************/
class XSpotPrivateRaid : public XSpot
{
public:
	static const int c_maxSquad;
	XSpotPrivateRaid( XWorld* pWorld );
	XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp );
	virtual ~XSpotPrivateRaid() {
		Destroy();
	}
	// get/setter
	const XList4<XHero*>& GetlistEnterEnemy() const {
		return m_listEnterEnemy;
	}
	GET_ACCESSOR_CONST( const XList4<ID>&, listEnterPlayer );
	GET_ACCESSOR_CONST( const xnLegion::xLegionDat&, legionDatPlayer );
//	GET_SET_ACCESSOR2( XSPLegion, spLegionPlayer );
	// public member
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return !IsNpc();
	}
	void UpdatePlayerEnterList( const XList4<ID>& _listHero, int lvAcc );
	inline bool IsEmptyLegionPlayer() const {
		return m_legionDatPlayer.m_listSquad.empty();
	}
	void SerializeForBattle( XArchive* pOut, const XParamObj2& param );
	XList4<XHero*> GetlistEnter( int idxSide );
//	XSPLegion GetspLegionPlayer();
//////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
private:
#endif // _SERVER
#ifdef _CLIENT
	void AddEnterHero( XHero* pHero, int idxSide );
	bool IsExistEnterHero( XHero* pHero, int idxSide );
	void DelEnterHero( XHero* pHero, int idxSide );
	void ChangeEnterHero( XHero* pHero1, XHero* pHero2, int idxSide );
	void ReplaceEnterHero( XHero* pHeroNew, XHero* pExistHero, int idxSide );
#endif // _CLIENT
	int GetidxEnterHero( XHero* pHero, int idxSide );
	void SetSelectEnterHero( XHero* pHero, int idxSide );		// 현재 선택된 영웅
	bool IsSelectedHero( XHero* pHero, int idxSide );
	XHero* GetSelectEnterHero( int idxSide );
//////////////////////////////////////////////////////////////////////////
private:
	// private member
	// 출전영웅 리스트(군단소속영웅포함)
	XList4<ID> m_listEnterPlayer;
	XList4<XHero*> m_listEnterEnemy;			
	XVector<XHero*> m_arySelected;
//	XSPLegion m_spLegionPlayer;							// 이 스팟 전용 플레이어측 군단 정보
	xnLegion::xLegionDat m_legionDatPlayer;					///< 스팟엔 플레이어의 군단객체(XLegion)을 두지 않는다. 데이터 형태로만 보관한다.
//////////////////////////////////////////////////////////////////////////
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Serialize( XArchive& ar ) override;
	BOOL DeSerialize( XArchive& ar, DWORD ver ) override;
	bool Update( XSPAcc spAcc );
	void CreateEnemyEnterHeroes( int lvSpot );
	inline XPropWorld::xPrivateRaid* GetpProp(){
		return static_cast<XPropWorld::xPrivateRaid*>( GetpBaseProp() );
	}
	void ProcCreateSquadron( xnLegion::xLegionDat*, const XList4<ID>& listHero, int lvAcc ) const;
	void DeSerializeForBattle( XArchive& ar, XArchive& arAdd, XSPAcc spAcc );
	void SerializeEnterEnemy( XArchive& ar ) const;
	void DeSerializeEnterEnemy( XArchive& ar, int verWorld );
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
}; // class XSpotPrivateRaid
