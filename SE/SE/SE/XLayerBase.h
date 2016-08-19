#pragma once
//#include "XLayerBase.h"

//
// 채널
#define CHA_POS		0x01
#define CHA_ROT		0x02
#define CHA_SCALE		0x04
#define CHA_EFFECT	0x08

class XBaseLayer;
class XBaseKey;
struct xLayerInfo;
class XBaseLayer : public std::enable_shared_from_this<XBaseLayer>
{
public:
	// 	enum xSpr:: xtLayer { xSpr::xLT_NONE = 0, xSpr:: xLT_IMAGE, xSpr::xLT_OBJECT,  xSpr::xLT_SOUND, xSpr::xLT_EVENT, xSpr::xLT_DUMMY, xLT_MAX };
	// 	enum xSpr::xtLayerSub { xSpr::xLTS_NONE=0, xSpr::xLTS_MAIN, xSpr::xLTS_POS, xSpr::xLTS_ROT, xSpr::xLTS_SCALE, xSpr::xLTS_EFFECT };
	static LPCTSTR GetTypeStr( xSpr::xtLayer type = xSpr::xLT_NONE );
	LPCTSTR GetTypeString( xSpr::xtLayer type = xSpr::xLT_NONE ) const;
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_NONE;
	}
public:
	static XSPBaseLayer sCreate( xSpr::xtLayer type );
	static XSPBaseLayer sCreate( const xLayerInfo& layerInfo );
//	static XSPBaseLayer sCreateRead( XResFile *pRes, int verSprDat );
private:
	ID m_idGlobalLocalKey = 0;
	//	ID m_idSerial;		// 고유 시리얼 번호
	ID m_idLayer;		// 고유한 레이어 번호(한 액션안에서의 고유한 번호)	// 액션이 통째로 카피되더라도 키가 속한 idLayer를 바꿀필요없이 각 키가 가리키는 레이어는 항상 일정하도록 하기위해 액션내에서는 고유하도록 함.
	float m_fPriority;		// 레이어 우선순위
	xSpr::xtLayer m_bitType;							// 
	BOOL m_bAbleOpen;					// 레이어가 열릴수 있는가
	BOOL m_bOpen;						// 레이어가 열려있는가
	bool m_bShow = true;						// show/hide
	int m_nLayer;								// 레이어번호. 번호는 type마다 따로매겨진다
	float m_fLastFrame;						// 이 레이어에 마지막으로 추가된 키의 프레임번호
	float m_scry;								// FrameView상에서 표시될 y위치
	CString m_strLabel;				// "ImageLayer", "CreateObj" 뭐 이런레이블
	DWORD m_dwDrawChannel;	// 그려져야 하는 채널이 비트로 표시된다. 이것은 XaniAction::CalcLayerY를 수행할때 마다 갱신된다
//	XBaseKey *m_pLastKey;		// 이 레이어에 적용된 마지막 키
//	ID m_idKey = 0;
	void Init() {
		//		m_idSerial = XE::GenerateID();
		m_idLayer = 0;
		m_fPriority = 0;
		m_bitType = xSpr::xLT_NONE;
		m_bAbleOpen = FALSE;
		m_bOpen = TRUE;
//		m_bShow = TRUE;
		m_nLayer = 0;
		m_fLastFrame = 0;
		m_scry = 0;
		m_dwDrawChannel = CHA_POS;
// 		m_pLastKey = NULL;
	}
public:
	XBaseLayer() { Init(); }
	XBaseLayer( xSpr::xtLayer type, LPCTSTR szLabel, BOOL bAbleOpen ) {
		Init();
		m_bitType = type;
		m_strLabel = szLabel;
		m_bAbleOpen = bAbleOpen;
	}
	XBaseLayer( const XBaseLayer& src ) {
		*this = src;
		// 카피후에 의미없는 데이타들은 초기화시키고 다시 할당받게 함.
		m_bShow = true;
//		m_pLastKey = nullptr;
	}
	virtual ~XBaseLayer() {}

	virtual bool OnCreate() { return true; }
	// this로부터 카피본 레이어를 만들어 낸다.
	virtual XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const = 0;
	virtual void SetNewInstanceInSameAction( ID idLayer, int nLayer ) /*= 0 */{
		m_idGlobalLocalKey = 0;
		m_idLayer = idLayer;
		m_nLayer = nLayer;
	}
	virtual void SetNewInstanceInOtherAction() /*= 0*/ {
		// 액션이 카피될때는 모든값 을 그대로 가져가도 됨.
	}
	ID getid() const {
		// 		return m_idSerial;
		return m_idLayer;
	}
	ID GenerateidKeyLocal() {
		return ++m_idGlobalLocalKey;
	}
	GET_SET_ACCESSOR_CONST( ID, idGlobalLocalKey );
	//	GET_SET_ACCESSOR( ID, idSerial );
	GET_SET_ACCESSOR_CONST( ID, idLayer );
	GET_SET_ACCESSOR_CONST( float, fPriority );
	GET_SET_ACCESSOR_CONST( xSpr::xtLayer, bitType );
	GET_SET_ACCESSOR_CONST( BOOL, bAbleOpen );
	GET_SET_ACCESSOR_CONST( BOOL, bOpen );
