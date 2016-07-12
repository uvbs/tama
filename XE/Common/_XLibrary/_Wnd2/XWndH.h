#pragma once 

class XWnd;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/12/23 15:29
*****************************************************************/
class XDelegateWnd
{
public:
	XDelegateWnd() {}
	virtual ~XDelegateWnd() {}
	//
	virtual void DelegateBeforeDraw( XWnd *pWnd ) {}
	// SetAutoUpdate()에서 호출되는 델리게이트
	virtual void DelegateAutoUpdate( XWnd *pWndUpdater ) {}
}; // class XDelegateWnd

XE_NAMESPACE_START( xnWnd )
//
// layout 클릭이벤트용
struct xClickEvent {
	_tstring m_xmlClick;					// layout 클릭 이벤트용
	std::string m_strNodeClick;		// layouit 클릭 이벤트용. 노드 이름
	XE::VEC2 m_vPopupByLayout;		// layout 클릭 이벤트
	XE::xAlign m_AlignClick = XE::xALIGN_NONE;			// layout 클릭 이벤트용,
	std::string m_strKeyTarget;
	std::string m_strType;			// 팝업형태냐 일반형태냐(땜빵)
	ID m_idText = 0;
};

/**
 @brief  툴팁 정보
*/
struct xTooltip {
	_tstring m_xml; 
	std::string m_strNode;
	ID m_idText = 0;
	float m_Length = 0;		// 문단 폭
};


//
XE_NAMESPACE_END; // xnWnd

