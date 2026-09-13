//
//  TYScriptEngine.m
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

#import "TYMainController.h"
#import "TYScriptEngine.h"
#import "TYResourceServer.h"
#import "TY-NSStringAddition.h"
#import "TYStageManager.h"
#import "TYMiscUtil.h"
#import "TYFileLog.h"
#import "TYEnviroment.h"
#import "TYInputStrController.h"
#import "TYArgment.h"
#import "TYStringVarArgment.h"
#import "TYIntVarArgment.h"
#import "TYConstStringArgment.h"
#import "TYNsaArchiver.h"
#import "TYScripterValues.h"

#import "StopNSLog.h"

@implementation TYScriptEngine

// gnscripterからコピペ(ぉ
#define myisdigit(c) ((c) >= '0' && (c) <= '9')
#define myishexdigit(c) (myisdigit(c) || (c) >= 'a' && (c) <= 'f' || (c) >= 'A' && (c) <= 'F')
#define myisalpha(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z' || (c) == '_')
#define myisalnum(c) (myisdigit(c) || myisalpha(c))
#define myvarchar(c) (myisalnum(c) || (c) == '%' || (c) == '$' || (c) == '[' || (c) == ']' || (c) == '?')

#define getbit(b,i) (*((b)+((i)>>3)) & (1 << ((i) & 0x7)))

#define MAX_LINE_LENGTH 1024

NSString* const TYNScriptRunTimeErrorException=@"TYNScriptRunTimeErrorException";
NSString* const TYNScriptSyntaxErrorException=@"TYNScriptSyntaxErrorException";

static id sharedEngine;

const static char *newLineCodeUnix="\n";
const static char *newLineCodeWin="\r\n";
const static char *newLineCodeMac="\r";

static int slideColumn(char**,unsigned); // 列スキップ処理

static int slideColumn(char **ptr,unsigned toColumn)
{
    unsigned column=0;
    BOOL escape=NO;
    
    while(column<toColumn){
        if(**ptr=='\0'){
            return 1; // line up
        }
        if(**ptr=='"'){
            escape = (escape) ? NO : YES;
        }
        if((!escape)&&(**ptr==':')){
            column++;
        }
        (*ptr)++;
    }
    return 0;
}

static int get_item(char**,char*);
static int get_vararg(char**,char*);
static void space_skip(char**);

// 引数を取得し、そのコンテキストを返す。
typedef enum { TYNoArgContext=-1,TYUndefinedArgContext,TYOperandArgContext,TYStringArgContext,TYIntergerArgContext,TYSeparaterArgContext,TYChainArgContext } TYArgContext;

