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
XWndImage* _CreateUnitFace( XWnd *pRoot, const std::string& ids, XGAME::xtUnit unit );
XWndImage* _CreateUnitFace( XWnd *pRoot, XGAME::xtUnit unit );
XWndImage* UpdateLevelByHero( XWnd *pRoot, const std::string& ids, int lv );
void CreateUpdateTopResource( XWnd *pRoot );
void CreateUpdateTopGuildCoin( XWnd *pRoot );
XWnd* GetpRootTopRes( XWnd *pRootFind );
XWnd* GetpWndTopRes( XGAME::xtResource resType, XWnd *pRootFind );
XGameWndAlert* DoAlertWithItem( XPropItem::xPROP* pPropItem, const _tstring& strText, XWnd::xtAlert type );
XGameWndAlert* DoAlertWithItem( const _tstring& strIdsItem, const _tstring& strText, XWnd::xtAlert type );
XGameWndAlert* DoAlertWithItem( ID idItem, const _tstring& strText, XWnd::xtAlert type );
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
	XWndCircleUnit();
	XWndCircleUnit( XGAME::xtUnit unit, const XE::VEC2& vPos, XHero *pHero );
	virtual ~XWndCircleUnit() { Destroy(); }
	// get/setter
	GET_SET_BOOL_ACCESSOR( bShowLevelSquad );
	// public member
	void SetUnit( XGAME::xtUnit unit ) {
		m_Unit = unit;
		SetbUpdate( true );
	}
private:
	// private member
	XGAME::xtUnit m_Unit = XGAME::xUNIT_NONE;
	XHero *m_pHero = nullptr;
	bool m_bShowLevelSquad = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndCircleUnit

inline XWndCircleUnit* xGetCtrlUnit( XWnd* pRoot, const std::string& key ) {
	return SafeCast<XWndCircleUnit*>( pRoot->Find( key ) );
}

/****************************************************************
* @brief 스킬과 스킬의 원형프레임을 그려주는 모듈.
* @author xuzhu
* @date	2015/12/01 22:01
*****************************************************************/
// class XWndCircleSkill : public XWndImage
// {
// public:
// 	XWndCircleSkill( const XSKILL::XSkillDat *pDat, const XE::VEC2& vPos, XHero *pHero);
// 	~XWndCircleSkill() { Destroy(); }
// 	// get/setter
// //	GET_SET_BOOL_ACCESSOR( bShowLevel );
// 	// public member
// 	void SetSkill( const XSKILL::XSkillDat *pDat, int level ) {
// 		m_pSkillDat = pDat;
// 		m_Level = level;
// 		SetbUpdate( true );
// 	}
// private:
// 	// private member
// 	const XSKILL::XSkillDat *m_pSkillDat = nullptr;
// 	XHero *m_pHero = nullptr;
// //	bool m_bShowLevel = false;
// 	int m_Level = 0;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// 	void Update() override;
// }; // class XWndCircleSkill
/****************************************************************
* @brief 특성과 특성의 원형프레임을 그려주는 모듈.
* @author xuzhu
* @date	2015/12/01 22:01
*****************************************************************/
class XWndCircleSkill : public XWnd {
public:
	XWndCircleSkill( const XSKILL::XSkillDat *pDat, const XE::VEC2& vPos, XHero *pHero );
	~XWndCircleSkill() {
		Destroy();
	}
	// get/setter
// 	GET_SET_BOOL_ACCESSOR( bShowLevel );
	// public member
	GET_SET_ACCESSOR_CONST( int, Level );
	void SetSkill( const XSKILL::XSkillDat *pDat, int level );
private:
	// private member
	const XSKILL::XSkillDat *m_pSkillDat = nullptr;
	XHero *m_pHero = nullptr;
//	bool m_bShowLevel = false;
	int m_Level = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndCircleSkill

inline XWndCircleSkill* xGetCtrlSkill( XWnd* pRoot, const std::string& key ) {
	return SafeCast<XWndCircleSkill*>( pRoot->Find( key ) );
}

/****************************************************************
* @brief 특성과 특성의 원형프레임을 그려주는 모듈.
* @author xuzhu
* @date	2015/12/01 22:01
*****************************************************************/
class XWndTechAbil : public XWnd
{
public:
	XWndTechAbil( const XSKILL::XSkillDat *pDat, const XE::VEC2& vPos, XHero *pHero);
	~XWndTechAbil() { Destroy(); }
	// get/setter
	GET_SET_BOOL_ACCESSOR( bShowLevel );
	// public member
	GET_SET_ACCESSOR_CONST( int, Level );
	void SetSkill( const XSKILL::XSkillDat *pDat, int level );
private:
	// private member
	const XSKILL::XSkillDat *m_pSkillDat = nullptr;
	XHero *m_pHero = nullptr;
	bool m_bShowLevel = false;
	int m_Level = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndTechAbil

