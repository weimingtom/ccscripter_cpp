//
//  TYCellImage.h
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

#import <Cocoa/Cocoa.h>

// Cell構造を持つイメージ。複数のImageを保持し、現在指定されている番号のImageにメッセージを転送することで機能を実現する。
// imageに転送できない場合はArrayへの転送も試みる。
@interface TYCellImage : NSObject {
    NSArray *imageArray;
    NSImage *currentImage;
    int currentCell;
}
-(id)initWithImages:(NSArray*)aArray;
//-(id)initWithAttributeString:(NSAttributeString*)aString colors:(NSArray*)colorStrings;
-(void)changeCell:(int)index;
-(int)currentCell;
-(int)cells;
-(BOOL)isCellImage;
-(BOOL)isAnimate;

- (void)forwardInvocation:(NSInvocation *)anInvocation; // override
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector; // override

@end

@interface NSImage (TYCellImageExtension)
-(void)changeCell:(int)index;
-(BOOL)isCellImage;
-(BOOL)isAnimate;
@end
