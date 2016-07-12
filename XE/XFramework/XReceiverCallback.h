/********************************************************************
	@date:	2016/02/18 10:44
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XReceiverCallback.h
	@author:	xuzhu
	
	@brief:	Device의 callback thread로부터 각종 결과값을 받아서 큐에 밀어넣고 메인스레드에서 뽑아서
	기능별로 처리하는 일반화 클래스.
*********************************************************************/
#pragma once
#include "XFramework/XReceiverCallbackH.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/18 10:46
*****************************************************************/
class XReceiverCallback
{
public:
public:
	static std::shared_ptr<XReceiverCallback>& sGet();
	static void sDestroyInstance();
public:
	XReceiverCallback() { Init(); }
	virtual ~XReceiverCallback() { Destroy(); }
	//
	void cbOnReceiveCallback( const std::string& strIds
													, const std::string& strParam1
													, const std::string& strParam2
													, const std::string& strParam3 = std::string() );
	void Process();
private:
	static std::shared_ptr<XReceiverCallback> s_spInstance;
	XLock m_Lock;
	XList4<xnReceiverCallback::xData> m_listCallbackData;
	void Init() {}
	void Destroy() {}
}; // class XReceiverCallback

