//
//  TYMarDLGPlugin.m
//  Tukuyomi
//
//  Created by toveta on Sun Aug 03 2003.
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

#import "TYMarDLGPlugin.h"


@implementation TYMarDLGPlugin
-(void)execPlugin:(id<TYCCScripterProxy>)ccsProxy argment:(NSString*)aStr
{
    NSEnumerator *enu;
    int result;
    
    enu = [ [ aStr componentsSeparatedByString:@"/" ] objectEnumerator ];
    
 
    if(loaded == NO) {
        loaded = [ [ NSBundle bundleForClass:[ self class ] ]
            loadNibFile:@"Sis3qDialog"
            externalNameTable:[ NSDictionary dictionaryWithObject:self forKey:@"NSOwner" ]
            withZone:NULL ];
    }

    [ FirstName setStringValue:[ enu nextObject ] ];
    [ SecondName setStringValue:[ enu nextObject ] ];
    [ KanaName setStringValue:[ enu nextObject ] ];
    [ NickName setStringValue:[ enu nextObject ] ];

    result = [ NSApp runModalForWindow:[ FirstName window ] ];
    [ [ FirstName window ] orderOut:self ];
    
    switch(result) {
    case NSOKButton:
        [ ccsProxy setRetunCode:1 string:
            [ [ NSArray arrayWithObjects:[ FirstName stringValue ],
                [ SecondName stringValue ],
                [ KanaName stringValue ],
                [ NickName stringValue ],nil ] componentsJoinedByString:@"/" ] ];
        break;
    default:
        [ ccsProxy setRetunCode:0 string:aStr ];
        break;
    }
}

- (IBAction)dialogOK:(id)sender
{
    [ NSApp stopModalWithCode:NSOKButton ];
}

- (IBAction)dialogCancel:(id)sender
{
    [ NSApp stopModalWithCode:NSCancelButton ];
}

@end

