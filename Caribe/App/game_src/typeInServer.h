/********************************************************************
	@date:	2016/03/14 21:31
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\typeInServer.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

class XGameUser;
class XGameSvrConnection;
class XClientConnection;
class XAccount;
class XSAccount;

typedef std::shared_ptr<XGameUser> XSPGameUser;
typedef std::shared_ptr<const XGameUser> XSPGameUserConst;
typedef std::shared_ptr<XGameSvrConnection> XSPGameSvrConnect;
typedef std::shared_ptr<const XGameSvrConnection> XSPGameSvrConnectConst;
typedef std::shared_ptr<XClientConnection> XSPXClientConnection;
typedef std::shared_ptr<const XClientConnection> XSPXClientConnectionConst;

typedef std::shared_ptr<XSAccount> XSPSAcc;
typedef std::shared_ptr<const XSAccount> XSPSAccConst;

