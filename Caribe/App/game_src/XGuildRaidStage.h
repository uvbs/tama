/********************************************************************
	@date:	2015/05/08 10:28
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XGuildRaidStage.h
	@author:	xuzhu
	
	@brief:	길드레이드 던전의 스테이지
*********************************************************************/
#pragma once

namespace xGuild {
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/05/08 10:29
*****************************************************************/
class XRaidStage
{
	void Init() {}
	void Destroy();
public:
	XRaidStage();
	virtual ~XRaidStage() { Destroy(); }
	//
}; // class XRaidStage
////////////////////////////////////////////////////////////////
XRaidStage::XRaidStage()
{
	Init();
}

void XRaidStage::Destroy()
{
}

}; // namespace xGuild