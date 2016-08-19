#pragma once

#include "XGraphicsD3DTool.h"
#include "XSprObjTool.h"
#include "Select.h"
#include "ToolEtc.h"
#include "ConstantDefine.h"
//#include "UndoMng.h"
#include "SprMng.h"
#include "SEFont.h"
#include "Global.h"
#include <stack>

class XBaseKey;
// edit mode
#define EM_MOVE		0
#define EM_ROTATE	1
#define EM_SCALE		2
// AXIS lock
#define AL_XY		0
#define AL_X			1
#define AL_Y			2


#define TOOL		GetTool()
#define SPROBJ		TOOL->GetSprObj()
#define SPRDAT		SPROBJ->GetpSprDat()
//#define SPROBJACT		SPROBJ->GetpObjActCurr()
#define VALID_ACT(A) \
		auto A = SPROBJ->GetspAction();		\
		if( !A )		return;

#define VALID_ACTOBJ(A) \
		auto A = SPROBJ->GetspActObjCurr();		\
		if( !A )		return;

// MIN값 단위로 반올림 조정
// ex) MIN:0.2 
// SRC=0.25 -> 0.2
// SRC=0.50 -> 0.6
#define ROUND_FLOAT(SRC, MIN)	( (int)(((SRC) + ((MIN)/2.0f)) / (MIN)) * (MIN) )

#define TRACK_X		100.0f				// 이름영역을 제외한 트랙영역의 시작x좌표
//#define TOOLBAR_HEIGHT		0.0f			// 툴바영역 높이
#define TOOLBAR_HEIGHT		32.0f			// 툴바영역 높이

class XViewport
{
public:
	XViewport( DWORD x, DWORD y, DWORD r, DWORD b );
	XViewport( DWORD x, DWORD y, const XE::VEC2& vSize )
		: XViewport( x, y, x + (DWORD)vSize.w, y + (DWORD)vSize.h ) {}
	XViewport( const XE::VEC2& vLT, const XE::VEC2& vSize )
		: XViewport( (DWORD)vLT.x, (DWORD)vLT.y, vSize ) {}
	~XViewport();
private:
	static _D3DVIEWPORT9 s_Viewport;	// 현재 뷰포트
	static std::stack<_D3DVIEWPORT9> s_stackViewport;	// 뷰포트 스택
private:
};

