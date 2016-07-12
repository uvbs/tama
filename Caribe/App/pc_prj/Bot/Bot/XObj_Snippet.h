/********************************************************************
	@date:	
	@file: 	
	@author:
	
	@brief:	
*********************************************************************/
#pragma once

/****************************************************************
* @brief 
* @author
* @date	
*****************************************************************/
class X$selected$Obj
{
public:
	X$selected$Obj();
	virtual ~X$selected$Obj() { Destroy(); }
	// get/setter
	GET_BOOL_ACCESSOR( bDestroy );
	// public member
private:
	// private member
	bool m_bDestroy = false;
private:
	// private method
	void Init() {}
	void Destroy();
	void OnCreate();
	void Process( float dt );
}; // class X$selected$Obj
