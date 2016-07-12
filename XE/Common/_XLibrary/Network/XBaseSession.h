#pragma once

#include "XPacket.h"
//#include "ContainerSTL.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 세션에서 발생하는 이벤트들의 델리게이트를 받길 원하는 클래스는 이것을 상속받아야 한다.
class XSession;
class XSessionDelegate
{
public:
	XSessionDelegate() {}
	virtual ~XSessionDelegate() {}

	virtual void DelegateConnect( XSession *pSession ) {}
	virtual void DelegateDisconnect( XSession *pSession ) {}
	virtual void DelegateAccept( XSession *pSession ) {}
	virtual void DelegateFinishListen( XSession *pSession ) {}
};


class XEBaseScene;
class XSession;
class XSession
{
public:
	enum xtMsg { 
		xNONE = 0,
		xON_CONNECT,
		xON_ACCEPT
	};
	enum xtMode { xMODE_NONE=0, xSERVER, xCLIENT };
private:
	void Init() {	
		m_pDelegate = NULL;
		m_Mode = xMODE_NONE;
		m_sizePacket = 0;
		m_bDisconnect = FALSE;
	}
	void Destroy() {	}
protected:
	xtMode m_Mode;		// 서버모드냐 클라모드냐
	XSessionDelegate *m_pDelegate;
	XQueueArray<BYTE> m_qBuffer;		// 배열을 이용한 큐구현
	int m_sizePacket;			// 버퍼로부터 읽어들여야할 패킷크기
	BOOL m_bDisconnect;		// 접속이 끊겼다
public:
	XSession( XSessionDelegate *pDelegate ) { 
        Init(); 
        m_pDelegate = pDelegate; 
        m_qBuffer.Create( 0xffff );		
    }
	virtual ~XSession() { Destroy(); }
	//
	virtual BOOL IsConnected( void ) = 0;
	virtual void Disconnect( void ) = 0;
	// 현재 쌓여있는 큐버퍼의 크기. 이게 너무 많으면 패킷펌핑이 느리다는 것이다.
	// 만약 큐버퍼는 바닥인데 반응이 느리다면 트래픽쪽이 과부하이다.
	int GetqBufferSize( void ) {
		return m_qBuffer.size();
	}
    void PushBlock( BYTE *buffer, int len ) {
        m_qBuffer.PushBlock( buffer, len );
    }
	// virtual
	virtual void SetDelegate( XSessionDelegate *pDelegate ) { m_pDelegate = pDelegate; }
	virtual void Send( const BYTE *pBuffer, int len ) {}
	void Send( XPacket& ar ) { Send( ar.GetBufferMem(), ar.size() );	}
    virtual BOOL PumpPacket( XPacket *pOutPacket );
    virtual void Process( void );
	// 하위 클래스는 p를 받아서 소모해야한다
//	virtual BOOL ProcessPacket( XPacket& p ) = 0;
};



