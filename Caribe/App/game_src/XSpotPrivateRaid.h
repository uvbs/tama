/********************************************************************
	@date:	2016/09/30 14:24
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XSpotPrivateRaid.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XSpot.h"
#include "constGame.h"


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
	const XList4<XHero*>& GetlistEnter( int idxSide ) const {
		return m_aryEnter[ idxSide ];
	}
	GET_SET_ACCESSOR2( XSPLegion, spLegionPlayer );
	// public member
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return !IsNpc();
	}
	void UpdatePlayerEnterList( const XList4<ID>& _listHero, XSPAccConst spAcc );
#ifdef _SERVER
private:
#endif // _SERVER
	void AddEnterHero( XHero* pHero, int idxSide );
	bool IsExistEnterHero( XHero* pHero, int idxSide );
	void DelEnterHero( XHero* pHero, int idxSide );
	void ChangeEnterHero( XHero* pHero1, XHero* pHero2, int idxSide );
	void ReplaceEnterHero( XHero* pHeroNew, XHero* pExistHero, int idxSide );
	int GetidxEnterHero( XHero* pHero, int idxSide );
	void SetSelectEnterHero( XHero* pHero, int idxSide );		// 현재 선택된 영웅
	bool IsSelectedHero( XHero* pHero, int idxSide );
	XHero* GetSelectEnterHero( int idxSide );
private:
	// private member
	XVector<XList4<XHero*>> m_aryEnter;			// 출전영웅 리스트
	XVector<XHero*> m_arySelected;
	XSPLegion m_spLegionPlayer;							// 이 스팟 전용 플레이어측 군단 정보
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
	void ProcCreateSquadron( XSPLegion spLegion, XList4<ID>* pOutlistHero, XSPAccConst spAcc ) const;
}; // class XSpotPrivateRaid
