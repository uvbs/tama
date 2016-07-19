#pragma once
#include "server/XEUserMng.h"
#include "XGameUser.h"

class XGameUser;
class XGameUserMng : public XEUserMng
{
	void Init() {}
	void Destroy();
public:
	XGameUserMng( int maxUser );
	virtual ~XGameUserMng() { Destroy(); }
	//
	inline XGameUser* GetUser( ID idAccount ) {
		return SafeCast<XGameUser*, XEUser*>( GetUserFromAccountID( idAccount ) );
	}
	//
};

extern XGameUserMng *XGAME_USERMNG;
