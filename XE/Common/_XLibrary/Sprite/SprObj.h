#pragma once

#include "etc/xMath.h"
#include "etc/xGraphics.h"
#include "Sprdef.h"
#ifdef _SPR_USE_LUA
#undef  _SPR_USE_LUA
#endif
#ifdef _SPR_USE_LUA
#include "XLua.h"
#endif
#ifdef _VER_OPENGL
//#include "Mathematics.h"
#endif

class XSprObj;
class XBaseKey;
class XKeyEvent;
class XBaseLayer;
class XLayerMove;
class XLayerImage;
class XLayerObject;
class XSprDat;
class XActDat;
class XSprite;
class XActObj;

struct LAYER_INFO;
struct XEFFECT_PARAM;
XE_NAMESPACE_START( xSpr )
struct xDat;
// KeyEvent 델리게이트시 넘겨주는 파라메터
struct xEvent {
	_tstring m_strSpr;
	ID m_idAct = 0;
	xRPT_TYPE m_PlayMode = xRPT_LOOP;
	XSprObj* m_pSprObj = nullptr;
	XKeyEvent* m_pKey = nullptr;
	ID m_idEvent = 0;
	XE::VEC2 m_vLocal;
	float m_dAng = 0;
	float m_fOverSec = 0;
	float m_Scale = 1.f;
	bool m_bTraceParent = false;
	float m_secLifeTime = 0;
	std::vector<std::string> m_aryStrParam;
	xEvent() : m_aryStrParam(4) {}
};
//
XE_NAMESPACE_END; // xSpr

class XLua;

//////////////////////////////////////////////////////////////////////////
class XBaseObj;
class XDelegateSprObj;
class XSprObj
{
public:
	static float s_LuaDt;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다
private:
#ifdef _XSPR_LAZY_LOAD
	// Lazy로딩할때 필요한 정보
	struct LAZY_INFO {
		ID idAct;			// SetAction되었으나 SprDat가 없어서 세팅되지 못하고 예약상태로 올라감.
		xRPT_TYPE playType;
		_tstring strFilename;	// 읽으려고 한 spr파일명
		CTimer timerLazyLoad;
		BOOL bKeepSrc;
		bool m_bUseAtlas = false;
//		XE::VEC3 vHSL;
		XE::xHSL m_HSL;
		LAZY_INFO() {
			idAct = 0;
			playType = xRPT_DEFAULT;
			bKeepSrc = FALSE;
		}
	};
	LAZY_INFO m_LazyInfo;
#endif
	struct xUseSprObj {
		ID m_idActParent = 0;
		ID m_idLayerParent = 0;
		XSprObj* m_pSprObj = nullptr;
	};
	DWORD m_dwID;												// 이 오브젝트만의고유한 아이디
//	xSpr::XSPDatConst m_spDat;
	xSpr::XSPDat m_spDat;
	XSprObj *m_pParentSprObj;		// this가 자식이면 부모포인터를 가리킨다
	XDelegateSprObj *m_pDelegate;		// 델리게이트
	int m_nNumSprObjs;
	XVector<xUseSprObj> m_aryUseSprObj;
	float m_fFrameCurrent;						// 현재 프레임번호
	BOOL m_bPause;
	int m_nKeyCurr;							// 현재 가리키고 있는 노드의 index
	XActObj *_m_pObjActCurr;				// 현재 선택된 액션. 이거 직접 쓰지말것. 이거 널로 되어있으면 경고띄워주기 위함
	int m_nNumObjActs;
	XActObj **m_ppObjActs;			// sprdat의 액션수만큼 똑같이 생성시켜 각각 레이어를 가지고 있는다
	// transform
	float	m_fAdjustAxisX, m_fAdjustAxisY;	// 회전축보정
	float m_fRotX, m_fRotY, m_fRotZ;
	float m_fScaleX, m_fScaleY;
	float	m_fAlpha;					// 외부지정 옵션
	float	m_ColorR, m_ColorG, m_ColorB;		// 컬러
	xDM_TYPE m_DrawMode;		// 외부지정 옵션
	DWORD m_dwDrawFlag;		// EFF_****
	xRPT_TYPE m_PlayType;			// 루핑등등
	BOOL m_bFinish;					// 애니메이션이 끝났는가
	float m_multiplySpeed;				// 애니메이션 스피드 배속
  bool m_bCallHandler = false;    // 이벤트키의 콜백이 실행중.
#ifdef _XASYNC_SPR
	struct tagAsync {
		ID m_idAsyncLoad = 0;					// 비동기 로딩중.
		ID m_idAct = 0;			// 비동기 로딩중에 들어온 setAction
		xRPT_TYPE m_playType = xRPT_LOOP;
		void Clear() {
			m_idAct = 0;
			m_idAsyncLoad = 0;
			m_playType = xRPT_LOOP;
		}
		} m_Async;
#endif // _XASYNC_SPR
#ifdef _SPR_USE_LUA
	XLua *m_pLua;					// SprObj마다 붙어있는 루아쓰레드
#endif 
	void Init() {
		m_dwID = XE::GenerateID();
		m_pDelegate = NULL;
		m_pParentSprObj = NULL;
		m_nNumSprObjs = 0;
//		m_ppSprObjs = NULL;
		m_fFrameCurrent = 0;
		m_bPause = FALSE;
		_m_pObjActCurr = NULL;
//		m_pParentKey = NULL;
		m_nKeyCurr = 0;
		m_ppObjActs = NULL;
		m_nNumObjActs = 0;
		m_PlayType = xRPT_LOOP;
		m_bFinish = FALSE;
		m_multiplySpeed = 1.0f;
		// transform
		SetScale( 1.0f, 1.0f );
//		m_fScaleX = m_fScaleY = 1.0f;		// 위에서 SetScale()로 하므로 지움
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		m_fAlpha = 1.0f;
		m_ColorR = m_ColorG = m_ColorB = 1.0f;
		m_DrawMode = xDM_ERROR;
		m_dwDrawFlag = 0;
#ifdef _SPR_USE_LUA
		m_pLua = NULL;
#endif
	}
	void Destroy();
	void SetpObjActCurr( XActObj *pObjAct ) { _m_pObjActCurr = pObjAct; }
	SET_ACCESSOR( DWORD, dwID );
	XSprDat* GetpSprDat();
public:
	XSprObj() { Init(); }		// 툴에서 필요해서 살림. new XSprObj;로만 생성해서 sprObj->Load()로 읽어서 에러검출 할수 있도록.
	XSprObj( DWORD dwID );
	XSprObj( LPCTSTR szFilename, XDelegateSprObj *pDelegate = nullptr );
	XSprObj( LPCTSTR szFilename, const XE::xHSL& hsl, XDelegateSprObj *pDelegate = nullptr );
	XSprObj( LPCTSTR szFilename, const XE::xHSL& hsl, bool bUseAtlas, XDelegateSprObj *pDelegate = nullptr );
	// for lua
	XSprObj( BOOL bKeepSrc, const char *cFilename );
#ifdef WIN32
	XSprObj( BOOL bKeepSrc, LPCTSTR szFilename );
#endif // WIN32
	XSprObj( const _tstring& strSpr, XDelegateSprObj *pDelegate = nullptr ) 
		: XSprObj( strSpr.c_str(), pDelegate ) {}

