//
//  TYEnviroment.m
//  Tukuyomi
//
//  Created by toveta on Thu Jan 17 2002.
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

#import "TYEnviroment.h"
#import "TYMiscUtil.h"


@implementation TYEnviroment

static NSDictionary *packageDict;
static NSMutableDictionary *userDict;

NSString* const TYFontNameEnviroment=@"DefaultFontName";
NSString* const TYAudioFilePathEnviroment=@"DefaultAudioTrackPath";
NSString* const TYWindowPointXEnviroment=@"WindowPointX";
NSString* const TYWindowPointYEnviroment=@"WindowPointY";
NSString* const TYBGMVolumeEnviroment=@"BGMVolume";
NSString* const TYVoiceVolumeEnviroment=@"VoiceVolume";
NSString* const TYSEVolumeEnviroment=@"SEVolume";
NSString* const TYScaleInFullScreenModeEnviroment=@"ScaleInFullScreenMode";
NSString* const TYEnableBoldFontEnviroment=@"EnableBoldFont";
NSString* const TYFullScreenModeEnviroment=@"isFullScreen";
NSString* const TYWaveExtraNameEnviroment=@"WAVExtraBundleName";
NSString* const TYDisableSelectScriptEnviroment=@"DisableSelectScript";
//NSString* const TYEnableContextMenuEnviroment=@"EnableContextMenu";
NSString* const TYLookbackBufferPageEnviroment=@"LookbackBufferPage";
NSString* const TYLookbackVoiceReplayEnviroment=@"LookbackVoiceReplay";
NSString* const TYDisplayResizeAtFullScreenEnviroment=@"DisplayResizeInFullScreenMode";
NSString* const TYAutomodeWaitTypeEnviroment=@"AutomodeWaitType";
NSString* const TYAutomodeWaitTimeEnviroment=@"AutomodeWaitTime";
NSString* const TYKidokumodeEnviroment=@"Kidokumode";
NSString* const TYEnablePlayMovieEnviroment=@"EnablePlayMovie";
NSString* const TYHideOtherFullScreenEnviroment=@"HideOtherInFullScreenMode";

NSString* const TYLastSelectedFolderPref=@"LastSelectedFolder";

+ (void)initialize
{
    NSBundle *bundle;
    NSString *path;

    bundle = [ NSBundle mainBundle ];
    packageDict = [ [ NSDictionary dictionaryWithContentsOfFile:[ bundle pathForResource:@"Settings" ofType:@"plist" ] ] retain ];

    path = [ getNScrRootDirectory() stringByAppendingPathComponent:ENVDATA_FILENAME ];
    userDict = [ [ NSMutableDictionary dictionaryWithContentsOfFile:path ] retain ];
    if(!userDict)
        userDict = [ [ NSMutableDictionary dictionary ] retain ];
}

+(void)reload
{
    if(userDict){
        NSMutableDictionary *aDict;
        NSString *path = [ getNScrRootDirectory() stringByAppendingPathComponent:ENVDATA_FILENAME ];
        aDict = [ [ NSMutableDictionary dictionaryWithContentsOfFile:path ] retain ];
        if(aDict){
            [ userDict release ];
            userDict = aDict;
        }
    }
}

+(id)objectForKey:(id)aObj
{
    id temp;

    if(temp = [ userDict objectForKey:aObj ]){
        return temp;
    } else if(temp = [ packageDict objectForKey:aObj ]){
        return temp;
    }

    return nil;
}

+(float)floatForKey:(id)aObj
{
    return [ [ self objectForKey:aObj ] floatValue ];
}

+(BOOL)boolForKey:(id)aKey
{
    id tmp;

    if(tmp = [ self objectForKey:aKey ])
        return [ tmp boolValue ];
    else
        return NO;
}

+(id)defaultObjectForKey:(id)aObj
{
    return [ packageDict objectForKey:aObj ];
}

+(void)setObject:(id)aObj forKey:(id)aKey
{
    [ userDict setObject:aObj forKey:aKey ];
}

+(void)save
{
    NSString *path;

    path = [ getNScrRootDirectory() stringByAppendingPathComponent:ENVDATA_FILENAME ];
    if(userDict)
        [ userDict writeToFile:path atomically:NO ];
}


@end
