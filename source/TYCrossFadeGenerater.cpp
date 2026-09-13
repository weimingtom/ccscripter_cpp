//
//  TYCrossFadeGenerater.m
//  Tukuyomi
//
//  Created by toveta on Wed Nov 21 2001.
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

#import "TYCrossFadeGenerater.h"
#import "TYStageManager.h"
#import "TYImageUtil.h"

@implementation TYCrossFadeGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    
    self = [ super init ];
    
    [ effect getValue:&effdef ];
    phaseInterval = effdef.time / 1000.0 / 256.0;
    time = effdef.time;

    [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];

    afterImage = [ atImage copyWithZone:[ atImage zone ] ];
    /*
    if(afterImage)
        [ afterImage retain ];
    */
    
    beforeBitmap = [ [ NSBitmapImageRep alloc ] initWithData:[ befImage TIFFRepresentation ] ];
    if(!beforeBitmap){
        beforeBitmap = blankBitmap(VSCREEN_WIDTH,VSCREEN_HEIGHT);
        [ beforeBitmap retain ];
    }
    afterBitmap = [ [ NSBitmapImageRep alloc ] initWithData:[ atImage TIFFRepresentation ] ];
    if(!afterBitmap){
        afterBitmap = blankBitmap(VSCREEN_WIDTH,VSCREEN_HEIGHT);
        [ afterBitmap retain ];
    }
    nowImage = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
    
    nowBitmapptr = malloc(VSCREEN_WIDTH *VSCREEN_HEIGHT *3);

    startDate = [ [ NSDate alloc ] init ];
    
    return self;
}

// private method
-(void)drawEffect
{
    int pixel = VSCREEN_WIDTH *VSCREEN_HEIGHT;
    unsigned char *src,*dst,*now;
    int spps,sppd,sppn;
    int i;
    int phase;
    // 経過時間を256分割単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *256;
    if(phase >= 256){
        // エフェクト終了
        [ self changeEffectionImage:afterImage ];
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
        return;
    } else {
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];
        dst = [ beforeBitmap bitmapData ];
        sppd = [ beforeBitmap samplesPerPixel ];
        src = [ afterBitmap bitmapData ];
        spps = [ afterBitmap samplesPerPixel ];
        now = (unsigned char*)nowBitmapptr;
        sppn =3;
        for(i=0; i < pixel; i++,src+=spps,dst+=sppd,now+=sppn){
            now[0] = (src[0] *phase + dst[0] * ( 0xFF - phase )) >> 8;
            now[1] = (src[1] *phase + dst[1] * ( 0xFF - phase )) >> 8;
            now[2] = (src[2] *phase + dst[2] * ( 0xFF - phase )) >> 8;
        }
    }
    
    [ nowImage lockFocus ];
    
    NSDrawBitmap(TYVirtualScreenRect(),VSCREEN_WIDTH,VSCREEN_HEIGHT,8,3,24,VSCREEN_WIDTH *3,NO,NO,NSCalibratedRGBColorSpace,&nowBitmapptr);
    
    [ nowImage unlockFocus ];
    
    [ self changeEffectionImage:nowImage ];

} // override

- (void)dealloc
{
    [ startDate release ];
    [beforeBitmap release];
    [afterBitmap release];
    if(nowBitmapptr)
        free((unsigned char*)nowBitmapptr);
    [super dealloc];
}

@end
