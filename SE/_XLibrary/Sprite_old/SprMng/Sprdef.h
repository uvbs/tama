#pragma once

class XSprObj;
#define MAX_ID			100			// 최대로 부여할수 있는 액션아이디 개수0~99번(defineAct.h)에서 옮겨옴. 툴에서는 defineAct.h를 컴파일 시키면 안되기 때문

#ifndef XFPS		// 프리컴파일헤더나 version.h등에서 XFPS가 정의되었다면 자체 디파인을 쓰지 않는다
#define XFPS		(60.0f)
#endif
#define XSPF		(1.0f/XFPS)		// sec per frame(프레임당 sec)

// play type
enum xRPT_TYPE {
	xRPT_LOOP=0,		// 루핑
	xRPT_1PLAY,			// 1번 플레이후 사라짐(레이어 클리어)
	xRPT_PINGPONG,		// 1회 왕복
	xRPT_PINGPONG_LOOP,	// 왕복을 반복xutilxuti
	xRPT_REVERSE,		// 뒤에서부터 거꾸로
	xRPT_1PLAY_CONT,	// 1번 플레이후 마지막 프레임에서 멈춰있음(더이상 사용하지 않음. 1PLAY로 하고 객체를 삭제하지만 않으면 같은효과)
	xRPT_DEFAULT,
};

typedef enum
{
	xVIEW_THUMB = 0,
	xVIEW_ADJUST,
	xVIEW_ANI,
	xVIEW_FRAME,
	xVIEW_MAX
} xVIEW_NUM;

namespace SPR
{
	// 점과 점사이를 잇는 라인타입
	enum xtLINE {
		xLINE_NONE,
		xLINE_LINE,
		xLINE_SPLINE_CATMULLROM,		// CatmullRom 스플라인
		xLINE_SPLINE_BEZIER,				// 베지어 스플라인
	};
	inline LPCTSTR GetStringLoopType( xRPT_TYPE typeLoop, BOOL bInfinite=FALSE ) {
		switch( typeLoop )
		{
		case xRPT_LOOP:	
			if( bInfinite )
				return _T("무한반복");
			else
				return _T("반복");
		case xRPT_1PLAY:	return _T("한번만");
		}
		return _T("");
	}
}

XE_NAMESPACE_START( xSpr )
struct USE_SPROBJ {
	TCHAR szFilename[ 128 ];
	XSprObj *pSprObj;
	DWORD dwID;
};
typedef enum xtEventKey	{
	xEKT_NONE = 0,
	xEKT_CREATE_OBJ,			// 오브젝트 생성 이벤트
	xEKT_HIT,					// 타격 이벤트
	xEKT_CREATE_SFX,			// 이펙트 생성
	xEKT_ETC = 999			// 기타 다용도 이벤트
};
typedef struct {
	union {
		struct {
			BYTE b[ 4 ];
		};
		struct {
			WORD w[ 2 ];
		};
		DWORD dwParam;
		int nParam;
		float fParam;
	};
} KEY_PARAM;
enum xtKey {
	xKT_NONE = 0,
	xKT_IMAGE,
	xKT_CREATEOBJ,
	xKT_SOUND,
	xKT_EVENT,
	xKT_DUMMY
};
enum xtKeySub { 
	xKTS_MAIN = 0, 
	xKTS_POS, 
	xKTS_EFFECT, 
	xKTS_ROT, 
	xKTS_SCALE, 
	xKTS_NONE = -1 
};
//
enum xtInterpolation { 
	xNONE = 0, 
	xLINEAR, 
	xACCEL, 
	xDEACCEL, 
	xSMOOTH, 
	xSPLINE 
};
enum xtLayer { 
	xLT_NONE = 0, 
	xLT_IMAGE,
	xLT_OBJECT, 
	xLT_SOUND, 
	xLT_EVENT, 
	xLT_DUMMY, 
	xLT_MAX 
};

enum xtLayerSub { 
	xLTS_NONE = 0, 
	xLTS_MAIN, 
	xLTS_POS, 
	xLTS_ROT, 
	xLTS_SCALE, 
	xLTS_EFFECT 
};

XE_NAMESPACE_END;

#define SET_KEY_PARAM_DWORD( V, IDX )       inline void Set##V( DWORD val )   { m_Param[IDX].dwParam = val; };
#define GET_KEY_PARAM_DWORD( V, IDX )       inline DWORD Get##V()           { return m_Param[IDX].dwParam; };
#define GET_SET_KEY_PARAM_DWORD( V, IDX )   SET_KEY_PARAM_DWORD( V, IDX ) GET_KEY_PARAM_DWORD( V, IDX )
//
#define SET_KEY_PARAM_INT( V, IDX )       inline void Set##V( int val )   { m_Param[IDX].nParam = val; };
#define GET_KEY_PARAM_INT( V, IDX )       inline int Get##V()           { return m_Param[IDX].nParam; };
#define GET_SET_KEY_PARAM_INT( V, IDX )   SET_KEY_PARAM_INT( V, IDX ) GET_KEY_PARAM_INT( V, IDX )
//
#define SET_KEY_PARAM_FLOAT( V, IDX )       inline void Set##V( float val )   { m_Param[IDX].fParam = val; };
#define GET_KEY_PARAM_FLOAT( V, IDX )       inline float Get##V()           { return m_Param[IDX].fParam; };
#define GET_SET_KEY_PARAM_FLOAT( V, IDX )   SET_KEY_PARAM_FLOAT( V, IDX ) GET_KEY_PARAM_FLOAT( V, IDX )
//
#define SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )       inline void Set##V( BOOL val )   { m_Param[IDX].b[IDXBYTE] = (BYTE)val; };
#define GET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )       inline BOOL Get##V()           { return (BOOL)(m_Param[IDX].b[IDXBYTE]); };
#define GET_SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )   SET_KEY_PARAM_BOOL( V, IDX, IDXBYTE ) GET_KEY_PARAM_BOOL( V, IDX, IDXBYTE )

