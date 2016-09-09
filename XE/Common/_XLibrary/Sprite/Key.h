/*
 *  Key.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

#include "VersionXE.h"
#include "Layer.h"
//#include "Sprdef.h"

class XSprDat;
class XSprObj;
class XSprite;
class XBaseKey;
class XActDat;
class XBaseRes;
class XActObj;
////////////////////////////////////////////////////
class XBaseKey
{
public:
	enum xTYPE { xKEY_NONE = 0, xKEY_IMAGE, xKEY_CREATEOBJ, xKEY_SOUND, xKEY_EVENT, xKEY_DUMMY, xKEY_MAX };
	enum xSUBTYPE { xSUB_MAIN, xSUB_POS, xSUB_EFFECT, xSUB_ROT, xSUB_SCALE };
	LPCTSTR GetTypeString( XBaseKey::xTYPE type = XBaseKey::xKEY_NONE ) { 
		if( type == XBaseKey::xKEY_NONE )
			type = GetType();
		switch( type ) 
		{
		case XBaseKey::xKEY_IMAGE:		return _T("xKEY_IMAGE");
		case XBaseKey::xKEY_CREATEOBJ:	return _T("xKEY_CREATEOBJ");
		case XBaseKey::xKEY_SOUND:		return _T("xKEY_SOUND");
		case XBaseKey::xKEY_EVENT:		return _T("xKEY_EVENT");
		case XBaseKey::xKEY_DUMMY:	return _T("xKEY_DUMMY");
		default:
			XLOG( "잘못된 키 타입 %d", (int)type );
			return _T("xKEY_ERROR");
		}
	}
	LPCTSTR GetSubTypeString( void ) {
		XBaseKey::xSUBTYPE type = m_SubType;
		switch( type ) 
		{
		case XBaseKey::xSUB_MAIN:		return _T("xSUB_MAIN");
		case XBaseKey::xSUB_POS	:	return _T("xSUB_POS");
		case XBaseKey::xSUB_EFFECT:		return _T("xSUB_EFFECT");
		case XBaseKey::xSUB_ROT:		return _T("xSUB_ROT");
		case XBaseKey::xSUB_SCALE:		return _T("xSUB_SCALE");
		default:
			XLOG( "잘못된 서브키 타입 %d", (int)type );
			return _T("xSUB_ERROR");
		}
	}
	static XBaseKey* sCreateLoad( XBaseRes* pRes, XSprDat* pSprDat, int verSpr );
private:
	static XBaseKey* sCreate( ID snKey, xTYPE type, xSUBTYPE typeSub, ID idLayer, ID idLocalInLayer, int verSpr );
	ID m_idKey;			// 고유 아이디
	ID m_idLocalInLayer = 0;	// 레이어내에서 고유한 키 아이디
	ID m_idLayer = 0;		// 참조하는 레이어
	XBaseKey::xTYPE m_Type;
	XBaseKey::xSUBTYPE m_SubType;
	float m_fFrame;
	xSpr::xtLayer m_LayerType;
	int m_nLayer;						// 속해있는 레이어 번호
	BOOL m_bLua;					// 루아코드를 갖는 키인가(실제 코드는 가지지 않음)
	void Init( void ) {
		m_idKey = 0;		// 게임에선 반드시 파일에서 로딩한 아이디만 있다
		m_Type = XBaseKey::xKEY_NONE; 
		m_SubType = XBaseKey::xSUB_MAIN;
		m_fFrame = 0; 
		m_LayerType = xSpr::xLT_NONE;
		m_nLayer = 0;
		m_bLua = FALSE;
	}
protected:		// draw관련
public:
	XBaseKey() { Init(); }
	XBaseKey( XBaseKey::xTYPE type ) { 
		Init(); 
		m_Type = type; 
		m_SubType = XBaseKey::xSUB_MAIN; 
	}
	XBaseKey( XBaseKey::xTYPE type, XBaseKey::xSUBTYPE subType ) { 
		Init(); 
		m_Type = type; 
		m_SubType = subType; 
	}
#ifndef _XSPR_LOAD2
	XBaseKey( XBaseKey& keyBase ) {		// 카피 생성자
		Init();
		m_idKey = keyBase.GetidKey();
		m_idLayer = keyBase.m_idLayer;
		m_idLocalInLayer = keyBase.m_idLocalInLayer;
		m_Type = keyBase.GetType();
		m_SubType = keyBase.GetSubType();
		m_fFrame = keyBase.GetfFrame(); 
		m_LayerType = keyBase.GetLayerType();
		m_nLayer = keyBase.GetnLayer();
		m_bLua = keyBase.GetbLua();		// 게임에선 루아코드까지 카피할필요는 없음. 애초에 읽지도 않음.
	}
#endif // not _XSPR_LOAD2
	virtual ~XBaseKey() {}

	GET_ACCESSOR_CONST( ID, idKey );
	GET_SET_ACCESSOR_CONST( float, fFrame );
	GET_SET_ACCESSOR_CONST( XBaseKey::xTYPE, Type );
	GET_SET_ACCESSOR_CONST( XBaseKey::xSUBTYPE, SubType );
	GET_ACCESSOR_CONST( xSpr::xtLayer, LayerType );
	GET_ACCESSOR_CONST( int, nLayer );
	GET_ACCESSOR_CONST( BOOL, bLua );
	GET_ACCESSOR_CONST( ID, idLayer );
	GET_ACCESSOR_CONST( ID, idLocalInLayer );

	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
#ifdef _XSPR_LOAD2
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) = 0;
#else
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes );
#endif // _XSPR_LOAD2
	virtual void SetNextKey( XBaseKey* pKey ) {}
	virtual void OnFinishLoad( XActDat *pAction ) {}
	virtual bool IsRandomType() const { return false; }
}; //BaseKey

////////////////////////////////////////////////////
class XKeyPos : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_POS;
private:
	XE::VEC2 m_vPosOrig;
	XE::VEC2 m_vPosRandomed;
	XE::VEC2 m_vRangeRandomX, m_vRangeRandomY;
	float m_radiusRandom = 0.f;
	XBaseLayer::xtInterpolation m_Interpolation = XBaseLayer::xNONE;		// 다음키까지 보간으로 이동해야함
	XKeyPos* m_pInterpolationEndKey = nullptr;	// 보간 끝키. 여기에 값이 있으면 this키는 자동으로 보간시작키가 된다
	int m_idxInterpolationEndKey = -1;
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_POS );
		m_vPosRandomed.Init();
		m_Interpolation = XBaseLayer::xNONE;
	}
protected:
public:
	XKeyPos() { Init(); }
	~XKeyPos() {}
	// get/set
	GET_ACCESSOR_CONST( const XE::VEC2&, vPosOrig );
	GET_ACCESSOR_CONST( XBaseLayer::xtInterpolation, Interpolation );
	GET_ACCESSOR_CONST( int, idxInterpolationEndKey );
	GET_ACCESSOR_CONST( XKeyPos*, pInterpolationEndKey );
	void Set( XBaseLayer::xtInterpolation interpolation ) { m_Interpolation = interpolation; }
	// virtual 
	void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	void OnFinishLoad( XActDat *pAction ) override;
	inline XE::VEC2 GetvPosRandomed() {
		if( m_vPosRandomed.IsInit() )
			m_vPosRandomed = AssignRandom();
		return m_vPosRandomed;
	}
	XE::VEC2 AssignRandom() const;
	bool IsRandomType() const override;
}; //KeyPos
/////////////////////////////////////////////////////
class XKeyEffect : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_EFFECT;
private:
	XBaseLayer::xtInterpolation m_Interpolation;	// 다음키까지 알파보간을 할건지 말건지
	DWORD m_dwFlag;			// 상/하, 좌/우 플립같은 토글성 이펙트는 이곳에 들어간다.
	xDM_TYPE m_DrawMode;			// screen, dodge, darken 찍기등 선택형 속성은 이곳에 들어간다.
	float m_fOpacityOrig = 1.f;				// 알파블랜드, 투명도 0.0 ~ 1.0
	float m_fOpacityRandomed = 9999.f;
	XE::VEC2 m_vRangeRandomed;
	XKeyEffect *m_pNextKey;	
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_EFFECT );
		m_Interpolation = XBaseLayer::xNONE;
		m_dwFlag = 0;
		m_DrawMode = xDM_NONE;
		m_pNextKey = nullptr;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_EFFECT ) {
			XLOG( "pKey의 Image타입이 EFFECT가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyEffect*, XBaseKey*>(pKey);
	}
public:
	XKeyEffect() { Init(); }
	~XKeyEffect() {}
	// get/set
	BOOL GetFlipHoriz( void ) { return (m_dwFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE; }
	BOOL GetFlipVert( void ) { return (m_dwFlag & EFF_FLIP_VERT) ? TRUE : FALSE; }
	GET_SET_ACCESSOR( float, fOpacityOrig );
	GET_SET_ACCESSOR( DWORD, dwFlag );
	GET_SET_ACCESSOR( xDM_TYPE, DrawMode );
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	void Set( XBaseLayer::xtInterpolation interpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity ) { 
		m_Interpolation = interpolation;
		m_dwFlag = dwDrawFlag;
		m_DrawMode = drawMode;
		m_fOpacityOrig = fOpacity;
	}
	XKeyEffect *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	virtual void SetNextKey( XBaseKey* pKey ) { SetpNextKey( pKey ); }
	inline float AssignRandom() const {
		return m_fOpacityOrig + xRandomF( m_vRangeRandomed.v1, m_vRangeRandomed.v2 );
	}
	float GetOpacityRandomed();
	virtual bool IsRandomType() const {		return false;	}
}; // KeyEffect
//////////////////////////////////////////////////////
class XKeyRot : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_ROT;
private:
	float m_dAngleOrig;					//  누적 절대각도. +720, -720도 모두 될수 있다.
	float m_dAngleRandomed = 9999.f;
	XE::VEC2 m_vRangeRandom;
	XBaseLayer::xtInterpolation m_Interpolation;		// 다음키까지 보간
	XKeyRot *m_pNextKey;
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_ROT );
		m_dAngleOrig = 0;
		m_Interpolation = XBaseLayer::xNONE;
		m_pNextKey = NULL;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_ROT ) {
			XLOG( "pKey의 Image타입이 ROTATE가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyRot*, XBaseKey*>(pKey);
	}
public:
	XKeyRot() { Init(); }
	~XKeyRot() {}
	// get/set
	GET_SET_ACCESSOR( float, dAngleOrig );
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	void AddAngleZ( float angle ) { 	
		m_dAngleOrig += angle; 	
	}
	void Set( XBaseLayer::xtInterpolation interpolation ) { 
		m_Interpolation = interpolation; 
	}
	XKeyRot *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	void SetNextKey( XBaseKey* pKey ) override { 
		SetpNextKey( pKey ); 
	}
	inline float AssignRandom() const {
		return m_dAngleOrig + xRandomF( m_vRangeRandom.v1, m_vRangeRandom.v2 );
	}
	float GetdAngRandomed() {
		if( m_dAngleRandomed == 9999.f )
			m_dAngleRandomed = AssignRandom();
		return m_dAngleRandomed;
	}
	virtual bool IsRandomType() const {
		return false;
	}
}; // KeyRot
////////////////////////////////////////////////////
class XKeyScale : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_SCALE;
private:
	XE::VEC2 m_vScaleOrig;
	XE::VEC2 m_vScaleRandomed;
	XE::VEC2 m_vRangeRandomX, m_vRangeRandomY, m_vRangeRandomXY;
	XBaseLayer::xtInterpolation m_Interpolation;
	XKeyScale *m_pNextKey;
	void Init() {
		SetSubType( XBaseKey::xSUB_SCALE );
		m_vScaleOrig.Set( 1.f );
		m_vScaleRandomed.Init();
		m_Interpolation = XBaseLayer::xNONE;
		m_pNextKey = NULL;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_SCALE ) {
			XLOG( "pKey의 서브타입이 SCALE가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyScale*, XBaseKey*>(pKey);
	}
public:
	XKeyScale() { Init(); }
	~XKeyScale() {}
	// get/set
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScaleOrig );
	GET_ACCESSOR_CONST( XBaseLayer::xtInterpolation, Interpolation );
	void AddScale( float sx, float sy ) { 
		m_vScaleOrig += XE::VEC2(sx,sy);
	}
	void MulScale( float xMul, float yMul ) {		// 인수는 1.0=100%인 배수가 들어온다
		m_vScaleOrig *= XE::VEC2( xMul, yMul );
	}		
	XKeyScale *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	void SetNextKey( XBaseKey* pKey ) override { 
		SetpNextKey( pKey ); 
	}
	inline XE::VEC2 GetvScaleRandomed() {
		if( m_vScaleRandomed.IsInit() )
			m_vScaleRandomed = AssignRandom();
		return m_vScaleRandomed;
	}
	XE::VEC2 AssignRandom() const;
	virtual bool IsRandomType() const {
		return false;
	}
}; //KeyScale
////////////////////////////////////////////////////
class XKeyImageLayer : public XBaseKey
{
public: 
private:
	void Init() {
		SetType( XBaseKey::xKEY_IMAGE );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey->GetType() != XBaseKey::xKEY_IMAGE ) {
			XLOG( "pKey의 타입이 IMAGE가 아니다 %d %3.2f", (int)pKey->GetType(), pKey->GetfFrame() );
			return NULL;
		}
#endif
		return pKey;
	}
public: 
	XKeyImageLayer() { Init(); }
	XKeyImageLayer( XBaseKey::xSUBTYPE imgtype ) { Init(); SetSubType( imgtype ); }
#ifndef _XSPR_LOAD2
	XKeyImageLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
#endif // not _XSPR_LOAD2
	virtual ~XKeyImageLayer() {}
	
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void SetNextKey( XBaseKey* pKey ) { XBREAK(1); }
};
////////////////////////////////////////////////////
class XKeyImage : public XKeyImageLayer
{
	int m_nSpr;		// m_pSprite의 인덱스 load할때만 씀
	XSprite *m_pSprite;
	void Init() {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_nSpr = 0;
		m_pSprite = NULL;
	}
public:
	XKeyImage() : XKeyImageLayer(XBaseKey::xSUB_MAIN) { Init(); }
#ifndef _XSPR_LOAD2
	XKeyImage( XBaseKey& keyBase, XKeyImage& key ) : XKeyImageLayer( keyBase ) {
		Init();
		m_nSpr = key.GetnSpr();
	}
#endif // not _XSPR_LOAD2
	~XKeyImage() {}
	GET_ACCESSOR( int, nSpr );
	GET_SET_ACCESSOR( XSprite*, pSprite );
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyImageLayer::SetpNextKey( pKey ); }
};
////////////////////////////////////////////////////

////////////////////////////////////////////////////
class XKeyObjLayer : public XBaseKey
{
public: 
private:
	void Init( void ) {
		SetType( XBaseKey::xKEY_CREATEOBJ );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
		if( XBREAKF( pKey->GetType() != XBaseKey::xKEY_CREATEOBJ, "pKey의 타입이 CREATEOBJ가 아니다 %d %3.2f", (int)pKey->GetType(), pKey->GetfFrame() ) )
			return NULL;
		return pKey;
	}
public:
	XKeyObjLayer() { Init(); }
#ifndef _XSPR_LOAD2
	XKeyObjLayer( XBaseKey::xSUBTYPE type ){ Init(); SetSubType( type ); } 
	XKeyObjLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
#endif // not _XSPR_LOAD2
	virtual ~XKeyObjLayer() {}
	
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void SetNextKey( XBaseKey* pKey ) {XBREAK(1);}
};
////////////////////////////////////////////////////
class XKeyCreateObj : public XKeyObjLayer
{	
	DWORD m_dwID;					// 4바이트짜리 고유번호
	TCHAR m_szSprName[128];		// 스프라이트 파일 이름
	int m_nAction;						// 스프라이트를 읽은후 SetAction()을 할 번호
	xRPT_TYPE m_PlayType;
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_dwID = 0;
		memset( m_szSprName, 0, sizeof(m_szSprName) );
		m_nAction = 0;
		m_PlayType = xRPT_LOOP;
	}
	void Destroy( void ) ;
public:
	XKeyCreateObj() { Init(); }
#ifndef _XSPR_LOAD2
	XKeyCreateObj( XBaseKey& keyBase, XKeyCreateObj& key ) : XKeyObjLayer( keyBase ) {
		Init();
		m_dwID = key.GetdwID();
		_tcscpy_s( m_szSprName, key.GetszSprName() );
		m_nAction = key.GetnAction();
		m_PlayType = key.GetPlayType();
	}
#endif // not _XSPR_LOAD2
	virtual ~XKeyCreateObj() { Destroy(); }
	
	GET_ACCESSOR( LPCTSTR, szSprName );
	GET_ACCESSOR( int, nAction );
	GET_ACCESSOR( DWORD, dwID );
	GET_SET_ACCESSOR( xRPT_TYPE, PlayType );
	void Set( xRPT_TYPE playType ) {
		m_PlayType = playType;
	}
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyObjLayer::SetpNextKey( pKey ); }
};

////////////////////////////////////////////////////
namespace KE {
	enum xTYPE {
		NONE = 0,
		CREATE_OBJ,			// 오브젝트 생성 이벤트
		HIT,					// 타격 이벤트
		CREATE_SFX,
		ETC = 999			// 기타 다용도 이벤트
	};
	struct KEY_PARAM {
		union {
			struct {
				BYTE b[4];
			};
			struct {
				WORD w[2];
			};
			DWORD dwParam;
			int nParam;
			float fParam;
		};
	};
}
#define SET_KEY_PARAM_DWORD( V, IDX )       inline void Set##V( DWORD val )   { m_Param[IDX].dwParam = val; };
#define GET_KEY_PARAM_DWORD( V, IDX )       inline DWORD Get##V() const { return m_Param[IDX].dwParam; };
#define GET_SET_KEY_PARAM_DWORD( V, IDX )   SET_KEY_PARAM_DWORD( V, IDX ) GET_KEY_PARAM_DWORD( V, IDX )
//
#define SET_KEY_PARAM_INT( V, IDX )       inline void Set##V( int val )   { m_Param[IDX].nParam = val; };
#define GET_KEY_PARAM_INT( V, IDX )       inline int Get##V() const { return m_Param[IDX].nParam; };
#define GET_SET_KEY_PARAM_INT( V, IDX )   SET_KEY_PARAM_INT( V, IDX ) GET_KEY_PARAM_INT( V, IDX )
//
#define SET_KEY_PARAM_FLOAT( V, IDX )       inline void Set##V( float val )   { m_Param[IDX].fParam = val; };
#define GET_KEY_PARAM_FLOAT( V, IDX )       inline float Get##V() const { return m_Param[IDX].fParam; };
#define GET_SET_KEY_PARAM_FLOAT( V, IDX )   SET_KEY_PARAM_FLOAT( V, IDX ) GET_KEY_PARAM_FLOAT( V, IDX )
//
#define SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )       inline void Set##V( BOOL val )   { m_Param[IDX].b[IDXBYTE] = (BYTE)val; };
#define GET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )       inline BOOL Get##V() const { return (BOOL)(m_Param[IDX].b[IDXBYTE]); };
#define GET_SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )   SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE ) GET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )

class XKeyEvent : public XBaseKey
{
	KE::xTYPE m_Event;		// 
	float m_lx, m_ly;				// 이벤트 발생 좌표. 센터를 기준으로한 로컬좌표
	KE::KEY_PARAM	m_Param[16];		// 4 * 16 byte만큼을 파라메터로 쓸수 있다
	_tstring m_strSpr;
	std::vector<std::string> m_aryStrParam;
	void _Init() {
		m_lx = m_ly = 0;
		m_Event = KE::NONE;
		memset( m_Param, 0, sizeof(m_Param) );
	}
	void _Destroy() {}
public:
	XKeyEvent() 
		: XBaseKey( XBaseKey::xKEY_EVENT ) 
		, m_aryStrParam(4) { 
		_Init(); 
	}
#ifndef _XSPR_LOAD2
	XKeyEvent( XBaseKey& keyBase, XKeyEvent& key ) : XBaseKey( keyBase ) {
		_Init();
		m_Event = key.GetEvent();
		m_lx = key.Getlx();
		m_ly = key.Getly();
		memcpy_s( (void*)m_Param, sizeof(m_Param), (void*)key.GetParam(), sizeof(m_Param) );
//		m_Param = key.GetParam();
	}
#endif // not _XSPR_LOAD2
	virtual ~XKeyEvent() { _Destroy(); }
	
	GET_ACCESSOR( float, lx );
	GET_ACCESSOR( float, ly );
	GET_ACCESSOR( KE::xTYPE, Event );
	GET_ACCESSOR( _tstring&, strSpr );
	GET_ACCESSOR( const KE::KEY_PARAM*, Param );
	void SetPos( float lx, float ly ) { m_lx = lx; m_ly = ly; }
	void SetAngle( float angle ) { SeteventdAng( angle ); }
	float GetAngle( void ) { return GeteventdAng(); }
	void SetSprFile( const char* cSpr ) {
		m_strSpr = C2SZ( cSpr );
	}
	LPCTSTR GetSprFile( void ) {
		return m_strSpr.c_str();
	}
	GET_SET_KEY_PARAM_DWORD( eventidObj, 0 );
	GET_SET_KEY_PARAM_FLOAT( eventdAng, 1 );
	GET_SET_KEY_PARAM_DWORD( idAct, 2 );
	GET_SET_KEY_PARAM_DWORD( typeLoop, 3 );
	GET_SET_KEY_PARAM_FLOAT( secLifeTime, 4 );
	GET_SET_KEY_PARAM_BOOL( bTraceParent, 5, 0 );	// b[0]사용
	GET_SET_KEY_PARAM_FLOAT( Scale, 6 );
	
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
}; // class XKeyEvent

////////////////////////////////////////////////////////////////////////////////////////////
// class XKeyDummyLayer 
class XKeyDummyLayer : public XBaseKey
{
	static const XBaseKey::xTYPE ThisType = XBaseKey::xKEY_DUMMY;
	static const xSpr::xtLayer ThisLayerType = xSpr::xLT_DUMMY;
public: 
private:
	void Init( void ) {
		SetType( ThisType );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
		if( XBREAKF( pKey->GetType() != ThisType, "pKey의 타입이 %s가 아니다-> %s %3.2f", pKey->GetTypeString( ThisType ), pKey->GetTypeString(), pKey->GetfFrame() ) )
			return NULL;
		return pKey;
	}
public:
	XKeyDummyLayer() { Init(); }
	XKeyDummyLayer( XBaseKey::xSUBTYPE type ){ Init(); SetSubType( type ); } 
#ifndef _XSPR_LOAD2
	XKeyDummyLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
#endif // not _XSPR_LOAD2
	virtual ~XKeyDummyLayer() {}

	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) override {
		XBaseKey::Execute( pSprObj );
		XBREAK( GetLayerType() != ThisLayerType );
	}
};

class XKeyDummy : public XKeyDummyLayer
{
private:
	BOOL m_bActive;
	ID m_id;
	void Init() {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_bActive = TRUE;
		m_id = 0;
	}
public:
	XKeyDummy() { Init(); 	}
#ifndef _XSPR_LOAD2
	XKeyDummy( XBaseKey& keyBase, XKeyDummy& key ) : XKeyDummyLayer( keyBase ) {
		Init();
		m_bActive = key.GetbActive();
		m_id = key.Getid();
	}
#endif // not _XSPR_LOAD2
	virtual ~XKeyDummy() { }

	GET_SET_ACCESSOR( BOOL, bActive );
	GET_ACCESSOR( ID, id );
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
};

////////////////////////////////////////////////////
class XKeyPlaySound : public XBaseKey
{
public:
	XKeyPlaySound() : XBaseKey( XBaseKey::xKEY_SOUND ) {}
#ifndef _XSPR_LOAD2
	XKeyPlaySound( XBaseKey& keyBase, XKeyPlaySound& key ) : XBaseKey( keyBase ) {	}
#endif // not _XSPR_LOAD2
	virtual ~XKeyPlaySound() {}
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) {}
};

class XKeySound : public XBaseKey
{
	ID m_idSound;
	float m_fVolume;
	void Init() {
		m_idSound = 0;
		m_fVolume = 0;
	}
	void Destroy() {}
public:
	XKeySound() { Init(); }
#ifndef _XSPR_LOAD2
	XKeySound( XBaseKey& keyBase, XKeySound& key ) : XBaseKey( keyBase ) {
		Init();
		m_idSound = key.GetidSound();
		m_fVolume = key.GetfVolume();
	}
#endif // not _XSPR_LOAD2
	virtual ~XKeySound() { Destroy(); }
	void Execute( XSprObj *pSprObj, float fOverSec=0 ) override;
	GET_ACCESSOR( ID, idSound );
	GET_ACCESSOR( float, fVolume );
	virtual void Load( XSprDat *pSprDat, XBaseRes *pRes ) override;
};


#include "Key.inl"

XE_NAMESPACE_START( XE )
//
inline bool IsInvalidKeyType( XBaseKey::xTYPE type ) {
	return type <= XBaseKey::xKEY_NONE || type >= XBaseKey::xKEY_MAX;
}
inline bool IsValidKeyType( XBaseKey::xTYPE type ) {
	return !IsInvalidKeyType( type );
}
//
XE_NAMESPACE_END; // XE

