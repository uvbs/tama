#pragma once

class XExpTableUser
{
	XArrayLinear<DWORD> m_Table;
	void Init() {
	}
	void Destroy() {}
public:
	XExpTableUser() { Init(); }
	virtual ~XExpTableUser() { Destroy(); }
	//
	/**
	 @brief level레벨의 최대경험치를 구한다.
	*/
	DWORD GetMaxExp( int level ) {
		return m_Table[ level ];
	}
	/**
	 @brief 최대가능 레벨을 얻는다
	*/
	int GetMaxLevel( void ) {
		return m_Table.size() - 1;
	}
	//
	BOOL Load( LPCTSTR szFile );
};


extern XExpTableUser *EXP_TABLE_USER;

