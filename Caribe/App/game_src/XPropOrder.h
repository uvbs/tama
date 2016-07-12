/********************************************************************
	@date:	2015/06/22 15:47
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XPropOrder.h
	@author:	xuzhu
	
	@brief:	명령 프로퍼티
*********************************************************************/
#pragma once

#include "XXMLDoc.h"

XE_NAMESPACE_START( xHelp )
enum xtOrder {
	xHO_NONE,
	xHO_DIALOG,			/// 대화창
	xHO_INDICATE,		/// 화살표 생성
	xHO_DELAY,			/// 강제 딜레이
	xHO_TOUCH,			/// 터치 막음/풀음
	xHO_CAMERA,			/// 카메라이동
	xHO_READY_TOUCH,	/// 특정 윈도우의 터치를 기다림
	xHO_MODE_CHANGE,	/// 모드 전환류
};
/// 명령 종료방식
enum xtFinish {
	xFIN_NONE,
	xFIN_TOUCH,			/// 터치
//	xFIN_PUSH_BUTT,		/// 특정 버튼 누름.
	xFIN_DELAY,			/// 딜레이
	xFIN_FADE,			/// 페이드인아웃
	xFIN_END_ANI,		/// 애니메이션의 종료
	xFIN_MOVE_CAMERA,	/// 카메라의 이동
	xFIN_END_ORDER,		/// 다른명령이 끝날때 함께 끝남 
	xFIN_PUSH_TARGET,	/// ids_target이 가리키는곳을 터치하면 종료
	xFIN_ORDER_COMPLETE,		/// 명령객체가 정상수행을 마치고 직접 complete를 보내줄때
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/22 15:47
*****************************************************************/
class XPropOrder
{
	// 프로퍼티의 값들은 바뀌는값이 아니라서 그냥 퍼블릭으로 함.
public:
#ifdef _xIN_TOOL
	std::string m_strXmlIds;	// 저장용. "dialog" "indicate" 이런거를 받아둠.
	std::string m_strXmlTypeEnd;	// m_typeEnd의 원래 문자열
#endif // _xIN_TOOL
	std::string m_strIdentifer;		// 각 오더의 고유 식별자(없을수도 있음)
	xtOrder m_Type = xHO_NONE;
	std::string m_idsEnd;		// m_typeEnd가 xFIN_WITH_ORDER일때 그 오더의 식별자.
//	std::string m_idsTarget;
	std::vector<std::string> m_aryIdsTarget;	// 눌러야하는 타겟
	std::vector<std::string> m_aryIdsAllow;		// 누르기를 허용하는 타겟
	xtFinish m_typeEnd = xFIN_NONE;
	int m_idx = -1;
public:
	XPropOrder( xtOrder type ) { 
		Init(); 
		m_Type = type;
	}
	virtual ~XPropOrder() { Destroy(); }
	bool IsEndTypeTouch() {
		return m_typeEnd == xFIN_TOUCH;
	}
	//
	virtual bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) = 0;
	bool SaveCommon( XEXmlNode& nodeSeq );
	virtual void Serialize( XArchive& ar ) const;
	virtual void DeSerialize( XArchive& ar, int );
private:
	void Init() {}
	void Destroy() {}
}; // class XPropOrder
/****************************************************************
* @brief 어떤 위치를 가리키는 인디케이터(주로 화살표)
* @author xuzhu
* @date	2015/06/22 16:28
*****************************************************************/
class XPropIndicate : public XPropOrder
{
public:
	XE::VEC2 m_vPos;		// 가리키는 위치
	float m_dAng = 0;		// 인디케이터의 방향각도
	_tstring m_strSpr;		// 인디케이터 spr
	ID m_idAct = 1;
	std::string m_idsRoot;	// 인디케이트를 특정 윈도우밑에 붙여야 한다면 그 식별자
	float m_secLife = 0;		// 일정시간후에 자동종료되어야 한다면..
public:
	XPropIndicate() : XPropOrder( xHO_INDICATE ) { Init(); }
	virtual ~XPropIndicate() { Destroy(); }
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	void Serialize( XArchive& ar ) const override;
	void DeSerialize( XArchive& ar, int ) override;
private:
	void Init() {}
	void Destroy() {}
}; // class XPropIndicate


