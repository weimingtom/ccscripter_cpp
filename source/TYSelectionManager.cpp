//  Tukuyomi
//
//  Created by toveta on Tue Nov 06 2001.
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

#import "TYSelectionManager.h"
#import "TYCellImage.h"


@implementation TYSelectionManager
-(void)initialButtonImage
{
    NSEnumerator *enm;
    NSImage *temp;
    
    // 全てのボタンを描く
    enm = [ btnArray objectEnumerator ];
    [ drawBuffer lockFocus ];
    while( (temp = [ enm nextObject ]) != nil){
        [ temp compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    }
    [ drawBuffer unlockFocus ];
    
    [ [ stageManager compositeLayer ] lockFocus ];
    [ drawBuffer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    [ [ stageManager compositeLayer ] unlockFocus ];
    
    beforeSelection = 0;
    return;

}

-(BOOL)changeSelection:(int)index
{
    int i,count;
    id temp;
    
    if(beforeSelection == index)
        return NO;
    
    count = [ btnArray count ];
    
    [ drawBuffer lockFocus ];
    NSRectFillUsingOperation(TYVirtualScreenRect(),NSCompositeClear);
    for(i=0; i < count; i++){
        temp = [ btnArray objectAtIndex:i ];
        
        if(i == index-1){
            [ temp changeCell:1 ];
        } else {
            [ temp changeCell:0 ];
        }
        
        [ temp compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    }
    [ drawBuffer unlockFocus ];
    
    [ [ stageManager compositeLayer ] lockFocus ];
    [ drawBuffer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    [ [ stageManager compositeLayer ] unlockFocus ];

    beforeSelection = index;
    
    return YES;
}

@end
