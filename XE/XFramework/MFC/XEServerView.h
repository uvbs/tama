#pragma once

class CView;
class XEServerView
{
public:
	static CView* sGetView() { return s_pView; }
private:
	static CView *s_pView;
	_tstring m_strInfo;
	//
	int m_numDestroy;
	void Init() {
		m_numDestroy = 0;
	}
	void Destroy();
public:
	XEServerView( CView *pView ) { 
		Init(); 
		XBREAK( pView == NULL );
		s_pView = pView;
	}
	virtual ~XEServerView() { Destroy(); }
	//
	GET_SET_ACCESSOR( int, numDestroy );
	//
//	void OnInitialUpdate( _tstring strINI=_T("") );
	void OnDraw( int nTextID );
	virtual void OnDrawBefore( _tstring& str ) {}
};


