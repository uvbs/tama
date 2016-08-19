#pragma once 
#include "string.h"

class XError
{
	static TCHAR s_szError[ 1024 ];		
public:
	XError() {}
	virtual ~XError() {}
	//
	LPCTSTR SetErrorString( LPCTSTR szError ) {
		_tcscpy_s( s_szError, szError );
		return s_szError;
	}
	LPCTSTR GetErrorString( void ) { return s_szError; }
};