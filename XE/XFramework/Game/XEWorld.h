#pragma once
#include "XEObjMng.h"

class XEObjMng;
class XEWndWorld;
class XBatchRenderer;

// 게임프레임워크에서 오브젝트들이 들어갈 월드전체를 표현하는 추상화 객체
class XEWorld
{
	XE::VEC2 m_vwSize;		// 월드 전체 크기
	XEObjMng *m_pObjMng;
//	XRenderCmdMng* m_pRenderCmdMng = nullptr;
	void Init() {
		m_pObjMng = NULL;
	}
	void Destroy();
public:
	XEWorld( const XE::VEC2& vwSize );
	virtual ~XEWorld() { Destroy(); }
	//
	virtual void Release( void );
	GET_ACCESSOR_PTR( XEObjMng*, pObjMng );
//	GET_ACCESSOR_MUTABLE( XEObjMng*, pObjMng );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vwSize );
	void SetObjMng( XEObjMng *pObjMng );
	//
	XSPWorldObj GetObjByidObj( ID idObj );
	XE::VEC3 ClippingObj( const XE::VEC3& vwPos, const XE::VEC3& vSize );
	virtual void DestroyAllObj( void );
	//
	ID AddObj( const XSPWorldObj& spObj );
// 	ID AddObj( int type, const XSPWorldObj& spObj );
	virtual XEObjMng* CreateObjMng( int maxObj );
	virtual int Process( XEWndWorld *pWndWorld, float dt );
	virtual void Draw( XEWndWorld *pWndWorld );
	//
	// 오브젝트가 삭제되기전에 호출된다.
	virtual void OnDestroyObj( XEBaseWorldObj *pObj ) {}
};
