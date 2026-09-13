//
//  TYCartainEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sun Dec 02 2001.
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

#import "TYCartainEffectGenerater.h"
#import "TYStageManager.h"

#define PIXEL_OF_COLUMN 24

@implementation TYCartainEffectGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    
    self = [ super init ];
    
    [ effect getValue:&effdef ];
    switch(effdef.type){
    case TYEffectTopCartain:
        reverse = YES;
        break;
    case TYEffectRightCartain:
        reverse = YES;
    case TYEffectLeftCartain:
        horizontal = YES;
        break;
    default:
        break;
    }

    if(horizontal) {
        numberOfColumn = VSCREEN_WIDTH / PIXEL_OF_COLUMN; 
        phaseMAX = PIXEL_OF_COLUMN +numberOfColumn -1;
        pointMAX = VSCREEN_WIDTH;
        remainder = VSCREEN_WIDTH % PIXEL_OF_COLUMN;
    } else {
        numberOfColumn = VSCREEN_HEIGHT / PIXEL_OF_COLUMN;
        phaseMAX = PIXEL_OF_COLUMN +numberOfColumn -1;
        pointMAX = VSCREEN_HEIGHT;
    }
    phaseInterval = effdef.time / 1000.0 / phaseMAX;
    time = effdef.time;
    
    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
    
    afterImage = [ atImage copyWithZone:[ atImage zone ] ];
    nowImage = [ befImage copyWithZone:[ befImage zone ] ];
    
    startDate = [ [ NSDate alloc ] init ];
    
    return self;
    
} // override

-(void)drawEffect
{
    int x,y,w,h;
    int phase,movePhase,currentPhase;
    int i,loc,*mp;
    
    // 経過時間をフェイズ単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *phaseMAX;
    if(phase >= phaseMAX){
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
        w = 1;
        h = VSCREEN_HEIGHT;
        y = 0;
        mp = &x;
    } else {
        w = VSCREEN_WIDTH;
        h = 1;
        x = 0;
        mp = &y;
    }
    
    [ nowImage lockFocus ];
    for(currentPhase=beforePhase+1; currentPhase <= phase; currentPhase++){
        for(i=0,loc=currentPhase -1; i < numberOfColumn; i++,loc--){
            if((loc >= 0)&&(loc < PIXEL_OF_COLUMN)){
                *mp = (reverse) ? pointMAX -PIXEL_OF_COLUMN *i -loc -1 : PIXEL_OF_COLUMN *i +loc;
                [ afterImage compositeToPoint:NSMakePoint(x,y) fromRect:NSMakeRect(x,y,w,h) operation:NSCompositeCopy ];
            }
        }
        // 余りがあればその処理
        if(remainder){
            if((loc >= 0)&&(loc < remainder)){
                *mp = (reverse) ? pointMAX -PIXEL_OF_COLUMN *i -loc -1 : PIXEL_OF_COLUMN *i +loc;
                [ afterImage compositeToPoint:NSMakePoint(x,y) fromRect:NSMakeRect(x,y,w,h) operation:NSCompositeCopy ];
            }
        }
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
