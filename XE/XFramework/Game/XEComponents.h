/********************************************************************
	@date:	2014/09/16 20:26
	@file: 	D:\xuzhu_work\Project\iPhone\XE\XFramework\Game\XEComponents.h
	@author:	xuzhu
	
	@brief:	게임객체에 쓸수 있는 깨알같은 콤포넌트들 모음
*********************************************************************/
#pragma once


/****************************************************************
* @brief 객체의 진영(camp)을 구분할수 있는 기능의 콤포넌트
* 상위28비트는 최대28개의 진영을 표현한다.
* 하위4비트는 최대4개의 편을 표현한다. 같은편끼리 비트를 공유한다. 비트가 모두 0이면 중립이다.
* 예를들어 아군2팀(우리편) 적A 2팀(적편) 적B 2팀(적편) 중립1팀이라면 총 7개의 진영이 되고 다음과 같이 표현할수 있다.
* 플레이어팀: 0001 0001 아군팀: 0010 0001. 하위B0비트를 공통으로 하고 있다.
* 적A1팀:    0100 0010 적B2팀:  1000 0010. 하위B1비트를 공통으로 하고 있다.
* 적B1팀:  01 0000 0010 적B2팀:  10 0000 0010. 하위B1비트를 공통으로 하고 있다.
* 중립팀:   100 0000 0000. 공통비트가 없으므로 중립이된다. 중립은 우리가 먼저 공격하지 않는이상 먼저 선제공격을 하지 않는다.
* @author xuzhu
* @date	2014/09/16 20:26
*****************************************************************/
class XECompCamp
{
public:
	/// 두파라메터는 같은편이 아닌가?
	static bool sIsNotFriendly( BIT bitCamp1, BIT bitCamp2 ) {
		return (bitCamp1 & bitCamp2) == 0;		// 공유하는 비트가 없으면 다른편
	}
	/// 두 파라메터는 같은편인가?
	static bool sIsFriendly( BIT bitCamp1, BIT bitCamp2 ) {
		return ( bitCamp1 & bitCamp2 ) != 0;		// 공유하는 비트가 있으면 같은편
	}
	/// 파라메터는 중립인가
	static bool sIsNeutrality( BIT bitCamp ) {
		return (bitCamp & 0x0F) == 0;		// 하위4비트가 모두 0인가
	}
	/// 파라메터의 편(Side) 정보만 얻는다.
	static BIT sGetSide( BIT bitCamp ) {
		return bitCamp & 0x0F;
	}
	/// 파라메터와 다른편 필터를 만든다.
	static BIT sGetOtherSideFilter( BIT bitCamp ) {
		return ~(bitCamp & 0x0F);
	}
	/// 중립필터를 얻는다.
	static BIT sGetNeutralitySideFilter( void ) {
		return 0;		// 하위4비트가 모두 0
	}
private:
	BIT m_bitCamp;
	void Init() {
		m_bitCamp = 0;
	}
	void Destroy() {}
public:
	XECompCamp() { Init(); }
	virtual ~XECompCamp() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( BIT, bitCamp );
	//
	template<typename T>
	bool operator == ( T bitCamp ) const {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return m_bitCamp == (BIT)bitCamp;
	}
	template<typename T>
	bool operator != ( T bitCamp ) const {
		return m_bitCamp != bitCamp;
	}
	operator BIT() const {
		return m_bitCamp;
	}
	/**
	 @brief this와 비교해서 pDst는 적인가?
	 공유되는 비트가 없으면 적으로 간주. 
	*/
	bool IsEnemy( BIT bitDst ) const {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return (m_bitCamp & bitDst) == 0;		// 
	}
	/**
	 @brief this와 비교해서 bitDst는 우호적인 편인가
	 공유되는 비트가 있으면 같은편으로 본다.
	*/
	bool IsFriendly( BIT bitDst ) {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return ( m_bitCamp & bitDst ) != 0;
	}
	/**
	 @brief this가 bitDst와 같은 진영인가
	*/
	bool IsSameSide( BIT bitDst ) {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return ( m_bitCamp == bitDst );
	}
	bool IsSameSide( XECompCamp& camp ) {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return ( m_bitCamp == camp.GetbitCamp() );
	}
	/**
	 @brief this가 bitDst와 다른진영인가
	 적인가 아군인가를 구분하는게 아님을 주의.
	*/
	bool IsDifferentSide( DWORD bitDst ) {
		XBREAK( m_bitCamp == 0 );	// 초기화값이 있으면 안된다.
		return ( m_bitCamp != bitDst );
	}
};

/****************************************************************
* @brief 각종 비트처리용 공용클래스
* @author xuzhu
* @date	2015/05/22 19:29
*****************************************************************/
class XECompBit
{
	BIT m_Bit = 0;
	void Init() {}
	void Destroy() {}
public:
	XECompBit() { Init(); }
	virtual ~XECompBit() { Destroy(); }
	// 해당비트(bit)가 1인지
	inline bool IsBit( BIT bit ) {
		return (m_Bit & bit) != 0;
	}
	// 해당비트(bit)가 1이 아닌지
	inline bool IsNotBit( BIT bit ) {
		return !IsBit( bit );
	}
	inline bool GetBit( BIT bit ) {
		return IsBit( bit );
	}
	// 해당비트(bit)를 1로 만든다.
	inline void SetBit( BIT bit ) {
		m_Bit |= bit;
	}
	// 해당비트(bit)를 0으로 만든다.
	inline void ClearBit( BIT bit ) {
		m_Bit |= (~bit);
	}
	inline void Clear() {
		m_Bit = 0;
	}
	inline void Set( BIT bits ) {
		m_Bit = bits;
	}
	inline BIT Get() {
		return m_Bit;
	}
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar, int ver = 0 );
}; // class XECompBit


