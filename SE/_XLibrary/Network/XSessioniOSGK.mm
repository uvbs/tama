#include "XSessioniOSGK.h"
#include "XSessioniOSGK_objc.h"
#include "Alert.h"

#ifdef __NETWORK

// »ý¼ºÀÚ
// client mode
XSessioniOSGK::XSessioniOSGK( XSessionDelegate *pDelegate, GKSession *pGKSession ) 
: XSession( pDelegate )
{ 
	Init(); 
	m_pGKSession = pGKSession;
}
XSessioniOSGK::XSessioniOSGK( XSessionDelegate *pDelegate ) 
: XSession( pDelegate )
{ 
	Init(); 
}

void XSessioniOSGK::Destroy() 
{
    m_pGKSession.available = NO;
    [m_pGKSession disconnectFromAllPeers];
    [m_pGKSession setDataReceiveHandler: nil withContext: nil];
    m_pGKSession.delegate = nil;
    [m_pGKSession release];
}

/*void XSessioniOSGK::SetpGKSession( void *pGKSession )
{
    m_pObjc = new XSessioniOSGK_objc( (GKSession*)pGKSession );
    m_pObjc->SetpGKSession( (GKSession *)pGKSession );
}
void* XSessioniOSGK::GetpGKSession( void )
{
    return (void*)m_pObjc->GetpGKSession();
}*/

BOOL XSessioniOSGK::IsConnected( void ) 
{
	return (m_pGKSession.delegate)? TRUE : FALSE;
}
void XSessioniOSGK::Disconnect( void ) 
{
	m_pGKSession.available = NO;
	[m_pGKSession disconnectFromAllPeers];
}

void XSessioniOSGK::Send( const BYTE *pBuffer, int len )
{
    //
    NSData *data = [NSData dataWithBytes:pBuffer length:len];
    //
    NSLog(@"send");
    NSError *error = nil;
    
    if (![m_pGKSession sendDataToAllPeers:data withDataMode:GKSendDataReliable error:&error]) {
        // You will do real error handling
        XALERT( "Network error" );
//        NSLog(@"Error sending data: %@", [error localizedDescription]);
    }
}

//////////////////////////////////////////////////////////////////////////////////

#endif // network