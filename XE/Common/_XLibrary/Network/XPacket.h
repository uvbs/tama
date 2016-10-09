#pragma once

#include "XArchive.h"

////////////////////////////////////////////////////////////////
/**
 프로토콜 헤더와 고유키값이 헤더로 있는 아카이브로써 일반적으로 네트워크
 패킷으로 사용된다. XPacket이 XArchive와 다른점은 아카이브의 헤더에
 언제나 idPacket(DWORD)과 idKey(DWORD)가 있다는것을 보증한다는 것이다.
*/
#define xMANUAL_MODE			1.0f

class XPacket : public XArchive
{
	ID m_idPacket;
	ID m_idKey;
	void Init() {
		m_idPacket = 0;
		m_idKey = 0;
	}
	void Destroy() {}
public:
	/**
	 일반적으로 쓰기용 패킷을 초기화할때 사용한다. 
	 패킷헤더만 넣으면 고유키값은 자동으로 만들어 헤더 다음에 붙여준다
	*/
	// 생성자를 수동으로 만들때 사용하는 다른 버전. 빈걸로 생성하고 SetPacketHeader()로 채운다
	XPacket() {
		Init();
	}
	XPacket( ID idPacket );
	XPacket( ID idPacket, ID idKey );
//	XPacket( int sizeMax, ID idPacket, ID idKey );
	XPacket( ID idPacket, bool bDynamicBuffer, int sizeMax )
		: XPacket( idPacket, XE::GenerateID(), bDynamicBuffer, sizeMax ) {}
	XPacket( ID idPacket, ID idKey, bool bDynamicBuffer, int sizeMax );
	/**
	 읽기전용 패킷을 만들때 필요로 한다. 네트워크의 받는측(클라이언트)에서 보통사용한다.
	*/
	XPacket( bool readOnly ) {
		Init();
	}
	/**
	 생성자를 수동으로 만들때 사용. xuzhu외에 사용하지 말것.
	*/
	XPacket( float __manualMode ) {
		Init();
		m_idKey = XE::GenerateID();
	}
	//
	GET_ACCESSOR( ID, idKey );
	GET_ACCESSOR( ID, idPacket );
	/// xuzhu외에 사용금지
	void _SetidPacket( ID idPacket ) {
		m_idPacket = idPacket;
	}
	void SetPacketHeader( ID idPacket ) {
		XBREAK( !IsStart() );
		m_idPacket = idPacket;
		*this << idPacket;
		*this << m_idKey;
	}
	void SetPacketHeader2( ID idPacket ) {
		XBREAK( !IsStart() );
		m_idPacket = idPacket;
		m_idKey = XE::GenerateID();
		*this << idPacket;
		*this << m_idKey;
	}
	void SetPacketHeader3( ID idPacket, ID idKey ) {
		XBREAK( !IsStart() );		// 갑자기 여기서 죽으면 솔루션정리후 리빌드.
		m_idPacket = idPacket;
		*this << idPacket;
		m_idKey = idKey;
		*this << idKey;
	}
	XPacket( XPacketMem *pPacketMem ) 
		: XArchive( pPacketMem ) {
		Init();
	}
	virtual ~XPacket() { Destroy(); }
};
