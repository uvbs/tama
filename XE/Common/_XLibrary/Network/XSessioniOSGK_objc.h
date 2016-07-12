#pragma once
#ifdef __NETWORK
#import <GameKit/GameKit.h>

class XSessioniOSGK_objc 
{
    GKSession *m_pGKSession;
	void Init() {
        m_pGKSession = NULL;
	}
	void Destroy() {
        m_pGKSession.available = NO;
        [m_pGKSession disconnectFromAllPeers];
        [m_pGKSession setDataReceiveHandler: nil withContext: nil];
        m_pGKSession.delegate = nil;
        [m_pGKSession release];
	}
public:
	XSessioniOSGK_objc( GKSession *pGKSession );
	virtual ~XSessioniOSGK_objc() { Destroy(); }
    //
    GET_SET_ACCESSOR( GKSession*, pGKSession );
	//
	BOOL IsConnected( void ) {
        return (m_pGKSession.delegate)? TRUE : FALSE;
	}
	void Send( const char *pBuffer, int len );
	void Disconnect( void ) {
        m_pGKSession.available = NO;
        [m_pGKSession disconnectFromAllPeers];
	}
};



#endif // network

