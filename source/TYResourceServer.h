//
//  TYResourceServer.h
//  Tukuyomi
//
//  Created by toveta on Thu Jun 14 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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
#import "TYArchiver.h"

@class NSImage;
@class NSBitmapImageRep;
@class TYEffectPatternMap;

// NScripterのシナリオファイル以外の外部リソースを管理します。
// リソースの要求があった場合、まず各アーカイバの仮想パスとして検索し、次にディスク内の実ファイルを探します。
@interface TYResourceServer : NSObject {
    NSMutableArray *ty_archivers;
    NSString *defaultTransmode;
    BOOL filelog;
    NSMutableDictionary *filelogDict;

    NSMutableDictionary *spiPluginDict;
    NSMutableDictionary *soundPressPluginDict;
    NSMutableDictionary *effectPatternDict;
    NSMutableDictionary *executableBundles;
}

+(id)sharedServer;

-(void)addArchiver:(NSString*)path;

-(NSString*)getFilePath:(NSString*)path; // ディスク中にファイルが存在するならそのパスを返す。
-(NSData*)getData:(NSString*)path; // プラグインを使用せずにデータ取得。
-(NSImage*)getImage:(NSString*)path transMode:(BOOL)aBool animate:(BOOL)isAnimate;
-(NSImage*)getImage:(NSString*)path transMode:(BOOL)aBool; // イメージを取得し、タグに従い加工して返す
-(NSImage*)getImageFromString:(NSString*)str;
-(NSBitmapImageRep*)getBitmap:(NSString*)path;
-(NSImage*)transrateImageFromBitmap:(NSBitmapImageRep*)bitmap transMode:(NSString*)transmode; // private
-(NSData*)getSoundData:(NSString*)path;
-(TYEffectPatternMap*)getEffectPattern:(NSString*)path;
-(NSString*)getFilePathMakeTemp:(NSString*)path; // ファイルがディスク中に存在しなければ、Tempファイルを作成し、そのパスを返す。

-(void)addSpi:(NSString*)pluginName extension:(NSString*)extension;
-(void)addSoundPressPlugin:(NSString*)pluginName extension:(NSString*)extension;
-(void)setDefaultTransMode:(NSString*)transmode;
-(void)addEffectPattern:(NSString*)path;

-(void)executeBundle:(NSString*)pathAndArg;

-(BOOL)filelog:(NSString*)path;
-(void)addlog:(NSString*)filename;
-(BOOL)fchk:(NSString*)filename;
-(BOOL)saveLog:(NSString*)path;

#define FILELOG_FILENAME @"NScrflog.dat"
#define WIN_PATH_DELIMITER "\\"


@end