static int get_item(char** ptr,char* buffer)
{
    int context=TYUndefinedArgContext;

    if( (**ptr=='%') || (**ptr=='?') ){
        context = TYIntergerArgContext;
        get_vararg(ptr,buffer);
        return context;
    } else if( **ptr=='$') {
        context = TYStringArgContext;
        get_vararg(ptr,buffer);
        return context;
    } else if( **ptr=='"' ) {
        context = TYStringArgContext;
        *buffer=**ptr; buffer++; (*ptr)++;
        while(**ptr && **ptr != '"'){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer=**ptr; buffer++; (*ptr)++;
        *buffer='\0';
        return context;
    } else if(**ptr=='(') {
        char strbuffer[MAX_LINE_LENGTH+1];
        // MEMO (string) string string 構文に対応
        context = TYStringArgContext;

        *buffer = **ptr;
        (*ptr)++; buffer++;
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *buffer = '\0';
        strcat(buffer,strbuffer);
        buffer += strlen(strbuffer);
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }        
        *buffer = **ptr;
        (*ptr)++; buffer++;
        // second
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *(buffer) = '\0';
        strcat(strbuffer," "); // Obj-Cのパーサで楽するために必ずスペースを追加
        strcat(buffer,strbuffer);
        buffer += strlen(strbuffer);
        // third
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *(buffer) = '\0';
        strcat(buffer,strbuffer);

        return context;
    } else if(isdigit(**ptr)||(**ptr=='-')) {
        context = TYIntergerArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;        
        while(**ptr && isdigit(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer= '\0';
        return context;
    } else if(isalpha(**ptr)) {
        context = TYUndefinedArgContext;
        while(**ptr && myisalnum(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr && strchr("!<>=",**ptr)){
        context = TYOperandArgContext;
        while(**ptr && strchr("!<>=",**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr==',') {
        context = TYSeparaterArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        *buffer='\0';
        return context;
    } else if(**ptr=='&') {
        context = TYChainArgContext;
        while(**ptr && **ptr=='&'){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr=='*' || **ptr=='#') {
        context = TYStringArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        while(**ptr && myisalnum(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    }
        
    return TYNoArgContext;
}

static int get_vararg(char** ptr,char* buffer)
{
    while(**ptr){
        if(isalnum(**ptr)){
            while(**ptr && (myvarchar(**ptr))){
                *buffer = **ptr;
                buffer++; (*ptr)++;
            }
            *buffer='\0';
            return 0;
            /*
        } else if(isdigit(**ptr)){
            while(**ptr && (isdigit(**ptr)) ){
                *buffer = **ptr;
                buffer++; (*ptr)++;
            }
            if((**ptr == '[')||(**ptr == ']'))
                continue;
                
            *buffer='\0';
            return 0;
            */
        } else {
            *buffer = **ptr;
            buffer++; (*ptr)++;
        }
    }
    
    *buffer='\0';

    return 0;
}

static void space_skip(char** ptr)
{
    while(**ptr && isspace(**ptr))
        (*ptr)++;
}

static __inline__ TYScriptPoint TYMakeScriptPoint(unsigned line,unsigned column)
{
    TYScriptPoint scPoint;
    
    scPoint.line = line;
    scPoint.column = column;
    
    return scPoint;
}

extern void TYParserSetCString(const char *ptr);
extern int yyparse();

// プログラム中で起動するエンジンは常に一つ。それを返す。
+(id)sharedEngine
{
    return sharedEngine;
}

// 指定されたパスからスクリプトファイルを探し、読み込み、デコード、初期化(行ごとにスプリットしてArrayに格納）を行う。
-(id)initWithContentsOfFile:(NSString*)path
{
    NSFileManager *fileManager;
    NSMutableData *ScriptData;
    NSString *truePath;
    NSData *addData;
    NSArray *filesArray;
    unsigned char* encodedCharptr;
    unsigned scriptLength;
    const unsigned char *stringSourceptr;
    int	i;
    enum { mode_multi,mode_single } scriptMode=mode_single;
    id arp;
    
    self = [ super init ];
    
    // pathフォルダ内を検索してスクリプトファイルをさがす。
    // 0.txt~を探し、それがなければnscript.datを探す。
    fileManager = [ NSFileManager defaultManager ];
    filesArray = [ fileManager directoryContentsAtPath:path ];
    for(i = 0; i < [ filesArray count ]; i++ ){
        if( [ [ filesArray objectAtIndex:i ] isEqualToString:@"0.txt" ]){
            scriptMode=mode_multi; 
            break;
        }
    }
    
    if(scriptMode==mode_single){
        truePath = [ [ NSString stringWithString:path ] stringByAppendingPathComponent:@"nscript.dat" ];
    
        // 暗号化されたスクリプトをバイナリのまま読み込み
        ScriptData = [ [ NSMutableData alloc ] initWithContentsOfFile:truePath ]; // 不要になった時点で即座にリリースする
        if( ScriptData == nil){
            NSLog(@"Script File is not Found");
            return nil;
        }

        encodedCharptr = [ ScriptData mutableBytes ];

        // 変換用バッファー領域の確保
        scriptLength = [ ScriptData length ];
    
        // 各バイトを132によるXORで変換
        for( i=0; i < scriptLength; i++,encodedCharptr++){
                *encodedCharptr = (*encodedCharptr) ^ 132;
        }
        stringSourceptr = [ ScriptData bytes ];
        
    } else {
        // 暗号化されていないテキストファイル 0.txt~を読み込み、連結していく。
        i=0;

        ScriptData = [ [ NSMutableData alloc ] initWithCapacity:1 ]; // 不要になった時点で即座にリリースする
        arp = [ [ NSAutoreleasePool alloc ] init ];

        while(1){
            addData = [ [ [ NSData alloc ] initWithContentsOfFile:
                            [ path stringByAppendingPathComponent:
                                [ NSString stringWithFormat:@"%d.txt",i ] ] ] autorelease ];
            if(addData){
                [ ScriptData appendData:addData ];
                [ ScriptData appendData:[ NSData dataWithBytes:"\r\n" length:2 ] ];
            } else {
                break;
            }
            i++;
        }
        scriptLength = [ ScriptData length ];
        stringSourceptr = [ ScriptData bytes ];
        [ arp release ];
    }
    
    // 改行コードの判別を行う。
    // MEMO:暗号化されてない場合でなおかつスクリプトが複数ファイルに分かれていると、改行コードはCR+LF限定。
    for(i =0 ; i < scriptLength ; i++){
        if (stringSourceptr[i] == '\n'){
            strcpy(newLineCode,newLineCodeUnix);
            break;
        } else if (stringSourceptr[i] == '\r'){
            if(stringSourceptr[i+1] == '\n'){
                strcpy(newLineCode,newLineCodeWin);
                break;
            } else {
                strcpy(newLineCode,newLineCodeMac);
            }
        }
    }
    
    // 変換したバイトデータからNSStringの生成、および行単位での配列への格納を行う。
    // encodingは"Japanese(Win,DOS)"を指定する。
    // MEMO:初期化時点でラベルマップを作成する必要がある。多重定義があればエラーにすること。
    {
        NSString *str;
        str = [ [ [ NSString alloc ] initWithData:ScriptData encoding:NSShiftJISStringEncoding ] autorelease ];
        [ ScriptData release ];
        ScriptArray = [ str componentsSeparatedByString:[ NSString stringWithCSJISString:newLineCode ] ];
        [ ScriptArray retain ];
        //[ str release ]; //releaseすると落ちる。Arrayには参照しか格納されていないということ？
    }

    // ラベル辞書作成
    {
        NSCharacterSet *cset=[ NSCharacterSet varNameCharacterSet ];
        NSEnumerator *enu=[ ScriptArray objectEnumerator ];
        int line=0;
        NSString *buff;
        id tmp;
        NSScanner *scanner;

        labelDict = [ [ NSMutableDictionary alloc ] initWithCapacity:[ ScriptArray count ] /512 ];
        
        while(tmp = [ enu nextObject ]){
            scanner = [ NSScanner scannerWithString:tmp ];

            [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                                 intoString:nil ];
            if([ scanner scanString:@"*" intoString:nil ]){ // 行の前後に空白が入っても大丈夫
                [ scanner scanCharactersFromSet:cset
                                        intoString:&buff ];
                [ labelDict setObject:[ NSNumber numberWithInt:line ]
                               forKey:[ @"*" stringByAppendingString:[ buff lowercaseString ] ] ];
            }

            line++;
        }
    }

    
    // その他のインスタンス変数の初期化
    numAliases = [ [ NSMutableDictionary alloc ] initWithCapacity:1 ];
    strAliases = [ [ NSMutableDictionary alloc ] initWithCapacity:1 ];
    scripterValues = [ [ TYScripterValues ScripterValues ] retain ];
    multiLineCmdStringSet = [ [ NSSet alloc ] initWithObjects:MULTILINE_CMD_STRINGS,nil ];
    returnStack = [ [ NSMutableArray arrayWithCapacity:1 ] retain ];
    timerStartDate = [ [ NSDate alloc ] init ];
    loopStack = [ [ NSMutableArray arrayWithCapacity:1 ] retain ];
    textgosubPointIndex = -1;
    undefinedComSet = [ [ NSMutableSet set ] retain ];

    // グローバル変数のロード前にvalue宣言のチェック
    {
        int value = [ self scanGlobalValue ];
        if (value > 0) {
            NSLog(@"change global ID bottom = %d",value);
            [ TYScripterValues setMinGlobalNo:value ];
        }
    }
    
    // グローバル変数は開始直後にあらかじめロードされる。
    [ scripterValues loadGlobalValues:
        [ getNScrRootDirectory() stringByAppendingPathComponent:GLOBAL_SAVEFILE_NAME ] ];

    // 開始位置初期化
    [ self labeljump:@"*define" ];

    // 乱数の種を設定
    {
        NSCalendarDate *cDate=[ NSCalendarDate calendarDate ];
        unsigned int seed= ([ cDate hourOfDay ] *60 +[ cDate minuteOfHour ]) *60 +[ cDate secondOfMinute ];
        srandom(seed);
    }
    // sharedに格納
    sharedEngine = self;
    
    return self;
}

-(BOOL)isModeSVGA
{
    NSScanner *scanner;

    scanner = [ NSScanner scannerWithString:[ ScriptArray objectAtIndex:0 ] ];
    [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                         intoString:NULL ];
    return [ scanner scanString:@";mode800" intoString:NULL ];
    
}

-(int)scanGlobalValue
{
    NSScanner *scanner;
    NSString *strBuf;

    scanner = [ NSScanner scannerWithString:[ ScriptArray objectAtIndex:0 ] ];
    [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                         intoString:NULL ];
    [scanner scanUpToString:@"value" intoString:NULL ];
    if ([ scanner isAtEnd ] == YES) {
        return 0;
    } else {
        [ scanner scanString:@"value" intoString:NULL ];
        if ([ scanner scanCharactersFromSet:[ NSCharacterSet decimalDigitCharacterSet ]
                                 intoString:&strBuf ] == YES) {
            return [ strBuf intValue ];
        }
        
        return 0;
    }    
}

-(id)encodeWithSaveData
{
    NSDictionary *aDict;
    NSData *valueData;
    NSValue *pointValue;
    //TYScriptPoint point;

    
    valueData = [ scripterValues encodeWithSaveData ];

    /*
    {
        aDict = [ NSDictionary dictionaryWithObjectsAndKeys:valueData,TYLocalValuesSaveData,nil ];
    
        return aDict;
    }
     */


    //point = TYMakeScriptPoint(runLine,runColumn);
    // 選択肢のクリック待ちの間に呼ばれたのであれば、該当行（列）の処理を復帰時にやり直す必要があるので。
    if([ (TYMainController*)controller status ]==TYSelectStatus){
        if(savePoint.column == 0){
            savePoint.line--;
        }
    } else if([ (TYMainController*)controller status ]==TYScriptRunningStatus){
        // スクリプト処理中なら再開時に次のカラムに進んでいる必要がある。
        savePoint.column++;
    }
    pointValue = [ NSValue valueWithBytes:&savePoint objCType:@encode(TYScriptPoint) ];

    if(textgosubPointIndex == -1){
        aDict = [ NSDictionary dictionaryWithObjectsAndKeys:valueData,TYLocalValuesSaveData,
            pointValue,TYScriptPointSaveData,
            returnStack,TYReturnStackSaveData,nil ];
    } else {
        aDict = [ NSDictionary dictionaryWithObjectsAndKeys:valueData,TYLocalValuesSaveData,
            pointValue,TYScriptPointSaveData,
            returnStack,TYReturnStackSaveData,
            [ NSNumber numberWithInt:textgosubPointIndex ] , TYTextgosubIndexSaveData ,nil ];
    }

    return aDict;

}

-(void)decodeWithSaveData:(id)aObject
{
    id tmp;
    TYScriptPoint point;
    
    [ scripterValues decodeWithSaveData:[ aObject objectForKey:TYLocalValuesSaveData ] ];

    tmp = [ aObject objectForKey:TYReturnStackSaveData ];
    if(tmp){
        if(returnStack)
            [ returnStack release ];
        returnStack = [ tmp retain ];
    }

    tmp = [ aObject objectForKey:TYScriptPointSaveData ];
    if(tmp){
        [ tmp getValue:&point ];

        runLine=point.line;
        runColumn=point.column;
    }
    
    
    tmp = [ aObject objectForKey:TYTextgosubIndexSaveData ];
    if(tmp){
        textgosubPointIndex = [ tmp intValue ];
    } else {
        textgosubPointIndex = -1;
    }
}

-(void)resetGame
{
    [ scripterValues resetGame ];

    [ returnStack release ];
    returnStack = [ [ NSMutableArray arrayWithCapacity:1 ] retain ];
    
    textgosubPointIndex = -1;

    [ self labeljump:@"*start" ];
}

-(void)setController:(id)cont stageManager:(id)manager
{
    controller = cont;
    stageManager = manager;
}

// NScriptを実行します。スクリプトを解釈し、コマンド名からメソッドを生成して起動します。
-(void)runScript
{
    NSString *currentString;
    const char *startptr;
    char *currentchars;
    char command[MAX_LINE_LENGTH]; 
    NSMutableArray *cmdArray;
    int j;
    BOOL conditionResult;
    
    breakRun = NO;
    
    [ controller setStatus:TYScriptRunningStatus ];
    // イベントを待つ必要が来るまで、処理を続けます。
    while(1){
        // trapの捕捉チェック
        if(labelWithTrap && fireOfTrap){
            [ labelWithTrap autorelease ];
            [ self labeljump:labelWithTrap ];
            [ controller setTrap:NO ];
            fireOfTrap=NO;
            labelWithTrap=nil;
        }
        
        // 列の処理中でなければ行を進める
        if( runColumn == 0){
            runLine++;
            //NSLog2(@"%d",runLine);
        }
        currentString = [ ScriptArray objectAtIndex:runLine ];
        [ currentData release ];
        currentData = [ currentString getCSJISData ];
        [ currentData retain ];
        currentchars = [ currentData mutableBytes ];
        // バッファオーバーフロー対策
        if([ currentData length ] > MAX_LINE_LENGTH){
            NSLog(@"too long line. omit data over %d",MAX_LINE_LENGTH);
            currentchars[MAX_LINE_LENGTH -1] = '\0';
        }
        startptr = currentchars;
        // 列の処理中なら開始位置までずらす
        if(runColumn){
            if(slideColumn(&currentchars,runColumn)){
                // 次行にシフトした
                runLine++; runColumn=0;
                currentString = [ ScriptArray objectAtIndex:runLine ];
                [ currentData release ];
                currentData = [ currentString getCSJISData ];
                [ currentData retain ];
                currentchars = [ currentData mutableBytes ];
                startptr = currentchars;                
            }
        }
        savePoint.line=runLine;
        savePoint.column=runColumn;

        TYParserSetCString(currentchars);
        yyparse();
        
        /*
        // 先頭スペースの読み飛ばし
        while((*currentchars)&&(isspace(*currentchars))){
            currentchars++;
        }
        
        // ラベルならラベルログを追加し、次へ
        if(currentchars[0]=='*'){
            if(labelLog)
                [ labelLog addLog:[ NSString stringWithCSJISString:currentchars ] ];
            runColumn=0;
            continue;
        }
        // コメント、空行なら次へ
        if( (strchr(";*~",currentchars[0]))||(currentchars[0]=='\0') ){
            runColumn=0;
            continue;
        }

        if (isalpha(*currentchars)) {
        // 命令文
            
            // チャンク毎に配列に格納する
            cmdArray = [ NSMutableArray array ];
        
            // コマンドの取得。コマンドが終了するまで読み込み。
            for(j=0; myisalnum(*currentchars); currentchars++,j++){
                command[j] = *currentchars;
            }
            command[j] = '\0';
            [ cmdArray addObject:[ [ NSString stringWithCSJISString:command ] lowercaseString ] ];

            // 通常の命令文とif系で講文解釈が違うので分岐
            if( strncmp(command,"for",3)==0 ){
                [ self registLoop:currentchars ];
                continue;
            } else if( (strncmp(command,"if",2)!=0)&&(strncmp(command,"notif",5)!=0) ){
                [ self splitArgNormal:currentchars Array:cmdArray ]; // 通常の引き数解釈
            } else {
                if(strncmp(command,"if",2)==0){
                    conditionResult = [ self splitArgCondition:&currentchars trueflg:YES ]; // if
                } else {
                    conditionResult = [ self splitArgCondition:&currentchars trueflg:NO ]; // notif
                }

                if(conditionResult==NO){
                    runColumn=0;
                    continue;
                } else {
                    // 先頭スペースの読み飛ばし
                    while((*currentchars)&&(*currentchars==' ')){
                        currentchars++;
                    }
                    // チャンク毎に配列に格納する
                    cmdArray = [ NSMutableArray array ];
                
                    // コマンドの取得。
                    for(j=0; (*currentchars)&&(myisalnum(*currentchars)); currentchars++,j++){
                        command[j] = *currentchars;
                    }
                    command[j] = '\0';
                    if( strncmp(command,"for",3)==0 ){
                        [ self registLoop:currentchars ];
                        continue;
                    }
                    [ cmdArray addObject:[ [ NSString stringWithCSJISString:command ] lowercaseString ] ];
                    [ self splitArgNormal:currentchars Array:cmdArray ];
                }
            }

            // スクリプトを実行。
            if(![ self eval:cmdArray ]){
                NSLog(@"Undefined script '%@' at LINE %d\n",[ cmdArray objectAtIndex:0 ],runLine+1);
            }
        } else if (*currentchars==':') {
            runColumn++;
        } else {
            // 表示文の時
            NSString* printString;
            
            if(currentchars==startptr){
                printString = [ ScriptArray objectAtIndex:runLine ];
            } else {
                printString = [ NSString stringWithCSJISString:currentchars ];
            }
            while(*currentchars &&(*currentchars !=':'))
                currentchars++;
            if(!currentchars){
                runColumn=0;
            } else {
                runColumn++;
            }
            // 既読スキップ使用時のチェック
            if(kidokuptr){
                if(isWatchKidoku){
                    //NSLog(@"runLine=%d,kidokuchr=%d,bit = %d",runLine,*(kidokuptr+(runLine>>3)),getbit(kidokuptr,runLine));
                    if(!(*(kidokuptr+(runLine>>3)) & (1 << (runLine & 0x7)))){
                        [ stageManager setSkipStatus:TYNoSkip ];
                        isWatchKidoku=NO;
                        *(kidokuptr+(runLine >> 3)) |= 1 << (runLine & 0x7);
                        //NSLog(@"runLine=%d,kidokuchr=%d,bit = %d",runLine,*(kidokuptr+(runLine>>3)),getbit(kidokuptr,runLine));
                    }
                } else {
                    *(kidokuptr+(runLine >> 3)) |= 1 << (runLine & 0x7);
   //                 NSLog(@"bit = %d",getbit(kidokuptr,runLine));
                }
            }
            [ stageManager textPrint:printString ];
        }
         */
        
        if(breakRun) {
            break;
        }
    }
}

-(void)ty__print:(NSMutableArray*)argments
{
    NSString *aStr = [ argments objectAtIndex:1 ];
    
    // 既読スキップ使用時のチェック
    if(kidokuptr){
        if(isWatchKidoku){
            if(!(*(kidokuptr+(runLine>>3)) & (1 << (runLine & 0x7)))){
                [ stageManager setSkipStatus:TYNoSkip ];
                isWatchKidoku=NO;
                *(kidokuptr+(runLine >> 3)) |= 1 << (runLine & 0x7);
            }
        } else {
            *(kidokuptr+(runLine >> 3)) |= 1 << (runLine & 0x7);
        }
    }
    
    [ stageManager textPrint:aStr ];
 }

-(BOOL)eval:(NSMutableArray*)argments
{
    SEL cmdSEL;
    
    // コマンド名からセレクタを生成（プレフィックス"ty_"を付与する。メソッド名の衝突防止）
    // NScriptでは大小文字の区別をしない。
    cmdSEL = NSSelectorFromString([ [ NSString stringWithFormat:@"ty_%@:",[ argments objectAtIndex:0] ] lowercaseString ]);
    NSLog2(@"[%5d]%@",runLine+1,NSStringFromSelector(cmdSEL));
    
    // セレクタを自身が実装しているかどうかチェック。
    if( [ self respondsToSelector:cmdSEL ]){
        // セレクタをコール。ほんとは例外のハンドリングした方がいいのかね～
        [ self performSelector:cmdSEL withObject:argments ];
    } else if ([ stageManager respondsToSelector:cmdSEL ]){
        [ stageManager performSelector:cmdSEL withObject:argments ];
    } else if ([ controller respondsToSelector:cmdSEL ]){
        [ controller performSelector:cmdSEL withObject:argments ];
    } else {
        // vXXXX:およびdvXXXX:への対応
        NSScanner *scanner=[ NSScanner scannerWithString:[ argments objectAtIndex:0 ] ];
        NSString *cmdStr,*numStr;
        
        if([ scanner scanString:@"dv" intoString:&cmdStr ] || 
           [ scanner scanString:@"v" intoString:&cmdStr ]){
            [ scanner scanCharactersFromSet:[ NSCharacterSet decimalDigitCharacterSet ]
                                 intoString:&numStr ];                                 
            [ controller performSelector:NSSelectorFromString([ [ NSString stringWithFormat:@"ty_%@:",cmdStr ] lowercaseString ])
                                withObject:[ NSMutableArray arrayWithObjects:cmdStr,numStr,nil ] ];
        } else {
            if ([ undefinedComSet containsObject:[ argments objectAtIndex:0 ] ] == NO) {
                NSLog(@"Undefined script '%@' at LINE %d\n",[ argments objectAtIndex:0 ],runLine+1);
                [ undefinedComSet addObject:[ argments objectAtIndex:0 ] ];
            }
            return NO;
        }
    }
    
    return YES;
}

-(BOOL)breakRun
{
    return breakRun;
}

-(void)setBreakRun:(BOOL)aFlag
{
    breakRun = aFlag;
}

-(void)lineEnd // 行の終了を通知。
{
    //runColumn++;
}

// 引き数部を分割して、arrayに格納する
-(int)splitArgNormal:(char *)currentchars Array:(NSMutableArray *)cmdArray;
{
    char * buffer=currentchars;
    char command[MAX_LINE_LENGTH];
    int context; // ダミーの値
    BOOL multiLinePossible=YES;

    // 引数とれなくなるまでループ
    space_skip(&buffer);
    context = get_item(&buffer,command);

    while(context!=TYNoArgContext){
        [ cmdArray addObject:[ NSString stringWithCSJISString:command ] ];
        space_skip(&buffer);
        context = get_item(&buffer,command);
        if(context==TYSeparaterArgContext){
            space_skip(&buffer);
            context = get_item(&buffer,command);
        } else {
            multiLinePossible=NO;
            break;
        }
    }

    // 列のインクリメントチェック。そうでなければ複数行サポートコマンドかチェック
    if(*buffer==':'){
        runColumn++;
    } else {
        if(multiLinePossible && [ self isMultLineSupport:[ cmdArray objectAtIndex:0 ] ]){
            [ self splitArgNormal:[ self nextLine ] Array:cmdArray ];
        } else {
            runColumn=0;
        }
    }        

    return 0;

    /*
    int i=0,j=0;
    char command[256];
    enum { aas_SpaceSkip,aas_CommaSkip,aas_GetArg,aas_GetArgStr } argAnalyzerStat;
    BOOL chained=NO;

    argAnalyzerStat = aas_SpaceSkip;
    while(currentchars[i]){
        switch (argAnalyzerStat) {
        case aas_SpaceSkip:	// 次の引き数が始まるまでのスペースを飛ばします。
            if(!isspace(currentchars[i])){
                if(currentchars[i] == ':'){ // 命令文の連結子
                    runColumn++;
                    chained=YES;
                    break;
                }
                
                j=0;
                command[j] = currentchars[i];
                j++;
                if(currentchars[i] == '"'){
                    argAnalyzerStat = aas_GetArgStr;
                } else {
                    argAnalyzerStat = aas_GetArg;
                }
            }
            i++;
            break;

        case aas_GetArgStr: // 文字列定数を取得します。
            command[j] = currentchars[i];
            j++;
            if(currentchars[i] == '"'){
                argAnalyzerStat = aas_CommaSkip;
                //　引き数を生成
                command[j] = '\0';
                [ cmdArray addObject:[ NSString stringWithCSJISString:command ] ];
            }
            i++;
            break;
                    
        case aas_GetArg: // それ以外の引き数を取得します。
            if( (currentchars[i] != ',')&&(!isspace(currentchars[i]))&&(currentchars[i] != ':') ){
                command[j] = currentchars[i];
                j++;
                i++;
            } else {
                argAnalyzerStat = aas_CommaSkip;
                // 引き数を生成
                command[j] = '\0';
                [ cmdArray addObject:[ NSString stringWithCSJISString:command ] ];
                if(currentchars[i] == ':' ){ // 連結文指定子
                    runColumn++;
                    chained=YES;
                    break;
                }
            }
            break;

        case aas_CommaSkip: // 引き数区切りのカンマまで飛ばします。
            if(currentchars[i] == ','){
                argAnalyzerStat = aas_SpaceSkip;
            } else if(currentchars[i] == ':' ) {
                runColumn++;
                chained=YES;
                break;
            } else if(!isspace(currentchars[i])) {
                // カンマのない不正な構文
                //[ NSException raise:@"TYNScriptSyntaxError" format:@"lack comma" ];
                // そのコマンドで引数が足りない場合のみエラーを吐くので、ここではエラーとしない。
            }
            i++;
            break;
        
        default:
            NSLog(@"Show Debbuger!\n");
            return -1;
        }
        
        // 連結子が見つかって、連結部が格納されていたら
        if(chained){
            break;
        }
    }
    
    // 引き数の生成途中でブレイクしていたら
    if(argAnalyzerStat == aas_GetArg){
        command[j] = '\0';
        [ cmdArray addObject:[ NSString stringWithCSJISString:command ] ];
    } else if(argAnalyzerStat == aas_SpaceSkip){
        // ２つ目以降の引数を探す途中でブレイクしていたら
        if([ self isMultLineSupport:[ cmdArray objectAtIndex:0 ] ]){
            [ self splitArgNormal:[ self nextLine ] Array:cmdArray ];
        }
    }

    if(!chained){
        runColumn=0;
    }
    
    return 0;
     */
}

// コマンドが複数行に渡る記述を許されているかどうかを返す
-(BOOL)isMultLineSupport:(NSString*)command
{
    return [ multiLineCmdStringSet containsObject:command ];
}

// 次の行へ処理を進め、スクリプトを取り出す。
-(char*)nextLine
{
    runLine++;
    [ currentData release ];
    currentData = [ [ ScriptArray objectAtIndex:runLine ] getCSJISData ];
    [ currentData retain ];
    return (unsigned char*)[ currentData mutableBytes ];
}

// forループの登録
/*
-(void)registLoop:(char *)currentchars
{
    char buffer[256];
    int context;
    id targetNo;
    int initValue;
    int limit;
    int step;
    NSString *var;
    TYLoopStruct loop;
    
    // バインドする変数番号を取得
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    NSAssert((context==TYIntergerArgContext&& buffer[0] == '%'),
             TYNScriptSyntaxErrorException);
    var = [ NSString stringWithCSJISString:buffer ];
    targetNo = [ self getIdNoOfArgment:var ];
    // 演算子=のチェック
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    NSAssert((context==TYOperandArgContext && strcmp(buffer,"=")==0),
             TYNScriptSyntaxErrorException);
    // 初期化する値を取得
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    NSAssert((context==TYIntergerArgContext || context==TYUndefinedArgContext),
             TYNScriptSyntaxErrorException);
    var = [ NSString stringWithCSJISString:buffer ];
    initValue = [ [ self getValueOfArgment:var ] intValue ];
    // to句チェック
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    NSAssert((context==TYUndefinedArgContext),TYNScriptSyntaxErrorException);
    // 脱出条件の値を取得
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    NSAssert((context==TYUndefinedArgContext || context==TYIntergerArgContext),
             TYNScriptSyntaxErrorException);
    var = [ NSString stringWithCSJISString:buffer ];
    limit = [ [ self getValueOfArgment:var ] intValue ];
    // step句の存在チェック
    space_skip(&currentchars);
    context = get_item(&currentchars,buffer);
    if(context==TYNoArgContext){
        step=1;
    } else {
        NSAssert((context==TYUndefinedArgContext && (strcmp(buffer,"step")==0)),
                 TYNScriptSyntaxErrorException);
        // step値の取得
        space_skip(&currentchars);
        context = get_item(&currentchars,buffer);
        NSAssert((context==TYUndefinedArgContext || context==TYIntergerArgContext),
                    TYNScriptSyntaxErrorException);
        var = [ NSString stringWithCSJISString:buffer ];
        step = [ [ self getValueOfArgment:var ] intValue ];
    }

    // 変数に初期化値セット
    [ scripterValues setIntValue:targetNo object:[ NSNumber numberWithInt:initValue ] ];
    
    // 初期化値が既にループの脱出条件を満たしている場合
    if(step > 0){
        if(initValue > limit){
            [ self jumpToLoopEnd ];// nextにジャンプ?
            return;
        }
    } else if(step < 0){
        if(initValue < limit){
            [ self jumpToLoopEnd ];// nextにジャンプ?
            return;
        }
    }

    // loopオブジェクト作成。スタックに積む。
    loop = TYMakeLoopStruct(TYMakeScriptPoint(runLine,runColumn),targetNo,limit,step);
    [ loopStack addObject:[ [ [ TYLoopValue alloc ] initWithStruct:loop ] autorelease ] ];
    
    runColumn++;
}
*/

-(void)registLoopTargetID:(id)varID initValue:(id)initVal limitValue:(id)limitVal step:(id)stepVal
{
    TYLoopStruct loop;    

    NSLog2(@"push for stack at %d",runLine);
    // 変数に初期化値セット
    [ scripterValues setIntValue:varID object:[ initVal intNumber ] ];
    
    // MEMO:初期化値が既にループの脱出条件を満たしている場合の対処が必要?

    // loopオブジェクト作成。スタックに積む。
    loop = TYMakeLoopStruct(TYMakeScriptPoint(runLine,runColumn),varID,[ limitVal intValue ],[ stepVal intValue ]);
    [ loopStack addObject:[ [ [ TYLoopValue alloc ] initWithStruct:loop ] autorelease ] ];
}


static int chkNext(char**,int);

static int chkNext(char** ptr,int stColumn)
{
    int column;
    int result=-1;
    
    for(column=stColumn; !slideColumn(ptr,column); column++){
        space_skip(ptr);
        if(strncmp("next",*ptr,4)==0){
            result = column;
            break;
        }
    }
    
    return result;
}

// 直後のnext文までジャンプします。
-(void)jumpToLoopEnd
{
    char *ptr;
    int column=runColumn +1;
    // バイナリィ・ポットの連結文中のnextにも対応。
    while(1) {
        ptr = (char*)[ [ ScriptArray objectAtIndex:runLine ] cSJISString ];
        column=chkNext(&ptr,column);
        if(column >= 0){
            runColumn = column +1;
            break;
        } else {
            runLine++;
            column=0;
        }
    }
}

// if 系構文の解釈と条件判定。
-(int)splitArgCondition:(char**)currentcharsptr trueflg:(BOOL)trueflg
{
    int k,count;
    NSMutableArray *conditions;
    char buffer[MAX_LINE_LENGTH];
    char sentence[MAX_LINE_LENGTH];
    int context=TYChainArgContext;
    BOOL judgeResult;

    conditions = [ NSMutableArray array ];

    while(context==TYChainArgContext){
        space_skip(currentcharsptr);
        context = get_item(currentcharsptr,buffer);
        strcpy(sentence,buffer);
        if(context==TYIntergerArgContext){
            // 左辺確定。続けて演算子と右辺を取得
            space_skip(currentcharsptr);
            context = get_item(currentcharsptr,buffer);
            if(context != TYOperandArgContext){
                NSLog(@"invalid argment [%s]. it needs operand here",buffer);
                return NO;
            }
            strcat(sentence,buffer);

            space_skip(currentcharsptr);
            context = get_item(currentcharsptr,buffer);
            if((context != TYIntergerArgContext )&&(context != TYUndefinedArgContext)){
                NSLog(@"invalid argment [%s]. it needs interger here",buffer);
                return NO;
            }
            strcat(sentence,buffer);
            [ conditions addObject:[ NSString stringWithCSJISString:sentence ] ];

        } else if(context==TYStringArgContext) {
            // 左辺確定。続けて演算子と右辺を取得
            [ conditions addObject:@"string" ];
            space_skip(currentcharsptr);
            context = get_item(currentcharsptr,buffer);
            if(context != TYOperandArgContext){
                NSLog(@"invalid argment [%s]. it needs operand here",buffer);
                return NO;
            }
            strcat(sentence,buffer);

            space_skip(currentcharsptr);
            context = get_item(currentcharsptr,buffer);
            if((context != TYStringArgContext )&&(context != TYUndefinedArgContext)){
                NSLog(@"invalid argment [%s]. it needs string here",buffer);
                return NO;
            }
            strcat(sentence,buffer);
            [ conditions addObject:[ NSString stringWithCSJISString:sentence ] ];
        } else if(context==TYUndefinedArgContext){
            if((strcmp(buffer,"fchk")==0)||(strcmp(buffer,"lchk")==0)){
                [ conditions addObject:[ NSString stringWithCSJISString:buffer ] ];
                space_skip(currentcharsptr);
                context = get_item(currentcharsptr,buffer);
                if((context==TYStringArgContext)||(context==TYUndefinedArgContext)){
                    [ conditions addObject:[ NSString stringWithCSJISString:buffer ] ];
                } else {
                    NSLog(@"invalid argment [%s]. it needs string here",buffer);
                    return NO;
                }
            } else {
                // とりあえず左辺とみなす。続けて演算子と右辺を取得。ってストリングとか来てたらエラーなんですけどね。
                space_skip(currentcharsptr);
                context = get_item(currentcharsptr,buffer);
                if(context != TYOperandArgContext){
                    NSLog(@"invalid argment [%s]. it needs operand here",buffer);
                    return NO;
                }
                strcat(sentence,buffer);

                space_skip(currentcharsptr);
                context = get_item(currentcharsptr,buffer);
                if(context != TYOperandArgContext){
                    NSLog(@"invalid argment [%s]. it needs interger here",buffer);
                    return NO;
                }
                strcat(sentence,buffer);
                [ conditions addObject:[ NSString stringWithCSJISString:sentence ] ];

            }
        } else {
            NSLog(@"invalid argment [%s].it needs interger or \"fchk\" or \"lchk\" here",buffer);
            return NO;
        }

        space_skip(currentcharsptr);
        if(**currentcharsptr=='&'){
            context = get_item(currentcharsptr,buffer);
        } else {
            break;
        }
    }
    
    // 条件文の真偽を判定。trueflg=YESのとき全て真なら実行、trueflg=NOのとき、全て偽なら実行
    for(k = 0,count=[ conditions count ]; k < count ; k++){
        if([ [ conditions objectAtIndex:k ] isEqualToString:@"fchk" ]){
            k++;
            judgeResult = [ self judgefchk:[ conditions objectAtIndex:k ] ];
        } else if([ [ conditions objectAtIndex:k ] isEqualToString:@"lchk" ]){
            k++;
            judgeResult = [ labelLog isRead:[ self getStringOfArgment:[ conditions objectAtIndex:k ] ] ];
        } else if([ [ conditions objectAtIndex:k ] isEqualToString:@"string" ]){
            k++;
            judgeResult = [ self judgeCondition:[ conditions objectAtIndex:k ] string:YES ];
        } else {
            judgeResult = [ self judgeCondition:[ conditions objectAtIndex:k ] string:NO ];
        }
        
        if(judgeResult != trueflg){
            return NO; // 実行されないことが判明したら抜ける。
        }
    }
    
    return YES;
}

// その画像（タグ情報含む）が読み込まれたかどうかを返す。
-(BOOL)judgefchk:(NSString*)file
{
    return [ [ TYResourceServer sharedServer ] fchk:[ self getStringOfArgment:file ] ];
}

-(BOOL)judgelchk:(NSString*)label
{
    return [ labelLog isRead:[ self getStringOfArgment:label ] ];
}

-(void)addLabelLog:(NSString*)string
{
    if(labelLog)
        [ labelLog addLog:string ];
}

-(BOOL)judgeCondition:(NSString*)condition string:(BOOL)aBool
{
    char valueString1[MAX_LINE_LENGTH];
    char valueString2[MAX_LINE_LENGTH];
    char cond[3];
    const char *conditionchars;
    int value1,value2;
    const char *operandchars = "!<>=";
    int i,j;
    
    conditionchars = [ condition cSJISString ];
    
    // 左辺の取得
    for(i=0,j=0; (conditionchars[i])&&(strchr(operandchars,conditionchars[i]) == NULL) ; i++,j++ ){
        valueString1[j] = conditionchars[i];
    }
    valueString1[j] = '\0';
    
    // 演算子の取得
    for( j=0; (conditionchars[i])&&(strchr(operandchars,conditionchars[i]) != NULL) ; i++,j++ ){
        cond[j] = conditionchars[i];
    }
    cond[j] = '\0';
    
    // 右辺の取得
    for(j=0; (conditionchars[i]) ; i++,j++ ){
        valueString2[j] = conditionchars[i];
    }
    valueString2[j] = '\0';

    if(aBool){
        NSString *string1,*string2;
        // 文字列の場合の処理
        string1 = [ self getStringOfArgment:[ NSString stringWithCSJISString:valueString1 ] ];
        string2 = [ self getStringOfArgment:[ NSString stringWithCSJISString:valueString2 ] ];

        // 判定
        if( (strcmp(cond,"==")==0)||(strcmp(cond,"=")==0) ){ // 等しい
            return [ string1 isEqualToString:string2 ];
        } else if ( (strcmp(cond,"!=")==0)||(strcmp(cond,"<>")==0) ) { // 等しく無い
            return ![ string1 isEqualToString:string2 ];
        } else if (strcmp(cond,">")==0) { // 左辺＞右辺
            return ([ string1 compare:string2 ] == NSOrderedDescending);
        } else if (strcmp(cond,"<")==0) { // 左辺＜右辺
            return ([ string1 compare:string2 ] == NSOrderedAscending);
        } else if (strcmp(cond,">=")==0) { // 左辺＞＝右辺
            return  ([ string1 compare:string2 ] >= NSOrderedSame);
        } else if (strcmp(cond,"<=")==0) { // 左辺＜＝右辺
            return ([ string1 compare:string2 ] <= NSOrderedSame);
        } else {
            NSLog(@"It's illegal operand![%s]",conditionchars);
            return -1;
        }
    } else {
        // 数値の場合の処理
        value1 = [ [ self getValueOfArgment:[ NSString stringWithCSJISString:valueString1 ] ] intValue];
        value2 = [ [ self getValueOfArgment:[ NSString stringWithCSJISString:valueString2 ] ] intValue];

        // 判定
        if( (strcmp(cond,"==")==0)||(strcmp(cond,"=")==0) ){ // 等しい
            return (value1 == value2);
        } else if ( (strcmp(cond,"!=")==0)||(strcmp(cond,"<>")==0) ) { // 等しく無い
            return (value1 != value2);
        } else if (strcmp(cond,">")==0) { // 左辺＞右辺
            return (value1 > value2);
        } else if (strcmp(cond,"<")==0) { // 左辺＜右辺
            return (value1 < value2);
        } else if (strcmp(cond,">=")==0) { // 左辺＞＝右辺
            return (value1 >= value2);
        } else if (strcmp(cond,"<=")==0) { // 左辺＜＝右辺
            return (value1 <= value2);
        } else {
            NSLog(@"It's illegal operand![%s]",conditionchars);
            return -1;
        }
    }
}

-(void)jump:(unsigned)target
{
    runLine = target;
    runColumn = 0;
}

// 指定されたラベルを探しそこに移動
-(void)labeljump:(NSString*)label
{
    id tmp;

    if(tmp = [ labelDict objectForKey:[ label lowercaseString ] ]){
        // ラベルログに追加
        [ labelLog addLog:label ];
        runLine = [ tmp intValue ];
        runColumn =0;
        return;
    } else {
        NSLog(@"Not Found Label:%@",label);
        [ NSException raise:@"TYNScriptRuntimeError" format:@"Not Found Label:%@",label ];
    }

    /*
    [ self labeljump:label fromLine:0 reverse:NO ];
    return;
     */
    /*
    NSString *search_label;
    unsigned line;
    unsigned length;
    
    search_label = [ NSString stringWithFormat:@"%@",label ];
    length = [ ScriptArray count ];
    for(line=0; line < length; line++){
        if(![ ScriptArray objectAtIndex:line]){
            continue;
        }
        if([ search_label isEqualToString:[ ScriptArray objectAtIndex:line] ]){ // 行の前後に空白が入ったりするとまずいが
            runLine = line;
            runColumn = 0;
            return;
        }
    }
    
    NSLog(@"Not Found Label:%@",label);
    [ NSException raise:@"TYNScriptRuntimeError" format:@"Not Found Label:%@",label ];

    return;
    */
}

-(void)labeljump:(NSString*)label fromLine:(int)line reverse:(BOOL)aBool
{
    unsigned l;
    unsigned length;
    NSScanner *scanner;
    NSString *str;

    if(labelLog)
        [ labelLog addLog:label ];

    if(aBool==NO){
        length = [ ScriptArray count ];

        for(l=line; l < length; l++){
            str = [ ScriptArray objectAtIndex:l];
            if(![ str length ]){
                continue;
            }
            scanner = [ NSScanner scannerWithString:str ];
            [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                                 intoString:nil ];

            if([ scanner scanString:label intoString:nil ]){ // 行の前後に空白が入っても大丈夫
                if([ scanner isAtEnd ] ||
                   [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                                        intoString:nil ]){

                    runLine = l;
                    runColumn = 0;
                    return;
                }
            }
        }
    } else {
        for(l=line; l >= 0; l--){
            if(![ ScriptArray objectAtIndex:l]){
                continue;
            }
            scanner = [ NSScanner scannerWithString:[ ScriptArray objectAtIndex:l] ];
            [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                                 intoString:nil ];
            if([ scanner scanString:label intoString:nil ]){ // 行の前後に空白が入っても大丈夫
                if([ scanner isAtEnd ] ||
                   [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceAndNewlineCharacterSet ]
                                     intoString:nil ]){                    

                    runLine = l;

                    runColumn = 0;

                    return;
                }
            }
        }
    }

    NSLog(@"Not Found Label:%@",label);
    [ NSException raise:@"TYNScriptRuntimeError" format:@"Not Found Label:%@",label ];

    return;
}

// 引き数から数値オブジェクトを取り出す。MEMO:数字変数による再帰的な参照を可能にしなければならない
-(id)getValueOfArgment:(NSString *)argment
{
    /*
    NSString *firstCharStr;
    int scanValue;
    NSNumber *resultForAlias;
     */

    return [ argment intNumber ];
    /*
    firstCharStr = [ argment substringWithRange:NSMakeRange(0,1) ];
    // 1文字目が何かによって分岐
    if([ firstCharStr isEqualToString:@"%" ]) {
        // 変数の場合、Indexの指定を再帰で評価する
        return [ scripterValues getIntValue:[ self getValueOfArgment:[ argment substringFromIndex:1 ] ] ];
    } else if([ firstCharStr isEqualToString:@"?" ]){
        // 配列変数だった場合
        return [ scripterValues getIntValue:[ self getArrayIdOfArgment:argment ] ];
    }

    if([ [ NSScanner scannerWithString:argment ] scanInt:&scanValue ]){
        // 定数の場合
        return [ NSNumber numberWithInt:scanValue ];
    } else {
        // エイリアス？
        resultForAlias = [ numAliases objectForKey:[ argment lowercaseString ] ];
        if(resultForAlias){
            return resultForAlias;
        } else {
            // 例外発生
            goto ANALYZE_ERROR;
            //[ NSException raise:@"TYNScriptRuntimeError" format:@"Illegal Value Index:%@",argment ];
            //return [ NSNumber numberWithInt:0 ];
        }
    }
    
ANALYZE_ERROR:
    NSLog(@"Not Get Interger Value from [%@]\n",argment);
    return [ NSNumber numberWithInt:0 ];
    */
}

// 引き数から文字列オブジェクトを取り出す。
-(id)getStringOfArgment:(NSString *)argment
{
    /*
    NSString *firstCharStr;
    NSString *scanStr;
    NSString *resultForAlias;
     */
     return [ argment stringObj ];
     /*
    // 1文字目が何かによって分岐
    firstCharStr = [ argment substringWithRange:NSMakeRange(0,1) ];
    if( [ firstCharStr isEqualToString:@"\"" ]){
    // 文字定数の場合
        // 先頭と末尾の'"'を削除した文字列をそのまま返す。
        if([ [ NSScanner scannerWithString:[ argment substringFromIndex:1 ] ] scanUpToString:
                @"\"" intoString:&scanStr ]){
            return scanStr;
        } else {
            return @"";
        }
        //NSLog(@"get constant String %@ from %@",scanStr,argment );
        //return (scanStr) ? scanStr : @"";
    } else if( [ firstCharStr isEqualToString:@"$" ]) {
        // 変数のインデックスを再帰的に取得
        return [ scripterValues getStringValue:[ self getValueOfArgment:
                    [ argment substringFromIndex:1 ] ] ];
    } else if( [ firstCharStr isEqualToString:@"(" ]) {
        NSScanner *scanner=[ NSScanner scannerWithString:argment ];
        NSString *fileName,*trueStr,*falseStr;

        NSLog2(@"%@",argment);
        [ scanner setScanLocation:1 ];
        [ scanner scanUpToString:@")" intoString:&fileName ];
        [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
        [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceCharacterSet ]
                             intoString:nil ];
        if([ [ argment substringWithRange:NSMakeRange([ scanner scanLocation ],1) ] isEqualToString:@"\"" ]) {
            [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
            [ scanner scanUpToString:@"\"" intoString:&trueStr ];
            [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
        } else {
            [ scanner scanUpToCharactersFromSet:[ NSCharacterSet whitespaceCharacterSet ]
                                     intoString:&trueStr ];
        }

        if([ [ TYResourceServer sharedServer ] fchk:[ self getStringOfArgment:fileName ] ]){
            NSLog2(@"fileName=%@[%@],TRUE\n,result=%@[%@]",fileName,[ self getStringOfArgment:fileName ],
                   trueStr,[ self getStringOfArgment:trueStr ]);
            return [ self getStringOfArgment:trueStr ];
        } else {
            [ scanner scanCharactersFromSet:[ NSCharacterSet whitespaceCharacterSet ]
                                 intoString:nil ];
            
            if([ [ argment substringWithRange:NSMakeRange([ scanner scanLocation ],1) ] isEqualToString:@"\"" ]) {
                [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
                [ scanner scanUpToString:@"\"" intoString:&falseStr ];
            } else {
                [ scanner scanUpToCharactersFromSet:[ NSCharacterSet whitespaceCharacterSet ]
                                         intoString:&falseStr ];
            }
            NSLog2(@"fileName=%@[%@],FALSE\n,result=%@[%@]",fileName,[ self getStringOfArgment:fileName ],
                   falseStr,[ self getStringOfArgment:falseStr ]);

            return [ self getStringOfArgment:falseStr ];
        }

        

    } else {
    // エイリアスの場合
        resultForAlias = [ strAliases objectForKey:[ argment lowercaseString ] ];
        if(resultForAlias){
            return resultForAlias;
        } else {
            return argment; // エイリアスに登録されていなければ文字列をそのまま返す、文字列ラベル展開対策
            //goto ANALYZE_ERROR;
        }
    }
    */

/*    
ANALYZE_ERROR:
    NSLog(@"Not Get String Value from [%@]\n",argment);
    return nil;
 */

}

-(id)getIdNoOfArgment:(TYArgment*)argment
{
    return [ argment varID ];
    /*
    if([ argment length ] &&
       ([ [ argment substringWithRange:NSMakeRange(0,1) ] isEqualToString:@"?" ])){
       // 配列型
        return [ self getArrayIdOfArgment:argment ];
    } else {
        return [ self getValueOfArgment:[ argment substringFromIndex:1 ] ];
    } 
     */
}

-(id)getArrayIdOfArgment:(NSString *)argment
{
    return [ argment varID ];
    /*
    // 配列変数の指定を取得。
    NSMutableArray *aArray = [ NSMutableArray array ];
    NSScanner *scanner;
    NSString *bufStr;
    NSString *stopString = @"[";
    
    scanner = [ NSScanner scannerWithString:[ argment substringFromIndex:1 ] ];
    
    while([ scanner scanUpToString:stopString intoString:&bufStr ]){
        // 変数を展開して格納
        [ aArray addObject:[ self getValueOfArgment:bufStr ] ];
        
        [ scanner setScanLocation:[ scanner scanLocation ]+1 ];
        [ scanner scanString:@"[" intoString:nil ];
        stopString = @"]";
    }
    
    return aArray;
    */
}

// エフェクト番号を返す。或いは、TYEffecterの-1番に登録し、-1を返す。
-(id)getEffectNoOfArgments:(NSMutableArray*)argments
{
    int count;
    
    count = [ argments count ];
    
    if(count == 1){
        return [ self getValueOfArgment:[ argments objectAtIndex:0 ] ];
    } else {
        [ argments insertObject:[ NSNull null ] atIndex:0 ];
        [ argments insertObject:[ NSNumber numberWithInt:-1 ] atIndex:1 ];
        [ stageManager performSelector:@selector(ty_effect:) withObject:argments ];
        return [ NSNumber numberWithInt:-1 ];
    }
}

-(id)intNumberWithVarID:(id)varID
{
    return [ scripterValues getIntValue:varID ];
}

-(id)stringValueWithVarID:(id)varID
{
    return [ scripterValues getStringValue:varID ];
}

-(id)intNumberWithAlias:(NSString*)aStr
{
    return [ numAliases objectForKey:aStr ];
}

-(id)stringValueWithAlias:(NSString*)aStr
{
    return [ strAliases objectForKey:aStr ];
}

// 文字列中の変数を展開した文字列を返す。
-(NSString*)stringOfReplaceVars:(NSString *)aString
{
    NSCharacterSet *varHeaderSet = [ NSCharacterSet characterSetWithCharactersInString:@"$%" ];
    NSString *replaceStr,*str=nil,*symbol;
    NSScanner *scanner;
    NSMutableArray *array;
    
    scanner = [ NSScanner scannerWithString:aString ];
    [ scanner scanUpToCharactersFromSet:varHeaderSet intoString:&str ];
    if([ scanner isAtEnd ]==NO){
        array = [ NSMutableArray arrayWithCapacity:1 ];
        while(1){
            if(str)
                [ array addObject:str ];
            symbol = [ [ scanner string ] substringWithRange:NSMakeRange([ scanner scanLocation ],1) ];
            [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
            [ scanner scanCharactersFromSet:[ NSCharacterSet varNameCharacterSet ] intoString:&str ];
            if([ symbol isEqualToString:@"$" ]){
                [ array addObject:[ self getStringOfArgment:
                    [ TYStringVarArgment argmentWithVarID:
                        [ NSNumber numberWithInt:[ str intValue ] ] ] ] ];
            } else if([ symbol isEqualToString:@"%" ]){
                NSNumber *number;
                number = [ self getValueOfArgment:
                            [ TYIntVarArgment argmentWithVarID:
                                [ NSNumber numberWithInt:[ str intValue ] ] ] ];
                [ array addObject:[ number stringValue ] ];
            }

            str = nil;
            [ scanner scanUpToCharactersFromSet:varHeaderSet intoString:&str ];
            if([ scanner isAtEnd ])
                break;
        }
        if(str)
            [ array addObject:str ];

        replaceStr = [ array componentsJoinedByString:@"" ];
    } else {
        replaceStr = aString;
    }
    
    return replaceStr;
}

-(void)saveExternalData
{
    if(globalOn){
        [ scripterValues saveGlobalValues:[ getNScrRootDirectory()
            stringByAppendingPathComponent:GLOBAL_SAVEFILE_NAME ] ];
    }

    if(labelLog)
        [ labelLog writeToFile:[ getNScrRootDirectory() stringByAppendingPathComponent:LABELLOG_FILENAME ] ];

    if(kidokuData)
        [ kidokuData writeToFile:[ getNScrRootDirectory() stringByAppendingPathComponent:KIDOKU_FILENAME ] atomically:YES ];
}

#if YYDEBUG == 1
extern int yydebug;
-(void)ty__yydebug:(NSMutableArray *)argments
{
    yydebug = [ [ argments objectAtIndex:1 ] intValue ];
}
#endif

-(void)ty__log:(NSMutableArray*)argments
{
    TYArgment *arg;
    
    arg = [ argments objectAtIndex:1 ];
    switch([ arg argType ]) {
    case TYDigitArgType:
    case TYIntVarArgType:
        NSLog(@"log it's number,[%d]",[ arg intValue ]);
        break;
    case TYConstStringArgType:
    case TYStringVarArgType:
    case TYFchkArgType:
        NSLog(@"log it's string,[%@]",[ arg stringObj ]);
        break;
    case TYAliasArgType:
        NSLog(@"log it's alias,name[%@],string[%@],number[%@]",[arg rawString],[arg stringObj],[arg intNumber ]);
    }
    
}

-(void)ty_globalon:(NSMutableArray *)argments
{
    // グローバル変数をロードし、終了時に保存するようにする。
    
    /*aData = [ NSMutableData dataWithContentsOfFile:
                [ getAppDirectory() stringByAppendingPathComponent:GLOBAL_SAVEFILE_NAME ] ];
    */
    
    
    // MEMO:書き込み権限のチェックも入れる
    {
        globalOn = YES;
    }

}

/*
-(void)saveGlobal // アプリ終了時に呼ぶ事
{
    if(globalOn){         
        [ scripterValues saveGlobalValues:[ getAppDirectory() 
            stringByAppendingPathComponent:GLOBAL_SAVEFILE_NAME ] ];
    }
}
*/


/*
-(void)saveLabelLog
{
    if(labelLog)
        [ labelLog writeToFile:[ getAppDirectory() stringByAppendingPathComponent:LABELLOG_FILENAME ] ];
}
*/

-(int)runLine
{
    return runLine;
}

-(void)moveRunLine:(int)aInt
{
    runLine += aInt;
}

-(void)incRunColumn
{
    runColumn++;
}

-(void)resetRunColumn
{
    runColumn=0;
}

-(void)ty_labellog:(NSMutableArray *)argments
{
    labelLog = [ [ TYFileLog alloc ] initWithContentsOfFile:[ getNScrRootDirectory()  	stringByAppendingPathComponent:LABELLOG_FILENAME ] ];
    
}

-(void)ty_getversion:(NSMutableArray *)argments
{
    NSNumber *verNum;
    id idno;
    verNum = [ TYEnviroment objectForKey:@"NScripterVersion" ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];

    [ scripterValues setIntValue:idno object:verNum ];
}

// 配列変数の宣言
-(void)ty_dim:(NSMutableArray *)argments
{
    id idno;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    
    [ scripterValues defineArrayValue:idno ];
}

-(void)ty_movl:(NSMutableArray *)argments
{
    id idno;
    NSEnumerator *enu;
    NSMutableArray *array = [ NSMutableArray array ];
    id tmp;
    
    enu = [ argments objectEnumerator ];
    [ enu nextObject ];
    
    idno = [ self getArrayIdOfArgment:[ enu nextObject ] ];
    
    while(tmp = [ enu nextObject ]){
        [ array addObject:[ self getValueOfArgment:tmp ] ];
    }
    
    [ scripterValues setArrayLine:idno values:array ];

}

//  変数に代入
-(void)ty_mov:(NSMutableArray *)argments
{
    TYVarType type;
    id arg;
    id idno;

    type = [ [ argments objectAtIndex:1 ] varType ];
    
    // 変数のIDを取得
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
        
    if(type == TYStringVarType){
        arg = [ self getStringOfArgment:[ argments objectAtIndex:2 ] ];
        [ scripterValues setStringValue:idno object:arg ];
    } else if(type & TYDigitVarTypeMask) {
        arg = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];
        [ scripterValues setIntValue:idno object:arg ];
    }
}

-(void)ty_mov3:(NSMutableArray*)argments
{
    [ self movRepeat:3 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov4:(NSMutableArray*)argments
{
    [ self movRepeat:4 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov5:(NSMutableArray*)argments
{
    [ self movRepeat:5 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov6:(NSMutableArray*)argments
{
    [ self movRepeat:6 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov7:(NSMutableArray*)argments
{
    [ self movRepeat:7 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov8:(NSMutableArray*)argments
{
    [ self movRepeat:8 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov9:(NSMutableArray*)argments
{
    [ self movRepeat:9 argments:(NSMutableArray*)argments ];
}

-(void)ty_mov10:(NSMutableArray*)argments
{
    [ self movRepeat:10 argments:(NSMutableArray*)argments ];
}

-(void)movRepeat:(int)repeat_times argments:(NSMutableArray*)argments
{
    TYVarType type;
    int i;
    int intId;
    NSEnumerator *enm;
    id temp;

    enm = [ argments objectEnumerator ];
    [ enm nextObject ];

    temp = [ enm nextObject ];
    type = [ temp varType ];
    intId = [ [ self getIdNoOfArgment:temp ] intValue ];

    if(type & TYDigitVarTypeMask){
        for(i=0; i < repeat_times; i++,intId++){
            [ scripterValues setIntValue:[ NSNumber numberWithInt:intId ] object:[ self getValueOfArgment:[ enm nextObject ] ] ];
        }
    } else if(type == TYStringVarType) {
        for(i=0; i < repeat_times; i++,intId++){
            [ scripterValues setStringValue:[ NSNumber numberWithInt:intId ] object:[ self getStringOfArgment:[ enm nextObject ] ] ];
        }
    } else {
        NSLog(@"movX,error of arg 1,invalid context");
    }
}

-(void)ty_intlimit:(NSMutableArray *)argments
{
    [ scripterValues setIntLimitID:[ self getValueOfArgment:[ argments objectAtIndex:1 ] ]
                               low:[ self getValueOfArgment:[ argments objectAtIndex:2 ] ] 				      high:[ self getValueOfArgment:[ argments objectAtIndex:3 ] ] ];
}

// 変数のインクリメント
-(void)ty_inc:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] +1 ];
    [ scripterValues setIntValue:idno object:number ];
    
}
// 変数のデクリメント
-(void)ty_dec:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] -1 ]; 
    [ scripterValues setIntValue:idno object:number ];
    
}

// 変数の足し算 文字列なら連結処理
-(void)ty_add:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    NSNumber *argnum;
    NSString *string;
    NSString *argstr;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    if( [ [ argments objectAtIndex:1 ] varType ] == TYStringVarType ){
        argstr = [ self getStringOfArgment:[ argments objectAtIndex:2 ] ];
        string = [ scripterValues getStringValue:idno ];
        string = [ NSString stringWithFormat:@"%@%@",string,argstr ];
        [ scripterValues setStringValue:idno object:string ];
    } else {
        argnum = [ self getValueOfArgment:[argments objectAtIndex:2 ] ];
        number = [ scripterValues getIntValue:idno ];
        number = [ NSNumber numberWithInt:[ number intValue] +[ argnum intValue ] ];
        [ scripterValues setIntValue:idno object:number ];
    }
    
    return ;
}

// 変数の引き算
-(void)ty_sub:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    NSNumber *argnum;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    argnum = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] -[ argnum intValue ] ];
    [ scripterValues setIntValue:idno object:number ];
}

-(void)ty_mul:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    NSNumber *argnum;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    argnum = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] *[ argnum intValue ] ];
    [ scripterValues setIntValue:idno object:number ];
}

-(void)ty_div:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    NSNumber *argnum;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    argnum = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] /[ argnum intValue ] ];
    [ scripterValues setIntValue:idno object:number ];
}

-(void)ty_mod:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;
    NSNumber *argnum;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    argnum = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];

    number = [ scripterValues getIntValue:idno ];
    number = [ NSNumber numberWithInt:[ number intValue] % [ argnum intValue ] ];
    [ scripterValues setIntValue:idno object:number ];
}

-(void)ty_cmp:(NSMutableArray *)argments
{
    id idno;
    int result;
    
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    result = [ (NSString*)[ self getStringOfArgment:[ argments objectAtIndex:2 ] ] compare: 
                [ self getStringOfArgment:[ argments objectAtIndex:3 ] ] ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:result ] ];            
    
}

-(void)ty_atoi:(NSMutableArray*)argments
{
    id idno;
    NSString *str;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    str = [ self getStringOfArgment:[ argments objectAtIndex:2 ] ];

    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:[ str intValue ] ] ];
}

