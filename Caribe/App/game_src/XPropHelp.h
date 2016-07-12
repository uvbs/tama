/********************************************************************
	@date:	2015/06/22 18:55
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XPropHelp.h
	@author:	xuzhu
	
	@brief:	시퀀스 프로퍼티 관리자
	@note: 인스턴스 네이밍 규칙:
			명시적으로 "Prop"이라는 네이밍이 없으면 그건 인스턴스로 간주한다
*********************************************************************/
#pragma once
#include "XXMLDoc.h"

const int VER_PROP_HELP = 1;

XE_NAMESPACE_START( xHelp )
// class XPropOrder;
// class XPropSeq;
// class XPropDialog;
// class XPropIndicate;
// class XPropCamera;
// class XPropTouch;
// class XPropReadyTouch;
// class XPropDelay;
// class XPropMode;
// enum xtEvent {
// 	xHE_NONE,
// 	xHE_END_CAMERA_MOVING,	// 카메라 이동이 끝남
// 	xHE_DRAGGED_SLOT,		// 전투전 편성씬에서 부대슬롯을 드래그
// };
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/22 16:46
*****************************************************************/
class XPropHelp : public XXMLDoc
{
public:
	static std::shared_ptr<XPropHelp>& sGet();
	static bool s_bTextAdded;	// 저장시 새로운 텍스트가 텍스트테이블에 추가됐다.
public:
	XPropHelp();
	virtual ~XPropHelp() { Destroy(); }
	//
	bool Load( LPCTSTR szXml );
	PropSeqPtr GetspProp( const std::string& strIdentifier );
	std::shared_ptr<XPropSeq> GetspProp( const char *cIdentifier ) {
		std::string ids = cIdentifier;
		return GetspProp( ids );
	}
	void DestroyAll();
// 	PropSeqPtr FindSeqByCond( const std::string& idsSeqLast, 
// 							XGAME::xtActionEvent actEvent,
// 							DWORD dwParam );
// 	PropSeqPtr FindSeqByCond( const std::string& idsSeqLast, 
// 							XGAME::xtActionEvent actEvent,
// 							const std::string& strParam );
	PropSeqPtr FindSeqByCond( const std::string& idsSeqLast, 
							XGAME::xtActionEvent actEvent,
							DWORD dwParam = DEFAULT,
							const std::string& strParam = std::string(),
							DWORD dwParam2 = DEFAULT );
	int GetPropToAry( std::vector<PropSeqPtr> *pOutAry ) {
		for( auto itor : m_mapSeq )
			pOutAry->push_back( itor.second );
		return pOutAry->size();
	}
	bool Save( LPCTSTR szXml );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
private:
	static std::shared_ptr<XPropHelp> s_spInstance;
	std::map<std::string, std::shared_ptr<XPropSeq>> m_mapSeq;
#ifdef _xIN_TOOL
	XList4<std::shared_ptr<XPropSeq>> m_listSeq;		// 저장때 순서를 유지하기위해 리스트에도 받아둠.
#endif // _xIN_TOOL
	bool m_bError = false;
	_tstring m_idsSeqLast;
	void Init() {}
	void Destroy();
	bool OnDidFinishLoad() override;
	std::shared_ptr<XPropSeq> LoadSeq2( const std::string& idsSeq );
	std::shared_ptr<XPropSeq> LoadSeq( XEXmlNode& node );
	void LoadOrderCommon( XEXmlNode& node, XPropOrder *pOut );
	PropOrderPtr LoadOrderDialog( XEXmlNode& node );
	PropOrderPtr LoadOrderIndicate( XEXmlNode& node );
	PropOrderPtr LoadOrderCamera( XEXmlNode& node );
	PropOrderPtr LoadOrderTouch( XEXmlNode& node );
	PropOrderPtr LoadOrderDelay( XEXmlNode& node );
//	PropOrderPtr LoadOrderReadyTouch( XEXmlNode& node );
	PropOrderPtr LoadOrderMode( XEXmlNode& node );
}; // class XPropHelp

XE_NAMESPACE_END;
