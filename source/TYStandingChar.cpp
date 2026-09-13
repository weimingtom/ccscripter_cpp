//
//  TYStandingChar.m
//  Tukuyomi
//
//  Created by toveta on Tue Jan 08 2002.
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

#import "TYStandingChar.h"
#import "TYStageManager.h"


@implementation TYStandingChar

static int underOffset;

+(void)setUnderline:(int)aInt
{
    underOffset = aInt;
}

+(int)underline
{
    return underOffset;
}

-(id)initWithPosition:(NSString*)str path:(NSString*)aPath
{
    NSSize imageSize;
    
    [ super init ];

    position = [ str retain ];
    
    imagePath = [ aPath retain ];

    [ self loadImageFromPath ];

    imageSize = (sourceImage) ? [ sourceImage size ] : NSZeroSize ;
    drawPoint.y=0;
    if([ str isEqualToString:LD_LOCATE_LEFT ]){
        drawPoint.x=VSCREEN_WIDTH /4 - imageSize.width /2;
    } else if([ str isEqualToString:LD_LOCATE_CENTER ]){
        drawPoint.x=VSCREEN_WIDTH /2 - imageSize.width /2;
    } else if([ str isEqualToString:LD_LOCATE_RIGHT ]){
        drawPoint.x=VSCREEN_WIDTH *3 /4 - imageSize.width /2;
    } else {
        [ self autorelease ];
        return nil;
    }

    alpha=SPRITE_ALPHA_MAX;
    
    return self;
}

-(void)draw
{
    drawPoint.y += underOffset;
    [ super draw ];
    drawPoint.y -= underOffset;
}

-(void)dealloc
{
    [ position release ];
    [ super dealloc ];
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)aDecoder
{
    [super initWithCoder:aDecoder];
    position = [[aDecoder decodeObject] copy];

    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ super encodeWithCoder:aCoder ];
    [aCoder encodeObject:position];
}

@end
