/*
 *  XBaseObj.h
 *  Game
 *
 *  Created by xuzhu on 11. 1. 27..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */


#ifndef __XBASEOBJ_H__
#define __XBASEOBJ_H__

#include <list>
using namespace std;
#include "SprObj.h"
#include "XFactory.h"
class XBaseObj
{
private:
	XSprObj *m_pSprObj;
	void _Init( void ) {
		m_pSprObj = NULL;
	}
	void _Destroy( void ) { 
		SAFE_DELETE( m_pSprObj ); 
	}
protected:
	SET_ACCESSOR( XSprObj*, pSprObj );
public:
	XBaseObj() { _Init();}
	XBaseObj( XSprObj *pSprObj ) { 
		_Init();  
		m_pSprObj = pSprObj;
	}
	XBaseObj( LPCTSTR szSprName, ID idAct, xRPT_TYPE playType );
	virtual ~XBaseObj() { _Destroy(); }	
	// get/set
	GET_ACCESSOR( XSprObj*, pSprObj );
	void SetRotate( float angle ) { 
		if( m_pSprObj )
			m_pSprObj->SetRotate( angle ); 
	}
	// virtual
//	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XBaseObj* pParent=NULL ) { return new XSprObj( szSpr, pParent ); }
	virtual void OnEventCreateObj( float lx, float ly, WORD id, float fAngle, float fOverSec ) {}
	virtual void OnProcessDummy( ID id, const XE::VEC2& vLocalT, float ang, const XE::VEC2& vScale, float dt, float fFrmCurr ) {}
};

typedef list<XBaseObj *>				XBaseObj_List;
typedef list<XBaseObj *>::iterator		XBaseObj_Itor;

#endif



