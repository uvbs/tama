#pragma once
/*
 *  defineAct.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 22..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "sprite/sprDef.h"
//#define MAX_ID			100			// 최대로 부여할수 있는 액션아이디 개수0~99번
// 고정 액션 아이디 // MAX_ID - 1 부터 차례로 내려오며 부여한다	
#define ACT_IDLE1		99			// 대기1
#define ACT_RUN			98			// 달리기
#define ACT_ATTACK1		97			// 공격1
#define ACT_ATTACK2		96			// 공격2
#define ACT_ATTACK3		95			// 공격3
#define ACT_SKILL1		94
//#define ACT_SKILL2		93
//#define ACT_SKILL3		92
#define ACT_DIE			91				// 죽음
#define ACT_STUN		90			// 스턴
// 유동 액션아이디(1~)					// 툴에서 자동으로 매겨지는 아이디번호는 1부터 시작한다

// 이벤트 아이디
#define xSPREVT_HIT		1	///< 근접타격
#define xSPREVT_SHOOT	2	///< 원거리 발사
#define xSPREVT_SKILL	3	///< 스킬
