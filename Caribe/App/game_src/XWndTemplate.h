/********************************************************************
	@date:	2016/02/02 12:18
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XWndTemplate.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWndImage.h"
#include "XStruct.h"
class XWnd;
class XWndImage;
class XWndArrow4Abil;
class XGameWndAlert;
class XWndPopup;
class XSpot;
namespace XSKILL {
	class XSkillDat;
}
XE_NAMESPACE_START( XGAME )
//
XWndImage* UpdateUnitFace( XWnd *pRoot, const std::string& ids, XGAME::xtUnit unit );
XWndImage* UpdateLevelByHero( XWnd *pRoot, const std::string& ids, int lv );
void CreateUpdateTopResource( XWnd *pRoot );
void CreateUpdateTopGuildCoin( XWnd *pRoot );
XWnd* GetpRootTopRes( XWnd *pRootFind );
XWnd* GetpWndTopRes( XGAME::xtResource resType, XWnd *pRootFind );
XGameWndAlert* DoAlertWithItem( const _tstring& strIdsItem, const _tstring& strText );
XWndPopup* DoPopupBattleResult( XGAME::xBattleResult& result, XWnd* pParent, XSpot* pBaseSpot );
//
XE_NAMESPACE_END; // XGAME

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/05/13 19:28
*****************************************************************/
class XWndTooltip : public XWnd
{
public:
	XWndTooltip( const _tstring& strXml
						, const std::string& strKey
						, LPCTSTR fileFrame );
	virtual ~XWndTooltip() { Destroy(); }
	// get/setter
	// public member
	void SetText( LPCTSTR szText, const char* cKey = "__text.tooltip" );
	inline void SetText( const _tstring& strText, const char* cKey = "__text.tooltip" ) {
		SetText( strText.c_str(), cKey );
	}
	void AutoPosByWinSrc( XWnd* pWndTrace );
	void SetDirection( const XE::VEC2& vEnd );
	void SetLineLength( float w );
	void SetAutoSize() override;
private:
	// private member
	XWndArrow4Abil* m_pWndArrow = nullptr;
	bool m_bTouchNC = false;
	float m_Length = 0;		// 문단폭. 디폴트
private:
	// private method
	void Init() {}
	void Destroy() {}
	void OnNCLButtonDown( float lx, float ly ) override;
	void OnNCLButtonUp( float lx, float ly ) override;
	int OnFinishAppear( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndTooltip

/****************************************************************
* @brief 유닛과 유닛의 원형프레임을 그려주는 모듈
* @author xuzhu
* @date	2015/11/25 19:48
*****************************************************************/
class XWndCircleUnit : public XWndImage
{
public:
	XWndCircleUnit( XGAME::xtUnit unit, const XE::VEC2& vPos, XHero *pHero );
	virtual ~XWndCircleUnit() { Destroy(); }
	// get/setter
	GET_SET_BOOL_ACCESSOR( bShowLevelSquad );
	// public member
private:
	// private member
	XGAME::xtUnit m_Unit;
	XHero *m_pHero = nullptr;
	bool m_bShowLevelSquad = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
	void SetUnit( XGAME::xtUnit unit ) {
		m_Unit = unit;
		SetbUpdate( true );
	}
}; // class XWndCircleUnit

/****************************************************************
* @brief 스킬과 스킬의 원형프레임을 그려주는 모듈.
* @author xuzhu
* @date	2015/12/01 22:01
*****************************************************************/
class XWndCircleSkill : public XWndImage
{
public:
	XWndCircleSkill( XSKILL::XSkillDat *pDat, const XE::VEC2& vPos, XHero *pHero);
	virtual ~XWndCircleSkill() { Destroy(); }
	// get/setter
	GET_SET_BOOL_ACCESSOR( bShowLevel );
	// public member
private:
	// private member
	XSKILL::XSkillDat *m_pSkillDat = nullptr;
	XHero *m_pHero = nullptr;
	bool m_bShowLevel = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndCircleSkill


