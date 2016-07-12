//
//  AVAudio.cpp
//  Game
//
//  Created by 성정우 on 11. 6. 1..
//  Copyright 2011 LINKS CO.,LTD. All rights reserved.
//
// AVFoundation(AVAudioPlayer 사용)
#import <AVFoundation/AVFoundation.h>
#include "AVAudio.h"

AVAudioPlayer *player = nil;

void AVLoad( LPCTSTR szFilename, BOOL repeat )
{
	if( player ) {
		[player stop];
//a		[player release];
		player = nil;
	}
    NSString *strPath = [NSString stringWithUTF8String:szFilename];
	NSURL *fileURL = [[NSURL alloc] initFileURLWithPath: strPath ];
	
//	NSURL *fileURL = [[NSURL alloc] initFileURLWithPath: [[NSBundle mainBundle] pathForResource:@"bgm1" ofType:@"mp3"]];
	player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];	
	if (player)
	{
		player.numberOfLoops = (repeat==TRUE)? 999 : 0;
//		player.delegate = self;
		[player prepareToPlay];
	}
}

void AVPlay( void )
{
	[player play];
}

void AVSetVolume( float vol )
{
	player.volume = vol;
}

void AVStop( void )
{
	[player pause];
}

void AVDestroy( void )
{
	if( player ) {
//a		[player release];
		player = nil;
	}
}
