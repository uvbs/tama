/*
 *  SprDat.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#ifndef __SPRDAT_H__
#define __SPRDAT_H__
//#include <OpenGLES/ES1/gl.h>
//#include <OpenGLES/ES1/glext.h>
//#include "XGraphics.h"
#include "xColor.h"
#include "ResMng.h"
#include "Layer.h"
#include "Key.h"
#include "Sprite.h"

#include <list>
using namespace std;

class XSprDat;
class XObjAct;

typedef struct 
{
	ID idLayer;						// 레이어 아이디
	XBaseLayer::xTYPE type;		//레이어타입
	int nLayer;						// 레이어번호
	float fAdjustAxisX, fAdjustAxisY;
} LAYER_INFO;
class XAniAction
{
	XSprDat *m_pSprDat;
	DWORD m_ID;						// 이 액션의고유번호(걷기번호 뛰기번호등이 들어간다)
	TCHAR m_szActName[128];
	int m_nNumKeys;
	XBaseKey **m_ppKeys;
	int m_nNumLayerInfo;
	LAYER_INFO **m_ppLayerInfo;		// 가지고 있는 레이어정보. 타입만 가지고 있고 실제 레이어는 XSprObj가 가지고 있다

	float m_fMaxFrame;
	float m_fSpeed;								// 1/60초당 몇프레임이 넘어가는가? 이 SprDat의 디폴드 속도. 같은것이 XSprObj에도 들어갈것이다. 왜냐하면 게임내에서도 자유롭게 속도를 조절할수 있어야 하니까
	xRPT_TYPE m_PlayMode;			// 루핑/한번만플레이 등
	float m_RepeatMark;				// 도돌이표 위치
	XE::VEC2 m_vBoundBox[2];			// 바운딩박스. LeftTop, RightBottom
		
	void Init( void ) {
		memset( m_szActName, 0, sizeof(m_szActName) );
		m_fSpeed = 0.2f;
		m_fMaxFrame = 0;
		m_ID = 0;
		m_ppKeys = NULL;
		m_ppLayerInfo = NULL;
		m_nNumLayerInfo = 0;
		m_nNumKeys = 0;
		m_pSprDat = NULL;
		m_PlayMode = xRPT_LOOP;
		m_RepeatMark = 0;
		InitBoundBox();
	}
	void Destroy( void );
	GET_SET_ACCESSOR( XSprDat*, pSprDat );
public:
	XAniAction( XSprDat *pSprDat, DWORD id ) { Init(); m_ID = id; m_pSprDat = pSprDat; Create(); }
	XAniAction( XSprDat *pSprDat, DWORD id, LPCTSTR szActName ) { Init(); m_ID = id; m_pSprDat = pSprDat; _tcscpy_s( m_szActName, szActName ); Create(); }
	~XAniAction() { Destroy(); }
	
	GET_ACCESSOR( DWORD, ID );
	GET_SET_ACCESSOR( float, fSpeed );
	GET_SET_ACCESSOR( float, fMaxFrame );
	GET_ACCESSOR( int, nNumKeys );
	GET_ACCESSOR( int, nNumLayerInfo );
	LPCTSTR GetszActName( void ) { return m_szActName; }
	GET_ACCESSOR( float, RepeatMark );
	GET_ACCESSOR( xRPT_TYPE, PlayMode );
	const XE::VEC2& GetBoundBoxLT( void ) { return m_vBoundBox[0]; }
	const XE::VEC2& GetBoundBoxRB( void ) { return m_vBoundBox[1]; }
	void InitBoundBox( void ) {
		m_vBoundBox[0].Set( 999999.f, 999999.f );
		m_vBoundBox[1].Set( -999999.f, -999999.f );
	} 
	// 바운딩박스를 가지고 있는가? FALSE라면 bb값을 사용해선 안됨.
	BOOL IsHaveBoundBox( void ) {
		if( m_vBoundBox[0].x > 9999.f || m_vBoundBox[0].y > 9999.f ||
			m_vBoundBox[1].x < -9999.f || m_vBoundBox[1].y < -9999.f )
			return FALSE;
		return TRUE;
	}
//	XBaseKey_Itor GetKeyItorBegin( void ) { return m_KeyList.begin(); }	
	
	void Create( void );
	void Load( XSprDat *pSprDat, XResFile *pRes );
	
//	void KeyRemapLayer( XObjAct *pObjAct );
	// key
#ifdef _XDEBUG
	XBaseKey *GetKey( int idx );
#else
	XBaseKey *GetKey( int idx ) { 
		return m_ppKeys[ idx ]; 
	}
#endif 
	XKeyPos *CreatePosKey( XBaseKey::xTYPE type );
	XKeyRot *CreateRotKey( XBaseKey::xTYPE type );
	XKeyScale *CreateScaleKey( XBaseKey::xTYPE type );
	XKeyEffect *CreateEffectKey( XBaseKey::xTYPE type );
	int AddKey( int idx, XBaseKey *pNewKey, float fFrame );
//	XKeyCreateObj *AddKeyCreateObj( int idx, DWORD id, LPCTSTR szSprObj, DWORD idAct, xRPT_TYPE playType, XBaseLayer::xTYPE type, int nLayer, float fFrame, float x, float y );
//	XKeyPos *AddKeyPos( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, BOOL bInterpolation, float x, float y );
//	XKeyRot *AddKeyRot( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, BOOL bInterpolation, float az );
//	XKeyScale *AddKeyScale( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, BOOL bInterpolation, float sx, float sy );
//	XKeyEffect *AddKeyEffect( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, BOOL bInterpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity );
//	XKeyEvent *AddKeyEvent( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, KE::xTYPE Event, float x, float y );
//	XKeySound *AddKeySound( int idx, float fFrame, XBaseLayer::xTYPE type, int nLayer, ID idSound );
	void ExecuteKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame );
	void JumpKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame );
	//etc
	// layer info
#ifdef _XDEBUG
	LAYER_INFO *GetLayer( int idx );
#else
	LAYER_INFO *GetLayer( int idx ) {
		return m_ppLayerInfo[ idx ];
	}
#endif
	LAYER_INFO *AddLayerInfo( int idx, XBaseLayer::xTYPE type, int nLayer, ID idLayer );
};

#define MAX_ID			100

class XSprDat
{
public:
	enum { SPR_VER = 22 };
private:
	int m_nRefCnt;
	BOOL m_bHighReso;			// 고해상도 스프라이트(아이폰 전용)
	BOOL m_bKeepSrc;				// 이미지데이타를 메모리에 보존
	TCHAR m_szFilename[ 256 ];
	int m_indexFromID[ MAX_ID ];							// 실제 게임에서 로딩할때 액션아이디에 대응하는배열에 액션리스트의 인덱스가 들어있다
	int m_nVersion;
	int m_nNumSprites;
	XSprite **m_ppSprites;						
	int m_nNumActions;
	XAniAction **m_ppActions;					
	float m_fScaleFactor;						// 고해상도 이미지가 들어있으면 이것이 2가 된다.
	int m_nRepeat;								// 1회플레이, 루핑, 왔다갔다 등
	char *m_pcLuaAll;							// .spr파일 안에서 쓰는 모든 루아코드를 여기에 합친다
	
	void Init( void ) {
		m_nRefCnt = 0;
		m_bHighReso = TRUE;
		m_bKeepSrc = FALSE;
		m_nVersion = 0;
		memset( m_indexFromID, -1, sizeof(m_indexFromID) );
		m_fScaleFactor = 1.0f;
		m_nRepeat = 0;
		memset( m_szFilename, 0, sizeof(m_szFilename) );
		m_ppSprites = NULL;
		m_ppActions = NULL;
		m_nNumActions = m_nNumSprites = 0;
		m_pcLuaAll = NULL;
	}
	void Destroy( void );
	
	void AddAction( int idx, XAniAction *pAction );
public:
	XSprDat() { Init(); }
	~XSprDat() { Destroy(); }
	
	GET_ACCESSOR( BOOL, bHighReso );
	GET_ACCESSOR( LPCTSTR, szFilename );
	GET_ACCESSOR( int, nNumSprites );
	GET_ACCESSOR( int, nNumActions );
	GET_ACCESSOR( int, nRefCnt );
	GET_ACCESSOR( const char*, pcLuaAll );
	GET_ACCESSOR( BOOL, bKeepSrc );
	BOOL IsUpperVersion( int nVersion ) { return (m_nVersion >= nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 같거나 높은가
	BOOL IsLowerVersion( int nVersion ) { return (m_nVersion < nVersion)? TRUE : FALSE; }	// 이 파일이 nVersion보다 버전이 낮은가
	
	void AddRefCnt( void ) { ++m_nRefCnt; }
	void DecRefCnt( void ) { --m_nRefCnt; }
	// file
	BOOL Load( LPCTSTR szFilename, BOOL bSrcKeep=FALSE );
	//key
	
	// action
	XAniAction* GetActionIndex( int index ) { return m_ppActions[ index ]; }
	int GetActionIndexFromID( DWORD id ) { return m_indexFromID[ id ]; }
	XAniAction* GetAction( DWORD id ) {
		int idx = m_indexFromID[id];
#ifdef _XDEBUG
		if( XBREAKF( idx == -1, "%s에 %d ID의 액션이 없다", m_szFilename, id ) ) 
			idx = 0;		// 다운은 안되도록
#endif
		return m_ppActions[ idx ]; 
	}
	
	// sprite
	XSprite* GetSprite( int nSpr ) { 
		if( XBREAK( nSpr >= m_nNumSprites || nSpr < 0 ) ) {
			return NULL;
		}
		XBREAK( m_ppSprites[ nSpr ] == NULL );
		return m_ppSprites[ nSpr ]; 
	}
	void AddSprite( int idx, XSprite *pSpr );
	XSprite* AddSprite( int nWidth, int nHeight, int nAdjustX, int nAdjustY, DWORD *pImg );
	
};

typedef list<XSprDat *>				XSprDat_List;
typedef list<XSprDat *>::iterator	XSprDat_Itor;
void SetDirSpr( LPCTSTR szDir );
#endif