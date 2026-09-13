//
//  TYAudioPathController.m
//  Tukuyomi
//
//  Created by toveta on Sat Jan 19 2002.
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

#import "TYAudioPathController.h"


@implementation TYAudioPathController
-(IBAction)dialogOk:(id)sender
{
    NSString *path;
    NSString *fn;
    NSScanner *scanner;

    fn = [ filename stringValue ];
    if([ fn length ]==0){
        NSBeep();
        [ errorField setStringValue:NSLocalizedString(@"Please be sure to input a file name.","file name is blank") ];
        return;
    }

    scanner = [ NSScanner scannerWithString:fn ];
    [ scanner scanUpToString:@"*" intoString:NULL ];
    if([ scanner isAtEnd ]){
        NSBeep();
        [ errorField setStringValue:NSLocalizedString(@"An asterisk is required for a file name.","") ];
        return;
    }

    path = [ [ directory stringValue ] stringByAppendingPathComponent:fn ];

    [ self changeAudioPath:path ];
    [ window orderOut:self ];
}

-(IBAction)dialogCancel:(id)sender
{
    [ window orderOut:self ];
}

-(IBAction)restoreDefault:(id)sender
{
    NSString *dir,*file;

    dir = [ defaultPath stringByDeletingLastPathComponent ];
    file = [ defaultPath lastPathComponent ];

    [ directory setStringValue:dir ];
    [ filename setStringValue:file ];
}

-(IBAction)selectDirectory:(id)sender
{
    int stat;
    NSOpenPanel *op = [ NSOpenPanel openPanel ];

    [ op setCanChooseDirectories:YES ];
    [ op setCanChooseFiles:NO ];


    stat = [ op runModalForDirectory:[ directory stringValue ]
                                file:nil
                               types:nil ];

    if(stat==NSOKButton){
        [ directory setStringValue:[ op filename ] ];
    }
}

-(void)openWithPath:(NSString*)aPath defaultPath:(NSString*)defPath
{
    defaultPath = [ defPath retain ];
    initPath = [ aPath retain ];

    [ directory setStringValue:[ initPath stringByDeletingLastPathComponent ] ];
    [ filename setStringValue:[ initPath lastPathComponent ] ];

    [ window orderFront:self ];
}

-(void)setDelegate:(id)aObject
{
    delegate = aObject;
}

    // delegate implementation
-(void)changeAudioPath:(NSString*)aPath
{
    if(delegate)
        [ delegate changeAudioPath:aPath ];
}

- (void)dealloc
{
    [defaultPath release];
    [initPath release];
    [super dealloc];
}

@end
