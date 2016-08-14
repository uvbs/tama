#pragma once
#include <memory>
#include "sprite/XDelegateSprObj.h"
#include "sprite/Sprdef.h"

class XEWndWorld;
class XArchive;
class XEBaseWorldObj;
class XSprObj;

class XEBaseWorldObj : public XDelegateSprObj,
					public std::enable_shared_from_this<XEBaseWorldObj>
{
	static DWORD s_idSerial;
public:	
	static DWORD sGenerateID() {
		return s_idSerial++;
	}
	enum {
		xFG_NONE = 0,
		xFG_TOUCHABLE = 0x01,
		xFG_ERROR=0x80000000,
	};
private:
	int m_Type;					///< 오브젝트의 대분류(유닛인가 or sfx인가 같은...)
	int m_Destroy;
	ID m_snObj;					///< 인스턴스의 고유 번호
	XSprObj *m_pSprObj;
	XSurface *m_pSurface;		///< 애니메이션 없는 객체의 경우 이걸 써본다.
	XE::VEC3 m_vwPos;
	XE::VEC3 m_vScale;			
	float m_Alpha;				///< 사라지는등의 연출을 위해 사용
	XEWndWorld *m_pWndWorld;
	DWORD m_dwFlag;
	_tstring m_strSpr;		// spr파일 이름.
	void Init() {
		m_Type = 0;
		m_snObj = sGenerateID();	// 순차적으로 번호가 생성되므로 나중에 생성된 객체가 먼저생성된 객체보다 위에 찍히게 하고 싶을때 소트용으로 써도 된다.
		m_pSprObj = NULL;
		m_pSurface = NULL;
		m_Destroy = 0;
		m_pWndWorld = NULL;
		m_dwFlag = 0;
		m_vScale.Set(1.f);
		m_Alpha = 1.f;
	}
	void Destroy();
	SET_ACCESSOR( ID, snObj );
protected:
	SET_ACCESSOR( int, Type );
public:
	XEBaseWorldObj( XEWndWorld *pWndWorld ) { 
		Init(); 
		m_pWndWorld = pWndWorld;
	}
	XEBaseWorldObj( XEWndWorld *pWndWorld, int type, LPCTSTR szSpr, ID idAct );
	XEBaseWorldObj( XEWndWorld *pWndWorld, int type, const XE::VEC3& vPos, LPCTSTR szSpr, ID idAct );
	XEBaseWorldObj( XEWndWorld *pWndWorld, int type, const XE::VEC3& vPos, LPCTSTR szImg );
	XEBaseWorldObj( XEWndWorld *pWndWorld, int type, const XE::VEC3& vPos ) { 
		Init(); 
		m_Type = type;
		m_vwPos = vPos;
		m_pWndWorld = pWndWorld;
	}
	virtual ~XEBaseWorldObj() { Destroy(); }
	//
	WorldObjPtr GetThis() {
		return shared_from_this();
	}
	XSPWorldObjConst GetThisConst() const {
		return shared_from_this();
	}
	void SetDestroy( int n ) { m_Destroy = n; }
	GET_ACCESSOR_CONST( int, Type );
	GET_ACCESSOR_CONST( ID, snObj );
	//GET_ACCESSOR( ID, idObj );
// 	ID GetsnObj() {
// 		return m_snObj;
// 	}
	ID getid() {
		return m_snObj;
	}
	GET_ACCESSOR_CONST( int, Destroy );
	GET_SET_ACCESSOR( XEWndWorld*, pWndWorld );
	GET_ACCESSOR( XSprObj*, pSprObj );
	GET_ACCESSOR( XSurface*, pSurface );
//	GET_SET_ACCESSOR( BIT, bitSide );
//	GET_SET_ACCESSOR( float, Z );
	/// this가 filtComp와 같은 편이냐
	float AddZ( float add ) {
//		return m_Z += add;
		m_vwPos.z += add;
		return m_vwPos.z;
	}
	BOOL IsDestroy() const {
		return (m_Destroy == 1 );
	}
	/**
	 @brief 오브젝트의 스케일값.
	 이미지의 스케일값이 아님을 주의. 오브젝트는 카메라에따라 확대축소가 일어날수 있기때문에
	 GetScaleImage()는 최종적으로 화면에 출력되는 스케일값을 얻지만, GetScaleObj는 카메라와는 관계없이
	 이 오브젝트가 어떤스케일 상태인지를 나타낸다.
	*/
	const XE::VEC3& GetScaleObj() const {
		return m_vScale;
	}
	/**
	 @brief 최종 화면에 출력되는 이미지가 얼만큼의 값으로 스케일되었는지 리턴
	 GetScaleObj() * m_pSprObj->GetScale()이 된다.
	*/
	XE::VEC2 GetScaleImage();
	/**
	 @brief 오브젝트의 스케일을 조절한다.
	*/
	void SetScaleObj( float scale ) {
		m_vScale.Set(scale);
	}
	/**
	 @brief 오브젝트의 스케일을 조절한다
	*/
	void SetScaleObj( float sx, float sz ) {
		m_vScale.Set( sx, m_vScale.y, sz );
	}
	GET_SET_ACCESSOR_CONST( float, Alpha );
	void SetRotateY( float dAng );
	/// 터치가능 속성을 지정한다.
	void SetTouchable( BOOL bFlag ) {
		if( bFlag )
			m_dwFlag |= xFG_TOUCHABLE;
		else
			m_dwFlag &= ~xFG_TOUCHABLE;
	}
	BOOL GetTouchable() const {
		return (m_dwFlag & xFG_TOUCHABLE);
	}
	void SetError() {
		m_snObj = 0;
	}
	BOOL IsError() const {
		return m_snObj == 0;
	}
	BOOL IsSuccess() const {
		return ! IsError();
	}
/*
	void SetError() {
		m_dwFlag |= xFG_ERROR;
	}
	BOOL IsError() {
		return (m_dwFlag & xFG_ERROR);
	}
*/
	virtual int GetClassType() const { return m_Type; }
	//
	virtual int Serialize( XArchive& ar );
	virtual int DeSerialize( XArchive& ar );
	//
	BOOL LoadSpr( LPCTSTR szSpr, const XE::xHSL& hsl, ID idAct, xRPT_TYPE typeLoop=xRPT_LOOP );
	inline BOOL LoadSpr( LPCTSTR szSpr, ID idAct, xRPT_TYPE typeLoop = xRPT_LOOP ) {
		return LoadSpr( szSpr, XE::xHSL(), idAct, typeLoop );
	}
	inline BOOL LoadSpr( const _tstring& strSpr, ID idAct, xRPT_TYPE typeLoop = xRPT_LOOP ) {
		return LoadSpr( strSpr.c_str(), XE::xHSL(), idAct, typeLoop );
	}
	inline BOOL LoadSpr( const _tstring& strSpr, const XE::xHSL& hsl, ID idAct, xRPT_TYPE typeLoop = xRPT_LOOP ) {
		return LoadSpr( strSpr.c_str(), hsl, idAct, typeLoop );
	}
	void LoadImage( LPCTSTR szImg );
	//	
	virtual void FrameMove( float dt );
	virtual void Draw( const XE::VEC2& vPos, float scale=1.f, float alpha=1.f );
	virtual XE::VEC3 GetvwPos() const {
		return m_vwPos;
	}
	virtual void SetvwPos( const XE::VEC3& vwPos ) {
		m_vwPos = vwPos;
	}
	// 
	XE::VEC2 GetWindowPos();
	virtual void AddPos( float wx, float wy, float wz ) {
		m_vwPos.x += wx;
		m_vwPos.y += wy;
		m_vwPos.z += wz;
	}
	void AddPos( const XE::VEC3& vDelta ) {
		m_vwPos += vDelta;
	}
	/**
	 @brief 스케일링된 오브젝트의 로컬 중앙좌표를 구한다.
	 GetWidth,GetHeight는 하위클래스에서 바운딩박스등으로 구현해야 한다.
	 바닥을 딛고 서있는류의 오브젝트를 기준으로 한것이다.
	*/
	XE::VEC3 GetvCenterLocal() const {
		return XE::VEC3( 0, 0, GetSize().h / -2.f );
	}
	/**
	 @brief 객체의 머리위 좌표
	 @param adjZ 머리위 좌표에서 추가 보정치
	*/
	XE::VEC3 GetvTopLocal( float adjZ ) const {
		auto vSize = GetSize()/* * m_vScale*/;
		return XE::VEC3( 0, 0, -(vSize.z + adjZ) );
	}
	/**
	 @brief 스케일링 되지 않은 오브젝트의 중앙로컬좌표를 얻는다.
	*/
// 	XE::VEC3 GetvCenterLocalNoScaled() {
// 		return XE::VEC3( 0, 0, GetSizeNoScaled().h / -2.f );
// 	}
	/**
	 @brief 스케일링된 오브젝트의 월드 중앙좌표를 구한다.
	*/
	XE::VEC3 GetvCenterWorld() const {
		return GetvwPos() + GetvCenterLocal();
	}
	/**
	 @brief 오브젝트 중심좌표를 2d투영한 좌표로 돌려준다.
	*/
 	XE::VEC2 GetvsCenter();
	/**
	 @brief 오브젝트의 머리위(상단)좌표를 2d투영한 좌표로 돌려준다.
	 @param adjZ 상단좌표에 추가보정치를 준다. 월드좌표계
	*/
	XE::VEC2 GetvsTop( float adjZ = 0.f );
	/**
	 @brief 오브젝트상단의 월드좌표를 얻는다.
	*/
	XE::VEC3 GetvwTop( float wAdjZ = 0.f ) const {
		return GetvwPos() + GetvTopLocal(wAdjZ);
	}
	/**
	 @brief 오브젝트 중심기준 좌표계로 바운딩박스를 만든다.
	*/
	virtual XE::xRECT GetBoundBoxLocal();
	/**
	 @brief 오브젝트의 바운딩 영역을 월드좌표로 만들어 돌려준다.
	*/
	XE::xRECT GetBoundBoxWorld() {
		XE::xRECT rect = GetBoundBoxLocal();
		XE::VEC2 vwPos = m_vwPos.ToVec2();
		vwPos.y += m_vwPos.z;
		rect.vLT += vwPos;
		rect.vRB += vwPos;
		return rect;
	}
	/**
	 @brief 오브젝트의 바운딩 영역을 스크린좌표로 만들어 돌려준다.
	*/
	XE::xRECT GetBoundBoxScreen();
	XE::xRECT GetBoundBoxWindow();
	//
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnTouch( const XE::xRECT& bbWindow, const XE::VEC2& vlTouch ) {}
	/**
	 @brief 화면에서 얼만큼 벗어나면 객체가 삭제되는지를 물어봄. vDraw는 화면에 찍힐 좌표
	*/
	virtual BOOL DelegateOutOfBoundary( const XE::VEC2& vDraw ) { return FALSE; }
	/**
	 @brief 상속받은 클래스는 자신이 참조하고 있는 shared_ptr을 모두 반환해야한다.
	*/
	virtual void Release() = 0;
	
	/**
	 @brief 오브젝트의 바운딩박스를 기반으로 스케일링 되지 않은 오브젝트 크기를 요구한다.
	*/
//	virtual XE::VEC3 GetSizeNoScaled() const { return XE::VEC3(); }
	/**
	 @brief m_vScale스케일 반영된 오브젝트의 크기를 돌려준다.
	*/
	virtual XE::VEC3 GetSize() const { return XE::VEC3(); }
	/**
	 @brief vwSrc와 this의 거리sq 를 돌려준다.
	*/
	float GetDistSqBetweenPos( const XE::VEC3& vwSrc ) const {
		return (vwSrc - m_vwPos).Lengthsq();
	}
	
friend class XEObjMng;
};

