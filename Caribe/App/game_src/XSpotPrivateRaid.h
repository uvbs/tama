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
// 	static const int c_maxSquad;
// 	static const int c_maxWins;
	XSpotPrivateRaid( XWorld* pWorld );
	XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp );
	virtual ~XSpotPrivateRaid() {
		Destroy();
	}
	// get/setter
	const XList4<XSPHero>& GetlistEnterEnemy() const {
		return m_listEnterEnemy;
	}
	GET_ACCESSOR_CONST( const XList4<ID>&, listEnterPlayer );
	GET_ACCESSOR_CONST( const xnLegion::xLegionDat&, legionDatPlayer );
	GET_ACCESSOR_CONST( int, numWins );
	GET_ACCESSOR_CONST( xSec, secTimer );
	// public member
	// 남은 승리횟수
	inline int GetwinRemain() const {
		const int maxWins = GetpBaseProp()->m_Param.GetInt( "num_win" );
		return maxWins - m_numWins;
	}
	// 리셋주기
	inline xSec GetsecReset() const {
		return xHOUR_TO_SEC(1);
	}
	// 리셋까지 남은 시간(초)
	xSec GetsecRemainUntilReset() const {
		return GetsecReset() - (XTimer2::sGetTime() - m_secTimer);
	}
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
	XList4<XSPHero> GetlistEnter( int idxSide );
	void ResetTry();
	//	XSPLegion GetspLegionPlayer();
//////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
private:
#endif // _SERVER
#ifdef _CLIENT
	void AddEnterHero( XSPHero pHero, int idxSide );
	bool IsExistEnterHero( XSPHero pHero, int idxSide );
	void DelEnterHero( XSPHero pHero, int idxSide );
	void ChangeEnterHero( XSPHero pHero1, XSPHero pHero2, int idxSide );
	void ReplaceEnterHero( XSPHero pHeroNew, XSPHero pExistHero, int idxSide );
#endif // _CLIENT
	int GetidxEnterHero( XSPHero pHero, int idxSide );
	void SetSelectEnterHero( XSPHero pHero, int idxSide );		// 현재 선택된 영웅
	bool IsSelectedHero( XSPHero pHero, int idxSide );
	XSPHero GetSelectEnterHero( int idxSide );
//////////////////////////////////////////////////////////////////////////
private:
	// private member
	// 출전영웅 리스트(군단소속영웅포함)
	XList4<ID> m_listEnterPlayer;
	XList4<XSPHero> m_listEnterEnemy;			
	XVector<XSPHero> m_arySelected;
//	XSPLegion m_spLegionPlayer;							// 이 스팟 전용 플레이어측 군단 정보
	xnLegion::xLegionDat m_legionDatPlayer;					///< 스팟엔 플레이어의 군단객체(XLegion)을 두지 않는다. 데이터 형태로만 보관한다.
	int m_numWins = 0;				// 승리가능한 회수
	xSec m_secTimer = 0;					// 1시간 체크를 위한 타이머
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
	
//	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum ) const override;
}; // class XSpotPrivateRaid
