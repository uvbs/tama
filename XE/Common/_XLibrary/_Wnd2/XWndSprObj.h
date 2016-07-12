#pragma once
#include "XWnd.h"
#include "sprite/SprObj.h"
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
	void Init() {
		m_pSprObj = NULL;
//		m_bCreate = FALSE;
		m_idAct = 0;
		m_loopType = xRPT_LOOP;
		m_bDraw = TRUE;
	}
	void Destroy() {
//		if( m_bCreate )
			SAFE_DELETE( m_pSprObj );
	}
	XSprObj* CreateSprObj( LPCTSTR szSpr, ID idAct, xRPT_TYPE loopType, bool bAsyncLoad );
protected:
	XSprObj *m_pSprObj;
	ID m_idAct;
	xRPT_TYPE m_loopType;
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

	//
	virtual ~XWndSprObj() { Destroy(); }
	//
	GET_ACCESSOR( const XSprObj*, pSprObj );
	GET_SET_ACCESSOR( xRPT_TYPE, loopType );
	GET_SET_ACCESSOR( BOOL, bDraw );
	GET_SET_ACCESSOR( XEDelegateSprObj*, pDelegate );
	GET_SET_ACCESSOR( const XE::VEC3&, vRotate );
	ID GetidAct() {
		return (m_pSprObj)? m_pSprObj->GetActionID() : 0;
	}
	// 델리게이트가 XEDelegateSprObj와 XDelegateSprObj로 나눠져있어 뽀록을 사용함 ㅠㅠ;
	void SetpDelegateBySprObj( XDelegateSprObj *pDelegate ) {
		if( m_pSprObj )
			m_pSprObj->SetpDelegate( pDelegate );
	}
	inline void SetRotateX( float dAng ) {
		m_vRotate.x = dAng;
	}
	inline void SetRotateY( float dAng ) {
		m_vRotate.y = dAng;
	}
	inline void SetRotateZ( float dAng ) {
		m_vRotate.z = dAng;
	}
	inline void SetColor( XCOLOR col ) {
		if( m_pSprObj )
			m_pSprObj->SetColor( col );
	}
	inline void SetColor( float r, float g, float b ) {
		if( m_pSprObj )
			m_pSprObj->SetColor( r, g, b );
	}
	inline void SetFlipHoriz( BOOL bFlag ) {
		if( m_pSprObj )
			m_pSprObj->SetFlipHoriz( bFlag );
	}
	inline void SetFlipVert( BOOL bFlag ) {
		if( m_pSprObj )
			m_pSprObj->SetFlipVert( bFlag );
	}
	inline void SetFlipHoriz( bool bFlag ) {
		if( m_pSprObj )
			m_pSprObj->SetFlipHoriz( xboolToBOOL(bFlag) );
	}
	inline void SetFlipVert( bool bFlag ) {
		if( m_pSprObj )
			m_pSprObj->SetFlipVert( xboolToBOOL(bFlag) );
	}
	inline void SetFlip( bool bFlip ) {		// 이딴건 왜 만들었데
		SetFlipHoriz( xboolToBOOL(bFlip) );
// 		if( m_pSprObj ) {
// 			if( bFlip )
// 				m_pSprObj->SetRotateY( 180.f );
// 			else
// 				m_pSprObj->SetRotateY( 0 );
// 		}
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
// 	void SetSprObj( LPCTSTR szSpr, ID idAct=0, xRPT_TYPE loopType = xRPT_LOOP, bool ASyncLoad );
// 	void SetSprObj( const _tstring& strSpr, ID idAct=0, xRPT_TYPE loopType = xRPT_LOOP) {
// 		SetSprObj( strSpr.c_str(), idAct, loopType );
// 	}
	void SetAction( ID idAct, xRPT_TYPE typeLoop = xRPT_LOOP ) {
		if( m_pSprObj )
			m_pSprObj->SetAction( idAct, typeLoop );
	}
	void GoFirstFrame( void ) {
		if( m_pSprObj )
			m_pSprObj->ResetAction();
	}
	void GoRandomFrame() {
		if( m_pSprObj )
			m_pSprObj->JumpToRandomFrame();
	}
	void SetSizeSprObjHeight( float h ) {
		float ratio = h / m_pSprObj->GetHeight();
		SetScaleLocal( ratio );
	}
	virtual int Process( float dt ) override;
	virtual void Draw( void ) override;
	// 
// 	XE::xRECT GetBoundBoxByVisibleFinal() const override {
// 		if( m_pSprObj && m_pSprObj->GetAction() ) {
// 			XE::xRECT rect;
// 			auto vPos = GetPosFinal();
// 			rect.vLT = vPos + m_pSprObj->GetAction()->GetBoundBoxLT() * m_scaleBB;
// 			rect.vRB = vPos + m_pSprObj->GetAction()->GetBoundBoxRB() * m_scaleBB;
// 			return rect;
// 		} else {
// 			return XWnd::GetBoundBoxByVisibleFinal();
// 		}
// 	}
	/**
	 @brief 부모기준 로컬좌표계로 돌려준다.
	*/
// 	XE::xRECT GetBoundBoxByVisibleLocal( void ) override {
// 		if( m_pSprObj && m_pSprObj->GetAction() ) {
// 			const float scale = GetScaleLocal().x * m_scaleBB;
// 			const auto pAct = m_pSprObj->GetAction();
// 			return XE::xRECT( pAct->GetBoundBoxLT(), pAct->GetBoundBoxSize() ) * scale;
// // 			rect.vLT = m_pSprObj->GetAction()->GetBoundBoxLT() * scale;
// // 			rect.vRB = m_pSprObj->GetAction()->GetBoundBoxRB() * scale;
// //			return rect;
// 		}
// 		else {
// 			return XWnd::GetBoundBoxByVisibleLocal();
// 		}
// 	}
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