//////////////////////////////////////////////////////////////////////////
//#define TOOL_VER		_T("2014.1.20")		// 쉐이더수정됐으면 리소스에 새로 갱신해서 릴리즈 빌드해야 한다
/**
 @brief UI로부터 호출되어지는 최상위 함수군
 플랫폼별 OnClickXXX()류 함수에서 불려지는 최상위 함수들
 다른언어로 포팅할때 그 언어 플래폼에 의존적이지 않게 하기위해 만들어짐.
 ver30: 포지션키 랜덤요소 적용
*/
/*
ver26 : 각 키에 idLayer저장
ver32 : KeyEvent의 id값이 2바이트에서 4바이트로 변경
ver33 : 이벤트키 속성추가(문자열)
*/
class XTool
{
public:
	enum { SPR_VER = 33 };
private:
	XSprObjTool *m_pSprObj;
//	XSPAction m_spSelAct;			// 현재선택된 액션 fuck 이거 필요없을거 같은데
	XSPActObj m_spSelActObj;			// 현재선택된 액션 fuck 이거 필요없을거 같은데
	int m_nSelFrm;						// 현재선택된 프레임
	XSPBaseLayer m_spSelLayer;			// 선택된 레이어
	XSelect m_SelectKey;
	XSelect m_ShadowKey;
	XSelect m_SelSpr;
	XSelect m_SelToolTipKey;			// 툴팁표시를 위해 해당되는 모든키
	XSelect m_SelUndoKeys;			// 프레임뷰에서 키다중 이동시에 쓰는 언두들 리스트
	CString m_strFilename;				// 저장한 파일명
	CString m_strPath;					// 오픈한 파일이 있는 패스
	CString m_strWorkPath;			// 기본 워킹폴더
	CString m_strCmdLine;				// 커맨드 라인으로 들어온 파일명
	int m_nEditMode;				// 이동/회전/확축모드
	int m_AxisLock;					// 이동시 좌표축 고정여부
	CDefine 	*m_pDefineAct;
	int m_idxBaseSpr;		// AdjustView에서 사용하는 기준스프라이트 인덱스번호
	BOOL m_bSlowDrag;		// 드래그 속도 감속모드
	BOOL m_bFrameSkip;		// 프레임 스키핑
	XSurface *m_psfcBg = nullptr;
	_tstring m_pathBg;
public:
	void Init()	{
		m_pSprObj = NULL;
		m_strFilename = "";
		m_nEditMode = 0;
		m_AxisLock = AL_XY;
		m_idxBaseSpr = -1;
		m_bSlowDrag = FALSE;
#ifdef _DEBUG
		m_bFrameSkip = FALSE;
#else
		m_bFrameSkip = TRUE;
#endif
	}
	void Destroy();
	XTool();
	~XTool() { 
		Destroy(); 
		SAFE_DELETE( m_pDefineAct );
		SAFE_DELETE( SE::g_pFont );
	}
	//
	XSprObjTool *GetSprObj() { return m_pSprObj; }
	void SetSelAct( DWORD id );
	void SetSelActIdx( int index );
	XSPAction DelAct( ID idAct = 0 );
//	void AllViewUpdate();
	XSPActObj GetSelActObj() const { return m_spSelActObj; }
	XSPActionConst GetSelAct() const;
	void SetSelFrm( int nFrm ) { m_nSelFrm = nFrm; }
	int GetSelFrm() { return m_nSelFrm; }
	GET_ACCESSOR( XSPBaseLayer, spSelLayer );
	void SetspSelLayer( XSPBaseLayer spLayer );
	GET_SET_ACCESSOR( const CString&, strFilename );
	GET_SET_ACCESSOR( const CString&, strPath );
	GET_SET_ACCESSOR( const CString&, strCmdLine );
	GET_ACCESSOR( const CString&, strWorkPath );
	GET_SET_ACCESSOR( int, nEditMode );
	GET_SET_ACCESSOR( int, AxisLock );
	GET_ACCESSOR( XSelect&, SelToolTipKey );
	GET_ACCESSOR( XSelect&, SelectKey );
	GET_ACCESSOR( XSelect&, SelUndoKeys );
	GET_ACCESSOR( XSelect&, SelSpr );
	GET_ACCESSOR( XSelect&, ShadowKey );
	GET_ACCESSOR( CDefine*, pDefineAct );
	GET_SET_ACCESSOR( int, idxBaseSpr );
	GET_SET_ACCESSOR( BOOL, bSlowDrag );
	GET_SET_ACCESSOR( BOOL, bFrameSkip );
	GET_ACCESSOR( XSurface*, psfcBg );
	GET_ACCESSOR( const _tstring&, pathBg );
	//
	void Create( BOOL bCreateDat = TRUE );
	BOOL LoadSpr( LPCTSTR szFullPath );
//	void AddAction( XObjAct *pObjAct );
	void AddAction( XSPAction spAction );
	XSPActObj CreateAction( LPCTSTR szName );
	void CopyAct( XSPActionConst spSrc );
	BOOL AddAniFrame( int nFileIdx, LPCTSTR szFilename, BOOL bCreateKey );
	BOOL AddAniFrame( int idx, int w, int h, DWORD *pImage, LPCTSTR szFilename, BOOL bCreateKey );
	BOOL AddKeyFromSelSpr( xSPLayerImage spLayer );
	void CutAndAdjustImage( int *pWidth, int *pHeight, int *pAdjustX, int *pAdjustY, DWORD **ppImage );
	void CutContinuosImage( LPCTSTR szFilename );
	BOOL CutAlignedImage( LPCTSTR szFilename );
// 	void GetSelectKey().Add( XBaseKey *pKey ) { m_SelectKey.Add( pKey ); }
// 	void GetSelectKey().Del( XBaseKey *pKey ) { m_SelectKey.Del( pKey ); }
// 	BOOL GetSelectKey().Find( XBaseKey *pKey ) { return m_SelectKey.Find( pKey ); }
// 	void GetSelectKey().GetNextClear() { m_SelectKey.GetNextClear(); }
// 	XBaseKey *GetNextSelectKey() { return (XBaseKey *)m_SelectKey.GetNext(); }
// 	int GetSelectKey().GetNum() { return m_SelectKey.GetNum(); }
// 	void GetSelectKey().Clear() { m_SelectKey.Clear(); }
// 	XBaseKey* GetLastSelectKey() { return (XBaseKey *)m_SelectKey.GetLast(); }				// 가장마지막으로 선택된 키를 돌려준다 
	// shadow
// 	void AddShadowKey( XBaseKey *pKey ) { m_ShadowKey.Add( pKey ); }
	void CreateShadowKey();		
	void ApplyShadowKey();
// 	void DestroyShadowKey() { m_ShadowKey.Destroy(); }
// 	void GetShadowKey().Clear() { m_ShadowKey.GetNextClear(); }
// 	XBaseKey *GetNextShadowKey() { return (XBaseKey *)m_ShadowKey.GetNext(); }
// 	int GetNumShadowKey() { return m_ShadowKey.GetNum(); }
	// sprite
// 	void GetSelSpr().Add( XSprite *pSpr ) { m_SelSpr.Add( pSpr ); }
// 	void GetSelSpr().Del( XSprite *pSpr ) { m_SelSpr.Del( pSpr ); }
// 	BOOL GetSelSpr().Find( XSprite *pSpr ) { return m_SelSpr.Find( pSpr ); }
// 	void GetNextSelSprClear() { m_SelSpr.GetNextClear(); }
// 	XSprite *GetNextSelSpr() { return (XSprite *)m_SelSpr.GetNext(); }
// 	int GetSelSpr().GetNum() { return m_SelSpr.GetNum(); }
// 	void ClearSelSpr() { m_SelSpr.Clear(); }
// 	XSprite* GetLastSelSpr() { return (XSprite *)m_SelSpr.GetLast(); }				// 가장마지막으로 선택된 키를 돌려준다 
	// tool
	template<class T>
	void AdjustAxisLock( T *x, T *y )
	{
		if( GetAxisLock() == AL_X )		// x축으로만 이동하는 모드
			*y = 0;
		else if( GetAxisLock() == AL_Y )
			*x = 0;
	}

	void LoadINI();
	void SaveINI( LPCTSTR szFilename=NULL );
	XDragDrop m_DragDrop;
	bool LoadBg( LPCTSTR szFullpath );
	XSPLayerMove GetspSelLayerMove();
	xSPLayerImage GetspSelLayerImage();
	XSPLayerObject GetspSelLayerObject();
	XSPBaseLayer AddLayer( xSpr::xtLayer type );
	//xSPLayerImage AddLayerImage();
	//XSPLayerObject AddLayerObject();
	//XSPLayerSound AddLayerSound();
	//XSPLayerDummy AddLayerDummy();
	//XSPLayerEvent AddLayerEvent();
	XSPBaseLayer AddLayerFromCopy( XSPBaseLayer spLayerSrc );
//	void SetKeyCursor( const XE::POINT& vDir );
private:
}; // class XTool

typedef list<RECT>				XRect_List;
typedef list<RECT>::iterator	XRect_Itor;

XTool* GetTool();



