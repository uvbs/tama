#pragma once
/*!
 * \class XSprObj
 *
 * \brief 스프라이트 인스턴스 객체
 *
 * \author xuzhu
 * \date 8월 2015
 * \note 이 클래스는 SE툴 전용이며 sprobj인스턴스가 한개이상 생성되지 않는것을 기반하고 있다.
 */
#include "Sprdef.h"
// 이 액션에서 로드될 XSprDat들은 미리 모두 로드된다
class XBaseKey;
class XBaseLayer;
class XSprDat;
class XLuaSprObj;
class XSprObj;
class XActObj;
class XSprite;

class XSprObj
{
//friend class XTool;	// 이렇게하고 왠만한 멤버는 모두 private으로 만든다.sprobj의 변형은 항상 tool을 거치도록 하기 위함
private:
	DWORD m_dwID;												// 이 오브젝트만의고유한 아이디
	XSprDat *m_pSprDat;
	std::list<xSpr::USE_SPROBJ *> m_listSprObj;					// 사전에 로드되는 XSprObj들의 리스트
	float m_fFrameCurrent = 0;						// 현재 프레임번호
	BOOL m_bPause;
	XSprObj *m_pParent;						// object layer로 생선된 XSprObj의 경우 부모가 있다
	XBaseKey *m_pParentKey;						// 이 오브젝트를 생성하게한 키가 있다면 저장. 없으면 널
	XBaseKey_Itor m_itorKeyCurr;			// 현재 가리키고 있는 노드의 iterator
//	XObjAct *m_pObjActCurr;				// 현재 선택된 액션
//	std::list<XObjAct *> m_listObjAct;			// sprdat의 액션수만큼 똑같이 생성시켜 각각 레이어를 가지고 있는다
//	XSPAction m_spActionCurr;			// 현재 선택된 액션
	XSPActObj m_spActObjCurr;			// 현재 선택된 액션
	xRPT_TYPE m_PlayType;
	BOOL m_bFinish;					// 애니메이션이 끝났는가
	// transform
	XE::VEC2 m_vPos;							// 부모로부터의 로컬좌표
	float	m_fAdjustAxisX, m_fAdjustAxisY;	// 회전축보정
	float	m_fScaleX, m_fScaleY;
	float	m_fRotX, m_fRotY, m_fRotZ;		// angle단위
	D3DXMATRIX m_mWorld;
	float	m_fAlpha;
	xDM_TYPE m_DrawMode;
	DWORD m_dwDrawFlag;		// EFF_****
	XList4<XSPActObj> m_listActObj;
	void Init() {
//		m_dwGlobalID = 0;
		m_dwID = 0;
		m_pSprDat = NULL;
//		m_pObjActCurr = NULL;
		m_bPause = FALSE;
		m_pParent = NULL;
		m_pParentKey = NULL;
		m_PlayType = xRPT_LOOP;
		m_bFinish = FALSE;
		// transform
		m_fScaleX = m_fScaleY = 1.0f;
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		m_fAlpha = 1.0f;
		m_DrawMode = xDM_NORMAL;
		m_dwDrawFlag = 0;
		//
	}
	void Destroy();
//	SET_ACCESSOR( XObjAct*, pObjActCurr );
public:
	XSprObj() { Init(); }
	XSprObj( DWORD dwID );
	XSprObj( const char *cSpr );
	virtual ~XSprObj() { Destroy(); }

