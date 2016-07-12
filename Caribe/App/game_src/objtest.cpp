#include "stdafx.h"
#include "objtest.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


class Unit {
	메시지큐 qMsgBack;		// 다음프레임에 처리해야할 메시지
	메시지큐 qMsgFront;		// 현재 프레임에 처리해야할 메시지
	SprFrameMove() {
		pSprObj->FrameMove();
	}
	FlipQ() {
		swap( qMsgBack, qMsgFront );	// 백단에 쌓여있던 큐를 처리하기위해 프런트로 바꾼다.
	}
	ProcessMsg() {
		while( 메시지 = qMsgFront.pop() ) {
			switch( 메시지 ) {
			case 공격: {
				피해량 = 메시지.공격데미지 - 내방어력;
				hp -= 피해량;
				if( hp <= 0 ) {
					죽는상태전환
				}
				메시지.공격자->PushMsgQBack( 피격_피드백 );
			};
			case 피격_피드백: {
				if( 메시지.상태 == 피격성공 ) {
					if( 흡혈상태 ) {
						++hp;
					}
				}
				if( 메시지.타겟->IsLive() == 죽음 ) {
					++score;
				}
			}
			case 마법적용: {
				메시지.적용효과;
				ApplyEffect( 메시지.적용스킬, 메시지.적용효과 )
			}

			}
		}
	}
	OnEventSpr( 이벤트 ) {
		if( 이벤트 == 일반타격 ) {
			if( 타겟 ) {
				타겟->PushMsgQBack( 공격 );
			}
		} else 
		if( 이벤트 == 마법_사용 ) {
			if( 타겟 ) {
				타겟->PushMsgQBack( 적용효과 );
			}
		}
	}
	ProcessAI() {
		if( 상태 == 타겟검색 ) {
			타겟 = FindTarget();
			if( 타겟 ) {
				PushFSMMsg( 추적 )
			}
		} else
		if( 상태 == 추적 ) {
			if( 타겟 ) {
				vDelta = 타겟쪽으로의 이동값;
			} else {

			}
		}
	}

};

