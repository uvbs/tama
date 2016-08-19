#pragma once

#include "XPacket.h"
#include "ContainerSTL.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ǿ��� �߻��ϴ� �̺�Ʈ���� ��������Ʈ�� �ޱ� ���ϴ� Ŭ������ �̰��� ��ӹ޾ƾ� �Ѵ�.
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


class XBaseScene;
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
	xtMode m_Mode;		// �������� Ŭ�����
	XSessionDelegate *m_pDelegate;
	XQueueArray<BYTE> m_qBuffer;		// �迭�� �̿��� ť����
	int m_sizePacket;			// ���۷κ��� �о�鿩���� ��Ŷũ��
	BOOL m_bDisconnect;		// ������ �����
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
	// ���� �׿��ִ� ť������ ũ��. �̰� �ʹ� ������ ��Ŷ������ �����ٴ� ���̴�.
	// ���� ť���۴� �ٴ��ε� ������ �����ٸ� Ʈ�������� �������̴�.
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
	// ���� Ŭ������ p�� �޾Ƽ� �Ҹ��ؾ��Ѵ�
//	virtual BOOL ProcessPacket( XPacket& p ) = 0;
};



