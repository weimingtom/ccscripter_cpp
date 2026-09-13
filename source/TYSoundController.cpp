//
//  TYSoundController.m
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

#import <AppKit/AppKit.h>
#import "TYSoundController.h"
#import "TYResourceServer.h"
//#import "TYWaveBooster.h"
#import "TYWave2Aiff.h"

NSString* const TYSoundStartNotification=@"TYSoundStartNotification";
NSString* const TYSoundFinishNotification=@"TYSoundFinishNotification";

@implementation TYSoundController

//static BOOL postingNotification;

/*
+(void)setPostingNotification:(BOOL)aBool
{
    postingNotification = aBool;
}
*/
-(id)initWithResource:(NSString*)path loop:(BOOL)aBool
{
    return [ self initWithResource:path loop:aBool volume:TYSOUND_VOLUME_MAX ];
}

-(id)initWithResource:(NSString*)path loop:(BOOL)aBool volume:(int)volume
{
    NSData *data;
    self = [ super init ];
    
    isLoop = aBool;
    
    if(volume > TYSOUND_VOLUME_MAX)
        volume = TYSOUND_VOLUME_MAX;
    if(volume < 0)
        volume = 0;
    /*
    if(volume==TYSOUND_VOLUME_MAX)
        data = [ [ TYResourceServer sharedServer ] getSoundData:path ];
    else
        data = TYWaveBoost([ [ TYResourceServer sharedServer ] getSoundData:path ],volume);
     */
    
    data = TYWave2Aiff([ [ TYResourceServer sharedServer ] getSoundData:path ],volume);
    //TYVerifyAiff(data);

    // NOTE:Mac OS X 10.3のNSSoundのstopメソッドにはオブジェクトの参照カウントを減らしてしまう致命的なバグがあるため、
    // ここで複数回のretainを行う。
    if(data)
        playSound = [ [ [ [ NSSound alloc ] initWithData:data ] retain ] retain ];

    [ playSound setDelegate:self ];

    soundPath = [ path retain ];

    return self;
}

-(void)play
{
    BOOL aBool;
    
    if(postingNotification && (!isLoop) && playSound)
        [ [ NSDistributedNotificationCenter defaultCenter ] postNotificationName:TYSoundStartNotification
                                                               object:nil ];

    aBool = [ playSound play ];
    if(!aBool)
        NSLog(@"can't play wave file. path=%@",soundPath);
}

-(void)setPostingNotification:(BOOL)aBool
{
    postingNotification = aBool;
}

-(NSString*)soundPathIfNeedPlayingAtLoad
{
    if(playSound){
        if([ playSound isPlaying ]&&isLoop){
            return soundPath;
        }
    }

    return nil;
}

/*
-(void)playIfPlaying
{
    if(isPlaying)
        [ playSound play ];
}
*/

-(void)stop
{
    if(postingNotification && (!isLoop))
        [ [ NSDistributedNotificationCenter defaultCenter ] postNotificationName:TYSoundFinishNotification
                                                               object:nil ];

        [ playSound stop ];
    
}

// サウンド演奏終了時のdelegate?
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)aBool
{
    if(aBool && isLoop){
        [ sound play ];
    } else {
        if(postingNotification)
            [ [ NSDistributedNotificationCenter defaultCenter ] postNotificationName:TYSoundFinishNotification
                                                                   object:nil ];
    }
}

- (void)dealloc
{
    int rc = [playSound retainCount ];
    int i;
    
    if (rc != 3) {
        NSLog(@"incorrect retain count !! = %d",rc);
    }
    if (rc > 3) {
        rc = 3;
    }
    for (i=rc; i>0; i--) {
        [playSound release];
    }

    isLoop = NO;
    [soundPath autorelease];
    soundPath = nil;
    [super dealloc];
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------
/*
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    isPlaying=([ playSound isPlaying ] && isLoop ) ? YES : NO;
    
    [ aCoder encodeObject:soundPath ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&isLoop ];
    [ aCoder encodeValueOfObjCType:@encode(BOOL) at:&isPlaying ];

}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    soundPath = [[aDecoder decodeObject] copy];
    [ aCoder decodeValueOfObjCType:@encode(BOOL) at:&isLoop ];
    [ aCoder decodeValueOfObjCType:@encode(BOOL) at:&isPlaying ];
    
    return self;
}
*/

@end
