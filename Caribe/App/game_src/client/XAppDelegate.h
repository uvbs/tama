#pragma once
#include "XFramework/client/XApp.h"
#include "XFramework/client/XLayout.h"

////////////////////////////////////////////////////////////////
class XAppDelegate : public XApp
										, public XDelegateLayout
{
	void Init() {}
	void Destroy() {
		s_pInstance = nullptr;
	}
public:
	XAppDelegate();
	virtual ~XAppDelegate() { Destroy(); }
	//
	virtual XClientMain* OnCreateAppMain( XE::xtDevice device, float widthPhy, float heightPhy );
	XWnd* DelegateCreateCustomCtrl( const std::string& strcCtrl
																, TiXmlElement* pElemCtrl
																, const XWnd *pParent
																, const XLayout::xATTR_ALL& attrAll) const override;
public:
	static XAppDelegate* s_pInstance;
	static XAppDelegate* sGet() {		return s_pInstance;	}
private:
};
