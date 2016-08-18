/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndButton.h"
#include "JWWnd.h"
#include "Sprite/XDelegateSprObj.h"

namespace XGAME {
struct xBattleResult;
};
namespace XSKILL {
class XSkillDat;
};

class XUnitHero;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/11/23 21:46
*****************************************************************/
class XWndBattleAfterHeroExp : public XWndPopup
{
public:
	XWndBattleAfterHeroExp( int idxLegion, XGAME::xBattleResult* pResult );
	virtual ~XWndBattleAfterHeroExp() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_idxLegion = 0;
	XGAME::xBattleResult* m_pResult = nullptr;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndBattleAfterHeroExp

/****************************************************************
* @brief 스킬사용버튼.
* @author xuzhu
* @date	2014/10/28 9:29
*****************************************************************/
class XWndSkillButton : public XWndButton
{
	CTimer m_timerCool;
	ID m_snHero = 0;
	void Init() {}
	void Destroy();
public:
	XWndSkillButton( float x, float y, XSKILL::XSkillDat *pSkillDat, XUnitHero *pUnitHero );
	virtual ~XWndSkillButton() { Destroy(); }
	///< 
	GET_ACCESSOR_CONST( ID, snHero );
	void SetCoolTimer( float sec );
	bool IsCoolTime() {
		return m_timerCool.IsOn() && !m_timerCool.IsOver();
	}
	int Process( float dt ) override;
	void Draw( void ) override;

}; // class XWndSkillButton

/**
 @brief 
 
*/
class XWndFaceInBattle : public XWndStoragyItemElem 
												, public XDelegateSprObj
{
	XHero* m_pHero = nullptr;
	int m_Side = 0;		// 1은 아군 2는 적군
	XSPSquad m_spSquadObj;
	XSprObj* m_psoSkill = nullptr;
	_tstring m_strText;
	void Release() override {
		m_spSquadObj.reset();
	}
public:
	/// @param side 1:아군 2:적군
	XWndFaceInBattle( XSPSquad spSquadObj, int side );
	~XWndFaceInBattle();
	//
	BOOL OnCreate() override;
	void Update() override;
	int Process( float dt ) override;
	void OnUseSkill( const _tstring& strText );
	void Draw() override;
	BOOL OnDelegateDrawImageLayerBefore( XSprObj *pSprObj, XSprite *pSprSrc, XLayerImage *pImageLayer, XEFFECT_PARAM *pEffectParam, float x, float y, const MATRIX &mParent ) override;
};
