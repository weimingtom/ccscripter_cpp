//
//  TYEnviroment.h
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


#import <Foundation/Foundation.h>

// パッケージ内の定義ファイルを読み込み、さらにアプリのディレクトリ内の環境設定保存ファイルを読み込む。
@interface TYEnviroment : NSObject {
}
+(id)objectForKey:(id)aObj;
+(id)defaultObjectForKey:(id)aObj;
+(void)setObject:(id)aObj forKey:(id)aKey;
+(void)save;
+(float)floatForKey:(id)aObj;
+(BOOL)boolForKey:(id)aKey;

+(void)reload;
@end

#define ENVDATA_FILENAME @"envdata.plist"

// constants of dictionry-key
extern NSString* const TYFontNameEnviroment;
extern NSString* const TYAudioFilePathEnviroment;
extern NSString* const TYWindowPointXEnviroment;
extern NSString* const TYWindowPointYEnviroment;
extern NSString* const TYBGMVolumeEnviroment;
extern NSString* const TYVoiceVolumeEnviroment;
extern NSString* const TYSEVolumeEnviroment;
extern NSString* const TYScaleInFullScreenModeEnviroment;
extern NSString* const TYEnableBoldFontEnviroment;
extern NSString* const TYFullScreenModeEnviroment;
extern NSString* const TYWaveExtraNameEnviroment;
extern NSString* const TYDisableSelectScriptEnviroment;
//extern NSString* const TYEnableContextMenuEnviroment;
extern NSString* const TYLookbackBufferPageEnviroment;
extern NSString* const TYLookbackVoiceReplayEnviroment;
extern NSString* const TYDisplayResizeAtFullScreenEnviroment;
extern NSString* const TYAutomodeWaitTypeEnviroment;
extern NSString* const TYAutomodeWaitTimeEnviroment;
extern NSString* const TYKidokumodeEnviroment;
extern NSString* const TYEnablePlayMovieEnviroment;
extern NSString* const TYHideOtherFullScreenEnviroment;

extern NSString* const TYLastSelectedFolderPref;
