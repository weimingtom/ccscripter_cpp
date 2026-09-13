//
//  TYSaveLoadManager.m
//  Tukuyomi
//
//  Created by toveta on Sun Mar 02 2003.
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

#include "TYSaveLoadManager.h"
#include "TYMiscUtil.h"
#include "TYMainController.h"

@implementation TYSaveLoadManager

NSString* const TYDataSaveNotification=@"TYDataSaveNotification";
NSString* const TYSaveNumberKey=@"TYSaveNumberKey";
NSString* const TYSaveDateStringKey=@"TYSaveDateStringKey";

-(id)init
{
    self = [ super init ];
    
    [ self setNames:[ NSArray arrayWithObjects:NSLocalizedString(@"<SAVE>",nil),
                                               NSLocalizedString(@"<LOAD>",nil),
                                               NSLocalizedString(@"bookmark",nil),nil ] ];
    
    return self;
}

-(void)setNames:(NSArray*)aArray
{
    if(names)
        [ names release ];
    
    names = [ aArray retain ];
}

-(NSArray*)names
{
    return names;
}

-(void)setNumber:(int)number
{
    int i;
    
    dataNum = number;
    if(bookmarkArray)
        [ bookmarkArray release ];
    if(existsArray)
        [ existsArray release ];
        
    bookmarkArray = [ [ NSMutableArray array ] retain ];
    existsArray = [ [ NSMutableArray array ] retain ];
    
    for(i = 0; i < number; i++){
        [ bookmarkArray addObject:[ NSNull null ] ];
        [ existsArray addObject:[ NSNull null ] ];
    }
}

-(int)number
{
    return dataNum;
}

-(void)resetDateWithNumber:(int)number
{
    NSString *filename,*path;
    NSString *bookMarkBase = [ names objectAtIndex:2 ];
    NSString *bookMark;
    NSFileManager *manager = [ NSFileManager defaultManager ];
    BOOL exists;

    if(dataNum < number || number <= 0)
        return;
    
    filename = [ NSString stringWithFormat:SAVEDATA_NAME,number ];
    path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];
    
    bookMark = [ bookMarkBase stringByAppendingString:TYWideCharDecimalString(number) ];
    if(number < 10)
        bookMark = [ bookMark stringByAppendingString:NSLocalizedString(@"WSPACE",nil) ];
    bookMark = [ bookMark stringByAppendingString:NSLocalizedString(@"WSPACE",nil) ];

    exists = [ manager fileExistsAtPath:path ];
    if(!exists){
        bookMark = [ bookMark stringByAppendingString:
                        NSLocalizedString(@"NScrNoSaveDataDate",nil) ]; 
    } else {
        NSDate *aDate;
        NSArray *calArray;
        NSString *dateStr;
            
        aDate = [ [ manager fileAttributesAtPath:path traverseLink:NO ] objectForKey:NSFileModificationDate ];
        
        dateStr = [ aDate descriptionWithCalendarFormat:@"%m,%d,%H,%M" timeZone:nil locale:nil ];
        calArray = [ dateStr componentsSeparatedByString:@"," ];
        bookMark = [ bookMark stringByAppendingString:
                        [ NSString stringWithFormat:
                            NSLocalizedString(@"NScrSaveDateFormat",nil),
                            TYWideCharDecimalStringWithString([ calArray objectAtIndex:0 ]),
                            TYWideCharDecimalStringWithString([ calArray objectAtIndex:1 ]),
                            TYWideCharDecimalStringWithString([ calArray objectAtIndex:2 ]),
                            TYWideCharDecimalStringWithString([ calArray objectAtIndex:3 ])
                        ] ];                            
    }

    [ bookmarkArray replaceObjectAtIndex:number -1 withObject:bookMark ];
    [ existsArray replaceObjectAtIndex:number -1 withObject:[ NSNumber numberWithBool:exists ] ];
}

-(void)dataLoad
{
    int i;
    
    if(!loaded){
        loaded = YES;
        // Loadされていなければセーブデータファイルを検索
        for(i=1; i <= dataNum; i++){
            [ self resetDateWithNumber:i ];
        }
    }
}

-(NSArray*)dateStrings:(BOOL)isLoad
{
    NSString *strTitle;
    
    strTitle = [ names objectAtIndex:(isLoad) ? 1 : 0 ];

    [ self dataLoad ];
    
    return [ [ NSArray arrayWithObject:strTitle ] arrayByAddingObjectsFromArray:bookmarkArray ];
}

-(NSArray*)existsArray
{
    return existsArray;
}

- (void)dealloc
{
	[names release];
        [bookmarkArray release];
        [existsArray release];
	[super dealloc];
}

@end
