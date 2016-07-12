#pragma once
#include "XDBAccount.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 14:24
*****************************************************************/
class XAccount : public XDBAccount
{
public:
	XAccount();
	virtual ~XAccount() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy();
}; // class XAccount