//	GET_SET_ACCESSOR( BOOL, bShow );
	GET_SET_BOOL_ACCESSOR( bShow );
	GET_SET_ACCESSOR_CONST( int, nLayer );
	GET_SET_ACCESSOR_CONST( float, fLastFrame );
	GET_SET_ACCESSOR_CONST( float, scry );
	SET_ACCESSOR( CString, strLabel );
	CString GetstrLabel() const { XBREAK( m_idLayer == 0 ); return m_strLabel; }
	GET_SET_ACCESSOR_CONST( DWORD, dwDrawChannel );
//	GET_SET_ACCESSOR_CONST( XBaseKey*, pLastKey );
	//	SET_ACCESSOR( SPBaseLayer, pNextLayer );
//	BOOL IsTypeLayerMove() const { return ( m_Type == xSpr::xLT_IMAGE || m_Type == xSpr::xLT_OBJECT || m_Type == xSpr::xLT_DUMMY ) ? TRUE : FALSE; }
	BOOL IsTypeLayerMove() const {
		return (m_bitType & xSpr::xLT_MOVE ) ? TRUE : FALSE;
	}
	BOOL IsTypeLayerImage() const {
		return (m_bitType == xSpr::xLT_IMAGE) ? TRUE : FALSE;
	}
	BOOL IsTypeLayerObj() const {
		return (m_bitType == xSpr::xLT_OBJECT) ? TRUE : FALSE;
	}
	BOOL IsTypeLayerDummy() const {
		return (m_bitType == xSpr::xLT_DUMMY) ? TRUE : FALSE;
	}
	BOOL IsTypeLayerSound() const {
		return (m_bitType == xSpr::xLT_SOUND) ? TRUE : FALSE;
	}
	void SetBitDrawChannel( DWORD dwBit ) {
		m_dwDrawChannel |= dwBit;
	}
	void ClearBitDrawChannel( DWORD dwBit ) {
		m_dwDrawChannel &= ~dwBit;
	}
	BOOL GetBitDrawChannel( DWORD dwBit ) const {
		return (m_dwDrawChannel & dwBit) ? TRUE : FALSE;
	}
	virtual void Clear() {
		//		m_bOpen = FALSE;
		//		m_fLastFrame = 0;
		//		m_scry = 0;
	}
	float GetHeight();		// FrameView상에서 표시될 레이어의 높이를 구한다
	static void sSave( XResFile *pRes, XSPBaseLayer spLayer );
	void Load( XResFile *pRes );
	virtual void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj ) {}
	virtual void MoveFrame( float fFrmCurr, XSPActObjConst spActObj ) {}
	virtual void Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj ) {}
	virtual void DrawLabel( float left, float top, XCOLOR colFont = XCOLOR_WHITE );
	virtual CString GetLabel();
	virtual BOOL IsSameLayer( XSPBaseLayer spLayer ) const {
		if( getid() == spLayer->getid() ) {
			XASSERT( m_bitType == spLayer->GetbitType() && m_nLayer == spLayer->GetnLayer() );
			return TRUE;
		}
		return FALSE;
	}
	bool IsSameLayer( XBaseKey* pKey ) const;
	bool IsSameType( xSpr::xtLayer bitLayer ) const {
		return (m_bitType & bitLayer) != 0;
	}

	virtual DWORD GetPixel( float cx, float cy,
													float mx, float my,
													const D3DXMATRIX &m,
													BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL ) const { return 0; }
	virtual DWORD GetPixel( const D3DXMATRIX &mCamera, const D3DXVECTOR2& mv, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL ) const { return 0; }
//	virtual XSPBaseLayer CopyDeep() = 0;
	virtual void OnSelected( int nEvent ) {};
	virtual XBaseKey* DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType ){ return NULL; }
//	virtual void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj ) {}
	// 레이어의 바운딩 박스를 얻는다. 부모 행렬이 주어지면 그것을 반영한 값을 리턴한다.
	virtual XE::xRECT GetBoundBox( const D3DXMATRIX& mParent ) const { return XE::xRECT(); }
	inline XE::xRECT GetBoundBox() const { 
		D3DXMATRIX m;
		D3DXMatrixIdentity( &m );
		return GetBoundBox( m );
	}
	BOOL CheckPushPlus( CPoint point );
protected:
	XSPBaseLayer GetThis() {
		return shared_from_this();
	}
private:
};
