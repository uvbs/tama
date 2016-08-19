#pragma once 

// 이동가능한 인터페이스
class XMovableInterface
{
	BOOL m_bActive;
	XE::VEC2 m_vPos;
	XE::VEC2 m_vScale;
	BOOL m_bHighlight;
	void Init() {
		m_bActive = FALSE;
		m_bHighlight = FALSE;
		m_nDragState = 0;
		m_vScale.Set( 1.0f );
	}
	void Destroy() {}
protected:
	int m_nDragState;
public:
	XMovableInterface() { Init(); }
	XMovableInterface( float x, float y ) { Init(); m_vPos.Set( x, y ); }
	virtual ~XMovableInterface() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bActive );
	const XE::VEC2& GetPosL() { return m_vPos; }
	XE::VEC2 GetPosT() { return m_vPos * m_vScale; }
	GET_SET_ACCESSOR( const XE::VEC2&, vScale );
	void SetPos( float x, float y ) { m_vPos.Set( x, y ); }
	void SetPos( const XE::VEC2& vPos ) { m_vPos = vPos; }
	GET_SET_ACCESSOR( BOOL, bHighlight );
	void AddPos( float dx, float dy ) { m_vPos.x += dx; m_vPos.y += dy; }
	void AddPos( const XE::VEC2& vDelta ) { m_vPos += vDelta; }
	BOOL IsHighlight() { return (m_bActive)? m_bHighlight : FALSE; }
	// virtual
	void Draw( const XE::VEC2& vCenter, const XE::VEC2& vCenterL, const XE::VEC2& vMouse ) { Draw( vCenter.x, vCenter.y, vCenterL.x, vCenterL.y, vMouse ); }
	virtual void Draw( float cx, float cy, float clx, float cly, const XE::VEC2& vMouse ) = 0;
	virtual void OnMouseMove( float mxl, float myl ) {}
	virtual void OnLButtonDown( float cx, float cy, const CPoint &point ) {}
	virtual void OnLButtonUp( float cx, float cy, const CPoint &point ) {}
//	virtual void DragMove( float cx, float cy, const CPoint &point, const CPoint &prevPoint );
	virtual void DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist );
};
// 십자선
class XMICross : public XMovableInterface
{
	XE::VEC2 m_vSize;		// 레이어 x,y좌표로 부터의 상대좌표
	float m_fThickness;		// 마우스로 선택할수 있는 두께
	void Init() {
		m_fThickness = 3.0f;
	}
	void Destroy() {}
public:
	XMICross() { Init(); SetbActive( TRUE ); }
	XMICross( float x, float y, float w, float h ) : XMovableInterface( x, y ) {
		Init();
		m_vSize.Set( w, h );
		SetbActive( TRUE );
	}
	virtual ~XMICross() { Destroy(); }
	// get/set
	XE::VEC2 GetSizeT() { return m_vSize * GetvScale(); }		// Transformed
	//
	void Draw( const XE::VEC2& vCenter, const XE::VEC2& vCenterL, const XE::VEC2& vMouse ) { Draw( vCenter.x, vCenter.y, vCenterL.x, vCenterL.y, vMouse ); }
	virtual void Draw( float cx, float cy, float clx, float cly, const XE::VEC2& vMouse );
	virtual void OnMouseMove( float mxl, float myl );
};
// event : create obj 십자선
// 방향있는 십자선
class XMICrossDir : public XMovableInterface
{
	float _m_w;					// 레이어 x,y좌표로 부터의 상대좌표
	float _m_toph, _m_bottomh;		// 0도방향으로 세로선에 좀더 길기때문에 XMovableInterface::m_xy 로부터의 top, bottom의 좌표가 필요했다
	float m_fThickness;		// 마우스로 선택할수 있는 두께
	float m_fAngle;
	float m_fPrevAngle;
	BOOL m_bHighlightDir;		// 방향부분 하이라이트
	float m_fDistAngleLocal;
	XE::VEC2 m_vAdjustAxis;	// 회전/스케일 축
	void Init() {
		_m_w = _m_toph = _m_bottomh = 0;
		m_fThickness = 3.0f;
		m_fAngle = m_fPrevAngle = 0;
		m_bHighlightDir = FALSE;
		m_fDistAngleLocal = 0;
		SetbActive( TRUE );
	}
	void Destroy() {}
public:
	XMICrossDir() { Init(); }
	XMICrossDir( float x, float y, float w, float th, float bh, const XE::VEC2 &vAdjustAxis=XE::VEC2(0,0) ) : XMovableInterface( x, y ) {
		Init();
		_m_w = w;		
		_m_toph = th;	  _m_bottomh = bh;
		m_vAdjustAxis = vAdjustAxis;
	}
	virtual ~XMICrossDir() { Destroy(); }

	GET_SET_ACCESSOR( BOOL, bHighlightDir );
	GET_SET_ACCESSOR( float, fAngle );
	SET_ACCESSOR( float, fPrevAngle );
	float GetwT() { return _m_w * GetvScale().x; }
	float GetTophT() { return _m_toph * GetvScale().y; }
	float GetBottomhT() { return _m_bottomh * GetvScale().y; }
	void Draw( const XE::VEC2& vCenter, const XE::VEC2& vCenterL, const XE::VEC2& vMouse ) { Draw( vCenter.x, vCenter.y, vCenterL.x, vCenterL.y, vMouse ); }
	virtual void Draw( float cx, float cy, float clx, float cly, const XE::VEC2& vMouse );
	virtual void OnMouseMove( float mxl, float myl );
//	virtual void DragMove( float cx, float cy, const CPoint &point, const CPoint &prevPoint );
	virtual void DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist );
	virtual void OnLButtonDown( float cx, float cy, const CPoint &point );
	virtual void OnLButtonUp( float cx, float cy, const CPoint &point );
};

class XLayerMove;
class XBaseKey;
class XMILayerCrossDir : public XMICrossDir
{
	XSPLayerMove m_spLayer;
	XBaseKey *m_pDragKey;
	void Init() {
		m_pDragKey = NULL;
	}
	void Destroy() {}
public:
	XMILayerCrossDir() { Init(); }
	XMILayerCrossDir( const XMILayerCrossDir& src ) {
		*this = src;
		m_spLayer = nullptr;
		m_pDragKey = nullptr;
	}
	XMILayerCrossDir( XSPLayerMove spLayer, float x, float y, float w, float th, float bh ) : XMICrossDir( x, y, w, th, bh ) {
		Init();
		m_spLayer = spLayer;
	}
	virtual ~XMILayerCrossDir() { Destroy(); }
	//
	SET_ACCESSOR( XSPLayerMove, spLayer );
	SET_ACCESSOR( XBaseKey*, pDragKey );

//	virtual void DragMove( float cx, float cy, const CPoint &point, const CPoint &prevPoint );
	virtual void DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist );
	virtual void OnLButtonUp( float cx, float cy, const CPoint &point );
};