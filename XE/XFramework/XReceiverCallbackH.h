#pragma once
#include "etc/Global.h"

XE_NAMESPACE_START( xnReceiverCallback )
//
// 디바이스의 스레드에서 이구조로 저장을 하고 게임 메인스레드에서 뽑아서 각 기능에 따라 사용한다.
struct xData {
	std::string m_strIds;			// 콜백 식별자
	XVector<std::string> m_aryParam;
	xData() : m_aryParam(3) {}
	xData( const std::string& strIds
		, const std::string& strParam1
		, const std::string& strParam2
		, const std::string& strParam3 = std::string() ) : m_aryParam( 3 ) {
		m_strIds = strIds;
		m_aryParam[0] = strParam1;
		m_aryParam[1] = strParam2;
		m_aryParam[2] = strParam3;
	}
};
//
XE_NAMESPACE_END; // xnReceiverCallback

