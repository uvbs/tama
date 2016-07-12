#pragma once
#if 0
//#include "XGameSvrMng.h"
#include "XGSvrMng.h"

class XGSvr;
class XGSvrMng;

class XGameSvrMng: public XGSvrMng
{
	void Init() {}
	void Destroy();
public:
	XGameSvrMng(int maxSvr);
	virtual ~XGameSvrMng(){Destroy();}

	inline XGSvrMng* GetGameSvr( ID SvrID ) {
		return SafeCast<XGSvrMng*>( GetSvrFromSvrID( SvrID ) );
	}
};

//extern XGameSvrMng *XGAME_GSVRMNG;
#endif // 0