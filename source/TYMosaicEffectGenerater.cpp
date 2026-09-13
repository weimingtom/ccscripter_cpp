//
//  TYMosaicEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Wed Nov 06 2002.
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

#import "TYMosaicEffectGenerater.h"
#import "TYStageManager.h"
#import "TYImageUtil.h"

#define PHASE_NUM 6

static const int blockSizeTable[2][PHASE_NUM] = { { 160,80,40,20,10,5 },
                                                  { 200,100,50,25,10,5 } };
                           
@implementation TYMosaicEffectGenerater

-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    
    self = [ super init ];
    
    [ effect getValue:&effdef ];
    phaseInterval = effdef.time / 1000.0 / PHASE_NUM;
    //time = effdef.time;

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];

    inToward = ( effdef.type == TYEffectMosaicIn ) ? YES : NO;
    if(inToward){
        sourceBitmap = [ [ NSBitmapImageRep alloc ] initWithData:[ atImage TIFFRepresentation ] ];
        afterImage = [ atImage copyWithZone:[ atImage zone ] ];
    } else {
        sourceBitmap = [ [ NSBitmapImageRep alloc ] initWithData:[ befImage TIFFRepresentation ] ];
    }
    if(!sourceBitmap){
        sourceBitmap = blankBitmap(VSCREEN_WIDTH,VSCREEN_HEIGHT);
        [ sourceBitmap retain ];
    }
    
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];

    nowBitmapptr = malloc(VSCREEN_WIDTH *VSCREEN_HEIGHT *3);

    phase = 0;

    return self;
}

// private method
-(void)drawEffect
{
    unsigned char *src,*now;
    int spps,sppn;
    int blockSize,w,h,wide,high,pw,ph,idx;
    unsigned long total[3];
    
    // 経過時間をPHASE単位で取得
    //phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *PHASE_NUM;
    if(phase >= PHASE_NUM){
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
        // エフェクト終了
        if(inToward){
            [ self changeEffectionImage:afterImage ];
            return;
        }
        phase = PHASE_NUM -1;
    } else {
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];
    }
    
    src = [ sourceBitmap bitmapData ];
    spps = [ sourceBitmap samplesPerPixel ];
    now = (unsigned char*)nowBitmapptr;
    sppn =3;
    wide = [ sourceBitmap pixelsWide ];
    high = [ sourceBitmap pixelsHigh ];
    if(inToward)
        blockSize = blockSizeTable[((VSCREEN_HEIGHT==600) ? 1 : 0)][ phase ];
    else
        blockSize = blockSizeTable[((VSCREEN_HEIGHT==600) ? 1 : 0)][ PHASE_NUM -phase -1];
        
    for(h = 0; h < VSCREEN_HEIGHT; h+=blockSize){
        for(w = 0; w < VSCREEN_WIDTH; w+=blockSize){
            // ブロックの平均値を求める
            total[0] = 0;
            total[1] = 0;
            total[2] = 0;                
            for(ph = 0; ph < blockSize; ph++){
                idx = ((h +ph) * VSCREEN_WIDTH +w) *spps;
                for(pw = 0; pw < blockSize; pw++,idx+=spps){
                    total[0] += src[idx];
                    total[1] += src[idx+1];
                    total[2] += src[idx+2];
                }
            }
            // ブロック内の各ピクセルに値を設定
            total[0] /= (blockSize *blockSize);
            total[1] /= (blockSize *blockSize);
            total[2] /= (blockSize *blockSize);                
            for(ph = 0; ph < blockSize; ph++){
                idx = ((h +ph) * VSCREEN_WIDTH +w) *sppn;
                for(pw = 0; pw < blockSize; pw++,idx+=sppn){
                    now[idx] = total[0];
                    now[idx+1] = total[1];
                    now[idx+2] = total[2];
                }
            }
        }
    }
    
    [ nowImage lockFocus ];
    
    NSDrawBitmap(TYVirtualScreenRect(),VSCREEN_WIDTH,VSCREEN_HEIGHT,8,3,24,VSCREEN_WIDTH *3,NO,NO,NSCalibratedRGBColorSpace,&nowBitmapptr);
    
    [ nowImage unlockFocus ];
    
    [ self changeEffectionImage:nowImage ];

    phase++;
}

- (void)dealloc
{
    [sourceBitmap release];
    if(nowBitmapptr)
        free((unsigned char*)nowBitmapptr);
    [super dealloc];
}

@end
