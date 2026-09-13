//
//  TYAnimationCellImage.m
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

#import "TYAnimationCellImage.h"
#import "StopNSLog.h"

NSString* const TYAnimationModeInfo=@"TYAnimationModeInfo"; // number of constant
NSString* const TYAnimationIntervalInfo=@"TYAnimationIntervalInfo"; // number or numbers in Array

@implementation TYAnimationCellImage
-(id)initWithImages:(NSArray*)aArray infoDict:(NSDictionary*)aDict
{
    //NSLog2(@"animeImage source=\n%@",aArray);
    self = [ super initWithImages:aArray ];

    // アニメ指定判定
    mode = [ [ aDict objectForKey:TYAnimationModeInfo ] intValue ];

    // タイマ作成
    if(mode!=TYAnimationNoPlay){
        interval = [ [ aDict objectForKey:TYAnimationIntervalInfo ] retain ];
    }

    return self;
}

-(void)setDelegate:(id)aObject
{
    delegate = aObject;
}

-(void)changingCell
{
    if(!isPlaying)
        return;
    
    switch(mode){
        case TYAnimationLoopingPlayback:
            if([ self currentCell ] +1 >= [ self cells ]){
                [ self changeCell:0 ];
            } else {
                [ self changeCell:[ self currentCell ]+1 ];
            }
            [ self setNextTimer ];
            break;
        case TYAnimationNormalPlayback:

            if([ self currentCell ]+1==[ self cells ]){
                [ self stop ];
            } else {
                [ self changeCell:[ self currentCell ]+1 ];
                [ self setNextTimer ];
            }
                break;
        case TYAnimationLoopingBackAndForthPlayback:
            if(!back){
                if([ self currentCell ]+1 == [ self cells ]-1){
                    back=YES;
                }
                [ self changeCell:[ self currentCell ]+1 ];
            } else {
                if([ self currentCell ]==1){
                    back=NO;
                }
                [ self changeCell:[ self currentCell ]-1 ];
            }
            [ self setNextTimer ];
            break;
        default:
            break;
    }

    /*
    if([ self currentCell ] +1 > [ self cells ]){
        [ self changeCell:0 ];
    } else {
        [ self changeCell:[ self currentCell ]+1 ];
    }
     */

    [ self didChangeCell:self ];
}

// implemantation for delegate
-(void)didChangeCell:(id)sender
{    
    if([ delegate respondsToSelector:@selector(didChangeCell:) ]){
        [ delegate didChangeCell:self ];
    }
}

// animation operate
-(void)play
{
    if(mode==TYAnimationNoPlay)
        return;

    back=NO;
    [ self changeCell:0 ];
    [ self resume ];
}
-(BOOL)isPlaying
{
    return isPlaying;
}

-(void)stop
{
    // タイマストップ
    isPlaying = NO;
}
-(void)resume
{
    
    if(mode==TYAnimationNoPlay)
        return;
    isPlaying=YES;

    [ self setNextTimer ];
}

-(void)setNextTimer
{
    int delay;

    // タイマ起動
    if([ interval respondsToSelector:@selector(intValue) ]){
        delay = [ interval intValue ];
    } else {
        delay = [ [ interval objectAtIndex:[ self currentCell ] ] intValue ];
    }

    [ self performSelector:@selector(changingCell) withObject:nil afterDelay:delay /1000.0 ];
}

-(BOOL)isAnimate { return YES; }

- (void)dealloc
{
    [ NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changingCell) object:nil ];
    [interval release];
    [super dealloc];
}

@end