	~XSprObj() { Destroy(); }
	
	inline void Reset() {	// new로 생성이 안되고 캐시에서 다시 가져오게 됐을경우에도 기본적인 사항은 리셋시켜함
		ResetAction();
		SetpObjActCurr( NULL );
	}
	// get/set
	GET_ACCESSOR_CONST( DWORD, dwID );
	inline ID GetidSprObj() const { return m_dwID; }
  GET_SET_ACCESSOR_CONST( bool, bCallHandler );
	inline XActObj* GetpObjActCurr() const;
	XActObj* GetpObjAct( ID idAct ) const;
	bool IsError() const;
	// 현재 비동기 로딩중인가?
//	GET_SET_ACCESSOR( XBaseKey*, pParentKey );
	GET_ACCESSOR_CONST( float, fFrameCurrent );
	GET_SET_ACCESSOR_CONST( BOOL, bPause );
	inline void SetbPause( bool bPause ) {
		m_bPause = (bPause)? TRUE : FALSE;
	}
	inline bool IsPlaying() const { return !m_bPause; }
	GET_ACCESSOR_CONST( xRPT_TYPE, PlayType );
#ifdef _SPR_USE_LUA
	GET_ACCESSOR( XLua*, pLua );
#endif
//	GET_ACCESSOR( BOOL, bFinish );
	void SetRotateX( float angle ) { m_fRotX = angle; }
	void SetRotateY( float angle ) { m_fRotY = angle; }
	void SetRotateZ( float angle ) { m_fRotZ = angle; }
	void SetRotate( float angle ) { SetRotateZ( angle ); }
	GET_ACCESSOR_CONST( float, fRotX );
	GET_ACCESSOR_CONST( float, fRotY );
	GET_ACCESSOR_CONST( float, fRotZ );
	float GetRotate() const { return GetfRotZ(); }
	void SetScale( float scalexy ) { SetScale( scalexy, scalexy ); }
	void SetScale( const XE::VEC2& vScale ) { SetScale( vScale.x, vScale.y ); }
	void SetScale( float sx, float sy ) { m_fScaleX = sx; m_fScaleY = sy; 	}
	float GetScaleX() const { return m_fScaleX; }
	float GetScaleY() const { return m_fScaleY; }
	XE::VEC2 GetScale() const { return XE::VEC2( m_fScaleX, m_fScaleY ); }
	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	void SetAdjustAxis( const XE::VEC2& vAdjAxis ) { 
		m_fAdjustAxisX = vAdjAxis.x; m_fAdjustAxisY = vAdjAxis.y; 
	}
	GET_SET_ACCESSOR_CONST( xDM_TYPE, DrawMode );
	GET_SET_ACCESSOR_CONST( float, fAlpha );
	GET_SET_ACCESSOR_CONST( float, multiplySpeed );
	void SetColor( float r, float g, float b ) {
		m_ColorR = r;
		m_ColorG = g;
		m_ColorB = b;
	}
	void SetColor( XCOLOR col ) {
		m_ColorR = XCOLOR_RGB_R(col) / 255.f;
		m_ColorG = XCOLOR_RGB_G(col) / 255.f;
		m_ColorB = XCOLOR_RGB_B(col) / 255.f;
	}
	GET_ACCESSOR_CONST( float, ColorR );
	GET_ACCESSOR_CONST( float, ColorG );
	GET_ACCESSOR_CONST( float, ColorB );
//	LPCTSTR GetszFilename() { return m_pSprDat->GetszFilename(); }
	LPCTSTR GetSprFilename();
	inline LPCTSTR GetszFilename() {
		return GetSprFilename();
	}
	void SetFlipHoriz( BOOL bFlag ) {
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_HORIZ;
			SetRotateY(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_HORIZ;  
			SetRotateY(0);
		}
	}
		
