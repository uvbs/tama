#pragma once

XE_NAMESPACE_START( xCampaign )
	
enum xtStageState {
	xSS_NONE = 0,
	xSS_ENTERABLE,	// 스테이지에 진입가능한 상태
	xSS_CLEARED,	// 클리어함. 진입불가
	xSS_LOCK,		// 잠긴상태
	xSS_BOSS,		// 보스상태
	xSS_RETRY,		// 재도전할수 있는곳.
	xSS_NEW_OPEN,	// 새로오픈된곳.
};
	
XE_NAMESPACE_END;