-(void)ty_itoa:(NSMutableArray*)argments
{
    id idno;
    int value;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    value = [ [ self getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];

    [ scripterValues setStringValue:idno object:[ NSString stringWithFormat:@"%d",value ] ];
}

-(void)ty_len:(NSMutableArray*)argments
{
    id idno;
    unsigned length;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    length = [ [ self getStringOfArgment:[ argments objectAtIndex:2 ] ] cSJISStringLength ];

    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:length ] ];
}

-(void)ty_mid:(NSMutableArray *)argments
{
    id idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    NSString *inStr = [ self getStringOfArgment:[ argments objectAtIndex:2 ] ];
    int offset = [ [ self getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    int length = [ [ self getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];
    
    [ scripterValues setStringValue:idno object:[ inStr substringSJISRange:NSMakeRange(offset,length) ] ];
}

-(void)ty_splitstring:(NSMutableArray *)argments
{
    NSString *scrStr;
    NSString *tmpStr;
    NSEnumerator *argEnu,*scrEnu;
    
    argEnu = [ argments objectEnumerator ];
    [ argEnu nextObject ];
    scrStr = [ [ argEnu nextObject ] stringObj ];
    scrEnu = [ [ scrStr componentsSeparatedByString:[ [ argEnu nextObject ] stringObj ] ] objectEnumerator ];
    while(tmpStr = [ scrEnu nextObject ]) {
        [ self ty_mov:[ NSMutableArray arrayWithObjects:@"mov",
                            [ argEnu nextObject ],
                            [ TYConstStringArgment argmentWithString:tmpStr ],nil ]  ];
    }
}

-(void)ty_rnd:(NSMutableArray *)argments
{
    id idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    int max = [ [ self getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    int result;
    
    result = TYRandom(0,max -1);
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:result ] ];
}

-(void)ty_rnd2:(NSMutableArray *)argments
{
    int min,max,result;
    id idno;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    min = [ [ self getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    max = [ [ self getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];

    result = TYRandom(min,max);

    //NSLog(@"random,min=%d,max=%d,result=%d",min,max,result);
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:result ] ];
}

-(void)ty_date:(NSMutableArray *)argments
{
    NSCalendarDate *cDate;
    id idno;
    int d;

    cDate = [ NSCalendarDate calendarDate ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    d = [ cDate yearOfCommonEra ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:2 ] ];
    d = [ cDate monthOfYear ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:3 ] ];
    d = [ cDate dayOfMonth ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
}

-(void)ty_time:(NSMutableArray *)argments
{
    NSCalendarDate *cDate;
    id idno;
    int d;

    cDate = [ NSCalendarDate calendarDate ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    d = [ cDate hourOfDay ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:2 ] ];
    d = [ cDate minuteOfHour ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:3 ] ];
    d = [ cDate secondOfMinute ];
    [ scripterValues setIntValue:idno object:[ NSNumber numberWithInt:d ] ];
}

// 文字列エイリアスの登録
-(void)ty_stralias:(NSMutableArray *)argments
{
    [ strAliases setObject: [ self getStringOfArgment:[ argments objectAtIndex:2 ] ] 
                forKey:[ [ argments objectAtIndex:1 ] lowercaseString ] ];
    return;
}

// 数字エイリアスの登録
-(void)ty_numalias:(NSMutableArray *)argments
{
    [ numAliases setObject:[ self getValueOfArgment:[ argments objectAtIndex:2 ] ]
                forKey:[ [ argments objectAtIndex:1 ] lowercaseString ] ];
    return;
}

// アーカイバを登録
-(void)ty_arc:(NSMutableArray *)argments
{
    [ [ TYResourceServer sharedServer ] addArchiver:[ self getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
}

// NSAアーカイブを読む
-(void)ty_nsa:(NSMutableArray *)argments
{
    int i;
    NSString *dir = ( nsaDir ) ? nsaDir : @"";

    [ [ TYResourceServer sharedServer ] addArchiver:
        [ dir stringByAppendingPathComponent:[ NSString stringWithFormat:DEFALUT_NSA_ARCHIVE,@"" ] ] ];

    for(i=1;i<=9;i++){
        [ [ TYResourceServer sharedServer ] addArchiver:
            [ dir stringByAppendingPathComponent:[ NSString stringWithFormat:DEFALUT_NSA_ARCHIVE,[ NSString stringWithFormat:@"%d",i ] ] ] ];
    }

}

-(void)ty_nsadir:(NSMutableArray *)argments
{
    nsaDir = [ [ self getStringOfArgment:[ argments objectAtIndex:1 ] ] retain ];
}

-(void)ty_ns2:(NSMutableArray *)argments
{
    [ TYNsaArchiver setMode:TY2TypeNsaFileMode ];
    [ self ty_nsa:nil ];
}

-(void)ty_ns3:(NSMutableArray *)argments
{
    [ TYNsaArchiver setMode:TY3TypeNsaFileMode ];
    [ self ty_nsa:nil ];
}

// ウインドウタイトルの変更
/*-(void)ty_caption:(NSMutableArray *)argments
{
    [ controller ty_caption:[ self getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
    return;
}*/


// エフェクトを登録
/*-(void)ty_effect:(NSMutableArray *)argments
{
    int count;
    NSNumber *effectNo=nil,*effectType=nil,*time=nil;
    NSString *path=nil;
    
    count = [ argments count ];
    
    switch(count){
    case 5:
        path = [ self getStringOfArgment:[ argments objectAtIndex:4 ] ];
    case 4:
        time = [ self getValueOfArgment:[ argments objectAtIndex:3 ] ];
    case 3:
        effectType = [ self getValueOfArgment:[ argments objectAtIndex:2 ] ];
        effectNo = [ self getValueOfArgment:[ argments objectAtIndex:1 ] ];
    }
    
    [ [ stageManager effecter ] setEffectNo:[ effectNo intValue ]
                                type:[ effectType intValue ]
                                time:[ time intValue ]
                                path:path ];
                                
}*/

/*-(void)ty_bg:(NSMutableArray *)argments
{
    NSNumber *effNo;
    
    effNo = [ self getEffectNoOfArgments:[ NSMutableArray arrayWithArray:
        [ argments subarrayWithRange:NSMakeRange(2,[ argments count]-2) ] ] ];

    [ stageManager ty_bg:[ self getStringOfArgment:[ argments objectAtIndex:1 ] ] effectNo:[ effNo intValue ] ];
}*/

/*-(void)ty_click:(NSMutableArray *)argments
{
    [ self setBreakRun:YES ];
    [ controller ty_click ];
}*/


/*-(void)ty_wait:(NSMutableArray *)argments
{
    [ self setBreakRun:YES ];
    [ controller setStatus:TYTimeWaitStatus ];
    [ self performSelector:@selector(runScript) withObject:nil afterDelay:
        [ [ self getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] / 10 ];
}*/

-(void)ty_jumpf:(NSMutableArray *)argments
{
    [ self labeljump:@"~" fromLine:runLine reverse:NO ];
}

-(void)ty_jumpb:(NSMutableArray *)argments
{
    [ self labeljump:@"~" fromLine:runLine reverse:YES ];
}

-(void)ty_goto:(NSMutableArray *)argments
{
    [ self labeljump:[ self getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
}

-(void)ty_gosub:(NSMutableArray *)argments
{
    TYScriptPoint sPoint;
    sPoint = TYMakeScriptPoint(runLine,runColumn);
    [ returnStack addObject:[ NSValue valueWithBytes:&sPoint objCType:@encode(TYScriptPoint) ] ];
    [ self labeljump:[ self getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
}

-(void)ty_return:(NSMutableArray *)argments
{
    TYScriptPoint sPoint;
    [ [ returnStack lastObject ] getValue:&sPoint ];
    [ self jump:sPoint.line ];
    runColumn = sPoint.column;
    [ returnStack removeLastObject ];
    
    // システムカスタマイズ時、textgosubで呼ばれたところから返るなら
    if((textgosubPointIndex > -1) &&
       (textgosubPointIndex == [ returnStack count ])){
       [ self setBreakRun:YES ];
       [ stageManager resumePrinting ];
       textgosubPointIndex = -1;
    }
}

-(void)ty_tablegoto:(NSMutableArray *)argments
{
    int num=[ [ self getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    [ self ty_goto:[ NSMutableArray arrayWithObjects:@"goto",[ argments objectAtIndex:2+num ],nil ] ];
}

-(void)ty_skip:(NSMutableArray *)argments
{
    int step = [ [ self getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    if(step)
        runLine += step -1;
}

-(void)ty_next:(NSMutableArray *)argments
{
    TYLoopStruct loop;
    int now;

    NSAssert(([ loopStack count ] > 0),@"do next,but no loop stack");
    [ [ loopStack lastObject ] getValue:&loop ];

    now=[ [ scripterValues getIntValue:loop.varNo ] intValue ]+loop.step;
    [ scripterValues setIntValue:loop.varNo object:[ NSNumber numberWithInt:now ] ];

    if((loop.step>0 && now>loop.limit) || (loop.step<0 && now<loop.limit)){
        NSLog2(@"remove for stack");
        [ loopStack removeLastObject ];
    } else {
        runLine=loop.point.line;
        runColumn=loop.point.column;
        if(runColumn==0)
            runColumn++;
    }
}

-(void)ty_break:(NSMutableArray *)argments
{
    NSLog2(@"remove for stack");
    [ loopStack removeLastObject ];
    if([ argments count ] >= 2) {
        [ self ty_goto:argments ];
    } else
        [ self jumpToLoopEnd ];
}

-(void)ty_game:(NSMutableArray *)argments
{
    [ self labeljump:@"*start" ];
}

-(void)ty_resettimer:(NSMutableArray *)argments
{
    [ timerStartDate release ];
    timerStartDate = [ [ NSDate alloc ] init ];
}

-(void)ty_waittimer:(NSMutableArray *)argments
{
    int timerValue;
    NSTimeInterval interval=[ timerStartDate timeIntervalSinceNow ];
    NSTimeInterval delay;

    timerValue = [ [ self getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];

    delay = timerValue / 1000.0 +interval;
    if(delay < 0)
        delay = 0;

    [ self setBreakRun:YES ];    

    [ self performSelector:@selector(runScript) withObject:nil afterDelay:
        timerValue / 1000.0 +[ timerStartDate timeIntervalSinceNow ] ];
}

-(void)ty_gettimer:(NSMutableArray *)argments
{
    id idno;
    NSNumber *number;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];

    number = [ NSNumber numberWithInt:-[ timerStartDate timeIntervalSinceNow ] *1000 ];
    [ scripterValues setIntValue:idno object:number ];
}

-(void)ty_loadgame:(NSMutableArray *)argments
{
    NSNumber *number;

    number = [ self getValueOfArgment:[ argments objectAtIndex:1 ] ];

    [ controller loadLocalData:number ];    
}

-(void)ty_savegame:(NSMutableArray *)argments
{
    NSNumber *number;

    number = [ self getValueOfArgment:[ argments objectAtIndex:1 ] ];

    [ controller saveLocalData:number ];
}

// プログラムの終了
-(void)ty_end:(NSMutableArray *)argments
{
    // 初期設定のセーブ
    // グローバル変数のセーブ
    [ NSApp terminate:self ];
}

-(void)ty_inputstr:(NSMutableArray *)argments
{
    id idno;
    TYInputStrController *input;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    input = [ TYInputStrController dialog ];
    [ input runModalCaption:[ self getStringOfArgment:[ argments objectAtIndex:2 ] ]
                     length:[ [ self getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ]
                   notAscii:[ [ self getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ] ];

    [ scripterValues setStringValue:idno object:[ input string ] ];

}

-(void)ty_input:(NSMutableArray *)argments
{
    id idno;
    TYInputStrController *input;

    idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    input = [ TYInputStrController dialog ];
    [ input runModalCaption:[ self getStringOfArgment:[ argments objectAtIndex:2 ] ]
                 defaultStr:[ self getStringOfArgment:[ argments objectAtIndex:3 ] ]
                     length:[ [ self getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ]
                   notAscii:[ [ self getValueOfArgment:[ argments objectAtIndex:5 ] ] intValue ] ];

    [ scripterValues setStringValue:idno object:[ input string ] ];

}

-(void)ty_mesbox:(NSMutableArray *)argments
{
    NSRunInformationalAlertPanel([ self getStringOfArgment:[ argments objectAtIndex:2 ] ],[ self getStringOfArgment:[ argments objectAtIndex:1 ] ],@"OK",nil,nil);
    
}

-(void)ty_getreg:(NSMutableArray *)argments
{
    NSDictionary *aDict=[ NSDictionary dictionaryWithContentsOfFile:[ getNScrRootDirectory() stringByAppendingPathComponent:REGISTRY_NAME ] ];
    NSString *key;
    NSString *name;
    NSString *value;
    NSDictionary *keyDict;
    id idno;

    key = [ self getStringOfArgment:[ argments objectAtIndex:2 ] ];
    name = [ self getStringOfArgment:[ argments objectAtIndex:3 ] ];

    if((aDict)&&(keyDict = [ aDict objectForKey:key ])){
        value = [ keyDict objectForKey:name ];
        idno = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
        [ scripterValues setStringValue:idno object:value ];
    }
    
}

-(void)ty_getcursorpos:(NSMutableArray*)argments
{
    NSPoint point;
    id varX,varY;
    
    varX = [ self getIdNoOfArgment:[ argments objectAtIndex:1 ] ];
    varY = [ self getIdNoOfArgment:[ argments objectAtIndex:2 ] ];
    
    point = [ stageManager drawPoint ];
    [ scripterValues setIntValue:varX object:[ NSNumber numberWithInt:point.x ] ];
    [ scripterValues setIntValue:varY object:[ NSNumber numberWithInt:VSCREEN_HEIGHT -point.y ] ];
}

-(void)ty_lr_trap:(NSMutableArray*)argments 
{
    [ self ty_trap:argments ];
}

-(void)ty_trap:(NSMutableArray*)argments
{
    NSString *arg=[ argments objectAtIndex:1 ];
    NSString *carg = [ self getStringOfArgment:arg ];

    if([ arg compare:@"off" options:NSCaseInsensitiveSearch ]==NSOrderedSame){
        if(labelWithTrap)
            [ labelWithTrap release ];
        labelWithTrap=nil;
        fireOfTrap = NO;
        [ controller setTrap:NO ];
    } else if([ arg compare:@"stop" options:NSCaseInsensitiveSearch ]==NSOrderedSame){
        fireOfTrap = NO;
        [ controller setTrap:NO ];        
    } else if([ arg compare:@"resume" options:NSCaseInsensitiveSearch ]==NSOrderedSame){
        fireOfTrap = NO;
        [ controller setTrap:YES ];    
    } else {
        labelWithTrap=[ carg retain ];
        fireOfTrap = NO;
        [ controller setTrap:YES ];
    }
}

-(void)fireOfTrap
{
    if(labelWithTrap)
        fireOfTrap=YES;
}

-(void)fireOfTextgosub:(NSString*)aLabel
{
    TYScriptPoint point;
    point = TYMakeScriptPoint(runLine,runColumn);
    [ self ty_gosub:[ NSMutableArray arrayWithObjects:@"gosub",aLabel,nil ] ];
    textgosubPointIndex = [ returnStack count ] -1;
    [ self runScript ];
}

-(void)enableKidoku
{
    kidokuData=[ [ NSMutableData alloc ] initWithContentsOfFile:
        [ getNScrRootDirectory() stringByAppendingPathComponent:KIDOKU_FILENAME ] ];
    if(!kidokuData){
        kidokuData = [ [ NSMutableData alloc ] initWithLength:[ ScriptArray count ] / 8 +1 ];
    } else {
        if([ kidokuData length ] < [ ScriptArray count ] / 8 +1){
            [ kidokuData setLength:[ ScriptArray count ] / 8 +1 ];
        }
    }

    kidokuptr = (unsigned char*)[ kidokuData mutableBytes ];
}

-(void)setWatchKidoku:(BOOL)aBool
{
    isWatchKidoku = aBool;
}

@end