	void SetFlipVert( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_VERT;
			SetRotateX(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_VERT;  
			SetRotateX(0);
		}
	}
	BOOL IsFlipHoriz() {
		return (m_dwDrawFlag & EFF_FLIP_HORIZ);
	}
	GET_SET_ACCESSOR( XDelegateSprObj*, pDelegate );
	GET_SET_ACCESSOR( XSprObj*, pParentSprObj );
	void AddRotate( float ax, float ay, float az ) { m_fRotX += ax; m_fRotY += ay; m_fRotZ += az; }
	float GetSpeedCurrentAction();
	float GetWidth();
	float GetHeight();
	XE::VEC2 GetSize() { return XE::VEC2( GetWidth(), GetHeight() ); }
	XE::VEC2 GetAdjust();
	BOOL IsFinish() const {				// 애니메이션이 끝났는가?
		return m_bFinish;
	}
	void DoFinish() {
		m_bFinish = TRUE;
	}

	// frame
	void Transform( XE::VEC2 *pvOutPos ) { 
		Transform( &pvOutPos->x, &pvOutPos->y ); 
	}
	void Transform( float *lx, float *ly );
	void Transform( float *fAngle );
	void FrameMove( float dt );
	void JumpKeyPos( XActDat *pAction, float fJumpFrame );	// fFrameCurrent위치로 바로 점프한다. 그사이의 키는 실행하지 않는다
	void JumpToRandomFrame();
	// draw
//	void Draw( int x, int y ) { Draw( (float)x, (float)y ); }
#ifdef _VER_OPENGL
	void Draw( float x, float y );
	void Draw( float x, float y, const MATRIX &m );
#endif
#ifdef _VER_DX
	inline void Draw( float x, float y ) { 
		D3DXMATRIX m; 
		D3DXMatrixIdentity( &m ); 
		Draw( x, y, m ); 
	}
	void Draw( float x, float y, const D3DXMATRIX &m );
#endif
	inline void Draw( const XE::VEC2& vPos ) { 
		Draw( vPos.x, vPos.y ); 
	}
	inline void Draw( const XE::VEC2& vPos, const MATRIX &m ) {
		Draw( vPos.x, vPos.y, m );
	}
	// action
	bool IsHaveAction( ID idAct );
	void ResetAction();
	void SetAction( DWORD id, xRPT_TYPE playType = xRPT_LOOP, BOOL bExecFrameMove=TRUE );
	const XActDat *GetAction() const;
	inline XActDat* GetActionMutable() {
		return const_cast<XActDat*>( GetAction() );
	}
	const XActDat *GetAction( ID idAct ) const;
	XActDat* GetActionMutable( ID idAct ) {
		return const_cast<XActDat*>( GetAction( idAct ) );
	}
	ID GetidActByRandom() const;
	ID GetActionID() const;
	XActObj *AddObjAct( int idx, const XActDat *pAction );
	// 애니메이션 최대 플레이 시간을 초단위로 얻는다.
	float GetPlayTime() const;
	// key
	void SetKeyCurrStart() {	m_nKeyCurr = 0; }			// 현재 가리키는 키를 맨 처음으로 돌린다.
	
