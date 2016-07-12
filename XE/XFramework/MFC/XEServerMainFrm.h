#pragma once

class XEServerMainFrame
{
public:
	static XEServerMainFrame* sGet() {
		return s_pMainFrm;
	}
private:
	static XEServerMainFrame *s_pMainFrm;
	CString m_strTitle;
	void Init() {}
	void Destroy() {}
public:
	XEServerMainFrame( LPCTSTR szName ) { 
		Init(); 
		m_strTitle = szName;
		s_pMainFrm = this;
	}
	virtual ~XEServerMainFrame() { Destroy(); }
	//
	void SetTitle();
	virtual CString GetstrTitle() const;
};
