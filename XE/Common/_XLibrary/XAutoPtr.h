#pragma once

template<typename T>
class XAutoPtr
{
	T m_ptr;
	void Init() {
		m_ptr = NULL;
	}
	void Destroy() {
		if( m_ptr )
			delete m_ptr;
		m_ptr = NULL;
	}
public:
	XAutoPtr() { Init(); }
	XAutoPtr( T ptr ) { 
		Init(); 
		m_ptr = ptr;
	}
	virtual ~XAutoPtr() { Destroy(); }
	//
	GET_SET_ACCESSOR(T, ptr );
};
