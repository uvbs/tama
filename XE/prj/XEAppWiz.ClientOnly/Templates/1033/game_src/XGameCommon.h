#pragma once

////////////////////////////////////////////////////////////////
class XGameLua;
/**
 클라/서버 공통 컨텐츠 객체
*/
class XGameCommon
{
	XGameLua *m_pLua;
	void Init() {
		m_pLua = NULL;
	}
	void Destroy();
public:
	XGameCommon();
	virtual ~XGameCommon() { Destroy(); }
	//
	GET_ACCESSOR( XGameLua*, pLua );
	//
	void Create( void );
	void CreateCommon( void );
	void DestroyCommon( void );
};
