/********************************************************************
	@date:	2015/05/08 10:21
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XRaid.h
	@author:	xuzhu
	
	@brief:	길드 레이드
*********************************************************************/
#pragma once

namespace xGuild {

class XRaidCity;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/05/08 10:23
*****************************************************************/
class XRaid
{
  XList4<XRaidCity*> m_listCities;      // 레이드 던전
	void Init() {}
	void Destroy();
public:
	XRaid();
	virtual ~XRaid() { Destroy(); }
	//
}; // class XRaid

}; // namespace xGuild

// x0508