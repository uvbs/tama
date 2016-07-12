#pragma once

class XEServerMain;
////////////////////////////////////////////////////////////////
class XEServerDelegate
{
	static XEServerMain *s_pMain;
	void Init() {
		s_pMain = NULL;
	}
	void Destroy();
public:
	XEServerDelegate() { Init(); }
	virtual ~XEServerDelegate() { Destroy(); }

	void Create( _tstring strINI=_T("") );
	// 서버의 메인 객체를 생성시켜야 한다.
	virtual XEServerMain* OnCreateMain( _tstring& strINI ) = 0;
};
