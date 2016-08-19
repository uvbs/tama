#pragma once
#include "Select.h"

class XBaseUndo : public XBaseElem
{
private:
	CString m_strText;
	void Init() {}
	void Destroy() {}
protected:
public:
	XBaseUndo( LPCTSTR szText ) { Init(); m_strText = szText; }
	virtual ~XBaseUndo() { Destroy(); }

	virtual void SetValueAfter( void ) { XBREAKF( 1, "구현되지 않았습니다" ); }
	virtual BOOL DoUndo( void ) = 0;
	virtual BOOL DoRedo( void ) = 0;
	virtual const CString& GetText( void ) { return m_strText; }
	void SetText( LPCTSTR szText ) { m_strText = szText; }
};

