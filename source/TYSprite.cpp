//
//  TYSprite.m
//  Tukuyomi
//
//  Created by toveta on Fri Oct 19 2001.
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

#import "TYSprite.h"
#import "TYCellImage.h"

@implementation TYSprite

-(void)draw
{
    if(alpha <= 0)
        return;
    else if(alpha<SPRITE_ALPHA_MAX)
        [ sourceImage compositeToPoint:drawPoint operation:NSCompositeSourceOver fraction:alpha *1.0 / SPRITE_ALPHA_MAX  ];
    else
        [ sourceImage compositeToPoint:drawPoint operation:NSCompositeSourceOver ];
}

-(NSRect)rect
{
    NSSize size=[ sourceImage size ];

    NSAssert((sourceImage) , @"failed to get Rectangle from Sprite,sprite has no Image");

    return NSMakeRect(drawPoint.x,drawPoint.y,size.width,size.height);
}

-(void)loadImageFromPath
{
    if(sourceImage)
        [ sourceImage release ];
    sourceImage = [ [ [ TYResourceServer sharedServer ] getImage:imagePath
                                                       transMode:YES
                                                         animate:YES ] retain ];
}

-(NSImage*)sourceImage
{
    if(!sourceImage)
        [ self loadImageFromPath ];
    return sourceImage;
}

-(void)setCell:(int)cellNo
{
    [ (TYCellImage*)sourceImage changeCell:cellNo ];
}

-(void)convertAlphaOfFix
{
    alpha = alpha *255 /100;
}

// implementation of NSCoding
- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [ aCoder encodeObject:imagePath ];
    [ aCoder encodeValueOfObjCType:@encode(NSPoint) at:&drawPoint ];
    [ aCoder encodeValueOfObjCType:"i" at:&alpha ];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    imagePath = [ [ aDecoder decodeObject ] copy ];
    [ aDecoder decodeValueOfObjCType:@encode(NSPoint) at:&drawPoint ];
    [ aDecoder decodeValueOfObjCType:"i" at:&alpha ];

    return self;
}

- (void)dealloc
{
    [ sourceImage release ];
    [ imagePath release ];
    [super dealloc];
}

@end
