/********************************************************************
	@date:	2016/08/11 13:09
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XComp.h
	@author:	xuzhu
	
	@brief:	전투씬 오브젝트용 각종 콤포넌트
*********************************************************************/
#pragma once

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/05/28 17:03
*****************************************************************/
class XCompObjBase
{
public:
	XCompObjBase() {
		Init();
	}
	virtual ~XCompObjBase() {
		Destroy();
	}
	// get/setter
	// public member
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XCompObjBase

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/05/28 17:02
*****************************************************************/
class XCompObjFont : public XCompObjBase
{
public:
	XCompObjFont();
	~XCompObjFont() {
		Destroy();
	}
	// get/setter
	GET_SET_ACCESSOR_CONST( const _tstring&, strText );
	GET_SET_ACCESSOR_CONST( xFONT::xtStyle, Style );
	GET_SET_ACCESSOR_CONST( XCOLOR, Col );
	// public member
	bool Load( LPCTSTR szFont, float size );
	void Draw( const XE::VEC2& vPos, float scale );
private:
	// private member
	XBaseFontDat *m_pfdFont = nullptr;
	_tstring m_strText;
	xFONT::xtStyle m_Style = xFONT::xSTYLE_NORMAL;
	XCOLOR m_Col = XCOLOR_WHITE;
private:
	// private method
	void Init() {}
	void Destroy();
}; // class XCompObjFont


/****************************************************************
* @brief 객체 이동에 관한 기본클래스
* @author xuzhu
* @date	2016/08/11 13:25
*****************************************************************/
class XCompObjMove : public XCompObjBase
{
protected:
	enum xtState {
		xST_NONE = 0,
		xST_DISAPPEAR = 99,
		xST_DESTROYED = 100,
	};
public:
	XCompObjMove();
	virtual ~XCompObjMove() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR_CONST( int, State );
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vwPos );
	inline const XE::VEC3& AddvwPos( const XE::VEC3& vwPos ) {
		m_vwPos += vwPos;
		return m_vwPos;
	}
	// public member
	virtual int FrameMove( float dt ) = 0;		// pure
	inline bool IsDisappear() const {
		return m_State == xST_DISAPPEAR;
	}
	inline bool IsDestroy() const {
		return m_State == xST_DESTROYED;
	}
	virtual float GetLerpTime() const { 
		return m_timerState.GetSlerp(); 
	}
	GET_ACCESSOR( CTimer&, timerState );
	const CTimer& GettimerStateConst() const {
		return m_timerState;
	}
protected:
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vwDelta );
	inline const XE::VEC3& AddvwDelta( const XE::VEC3& vwDelta ) {
		m_vwDelta += vwDelta;
		return m_vwDelta;
	}
private:
	// private member
	int m_State = 0;		// 0:튀어오름 1:바닥에 떨어져있음 2:사라지는중
	XE::VEC3 m_vwDelta;		// gravity값 더해지기전 값
	XE::VEC3 m_vwPos;		// 현재 객체의 위치. FrameMove()전에 세팅되어야 한다.
	CTimer m_timerState;		// 각 상태중의 진행시간
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XCompObjMove

//////////////////////////////////////////////////////////////////////////
/**
 @brief 제자리에서 올라가는 움직임
*/
class XCompObjMoveNormal : public XCompObjMove {
public:
	XCompObjMoveNormal( const XE::VEC3& vwDelta );
	~XCompObjMoveNormal() {		Destroy();	}
	// get/setter
	// public member
	int FrameMove( float dt );
	float GetLerpTime() const override {
		return GettimerStateConst().GetSlerp();
	}
private:
	// private member
//	CTimer m_timerLife;
	float m_Alpha = 1.f;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XCompObjMoveNormal

/****************************************************************
* @brief 바닥에 통통튀는 움직임.
* @author xuzhu
* @date	2016/05/28 17:02
*****************************************************************/
class XCompObjMoveBounce : public XCompObjMove {
public:
	// ex:power=20.f dAngZ=280.f
	XCompObjMoveBounce( const float power, const float dAngZ );
	XCompObjMoveBounce( float power, const XE::VEC2& vdAngZRange )
		: XCompObjMoveBounce( power, xRandomF( vdAngZRange.v1, vdAngZRange.v2 ) ) {}
	XCompObjMoveBounce( const XE::VEC2& vPowerRange, const XE::VEC2& vdAngZRange )
		: XCompObjMoveBounce( xRandomF( vPowerRange.v1, vPowerRange.v2 )
		, xRandomF( vdAngZRange.v1, vdAngZRange.v2 ) ) {}
	~XCompObjMoveBounce() {		Destroy();	}
	// get/setter
//	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vwPos );
//	GET_ACCESSOR_CONST( int, State );
	SET_ACCESSOR( float, Gravity );
	// public member
	int FrameMove( float dt );
private:
	// private member
//	int m_State = 0;		// 0:튀어오름 1:바닥에 떨어져있음 2:사라지는중
	//	XE::VEC3 m_vwDelta;		// gravity값 더해지기전 값
// 	XE::VEC3 m_vwDeltaNext;
// 	XE::VEC3 m_vwPos;		// 현재 객체의 위치. FrameMove()전에 세팅되어야 한다.
	bool m_bBounce = true;	// 바닥에 떨어지면 다시 튀어오를건지.
	float m_Gravity = 1.f;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XCompObjMoveBounce