	// get/set
	GET_ACCESSOR( XSprDat*, pSprDat );
	GET_ACCESSOR( DWORD, dwID );
	ID GetidSprObj() { return m_dwID; }
//	GET_ACCESSOR( XObjAct*, pObjActCurr );
	GET_ACCESSOR_CONST( const XList4<XSPActObj>&, listActObj );
//	GET_ACCESSOR( XSPAction, spActionCurr );
	GET_ACCESSOR_CONST( std::shared_ptr<XActObj>, spActObjCurr );
	GET_SET_ACCESSOR( XBaseKey*, pParentKey );
	float GetSpeedCurrentAction() const;
	void SetSpeedCurrentAction( float speed );
	float GetFrmCurr() const { return m_fFrameCurrent; }
	void SetFrmCurr( float fFrame ) { m_fFrameCurrent = fFrame; }
	BOOL GetPause() const { return m_bPause; }
	void SetPause( BOOL bPause ) { m_bPause = bPause;  }
	BOOL IsPause() const { return m_bPause; }
	BOOL IsPlaying() const { return !m_bPause; }
	GET_SET_ACCESSOR_CONST( xRPT_TYPE, PlayType );
	GET_ACCESSOR_CONST( BOOL, bFinish );
	GET_SET_ACCESSOR( XSprObj*, pParent );
	BOOL IsEndFrame() const;
	// get/set transform
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vPos );
	void SetScale( float fx, float fy ) { m_fScaleX = fx; m_fScaleY = fy; }
	void SetScale( const XE::VEC2& vScale ) { m_fScaleX = vScale.x; m_fScaleY = vScale.y; }
	SET_ACCESSOR( float, fAlpha );
	void SetRotateX( float angle ) { m_fRotX = angle; }
	void SetRotateY( float angle ) { m_fRotY = angle; }
	void SetRotateZ( float angle ) { m_fRotZ = angle; }
	void SetRotate( float ax, float ay, float az ) { m_fRotX = ax; m_fRotY = ay; m_fRotZ = az; }
	void SetRotate( const XE::VEC3& v3 ) { m_fRotX = v3.x; m_fRotY = v3.y; m_fRotZ = v3.z; }
	GET_ACCESSOR( float, fRotZ );
	void SetAdjustAxis( float adjx, float adjy ) { 
		m_fAdjustAxisX = adjx; 
		m_fAdjustAxisY = adjy; 
	}
	inline void SetAdjustAxis( const XE::VEC2& vAdjAxis ) { 
		SetAdjustAxis( vAdjAxis.x, vAdjAxis.y );
	}
	GET_ACCESSOR_CONST( float, fAdjustAxisX );
	GET_ACCESSOR_CONST( float, fAdjustAxisY );
	SET_ACCESSOR( xDM_TYPE, DrawMode );
	void SetFlipHoriz( BOOL bFlag ) { 
		( bFlag ) ? m_dwDrawFlag |= EFF_FLIP_HORIZ : m_dwDrawFlag &= ~EFF_FLIP_HORIZ; 
	}
	void SetFlipVert( BOOL bFlag ) { 
		( bFlag ) ? m_dwDrawFlag |= EFF_FLIP_VERT : m_dwDrawFlag &= ~EFF_FLIP_VERT; 
	}
	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }
	//
	// frame
	void NewSprDat();
	void FrameMove( float dt );
	void MoveFrame( float fFrame = -1.0f );
	void MoveFrameBySec( float sec );
//	void Reset();
	void Draw(){ 
		D3DXMATRIX m; 
		D3DXMatrixIdentity( &m ); 
		Draw( m ); 
	} 
	void Draw( const D3DXMATRIX &m );
	// action
	void ResetAction( XSPActObj spActObj );
	bool SetAction( DWORD id, xRPT_TYPE playType = xRPT_LOOP );
//	SPAction GetspAction();
	XSPAction GetspAction( ID idAct = 0 ) const;
	ID GetActionID() const;
//	void AddAction( SPAction spAction );
	XSPActObj CreateAction( LPCTSTR szActName );
	XSPActObj AddAction( XSPAction spAction );
		//	void CreateAction( SPAction spAction );
// 	XObjAct* DelAction( DWORD id );
// 	XObjAct *AddObjAct( SPAction spAction );
// 	void AddObjAct( XObjAct *pNewObjAct );
	void DelAction( ID idAct );
//	void AddAction( SPAction spAction );
	// key
//	void SetKeyCurrStart();
// 	XBaseKey* AddKey( SPAction spAction, SPBaseLayer spLayer, XSprite *pSprite );
// 	int AddKey( SPAction spAction, SPBaseLayer spLayer, float fFrame, XSprite *pSprite );
// 	int AddKey( SPAction spAction, SPBaseLayer spLayer, float fFrame, float x, float y );
// 	inline int AddKey( SPAction spAction, SPBaseLayer spLayer, float fFrame, const XE::VEC2& vPos ) {
// 		return AddKey( spAction, spLayer, fFrame, vPos.x, vPos.y );
// 	}
//	XKeyCreateObj *AddKeyCreateObj( LPCTSTR szSprObj, ID idAct, xRPT_TYPE playType, SPBaseLayer spLayer, float fFrame, float x, float y );
	void DelKey( XSPActionConst spAction, XBaseKey *pDelKey );
	void JumpKeyPos( XSPAction spAction, float fJumpFrame );		// fJumpFrame위치로 바로 점프한다. 그사이의 키는 실행하지 않는다

	// layer
	template<typename T>
	std::shared_ptr<T> GetspLayerByidLayer( ID idLayer ) const;
	XSPBaseLayer GetLayer( xSpr::xtLayer type, int nLayer );
	XSPBaseLayer GetspLayerCurrAct( ID idLayer ) const;
	XSPBaseLayer GetspLayer( XSPActionConst spAction, ID idLayer ) const;
	XSPBaseLayer GetspLayer( ID idAct, ID idLayer ) const;
	XSPBaseLayer GetspLayer( XBaseKey* pKey ) const;
	//	XSPLayerMove GetspLayerMove( xSpr::xtLayer type, int nLayer );
	void DelLayer( XSPBaseLayer spLayer, BOOL bDelLayerOnly = FALSE );
	void UpdateLayersYPos( const XE::VEC2& sizeKey );
