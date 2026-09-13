//
//  TYPreferencePanelController.m
//  Tukuyomi
//
//  Created by toveta on Sat Jul 20 2002.
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

#import "TYPreferencePanelController.h"
#import "TYEnviroment.h"


static id sharedObject;

@implementation TYPreferencePanelController
+(int)showDialog
{
    if(!sharedObject)
        sharedObject = [ [ [ self class ] alloc ] init ];

    [ sharedObject loadEnviroment ];
    [ [ sharedObject panel ] orderFront:self ];
    return [ [NSApplication sharedApplication] runModalForWindow:[ sharedObject panel ] ];
}

-(id)init
{
    self = [ super init ];
    [ NSBundle loadNibNamed:@"Preference"
                      owner:self ];
    return self;
}

-(void)loadEnviroment
{
    [ scaleInFullScreen setState:([ TYEnviroment boolForKey:TYDisplayResizeAtFullScreenEnviroment ]) ? NSOnState : NSOffState ];
    [ enableBold setState:([ TYEnviroment boolForKey:TYEnableBoldFontEnviroment ]) ? NSOnState : NSOffState ];
    [ enableAutoReplay setState:([ TYEnviroment boolForKey:TYLookbackVoiceReplayEnviroment ]) ? NSOnState : NSOffState ];
    [ bufferStepper setIntValue:[ [ TYEnviroment objectForKey:TYLookbackBufferPageEnviroment  ] intValue ] ];
    [ bufferPage setIntValue:[ [ TYEnviroment objectForKey:TYLookbackBufferPageEnviroment  ] intValue ] ];
    [ enableMovie setState:([ TYEnviroment boolForKey:TYEnablePlayMovieEnviroment ]) ? NSOnState : NSOffState ];
    [ hideOtherFullScreen setState:([ TYEnviroment boolForKey:TYHideOtherFullScreenEnviroment ]) ? NSOnState : NSOffState ];
    [ hideOtherFullScreen setEnabled:[ scaleInFullScreen state ] ];
}

-(id)panel
{
    return panel;
}

-(IBAction)dialogOK:(id)sender
{
    [ panel orderOut:self ];
    [ TYEnviroment setObject:[ NSNumber numberWithBool:([ scaleInFullScreen state ] == NSOnState) ]
                      forKey:TYDisplayResizeAtFullScreenEnviroment ];
    [ TYEnviroment setObject:[ NSNumber numberWithBool:([ enableBold state ] == NSOnState) ]
                      forKey:TYEnableBoldFontEnviroment ];
    [ TYEnviroment setObject:[ NSNumber numberWithInt:([ enableAutoReplay state ] == NSOnState) ? 1 : 0 ]
                      forKey:TYLookbackVoiceReplayEnviroment ];
    [ TYEnviroment setObject:[ NSNumber numberWithInt:[ bufferPage intValue ] ] 
                      forKey:TYLookbackBufferPageEnviroment ];
    [ TYEnviroment setObject:[ NSNumber numberWithBool:([ enableMovie state ] == NSOnState ) ]
                      forKey:TYEnablePlayMovieEnviroment ];
    [ TYEnviroment setObject:[ NSNumber numberWithBool:([ hideOtherFullScreen state ] == NSOnState ) ]
                      forKey:TYHideOtherFullScreenEnviroment ];
    
    [[NSApplication sharedApplication] stopModalWithCode:NSOKButton ];
}

-(IBAction)dialogCancel:(id)sender
{
    [ panel orderOut:self ];
    [[NSApplication sharedApplication] stopModalWithCode:NSCancelButton ];
}

-(IBAction)changeFullScreen:(id)sender
{
    [ hideOtherFullScreen setEnabled:[ sender state ] ];
}

@end
