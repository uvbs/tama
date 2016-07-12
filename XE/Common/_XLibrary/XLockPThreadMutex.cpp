#include "stdafx.h"
#include "XLockPThreadMutex.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if !defined(_SERVER) && !defined(_XBOT)		// not server
////////////////////////////////////////////////////////////
// static
XThreadID_pthread_t XLockPthreadMutex::sGetThreadID( void ) {
	pthread_t pthid = pthread_self();
	XThreadID_pthread_t idThread( pthid );
//	DWORD idThread = (DWORD) pthid.p;
	return idThread;
}

#endif 