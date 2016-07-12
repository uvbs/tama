/*
 *  civmm.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 8..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

class XWndEdit;
void DoNearbyPeerPicker( void );

//
void DoGameCenter( void );
void DoSendMsg( char *msg );
void DoSendScore( int score );
void DoSendAchievement( LPCTSTR szIdentifier, float percent );
void ShowLeaderboard( void );
void ShowAchievementboard( void );
//
void SendRequestPurchase( ID idProduct );

namespace IOS {
    extern int x_idEditField;
    void DoEditBox( float x, float y, float w, float h, XWndEdit *pWndHander );
    void DoModalEditBox( int idEditField ) ;
	void SendReqPurchase( const char *cSkuProduct );
	void DoLoginFacebook( void );
	void DoLoginIni3( void );
	void nativeOnResultAuthenFacebook( const char *cUserId, const char *cUserName );
	struct MEM_INFO {
		DWORD used;
		DWORD free;
		DWORD total;
		MEM_INFO() {
			used = 0;
			free = 0;
			total = 0;
		}
	};
	BOOL GetFreeMem( MEM_INFO *pOut );
	void SetAutoSleepMode( BOOL bFlag );
	void RestoreAutoSleepMode( void );
};
