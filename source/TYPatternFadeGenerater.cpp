//
//  TYPatternFadeGenerater.m
//  Tukuyomi
//
//  Created by toveta on Mon Nov 04 2002.
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

#import "TYPatternFadeGenerater.h"
#import "TYEffectPatternMap.h"
#import "TYResourceServer.h"
#import "TYStageManager.h"
#import "TYImageUtil.h"

@implementation TYPatternFadeGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectDefinition effdef;
    TYEffectPatternMap *patternMap;
    
    self = [ super init ];
    
    [ effect getValue:&effdef ];
    patternMap = [ [ TYResourceServer sharedServer ] getEffectPattern:effdef.path ];
    if(!patternMap){
        NSLog(@"can't load pattern Map");
        [ self autorelease ];
        return nil;
    }
    patternptr = [ patternMap patternMapData ];
    patternWide = [ patternMap pixelsWide ];
    patternHigh = [ patternMap pixelsHigh ];
    
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
    unsigned char *src,*dst,*now;
    int spps,sppd,sppn;
    int phase;
    // 経過時間を256分割単位で取得
    phase =  (-[ startDate timeIntervalSinceNow ] *1000) / time *256;
    if(phase >= 256){
        // エフェクト終了
        [ self changeEffectionImage:afterImage ];
        [ self performSelector:@selector(effectFinished) withObject:nil afterDelay:0 ];
        return;
    } else {
        int w,h,ph,pw;
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:phaseInterval ];
        dst = [ beforeBitmap bitmapData ];
        sppd = [ beforeBitmap samplesPerPixel ];
        src = [ afterBitmap bitmapData ];
        spps = [ afterBitmap samplesPerPixel ];
        now = (unsigned char*)nowBitmapptr;
        sppn =3;
        
        for(h=0,ph=0; h < VSCREEN_HEIGHT; h++){
            for(w=0,pw=0; w < VSCREEN_WIDTH; w++){
                if(patternptr[patternWide *ph +pw] < phase){
                    now[0] = src[0];
                    now[1] = src[1];
                    now[2] = src[2];
                } else {
                    now[0] = dst[0];
                    now[1] = dst[1];
                    now[2] = dst[2];
                }
                pw++;
                if(pw >= patternWide)
                    pw=0;
                src+=spps,dst+=sppd,now+=sppn;
            }
            ph++;
            if(ph >= patternHigh)
                ph=0;
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
    [ super dealloc ];
}


@end
