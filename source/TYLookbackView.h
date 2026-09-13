//
//  TYLookbackView.h
//  Tukuyomi
//
//  Created by toveta on Thu Mar 06 2003.
//  Copyright (c) 2003 toveta All rights reserved.
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

@class TYLookbackLayer;
@class TYControllButton;
@class TYCellImage;

@interface TYLookbackView : NSView {
    NSArray *lookbackArray;
    int index;
    //NSColor *menuWindowColor;
    NSDictionary *attDict;
    id controller;
    NSImage *drawBuffer;
    TYControllButton *button[2];
    NSTrackingRectTag rectTag[2];
    NSRect btnRect[2];
}
-(id)initWithFrame:(NSRect)aFrame
        controller:(id)aObj
        attributes:(NSDictionary*)aDict
            layout:(NSMutableDictionary*)layoutDic
            buffer:(NSArray*)aArray
           ubutton:(TYCellImage*)uImage
           dbutton:(TYCellImage*)dImage;

-(void)setPageIndex:(int)aInt;

-(void)updatesTrackingRect;

-(void)endMode;


-(void)up_action;
-(void)down_action;
-(void)cancel_action:(id)sender;

-(IBAction)execButton:(id)sender;

@end
