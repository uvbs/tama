#pragma once
#include "XKeyBase.h"

//////////////////////////////////////////////////////////////////////////
class XKeyPos : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
//	static xSpr::xtKey sGetType() {
//		return xSpr::xKT_MOVE;
//	}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_POS;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_MOVE;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_POS;
	}
//	static const xSpr::xtKeySub s_subType = xSpr::xKTS_POS;
	static XKeyPos* Cast( XBaseKey *pBaseKey ) {
		if( pBaseKey == nullptr )	return nullptr;
		if( pBaseKey->GetSubType() != xSpr::xKTS_POS ) return nullptr;
		return SafeCast<XKeyPos*>( pBaseKey );
	}
// 	static void Link( XBaseKey *pKey1, XBaseKey *pKey2 ) {
// 		XKeyPos *pPosKey1 = XKeyPos::Cast( pKey1 );
// 		XKeyPos *pPosKey2 = XKeyPos::Cast( pKey2 );
// 		pPosKey1->SetpNextPathKey( pPosKey2 );		// 앞키에 뒤키를 연결시킴
// 		pPosKey2->SetpPrevPathKey( pPosKey1 );		// 뒤키에 앞키를 연결시킴
// 	}
	// 외부호출 방지용
	XKeyPos( const XKeyPos& src ) {
		*this = src;
	}
private:
	float m_x, m_y;		// 나중에 VEC2형으로 최적화 시킬것
	XE::VEC2 m_vRangeX, m_vRangeY;	// 랜덤요소
	float m_Radius = 0;		// 랜덤요소
	XE::VEC2 m_vPos;		// 랜덤요소까지 결정된 최종 좌표
	xSpr::xtInterpolation m_Interpolation;		// 다음키까지 보간으로 이동해야함
// 	ID m_idPath;			// 패스 아이디
// 	SPR::xtLINE m_PathType;	// 선형패스냐, 스플라인패스냐 이런거
//	ID m_idInterpolationEndKey;	// 보간 끝키. 여기에 값이 있으면 this키는 자동으로 보간시작키가 된다
// 	ID m_idNextPathKey;	// 패스로 연결되어 있는 다음키
// 	ID m_idPrevPathKey;	// 패스로 연결되어 있는 이전키
// 	XCOLOR m_colPath;		// 패스컬러
//	XSPLayerMove m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType( xSpr::xKTS_POS );
		m_x = m_y = 0;
		m_vPos.Init();
		m_Interpolation = xSpr::xLINEAR;
// 		m_idNextPathKey = m_idPrevPathKey = 0;
//		m_idInterpolationEndKey = 0;
// 		m_idPath = 0;
// 		m_PathType = SPR::xLINE_LINE;
// 		m_colPath = XCOLOR_WHITE;
	}
protected:
	XKeyPos() : XBaseKey() { Init(); }
public:
	XKeyPos( XSPAction spAction, ID idLayer, ID idLocalInLayer )
		: XBaseKey( spAction, idLayer, idLocalInLayer ) {
		Init();
	}
	XKeyPos( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float x, float y );
	virtual ~XKeyPos() {	}
//	XKeyPos( const XKeyPos& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer );
	XBaseKey* CreateCopy() override;
	// get/set
	GET_SET_ACCESSOR( float, x );
	GET_SET_ACCESSOR( float, y );
	GET_SET_ACCESSOR( xSpr::xtInterpolation, Interpolation );
// 	GET_SET_ACCESSOR( ID, idNextPathKey );
// 	GET_SET_ACCESSOR( ID, idPrevPathKey );
// 	GET_SET_ACCESSOR( ID, idInterpolationEndKey );
// 	void SetpInterpolationEndKey( XKeyPos *pPosKey ) {
// 		m_idInterpolationEndKey = ( pPosKey ) ? pPosKey->GetidKey() : 0;
// 	}
// 	GET_SET_ACCESSOR( SPR::xtLINE, PathType );
// 	GET_SET_ACCESSOR( ID, idPath );
// 	GET_SET_ACCESSOR( XCOLOR, colPath );
	GET_ACCESSOR_CONST( const XE::VEC2&, vRangeX );
	GET_ACCESSOR_CONST( const XE::VEC2&, vRangeY );
	GET_ACCESSOR_CONST( float, Radius );
	bool IsRandomFactor() const {
		return( !m_vRangeX.IsZero() || !m_vRangeY.IsZero() || m_Radius != 0 );
	}
	//GET_ACCESSOR( xSPLayerMove, spLayer );
	void AddPos( float dx, float dy ) { m_x += dx; m_y += dy; }
	void SetPos( float x, float y ) { m_x = x; m_y = y; }
// 	BOOL IsSamePath( XKeyPos *pKeyTarget );
// 	BOOL IsHeadPath() { 
// 		return ( m_idPath && m_idPrevPathKey == 0 ) ? TRUE : FALSE; 
// 	}	// 이 키가 패스의 헤드냐
// 	BOOL IsTailPath() { 
// 		return ( GetpNextPathKey() == nullptr ) ? TRUE : FALSE; 
// 	}	// 이 키가 패스의 꼬리냐

	//
// 	XKeyPos* GetpNextPathKey( ID idPathSrc = 0 );
// 	void SetpNextPathKey( XKeyPos *pPosKey ) { 
// 		m_idNextPathKey = ( pPosKey ) ? pPosKey->GetidKey() : 0; 
// 	}
// 	XKeyPos* GetpPrevPathKey();
// 	void SetpPrevPathKey( XKeyPos *pPosKey ) { 
// 		m_idPrevPathKey = ( pPosKey ) ? pPosKey->GetidKey() : 0; 
// 	}
	XKeyPos* GetpInterpolationEndKey();
	// virtual 
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec = 0 ) override;
	virtual BOOL EditDialog();
	virtual void ValueAssign( XBaseKey *pSrc ) {
		XKeyPos *pKey = dynamic_cast<XKeyPos *>( pSrc );
		XBREAK( pKey == nullptr );
		*this = *pKey;
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
//	virtual void OnAdded();
	// XBaseElem
// 	virtual XE::VEC2 GetPos() { return XE::VEC2( m_x, m_y ); }
	virtual XE::VEC2 GetPos();
	virtual void SetPos( const XE::VEC2& vPos ) { m_x = vPos.x; m_y = vPos.y; }
	XE::VEC2 GetvPosOrig() const {
		return XE::VEC2( m_x, m_y );
	}
	XE::xRECT GetRectRandom() const {
		const auto vLT = XE::VEC2( m_vRangeX.v1, m_vRangeY.v1 );
		const auto vRB = XE::VEC2( m_vRangeX.v2, m_vRangeY.v2 );
		return XE::xRECT( vLT, vRB - vLT );
	}
	//	virtual BOOL IsInside( const XE::VEC2& vStart, const XE::VEC2& vEnd, const XE::VEC2& size );
	//	virtual BOOL IsInside( float x, float y, float w, float h );
	inline XKeyPos* GetpNextPosKey() {		// this의 키타입/서브타입과 같은종류의 키를 this가 속한 레이어에서 찾는다
		return SafeCast<XKeyPos*>( GetNextKey() );
	}
	inline XKeyPos* GetpPrevPosKey() {
		return SafeCast<XKeyPos*>( GetPrevKey() );
	}
	void InitRandom() override {
		m_vPos.Init();		// 랜덤적용되는 변수는 초기화를 시킴
	}
private:
	XE::VEC2 AssignRandom() const;
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // class XKeyPos
