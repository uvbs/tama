/********************************************************************
	@date:	2015/05/08 10:27
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XGuildRaidCity.h
	@author:	xuzhu
	
	@brief:	길드 레이드 던전
*********************************************************************/
#pragma once

XE_NAMESPACE_START( xGuild )

class XRaidStage;
/****************************************************************
* @brief 길드 레이드에서 던전 하나
* @author xuzhu
* @date	2015/05/08 10:27
*****************************************************************/
class XRaidCity
{
  ID m_snCity = 0;
  bool m_bClear = false;      // 클리어 여부. 이번 레이드턴에 끝스테이지까지 클리어했는지.
  bool m_bConquest = false;   // 정복 여부(단한번이라도 끝까지 클리어한적이 있으면 true)
  XTimerTiny m_timerLimit;    // 던전이 열리면 그때부터 타이머 시작
  XList4<XRaidStage*> m_listStages;   // 스테이지 리스트
  xSec m_secLimit = xHOUR_TO_SEC(24);   // 공략 제한시간(이 시간이 지나면 던전은 리셋된다.)
  int m_idxStage = 0;         // 현재 공략중인 스테이지 인덱스
	void Init() {
    m_snCity = XE::GenerateID();
  }
	void Destroy();
public:
	XRaidCity();
	virtual ~XRaidCity() { Destroy(); }
	//
  GET_SET_BOOL_ACCESSOR( bClear );
  GET_ACCESSOR( int, idxStage );

  /// 공략제한시간까지 남은시간
  xSec GetsecRemainLimit() {
    return m_secLimit - m_timerLimit.GetsecPass();
  }
  bool IsTimeOver() {
    return m_timerLimit.GetsecPass() >= m_secLimit;
  }

}; // class XRaidCity

XE_NAMESPACE_END; // namespace xGuild

