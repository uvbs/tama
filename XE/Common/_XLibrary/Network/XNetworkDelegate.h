#pragma once
#include "xenDef.h"

class XENetworkConnection;
class XCrypto;
class XNetworkDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XNetworkDelegate() { Init(); }
	virtual ~XNetworkDelegate() { Destroy(); }
	//
	// 소켓에 연결(connect)이 성공함. login성공이 아님에 주의.(XESocketClient::ProcessReservedPacket()로부터 호출됨)()
	virtual void DelegateConnect( XENetworkConnection *pConnect, DWORD param ) {}
	// 소켓에 지정된 시간안에 연결되지 못함
	virtual void DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error );
	// 
//	virtual XCrypto* DelegateGetCrypter( void ) { return NULL; }
};
