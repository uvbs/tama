#pragma once
#ifdef _SPR_USE_LUA
#undef  _SPR_USE_LUA
#endif

//#define MAX_ID			100			// 최대로 부여할수 있는 액션아이디 개수0~99번(defineAct.h)에서 옮겨옴. 툴에서는 defineAct.h를 컴파일 시키면 안되기 때문

#ifndef XFPS		// 프리컴파일헤더나 version.h등에서 XFPS가 정의되었다면 자체 디파인을 쓰지 않는다
#define XFPS		(60.0f)
#endif
#define XSPF		(1.0f/XFPS)		// sec per frame(프레임당 sec)

// play type
enum xRPT_TYPE : int {
	xRPT_LOOP=0,		// 루핑
	xRPT_1PLAY,			// 1번 플레이후 사라짐(레이어 클리어)
	xRPT_PINGPONG,		// 1회 왕복
	xRPT_PINGPONG_LOOP,	// 왕복을 반복
	xRPT_REVERSE,		// 뒤에서부터 거꾸로
	xRPT_1PLAY_CONT,	// 1번 플레이후 마지막 프레임에서 멈춰있음(더이상 사용하지 않음. 1PLAY로 하고 객체를 삭제하지만 않으면 같은효과)
	xRPT_DEFAULT,
};

namespace SPR
{
	// 점과 점사이를 잇는 라인타입
	enum xtLINE {
		xLINE_NONE,
		xLINE_LINE,
		xLINE_SPLINE_CATMULLROM,		// CatmullRom 스플라인
		xLINE_SPLINE_BEZIER,				// 베지어 스플라인
	};
}

namespace xSpr {
	enum xtLayer {
		xLT_NONE = 0
	, xLT_IMAGE
	, xLT_OBJ
	, xLT_SOUND
	, xLT_EVENT
	, xLT_DUMMY
	, xLT_MAX
	};
}