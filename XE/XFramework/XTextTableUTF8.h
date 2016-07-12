#pragma once
#ifdef __cplusplus
#include "etc/Token.h"
#include <map>
using namespace std;

class XTextTableUTF8;
// UTF8 전용 텍스트 테이블(장차 XTextTable과 합쳐서 템플릿 버전으로 바뀔예정)
class XTextTableUTF8
{
	ID m_maxID;
	map<ID, const char*> m_mapText;	
	map<ID, const char*>::iterator m_Itor;
	void Init() {
		m_maxID = 0;
	}
	void Destroy();
public:
	// 테이블에 보관할때 어떤 인코딩타입으로 보관할건지 지정
	XTextTableUTF8() {	
		Init(); 
	}
	virtual ~XTextTableUTF8() { Destroy(); }
	// get set
	GET_ACCESSOR( ID, maxID );
	const char* GetText( ID id ) {
		if( id == 0 )
			return "";
		const char* szText = m_mapText[ id ];
		if( XBREAKF( szText == NULL, "UTF8 GetText(%d) is null", id ) )
			return "";
		return szText;
	}
	const char* Find( ID idText );
	//
	BOOL Load( LPCTSTR szFilename );
//	void GetNextClear( void ) {
//		m_Itor = m_mapText.begin();
//	}
	const char* GetNext( void ) {
		if( m_mapText.end() == m_Itor )
			return NULL;
		return (*(m_Itor++)).second;
	}
};
#endif

#define XU8TEXT(TABLE, id)	((TABLE)? TABLE->GetText( id ) : "")
