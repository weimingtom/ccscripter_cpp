//
//  TYSaveLoadDialogController.m
//  Tukuyomi
//
//  Created by toveta on Sun Jan 06 2002.
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

#import "TYSaveLoadDialogController.h"


@implementation TYSaveLoadDialogController
-(id)init
{
    self = [ super init ];

    dataDict = [ [ NSMutableDictionary dictionary ] retain ];
    dataNum=9;

    return self;
}

-(IBAction)dialogOk:(id)sender
{
    selectedRow = [ tableView selectedRow ];
    [[NSApplication sharedApplication] 
            stopModalWithCode:DIALOG_OK];
}

-(IBAction)dialogCancel:(id)sender
{
    [[NSApplication sharedApplication] 
            stopModalWithCode:DIALOG_CANCEL];
}

-(IBAction)doubleClick:(id)sender
{
    // クリックしたオブジェクトのチェックがほんとは要る。これだとカラムヘッダーのダブルクリックにも反応する。
    if([ okButton isEnabled ]){
        [ self dialogOk:self ];
    }
}

-(void)awakeFromNib
{
    [ tableView setDoubleAction:@selector(doubleClick:) ];
    [ tableView setTarget:self ];
}

-(NSWindow*)window
{
    return window;
}

-(void)setTitle:(NSString*)title
{
    [ window setTitle:title ];
}

-(void)setNumber:(int)number
{
    dataNum = number;
}

-(void)setDate:(NSCalendarDate*)aDate number:(int)number
{
    [ dataDict setObject:aDate forKey:[ NSNumber numberWithInt:number ] ];
}

-(void)setLoaded:(BOOL)aBool
{
    loaded = aBool;
}

-(BOOL)isLoaded
{
    return loaded;
}

-(int)number
{
    return dataNum;
}

-(int)selectedRow
{
    return selectedRow;
}

-(void)setLoadMode:(BOOL)aBool
{
    loadMode = aBool;
}

// implementation of NSTableDataSource protocol
- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
    return dataNum;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)row
{
    id identifier;
    id data;

    identifier = [ tableColumn identifier ];

    if([ identifier isEqualToString:@"No" ]){
        return [ NSString stringWithFormat:@"%d",row+1 ];
    } else if([ identifier isEqualToString:@"Date" ]){
        data = [ dataDict objectForKey:[ NSNumber numberWithInt:row+1 ] ];
        if(data==nil){
            return @"No Data";
        } else {
            return [ data descriptionWithCalendarFormat:NSLocalizedString(@"SaveDateFormat","") ];
        }
    }

    return @"";
}

// delegate methods of NSTableView
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(!loadMode){
        [ okButton setEnabled:YES ];
        return;
    }
    
    if([ dataDict objectForKey:[ NSNumber numberWithInt:[ [ aNotification object ] selectedRow ]+1 ] ]){
        [ okButton setEnabled:YES ];
    } else {
        [ okButton setEnabled:NO ];
    }

    return;
}

@end
