//
//  TYNew_QuakeEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sun Jun 23 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "TYNew_QuakeEffectGenerater.h"
#import "TYStageManager.h"


@implementation TYNew_QuakeEffectGenerater

-(id)initWithImage:(NSImage*)image amplitude:(int)aInt time:(int)t
{
    self = [ super init ];

    afterImage = image;
    if(afterImage)
        [ afterImage retain ];
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];

    unionImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH +aInt *QUEAKE_POWER *2,
                                                             VSCREEN_HEIGHT +aInt *QUEAKE_POWER *2) ];
    [ unionImage lockFocus ];
    [ image compositeToPoint:NSMakePoint(aInt *QUEAKE_POWER , aInt *QUEAKE_POWER) operation:NSCompositeCopy ];
    [ unionImage unlockFocus ];
    time = t;
    amplitude = aInt;

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];

    startDate = [ [ NSDate date ] retain ];

    return self;
    
}

-(void)drawEffect
{
    if(time +[ startDate timeIntervalSinceNow ] *1000 < 0){
        [ self changeEffectionImage:afterImage ];
        [ self performSelector:@selector(quakeFinished) withObject:nil afterDelay:0 ];
        return;
    }

    [ nowImage lockFocus ];
    [ unionImage compositeToPoint:NSZeroPoint fromRect:NSMakeRect(random() % (amplitude *QUEAKE_POWER *3) ,
                                                                  random() % (amplitude *QUEAKE_POWER *3),
                                                                  VSCREEN_WIDTH,
                                                                  VSCREEN_HEIGHT) operation:NSCompositeCopy ];
         
    [ nowImage unlockFocus ];

    [ self changeEffectionImage:nowImage ];

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
}

-(void)dealloc
{
    [ startDate release ];
    [ unionImage release ];
    [ super dealloc ];
}

@end
