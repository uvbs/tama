#include "stdafx.h"
#include "xe.h"
#include "xUtil.h"
// namespace XE
namespace XE
{
	ID m_idGlobal = 1;			// �ٿ뵵 ���� ���̵�
	//
	ID GenerateID() 
	{ 
		DWORD idRand = 0;
		while( idRand == 0 )		// 0�� �������� ���ϵ���
			idRand = xRand();
		return idRand; 
	}
}


