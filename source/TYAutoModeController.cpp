//
//  TYAutoModeController.m
//  Tukuyomi
//
//  Created by toveta on Mon May 27 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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

#import "TYAutoModeController.h"


@implementation TYAutoModeController
+(id)dialog
{
    return [ [ [ [ self class ] alloc ] init ] autorelease ];
}

-(id)init
{
    self = [ super init ];

    [ NSBundle loadNibNamed:@"Automode"
                      owner:self ];

    return self;
}

-(int)runModalTextSpeed:(int)speed waitValue:(int)wait waitType:(TYAutoModeWaitType)type
{
    [ TextSpeed setIntValue:speed ];
    [ WaitNum setIntValue:wait ];
    [ WaitType setState:1 atRow:type column:0 ];
    [ [ TextSpeed window ] orderFront:self ];

    return [ [NSApplication sharedApplication] runModalForWindow:[ TextSpeed window ] ];
}

-(int)textSpeed { return [ TextSpeed intValue ]; }
-(int)waitType { return ([ [ WaitType selectedCell ] tag ] == 0) ? TYNumOfCharsAutomodeWait : TYEndFromDispAutomodeWait; }
-(int)waitNum { return [ WaitNum intValue ]; }

-(IBAction)dialogOk:(id)sender
{
    [ [ TextSpeed window ] orderOut:self ];
    [[NSApplication sharedApplication] stopModalWithCode:NSOKButton ];
}

-(IBAction)dialogCancel:(id)sender
{
    [ [ TextSpeed window ] orderOut:self ];
    [[NSApplication sharedApplication] stopModalWithCode:NSCancelButton ];
}

@end
