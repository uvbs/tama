/********************************************************************
	@date:	2014/09/15 15:52
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XEObjMngWithType.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/Game/XEObjMng.h"
#include "skill/SkillDef.h"
//#include "XFramework/Game/Skill/xSkill.h"

class XWndBattleField;
/****************************************************************
* @brief 기존 오브젝트 매니저에 추가로 타입별로 구분해서 관리할수 있도록 했다.
* @author xuzhu
* @date	2014/09/15 15:53
* @todo 배열을 아군적군까지 나눠서 쓰자.
*****************************************************************/
class XEObjMngWithType : public XEObjMng
{
public:
	static XEObjMngWithType* sGet() {
		return s_pInstance;
	}
private:
	static XEObjMngWithType* s_pInstance;
	XList4<UnitPtr> m_listUnits;
	XList4<WorldObjPtr> m_listEtc;
	void Init() {}
	void Destroy();
public:
	XEObjMngWithType( int maxObj, int maxType );
	virtual ~XEObjMngWithType() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const XList4<XSPUnit>&, listUnits );
	XList4<XSPUnit>& GetlistUnitsMutable() {
		return m_listUnits;
	}
	//
	virtual void Release();
	virtual ID AddUnit( const UnitPtr& spObj );
	virtual void AddUnit( ID idObj, const UnitPtr& spObj );
	virtual ID Add( const WorldObjPtr& spObj );
	virtual void Add( ID idObj, const WorldObjPtr& spObj );
	virtual void OnDestroyObj( XEBaseWorldObj *pObj );
	virtual WorldObjPtr Find( int type, ID idObj );
//	virtual void DestroyAllObj( void );
//	WorldObjPtr FindNearObjByFilter( const XE::VEC3& vwPos, float radius, BIT bitSide );
	UnitPtr FindNearObjByFunc( XEBaseWorldObj *pSrcObj, 
								const XE::VEC3& vwPos, 
								float meterRadius, 
								BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ) );
	UnitPtr FindNearUnitByFunc( XBaseUnit *pSrcObj,
							const XE::VEC3& vwPos,
							float meterRadius,
							bool bFindOutRange,
							BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ) );
	WorldObjPtr FindNearObjByMore( XEBaseWorldObj *pSrcObj, 
									const XE::VEC3& vwPos, 
									float radius, 
									BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ), 
									BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ) );
	UnitPtr FindNearUnitByMore( XBaseUnit *pSrcObj,
							const XE::VEC3& vwPos,
							float radius,
					BOOL( *pfuncFilter )( XBaseUnit*, XBaseUnit* ),
					BOOL( *pfuncCompare )( XBaseUnit*, XBaseUnit*, XBaseUnit* ));
	int GetListUnitRadius2( XVector<XSPUnit> *pOutAry
												, XEBaseWorldObj *pCenter
												, const XE::VEC2& vCenter
												, float pixelRadius
												, BIT bitSideFilter
												, int numCost
												, bool bIncludeCenter
												, BIT bitLive = XSKILL::xTF_LIVE ) const;
	void DrawVisible( XEWndWorld *pWndWorld, const XVector<XEBaseWorldObj*>& aryVisible ) override;
	UnitPtr GetPickUnit( XWndBattleField *pWndWorld, const XE::VEC2& vsPos, BIT bitCamp, ID snExcludeSquad=0, BOOL *pOutPickExclude=NULL );
	void DestroyObjWithType( int type, ID snObj );
};

