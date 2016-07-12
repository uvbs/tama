#pragma once
#include <map>
#include <string>

const int VER_CONSTANT = 1;


// "DefineObject.h"에서 정의된 #define들을 메모리에 가지고 있는다.
// 스크립트 처리를 할때 필요하다면 이것을 검색한다.
class XLua;
class XConstant 
{
public:
	enum { xERROR=0x7fffffff };
private:
	struct xCONSTANT
	{
		_tstring strName;
		int	value;
		_tstring strRemark;
		xCONSTANT() {
			value = 0;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};

	std::map<_tstring, xCONSTANT*> m_mapData;
	BOOL m_bFound;
	void Init() {
		m_bFound = FALSE;
	}
	void Destroy();
public:
	XConstant() { 	Init();	}
	XConstant( LPCTSTR szFilename ) { 
		Init(); 
		if( !Load( szFilename ) )
			XALERT( "%s를 찾을 수 없습니다", szFilename );
	}
	virtual ~XConstant();
	//
	BOOL IsFound( void ) {
		return m_bFound;
	}
	BOOL Load( LPCTSTR szFile );						// defineXXXX.h를 읽음
	BOOL LoadEnum( CToken& token );
	// szName을 키로해서 상수값을 찾는다.
	int GetValue( LPCTSTR szName ) {
		m_bFound = FALSE;;
		XBREAK( szName == NULL );
		XBREAK( XE::IsEmpty( szName ) == TRUE );
		_tstring str = szName;
		std::map<_tstring, xCONSTANT*>::iterator itor;
		itor = m_mapData.find( str );
		if( itor == m_mapData.end() )
			return xERROR;							// 못찾았으면 에러 리턴
		xCONSTANT *pConst = (*itor).second;
		XBREAK( pConst == NULL );			// 위에서 find로 검사했기때문에 NULL나와선 안됨.
		m_bFound = TRUE;
		return pConst->value;
	}
	int GetValue( const _tstring& strName ) {
		return GetValue( strName.c_str() );
	}
	void Add( LPCTSTR szName, int value, LPCTSTR szRemark=0 );
	int TokenGetNumber( CToken& token );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
#ifdef _XUSE_LUA
	void RegisterScript( XLua *pLua );
#endif
};

extern XConstant *CONSTANT;
