#pragma once
#include "XFramework/server/XServerMain.h"
#include "XGameCommon.h"

/**
 컨텐츠 데이타의 로딩등을 담당하는 객체
*/
class XGame : public XEContents, public XGameCommon
{
	static XGame *s_pGame;
public:
	static XGame* sCreateInstance( void );
	static XGame* sGet() {
		return s_pGame;
	}
private:
	void Init() {
	}
	void Destroy();
public:
	XGame();
	virtual ~XGame() { Destroy(); }
	//
private:
public:
	virtual void FrameMove( float dt );
	virtual void Create( void );
};

//extern XGame		*_GAME;

inline XGame* GetGame() {
	return XGame::sGet();
}
