//
//  TYShutterEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sat Dec 01 2001.
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

#import "TYShutterEffectGenerater.h"
#import "TYStageManager.h"

#define PIXEL_OF_COLUMN 16

@implementation TYShutterEffectGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    
    self = [ super init ];
    
    [ effect getValue:&effdef ];
    switch(effdef.type){
    case TYEffectTopShutter:
        reverse = YES;
        break;
    case TYEffectRightShutter:
        reverse = YES;
    case TYEffectLeftShutter:
        horizontal = YES;
        break;
    default:
        break;
    }

    phaseInterval = effdef.time / 1000.0 / PIXEL_OF_COLUMN;
    time = effdef.time;
    numberOfColumn = ((horizontal) ? VSCREEN_WIDTH : VSCREEN_HEIGHT ) / PIXEL_OF_COLUMN; 
    
    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
    
    afterImage = [ atImage copyWithZone:[ atImage zone ] ];
    nowImage = [ befImage copyWithZone:[ befImage zone ] ];
    
    startDate = [ [ NSDate alloc ] init ];
    
    return self;
    
} // override

-(void)drawEffect
{
    int x,y,w,h,mx,my;
    int phase,movePhase;
    int i;
    
    // 経過時間をピクセル単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *PIXEL_OF_COLUMN;
    if(phase >= PIXEL_OF_COLUMN){
        [ self changeEffectionImage:afterImage ];
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
        return;
    }
    
    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];

    movePhase = phase -beforePhase;
    // まあ、ありえないとは思うが。
    if(movePhase==0){
        return;
    }

    if(horizontal){
        w = movePhase;
        h = VSCREEN_HEIGHT;
        y = 0;
        my = 0;
        x = (reverse) ? VSCREEN_WIDTH -beforePhase -w : beforePhase;
        mx = (reverse) ? -PIXEL_OF_COLUMN : PIXEL_OF_COLUMN;
    } else {
        w = VSCREEN_WIDTH;
        h = movePhase;
        x = 0;
        mx =0;
        y = (reverse) ? VSCREEN_HEIGHT -beforePhase -h : beforePhase;
        my = (reverse) ? -PIXEL_OF_COLUMN : PIXEL_OF_COLUMN;
    }
    
    [ nowImage lockFocus ];
    for(i=0; i < numberOfColumn; i++,x+=mx,y+=my){
        [ afterImage compositeToPoint:NSMakePoint(x,y) fromRect:NSMakeRect(x,y,w,h) operation:NSCompositeCopy ];
    }
    [ nowImage unlockFocus ];
    
    [ self changeEffectionImage:nowImage ];

    beforePhase = phase;
    
    return ;
} // override

- (void)dealloc
{
    [ startDate release ];
    [super dealloc];
}

@end
