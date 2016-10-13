#pragma once
#include "XWnd.h"
//#include "sprite/SprObj.h"
#include "sprite/Sprdef.h"

class XSprObj;
class XDelegateSprObj;
enum xRPT_TYPE : int;
//-------------------------------------------------------------------------------------
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/12/02 22:00
*****************************************************************/
class XWndSprObj;
class XEDelegateSprObj
{
public:
	XEDelegateSprObj() { }
	virtual ~XEDelegateSprObj() { }
	// xRPT_1PLAY로 플레이했을경우 애니메이션이 끝나면 발생한다. 애니메이션객체를 파괴하고 싶다면  true를 리턴한다.
	virtual bool DelegateSprObjFinishAni( XWndSprObj *pWndSpr ) { return true; }
}; // class XEDelegateSprObj
//////////////////////////////////////////////////////////////////////////
class XWndSprObj : public XWnd
{
public:
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot
												, const XE::VEC2& vPos
												, const char *cIdentifier );
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot
												, const XE::VEC2& vPos
												, const std::string& ids ) {
		return sUpdateCtrl( pRoot, vPos, ids.c_str() );
	}
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot
																, LPCTSTR szSpr
																, ID idAct
																, const XE::VEC2& vPos
																, const char *cIdentifier );
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot
																, LPCTSTR szSpr
																, ID idAct
																, const XE::VEC2& vPos
																, const std::string& ids ) {
		return sUpdateCtrl( pRoot, szSpr, idAct, vPos, ids.c_str() );
	}
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot
																, LPCTSTR szSpr
																, ID idAct
																, const char *cIdentifier ) {
		return sUpdateCtrl( pRoot, szSpr, idAct, XE::VEC2(0), cIdentifier );
	}
	static XWndSprObj* sUpdateCtrl( XWnd *pRoot, LPCTSTR szSpr, ID idAct, bool bBatch, const XE::VEC2& vPos, const char *cIdentifier );
private:
//	BOOL m_bCreate;
	BOOL m_bDraw;		// SetbShow()랑은 다른것임. 이건 차일드에 영향없음.
	XE::VEC2 m_vOrig;	// 원래 사용자가 찍으려고 했던 원점 좌표
	float m_scaleBB = 1.f;	// 바운딩박스 스케일링
	bool m_bFlip = false;
	XE::VEC3 m_vRotate;
	_tstring m_strSpr;
	XEDelegateSprObj *m_pDelegate = nullptr;
	float m_secLife = 0.f;	// 0:loop -1:1play >0 : 생존시간
	CTimer m_timerLife;
	int m_Priority = 0;
	void Init() {
		m_pSprObj = NULL;
//		m_bCreate = FALSE;
		m_idAct = 0;
		m_loopType = xRPT_LOOP;
		m_bDraw = TRUE;
	}
	void Destroy();
protected:
	XSprObj *m_pSprObj;
	ID m_idAct;
	xRPT_TYPE m_loopType;
	void CreateSprObj( LPCTSTR szSpr, 
												 ID idAct, 
												 bool bUseAtlas,
												 bool bBatch,
												 bool bAsyncLoad, 
												 xRPT_TYPE loopType );
