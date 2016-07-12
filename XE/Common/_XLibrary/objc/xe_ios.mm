/*
 *  civmm.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 8..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#import <UIKit/UIKit.h>
#import <mach/mach.h>
#import <mach/mach_host.h>
#import "xe_ios.h"
#import "objc/XEViewController.h"
#include "XApp.h"
#ifdef _VER_IOS
#undef min
#undef max
#endif

namespace IOS
{
    int x_idEditField = 0;
	id x_idController = nil;
	void SetController( id idController )
	{
		x_idController = idController;
	}
	id GetController( void )
	{
		return x_idController;
	}
    void DoModalEditBox( int idEditField )
    {
        x_idEditField = idEditField;
        [x_idController DoModalEditBox];
//        [g_pViewController DoModalEditBox];
    }
	
    void DoEditBox( float x, float y, float w, float h, XWndEdit *pWndHander )
    {
        [x_idController DoEditBox:x y:y w:w h:h handler:pWndHander];
//        [g_pViewController DoEditBox:x y:y w:w h:h handler:pWndHander];
    }
	// In-App Purchase
	void SendReqPurchase( const char *cSkuProduct )
	{
		NSString *strSku;
		strSku = [NSString stringWithUTF8String:cSkuProduct];
		[x_idController sendRequestPurchase:strSku];
	}
	// Facebook
	void DoLoginFacebook( void )
	{
		[x_idController DoLoginFacebook];
	}
	void DoLoginIni3( void )
	{
		[x_idController DoLoginIni3];
	}
	void nativeOnResultAuthenFacebook( const char *cUserId, const char *cUserName )
	{
		XE::xRESULT_AUTHEN *pResult = new XE::xRESULT_AUTHEN;
		if( cUserId )
			pResult->string[0] = cUserId;
		else
			pResult->string[0] = "";
		if( cUserName )
			pResult->string[1] = cUserName;
		else
			pResult->string[1] = "";
		XE::OnResultLoginAuthen( pResult );
	}
	//
	BOOL GetFreeMem( MEM_INFO *pOut )
	{
		mach_port_t host_port;
		mach_msg_type_number_t host_size;
		vm_size_t pagesize;
		
		host_port = mach_host_self();
		host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
		host_page_size(host_port, &pagesize);
		vm_statistics_data_t vm_stat;
		if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
//			NSLog(@"Failed to fetch vm statistics");
			return FALSE;
		/* Stats in bytes */
		natural_t mem_used = (vm_stat.active_count +
							  vm_stat.inactive_count +
							  vm_stat.wire_count) * pagesize;
		natural_t mem_free = vm_stat.free_count * pagesize;
		natural_t mem_total = mem_used + mem_free;
		pOut->used = mem_used;
		pOut->free = mem_free;
		pOut->total = mem_total;
//		NSLog(@"used: %u free: %u total: %u", mem_used, mem_free, mem_total);
		return TRUE;
	}
	class XAutoSleepMode
	{
	public:
		BOOL m_oldSystem;
		XAutoSleepMode() {
			m_oldSystem = [UIApplication sharedApplication].idleTimerDisabled;
			NSLog(@"current AutoSleepMode=%d", m_oldSystem);
		}
		virtual ~XAutoSleepMode() {
			NSLog(@"reset AutoSleepMode=%d", m_oldSystem);
			[UIApplication sharedApplication].idleTimerDisabled = m_oldSystem;
		}
	};
	static XAutoSleepMode objSleepMode;
	static BOOL _x_oldSleepMode = YES;
	// 자동으로 절전모드로 들어가는걸 조절한다. NO를 하면 자동으로 꺼지지 않는다. 디폴트값은 YES이다.
	void SetAutoSleepMode( BOOL bFlag )
	{
		_x_oldSleepMode = [UIApplication sharedApplication].idleTimerDisabled;
		if( bFlag )
			[UIApplication sharedApplication].idleTimerDisabled = NO;
		else
			[UIApplication sharedApplication].idleTimerDisabled = YES;
	}
	// SetAutoSleepMode()로 설정을 하면 기존값을 백업받는데 그 백업받은 플래그로 복구한다.
	void RestoreAutoSleepMode( void )
 	{
		if( _x_oldSleepMode )
			[UIApplication sharedApplication].idleTimerDisabled = YES;
		else
			[UIApplication sharedApplication].idleTimerDisabled = NO;
		_x_oldSleepMode = YES;	// default
	}
};

