#pragma once
#include "XEObjMng.h"

class XEObjMng;
class XEWndWorld;
// 게임프레임워크에서 오브젝트들이 들어갈 월드전체를 표현하는 추상화 객체
class XEWorld
{
	XE::VEC2 m_vwSize;		// 월드 전체 크기
	XEObjMng *m_pObjMng;
	void Init() {
		m_pObjMng = NULL;
	}
	void Destroy();
public:
	XEWorld( const XE::VEC2& vwSize );
	virtual ~XEWorld() { Destroy(); }
	//
	virtual void Release( void );
	GET_ACCESSOR( XEObjMng*, pObjMng );
	GET_ACCESSOR( const XE::VEC2&, vwSize );
	void SetObjMng( XEObjMng *pObjMng );
	//
	WorldObjPtr* GetObjByidObj( ID idObj ) {
		if( idObj == 0 )
			return NULL;
		return m_pObjMng->Find( idObj );
	}
//	WorldObjPtr FindNearObjByFilter( const XE::VEC3& vwPos, float radiusSight, BIT bitSide );
// 	WorldObjPtr FindNearObjByFunc( XEBaseWorldObj *pSrcObj, const XE::VEC3& vwPos, float radius, BOOL ( *pFunc )( XEBaseWorldObj*, XEBaseWorldObj* ) );
// 	WorldObjPtr FindNearObjByMore( XEBaseWorldObj *pSrcObj, 
// 									const XE::VEC3& vwPos, 
// 									float radius, 
// 									BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ), 
// 									BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ) );
	XE::VEC3 ClippingObj( const XE::VEC3& vwPos, const XE::VEC3& vSize );
	virtual void DestroyAllObj( void );
	//
	ID AddObj( const WorldObjPtr& spObj );
	ID AddObj( int type, const WorldObjPtr& spObj );
	virtual XEObjMng* CreateObjMng( int maxObj );
	virtual int Process( XEWndWorld *pWndWorld, float dt );
	virtual void Draw( XEWndWorld *pWndWorld );
	//
	// 오브젝트가 삭제되기전에 호출된다.
	virtual void OnDestroyObj( XEBaseWorldObj *pObj ) {}
};
