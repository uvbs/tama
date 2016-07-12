#pragma once
#ifdef __cplusplus
#include "etc/Token.h"
#include <map>
//using namespace std;

class XSoundTable;
extern XSoundTable *SOUND_TBL;

class XSoundTable
{
	ID m_maxID;
	std::map<ID, std::string> m_mapFile;	
//	std::map<ID, std::string>::iterator m_Itor;
	std::map<std::string, ID> m_mapByKey;		// 파일명(키)으로 ID를 검색. 임시조치
	void Init() {
		XBREAK( SOUND_TBL != NULL );
		SOUND_TBL = this;
		m_maxID = 0;
	}
	void Destroy();
public:
	XSoundTable() {	Init(); }
	virtual ~XSoundTable() { Destroy(); }
	// get set
	GET_ACCESSOR_CONST( ID, maxID );
	const char* GetFileFromID( ID idSound ) const;
	const char* Find( ID idSound ) const;
	ID Find( const std::string& strKey ) const;
	//
	BOOL Load( LPCTSTR szFile );
	void Reload();
//	void GetNextClear( void ) {
//		m_Itor = m_mapFile.begin();
//	}
// 	const char* GetNext( void ) {
// 		if( m_mapFile.end() == m_Itor )
// 			return NULL;
// 		return ((*(m_Itor++)).second).c_str();
// 	}
};
#endif

#define XSOUND_FILE(IDSOUND)	((SOUND_TBL)? SOUND_TBL->GetFileFromID( IDSOUND ) : "")
