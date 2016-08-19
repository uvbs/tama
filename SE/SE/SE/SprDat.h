#pragma once

#include <stdio.h>
#include "xColor.h"
//#include "Sprite.h"
#include "ResMng.h"
#include <list>
#include <algorithm>					// find함수를 사용하기 위해
#include "xLayerInfo.h"

using namespace std;
class XSprDat;
class XSprObj;
//class XObjAct;
class XBaseUndo;
class CAnimationView;
class XSprite;
class XBaseKey;
class XBaseLayer;
struct LAYER_INFO;


typedef list<XSprite*>				XSprite_List;
typedef list<XSprite*>::iterator	XSprite_Itor;

struct xLayerInfoByAction {
	ID m_idAct = 0;
	std::vector<xLayerInfo> m_aryLayerInfo;
};

class XSprDat
{
	friend class XSprObj;
	friend class XActObj;
//	friend class XTool;
public:
	static char s_cGlobalLua[ 0x10000 ];			// 테스트용 루아 글루함수를 여기다 구현해서 구현한다
private:
	BOOL m_bHighReso;
	BOOL m_bKeepSrc;
//	ID m_idLayerGlobal;
	int m_nRefCnt;
	TCHAR m_szFilename[ 256 ];
	int m_indexFromID[ MAX_ID ];							// 실제 게임에서 로딩할때 액션아이디에 대응하는배열에 액션리스트의 인덱스가 들어있다
	int m_nVersion;
	XSprite_List m_listSprite;
	XSprite_Itor m_itorSprite;
	XList4<XSPAction>::iterator m_itorAction;
	XList4<XSPAction> m_listAction;		// 액션 객체 리스트
	int m_nRepeat;								// 1회플레이, 루핑, 왔다갔다 등
	XBaseRes::xtERROR	m_Error;				// 세이브/로드시에 에러가 났다면 여기에 에러값이 기록된다
	char *m_pcLuaAll;							// .spr파일 안에서 쓰는 모든 루아코드를 여기에 합친다
	void Init()	{
		m_bHighReso = TRUE;	// 일단은 트루. 툴내에 고해상도 체크기능이 생기면 펄스로 바꿈
		m_bKeepSrc = FALSE;
//		m_idLayerGlobal = 1;
		m_nRefCnt = 0;
		memset( m_szFilename, 0, sizeof(m_szFilename) );
		memset( m_indexFromID, 0, sizeof(m_indexFromID) );
		m_nVersion = 0;
//		m_fScaleFactor = 1.0f;
		m_nRepeat = 0;
		m_Error = XBaseRes::ERR_SUCCESS;
		m_pcLuaAll = NULL;
	}
	void Destroy();

