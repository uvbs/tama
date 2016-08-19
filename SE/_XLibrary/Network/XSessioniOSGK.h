#ifdef __NETWORK

#import <GameKit/GameKit.h>
#include <stdio.h>
#include "XBaseSession.h"

class XSessioniOSGK : public XSession
{
    GKSession *m_pGKSession;
	void Init() {
        m_pGKSession = NULL;
	}
	void Destroy();
public:
    XSessioniOSGK() : XSession( NULL ) { Init(); }
	XSessioniOSGK( XSessionDelegate *pDelegate, GKSession *pGKSession );
	XSessioniOSGK( XSessionDelegate *pDelegate );
	virtual ~XSessioniOSGK() { Destroy(); }
    //
//    void SetpGKSession( void *pGKSession );
//    void* GetpGKSession( void );
	//
	BOOL IsConnected( void );
	void Send( const BYTE *pBuffer, int len );
	void Disconnect( void );
	//

};
/*
class XSessioniOSGK : public XSession
{
    XSessioniOSGK_objc *m_pObjc;
	void Init() {
        m_pObjc = NULL;
	}
	void Destroy();
public:
    XSessioniOSGK() : XSession( NULL ) { Init(); }
	XSessioniOSGK( XSessionDelegate *pDelegate, void *pGKSession );
	XSessioniOSGK( XSessionDelegate *pDelegate );
	virtual ~XSessioniOSGK() { Destroy(); }
    //
    void SetpGKSession( void *pGKSession );
    void* GetpGKSession( void );
	//
	BOOL IsConnected( void );
	void Send( const BYTE *pBuffer, int len );
	void Disconnect( void );
	//

};
*/


#endif // network

