/********************************************************************
	@date:	2016/03/14 19:46
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\Common\_XLibrary\Network\xenDef.cpp
	@author:	xuzhu
	
	@brief:	서버/클라 네트워크 공통 static 모듈
*********************************************************************/
#include "stdafx.h"
#include "XWinNetwork.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

WSADATA XWinNetwork::s_WSAData = { 0, };

bool XWinNetwork::sStartUp()
{
	if( s_WSAData.wVersion == 0 ) {
		if( WSAStartup( MAKEWORD(1,1), &s_WSAData ) != 0 ) {
			return false;
		}
	}
	return true;
}
