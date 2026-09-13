//
//  TYStringImage.h
//  Tukuyomi
//
//  Created by toveta on Wed May 08 2002.
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

@interface TYStringImage : NSObject {
    /*
    NSImage *chache;
    NSAttributeString* string;
    BOOL shadow;
     */
    NSPoint origin;
    NSSize size;
    NSImage *image;
}
+(NSArray*)imagesWithString:(NSString*)aStr
                 attributes:(NSDictionary*)attributes
                     colors:(NSArray*)colorsArray
                 fontHeight:(int)fontHeight
                   fontWidth:(int)fontWidth
                   interval:(int)interval
                     shadow:(BOOL)shadow;

-(id)initWithImage:(NSImage*)aImage size:(NSSize)aSize offset:(NSPoint)point;
-(NSPoint)origin;
-(void)setOrigin:(NSPoint)point;
-(void)setOriginByValue:(NSValue*)pointValue;
@end