//	void DrawLayerLabel( float left, float top );
	bool SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2, const XE::VEC2& sizeKey );
	// etc
//	DWORD GenerateID() { return m_dwGlobalID++; }
	DWORD GenerateID() { return xRand(); }
	DWORD GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const;
	XSPBaseLayer GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv ) const;
	XSPBaseLayer GetLayerInPixel( float cx, float cy, float mx, float my ) const { 
		D3DXMATRIX m; 
		D3DXMatrixIdentity( &m ); 
		return GetLayerInPixel( cx, cy, mx, my, m ); 
	}
	XSPBaseLayer GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m ) const;
	D3DXMATRIX* GetMatrix( D3DXMATRIX *pOut, float lx, float ly ) const;
	const D3DXMATRIX& GetMatrix() const { return m_mWorld; }
	void UpdateMatrix();
	// file
	BOOL Save( LPCTSTR szFilename );
	BOOL Load( LPCTSTR szFilename );
	// preload sprobj
	void DestroySprObj( DWORD dwID );
	XSprObj* AddSprObj( LPCTSTR szSprObj/*, ID idAct*/, xRPT_TYPE playMode, ID idBase, ID idLocal, ID idActParent, ID idLayerParent );	// idSprObj = idBase + idLocal
	// virtual 
	// factory
//	virtual XObjAct* CreateObjAct( SPAction spAction );
	// virtual
	virtual XLuaSprObj* CreateScript();
	// lua
//	void LuaUpdate( float x, float y ) { FrameMove( s_LuaDt ); Draw( x, y ); }
	void LuaSetRotate( float angle ) { SetRotate( 0, 0, angle ); }
	void LuaSetScale( float scale ) { SetScale( scale, scale ); }
	void LuaSetScaleXY( float sx, float sy ) { SetScale( sx, sy ); }
	XSprObj* LuaAddSprObj( ID idBase, ID idLocal, const char *cSprObj, ID idAct, xRPT_TYPE playMode, ID idActParent, ID idLayerParent ) {		// 
//		XBREAK( idAct == 0 );		// 내부에선 idAct를 0으로 넘겨줘도 되는데 루아에선 0이면 안됨
		if( idAct == 0 ) {
			XLOG( "idAct == 0" );
		}
		return AddSprObj( Convert_char_To_TCHAR( cSprObj )/*, idAct*/, playMode, idBase, idLocal, idActParent, idLayerParent );
	}
	D3DXMATRIX GetLocalMatrix() const;
	XBaseKey_Itor ExecuteKeyByFrame( XSPActObj spActObj, const XBaseKey_Itor& itorStart, float fFrameCurr );
	XBaseKey* AddKeySprAtLast( ID idAct, ID idLayer, XSprite *pSprite );
	int AddKeySprWithFrame( ID idAct, ID idLayer, float fFrame, XSprite *pSprite );
	int AddKeyPosWithFrame( ID idAct, ID idLayer, float fFrame, float x, float y );
	inline int AddKeyPosWithFrame( ID idAct, ID idLayer, float fFrame, const XE::VEC2& vPos ) {
		return AddKeyPosWithFrame( idAct, idLayer, fFrame, vPos.x, vPos.y );
	}
	XSprite* AddAniFrame( const XE::VEC2& sizeSurface
											, const XE::VEC2& vAdj
											, const XE::VEC2& sizeMem
											, DWORD *pImg
											, BOOL bCreateKey );
	ID GetidAct() const;
	XSPActObj GetspActObjByIndex(int index);
	XSPActionConst GetspActionCurr() const;
	void TraceActObj();
	XSPActObj CreateAddCopyAction( XSPActionConst pSrcAction/*, LPCTSTR szActName*/ );
	XSPActObj GetspActObj( ID idAct ) const;
	XSprObj* GetpSprObjChild( ID idSprObj ) const;
private:
	void CalcBoundBox();
};

#include "SprObj.inl"