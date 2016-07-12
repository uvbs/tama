/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class Empty
{
	void Init() {}
	void Destroy() {}
public:
	Empty() { Init(); }
	virtual ~Empty() { Destroy(); }
	//
};

