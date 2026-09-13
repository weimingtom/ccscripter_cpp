//
//  TYScrollEffectGenerater.m
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

#import "TYScrollEffectGenerater.h"
#import "TYStageManager.h"

@implementation TYScrollEffectGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    
    self = [ super init ];

    // 何故か座標が増加する方向にreverse=YESを指定していますが、これはプログラムを間違えてそういう風に作ってしまったからです。(藁
    [ effect getValue:&effdef ];
    switch(effdef.type){
    case TYEffectBottomScroll:
        reverse = YES;
        break;
    case TYEffectLeftScroll:
        reverse = YES;
    case TYEffectRightScroll:
        horizontal = YES;
        break;
    default:
        break;
    }

    phaseMAX = (horizontal) ? VSCREEN_WIDTH : VSCREEN_HEIGHT ;
    phaseInterval = effdef.time / 1000.0 / phaseMAX;
    time = effdef.time;
    
    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
    
    unionImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH *(horizontal+1),VSCREEN_HEIGHT *((!horizontal) +1) ) ];
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    [ unionImage lockFocus ];
    {
        int x,y;
        x = VSCREEN_WIDTH *(reverse) *(horizontal);
        y = VSCREEN_HEIGHT *(reverse) *(!horizontal);
        [ befImage compositeToPoint:NSMakePoint(x,y) operation:NSCompositeCopy ];
        x = VSCREEN_WIDTH *(!reverse) *(horizontal);
        y = VSCREEN_HEIGHT *(!reverse) *(!horizontal);
        [ atImage compositeToPoint:NSMakePoint(x,y) operation:NSCompositeCopy ];
    }
    [ unionImage unlockFocus ];
    
    startDate = [ [ NSDate alloc ] init ];
    
    return self;
    
} // override

-(void)drawEffect
{
    int x,y;
    int phase;
    
    // 経過時間をイメージの高さ幅のピクセル単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *phaseMAX;
    if(phase >= phaseMAX){
        // エフェクト終了
        x = VSCREEN_WIDTH *(!reverse) *(horizontal);
        y = VSCREEN_HEIGHT *(!reverse) *(!horizontal);
        [ nowImage lockFocus ];
            [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(x,y,VSCREEN_WIDTH,VSCREEN_HEIGHT) operation:NSCompositeCopy ];
        [ nowImage unlockFocus ];
        [ self changeEffectionImage:nowImage ];
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
    } else {
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];
        x = (VSCREEN_WIDTH *(reverse) +phase *((reverse) ? -1 : 1)) *(horizontal);
        y = (VSCREEN_HEIGHT *(reverse) +phase *((reverse) ? -1 : 1 ))*(!horizontal);
        [ nowImage lockFocus ];
            [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(x,y,VSCREEN_WIDTH,VSCREEN_HEIGHT) operation:NSCompositeCopy ];
        [ nowImage unlockFocus ];
        
        [ self changeEffectionImage:nowImage ];
    }
    
    return ;
} // override

- (void)dealloc
{
    [ startDate release ];
    [unionImage release];
    [super dealloc];
}

@end
