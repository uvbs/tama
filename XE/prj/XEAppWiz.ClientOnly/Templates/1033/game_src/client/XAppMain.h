#pragma once
#include "client/XClientMain.h"

// 게임의 논리적 해상도
#define XRESO_WIDTH		640
#define XRESO_HEIGHT	360

class XGame;
////////////////////////////////////////////////////////////////
class XAppMain : public XClientMain
{
public:
	static XAppMain* sCreate( XE::xtDevice device, int widthPhy, int heightPhy );
private:
	_tstring m_strUUIDFile;			// uuid파일명

	void Init() {
		m_strUUIDFile = _T("login.txt");
	}
	void Destroy();
public:
	XAppMain();
	virtual ~XAppMain() { Destroy(); }
	//
	GET_TSTRING_ACCESSOR( strUUIDFile );
	//
	void DidFinishCreate( void );
	void DidFinishInitEngine( void );
	XEContent* CreateGame( void );
	void Draw( void );
	void ConsoleMessage( LPCTSTR szMsg );
	void SaveCheat( FILE *fp );
	void LoadCheat( CToken& token );
	void OnError( XE::xtError codeError, DWORD p1, DWORD p2 );
	BOOL DoEditBox( TCHAR *pOutText, int lenOut );
};

extern XAppMain *XAPP;

