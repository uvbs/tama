#pragma once
#include "XArchive.h"
#include "XList.h"
#include "XStruct.h"

namespace XGAME {
	struct xReward;
}
/**
 ver 100: 시작
 ver 101: serialize format 변경
*/
#define VER_POST			101

class XPostItem
{
public:
	XPostItem();
	XPostItem( XGAME::xtPOSTResource type, int count );
	XPostItem( XGAME::xtPOSTResource type, ID idItem, int count ) {
		Init();
		m_typeprop = type;
		m_idprop = idItem;
		m_nCount = count;
	}
	~XPostItem();
	void Init();

	XGAME::xtPOSTResource GetItemType() const { return m_typeprop; }
	void SetItemType(XGAME::xtPOSTResource idx){ m_typeprop = idx; }

	ID GetItemID() const { return m_idprop; }
	void SetItemID(ID idx){ m_idprop = idx; }

	int GetnCount() const { return m_nCount;}
	void SetnCount(int n){ m_nCount = n; }

	void Serialize(XArchive& p) const;
	void DeSerialize(XArchive& p, int verPost );
	XGAME::xReward GetReward();
private:
	ID		m_idprop = 0;
	XGAME::xtPOSTResource m_typeprop = XGAME::xPOSTRES_INIT;
	int		m_nCount = 1;
};

/**
 @brief 메일 하나의 객체
*/
class XPostInfo
{
public:
	struct sPostItem {
		ID		sidprop;
		ID		stypeprop;
		int		sCount;
	};
public:
	XPostInfo(){
		Init();
	}
	XPostInfo( ID snPost ) {
		Init();
		m_Index = snPost;
	}
	virtual ~XPostInfo(){}

	ID GetsnPost() const {
		return m_Index;
	}
	GET_SET_TSTRING_ACCESSOR( strRecvName );
	LPCTSTR GetstrSenderName() const {
		if( !m_strSenderName.empty() )
			return m_strSenderName.c_str();
		return XTEXT(m_idTextSender);
	}
	LPCTSTR GetstrTitle() const {
		if( !m_strTitle.empty() )
			return m_strTitle.c_str();
		return XTEXT(m_idTextTitle);
	}
	LPCTSTR GetstrMessage() const {
		if( !m_strMessage.empty() )
			return m_strMessage.c_str();
		return XTEXT(m_idTextMsg);
	}
	SET_TSTRING_ACCESSOR( strSenderName );
	SET_TSTRING_ACCESSOR( strTitle );
	SET_TSTRING_ACCESSOR( strMessage );
	GET_SET_ACCESSOR_CONST( ID, RewardTableid );
	GET_SET_ACCESSOR_CONST( ID, Index );
	GET_SET_ACCESSOR_CONST( XGAME::xtPostType, PostType );
	GET_SET_ACCESSOR_CONST( ID, idTextSender );
	GET_SET_ACCESSOR_CONST( ID, idTextTitle );
	GET_SET_ACCESSOR_CONST( ID, idTextMsg );
	GET_ACCESSOR_CONST( const XList4<XSPPostItem>&, listPostItems );
	int GetnumItems() const {
		return m_listPostItems.size();
	}
	void AddlistPostItems( XSPPostItem spItem );
	XSPPostItem AddPostGem( int numGem );
	XSPPostItem AddPostGold( int numGold );
	XSPPostItem AddPostGuildCoin( int num );
	XSPPostItem AddPostItem( ID idItem, int num );
	void DelPostItems();

	void Init();
	BOOL IsExpirePost();
	int ReceiveItem( int itempos, int itempropid );				//받을 개수를 반환

	void Serialize( XArchive& p ) const;
	void DeSerialize( XArchive& p, int ver );

	ID getid() {
		return m_Index;
	}
	void SerializeItems( XArchive& ar ) const;
	void DeSerializeItems( XArchive& ar, int num, int verPost );
private:
	XList4<XSPPostItem>	m_listPostItems;			// 우편함 인벤
	ID m_idTextSender = 0;
	ID m_idTextTitle = 0;
	ID m_idTextMsg = 0;
	// tstring에 값이 있을경우 그값이 우선되고 없으면 ID로 텍스트를 찾는다.
	_tstring			m_strSenderName;			// 보낸사람이름
	_tstring			m_strRecvName;				// 받는사람이름
	_tstring			m_strTitle;					// 우편 제목
	_tstring			m_strMessage;				// 우편 메세지.	
	ID					m_Index;					// mailindex
	ID					m_RewardTableid;			// 받을 물품..id..같은거
	XGAME::xtPostType m_PostType;					// 우편 타입.	
};

