/********************************************************************
	@date:	2016/03/02 12:53
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj\$selected$\$selected$\X$selected$Mng.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class X$selected$Obj;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 12:53
*****************************************************************/
class X$selected$Mng
{
public:
	static std::shared_ptr<X$selected$Mng>& sGet();
	static void sDestroyInstance();
public:
	X$selected$Mng();
	virtual ~X$selected$Mng() { Destroy(); }
	//
private:
	static std::shared_ptr<X$selected$Mng> s_spInstance;
	XList4<X$selected$Obj*> m_list$selected$s;
	void Init() {}
	void Destroy();
	X$selected$Obj* Add$selected$Obj( X$selected$Obj* pObj );
	void Process( float dt );
	void Destroy$selected$Obj( X$selected$Obj* pObj );
}; // class X$selected$Mng

