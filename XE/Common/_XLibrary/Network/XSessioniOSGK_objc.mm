#include "XSessioniOSGK_objc.h"

#ifdef __NETWORK
#include "Alert.h"

// 생성자
// client mode
XSessioniOSGK_objc::XSessioniOSGK_objc( GKSession *pGKSession ) 
{ 
	Init(); 
    m_pGKSession = pGKSession;
}
void XSessioniOSGK_objc::Send( const char *pBuffer, int len )
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
