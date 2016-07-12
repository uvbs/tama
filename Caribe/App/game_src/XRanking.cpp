#include "stdafx.h"
#include "XRanking.h"

XRanking* RANKING_DB = NULL;
XRanking* XRanking::s_pRanking;

XRanking* XRanking::sCreateInstance( void ) 
{
  s_pRanking = new XRanking;
  return s_pRanking;
}

void XRanking::Init()
{	
	m_Ranklist.clear();
}

void XRanking::cleanrank()
{
	std::list<s_rank*>::iterator begin = m_Ranklist.begin();	

	while (begin != m_Ranklist.end())
	{		
		s_rank* ptr = (*begin);
		if (ptr)
		{
			delete ptr;
			ptr = NULL;
		}		
		begin++;
	}
	m_Ranklist.clear();
}

void XRanking::Serialize(XArchive& ar)
{
	const int size = (int)m_Ranklist.size();
	ar << size;
	XBREAK( size > 100 );
	
	std::list<s_rank*>::iterator begin = m_Ranklist.begin();
	
	while (begin != m_Ranklist.end())
	{		
		s_rank* ptr = (*begin);
		ar << ptr->nRank;
		ar << ptr->s_idaccount;
		ar << ptr->s_username; //NickName
		ar << ptr->s_ladderpoint;		
		begin++;
	}	
}

void XRanking::DeSerialize(XArchive& ar)
{
	DWORD dw1;
	ar >> dw1;

	XBREAK( dw1 > 100 );
	cleanrank();

	for (int n = 0; n < (int)dw1; n++)
	{
		DWORD dw2;
		DWORD rank = 0;
		_tstring strBuff;
		ar >> rank;
		s_rank* ptr = new s_rank;
		ar >> dw2;
		ptr->s_idaccount = dw2;
		ar >> strBuff;
		ptr->s_username = strBuff; //NickName
		ar >> dw2;
		ptr->s_ladderpoint = dw2;
		ptr->nRank = rank;
		m_Ranklist.push_back(ptr);
	}	
}

void XRanking::CreateDeSerialize(XArchive& ar)
{
	DWORD dw1;
	ar >> dw1;

	cleanrank();

	for (int n = 0; n < (int) dw1; n++)
	{		
		DWORD dw2;
		_tstring strBuff;

		s_rank* ptr = new s_rank;
		ar >> dw2; 
		ptr->s_idaccount = dw2;
		ar >> strBuff;
		ptr->s_username = strBuff; //NickName
		ar >> dw2; 
		ptr->s_ladderpoint = dw2;
		ptr->nRank = n + 1;
		
		m_Ranklist.push_back(ptr);
	}	
	int i = 0;
}

bool XRanking::UpdateRank()
{	
	if (m_timerGetRank.IsOver() || m_timerGetRank.IsOff())
	{
		m_timerGetRank.Set(17);
		return true;
	}
	return false;
}