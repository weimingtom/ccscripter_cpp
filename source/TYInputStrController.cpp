//
//  TYInputStrController.m
//  Tukuyomi
//
//  Created by toveta on Thu Jan 24 2002.
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

#import "TYInputStrController.h"
#import "TY-NSStringAddition.h"

@implementation TYInputStrController
-(IBAction)dialogOk:(id)sender
{
    [ window orderOut:self ];
    
    [[NSApplication sharedApplication] 
        stopModalWithCode:NSOKButton ];
}

+(id)dialog
{
    return [ [ [ [ self class ] alloc ] init ] autorelease ];
}

-(id)init
{
    self = [ super init ];

    [ NSBundle loadNibNamed:@"InputStrDialog" owner:self ];

    [ [ NSNotificationCenter defaultCenter ] addObserver:self
                                                selector:@selector(textDidChange:)
                                                    name:nil
                                                  object:input ];
    return self;
}

-(int)runModalCaption:(NSString*)aStr length:(int)aInt notAscii:(BOOL)aBool
{
    return [ self runModalCaption:aStr defaultStr:@"" length:aInt notAscii:aBool ];
}

-(int)runModalCaption:(NSString*)aStr defaultStr:(NSString*)defStr length:(int)aInt notAscii:(BOOL)aBool
{
    [ caption setStringValue:aStr ];
    [ input setStringValue:defStr ];
    length = aInt;
    notAscii = aBool;
    
    return  [[NSApplication sharedApplication] runModalForWindow:window ];
}

-(NSString*)string
{
    return [ input stringValue ];
}

- (void)textDidChange:(NSNotification *)aNotification
{
    NSString *newStr = [ input stringValue ];
    
    // 最大文字数チェック
    if([ newStr cSJISStringLength ] > length){
        NSBeep();
        [ input setStringValue:beforeString ];
        return;
    }

    // 全角チェック
    if(notAscii){
        if([ newStr length ]*2 != [ newStr cStringLength ]){
            NSBeep();
            [ input setStringValue:beforeString ];
            return;
        }
    }

    if(beforeString)
        [ beforeString autorelease ];
    beforeString = [ newStr retain ];
    
    // 入力チェック
    if([ newStr length ]){
        [ okButton setEnabled:YES ];
    } else {
        [ okButton setEnabled:NO ];
    }
}

- (void)dealloc
{
    if(beforeString)
        [beforeString release];
    [super dealloc];
}

@end
