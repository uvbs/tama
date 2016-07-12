#pragma once

#include <Map>
#include "XLock.h"

class XRanking : public XLock
{
	static XRanking *s_pRanking;

public:
	static XRanking* sCreateInstance(void);
	static XRanking* sGet() {
		return s_pRanking;
	}
	struct s_rank
	{
		ID s_idaccount;
		int nRank;		
		int	s_ladderpoint;
		_tstring s_username; //NickName		
	};
public:
	XRanking(){ Init(); }
	~XRanking(){ cleanrank(); }

	void Init();
	void Serialize(XArchive& ar);
	void DeSerialize(XArchive& ar);			// 있는 랭킹을 이용한다.
	void CreateDeSerialize(XArchive& ar);	// 랭킹을 만들어 준다.

	int GetRankingSize(){ return (int)m_Ranklist.size(); }
	bool UpdateRank();
	GET_ACCESSOR(CTimer, timerGetRank);
private:	
	void cleanrank();
	std::list<s_rank*> m_Ranklist;
	DWORD m_dwlastGetTime = 0;
	CTimer m_timerGetRank;					//랭킹 확인용 타이머.
};

inline XRanking* GetRanking() {
	return XRanking::sGet();
}