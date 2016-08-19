#pragma once
class XSprite;
class XDragDrop
{
	BOOL m_bDrag;					// 드래그를 시작하면 1이된다.
	XSprite *m_pSpr;				// 드래그를 한 스프라이트
public:
	XDragDrop() { 
		m_bDrag = FALSE;
		m_pSpr = NULL;
	}
	~XDragDrop() {}

	BOOL GetDrag() { return m_bDrag; }
	void SetDrag( BOOL bDrag ) { m_bDrag = bDrag; }
	XSprite *GetSpr() { return m_pSpr; }
	void SetSpr( XSprite *pSpr ) { m_pSpr = pSpr; }

};
