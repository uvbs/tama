#pragma once

class XFLevel;

class XDelegateLevel
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateLevel() { Init(); }
	virtual ~XDelegateLevel() { Destroy(); }
	// 레벨업을 하려면 아래 2개 델리게이트를 구현해야 한다.
	///< level에서의 최대 exp의 값을 돌려줘야 한다.
	virtual DWORD OnDelegateGetMaxExp( const XFLevel* pLevel, int level, DWORD param1, DWORD param2 ) const { return 0xffffffff; }
	///< 최대 레벨값을 돌려줘야 한다.
	virtual int OnDelegateGetMaxLevel( const XFLevel* pLevel, DWORD param1, DWORD param2 ) const { return 0; }
	///< 레벨업 했을때 이벤트
	virtual void OnDelegateLevelUp( XFLevel *pLevel ) {}
	///< 레벨다운이 되서 레벨 바닥까지 도달했다.
	virtual void OnDelegateLevelBottom( XFLevel *pLevel ) {}
	
};
