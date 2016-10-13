#pragma once
#include "etc/xColor.h"
#include "Layer.h"
#include "Key.h"
#include "XE3d.h"
// #include "Sprite.h"

#include <list>

class XSprDat;
class XActObj;
class XBaseRes;

struct LAYER_INFO {
	ID idLayer;						// 레이어 아이디
	xSpr::xtLayer type;		//레이어타입
	int nLayer;						// 레이어번호
	float fAdjustAxisX, fAdjustAxisY;
};
class XActDat
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
	XE::VEC2 m_vScale;		// 이 액션의 전체 스케일링
	XE::VEC3 m_vRotate;		// 이 액션의 전체 회전
		
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
		m_vScale.Set( 1.f );
		InitBoundBox();
	}
	void Destroy( void );
	GET_ACCESSOR_CONST( const XSprDat*, pSprDat );
public:
	XActDat( XSprDat *pSprDat, DWORD id ) { Init(); m_ID = id; m_pSprDat = pSprDat; Create(); }
	XActDat( XSprDat *pSprDat, DWORD id, LPCTSTR szActName ) { Init(); m_ID = id; m_pSprDat = pSprDat; _tcscpy_s( m_szActName, szActName ); Create(); }
	~XActDat() { Destroy(); }
	
	GET_ACCESSOR_CONST( DWORD, ID );
	GET_SET_ACCESSOR_CONST( float, fSpeed );
	GET_SET_ACCESSOR_CONST( float, fMaxFrame );
	GET_ACCESSOR_CONST( int, nNumKeys );
	GET_ACCESSOR_CONST( int, nNumLayerInfo );
	LPCTSTR GetszActName( void ) const { return m_szActName; }
	GET_ACCESSOR_CONST( float, RepeatMark );
	GET_ACCESSOR_CONST( xRPT_TYPE, PlayMode );
	const XE::VEC2 GetBoundBoxLT( void ) const { 
		return m_vBoundBox[ 0 ];
	}
	const XE::VEC2 GetBoundBoxRB( void ) const { 
		return m_vBoundBox[ 1 ];
	}
	const XE::VEC2 GetBoundBoxSize() const {
		return m_vBoundBox[ 1 ] - m_vBoundBox[ 0 ];
	}
	void InitBoundBox( void ) {
		m_vBoundBox[0].Set( 999999.f, 999999.f );
		m_vBoundBox[1].Set( -999999.f, -999999.f );
	} 
	// 바운딩박스를 가지고 있는가? FALSE라면 bb값을 사용해선 안됨.
	bool IsHaveBoundBox( void ) const {
		return !( m_vBoundBox[0].x > 9999.f || m_vBoundBox[0].y > 9999.f ||
			m_vBoundBox[1].x < -9999.f || m_vBoundBox[1].y < -9999.f );
	}
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScale );
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vRotate );
//	XBaseKey_Itor GetKeyItorBegin( void ) { return m_KeyList.begin(); }	
	
	void Create( void );
	void Load( XSprDat *pSprDat, XBaseRes *pRes, int verSpr );

	// key
#ifdef _XDEBUG
	const XBaseKey *GetKey( int idx ) const;
#else
	const XBaseKey *GetKey( int idx ) const { 
		return m_ppKeys[ idx ]; 
	}
#endif 
	XBaseKey* GetKeyMutable( int idx ) {
		return const_cast<XBaseKey*>(GetKey( idx ));
	}
	XKeyPos *CreatePosKey( XBaseKey::xTYPE type );
	XKeyRot *CreateRotKey( XBaseKey::xTYPE type );
	XKeyScale *CreateScaleKey( XBaseKey::xTYPE type );
	XKeyEffect *CreateEffectKey( XBaseKey::xTYPE type );
	int AddKey( int idx, XBaseKey *pNewKey, float fFrame );
	void ExecuteKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame );
	void JumpKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame );
	//etc
	// layer info
#ifdef _XDEBUG
	const LAYER_INFO *GetLayer( int idx ) const;
#else
	const LAYER_INFO *GetLayer( int idx ) const {
		return m_ppLayerInfo[ idx ];
	}
#endif
	// 애니메이션 최대 플레이 시간을 초단위로 얻는다.
	float GetPlayTime( void ) const {
		return GetfMaxFrame() / GetfSpeed() / 60.f;
	}
private:
	LAYER_INFO *AddLayerInfo( int idx, xSpr::xtLayer type, int nLayer, ID idLayer );
};
