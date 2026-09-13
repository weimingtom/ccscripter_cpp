//
//  TYEffectPatternMap.m
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

#import "TYEffectPatternMap.h"


@implementation TYEffectPatternMap
-(id)initWithBitmap:(NSBitmapImageRep*)bitmap
{
    int pixelSize;
    unsigned char *dp,*bp;
    int i,d;
    
    self = [ super init ];
    
    pixelsWide = [ bitmap pixelsWide ];
    pixelsHigh = [ bitmap pixelsHigh ];
    pixelSize = pixelsWide * pixelsHigh;
    mapData = [ [ NSMutableData alloc ] initWithLength:pixelSize ];
    dp = [ mapData mutableBytes ];
    bp = [ bitmap bitmapData ];

    d = [ bitmap bitsPerPixel ];
    if((d != 24) && (d != 32)){
        NSImage *bufferImage;
        
        [ bitmap setSize:NSMakeSize(pixelsWide,pixelsHigh) ];
        bufferImage = [ [ [ NSImage alloc ] initWithSize:NSMakeSize(pixelsWide,pixelsHigh) ] autorelease ];
        [ bufferImage lockFocus ];
        [ [ [ [ NSImage alloc ] initWithData:[ bitmap TIFFRepresentation ] ] autorelease ] compositeToPoint:NSZeroPoint operation:NSCompositeCopy ];
        [ bufferImage unlockFocus ];
        
        bitmap = [ [ [ NSBitmapImageRep alloc ] initWithData:[ bufferImage TIFFRepresentation ] ] autorelease ];
        /*
        bufferImage = [ [ [ NSImage alloc ] initWithData:[ bitmap TIFFRepresentation ] ] autorelease ];
        [ bufferImage recache ];
        bitmap = [ [ [ NSBitmapImageRep alloc ] initWithData:[ bufferImage TIFFRepresentation ] ] autorelease ];
        */
        bp = [ bitmap bitmapData ];
        d = [ bitmap bitsPerPixel ];
    }
    d >>= 3;
    
    for(i=0; i < pixelSize; i++){
        *dp = *bp;
        dp++;
        bp+=d;
    }
    
    return self;
}

-(const unsigned char*)patternMapData
{
    return [ mapData bytes ];
}

-(unsigned int)pixelsWide { return pixelsWide; }
-(unsigned int)pixelsHigh { return pixelsHigh; }

-(void)dealloc
{
    [ mapData release ];
    [ super dealloc ];
}
@end
