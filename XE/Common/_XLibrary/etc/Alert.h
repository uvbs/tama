#ifndef __ALERT_H__
#define __ALERT_H__
#include "xeDef.h"

#ifdef WIN32
	#define	XALERT(F,...)		XAlert( MB_OK, _T(F), ##__VA_ARGS__ )
	// IDYES, IDNO
	#define	XALERT_YESNO(F,...) XAlert( MB_YESNO, _T(F), ##__VA_ARGS__ )
#else
	#define	XALERT(F,...)		XAlert( 0, _T(F), ##__VA_ARGS__ )
#endif

#ifdef _CLIENT
class XClientMain;
void SetAlertHandler( XClientMain *pHandler );
XClientMain* GetAlertHandler( void );
#endif 
int XAlert( int type, LPCTSTR str, ... );
int XAlert( XE::xtAlert type, LPCTSTR str, ... );
int XAlertString( int type, LPCTSTR szString );

#endif //  __ALERT_H__
