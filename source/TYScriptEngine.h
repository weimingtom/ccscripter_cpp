//
//  TYScriptEngine.h
//  Tukuyomi
//
//  Created by toveta on Thu Jun 07 2001.
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

// スクリプトファイルの読み込み、変換、初期化、内部処理を一手に引き受けるクラスです。
#import <Cocoa/Cocoa.h>
#import "TYScripterValues.h"
#import "TYSaveDataCoding.h"
#import "TYLoopValue.h"

@class TYFileLog;
@class TYArgment;

@interface TYScriptEngine : NSObject <TYSaveDataCoding>{
    id controller;
    id stageManager;

    BOOL breakRun;
    NSArray *ScriptArray;
    NSMutableData *currentData; // 処理中の行を保存しておくバッファー
    unsigned runColumn;
    unsigned runLine;
    TYScriptPoint savePoint;
    unsigned char newLineCode[3];
    NSMutableArray *returnStack; // gosub系ルーチンのコール地点のスタック
    NSSet *multiLineCmdStringSet; // 複数行にまたがるコマンドの設定
    NSMutableArray *loopStack; // for~nextループのスタック

    NSMutableDictionary *numAliases;  // ユーザー定義エイリアス
    NSMutableDictionary *strAliases;
    TYScripterValues *scripterValues; // ユーザー定義変数領域
    BOOL globalOn;
    NSString *nsaDir;

    TYFileLog *labelLog;
    
    NSDate *timerStartDate; // waittimer命令用の内部タイマ

    NSString *labelWithTrap;
    BOOL fireOfTrap;

    NSMutableDictionary *labelDict;

    NSMutableData *kidokuData;
    unsigned char* kidokuptr;
    BOOL isWatchKidoku;
    
    TYScriptPoint textgosubPoint;
    int textgosubPointIndex;
    
    NSMutableSet *undefinedComSet;
}
+(id)sharedEngine;
-(id)initWithContentsOfFile:(NSString*)path;

// 処理を依頼するオブジェクトを指定
-(void)setController:(id)cont stageManager:(id)manager;

-(void)runScript;
-(BOOL)eval:(NSMutableArray*)argments;
-(BOOL)breakRun;
-(void)setBreakRun:(BOOL)aFlag;
-(void)lineEnd; // 行の終了を通知。
-(void)jump:(unsigned)target;
-(void)labeljump:(NSString*)label;
-(void)labeljump:(NSString*)label fromLine:(int)line reverse:(BOOL)aBool;
-(void)jumpToLoopEnd;
-(void)saveExternalData;
//-(void)saveGlobal;
//-(void)saveLabelLog;

-(int)runLine;
-(void)moveRunLine:(int)aInt;
-(void)incRunColumn;
-(void)resetRunColumn;

-(void)registLoop:(char *)currentchars;
-(void)registLoopTargetID:(id)varID initValue:(id)initVal limitValue:(id)limitVal step:(id)stepVal; 
-(int)splitArgNormal:(char *)currentchars Array:(NSMutableArray *)cmdArray;
-(int)splitArgCondition:(char**)currentcharsptr trueflg:(BOOL)trueflg;
-(BOOL)isMultLineSupport:(NSString*)command;
-(char*)nextLine;
-(id)getIdNoOfArgment:(TYArgment *)argment;
-(id)getArrayIdOfArgment:(NSString *)argment;
-(id)getValueOfArgment:(NSString *)argment;
-(id)getStringOfArgment:(NSString *)argment;
-(id)getEffectNoOfArgments:(NSMutableArray*)argments;
-(NSString*)stringOfReplaceVars:(NSString *)aString;
-(BOOL)judgefchk:(NSString*)file;
-(BOOL)judgelchk:(NSString*)label;
-(BOOL)judgeCondition:(NSString*)condition string:(BOOL)aBool;

-(void)addLabelLog:(NSString*)string;

-(id)intNumberWithVarID:(id)varID;
-(id)stringValueWithVarID:(id)varID;
-(id)intNumberWithAlias:(NSString*)aStr;
-(id)stringValueWithAlias:(NSString*)aStr;

-(void)movRepeat:(int)repeat_times argments:(NSMutableArray*)argments;

-(void)fireOfTrap;

-(void)fireOfTextgosub:(NSString*)aLabel;

-(void)enableKidoku; // 既読スキップをフォロー開始
-(void)setWatchKidoku:(BOOL)aBool;

-(void)resetGame;

-(BOOL)isModeSVGA;
-(int)scanGlobalValue;

-(void)startPrint:(NSString*)aStr;

// 公開ハンドラ
-(void)ty_mov:(NSMutableArray *)argments;
-(void)ty_goto:(NSMutableArray *)argments;
-(void)ty_nsa:(NSMutableArray *)argments;
-(void)ty_trap:(NSMutableArray *)argments;

@end

#define DEFALUT_NSA_ARCHIVE @"arc%@.nsa"
#define GLOBAL_SAVEFILE_NAME @"gloval.sav"
#define MULTILINE_CMD_STRINGS @"select",@"selnum",@"selgosub",@"csel"
#define LABELLOG_FILENAME @"NScrllog.dat"
#define KIDOKU_FILENAME @"ccs_kidoku.dat"

extern NSString* const TYNScriptRunTimeErrorException;
extern NSString* const TYNScriptSyntaxErrorException;