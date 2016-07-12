#pragma once
#include "XFramework/server/XEUserMng.h"
//#include "XGameUser.h"

class XGameUser;
class XGameUserMng : public XEUserMng
{
	void Init() {}
	void Destroy();
public:
	XGameUserMng( int maxUser );
	virtual ~XGameUserMng() { Destroy(); }
	//
	XSPGameUser GetUser( ID idAccount );
	//
};

extern XGameUserMng *XGAME_USERMNG;
