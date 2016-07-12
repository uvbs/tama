#pragma once
//#include "XLib.h"
//#include "XList.h"
#include "etc/xMath.h"
#include "etc/Timer.h"
#include "sprite/SprObj.h"
#include "_Wnd2/XWnd.h"
// 게임에서 쓸수있을만한 여러가지 오브젝트 라이브러리

// XGRoll에 추가될수 있는 오브젝트의 기본형
class XGRollObj
{
	ID m_idObj;
//	XE::VEC3 m_vPos;	// 3차원 좌표계
//	XE::VEC3 m_vwPos;	// 트랜스폼된 월드 좌표
	X3D::VEC3 m_vPos;	// 3차원 좌표계
	X3D::VEC3 m_vwPos;	// 트랜스폼된 월드 좌표
	float m_dAngle;
	void Init() {
		m_idObj = 0;
		m_dAngle = 0;
	}
	void Destroy() {}
public:
//	XGRollObj( const XE::VEC3& vPos ) {
	XGRollObj( ID idObj ) {
		Init();
		m_idObj = idObj;
//		m_vPos = vPos;
	}
	virtual ~XGRollObj() {}
	//
	GET_ACCESSOR( ID, idObj );
	GET_ACCESSOR( X3D::VEC3&, vwPos );
	GET_SET_ACCESSOR( X3D::VEC3&, vPos );
	GET_SET_ACCESSOR( float, dAngle );
	//
	void Transform( const MATRIX& m );
	void Projection( XE::VEC2 *pvXY, float *pScale );	// 원근투영
	// virtual
			void Draw( const XE::VEC2& vPos, float scale, float alpha=1.0f ) { Draw( vPos.x, vPos.y, scale, alpha ); }
	virtual void Draw( float x, float y, float scale, float alpha=1.0f ) {}
	virtual int FrameMove( float dt ) { return 1; }
	virtual void OnLButtonDown( const XE::VEC2& vMouse ) {}
	virtual void OnMouseMove( const XE::VEC2& vMouse ) {}
	virtual void OnLButtonUp( const XE::VEC2& vMouse ) {}
};

class XGRollSprObj : public XGRollObj
{
	void Init( void ) {
		m_pSprObj = NULL;
	}
	void Destroy( void ) {
		SAFE_DELETE( m_pSprObj );
	}
protected:
	XSprObj *m_pSprObj;
public:
	XGRollSprObj( ID idObj, LPCTSTR szSpr, ID idAct ) : XGRollObj( idObj ) { 
		Init(); 
		m_pSprObj = new XSprObj( szSpr, NULL );
		m_pSprObj->SetAction( idAct );
	}
	virtual ~XGRollSprObj() { Destroy(); }	
	//
	GET_ACCESSOR( XSprObj*, pSprObj );

	virtual void Draw( float x, float y, float scale, float alpha );
};

