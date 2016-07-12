#pragma once
#include "XEBaseWorldObj.h"
#include "XList.h"

#if 1
#endif

class XEWndWorld;

////////////////////////////////////////////////////////////////
class XDelegateObjMng
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateObjMng() { Init(); }
	virtual ~XDelegateObjMng() { Destroy(); }
	//
	virtual void OnDelegateFrameMoveEachObj( float dt, ID idEvent, WorldObjPtr spObj ) {}
};
////////////////////////////////////////////////////////////////
class XEObjMng
{
	std::list<WorldObjPtr> m_listObj;
	CTimer m_timerSort;
	DWORD m_Attr;		// 오브젝트 매니저 각종 속성들( XF::OBJMNG_시리즈 )
	BOOL m_bAdded;	// 이번프레임에 오브젝트가 추가되었으면 TRUE
	XVector<XEBaseWorldObj*> m_aryVisible;	// 화면에 보이는 오브젝트만 추려낸것
	void Init() {
		m_Attr = 0;
		m_bAdded = TRUE;
	}
	void Destroy();
public:
	XEObjMng( int maxObj );
	virtual ~XEObjMng() { Destroy(); }
	//
	virtual void Release();
	GET_SET_ACCESSOR( DWORD, Attr );
	GET_ACCESSOR( std::list<WorldObjPtr>&, listObj );
	int GetNumObj( void ) {
		return m_listObj.size();
	}
	int GetNumVisibleObj( void ) {
		return m_aryVisible.size();
	}
	//
//	virtual WorldObjPtr FindNearObjByFilter( const XE::VEC3& vwPos, float radius, BIT bitSide );
	WorldObjPtr FindNearObjByFunc( XEBaseWorldObj *pSrcObj, const XE::VEC3& vwPos, float radius, BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ) );
	WorldObjPtr FindNearObjByMore( XEBaseWorldObj *pSrcObj,
											const XE::VEC3& vwPos,
											float radius,
											BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ),
											BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ) );
	//
	virtual ID Add( const WorldObjPtr& spObj );
	virtual void Add( ID snObj, const WorldObjPtr& spObj );
//	virtual void Del( XEBaseWorldObj *pObj );
	virtual void DestroyAllObj( void );
	virtual WorldObjPtr* Find( ID snObj );
	void FrameMove( XEWndWorld *pWndWorld, float dt );
	void FrameMoveDelegate( XDelegateObjMng *pDelegate, ID idEvent, float dt );
	void Draw( XEWndWorld *pWndWorld );
	virtual void DrawVisible( XEWndWorld *pWndWorld, const XVector<XEBaseWorldObj*>& aryVisible );
//	virtual void DrawVisible( XEWndWorld *pWndWorld, const XList4<XEBaseWorldObj*>& aryVisible );
	void OnLButtonUp( float lx, float ly );
	/**
	 @brief pObj가 FrameMove()에서 파괴되기전에 호출된다.
	*/
	virtual void OnDestroyObj( XEBaseWorldObj *pObj ) {}
};
