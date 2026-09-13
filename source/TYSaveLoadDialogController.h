//
//  TYSaveLoadDialogController.h
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

#import <Cocoa/Cocoa.h>

// TableViewのデータソース、ダイアログのController、セーブデータの管理オブジェクト、の３つの役割を持つ。
@interface TYSaveLoadDialogController : NSObject {
    IBOutlet id window;
    IBOutlet id tableView;
    IBOutlet id okButton;
    
    NSMutableDictionary *dataDict;
    int dataNum;
    int selectedRow;
    BOOL loaded;
    BOOL loadMode;
}
-(IBAction)dialogOk:(id)sender;
-(IBAction)dialogCancel:(id)sender;
-(IBAction)doubleClick:(id)sender;

-(NSWindow*)window;
-(void)setTitle:(NSString*)title;
-(void)setNumber:(int)number;
-(int)number;
-(void)setDate:(NSCalendarDate*)aDate number:(int)number;
-(void)setLoaded:(BOOL)aBool;
-(BOOL)isLoaded;
-(void)setLoadMode:(BOOL)aBool;
-(int)selectedRow;
@end

#define DIALOG_OK 128
#define DIALOG_CANCEL 129