#pragma once

XE_NAMESPACE_START( xSpot )
//
enum xtStateMandrake {
	xSM_NONE,
	xSM_NO_MATCH,		// ? 상태
	xSM_CHALLENGE,	// 도전상태
	xSM_DEFENSE,		// 방어상태
};
enum xtStateCastle {
	xSC_NONE,
	xSC_NO_MATCH,		// 아직 매치 안된상태
	xSC_ENEMY,			// 적이 점유중
	xSC_MY,					// 내꺼인상태
};
//
XE_NAMESPACE_END; // xSpot


