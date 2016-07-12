#pragma once

class CToken;
class XEBaseProp
{
	int m_Ver;
	TCHAR m_szBuff[ 256 ];
	_tstring m_strFilename;
	ID m_idKey = 0;							///< 생성될때마다 고유의 키값을 갖는다. 다른 객체에서 이 키값을 갖고 있다가 매니저가 재 로딩되었을 경우 키를 비교하여 다르면 포인터를 다시 읽어야 한다.
	void Init() {
		m_Ver = 0;
		m_szBuff[0] = 0;
    m_idKey = XE::GenerateID();
	}
	void Destroy() {}
public:
	XEBaseProp() { Init(); }
	virtual ~XEBaseProp() { Destroy(); }
	//
	GET_TSTRING_ACCESSOR( strFilename );
	GET_SET_ACCESSOR( int, Ver );
	GET_ACCESSOR( ID, idKey );
	//
	// 이 파일이 ver보다 같거나 높은가
	BOOL IsUpperVersion( int ver ) { 
		return (m_Ver >= ver)? TRUE : FALSE; 
	}	
	// 이 파일이 nVersion보다 버전이 낮은가
	BOOL IsLowerVersion( int ver ) { 
		return (m_Ver < ver)? TRUE : FALSE; 
	}	
	int TokenGetNumber( CToken& token );
	LPCTSTR GetTokenIdentifier( CToken& token );
	LPCTSTR TokenGetIdentifier( CToken& token ) {
		return GetTokenIdentifier( token );
	}
	LPCTSTR GetTokenString( CToken& token );
	LPCTSTR TokenGetString( CToken& token ) {
		return GetTokenString( token );
	}
	ID GetTokenID( CToken& token );
	ID TokenGetID( CToken& token ) {
		return GetTokenID( token );
	}
	BOOL Load( LPCTSTR szFilename, DWORD dwParam=0 );
	virtual int GetSize( void )=0;
	virtual BOOL ReadProp( CToken& token, DWORD dwParam )=0;
	virtual void OnDidFinishReadProp( CToken& token ) {}
	virtual void OnDidBeforeReadProp( CToken& token ) {}
};