// z방향으로 크게 원을 돌며 세로로 스크롤되는 류의 오브젝트의 원형
class XGRoll : public XWnd
{
public:
	static void RegisterScript( XLua *pLua );
private:
	XList<XGRollObj*> m_listObj;
	float m_rAngle;
	float m_rAngleStart, m_rAngleEnd;		// 자동으로 돌아갈 목표각도
	float m_rPrevAngle;			// 이전프레임 회전값
	float m_rAngleAccel;		// 회전 가속값
	float m_sizeRadius;			// 롤러 반지름 크기 
	BOOL m_bLock;				// 돌아가지 않게 잠금
	float m_rLockAngle;			// 어느위치에서 잠그는지
	CTimer m_timerUp;			// 보간을 시작한 시간
	XE::VEC3 m_vCenter;	// 3d상에서 롤의 중심좌표(회전축)
	XE::VEC3 m_vScale;		// 롤을 임의로 키우고 싶다면..
	XE::VEC2 m_vPos;	// 화면에 위치해야할 좌표
	XE::VEC2 m_Size;		// 크기
	XE::VEC2 m_vPrevMouse;	// 마우스 이전좌표
	int m_DragState;		// 0:안누른상태, 1:누른순간, 2:드래그중
	BOOL m_bMoving;		// 강제로 특정 인덱스로 돌림
	float m_rMoveAngle;
	void Init() { 
		m_rAngle = 0;
		m_rAngleStart = m_rAngleEnd = 0;
		m_rAngleAccel = 0;
		m_rPrevAngle = 0;
		m_DragState = 0;
		m_vScale.Set( 1.0f, 1.0f, 1.0f );
		m_sizeRadius = 0;
		m_bLock = FALSE;
		m_rLockAngle = 0;
		m_bMoving = FALSE;
		m_rMoveAngle = 0;
	}
	void Destroy() {
		m_listObj.Destroy();
	}
public:
	// win pos(x, y, w, h), radius(롤러반지름)
	template<typename T>
	XGRoll( T x, T y, T w, T h, float radius ) 
		: XWnd( x, y, w, h ) { 
		Init(); 
		m_sizeRadius = radius;
		m_vCenter = XE::VEC3( 0, 0, 1.0f + radius );	// 시점에서 1.0앞에 오브젝트가 보이게
		m_vPos.Set( x, y );
		m_Size.Set( w, h );
		// 최초 한번 OnStopRolling을 발생시키기위해서 살짝 돌려준다. 
		m_rAngle = m_rAngleStart = D2R(44.f);
		m_timerUp.Set(0.5f);		//손을 뗀순간 보간타이머 셋
		
	}
	virtual ~XGRoll(void) { Destroy(); }
	// get/set
//	GET_SET_ACCESSOR( float, rAngle );
	GET_SET_ACCESSOR( const XE::VEC3&, vScale );
	GET_ACCESSOR( float, rAngleAccel );
	void SetAngleAccel( float rAngAcc ) {
		m_rAngleAccel = rAngAcc;
		m_rAngleStart = m_rAngleEnd = 0;
		m_timerUp.Off();
	}
	GET_ACCESSOR( float, rAngle );
	BOOL IsRolling( void ) { return m_timerUp.IsOn(); }	// 돌고 있는가
	float GetAngle( void ) { return R2D( m_rAngle ); }		// degree단위 각도
	void SetAngle( float dAngle ) {	// 각도지정. 보간을 하며 돌아간다
		m_rAngleStart = m_rAngle;
		m_rAngleEnd = dAngle;
		m_timerUp.Set(0.25f);
	}
	float GetSelectAngle( void ) {
		float dMadi = 360.0f / m_listObj.size();
		float dAngle = R2D((3.141592*2) - m_rAngle);
		if( dAngle < 0 )
		{
			dAngle = fmod( dAngle, -360.f );
			dAngle += 360.0f;
		}
		dAngle = (int)((dAngle + (dMadi/2.f)) / dMadi) * dMadi;		// 45도 단위로 반올림
		if( dAngle < 0 ) {
			dAngle = fmod( dAngle, -360.f );		// % 360
			dAngle += 360.0f;
		}
		else
			dAngle = fmod( dAngle, 360.f );
		return dAngle;
	}
	int GetSelectIndex( void ) {
		float dMadi = 360.0f / m_listObj.size();
		int idx = (int)(GetSelectAngle() / dMadi);
		idx %= m_listObj.size();		// dAngle이 360.0이 나오면서 idx가 오버되는 경우가 있어서...
		return idx;
	}

	XGRollObj* GetSelectObj( void ) {
		float dAngle = GetSelectAngle();
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			if( pObj->GetdAngle() == dAngle )
				return pObj;
		} END_LOOP;
		return NULL;
	}
	//
	void AddObj( XGRollObj *pObj );
	void AddItem( ID idItem, LPCTSTR szSpr, ID idAct );
	void Lock( float rAngle ) {
		m_bLock = TRUE;
		m_rLockAngle = rAngle;
	}
	void Unlock( void ) {
		m_bLock = FALSE;
	}
	void MoveRoller( int idx ) {
		m_bMoving = TRUE;
		m_rAngleAccel = 0;
		m_rAngleStart = m_rAngle;
		m_rAngleEnd = -D2R(idx * (360.f / m_listObj.size()));		// 목표각도로 자동으로 돌아가게
		m_timerUp.Set( 0.5f );
	}
	void LuaAddItem( ID idItem, const char*cSpr, ID idAct ) {
		AddItem( idItem, Convert_char_To_TCHAR( cSpr ), idAct );
	}
	// virtual
	virtual int Process( float dt );
//			void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); };		// 상속받은 클래스에서는 스크린좌표 x, y에 그려주고 scale로 스케일링 하면 된다
	virtual void Draw( void );		// 상속받은 클래스에서는 스크린좌표 x, y에 그려주고 scale로 스케일링 하면 된다
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnStopRolling( int nSelectItem ) {}
	// lua

};

