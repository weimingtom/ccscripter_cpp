//
//  TYNScrSprite.h
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

#import <Cocoa/Cocoa.h>
#import "TYSprite.h"

// NScriptで定義するところのスプライト
@interface TYNScrSprite : TYSprite {
    int spriteID;
    BOOL visible;

    BOOL registDelegate;
}
//+(id)dummySprite:(int)idNo; // Array検索用のダミーを作成
-(id)initWithID:(int)idNo path:(NSString*)aPath point:(NSPoint)point alpha:(NSNumber*)alphaNum visible:(BOOL)aBool;
//-(id)initWithID:(int)idNo image:(NSImage*)aImage point:(NSPoint)point alpha:(NSNumber*)alphaNum visible:(BOOL)aBool;


-(void)moveX:(NSNumber*)x Y:(NSNumber*)y alpha:(NSNumber*)alphaNum;
-(void)absoluteMoveX:(NSNumber*)x Y:(NSNumber*)y alpha:(NSNumber*)alphaNum;
-(void)setVisible:(BOOL)aBool;
-(BOOL)visible;

-(int)spriteID;

-(void)draw; // override

extern NSString* const TYSpriteAnimationNotification;

@end
