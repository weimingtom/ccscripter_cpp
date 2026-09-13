//
//  TYSoundController.h
//  Tukuyomi
//
//  Created by toveta on Sun Oct 07 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
//
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY toveta ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL toveta OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>

#define TYSOUND_VOLUME_MAX 256

@class NSSound ;

// 効果音を扱うクラス。
@interface TYSoundController : NSObject {
    NSSound *playSound;
    NSString *soundPath;
    BOOL isLoop;
    BOOL postingNotification;
}
//+(void)setPostingNotification:(BOOL)aBool;

-(id)initWithResource:(NSString*)path loop:(BOOL)aBool;
-(id)initWithResource:(NSString*)path loop:(BOOL)aBool volume:(int)volume;
//-(void)setSound:(NSString*)soundPath loop:(BOOL)aBool;
-(void)play;
-(void)stop;
-(void)setPostingNotification:(BOOL)aBool;
-(NSString*)soundPathIfNeedPlayingAtLoad;

// サウンド演奏終了時のdelegate
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)aBool;

// 効果音設定変更時の通知を受ける。

//
extern NSString* const TYSoundStartNotification;
extern NSString* const TYSoundFinishNotification;

@end