/****************************************************************
* @brief 대화창용 프로퍼티
* @author xuzhu
* @date	2015/06/22 16:15
*****************************************************************/
class XPropDialog : public XPropOrder
{
public:
	_tstring m_strTalker;	// 대화자의 식별자
	ID m_idText = 0;		// id형태로 가지고 있을때
	XE::VEC2 m_vPos;		// 좌표를 직접 지정하고플때,
	bool m_bSavePos = false;	// 한번사용한 좌표를 다음 대화창에도 계속 사용할것인지
	XVector<std::shared_ptr<XPropIndicate>> m_aryIndicate;	// 인디게이터를 가질때 그 어레이
public:
	XPropDialog() : XPropOrder( xHO_DIALOG ), m_vPos(-1) { Init(); }
	virtual ~XPropDialog() { Destroy(); }
	//
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	_tstring GetstrText() {
		if( m_idText )
			return XTEXT(m_idText);
		return _m_strText;
	}
	void SetstrText( LPCTSTR szText ) {
		_m_strText = szText;
	}
	void Serialize( XArchive& ar ) const override;
	void DeSerialize( XArchive& ar, int ) override;
private:
	_tstring _m_strText;		// 대화 단락
	void Init() {}
	void Destroy() {}
}; // class XPropDialog

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 15:00
*****************************************************************/
class XPropCamera : public XPropOrder
{
public:
	XE::VEC2 m_vwDst;	// 카메라 포커스가 이동해야할 월드 좌표
	bool m_bPushPos = false;		// 카메라 이동전 카메라 좌표를 백업 받아두고 나중에 되돌아갈때 사용.
	bool m_bPopPos = false;			// pop_pos명령
public:
	XPropCamera() : XPropOrder( xHO_CAMERA ) { Init(); }
	virtual ~XPropCamera() { Destroy(); }
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	void Serialize( XArchive& ar ) const  override{
		XPropOrder::Serialize( ar );
		ar << m_vwDst;
		ar << xboolToByte(m_bPushPos);
		ar << xboolToByte(m_bPopPos);
		ar << (short)0;
	}
	void DeSerialize( XArchive& ar, int ver )  override{
		XPropOrder::DeSerialize( ar, ver );
		BYTE b0;
		ar >> m_vwDst;
		ar >> b0;		m_bPushPos = xbyteToBool(b0);
		ar >> b0;		m_bPopPos = xbyteToBool(b0);
		ar >> b0 >> b0;
	}
private:
	void Init() {}
	void Destroy() {}
}; // class XPropCamera

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 19:09
*****************************************************************/
class XPropTouch : public XPropOrder
{
public:
	bool m_bFlag = false;
//	std::string m_idsAllow;		// 터치를 허용하는 윈도우의 식별자
public:
	XPropTouch() : XPropOrder( xHO_TOUCH ) { Init(); }
	virtual ~XPropTouch() { Destroy(); }
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	void Serialize( XArchive& ar ) const  override{
		XPropOrder::Serialize( ar );
		ar << xboolToByte(m_bFlag);
		ar << (char)0;
		ar << (short)0;
	}
	void DeSerialize( XArchive& ar, int ver )  override{
		XPropOrder::DeSerialize( ar, ver );
		BYTE b0;
		ar >> b0;		m_bFlag = xbyteToBool(b0);
		ar >> b0 >> b0 >> b0;
	}
private:
	void Init() {}
	void Destroy() {}
}; // class XPropTouch

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:09
*****************************************************************/
class XPropDelay : public XPropOrder
{
public:
	float m_secDelay = 0.f;
public:
	XPropDelay() : XPropOrder( xHO_DELAY ) { Init(); }
	virtual ~XPropDelay() { Destroy(); }
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	void Serialize( XArchive& ar ) const  override{
		XPropOrder::Serialize( ar );
		ar << m_secDelay;
	}
	void DeSerialize( XArchive& ar, int ver )  override{
		XPropOrder::DeSerialize( ar, ver );
		ar >> m_secDelay;
	}
private:
	void Init() {}
	void Destroy() {}
}; // class XPropDelay

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:09
*****************************************************************/
// class XPropReadyTouch : public XPropOrder
// {
// public:
// public:
// 	XPropReadyTouch() : XPropOrder( xHO_READY_TOUCH ) { Init(); }
// 	virtual ~XPropReadyTouch() { Destroy(); }
// 	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
// private:
// 	void Init() {}
// 	void Destroy() {}
// }; // class XPropReadyTouch

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:09
*****************************************************************/
class XPropMode : public XPropOrder
{
public:
	std::string m_strMode;
	bool m_bFlag = false;
public:
	XPropMode() : XPropOrder( xHO_MODE_CHANGE ) { Init(); }
	virtual ~XPropMode() { Destroy(); }
	bool Save( XPropSeq *pSeq, XEXmlNode& nodeOrder ) override;
	void Serialize( XArchive& ar ) const  override{
		XPropOrder::Serialize( ar );
		ar << m_strMode;
		ar << xboolToByte(m_bFlag);
		ar << (char)0;
		ar << (short)0;
	}
	void DeSerialize( XArchive& ar, int ver )  override{
		XPropOrder::DeSerialize( ar, ver );
		_tstring strTemp;
		BYTE b0;
		ar >> strTemp;		m_strMode = SZ2C( strTemp );
		ar >> b0;		m_bFlag = xbyteToBool(b0);
		ar >> b0 >> b0 >> b0;
	}
private:
	void Init() {}
	void Destroy() {}
}; // class XPropMode


XE_NAMESPACE_END;
