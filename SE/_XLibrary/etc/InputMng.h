#ifndef __INPUTMNG_H__
#define __INPUTMNG_H__

#define INPUT_UP						0x00000001
#define INPUT_DOWN						0x00000002
#define INPUT_LEFT						0x00000004
#define INPUT_RIGHT						0x00000008
#define INPUT_TOUCHDOWN					0x00000010
#define INPUT_TOUCHUP					0x00000020

#define INPUT_UPMSG		( INPUT_TOUCHUP )

class XInputMng
{
public:
	static XInputMng *s_pInputMng;
protected:
	DWORD	m_dwInput;
	XE::VEC2 m_vPos;		// 마우스 좌표
	XE::VEC2 m_vPrevPos;	// 이전 마우스 좌표
//	int		m_nMouseX;
//	int		m_nMouseY;
//	int		m_nPrevX, m_nPrevY;
	
private:
	void _Init( void ) {
		XInputMng::s_pInputMng = this;
		m_dwInput = 0;
//		m_nMouseX = 0;
//		m_nMouseY = 0;
//		m_nPrevX = m_nPrevY = 0;
	}
	void _Destroy( void ) {}
public:
	XInputMng() { _Init(); }
	virtual ~XInputMng() { _Destroy(); }
//	GET_ACCESSOR( int, nPrevX );
//	GET_ACCESSOR( int, nPrevY );
	int GetnPrevX( void ) { return (int)m_vPrevPos.x; }
	int GetnPrevY( void ) { return (int)m_vPrevPos.y; }

	DWORD GetInput( void ) { return m_dwInput; }
	void SetInput( DWORD dwFlag ) { m_dwInput |= dwFlag; }
	void ClearInput( DWORD dwFlag ) { m_dwInput &= ~dwFlag; }
	int GetMouseX( void ) { return (int)m_vPos.x; }
	int GetMouseY( void ) { return (int)m_vPos.y; }
	const XE::VEC2& GetMousePos( void ) { return m_vPos; }

	void SetMousePos( float x, float y ) { SetMousePos( (int)x, (int)y ); }
	virtual void SetMousePos( int x, int y ) { m_vPos.x = (float)x; m_vPos.y = (float)y; }
	void SetPrevPos( int x, int y ) { m_vPrevPos.x = (float)x; m_vPrevPos.y = (float)y; }
	void ClearUpMsg( void );
};

#define INPUTMNG	XInputMng::s_pInputMng
#endif // __INPUTMNG_H__
