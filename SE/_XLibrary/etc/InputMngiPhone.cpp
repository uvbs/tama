#include "InputMngiPhone.h"

XInputMngiPhone* XInputMngiPhone::s_pInputMngiPhone;

/*void XInputMngiPhone::UpdateMsgProc( UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_TTALK:
			m_dwInput |= INPUT_TALKDOWN;
			m_dwInput &= ~INPUT_TALKUP;
			break;
		case VK_TEND:
			m_dwInput |= INPUT_TENDDOWN;
			m_dwInput &= ~INPUT_TENDUP;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) 
		{
		case VK_TTALK:
			m_dwInput |= INPUT_TALKUP;
			m_dwInput &= ~INPUT_TALKDOWN;
			break;
		case VK_TEND:
			m_dwInput |= INPUT_TENDUP;
			m_dwInput &= ~INPUT_TENDDOWN;
			break;
		}
		break; 
	case WM_LBUTTONDOWN:
		m_dwInput |= INPUT_TOUCHDOWN;
		m_dwInput &= ~INPUT_TOUCHUP;
		m_nMouseX = LOWORD( lParam );
		m_nMouseY = HIWORD( lParam );
		break;
	case WM_LBUTTONUP:
		m_dwInput |= INPUT_TOUCHUP;
		m_dwInput &= ~INPUT_TOUCHDOWN;
		m_nMouseX = LOWORD( lParam );
		m_nMouseY = HIWORD( lParam );
		break;
	case WM_MOUSEMOVE:
		m_nMouseX = LOWORD( lParam );
		m_nMouseY = HIWORD( lParam );
		break;
	} // switch
 
}*/

