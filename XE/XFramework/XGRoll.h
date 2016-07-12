#pragma once
//#include "XLib.h"
//#include "XList.h"
#include "etc/xMath.h"
#include "etc/Timer.h"
#include "sprite/SprObj.h"
#include "_Wnd2/XWnd.h"
// ���ӿ��� ������������ �������� ������Ʈ ���̺귯��

// XGRoll�� �߰��ɼ� �ִ� ������Ʈ�� �⺻��
class XGRollObj
{
	ID m_idObj;
//	XE::VEC3 m_vPos;	// 3���� ��ǥ��
//	XE::VEC3 m_vwPos;	// Ʈ�������� ���� ��ǥ
	X3D::VEC3 m_vPos;	// 3���� ��ǥ��
	X3D::VEC3 m_vwPos;	// Ʈ�������� ���� ��ǥ
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
	void Projection( XE::VEC2 *pvXY, float *pScale );	// ��������
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

// z�������� ũ�� ���� ���� ���η� ��ũ�ѵǴ� ���� ������Ʈ�� ����
class XGRoll : public XWnd
{
public:
	static void RegisterScript( XLua *pLua );
private:
	XList<XGRollObj*> m_listObj;
	float m_rAngle;
	float m_rAngleStart, m_rAngleEnd;		// �ڵ����� ���ư� ��ǥ����
	float m_rPrevAngle;			// ���������� ȸ����
	float m_rAngleAccel;		// ȸ�� ���Ӱ�
	float m_sizeRadius;			// �ѷ� ������ ũ�� 
	BOOL m_bLock;				// ���ư��� �ʰ� ���
	float m_rLockAngle;			// �����ġ���� ��״���
	CTimer m_timerUp;			// ������ ������ �ð�
	XE::VEC3 m_vCenter;	// 3d�󿡼� ���� �߽���ǥ(ȸ����)
	XE::VEC3 m_vScale;		// ���� ���Ƿ� Ű��� �ʹٸ�..
	XE::VEC2 m_vPos;	// ȭ�鿡 ��ġ�ؾ��� ��ǥ
	XE::VEC2 m_Size;		// ũ��
	XE::VEC2 m_vPrevMouse;	// ���콺 ������ǥ
	int m_DragState;		// 0:�ȴ�������, 1:��������, 2:�巡����
	BOOL m_bMoving;		// ������ Ư�� �ε����� ����
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
	// win pos(x, y, w, h), radius(�ѷ�������)
	template<typename T>
	XGRoll( T x, T y, T w, T h, float radius ) 
		: XWnd( x, y, w, h ) { 
		Init(); 
		m_sizeRadius = radius;
		m_vCenter = XE::VEC3( 0, 0, 1.0f + radius );	// �������� 1.0�տ� ������Ʈ�� ���̰�
		m_vPos.Set( x, y );
		m_Size.Set( w, h );
		// ���� �ѹ� OnStopRolling�� �߻���Ű�����ؼ� ��¦ �����ش�. 
		m_rAngle = m_rAngleStart = D2R(44.f);
		m_timerUp.Set(0.5f);		//���� ������ ����Ÿ�̸� ��
		
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
	BOOL IsRolling( void ) { return m_timerUp.IsOn(); }	// ���� �ִ°�
	float GetAngle( void ) { return R2D( m_rAngle ); }		// degree���� ����
	void SetAngle( float dAngle ) {	// ��������. ������ �ϸ� ���ư���
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
		dAngle = (int)((dAngle + (dMadi/2.f)) / dMadi) * dMadi;		// 45�� ������ �ݿø�
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
		idx %= m_listObj.size();		// dAngle�� 360.0�� �����鼭 idx�� �����Ǵ� ��찡 �־...
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
		m_rAngleEnd = -D2R(idx * (360.f / m_listObj.size()));		// ��ǥ������ �ڵ����� ���ư���
		m_timerUp.Set( 0.5f );
	}
	void LuaAddItem( ID idItem, const char*cSpr, ID idAct ) {
		AddItem( idItem, Convert_char_To_TCHAR( cSpr ), idAct );
	}
	// virtual
	virtual int Process( float dt );
//			void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); };		// ��ӹ��� Ŭ���������� ��ũ����ǥ x, y�� �׷��ְ� scale�� �����ϸ� �ϸ� �ȴ�
	virtual void Draw( void );		// ��ӹ��� Ŭ���������� ��ũ����ǥ x, y�� �׷��ְ� scale�� �����ϸ� �ϸ� �ȴ�
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnStopRolling( int nSelectItem ) {}
	// lua

};

