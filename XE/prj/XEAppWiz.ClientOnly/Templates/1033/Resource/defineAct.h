#ifndef __DEFINEACT_H__
#define __DEFINEACT_H__
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
#define ACT_IDLE1			99			// 대기1
#define ACT_IDLE2			98			// 대기2
#define ACT_WALK			97			// 걷기
#define ACT_MOVE			97			// 이동(걷기와 같은것임)
#define ACT_RUN			96				// 뛰기
#define ACT_ATTACK1		95			// 공격1
#define ACT_ATTACK2		94			// 공격2
#define ACT_ATTACK3		93			// 공격3
#define ACT_ATTACK4		92			// 공격4
#define ACT_DAMAGE		91			// 피격
#define ACT_DIE			90				// 죽음
#define ACT_WIN			89			// 승리
#define ACT_LOSE			88			// 패배
// 유동 액션아이디(1~)					// 툴에서 자동으로 매겨지는 아이디번호는 1부터 시작한다
		
#endif