/****************************************************************
* @brief 컴포넌트 베이스 클래스
* @author xuzhu
* @date	2016/02/02 13:09
*****************************************************************/
class XEComponent
{
public:
// 	XEComponent( const std::string& strIdentifier ) 
// 		: m_strIdentifier(strIdentifier) { 
// 		Init(); 
// 	}
	/**
	 @brief 
	 @param strcFunc 컴포넌트의 기능을 기술한다. ex)"alpha"
	*/
	XEComponent( const std::string& strcFunc )
		: m_strFunc(strcFunc) {
		Init();
	}
	virtual ~XEComponent() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR_CONST( const std::string&, strIdentifier );
	GET_ACCESSOR_CONST( const std::string&, strFunc );
// 	GET_BOOL_ACCESSOR( bActive );

	// public member
	virtual void Process( float dt ) {}
	virtual float GetValue() const = 0;
private:
	// private member
	std::string m_strIdentifier;		// 컴포넌트의 고유 식별자(검색이 필요치 않다면 식별자는 없어도 된다)
	std::string m_strFunc;					// 컴포넌트가 하는일 ex) "alpha"
// 	bool m_bActive = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XEComponent

/****************************************************************
* @brief 나타났다 사라졌다 하는 류의 움직임에 사용하는 컴포넌트
* @author xuzhu
* @date	2016/02/02 13:10
*****************************************************************/
class XECompWave;
class XDelegateCompWave
{
public:
	XDelegateCompWave() {}
	virtual ~XDelegateCompWave() {}
	// get/setter
	// public member
	virtual void DelegateEndState( XECompWave *pComp, int state ) {}
}; // class XDelegateCompWave
////////////////////////////////////////////////////////////////
class XECompWave : public XEComponent
{
public:
// 	XECompWave( const std::string& strIdentifier, XDelegateCompWave *pDelegate ) 
// 	: XEComponent( strIdentifier), m_pDelegate(pDelegate) { 
// 		Init(); 
// 		m_State = -1;
// 	}
// 	XECompWave( const std::string& strIdentifier, XDelegateCompWave *pDelegate, float secDelay, float secFade ) 
// 		: XECompWave( strIdentifier, pDelegate ) {
// 		m_secDelay = secDelay;
// 		m_secFade = secFade;
// 	}
	XECompWave( const std::string& strFunc, XDelegateCompWave *pDelegate )
		: XEComponent( strFunc ), m_pDelegate( pDelegate ) {
		Init();
		m_State = -1;
	}
	XECompWave( const std::string& strFunc, XDelegateCompWave *pDelegate, float secDelay, float secFade )
		: XECompWave( strFunc, pDelegate ) {
		m_secDelay = secDelay;
		m_secFade = secFade;
	}
	virtual ~XECompWave() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR_CONST( float, minValue );
	GET_SET_ACCESSOR_CONST( float, maxValue );

	// public member
private:
	// private member
	XDelegateCompWave *m_pDelegate = nullptr;
	int m_State = 0;			// 0:없음 1:나타나는중 2:다나타남 3:사라지는중 
	CTimer m_timerFade;
	CTimer m_timerDelay;
	float m_Value = 0;		// wave움직임을 0~1로 표현한 숫자.
	float m_secFade = 1.f;		// 나타나거나 사라질때 시간
	float m_secDelay = 5.f;		// 한 상태를 유지하는 시간.
	float m_minValue = 0.f;
	float m_maxValue = 1.f;
private:
	// private method
	void Init() {}
	void Destroy() {}
	float GetValue() const override {
		return m_Value;
	}
	void Process( float dt ) override;
}; // class XECompWave

/****************************************************************
* @brief Fade In이나 Out처럼 한쪽으로 변하는류의 컴포넌트
* @author xuzhu
* @date	2016/04/12 15:48
*****************************************************************/
class XECompFade : public XEComponent
{
public:
	/**
	 @brief 
	 @param secFade 나타나거나 사라지는 시간
	*/
	XECompFade( const std::string& strFunc/*, XDelegateCompFade* pDelegate*/ ) 
	: XEComponent( strFunc ) { 
		Init(); 
// 		XBREAK( secFade <= 0 );
//		m_pDelegate = pDelegate;
	}
	virtual ~XECompFade() { Destroy(); }
	// get/setter
	// public member
	float GetValue() const override {
		return m_Value;
	}
	inline void DoStart( bool bFadeIn, float secFade = 1.f ) {
		m_bFadeIn = bFadeIn;
		OnStart();
	}
	inline void DoStartFadeIn( float secFade = 1.f ) {
		m_bFadeIn = true;
		m_secFade = secFade;
		OnStart();
	}
	inline void DoStartFadeOut( float secFade = 1.f ) {
		m_bFadeIn = false;
		m_secFade = secFade;
		OnStart();
	}
	void OnFinishFade( std::function<void()> func ) {
		m_funcFinish = func;
	}
private:
	// private member
//	XDelegateCompFade* m_pDelegate = nullptr;
	std::function<void()> m_funcFinish;
	float m_Value = 0;		// 0~1의 변화갑
	bool m_bFadeIn = false;		// in이면 true, out이면 false
	float m_secFade = 1.f;
	CTimer m_timerFade;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Process( float dt ) override;
	void OnStart() {
		m_timerFade.Set( m_secFade );
	}
	void OnStop() {
		m_timerFade.Off();
	}
}; // class XECompFade
