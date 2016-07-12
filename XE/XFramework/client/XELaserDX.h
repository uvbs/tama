/********************************************************************
	@date:	2014/10/06 14:13
	@file: 	D:\xuzhu_work\Project\iPhone\XE\XFramework\client\XELaserDX.h
	@author:	xuzhu
	
	@brief:	2d laser effect
*********************************************************************/
#pragma once

/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/10/06 14:14
*****************************************************************/
class XELaserDX
{
	void Init() {}
	void Destroy();
public:
	XELaserDX();
	virtual ~XELaserDX() { Destroy(); }
};
