#pragma once;
// #include <stdio.h>
// #include <list>
// #include <map>
// #include "XKeyBase.h"
// #include "Select.h"
// #include "XGraphicsD3DTool.h"
// #include "Sprite.h"
// #include "SprDef.h"
// #include "XE.h"
// #include "SEFont.h"
// #include "XLua.h"
// using namespace std;
// 
// /*
// warning C4250: 'XKeyImagePos' : 우위에 따라 'XKeyImageLayer::XKeyImageLayer::IsSameLayer'을(를) 상속합니다.
// 1>        d:\homework\project\se\se\se\key.h(181) : 'XKeyImageLayer::IsSameLayer' 선언을 참조하십시오.*/
// //#pragma warning(disable:4250)		// 위 워닝 끔
// 
// class XBaseLayer;
// class XSprDat;
// class XSprObj;
// //class XAniAction;
// class XSprite;
// class XBaseKey;
// class XLua;
// 
// 
// 
// 
// 
// 
// 
// 
// 
// #define GET_NEXT_KEY_LOOP( A, K )	\
// 		XBaseKey *K;				\
// 		A->GetNextKeyClear();	\
// 		while( K = A->GetNextKey() )		\
// 
// //#pragma warning(default:4700)
// 

#define GET_NEXT_KEY_LOOP( A, K )	\
		XBaseKey *K;				\
		A->GetNextKeyClear();	\
		while( K = A->GetNextKey() )