	void AddAction( XSPAction spAction );
	XSPAction CreateAction( LPCTSTR szActName );
	void DelAction( ID idAct );
public:
	XSprDat() { Init(); }
	~XSprDat() { Destroy(); }

// 	ID GenerateIDLayer() { 
// 		return m_idLayerGlobal++; 
// 	}
//	GET_ACCESSOR( ID, idLayerGlobal );
	GET_ACCESSOR( LPCTSTR, szFilename );
	GET_ACCESSOR( XBaseRes::xtERROR, Error );
	BOOL IsUpperVersioin( int nVersion ) { return (m_nVersion >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersioin( int nVersion ) { return (m_nVersion < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 낮은가
	GET_SET_ACCESSOR( int, nVersion );
	GET_ACCESSOR( int, nRefCnt );
	GET_ACCESSOR( XList4<XSPAction>&, listAction );
	void AddRefCnt() { ++m_nRefCnt; }
	void DecRefCnt() { --m_nRefCnt; }
	void SetLua( const char *pSrc ) { 
		DestroyLua();
		int len = strlen(pSrc);
		if( m_pcLuaAll == NULL )
			m_pcLuaAll = new char[ len+1 ];
		strcpy_s( m_pcLuaAll, len+1, pSrc );
	}
	GET_ACCESSOR( const char*, pcLuaAll );
	GET_ACCESSOR( BOOL, bKeepSrc );
	//
	DWORD GenerateActID();		// 액션리스트를 검사해서 없는 번호를 리턴한다 최대 99까지다
	// file
	BOOL Save( LPCTSTR szFilename, const std::vector<xLayerInfoByAction>& aryLayerInfoByAction );
	BOOL Load( LPCTSTR szFilename, std::vector<xLayerInfoByAction> *pOutAryLayerInfo, BOOL bSrcKeep );
	BOOL ConvertLowReso();
	void AddSprite( XSprite *pSpr, BOOL bKeepPriority = FALSE );
	XSprite* AddSprite( float surfacew, float surfaceh, float adjustX, float adjustY, int nMemWidth, int nMemHeight, DWORD *pImg );
	inline XSprite* AddSprite( const XE::VEC2& sizeSurface
														, const XE::VEC2& vAdj
														, const XE::VEC2& sizeMem
														, DWORD *pImg ) {
		return AddSprite( sizeSurface.w, sizeSurface.h, vAdj.x, vAdj.y, (int)sizeMem.w, (int)sizeMem.h, pImg );
	}
	XSprite* DelSprite( XSprite *pSpr );
	XBaseKey *FindUseSprite( XSprite *pSpr );
	void SortSprite();		// nIdx값으로 소트한다
	XSprite* FindSprite( ID idSpr );
	// action
	XSPAction GetspActDatByIndex( int index );		// 이제 액션을 직접 건드리는건 금지된다. 반드시 XActObj를 통해서만 접근할것.
	XSPAction GetspAction( ID idAct );
	int GetNumActs() {
		return m_listAction.size();
	}
	XSprite *IsExistSpr( DWORD *pImg, int adjx, int adjy );		// pImg, adjx, adjy와 완전히 같은 스프라이트가 있는가
	XSprite *IsExistSprInfo( LPCTSTR szFilename, RECT *pRect = NULL );		// 주어진 인수와 같은 소스이미지를 가지는 스프라이트가 있는가. pRect가 NULL이면 rect정보는 비교하지 않음
	//key
//	XSprite* AddAniFrame( XSprObj *pSprObj, float surfacew, float surfaceh, float adjustX, float adjustY, int memw, int memh, DWORD *pImg, BOOL bCreateKey );
	// sprite
	int GetNumSprite() { return m_listSprite.size(); }
	void GetNextSprSet( int nSpr );
	void GetNextSprClear() { m_itorSprite = m_listSprite.begin(); }
	XSprite *GetNextSpr() {
		if( m_itorSprite == m_listSprite.end() )
			return NULL;
		return ( *m_itorSprite++ );
	}
	XSprite* GetSpriteIndex( int nSpr );
private:
	void GetNextActionClear() { m_itorAction = m_listAction.begin(); }
	XSPAction GetNextAction() {
		if( m_itorAction == m_listAction.end() )
			return XSPAction();
		return ( *m_itorAction++ );
	}
public:
//	XBaseKey* AddKey( SPAction spAction, SPLayerImage spLayer, XSprite *pSprite );
// 	int AddKey( SPAction spAction, SPLayerImage spLayer, float fFrame, XSprite *pSprite );
// 	int AddKey( SPAction spAction, SPLayerMove, float fFrame, float x, float y );
// 	inline int AddKey( SPAction spAction, SPLayerMove spLayer, float fFrame, const XE::VEC2& vPos ) {
// 		return AddKey( spAction, spLayer, fFrame, vPos.x, vPos.y );
// 	}
	void CompositLuaCodes( char *cDst, int bufflen );		// strDst에 이 파일내에서 쓰는 모든 루아코드를 합친다
//	void SetLayerInfo( ID idAct, const XList4<xLayerInfo>& listLayerInfo );
private:
	void DestroyLua() { SAFE_DELETE_ARRAY( m_pcLuaAll ); }
	void DrawPathLayer( XLayerMove *spLayer );
	int GetidxActionByidAct( ID idAct );
	void ReindexingSprite();
// 	XBaseKey* AddKeySprAtLast( XSPAction spAction, xSPLayerImage spLayer, XSprite *pSprite );
// 	int AddKeySprWithFrame( XSPAction spAction, xSPLayerImage spLayer, float fFrame, XSprite *pSprite );
// 	int AddKeyPosWithFrame( XSPAction spAction, XSPLayerMove spLayer, float fFrame, float x, float y );
// 	inline int AddKeyPosWithFrame( XSPAction spAction, XSPLayerMove spLayer, float fFrame, const XE::VEC2& vPos ) {
// 		return AddKeyPosWithFrame( spAction, spLayer, fFrame, vPos.x, vPos.y );
// 	}
};

//#define SPRDAT_LOOP( I )				\
//								list<USE_SPRDAT*>::iterator I; \
//								for( I = m_listSprDat.begin(); I != m_listSprDat.end(); I ++ ) 
		
//#define SPRDAT_MANUAL_LOOP( I )				\
//								list<USE_SPRDAT*>::iterator I; \
//								for( I = m_listSprDat.begin(); I != m_listSprDat.end(); ) 



typedef list<XSprDat *>			XSprDat_List;
typedef list<XSprDat *>::iterator	XSprDat_Itor;

