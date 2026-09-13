//
//  TYQuakeEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Tue Dec 11 2001.
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

#import "TYQuakeEffectGenerater.h"
#import "TYStageManager.h"

#define SHAKE_SIZE 12

@implementation TYQuakeEffectGenerater
-(id)initWithImage:(NSImage*)image quakeType:(int)type times:(int)times time:(int)second
{
    self = [ super init ];
    
    afterImage = image;
    if(afterImage)
        [ afterImage retain ];

    quakeType = type;
    if(quakeType==TYQuakeTypeHorizontal){
        horizontal=YES;
    }
    quakeTimes = times;
    quakeSecond = second;
    timeOfShake = quakeSecond / quakeTimes;
    timeOfQuarter = timeOfShake >> 2;

    
    unionImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH +SHAKE_SIZE *times *horizontal *2,VSCREEN_HEIGHT +SHAKE_SIZE *times *(!(horizontal)) *2 ) ];
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    [ unionImage lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill( NSMakeRect(0,0,VSCREEN_WIDTH+SHAKE_SIZE *quakeTimes *2,VSCREEN_HEIGHT+SHAKE_SIZE *quakeTimes *2));
    [ afterImage compositeToPoint:NSMakePoint(SHAKE_SIZE *times *horizontal,SHAKE_SIZE *times *(!(horizontal))) operation:NSCompositeCopy ];
    [ unionImage unlockFocus ];
    
    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
    
    startDate = [ [ NSDate date ] retain ];
    
    return self;
}

-(void)drawEffect
{
    int restTime;
    int shakeLevel;
    int shakeQuarter;
    int x,y,*mp;
    int phaseOfQuarter;
    
    restTime = quakeSecond +[ startDate timeIntervalSinceNow ] *1000;
    shakeLevel = restTime / timeOfShake +1;
    if(restTime <= 0){
        [ self changeEffectionImage:afterImage ];
        [ self performSelector:@selector(quakeFinished) withObject:nil afterDelay:0 ];
        return;
    }

    if(horizontal){
        mp = &x;
        y=0;
    } else {
        mp = &y;
        x=0;
    }
    *mp = SHAKE_SIZE *quakeTimes;
    
    phaseOfQuarter = restTime % (timeOfQuarter);
    shakeQuarter = ( restTime / (timeOfQuarter) ) % 4;
    // １回分の揺れ時間をさらに4分割して、どの時点か求める。
    switch(shakeQuarter){
    case 1:
        *mp += SHAKE_SIZE *shakeLevel *phaseOfQuarter /timeOfQuarter ;
        break;
    case 2:
        *mp += SHAKE_SIZE *shakeLevel *(timeOfQuarter-phaseOfQuarter) /timeOfQuarter;
        break;
    case 3:
        *mp -= SHAKE_SIZE *shakeLevel *phaseOfQuarter /timeOfQuarter;
        break;
    case 0:
        *mp -= SHAKE_SIZE *shakeLevel *(timeOfQuarter -phaseOfQuarter) /timeOfQuarter;
        break;
    }

    [ nowImage lockFocus ];
    [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(x,y,VSCREEN_WIDTH,VSCREEN_HEIGHT) operation:NSCompositeCopy ];
    [ nowImage unlockFocus ];

    [ self changeEffectionImage:nowImage ];

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
}

- (void)dealloc
{
    [ startDate release ];
    [ unionImage release ];
    [super dealloc];
}

@end

@implementation TYEffectGenerater (TYQuakeExtension)
-(void)quakeFinished {
    if(delegate){
        if([ delegate respondsToSelector:@selector(quakeFinished) ]){
            [ delegate quakeFinished ];
        }
    }
}
@end;