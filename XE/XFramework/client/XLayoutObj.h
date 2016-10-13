#pragma once

class XWnd;
class XLayout;
class XDelegateLayout;
////////////////////////////////////////////////////////////////
/**
 @brief 레이아웃 객체의 자동삭제를 위한 클래스
 보통 씬 레이아웃에 사용한다.
*/
class XLayoutObj
{
public:
	static XLayoutObj* sCreateLayout( LPCTSTR szXml, const char *cNodeName, XWnd *pRoot );
private:
	XLayout *m_pLayout = nullptr;
	void Init() {}
	void Destroy();
public:
	XLayoutObj() { Init(); }
	XLayoutObj( LPCTSTR szXml, XDelegateLayout *pDelegate = nullptr );
	XLayoutObj( const _tstring& strXml, XDelegateLayout *pDelegate = nullptr )
		: XLayoutObj( strXml.c_str(), pDelegate ) { }
	virtual ~XLayoutObj() { Destroy(); }
	//
	GET_ACCESSOR( XLayout*, pLayout );
	bool IsError() const {
		return m_pLayout == nullptr;
	}
	BOOL CreateLayout( const char *cKey, XWnd *pParent, const char *cKeyGroup = nullptr );
	inline bool CreateLayout( const std::string& strKey, XWnd *pParent ) {
		return CreateLayout( strKey.c_str(), pParent, nullptr ) != FALSE;
	}
	bool CreateLayout2( const std::string& strKey, XWnd *pParent );
};