	// layer
	XBaseLayer* GetpLayerByidLocalInLayer( XActObj* pActObj, ID idLocalInLayer );
	template<typename T>
	T* GetpLayerByidLocalInLayerT( XActObj* pActObj, ID idLocalInLayer ) {
		if( XASSERT(pActObj) )
			return SafeCast<T*>( pActObj->GetpLayerByidLocalInLayer( idLocalInLayer ) );
		return nullptr;
	}

	XBaseLayer *GetLayer( xSpr::xtLayer type, int nLayer );
	XLayerMove* GetpLayerMove( xSpr::xtLayer type, int nLayer );
	XLayerImage* GetpLayerImage( xSpr::xtLayer type, int nLayer );
	XLayerObject* GetpLayerObj( xSpr::xtLayer type, int nLayer );
	// etc
	DWORD GetPixel( float cx, float cy, float mx, float my, BYTE *pa=NULL, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL ) { 
		MATRIX m; 
		MatrixIdentity( m ); 
		return GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
	}
	DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa=NULL, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL );
	MATRIX* GetMatrix( MATRIX *pOut, float lx, float ly );
	BOOL IsDrawOutPartly( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyLeft( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyRight( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyBottom( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyTop( const XE::VEC2& vPos );
	void SetPlayTime( float secPlay );
	const XSprite* GetSprite( int idx ) const;
	XSprite* GetSpriteMutable( int idx );  // 이건 장차 없어져야한다.
	// file
	// preload sprobj
	XSprObj* AddSprObj( LPCTSTR szSprObj, ID idAct, xRPT_TYPE playMode, ID idBase, ID idLocal, ID idActParent, ID idLayerParent );
	// virtual 
#ifdef _SPR_USE_LUA
	virtual XLua* CreateScript();
	static void RegisterLua( XLua *pLua );
#endif
	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XDelegateSprObj *pDelegate=NULL );
	// lua
	void LuaSetAction( DWORD idAct, xRPT_TYPE playType ) { SetAction( idAct, playType );	}
	void LuaDraw( float x, float y ) { FrameMove( s_LuaDt ); Draw( x, y ); }
	void LuaSetRotate( float angle ) { SetRotate( angle ); }
	void LuaSetScale( float scale ) { SetScale( scale ); }
	void LuaSetScaleXY( float sx, float sy ) { SetScale( sx, sy ); }
	XSprObj* LuaAddSprObj( ID idBase, ID idLocal, const char *cSprObj, ID idAct, xRPT_TYPE playMode ) {
		XBREAKF( idAct == 0, "LuaAddSprObj: idAct가 지정되지 않았다" );
		return AddSprObj( Convert_char_To_TCHAR( cSprObj ), idAct, playMode, idBase, idLocal, 0, 0 );
	}
	BOOL RecreateLua() ;
//	friend class XKeyCreateObj;
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar );
#ifdef _XASYNC_SPR
	GET_SET_ACCESSOR_CONST( const struct tagAsync&, Async );
	inline bool IsAsyncLoading() const;
private:
//	BOOL Load( LPCTSTR szFilename, const XE::xHSL& hsl, bool bUseAtlas, BOOL bKeepSrc, bool bAsyncLoad );
	void OnFinishLoad( XSprDat* pSprDat );
//	void OnCompleteAsyncLoad( XSprDat* pSprDat );
	xSpr::XSPDat LoadInternal( LPCTSTR szFilename, const XE::xHSL& hsl, bool bUseAtlas, ID* pOutidAsync ) const;
	xSpr::XSPDat LoadInternal( const char* cFilename, const XE::xHSL& hsl, bool bUseAtlas, ID* pOutidAsync ) const;
#endif // _XASYNC_SPR
};

