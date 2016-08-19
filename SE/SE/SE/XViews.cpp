#include "stdafx.h"
#include "XViews.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "ThumbView.h"
#include "ActionListView.h"
#include "AdjustView.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XViews::sUpdateFrameView()
{
	GetFrameView()->Update();
}
void XViews::sUpdateAnimationView()
{
	GetAnimationView()->Update();
}
void XViews::sUpdateThumbView()
{
	GetThumbView()->Update();
}
void XViews::sUpdateActionListView()
{
	GetActionListView()->Update();
}
void XViews::sUpdateAdjustView()
{
	GetAdjustView()->Update();
}

void XViews::sUpdateAllView()
{
	sUpdateFrameView();
	sUpdateAnimationView();
	sUpdateThumbView();
	sUpdateActionListView();
	sUpdateAdjustView();
}
