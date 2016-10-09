/********************************************************************
	@date:	2016/10/01 6:39
	@file: 	C:\p4v\iPhone_zero\Caribe\App\game_src\XAsyncMng.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XAsyncMng;
XE_NAMESPACE_START( xnAsync )
//
struct xAsync {
friend class XAsyncMng;
	xSec m_secAfter;			// 몇초후에 실행할건지
	std::function<void()> m_funcAsync;
//private:
	CTimer m_Timer;
};


//
XE_NAMESPACE_END; // XGAME

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/10/01 6:39
*****************************************************************/
class XAsyncMng
{
public:
	static std::shared_ptr<XAsyncMng>& sGet();
	static void sDestroyInstance();
public:
	XAsyncMng();
	~XAsyncMng() { Destroy(); }
	//
	void Add( xSec secAfter, std::function<void()> func );
	void Process();
private:
	static std::shared_ptr<XAsyncMng> s_spInstance;
	XList4<xnAsync::xAsync> m_listAsync;	// 비동기로 실행되어야 하는 함수들 목록
	void Init() {}
	void Destroy() {}
}; // class XAsyncMng

