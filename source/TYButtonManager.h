//
//  TYButtonManager.h
//  Tukuyomi
//
//  Created by toveta on Thu Oct 11 2001.
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

#import <Cocoa/Cocoa.h>


@class TYStageManager;

@interface TYButtonManager : NSObject {
    TYStageManager *stageManager;
    NSImage *sourceImage;
    NSMutableArray *btnArray;
    NSImage *drawBuffer;
    int beforeSelection;
    NSMutableData *rectMap;
}
-(id)initWithImage:(NSImage*)srcImage;
-(NSImage*)image;
-(void)addButton:(NSNumber*)no x:(NSNumber*)x y:(NSNumber*)y width:(NSNumber*)width height:(NSNumber*)height selx:(NSNumber*)selx sely:(NSNumber*)sely;
//-(void)addButtonWithCellImage:(TYCellImage*)cellImage atPoint:(NSPoint)point;

//-(void)addButtonOfSprite:(TYNScrSprite*)sprite;
// -(void)clearButtons;
//-(NSArray*)effectiveRectangles;
-(NSData*)rectMap;
-(int)selectedButtonID;
-(int)incToIndex; // 上キーを押した結果のインデックス
-(int)decToIndex; // 下キーを押した結果のインデックス
-(void)initialButtonImage;
-(void)drawWithSelection:(int)status;

-(NSString*)description; // override
@end

enum {
 TYButtonUnInitialize=-1,
 TYButtonNoSelected=0,
};

@interface TYButton : NSObject <NSCopying>{
    NSNumber *idno;
    NSImage *unselectedImage;
    NSImage *selectedImage;
    NSRect drawRect;
}
-(id)initWithID:(NSNumber*)no image:(NSImage*)unselImage selectedImage:selImage rect:(NSRect)aRect;
-(void)drawWithSelect:(BOOL)aBool;
-(NSRect*)boundingRect;
-(NSNumber*)idNo;
-(NSString*)description; // override
-(BOOL)isOverlay;
@end