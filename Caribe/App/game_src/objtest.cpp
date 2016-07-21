#include "stdafx.h"
#include "objtest.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

유닛간 메시지교환 방식으로 변경
.데미지주기
.직접 상대유닛의 hp를 깍지 않고 메시지만 던진다.
.받는측은 받은 메시지들을 모두 한꺼번에 처리한다.
.한턴에 두명으로부터 데미지를 받았다면 데미지준 상대와 데미지양을 두번다 받아놓고 자기차례에서 그것들을 차례로 꺼내 적용한다.
.피드백
.데미지를 주고 그 결과를 공격자가 바로 알아야 하는경우
.피드백도 다시 피격자가 공격자에게 메시지를 날려야 한다.
.
메시지의 종류
.데미지
.밀어내기
.버프걸기
.
메인루프
{
	objMng->SprFrameMove();		// SprObj의 frameMove. 키 이벤트발생
	objMng->ProcessMsg();		// 메시지큐 처리.쌓인 메시지를 처리하고 다시 다른 객체에게 메시지를 전달한다.(이중버퍼를 둬서 다음프레임에 처리하도록 한다.)
	objMng->ProcessAI();		// AI가 객체에게 명령
	objMng->ProcessMove();		// 명령을 수행함.
	objMng->FlipQ();			// 메시지큐 플립
}

원칙: 공격자는 방어자에게 메시지는 전달가능. 방어자는 const상태여야 한다.

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