public:
	// 빈 객체를 만든다.
	XWndSprObj( float x, float y ) : XWnd( x, y ) { Init(); }
	XWndSprObj( int x, int y ) : XWnd( x, y ) { Init(); }
	XWndSprObj( int x, float y ) : XWnd( x, y ) { Init(); }
	XWndSprObj( float x, int y ) : XWnd( x, y ) { Init(); }
	XWndSprObj( const XE::VEC2& vPos ) : XWnd( vPos ) { Init(); }
	XWndSprObj( LPCTSTR szSpr, ID idAct, const XE::VEC2& vPos, xRPT_TYPE loopType=xRPT_LOOP );
	XWndSprObj( LPCTSTR szSpr, ID idAct, const XE::VEC2&& vPos, xRPT_TYPE loopType = xRPT_LOOP ) 
		: XWndSprObj( szSpr, idAct, vPos, loopType ) {}
	XWndSprObj( LPCTSTR szSpr, ID idAct, float x, float y, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( szSpr, idAct, XE::VEC2(x,y), loopType ) {}
	XWndSprObj( LPCTSTR szSpr, ID idAct, int x, int y, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( szSpr, idAct, XE::VEC2( x, y ), loopType ) {}
	XWndSprObj( const _tstring& strSpr, ID idAct, const XE::VEC2& vPos, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( strSpr.c_str(), idAct, vPos, loopType ) {}
	XWndSprObj( const _tstring& strSpr, ID idAct, const XE::VEC2&& vPos, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( strSpr.c_str(), idAct, vPos, loopType ) {}
	XWndSprObj( const _tstring& strSpr, ID idAct, float x, float y, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( strSpr.c_str(), idAct, XE::VEC2(x,y), loopType ) {}
	XWndSprObj( const _tstring& strSpr, ID idAct, int x, int y, xRPT_TYPE loopType = xRPT_LOOP )
		: XWndSprObj( strSpr.c_str(), idAct, XE::VEC2( x, y ), loopType ) {}
	XWndSprObj( LPCTSTR szSpr, ID idAct, bool bBatch, const XE::VEC2& vPos, xRPT_TYPE loopType );
	//
	virtual ~XWndSprObj() { Destroy(); }
	//
	GET_ACCESSOR( const XSprObj*, pSprObj );
	GET_SET_ACCESSOR_CONST( xRPT_TYPE, loopType );
	GET_SET_ACCESSOR_CONST( BOOL, bDraw );
	GET_SET_ACCESSOR( XEDelegateSprObj*, pDelegate );
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vRotate );
	GET_SET_ACCESSOR_CONST( int, Priority );
	ID GetidAct() const;
	// 델리게이트가 XEDelegateSprObj와 XDelegateSprObj로 나눠져있어 뽀록을 사용함 ㅠㅠ;
	void SetpDelegateBySprObj( XDelegateSprObj *pDelegate );
	inline void SetRotateX( float dAng ) {
		m_vRotate.x = dAng;
	}
	inline void SetRotateY( float dAng ) {
		m_vRotate.y = dAng;
	}
	inline void SetRotateZ( float dAng ) {
		m_vRotate.z = dAng;
	}
	void SetColor( XCOLOR col );
	void SetColor( float r, float g, float b );
	void SetFlipHoriz( BOOL bFlag );
	void SetFlipVert( BOOL bFlag );
	void SetFlipHoriz( bool bFlag );
	void SetFlipVert( bool bFlag );
	void SetFlip( bool bFlip ) {		// 이딴건 왜 만들었데
		SetFlipHoriz( xboolToBOOL(bFlip) );
	}
	void SetSprObj( LPCTSTR szSpr, ID idAct, xRPT_TYPE loopType, bool ASyncLoad );
	void SetSprObj( LPCTSTR szSpr, ID idAct ) {
		SetSprObj( szSpr, idAct, xRPT_LOOP, false );
	}
	void SetSprObj( LPCTSTR szSpr, ID idAct, bool bASyncLoad ) {
		SetSprObj( szSpr, idAct, xRPT_LOOP, bASyncLoad );
	}
	void SetSprObj( LPCTSTR szSpr, ID idAct, xRPT_TYPE loopType ) {
		SetSprObj( szSpr, idAct, loopType, false );
	}
	void SetSprObj( const _tstring& strSpr, ID idAct ) {
		SetSprObj( strSpr.c_str(), idAct, xRPT_LOOP, false );
	}
	void SetSprObj( const _tstring& strSpr, ID idAct, bool bASyncLoad ) {
		SetSprObj( strSpr.c_str(), idAct, xRPT_LOOP, bASyncLoad );
	}
	void SetSprObj( const _tstring& strSpr, ID idAct, xRPT_TYPE loopType ) {
		SetSprObj( strSpr.c_str(), idAct, loopType, false );
	}
	void SetSprObj( LPCTSTR szSpr, ID idAct, bool bUseAtlas, bool bBatch, bool bASyncLoad, xRPT_TYPE loopType );
	// 	void SetSprObj( LPCTSTR szSpr, ID idAct=0, xRPT_TYPE loopType = xRPT_LOOP, bool ASyncLoad );
// 	void SetSprObj( const _tstring& strSpr, ID idAct=0, xRPT_TYPE loopType = xRPT_LOOP) {
// 		SetSprObj( strSpr.c_str(), idAct, loopType );
// 	}
	void SetAction( ID idAct, xRPT_TYPE typeLoop = xRPT_LOOP );
	void GoFirstFrame( void );
	void GoRandomFrame();
	void SetSizeSprObjHeight( float h );
	virtual int Process( float dt ) override;
	virtual void Draw( void ) override;
	XE::xRECT GetBoundBoxByVisibleNoTrans() override;
	BOOL IsWndAreaIn( float lx, float ly ) override;
	bool IsWndAreaInByScreen( float x, float y ) override;
	// 충돌감지하는 바운딩박스의 크기를 조절할때 사용
	void SetBoundBoxScale( float scale ) {
		m_scaleBB = scale;
	}
	virtual void OnFinishAni() {}
	void DestroyForReload() override;
	void Reload() override;
	void SetsecLife( float sec ) {
		m_secLife = sec;
		if( sec < 0 )
			sec = 9999.f;		// -1은 무한.
		m_timerLife.Set( sec );
	}
};
