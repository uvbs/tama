#pragma once
#include "XFramework/server/XEUserMng.h"

class XGameUser;
class XGameUserMng : public XEUserMng
{
	void Init() {}
	void Destroy();
public:
	XGameUserMng( int maxUser );
	virtual ~XGameUserMng() { Destroy(); }
	//
	XSPGameUser GetspUser( ID idAccount );
	//
};

extern XGameUserMng *XGAME_USERMNG;
