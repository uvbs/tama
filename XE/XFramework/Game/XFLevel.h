#pragma once
#include "etc/Types.h"
#include "etc/Global.h"

class XDelegateLevel;
class XArchive;
class XFLevel
{
public:
	enum { XDEFAULT_MAX_EXP=0xffffffff };	// OnDelegateGetMaxExp의 디폴트값
private:
	ID m_idLevel = 0;		/// 식별자(필요할때만)
	DWORD m_Exp;
	int m_Level;
	BOOL m_bLevelUp;			///< 방금 레벨업 했음.
	DWORD m_expRemainByAfterLevelUp = 0;	///< 레벨업 하고 남은 exp. 자동레벨업 모드가 off상태에서만 작동한다.
	int m_lvLimit = 0;			// 한계레벨. 한계레벨이 지정되어있으면 그 레벨에 도달한 후부터는 경험치를 얻을 수 없다.
	XDelegateLevel *m_pDelegate;
	bool m_bAutoLevelup = true;			///< exp가 오버되면 자동으로 레벨업
	void Init() {
		m_Exp = 0;
		m_Level = 1;
		m_bLevelUp = FALSE;
		m_pDelegate = NULL;
	}
	void Destroy() {}
public:
	XFLevel() { Init(); }
	XFLevel( XDelegateLevel *pDelegate ) { 
		Init(); 
		XBREAK( pDelegate == NULL );
		m_pDelegate = pDelegate;
	}
	virtual ~XFLevel() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( DWORD, Exp );
	GET_SET_ACCESSOR_CONST( int, Level );
	GET_SET_ACCESSOR_CONST( XDelegateLevel*, pDelegate );
	GET_SET_ACCESSOR_CONST( BOOL, bLevelUp );
//	GET_SET_ACCESSOR( bool, bAutoLevelup );
	GET_SET_ACCESSOR_CONST( ID, idLevel );
	GET_SET_ACCESSOR_CONST( int, lvLimit );
	bool IsLvLimit() const {
		return m_Level >= m_lvLimit;
	}
	int GetMaxLevel() const;
	/**
	 @brief 꺼내는 동시에 클리어 한다.
	*/
	bool GetbLevelUpAndClear() {
		BOOL bUp = m_bLevelUp;
		m_bLevelUp = FALSE;
		return (bUp != 0);
	}
// 	bool IsAutoLevelup() {
// 		return m_bAutoLevelup;
// 	}
	bool IsFullExp() const ;
	//
	DWORD GetMaxExp( int level ) const;
	DWORD GetMaxExpCurrLevel() const {
		return GetMaxExp( m_Level );
	}
	XINT64 GetExpSum() const;
	BOOL AddExp( DWORD exp );
	BOOL AddExp( XINT64 exp );
	BOOL AddExp( int exp ) {
		XBREAK( exp < 0 );
		return AddExp( (DWORD)exp );
	}
	/// 렙업하기위해 남은 exp양
	DWORD GetExpRemain() const {
		return GetMaxExpCurrLevel() - GetExp();
	}
	BOOL SubExp( DWORD exp );
//	void AddLevelManual( int add );
	BOOL Serialize( XArchive& ar ) const;
	BOOL DeSerialize( XArchive& ar );
};

#define XFLEVEL_ACCESSOR(OBJ) \
	XFLevel& GetXFLevelObj() { return OBJ; } \
	int GetLevel() const { return OBJ.GetLevel(); } \
	void SetLevel( int level ) { XBREAK(level <= 0); OBJ.SetLevel( level ); } \
	DWORD GetExp() const { return OBJ.GetExp();	} \
	void SetExp( DWORD exp ) { OBJ.SetExp( exp );	} \
	BOOL AddExp( int add ) { return OBJ.AddExp( (DWORD)add ); } \
	BOOL AddExp( DWORD add ) { return OBJ.AddExp( add ); } \
	BOOL AddExp( XINT64 add ) { return OBJ.AddExp( add ); } \
	void SetpDelegateLevel( XDelegateLevel *pDelegate, DWORD idObj = 0 ) { OBJ.SetpDelegate( pDelegate ); OBJ.SetidLevel( idObj ); } \
	BOOL GetbLevelUp() const { return OBJ.GetbLevelUp(); } \
	int SerializeLevel( XArchive& ar ) const { return OBJ.Serialize( ar ); } \
	int DeSerializeLevel( XArchive& ar ) { return OBJ.DeSerialize( ar ); } \
	BOOL GetbLevelUpAndClear() { return OBJ.GetbLevelUpAndClear(); } \
	XINT64 GetExpSum() const { return OBJ.GetExpSum(); } \
	DWORD GetMaxExp( int level ) const { return OBJ.GetMaxExp( level ); } \
	DWORD GetMaxExpCurrLevel() const { return OBJ.GetMaxExpCurrLevel(); } \
	DWORD GetExpRemain() const { return OBJ.GetMaxExpCurrLevel() - OBJ.GetExp(); } \
	int GetMaxLevel() const { return OBJ.GetMaxLevel(); } \
	int GetlvLimit() const { return OBJ.GetlvLimit(); } \
	void SetlvLimit( int level ) { XBREAK(level <= 1); OBJ.SetlvLimit( level ); } \
	bool IsLvLimit() const { return OBJ.IsLvLimit(); } \


