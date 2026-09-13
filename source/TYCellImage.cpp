//
//  TYCellImage.m
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

#import "TYCellImage.h"


@implementation TYCellImage
-(id)initWithImages:(NSArray*)aArray
{
    self = [ super init ];
    
    imageArray = [ aArray retain ];
    
    currentImage = [ [ imageArray objectAtIndex:0 ] retain ];
    
    return self;
}

-(void)changeCell:(int)index
{
    currentCell = index;
    
    if(currentImage)
        [ currentImage release ];
    currentImage = [ [ imageArray objectAtIndex:index ] retain ];
}

-(int)currentCell
{
    return currentCell;
}

-(int)cells
{
    return [ imageArray count ];
}

-(BOOL)isCellImage { return YES; }
-(BOOL)isAnimate { return NO; }

- (void)dealloc
{
	[imageArray release];
        [currentImage release];
	[super dealloc];
}

- (BOOL)respondsToSelector:(SEL)aSelector
{
    return ([ super respondsToSelector:aSelector ] ||
            [ currentImage respondsToSelector:aSelector ] ||
            [ imageArray respondsToSelector:aSelector ]);
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    SEL sel = [ anInvocation selector ];
    if([ currentImage respondsToSelector:sel ])
        [ anInvocation invokeWithTarget:currentImage ];
    else if([ imageArray respondsToSelector:sel ])
        [ anInvocation invokeWithTarget:imageArray ];
    else
        [ self doesNotRecognizeSelector:sel ];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if([ super respondsToSelector:aSelector ])
        return [ super methodSignatureForSelector:aSelector ];
    else if([ currentImage respondsToSelector:aSelector ])
        return [ currentImage methodSignatureForSelector:aSelector ];
    else
        return [ imageArray methodSignatureForSelector:aSelector ];
}

- (NSString *)description
{
    return [ NSString stringWithFormat:@"TYCellImage current=%d,images=%@",currentCell,imageArray ];
}

@end

@implementation NSImage (TYCellImageExtension)
-(void)changeCell:(int)index { NSLog2(@"called selector 'changingcell:', but this image don't have cells"); }
-(BOOL)isCellImage { return NO; }
-(BOOL)isAnimate { return NO; }
@end
