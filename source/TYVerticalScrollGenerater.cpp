//
//  TYVerticalScrollGenerater.m
//  Tukuyomi
//
//  Created by toveta on Fri Nov 30 2001.
//  Copyright (c) 2001 toveta All rights reserved.
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


#import "TYVerticalScrollGenerater.h"
#import "TYStageManager.h"

@implementation TYVerticalScrollGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect reverse:(BOOL)aBool
{
    TYEffectDefinition effdef;
    
    self = [ super init ];

    reverse = aBool;
    
    [ effect getValue:&effdef ];
    phaseInterval = effdef.time / 1000.0 / VSCREEN_HEIGHT;
    time = effdef.time;

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
    
    unionImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT*2) ];    
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    [ unionImage lockFocus ];
        [ befImage compositeToPoint:NSMakePoint(0,VSCREEN_HEIGHT *(reverse)) operation:NSCompositeCopy ];
        [ atImage compositeToPoint:NSMakePoint(0,VSCREEN_HEIGHT *(!reverse)) operation:NSCompositeCopy ];
    [ unionImage unlockFocus ];
    
    startDate = [ [ NSDate alloc ] init ];
    
    return self;
}

// private method
-(void)drawEffect
{
    int phase;
    // 経過時間を480分割単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *VSCREEN_HEIGHT;
    if(phase >= VSCREEN_HEIGHT){
        // エフェクト終了
        [ nowImage lockFocus ];
            [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(0,(VSCREEN_HEIGHT *((reverse) ? -1 : 1)) + VSCREEN_HEIGHT *(reverse),VSCREEN_WIDTH,VSCREEN_HEIGHT) operation:NSCompositeCopy ];
        [ nowImage unlockFocus ];
        [ self changeEffectionImage:nowImage ];
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
    } else {
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];

        [ nowImage lockFocus ];
            [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(0,(phase *((reverse) ? -1 : 1)) + VSCREEN_HEIGHT *(reverse),VSCREEN_WIDTH,VSCREEN_HEIGHT) operation:NSCompositeCopy ];
        [ nowImage unlockFocus ];
        
        [ self changeEffectionImage:nowImage ];
    }

    return;
} // override

- (void)dealloc
{
    [ startDate release ];
    [unionImage release];
    [ nowImage release ];
    [super dealloc];
}

@end
