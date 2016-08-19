#include "XSessioniOSGKMatch.h"

#ifdef __NETWORK

// »ý¼ºÀÚ
// client mode
XSessioniOSGKMatch::XSessioniOSGKMatch( XSessionDelegate *pDelegate, GKMatch *pGKMatch ) 
: XSession( pDelegate )
{ 
	Init(); 
    m_pGKMatch = pGKMatch;
}
XSessioniOSGKMatch::XSessioniOSGKMatch( XSessionDelegate *pDelegate ) 
: XSession( pDelegate )
{ 
	Init(); 
}

void XSessioniOSGKMatch::Destroy() 
{
}

BOOL XSessioniOSGKMatch::IsConnected( void ) 
{
    return (m_pGKMatch)? TRUE : FALSE;
}
void XSessioniOSGKMatch::Disconnect( void ) 
{
	[m_pGKMatch disconnect];
//	[self setReceiveDataCallback:nil selector:nil];
	m_pGKMatch.delegate = nil;
}

void XSessioniOSGKMatch::Send( const BYTE *pBuffer, int len )
{
    //
    NSData *data = [NSData dataWithBytes:pBuffer length:len];
    //
    NSLog(@"send");
    
	[m_pGKMatch sendDataToAllPlayers:data withDataMode:GKSendDataReliable error:nil];
}

//////////////////////////////////////////////////////////////////////////////////

#endif // network

