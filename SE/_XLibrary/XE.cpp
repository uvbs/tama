#include "stdafx.h"
#include "xe.h"
#include "xUtil.h"
// namespace XE
namespace XE
{
	ID m_idGlobal = 1;			// 다용도 고유 아이디
	//
	ID GenerateID() 
	{ 
		DWORD idRand = 0;
		while( idRand == 0 )		// 0은 생성하지 못하도록
			idRand = xRand();
		return idRand; 
	}
}


