#ifdef __NETWORK

#import <GameKit/GameKit.h>
#include <stdio.h>
#include "XBaseSession.h"

class XSessioniOSGKMatch : public XSession
{
    GKMatch *m_pGKMatch;
	void Init() {
        m_pGKMatch = NULL;
	}
	void Destroy();
public:
    XSessioniOSGKMatch() : XSession( NULL ) { Init(); }
	XSessioniOSGKMatch( XSessionDelegate *pDelegate, GKMatch *pGKMatch );
	XSessioniOSGKMatch( XSessionDelegate *pDelegate );
	virtual ~XSessioniOSGKMatch() { Destroy(); }
	//
	BOOL IsConnected( void );
	void Send( const BYTE *pBuffer, int len );
	void Disconnect( void );
	//

};



#endif // network

