#pragma once
#include "XWnd.h"
#include "SprObj.h"
//-------------------------------------------------------------------------------------
class XWndSprObj : public XWnd
{
	XSprObj *m_pSprObj;
//	XE::VEC2 m_vPos;
	void Init() {
		m_pSprObj = NULL;
	}
	void Destroy() {
		SAFE_DELETE( m_pSprObj );
	}
public:
	template<typename T>
	XWndSprObj( XWndMng *pWndMng, LPCTSTR szSpr, ID idAct, T x, T y )		// 이거쓰지말고 밑에거 쓸것.
		: XWnd( pWndMng, x, y )
	{
		Init();
//		m_vPos.Set( x, y );
		SetPosLocal( x, y );
		m_pSprObj = new XSprObj( szSpr );
		m_pSprObj->SetAction( idAct );

	}
	template<typename T>
	XWndSprObj( LPCTSTR szSpr, ID idAct, T x, T y ) 
		: XWnd( NULL, x, y )
	{
		Init();
		SetPosLocal( x, y );
		m_pSprObj = new XSprObj( szSpr );
		m_pSprObj->SetAction( idAct );

	}
	virtual ~XWndSprObj() { Destroy(); }
	virtual void Draw( void ) {
		m_pSprObj->Draw( GetPosFinal() );
	}
};