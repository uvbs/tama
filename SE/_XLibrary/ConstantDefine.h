#ifndef		__OBJECTDEFINE_H__
#define		__OBJECTDEFINE_H__

//#include "Game.h"
//#include "../Game2DLib/GameLib.h"
//#include "fixed.h"
//#include "Sprite.h"
//#include "DefineObj.h"
//#include "OBSMng.h"
#include "Token.h"

//#define		MAX_DEFINE		1024
// "DefineObject.h"에서 정의된 #define들을 메모리에 가지고 있는다.
// 스크립트 처리를 할때 필요하다면 이것을 검색한다.

//
//	class CDefineObject
//
class CDefine
{
	enum { MAX_NAME=64, MAX_REMARK=256, MAX_DEFINE=1024 };
	struct DEFINE
	{
		TCHAR	m_strName[MAX_NAME];
		int			m_nValue;
		TCHAR	m_szRemark[MAX_REMARK];
	};

	int		m_nMaxDef;
	DEFINE	m_pList[ MAX_DEFINE ];

	void Init() {
		m_nMaxDef = 0; 
		memset( m_pList, 0, sizeof(m_pList) );
	}
public:
	CDefine() { 	Init();	}
	CDefine( LPCTSTR szFilename ) { 
		Init(); 
		if( !Load( szFilename ) )
			XALERT( "%s를 찾을 수 없습니다", szFilename );
	}
	~CDefine() {}

	GET_ACCESSOR( int, nMaxDef );
	XE::xRESULT	Load( LPCTSTR strFileName );			// defineObject.h를 읽음
	XE::xRESULT	LoadBIN( LPCTSTR szFileName );
//	int		Save( LPCTSTR szFileName );				// DefineObject.h를 쓴다.
	XE::xRESULT	SaveOBJECT( LPCTSTR szFileName );
	XE::xRESULT	SaveBIN( LPCTSTR szFileName );
	int		FindDefine( LPCTSTR szName )				// 구버전 szName으로 nVal바로 찾기
	{
		DEFINE	*pDefine = Find( szName );
		if( pDefine )
			return pDefine->m_nValue;
		else
			return -1;
	}		
	DEFINE	*Find( LPCTSTR szName );				// szName으로 찾기
	DEFINE	*Find( int nVal );					// nVal로 찾기
	DEFINE	*Find( LPCTSTR szName, int nVal );	// szName의 일부분과 nVal로 찾기
	DEFINE* Get( int idx ) { return m_pList + idx; }
	XE::xRESULT	AddDefine( LPCTSTR szDefine, int nVal, LPCTSTR szRemark = NULL );	// Define 추가
};


#endif

