//
//  TYAnimationCellImage.h
//  Tukuyomi
//
//  Created by toveta on Wed Feb 27 2002.
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
#import "TYCellImage.h"


typedef enum {
    TYAnimationLoopingPlayback=0,
    TYAnimationNormalPlayback,
    TYAnimationLoopingBackAndForthPlayback,
    TYAnimationNoPlay,
} TYAnimationMode;

// Animation-Infomateion Dictionary-Key
extern NSString* const TYAnimationModeInfo; // number of constant
extern NSString* const TYAnimationIntervalInfo; // number or numbers in Array

@interface TYAnimationCellImage : TYCellImage {
    id delegate;
    id interval;
    TYAnimationMode mode;
    BOOL isPlaying;
    BOOL intervalIsArray;
    BOOL back;
}
-(id)initWithImages:(NSArray*)aArray infoDict:(NSDictionary*)aDict;

-(void)setDelegate:(id)aObject;
//-(id)delegate;

-(void)changingCell;

// implemantation for delegate
-(void)didChangeCell:(id)sender;

// animation operate
-(void)play;
-(BOOL)isPlaying;
-(void)stop;
-(void)resume;

// private
-(void)setNextTimer;
@end
