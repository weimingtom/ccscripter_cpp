//
//  TYMainController.m
//  Tukuyomi
//
//  Created by toveta on Tue Sep 25 2001.
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
#import "TYStageManager.h"
#import "TYScriptEngine.h"
#import "TYVisualNovelView.h"
#import "TYSoundController.h"
#import "TYResourceServer.h"
#import "TYMiscUtil.h"
//#import "TYSaveLoadDialogController.h" --obsolute
#import "TYSaveLoadManager.h"
#import "TYEnviroment.h"
#import "TYAudioPathController.h"
#import "TYFullScreenWindow.h"
#import "TYFullScreenContentView.h"
#import "TYAutoModeController.h"
#import "StopNSLog.h"
#import "TYExtVolumeController.h"
#import "TYPreferencePanelController.h"
//#import "TYQTMusicPlayer.h"
#import "TYWave2Aiff.h"
#import "TYEraseModeView.h"
#import "TYRootMenuView.h"
#import "TYImageUtil.h"
#import "TYYesNoMenuView.h"
#import "TYSaveLoadMenuView.h"
#import "TYLookbackView.h"
#import "TYCellImage.h"
#import "TYScreenManager.h"
#import "TYMovieControllerView.h"
#import "TYCCSProxy.h"
#import "TYArgment.h"
#import "TYDigitArgment.h"

@implementation TYMainController

static id controller;

// 定数
static NSString* const INSTANT_WAVE_PATH_FORMAT=@"wav/%@.wav";

NSString* const TYRMenuFontSize=@"TYRMenuFontSize";
NSString* const TYRMenuFontPitchSize=@"TYRMenuFontPitchSize";
NSString* const TYRMenuShadowed=@"TYRMenuShadowed";
NSString* const TYRMenuBold=@"TYRMenuBold";
NSString* const TYRMenuBGColor=@"TYRMenuBGColor";
NSString* const TYRMenuSelectColor=@"TYRMenuSelectColor";
NSString* const TYRMenuNoSelectColor=@"TYRMenuNoSelectColor";
NSString* const TYRMenuNoDataColor=@"TYRMenuNoDataColor";

NSString* const TYVoicePlayNotification=@"TYVoicePlayNotification";

/*
- (void) awakeFromNib
{
}
*/

+(id)controller
{
    return controller;
}

- (id)init
{
    self = [ super init ];
    controller = self;
    return self;
}

// delegate 初期化処理
- (void) applicationDidFinishLaunching  : (NSNotification *) aNote
{
    id aObj;
    //NSImage *aImage;
    //NSBundle *bundle;

    // スクリプトファイルの読み込み、初期化
    ScriptEngine = [ [ TYScriptEngine alloc ] initWithContentsOfFile:getNScrRootDirectory() ];
    if(!ScriptEngine){
        if(![ TYEnviroment boolForKey:TYDisableSelectScriptEnviroment ]){
            NSUserDefaults *def=[ NSUserDefaults standardUserDefaults ];
            NSString *str;
            NSOpenPanel *op = [ NSOpenPanel openPanel ];
            [ op setTitle:NSLocalizedString(@"Choose Game Directory",nil) ];
            [ op setCanChooseDirectories:YES ];
            [ op setCanChooseFiles:NO ];
            [ op runModalForDirectory:[ def objectForKey:TYLastSelectedFolderPref ]
                                 file:nil
                                types:nil ];
            str = [ op filename ];
            if(str){
                setNScrRootDirectory(str);
                [ TYEnviroment reload ];
                [ def setObject:str forKey:TYLastSelectedFolderPref ];
                [ def synchronize ];
                ScriptEngine = [ [ TYScriptEngine alloc ] initWithContentsOfFile:getNScrRootDirectory() ];
            }
        }
        
        if(!ScriptEngine)
        [ [ self class ] alertAndTerminate:NSLocalizedString(@"Do something",nil)
                                       msg:NSLocalizedString(@"not exist 0.txt and nscript.dat",nil) ];
    }
    [ ScriptEngine retain ]; //　永続させる

    // MainWindowにContentViewを設定
    [ MainView setTarget:self ];
    [ MainView setFrame:TYVirtualScreenRect() ];
    /*
    aImage = [ [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ] autorelease ];
    [ aImage lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill(TYVirtualScreenRect());
    [ aImage unlockFocus ];
    [ MainView setImage:aImage ];
    */
    
    //NSLog(@"%d",[ MainWindow makeFirstResponder:MainView ]);
    [ MainWindow setContentSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];
    [ MainWindow setAcceptsMouseMovedEvents:YES ];
    //defaultContentView = [ [ MainWindow contentView ] retain ];
    [ [ MainWindow contentView ] addSubview:MainView ]; 
    //[ (NSWindow*)MainWindow setContentView:MainView ];
    [ MainWindow makeFirstResponder:MainView ];
    //[ MainWindow setNextResponder:MainView ];
    {
        NSNumber *xNum,*yNum;
        NSPoint windowPoint;
        NSRect windowFrame;

        if((xNum = [ TYEnviroment objectForKey:TYWindowPointXEnviroment ]) &&
           (yNum = [ TYEnviroment objectForKey:TYWindowPointYEnviroment ])){
            windowPoint.x = [ xNum intValue ];
            windowPoint.y = [ yNum intValue ];
            windowFrame = [ MainWindow frame ];
            windowFrame.origin = windowPoint;
            [ MainWindow setFrame:windowFrame display:NO ];
        } else {
            [ MainWindow center ];
        }
    }
    [ MainWindow makeKeyAndOrderFront:self ];

    [ (NSWindow*)FullScrrenWindow setContentView:[ [ [ TYFullScreenContentView alloc ] initWithFrame:NSZeroRect ] autorelease ] ];

    usingDirectWaveCannelSet = [ [ NSMutableSet alloc ] initWithCapacity:2 ];

    // 設定辞書の読み込み
    /* クラス化しました。
    bundle = [ NSBundle mainBundle ];
    TYSettingsDict = [ [ NSMutableDictionary dictionaryWithContentsOfFile:[ bundle pathForResource:@"Settings" ofType:@"plist" ] ] retain ];
     */
    
    /*
    audioSearchPath = [ [ NSBundle mainBundle ] localizedStringForKey:@"DEFAULT_AUDIO_TRACK_PATH"
                                                value:@"/Volumes/Audio CD/Track *.cdda"
                                                table:nil ];
     */
     // コンテキストメニューの設定、廃止
     /*
     if(![ TYEnviroment boolForKey:TYEnableContextMenuEnviroment ]){
        // Control+clickに反応するにはmenuがnilである必要がある
        [ MainView setMenu:nil ];
     }
     */

    saveLoadController = [ [ TYSaveLoadManager alloc ] init ];
    [ saveLoadController setNumber:9 ]; 
    
    // オーディオ検索パスの取得
    [ self setAudioPath:[ TYEnviroment objectForKey:TYAudioFilePathEnviroment ] ];    

    // SVGAモードかチェック
    {
        BOOL isModeSVGA = [ ScriptEngine isModeSVGA ];
        if(isModeSVGA){
            NSRect frame;
            NSLog(@"Boot With 800 x 600 mode");
            [ TYStageManager setModeSVGA ];
            frame = [ MainWindow frame ];
            frame.size = TYVirtualScreenSize();
            [ MainWindow setContentSize:frame.size ];
            frame.origin = NSZeroPoint;
            [ MainView setFrame:frame ];
            [ MainView setNeedsDisplay:YES ];
        }
    }
    
    // ステージマネージャの初期化
    StageManager = [ [ TYStageManager alloc ] init ];
    
    // オブジェクト同士を関連づける
    [ ScriptEngine setController:self stageManager:StageManager ];
    [ StageManager setController:self engine:ScriptEngine ];

    // ヴォリューム設定のロード
    defBGMVol= 1.0;
    defVoiceVol=1.0;
    defSEVol=1.0;
    
    //BGMView = [ [ TYQTMusicPlayer alloc ] initWithFrame:NSZeroRect ];
    BGMView = [ [ NSMovieView alloc ] initWithFrame:NSZeroRect ];
    [ MainView addSubview:BGMView ];
    [ BGMView showController:NO adjustingSize:NO ];
    [ BGMView setEditable:NO ];
    aObj = [ TYEnviroment objectForKey:TYBGMVolumeEnviroment ];
    if(aObj)
        BGMVolume = [ aObj floatValue ];
    else
        BGMVolume = 1.0;
    if(aObj = [ TYEnviroment objectForKey:TYVoiceVolumeEnviroment ])
        voiceVol = [ aObj floatValue ] *100 -100;
    if(aObj = [ TYEnviroment objectForKey:TYSEVolumeEnviroment ])
        seVol = [ aObj floatValue ] *100 -100;

    // 状態の初期化
    TYStatus = TYScriptRunningStatus;

    // イベント通知の登録
    [ [ NSNotificationCenter defaultCenter ] addObserver:self selector:@selector(didChangeKeyWindow:) name:NSWindowDidBecomeKeyNotification object:nil ];
    
    // 右クリックメニュー初期化
    rightMenuLayout = [ [ NSMutableDictionary alloc ] initWithCapacity:8 ];
    [ rightMenuLayout setObject:[ NSColor whiteColor ] forKey:TYRMenuSelectColor ];
    [ rightMenuLayout setObject:[ NSColor grayColor ] forKey:TYRMenuNoSelectColor ];
    [ rightMenuLayout setObject:[ NSColor grayColor ] forKey:TYRMenuNoDataColor ];
    [ rightMenuLayout setObject:getTextWindowColorWithHTMLFormat(@"#999999") forKey:TYRMenuBGColor ];
    [ self ty_menusetwindow:[ NSMutableArray arrayWithObjects:@"menusetwindow",
        [ NSNumber numberWithInt:26 ],[ NSNumber numberWithInt:26 ],
        [ NSNumber numberWithInt:0 ],[ NSNumber numberWithInt:2 ],
        [ NSNumber numberWithInt:0 ],[ NSNumber numberWithInt:1 ],
        @"#999999",nil ] ];

    // ウインドウモードの復元
    if([ TYEnviroment boolForKey:TYFullScreenModeEnviroment ]){
        [ self performSelector:@selector(fullScreenMode:) withObject:self afterDelay:0 ];
    }
    // スクリプト処理を開始。まだContextが作られていないので一度イベントループをまわす。
    [ ScriptEngine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    [ ScriptEngine saveExternalData ];
    [ [ TYResourceServer sharedServer ] saveLog:
        [ getNScrRootDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] ];
    {
        NSPoint windowPoint=[ MainWindow frame ].origin;
        [ TYEnviroment setObject:[ NSNumber numberWithInt:windowPoint.x ] forKey:TYWindowPointXEnviroment ];
        [ TYEnviroment setObject:[ NSNumber numberWithInt:windowPoint.y ] forKey:TYWindowPointYEnviroment ];
    }
    [ TYEnviroment setObject:[ NSNumber numberWithBool:(isFullScreenMode) ? YES : NO ] forKey:TYFullScreenModeEnviroment ];
    
    [ TYEnviroment save ];

    if(tmpPath)
        [ [ NSFileManager defaultManager ] removeFileAtPath:tmpPath handler:nil ];

    TYWave2AiffCleanUp();
    
    if (isFullScreenMode && [ TYEnviroment boolForKey:TYHideOtherFullScreenEnviroment ] ) {
        [ self windowMode:nil ];
        [ TYEnviroment setObject:[ NSNumber numberWithBool:YES ] forKey:TYFullScreenModeEnviroment ];
    }
    
    return NSTerminateNow;
}

+(void)alertAndTerminate:(NSString*)title msg:(NSString*)msg
{
    NSRunAlertPanel(title,msg,@"OK",nil,nil);
    [ NSApp terminate:nil ];
}

-(void)disableContextMenu
{
    // Control+clickに反応するにはmenuがnilである必要がある
    [ MainView setMenu:nil ];
}

- (void)setAudioPath:(NSString*)aPath
{
    // パスの展開
    aPath = [ aPath stringByExpandingTildeInPath ];

    if([aPath isAbsolutePath ] == NO){
        aPath = [ getNScrRootDirectory() stringByAppendingPathComponent:aPath ];
    }
    
    audioSearchPath = [ aPath retain ];
}

-(void)setStatus:(int)aStatus
{
    TYStatus = aStatus;
    if((aStatus == TYClickWaitStatus)&&(autoclickTimer)){
                [ self performSelector:@selector(select_action) withObject:nil afterDelay:autoclickTimer / 1000.0 ];
    } else if((aStatus == TYButtonWaitStatus)||(aStatus == TYSelectStatus)) {
        //[ MainWindow setAcceptsMouseMovedEvents:YES ];
    }
}

-(int)status
{
    return TYStatus;
}

-(void)saveLocalData:(NSNumber*)number
{
    NSMutableDictionary *aDict;
    NSMutableDictionary *objDict;
    NSString *filename,*path;
    NSCalendarDate *cDate=[ NSCalendarDate calendarDate ];

    aDict = [ NSMutableDictionary dictionaryWithCapacity:5 ];

    [ aDict setObject:cDate forKey:TYMakeDateSaveData ];
    [ aDict setObject:number forKey:TYNumberSaveData ];
    [ aDict setObject:[ NSNumber numberWithInt:TYSaveDataVersion ] forKey:TYVersionSaveData ];

    // 復帰時の状態を決定する。
    if([ self status ]==TYScriptRunningStatus){
        [ aDict setObject:[ NSNumber numberWithInt:TYScriptingSaveData ] forKey:TYDataTypeSaveData ];
    } else if( ([ self status ]==TYPrinting)&&([ StageManager status ]==TYTPClickWait) ){
        [ aDict setObject:[ NSNumber numberWithInt:TYPrintingWaitSaveData ] forKey:TYDataTypeSaveData ];
    } else {
        [ aDict setObject:[ NSNumber numberWithInt:TYScriptingSaveData ] forKey:TYDataTypeSaveData ];
    }

    objDict = [ self encodeWithSaveData ];
    if(objDict)
        [ aDict addEntriesFromDictionary:objDict ];
            
    objDict = [ ScriptEngine encodeWithSaveData ];
    if(objDict)
        [ aDict addEntriesFromDictionary:objDict ];
     
    objDict = [ StageManager encodeWithSaveData ];
    if(objDict)
        [ aDict addEntriesFromDictionary:objDict ];

    filename = [ NSString stringWithFormat:SAVEDATA_NAME,[ number intValue ] ];
    path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];
    [ NSArchiver archiveRootObject:aDict toFile:path ]; 
    //[ aDict writeToFile:path atomically:YES ];

    // データ一覧も更新
    [ saveLoadController resetDateWithNumber:[ number intValue ] ];    

    // グローバルデータのセーブも同時に行う。
    [ ScriptEngine saveExternalData ];
    [ [ TYResourceServer sharedServer ] saveLog:
        [ getNScrRootDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] ];
    
}

-(BOOL)loadLocalData:(NSNumber*)number
{
    NSString *filename,*path;
    NSDictionary *aDict;
    int scriptStatus;

    filename = [ NSString stringWithFormat:SAVEDATA_NAME,[ number intValue ] ];    
    path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];

    aDict = [ NSUnarchiver unarchiveObjectWithFile:path ];
    //aDict = [ NSDictionary dictionaryWithContentsOfFile:path ];
    if(!aDict)
        return NO;

    [ self decodeWithSaveData:aDict ];
    [ ScriptEngine decodeWithSaveData:aDict ];
    [ StageManager decodeWithSaveData:aDict ];

    if([ self status ] == TYScriptRunningStatus){
        [ ScriptEngine setBreakRun:YES ];
    }

    scriptStatus = [ [ aDict objectForKey:TYDataTypeSaveData ] intValue ];
    if(scriptStatus==TYScriptingSaveData){
        [ self setStatus:TYScriptRunningStatus ];
        [ ScriptEngine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
    } else {
        [ self setStatus:TYPrinting ];
        [ StageManager setStatus:TYTPClickWait];
    }
    
    return YES;
}

-(id)encodeWithSaveData
{
    NSMutableDictionary *dict = [ NSMutableDictionary dictionary ];
    
    NSString *wavePath;

    if(waveController){
        wavePath = [ waveController soundPathIfNeedPlayingAtLoad ];
        if(wavePath)
            [ dict setObject:wavePath forKey:TYWavePathSaveData ];
    }

    if(bgmPath){
        if([ BGMView loopMode ]==NSQTMovieLoopingPlayback){
            [ dict setObject:bgmPath forKey:TYBGMPathSaveData ];
        } else if(bgmForceSave){
            [ dict setObject:bgmPath forKey:TYBGMPathSaveData ];
            [ dict setObject:[ NSNumber numberWithBool:YES ] forKey:TYBGMNoLoopSaveData ];
        }
    }

    [ dict setObject:[ NSNumber numberWithInt:autoclickTimer ] forKey:TYAutoClickTimerSaveData ];
    
    return dict;
}

-(void)decodeWithSaveData:(id)aObject
{
    id temp;
    
    if([ BGMView isPlaying ])
        [ BGMView stop:self ];
    if(bgmPath){
        [ bgmPath release ];
        bgmPath = nil;
    }
    temp = [ aObject objectForKey:TYBGMPathSaveData ];
    if(temp){
        id temp2;
        temp2 = [ aObject objectForKey:TYBGMNoLoopSaveData ];
        [ self playSound:temp loop:(temp2) ? NO : YES forceSave:(temp2) ? YES : NO ];
    }

    if(waveController){
        [ waveController release ];
        waveController = nil;
    }
    temp = [ aObject objectForKey:TYWavePathSaveData ];
    if(temp){
        waveController = [ [ TYSoundController alloc ] initWithResource:temp loop:YES ];
        [ waveController play ];
    }

    temp = [ aObject objectForKey:TYAutoClickTimerSaveData ];
    if(temp){
        autoclickTimer = [ temp intValue ];
    } else {
        autoclickTimer = 0;
    }
    
}

-(int)autoclickTimer
{
    return autoclickTimer;
}

-(void)setImage:(NSImage*)aImage
{
    [ MainView setImage:aImage ];
    [ MainView setNeedsDisplay:YES ];
}

-(void)setImage:(NSImage*)aImage inRect:(NSRect)inRect
{
    [ MainView setImage:aImage ];
    if(isFullScreenMode){
        [ MainView setNeedsDisplayScalingRect:inRect ];
    } else {
        [ MainView setNeedsDisplayInRect:inRect ];
    }
}

-(NSImage*)image
{
    return [ MainView image ];
}

-(void)directDrawImage:(NSImage*)sourceImage inRect:(NSRect)inRect fromRect:(NSRect)fromRect
{
    [ MainView directDrawImage:sourceImage inRect:inRect fromRect:fromRect ];
}

-(NSImage*)makeImageFromMainView
{
    NSImage *image;
    NSBitmapImageRep *rep;

    [ MainView lockFocus ];
    rep = [ [ NSBitmapImageRep alloc ] initWithFocusedViewRect:[ MainView frame ] ];
    [ MainView unlockFocus ];
    if(!rep)
        return nil;
    [ rep setSize:TYVirtualScreenSize() ];

    image = [ [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ] autorelease ];
    [ image addRepresentation:rep ];
    return image;
}

-(void)setNeedsDisplayMainView:(BOOL)needs
{
    [ MainView setNeedsDisplay:YES ];
}

-(void)setAcceptsMouseMovedEvents:(BOOL)flag
{
    //[ MainWindow setAcceptsMouseMovedEvents:flag ];
}

-(NSWindow*)mainWindow { return MainWindow; }

-(void)ty_savenumber:(NSMutableArray *)argments
{
    [ saveLoadController setNumber:[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] ];
}

-(void)ty_savefileexist:(NSMutableArray *)argments
{
    NSString *path,*filename;
    BOOL exist;
    
    filename = [ NSString stringWithFormat:SAVEDATA_NAME,[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ] ];
    path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];

    exist = [ [ NSFileManager defaultManager ] fileExistsAtPath:path ];
    
    [ ScriptEngine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",[ argments objectAtIndex:1 ],[ NSNumber numberWithInt:(exist) ? 1 : 0 ],nil ] ];
}

-(void)ty_savetime:(NSMutableArray *)argments
{
    NSDictionary *aDict;
    NSString *path,*filename;
    int dateValue[4];
    NSMutableArray *evalArg;
    NSCalendarDate *date;
    int i;

    filename = [ NSString stringWithFormat:SAVEDATA_NAME,[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] ];
    path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];

    aDict = [ NSUnarchiver unarchiveObjectWithFile:path ];
    if(!aDict){
        evalArg = [ NSMutableArray arrayWithObjects:@"",[ argments objectAtIndex:2 ],[ NSNumber numberWithInt:0 ],nil
            ];
        [ ScriptEngine ty_mov:evalArg ];
    } else {
        date = [ aDict objectForKey:TYMakeDateSaveData ];
        dateValue[0] = [ date monthOfYear ];
        dateValue[1] = [ date dayOfMonth ];
        dateValue[2] = [ date hourOfDay ];
        dateValue[3] = [ date minuteOfHour ];
        for(i=0;i < 4; i++){
            evalArg = [ NSMutableArray arrayWithObjects:@"",[ argments objectAtIndex:2+i ],
                [ NSNumber numberWithInt:dateValue[i] ],nil ];
            [ ScriptEngine ty_mov:evalArg ];
        }
    }
}

-(void)ty_filelog:(NSMutableArray *)argments
{
    [ [ TYResourceServer sharedServer ] filelog:
        [ getNScrRootDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] ];
}

-(void)ty_mp3:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:NO forceSave:NO ];
}

-(void)ty_mp3save:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:NO forceSave:YES ];
}

-(void)ty_mp3loop:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:YES forceSave:NO ];
}

-(void)ty_mp3stop:(NSMutableArray *)argments
{
    [ self ty_playstop:argments ];
}

-(void)ty_play:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:YES forceSave:NO ];
}

-(void)ty_playonce:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:NO forceSave:NO ];
}

-(void)ty_bgm:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:YES forceSave:NO ];
}

-(void)ty_bgmonce:(NSMutableArray *)argments
{
    [ self playSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:NO forceSave:NO ];
}

-(void)playSound:(NSString*)argment loop:(BOOL)aBool forceSave:(BOOL)save
{
    NSArray *components;
    NSMovie *movie;
    int trackNo;
    NSString *path;

    if(![ argment length ]){
        [ self ty_playstop:nil ];
        return;
    }
    
    if(bgmPath)
        [ bgmPath autorelease ];
    bgmPath = [ argment retain ];
    bgmForceSave = save;
    
    if([ [ argment substringWithRange:NSMakeRange(0,1) ] isEqualToString:@"*" ]){
        // オーディオトラックの演奏
        trackNo = [ [ argment substringFromIndex:1 ] intValue ];
        components = [ audioSearchPath componentsSeparatedByString:@"*" ];
        path = [ NSString stringWithFormat:@"%@%02d%@",[ components objectAtIndex:0 ],trackNo,[ components objectAtIndex:1 ] ];
        movie = [ [ NSMovie alloc ] initWithURL:[ NSURL fileURLWithPath:path ] byReference:YES ];
    } else {
        NSData *data;
        NSString *filePath;
        //NSString *tempPath;

        if(filePath = [ [ TYResourceServer sharedServer ] getFilePath:bgmPath ]){
            movie = [ [ NSMovie alloc ] initWithURL:[ NSURL fileURLWithPath:filePath ] byReference:YES ];
        } else {
            NSString *newTmpPath;
            newTmpPath = [ getNScrRootDirectory() stringByAppendingPathComponent:[ NSString stringWithFormat:@".%@",[ winPathToUnix(bgmPath) lastPathComponent ] ] ];
            if([ tmpPath isEqualToString:newTmpPath ]){
                [ BGMView gotoBeginning:nil ];
                [ BGMView start:nil ];
                return;
            }
            [ [ NSFileManager defaultManager ] removeFileAtPath:tmpPath handler:nil ];
            [ tmpPath release ];
            tmpPath = newTmpPath;
            
            // メモリ上からNSMovieを作る方法がさっぱり分からないので、ディスクに一度書き込むことにする。
            data = [ [ TYResourceServer sharedServer ] getData:winPathToUnix(bgmPath) ];
            if(data){
                [ tmpPath retain ];
                [ data writeToFile:tmpPath atomically:NO ];
                movie = [ [ NSMovie alloc ] initWithURL:[ NSURL fileURLWithPath:tmpPath ] byReference:YES ];
                // MP3ファイルはbyReferenace:NOが無効なので
                //[ [ NSFileManager defaultManager ] removeFileAtPath:tempPath handler:nil ];
            } else {
                tmpPath = nil;
                movie = nil;
            }
        }
        path = bgmPath;
    }

    if(!movie){
        qtMovie = NULL;
        if(gMyTimer)
            [ gMyTimer invalidate ];
        gMyTimer = nil;
        [ BGMView stop:nil ];
        NSLog(@"can't play audio file = %@",path);
        return;
    }
    [ BGMView setMovie:movie ];
    if(aBool)
        [ BGMView setLoopMode:NSQTMovieLoopingPlayback ];
    else
        [ BGMView setLoopMode:NSQTMovieNormalPlayback ];
    [ BGMView setVolume:BGMVolume ];
    
    if (floor(NSAppKitVersionNumber) < NSAppKitVersionNumber10_2) {
        qtMovie = [ movie QTMovie ];
        gMyTimer = [NSTimer scheduledTimerWithTimeInterval:0.1		// interval, 0.1 seconds
                            target:self
                            selector:@selector(idleTimer:)		// call this method
                            userInfo:nil
                            repeats:YES];
    }
    [ BGMView start:nil ];
    NSLog2(@"playing=%d",[ BGMView isPlaying ]);
    [ movie release ];
}

-(void)ty_playstop:(NSMutableArray *)argments
{
    if([ BGMView isPlaying ])
        [ BGMView stop:self ];

    if(bgmPath)
        [ bgmPath release ];

    bgmPath=nil;
}

-(void) idleTimer:(id)sender
{
    MoviesTask(qtMovie,0);
}

// この後Quick Time APIを直接呼び出すと不具合が起きるので封印。
// AppleはとっととNSMovieViewのバグ直せ。 
-(void)ty_avi:(NSMutableArray *)argments
{
    [ self playMovie:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ]
            cancel:[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ] ];
}

-(void)ty_mpegplay:(NSMutableArray *)argments
{
    [ self playMovie:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ]
            cancel:[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ] ];
}

-(void)playMovie:(NSString*)aPath cancel:(BOOL)aBool
{
    NSString *filePath;
    Movie movie;

    if([ TYEnviroment boolForKey:TYEnablePlayMovieEnviroment ] == NO)
        return;
        
    filePath = [ [ TYResourceServer sharedServer ] getFilePathMakeTemp:aPath ];
    if(!filePath)
        return;
        
    movieView = [ [ NSMovieView alloc ] initWithFrame:TYVirtualScreenRect() ];
    [ movieView setMovie:
        [ [ [ NSMovie alloc ] initWithURL:
            [ [ [ NSURL alloc ] initFileURLWithPath:filePath ] autorelease ] 
                              byReference:YES ] autorelease ] ];
    [ movieView showController:NO adjustingSize:NO ];
    [ movieView setEditable:NO ];
    [ movieView setVolume:[ BGMView volume ] ];
    [ movieView setFrame:TYVirtualScreenRect() ];
    movie = [ [ movieView movie ] QTMovie ];
    [ movieView setLoopMode:NSQTMovieNormalPlayback ];

    ctrlView = [ [ TYMovieControllerView alloc ] initWithFrame:TYVirtualScreenRect() ];
    [ ctrlView setController:self ];
    [ MainView addSubview:movieView ];
    [ movieView addSubview:ctrlView ];
    [ MainWindow makeFirstResponder:ctrlView ];

    [ movieView start:self ];
    [ self performSelector:@selector(endCheckMovie)
                        withObject:nil 
                        afterDelay:GetMovieDuration(movie) / GetMovieTimeScale(movie) ];
    
    [ ScriptEngine setBreakRun:YES ];
}

-(void)endCheckMovie
{
    if([ movieView isPlaying ]) {
        [ self performSelector:@selector(endCheckMovie)
                            withObject:nil 
                            afterDelay:0.1 ];        
    } else {
        [ self endPlayMovie ];
    }
}

-(void)endPlayMovie
{
    if(!movieView) return;
    
    [ NSObject cancelPreviousPerformRequestsWithTarget:self
                                          selector:@selector(endCheckMovie) object:nil ];
    
    [ movieView stop:nil ];
    [ ctrlView removeFromSuperview ];
    [ movieView removeFromSuperview ];
    [ ctrlView autorelease ];
    [ movieView autorelease ];
    ctrlView = nil;
    movieView = nil;
    
    [ MainWindow makeFirstResponder:MainView ];
    if(isFullScreenMode==YES)
        [ [ FullScrrenWindow contentView ] setNeedsDisplay:YES ];
    [ ScriptEngine performSelector:@selector(runScript)  withObject:nil afterDelay:0 ];
}

-(void)ty_v:(NSMutableArray *)argments
{
    [ self playWaveSound:[ NSString stringWithFormat:INSTANT_WAVE_PATH_FORMAT,[ argments objectAtIndex:1 ] ] loop:NO ];
}

-(void)ty_wave:(NSMutableArray *)argments
{
    [ self playWaveSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:NO ];
}

-(void)ty_waveloop:(NSMutableArray *)argments
{
    [ self playWaveSound:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] loop:YES ];
}

-(void)playWaveSound:(NSString*)argment loop:(BOOL)aBool
{    
    if(waveController) {
        [ waveController stop ];
        // 10.3のNSSoundがThread Safeでは無いバグがあるようなので、解放を遅らせている。
        [ waveController performSelector:@selector(autorelease) withObject:nil afterDelay:15.0 ];
    }
    waveController = [ [ TYSoundController alloc ] initWithResource:argment loop:aBool ];

    if(enable_automode && !aBool)
        [ waveController setPostingNotification:YES ];
    [ waveController play ];
}

-(void)ty_wavestop:(NSMutableArray *)argments
{
    [ waveController stop ];
}

-(void)ty_dv:(NSMutableArray *)argments
{
    [ self playWaveChannel:0
                      path:[ NSString stringWithFormat:INSTANT_WAVE_PATH_FORMAT,[ argments objectAtIndex:1 ] ]
                      loop:NO ];
}

-(void)ty_dwave:(NSMutableArray *)argments
{
    [ self playWaveChannel:[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ]
                      path:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ]
                      loop:NO ];
}

-(void)ty_dwaveloop:(NSMutableArray *)argments
{
    [ self playWaveChannel:[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ]
                      path:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ]
                      loop:YES ];
}

-(void)playWaveChannel:(int)ch path:(NSString*)aPath loop:(BOOL)aBool
{
    int volume;
    
    NSAssert(((0 <= ch) && (ch < DIRECT_WAVE_MAX)),@"Direct Sound Channel Error");

    switch(ch){
        case 0:
        case DIRECT_WAVE_REPLAY:
            volume = (100 +voiceVol) *TYSOUND_VOLUME_MAX /100;
            break;
        default:
            volume = (100 +seVol) *TYSOUND_VOLUME_MAX /100;
    }

    
    if(*(directWaveController + ch)) {
        [ *(directWaveController + ch)stop ];
        // 10.3のNSSoundがThread Safeでは無いバグがあるようなので、解放を遅れせている。
        [ *(directWaveController + ch) performSelector:@selector(release) withObject:nil afterDelay:15.0 ];
    }
    
    *(directWaveController + ch) = [ [ TYSoundController alloc ] initWithResource:aPath
                                                                        loop:aBool
                                                                       volume:volume ];
    if((ch==0) && !aBool)
        [ *(directWaveController +ch) setPostingNotification:YES ];

    if(ch==0) 
        [ [ NSNotificationCenter defaultCenter ] 
            postNotificationName:TYVoicePlayNotification
                          object:aPath ]; 
    
    [ usingDirectWaveCannelSet addObject:[ NSNumber numberWithInt:ch ] ];

    [ *(directWaveController + ch) play ];
    
}

-(void)ty_dwavestop:(NSMutableArray *)argments
{
    int ch = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    NSAssert(((0 <= ch) && (ch < DIRECT_WAVE_MAX)),@"Direct Sound Channel Error");

    [ *(directWaveController + ch)stop ];
}

-(void)ty_stop:(NSMutableArray *)argments
{
    NSEnumerator *enu;
    id temp;
    
    if([ BGMView isPlaying ])
        [ BGMView stop:self ];

    if(bgmPath)
        [ bgmPath release ];

    bgmPath=nil;

    [ waveController stop ];

    enu = [ usingDirectWaveCannelSet objectEnumerator ];
    while( temp = [ enu nextObject ]){
        [ directWaveController[ [ temp intValue ] ] stop ];
    }
}

// ウインドウタイトル変更
-(void)ty_caption:(NSMutableArray *)argments
{
    [ MainWindow setTitle:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
}

// バージョン情報変更
-(void)ty_versionstr:(NSMutableArray *)argments
{
    scriptVersionStr = [ NSString stringWithFormat:@"%@\n%@",[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ],[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ] ];
    [ scriptVersionStr retain ];
}

-(void)ty_click:(NSMutableArray *)argments
{
    [ ScriptEngine setBreakRun:YES ];
    [ self setStatus:TYClickWaitStatus ];
}

-(void)ty_autoclick:(NSMutableArray *)argments
{
    autoclickTimer = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    //[ StageManager setAutoclickTimer:autoclickTimer ];
}

-(void)ty_wait:(NSMutableArray *)argments
{
    [ ScriptEngine setBreakRun:YES ];
    [ self setStatus:TYTimeWaitStatus ];
    [ ScriptEngine performSelector:@selector(runScript) withObject:nil afterDelay:
        [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] / 1000.0 ];
}

-(void)ty_delay:(NSMutableArray *)argments 
{
    [ ScriptEngine setBreakRun:YES ];
    [ self setStatus:TYClickWaitStatus ];
    [ self performSelector:@selector(select_action) withObject:nil afterDelay:
        [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] / 1000.0 ];
}

-(void)ty_spi:(NSMutableArray *)argments
{
    NSArray *argArray;
    NSString *arg;

    arg = [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    argArray = [ arg componentsSeparatedByString:@"|" ];

    [ [ TYResourceServer sharedServer ] addSpi:[ argArray objectAtIndex:0 ] extension:[ argArray objectAtIndex:1 ] ];
}

-(void)ty_soundpressplgin:(NSMutableArray *)argments
{
    NSArray *argArray;
    NSString *arg;

    arg = [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    argArray = [ arg componentsSeparatedByString:@"|" ];

    [ [ TYResourceServer sharedServer ] addSoundPressPlugin:[ argArray objectAtIndex:0 ] extension:[ argArray objectAtIndex:1 ] ];
}

-(void)ty_mode_ext:(NSMutableArray *)argments
{
    mode_ext=YES;
    [ self setEnableAutomode:YES ];
}

-(void)setEnableAutomode:(BOOL)aBool
{
    int i,n;
    NSMenu *menu;
    NSMenuItem *mItem=[ [ NSMenuItem alloc ] initWithTitle:NSLocalizedString(@"AutoMode...",nil)
                                                    action:NULL
                                             keyEquivalent:@"A" ];

    enable_automode=aBool;

    //[ TYSoundController setPostingNotification:YES ];

    [ mItem setEnabled:YES ];
    [ mItem setAction:@selector(openAutoMode:) ];
    [ mItem setTarget:self ];
    menu = [ NScrMenu submenu ];
    n = [ menu numberOfItems ];
    i = 0;
    while(1){
        if([ menu itemAtIndex:i ] == SkipMenu){
            [ menu insertItem:mItem atIndex:i ];
            break;
        }
        i++;
        if(i>=n){
            [ menu insertItem:mItem atIndex:i ];
            break;
        }
    }
    /*
     [ ScriptEngine setBreakRun:YES ];
     [ ScriptEngine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
     */

    
}

-(void)ty_automode_time:(NSMutableArray *)argments
{
    NSNumber *temp;
    
    if(!enable_automode)
        [ self setEnableAutomode:YES ];
    
    if(!(temp = [ TYEnviroment objectForKey:TYAutomodeWaitTimeEnviroment ])) {
        temp = [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ];
        [ TYEnviroment setObject:[ NSNumber numberWithInt:[ temp intValue ] ] forKey:TYAutomodeWaitTimeEnviroment ];
    }
}

-(void)ty_defmp3vol:(NSMutableArray *)argments
{
    defBGMVol=[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] /100.0;
}

-(void)ty_voicevol:(NSMutableArray*)argments
{
    // なんで100引くんだっけ? 自分でも分からん。
    voiceVol = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] -100;
}

-(void)ty_defvoicevol:(NSMutableArray *)argments
{
    defVoiceVol=[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] /100.0;
}

-(void)ty_sevol:(NSMutableArray*)argments
{
    seVol = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] -100;
}

-(void)ty_defsevol:(NSMutableArray *)argments
{
    defSEVol=[ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ] /100.0;
}

-(void)ty_mp3vol:(NSMutableArray *)argments
{
    int volume;
    
    volume = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    
    BGMVolume = volume / 100.0;
    [ BGMView setVolume:volume / 100.0 ];
}

-(void)ty_kidokuskip:(NSMutableArray *)argments
{
    if(![ TYEnviroment objectForKey:TYKidokumodeEnviroment ]) {
        [ TYEnviroment setObject:[ NSNumber numberWithBool:YES ] forKey:TYKidokumodeEnviroment ];
    }
    [ SkipMenu setTitle:NSLocalizedString(@"KidokuSkip",nil) ];
    [ SkipMenu setAction:@selector(skipWhileKidoku:) ];
    [ ScriptEngine enableKidoku ];
}

-(void)ty_kidokumode:(NSMutableArray *)argments
{
    [ TYEnviroment setObject:
        [ NSNumber numberWithBool:(([ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ]) ? YES : NO) ]
                      forKey:TYKidokumodeEnviroment ];
}

-(void)ty_reset:(NSMutableArray *)argments
{
    [ self resetGame:nil ];
}

-(void)ty_menu_window:(NSMutableArray *)argments
{
    [ self windowMode:nil ];
}

-(void)ty_menu_full:(NSMutableArray *)argments
{
    [ self fullScreenMode:nil ];
}

-(void)ty_savename:(NSMutableArray *)argments
{
    [ saveLoadController setNames:[ NSArray arrayWithObjects:
        [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ],
        [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ],
        [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:3 ] ],nil ] ];
}

-(void)ty_systemcall:(NSMutableArray *)argments
{
    NSString *arg=[ [ argments objectAtIndex:1 ] lowercaseString ];

    if([ arg isEqualToString:@"load" ]){
        [ self enterSaveLoadMenu:1 ];
    } else if([ arg isEqualToString:@"save" ]){
        [ self enterSaveLoadMenu:0 ];
    } else if([ arg isEqualToString:@"skip" ]){
        id envObj;
        envObj = [ TYEnviroment objectForKey:TYKidokumodeEnviroment ];
        if(envObj && [ envObj boolValue ]) 
            [ self skipWhileKidoku:nil ];
        else
            [ self skipToSelection:nil ];
    } else if([ arg isEqualToString:@"windowerase" ]){
        [ self eraseTextWindowMode:nil ];
    } else if([ arg isEqualToString:@"lookback" ]){
        [ self enterLookback:nil ];
    } else if([ arg isEqualToString:@"rmenu" ]){
        [ self openRightclickMenu:nil ];
    } else if([ arg isEqualToString:@"reset" ]){
        [ self enterYesNoModeTitle:NSLocalizedString(@"May I reset?",nil)
                         yesAction:@"_reset" 
                          noAction:@"" ];
    } else if([ arg isEqualToString:@"resetdlg" ]){
        [ self resetGame:[ NSNull null ] ];
    } else if([ arg isEqualToString:@"_reset" ]){
        [ self resetGame:nil ];
    } else if([ arg length ] >= 7) {
        NSString *cmd = [ arg substringWithRange:NSMakeRange(0,6) ];
        NSString *nostr = [ arg substringFromIndex:6 ];
        NSArray *names = [ saveLoadController names ];
        NSString *bookmark;
        NSString *title;
        
        if([ cmd isEqualToString:@"_qload" ]){
            bookmark = [ [ names objectAtIndex:2 ] stringByAppendingString:
                            TYWideCharDecimalStringWithString(nostr) ];
            title = [ NSString stringWithFormat:NSLocalizedString(@"May I Load?",nil),bookmark ];
            [ self enterYesNoModeTitle:title
                             yesAction:[ @"_eload" stringByAppendingString:nostr ]
                              noAction:@"" ];
        } else if([ cmd isEqualToString:@"_qsave" ]){
            bookmark = [ [ names objectAtIndex:2 ] stringByAppendingString:
                            TYWideCharDecimalStringWithString(nostr) ];
            title = [ NSString stringWithFormat:NSLocalizedString(@"May I Save?",nil),bookmark ];
            [ self enterYesNoModeTitle:title
                             yesAction:[ @"_esave" stringByAppendingString:nostr ]
                              noAction:@"" ];
        } else if([ cmd isEqualToString:@"_eload" ]) {
            [ NSObject cancelPreviousPerformRequestsWithTarget:ScriptEngine
                                                      selector:@selector(runScript)
                                                        object:nil ];
            [ self loadLocalData:[ NSNumber numberWithInt:[ nostr intValue ] ] ];
        } else if([ cmd isEqualToString:@"_esave" ]) {
            [ NSObject cancelPreviousPerformRequestsWithTarget:ScriptEngine
                                                      selector:@selector(runScript)
                                                        object:nil ];
            [ self saveLocalData:[ NSNumber numberWithInt:[ nostr intValue ] ] ];
        }
    }
}

-(void)enterYesNoModeTitle:(NSString*)title 
                 yesAction:(NSString*)yAction 
                  noAction:(NSString*)nAction
{
        TYMenuModeView *aView;
        aView = [ [ TYYesNoMenuView alloc ] initWithFrame:TYVirtualScreenRect()
                                            controller:self
                                            attributes:[ StageManager textAttDict ]
                                            titleAndAction:
                                                [ NSMutableArray arrayWithObjects:
                                                    title,
                                                    yAction,
                                                    nAction,nil ]
                                                layout:rightMenuLayout ];
        [ MainView addSubview:aView ];
        [ [ aView window ] makeFirstResponder:aView ];
        [ aView updatesTrackingRect ];
        [ self enterSystemMode ];
}

-(void)ty_rmenu:(NSMutableArray*)argments
{
    id temp;
    NSEnumerator *enu;
    
    if(rightMenuItemArray)
        [ rightMenuItemArray release ];
    
    rightMenuItemArray = [ [ NSMutableArray alloc ] initWithCapacity:[ argments count ] -1 ];
    
    enu = [ argments objectEnumerator ];
    [ enu nextObject ];
    while(temp = [ enu nextObject ]){
        [ rightMenuItemArray addObject:[ ScriptEngine getStringOfArgment:temp ] ];
        temp = [ enu nextObject ];
        [ rightMenuItemArray addObject:temp ];
    }
}

-(void)ty_menusetwindow:(NSMutableArray*)argments
{
    NSSize fontSize,pitchSize;
    int shadow,bold;
    NSColor *color;
    
    fontSize.width = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    fontSize.height = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    pitchSize.width = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    pitchSize.height = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];
    bold = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:5 ] ] intValue ];
    shadow = [ [ ScriptEngine getValueOfArgment:[ argments objectAtIndex:6 ] ] intValue ];
    color = getTextWindowColorWithHTMLFormat([ ScriptEngine getStringOfArgment:[ argments objectAtIndex:7 ] ]);
    
    [ rightMenuLayout setObject:[ NSValue valueWithSize:fontSize ] forKey:TYRMenuFontSize ];
    [ rightMenuLayout setObject:[ NSValue valueWithSize:pitchSize ] forKey:TYRMenuFontPitchSize ];
    [ rightMenuLayout setObject:[ NSNumber numberWithBool:shadow ] forKey:TYRMenuShadowed ];
    [ rightMenuLayout setObject:[ NSNumber numberWithBool:bold ] forKey:TYRMenuBold ];
    [ rightMenuLayout setObject:color forKey:TYRMenuBGColor ];
}

-(void)ty_menuselectcolor:(NSMutableArray*)argments
{
    NSColor *color;
    NSString* str;

    if(!rightMenuLayout)
        rightMenuLayout = [ [ NSMutableDictionary alloc ] initWithCapacity:8 ];

    str = [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    color = getColorWithHTMLFormat(str);
    [ rightMenuLayout setObject:color forKey:TYRMenuSelectColor ];
    
    str = [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ];
    color = getColorWithHTMLFormat(str);
    [ rightMenuLayout setObject:color forKey:TYRMenuNoSelectColor ];
    
    str = [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:3 ] ];
    color = getColorWithHTMLFormat(str);
    [ rightMenuLayout setObject:color forKey:TYRMenuNoDataColor ];
}

-(void)ty_lookbackcolor:(NSMutableArray*)argments
{
    lookbackColor = [ getColorWithHTMLFormat([ argments objectAtIndex:1 ]) retain ];
}

-(void)ty_lookbackbutton:(NSMutableArray*)argments
{
    lookbackBtn = [ NSArray arrayWithObjects:
                    [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ],
                    [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:2 ] ],
                    [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:3 ] ],
                    [ ScriptEngine getStringOfArgment:[ argments objectAtIndex:4 ] ],nil ];
    [ lookbackBtn retain ];
}

-(void)ty_getmousepos:(NSMutableArray*)argments
{
    NSPoint btnClickPoint;
    NSPoint screenPoint;
    NSPoint windowPoint;
    NSPoint viewPoint;

    // 10.2.5、convetPoint: fromView:nilがバグってる(泣
    screenPoint = [ NSEvent mouseLocation ];
    windowPoint = [ [ MainView window ] frame ].origin;
    viewPoint = [ MainView frame ].origin;
    
    btnClickPoint = NSMakePoint(screenPoint.x -windowPoint.x -viewPoint.x,
                                screenPoint.y -windowPoint.y -viewPoint.y);
    if(btnClickPoint.x < 0)
        btnClickPoint.x = 0;
    else if(btnClickPoint.x > VSCREEN_WIDTH)
        btnClickPoint.x = VSCREEN_WIDTH;
    
    if(btnClickPoint.y < 0)
        btnClickPoint.y = 0;
    else if(btnClickPoint.y > VSCREEN_HEIGHT)
        btnClickPoint.y = VSCREEN_HEIGHT;
    btnClickPoint.y = VSCREEN_HEIGHT -btnClickPoint.y;

    [ ScriptEngine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",[ argments objectAtIndex:1 ],[ NSNumber numberWithInt:btnClickPoint.x ],nil ] ];
    [ ScriptEngine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",[ argments objectAtIndex:2 ],[ NSNumber numberWithInt:btnClickPoint.y ],nil ] ];
}

-(void)ty_exec_dll:(NSMutableArray*)argments
{
    [ [ TYResourceServer sharedServer ] executeBundle:[ ScriptEngine getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
    [ ScriptEngine setBreakRun:YES ];
}

-(void)ty_getret:(NSMutableArray*)argments
{
    TYArgment *arg = [ argments objectAtIndex:1 ];
    
    if([ arg varType ] & TYDigitVarTypeMask) {
        [ ScriptEngine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",arg,
            [ NSNumber argmentWithInt:[ [ TYCCSProxy proxy ] returnCode ] ],nil ] ];
    } else if([ arg varType ] ==  TYStringVarType) {
        [ ScriptEngine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",arg,
            [ [ TYCCSProxy proxy ] returnString ],nil ] ];
    }
}

-(void)setTrap:(BOOL)aBool
{
    trapSelectAction = aBool;
}

- (void)changeFont:(id)fontManager
{
    [ StageManager changeFont:fontManager ];
}

- (IBAction)changeTextSpeed:(id)sender
{
    NSEnumerator *enu=[ [ SpeedMenu itemArray ] objectEnumerator ];
    id tmp;

    while((tmp = [ enu nextObject ])!=nil){
        [ tmp setState:NSOffState ];
    }
    [ sender setState:NSOnState ];
    
    [ StageManager changeTextSpeed:[ sender tag ] ];
}

-(int)selectedSpeedTag
{
    NSEnumerator *enu=[ [ SpeedMenu itemArray ] objectEnumerator ];
    id tmp;

    while((tmp = [ enu nextObject ])!=nil){
        if([tmp state ]==NSOnState){
            return [ tmp tag ];
        }
    }
    return -1;
}

- (IBAction)skipToSelection:(id)sender
{
    [ StageManager setSkipStatus:TYSelectionSkipMask ];
}

- (IBAction)skipWhileKidoku:(id)sender
{
    [ ScriptEngine setWatchKidoku:YES ];
    [ StageManager setSkipStatus:TYKidokuSkipMask ];
}

- (IBAction)versionDisp:(id)sender
{
    NSString *title;
    title = NSLocalizedString(@"SCRIPT_VERSION",nil);
    NSRunInformationalAlertPanel(title,scriptVersionStr,@"OK",nil,nil);
}
- (IBAction)openSaveDialog:(id)sender
{
    if([ self isPossibleEnterSystemMode ]){
        [ self enterSaveLoadMenu:0 ];
    } else {
        NSBeep();
    }

}

- (IBAction)openLoadDialog:(id)sender
{
    if([ self isPossibleEnterSystemMode ]){
        [ self enterSaveLoadMenu:1 ];
    } else {
        NSBeep();
    }
}

- (void)enterSaveLoadMenu:(int)loadmode
{
    NSArray *dArray;
    NSArray *eArray;
    TYSaveLoadMenuView *aView;
    
    dArray = [ saveLoadController dateStrings:loadmode ];
    eArray = [ saveLoadController existsArray ];
    
    aView = [ [ TYSaveLoadMenuView alloc ] initWithFrame:TYVirtualScreenRect()
                                              controller:self
                                                 layout:rightMenuLayout
                                                 attributes:[ StageManager textAttDict ]
        dateStrArray:[ saveLoadController dateStrings:loadmode ]
        existsArray:[ saveLoadController existsArray ]
        doLoad:loadmode ];
    [ MainView addSubview:aView ];
    [ [ aView window ] makeFirstResponder:aView ];
    [ aView updatesTrackingRect ];
    [ self enterSystemMode ];
}

/* obsolute
- (void)openSaveLoadDialog:(int)loadmode
{
    NSAutoreleasePool *aPool;
    NSDictionary* aDict;
    NSString *path,*filename;
    id aObj;
    int result;
    int i,num;
    
    if([ saveLoadController isLoaded ] == NO){
        // Loadされていなければセーブデータファイルを検索
        num = [ saveLoadController number ];
        aPool = [ [ NSAutoreleasePool alloc ] init ];
        for(i=1; i <= num; i++){
            filename = [ NSString stringWithFormat:SAVEDATA_NAME,i ];
            path = [ getNScrRootDirectory() stringByAppendingPathComponent:filename ];
            aDict = [ NSUnarchiver unarchiveObjectWithFile:path ];
            if(aDict){
                aObj = [ aDict objectForKey:TYMakeDateSaveData ];
                if(aObj)
                    [ saveLoadController setDate:aObj number:i ];
            }
        }
        [ aPool release ];

        [ saveLoadController setLoaded:YES ];
    }

    [ saveLoadController setLoadMode:loadmode ];
    [ NSBundle loadNibNamed:@"SaveLoadDialog" owner:saveLoadController ];

    if(loadmode){
        [ saveLoadController setTitle:@"LOAD" ];
    } else {
        [ saveLoadController setTitle:@"SAVE" ];
    }
    
    result = [[NSApplication sharedApplication]
            runModalForWindow:[ saveLoadController window ] ];
    [ [ saveLoadController window ] close ];

    if(result==DIALOG_OK){
        if(loadmode){
            // load
            [ self loadLocalData:[ NSNumber numberWithInt:[ saveLoadController selectedRow ] +1 ] ];
        } else {
            [ self saveLocalData:[ NSNumber numberWithInt:[ saveLoadController selectedRow ] +1 ] ];
        }
    }    
}
*/

- (IBAction)openAudioPathWindow:(id)sender
{
    if(!audioPathController){
        audioPathController = [ [ TYAudioPathController alloc ] init ];
        [ NSBundle loadNibNamed:@"AudioPath" owner:audioPathController ];
        [ audioPathController setDelegate:self ];
    } 

    [ audioPathController openWithPath:audioSearchPath defaultPath:[ TYEnviroment defaultObjectForKey:TYAudioFilePathEnviroment ] ];
}

- (IBAction)showDebugInfo:(id)sender
{
    TYGetDisplayInfo();
    NSRunInformationalAlertPanel(@"Debug Infomation",[ NSString stringWithFormat:@"Line:%d",[ ScriptEngine runLine ]+1 ], @"OK", nil, nil);

}

-(void)changeAudioPath:(NSString*)aPath
{
    if(audioSearchPath)
        [ audioSearchPath release ];

    audioSearchPath = [ aPath retain ];
    [ TYEnviroment setObject:audioSearchPath forKey:TYAudioFilePathEnviroment ];

    [ audioPathController autorelease ];
    audioPathController = nil;

    // BGM演奏中なら即座に再演奏チェック
    if(bgmPath){
        BOOL loop = ([ BGMView loopMode ]==NSQTMovieLoopingPlayback) ? YES : NO;
        if(loop || bgmForceSave){
            [ self playSound:bgmPath loop:loop forceSave:bgmForceSave ];
        }
    }
}

- (IBAction)windowMode:(id)sender
{
    NSRect mainFrame=[ MainView frame ];

    if(!isFullScreenMode)
        return;

    // システムモード中の変更は都合が悪いので
    if(isSystemMode){
        NSBeep();
        return;
    }

    isFullScreenMode=NO;

    TYEndFullScreen();
    
    [ [ MainView retain ] autorelease ];
    [ MainView removeFromSuperview ];
    [ [ MainWindow contentView ] addSubview:MainView ];
    mainFrame.origin.x = 0;
    mainFrame.origin.y = 0;
    mainFrame.size.width = VSCREEN_WIDTH;
    mainFrame.size.height = VSCREEN_HEIGHT;
    [ MainView setScaling:1.0 ];
    [ MainView setFrame:mainFrame ];

    [ MainWindow makeFirstResponder:MainView ];
    [ FullScrrenWindow setAcceptsMouseMovedEvents:NO ];
    [ MainWindow setAcceptsMouseMovedEvents:YES ];
    [ FullScrrenWindow orderOut:self ];
    [ MainWindow makeKeyAndOrderFront:self ];

    if([ TYEnviroment boolForKey:TYHideOtherFullScreenEnviroment ]) {
        [ NSApp unhideAllApplications:nil ];
    }
}

- (IBAction)fullScreenMode:(id)sender
{
#ifndef DEBUG
    NSRect screenRect;
    NSRect mainFrame=[ MainView frame ];
    NSSize fullScreenSize;

    if(isFullScreenMode)
        return;
    
    // システムモード中の変更は都合が悪いので
    if(isSystemMode){
        NSBeep();
        return;
    }

    if([ TYEnviroment boolForKey:TYDisplayResizeAtFullScreenEnviroment ]) {
        if([ TYEnviroment boolForKey:TYHideOtherFullScreenEnviroment ]) {
            [ NSApp hideOtherApplications:nil ];
        }
        fullScreenSize = TYStartFullScreen(VSCREEN_WIDTH,VSCREEN_HEIGHT);
    } else
        fullScreenSize = NSZeroSize;
    
    screenRect = [ [ FullScrrenWindow screen ] frame ];
    if(!NSEqualSizes(fullScreenSize,NSZeroSize)) {
        screenRect = NSMakeRect(0,
                                NSHeight(screenRect) -fullScreenSize.height,
                                fullScreenSize.width,
                                fullScreenSize.height);
    }
    
    [ [ MainView retain ] autorelease ];
    [ MainView removeFromSuperview ];
    [ [ FullScrrenWindow contentView ] addSubview:MainView ];
    {
        mainFrame.origin.x = (screenRect.size.width -mainFrame.size.width) /2;
        mainFrame.origin.y = (screenRect.size.height -mainFrame.size.height) /2;
    }
    [ MainView setFrame:mainFrame ];

    /*
     [ [ BGMView retain ] autorelease ];
     [ BGMView removeFromSuperview ];
     [ [ MainWindow contentView ] addSubview:BGMView ];
     */

    [ FullScrrenWindow setFrame:screenRect display:YES ];

    //[ MainWindow orderOut:self ];
    //[ MainView removeFromSuperview ];
    [ FullScrrenWindow makeFirstResponder:MainView ];
    [ FullScrrenWindow setLevelToFront ];
    [ FullScrrenWindow makeKeyAndOrderFront:self ];
    [ MainWindow setAcceptsMouseMovedEvents:NO ];
    [ FullScrrenWindow setAcceptsMouseMovedEvents:YES ];
    
    isFullScreenMode = YES;
#endif
}

-(void)didChangeKeyWindow:(NSNotification*)notification
{
    if(isFullScreenMode){
        if([ notification object ] != FullScrrenWindow){
            [ FullScrrenWindow setLevelToNormal ];
        } else {
            [ FullScrrenWindow setLevelToFront ];
        }
    }
}

- (IBAction)openBGMVolumeDialog:(id)sender
{
    if(!volumeController){
        float bV;
        id temp;
        bV = (temp = [ TYEnviroment objectForKey:TYBGMVolumeEnviroment ]) ? [ temp floatValue ] : defBGMVol;
        
        if(!mode_ext) {
            volumeController = [ [ TYBGMVolumeController alloc ] initWithBGMVolume:bV ];
        } else {
            float vV,sV;
            vV = (temp = [ TYEnviroment objectForKey:TYVoiceVolumeEnviroment ]) ? [ temp floatValue ] : defVoiceVol;
            sV = (temp = [ TYEnviroment objectForKey:TYSEVolumeEnviroment ]) ? [ temp floatValue ] : defSEVol;
            
            volumeController = [ [ TYExtVolumeController alloc ]initWithDefaultBGMVolume:defBGMVol
                                                                              bgmVolume:bV
                                                                           defaultVoiceVolume:defVoiceVol
                                                                                  voiceVolume:vV
                                                                              defaultSEVolume:defSEVol
                                                                                     seVolume:sV ];
        }
        
                 //[ [ (mode_ext) ? TYBGMVolumeController : TYExtVolumeContoller alloc ] init ];
        //[ NSBundle loadNibNamed:@"BGMVolume" owner:self ];
    }
    [ volumeController setDelegate:self ];
    [ volumeController showDialog ];
    [ self closeBGMVolumeDialog:volumeController ];
    
    //[ VolumeSlider setFloatValue:[ BGMView volume ] ];

    //[[NSApplication sharedApplication] runModalForWindow:[ VolumeSlider window ] ];
}

- (IBAction)preference:(id)sender
{
    [ TYPreferencePanelController showDialog ];
}

- (void)changeBGMVolume:(id)sender
{
    float volume = [ sender floatValue ];

    BGMVolume = volume;
    [ BGMView setVolume:volume ];
}

- (void)closeBGMVolumeDialog:(id)sender
{
    [ TYEnviroment setObject:[ NSNumber numberWithFloat:BGMVolume ]
                      forKey:TYBGMVolumeEnviroment ];
    if(mode_ext){
        [ TYEnviroment setObject:[ NSNumber numberWithFloat:[ volumeController voiceVolume ] ]
                          forKey:TYVoiceVolumeEnviroment ];
        voiceVol = [ volumeController voiceVolume ] *100 -100;

        [ TYEnviroment setObject:[ NSNumber numberWithFloat:[ volumeController seVolume ] ]
                          forKey:TYSEVolumeEnviroment ];
        seVol = [ volumeController seVolume ] *100 -100;        
    }
}

- (IBAction)openAutoMode:(id)sender
{
    static TYAutoModeController *autoDialog;
    if(!autoDialog)
        autoDialog=[ [ TYAutoModeController dialog ] retain ];

    if([ TYEnviroment objectForKey:TYAutomodeWaitTimeEnviroment ] == nil) {
        [ TYEnviroment setObject:[ NSNumber numberWithInt:0 ] forKey:TYAutomodeWaitTimeEnviroment ];
    }

    if([ autoDialog runModalTextSpeed:[ StageManager userTextSpeed ]
            waitValue:[ [ TYEnviroment objectForKey:TYAutomodeWaitTimeEnviroment ] intValue ]
            waitType:[ [ TYEnviroment objectForKey:TYAutomodeWaitTypeEnviroment ] intValue ] ] 
              == NSOKButton){
        [ TYEnviroment setObject:[ NSNumber numberWithInt:[ autoDialog waitNum ] ] forKey:TYAutomodeWaitTimeEnviroment ];
        [ TYEnviroment setObject:[ NSNumber numberWithInt:[ autoDialog waitType ] ] forKey:TYAutomodeWaitTypeEnviroment ];
        [ StageManager setAutoMode:[ autoDialog textSpeed ]
                          waitType:[ autoDialog waitType ]
                              wait:[ autoDialog waitNum ] ];
    }
}

- (IBAction)eraseTextWindowMode:(id)sender
{
    TYEraseModeView *aView;
    
    if(sender && ![ self isPossibleEnterSystemMode ]){
        NSBeep();
        return;
    }
    
    aView = [ [ TYEraseModeView alloc ] initWithFrame:TYVirtualScreenRect() controller:self ];
    [ aView autorelease ];
    [ MainView addSubview:aView ];
    [ [ aView window ] makeFirstResponder:aView ];
    [ self enterSystemMode ];
}

- (IBAction)openRightclickMenu:(id)sender
{
    if(sender && ![ self isPossibleEnterSystemMode ]){
        NSBeep();
        return;
    }
    
    if(!menuModeView){
        menuModeView = [ [ TYRootMenuView alloc ] initWithFrame:TYVirtualScreenRect()
                                            controller:self
                                            attributes:[ StageManager textAttDict ]
                                            menuItems:rightMenuItemArray
                                                layout:rightMenuLayout ];
    }
    
    [ MainView addSubview:menuModeView ];
    [ [ menuModeView window ] makeFirstResponder:menuModeView ];
    [ menuModeView updatesTrackingRect ];
    
    [ self enterSystemMode ];
}

- (IBAction)enterLookback:(id)sender
{
    TYLookbackView *aView;
    NSMutableDictionary *aDict;
    NSArray *queue;
    TYCellImage *img1,*img2;
    
    if(sender && ![ self isPossibleEnterSystemMode ]){
        NSBeep();
        return;
    }
    queue = [ StageManager lookbackQueue ];
    if(![ queue count ])
        return;
    
    // ボタン画像生成
    {
        TYResourceServer *server = [ TYResourceServer sharedServer ];
        NSImage *uoncur,*uoffcur,*doncur,*doffcur;
        
        if(!lookbackBtn) {
            lookbackBtn = [ NSArray arrayWithObjects:@"uoncur.bmp",
                                                     @"uoffcur.bmp",
                                                     @"doncur.bmp",
                                                     @"doffcur.bmp",nil ];
            [ lookbackBtn retain ];
        }
        
        uoncur = [ server getImage:[ lookbackBtn objectAtIndex:1 ] transMode:YES ];
        uoffcur = [ server getImage:[ lookbackBtn objectAtIndex:0 ] transMode:YES ];
        
        doncur = [ server getImage:[ lookbackBtn objectAtIndex:3 ] transMode:YES ];
        doffcur = [ server getImage:[ lookbackBtn objectAtIndex:2 ] transMode:YES ];
        
        if(uoncur && uoffcur)
            img1 = [ [ TYCellImage alloc ] initWithImages:
                        [ NSArray arrayWithObjects:uoncur,uoffcur,nil ] ];
        else
            img1 = nil;
            
        if(doncur && doffcur)
            img2 = [ [ TYCellImage alloc ] initWithImages:
                        [ NSArray arrayWithObjects:doncur,doffcur,nil ] ];
        else
            img2 = nil;
    }
    
    aDict = [ [ [ StageManager textAttDict ] mutableCopy ] autorelease ];
    if(lookbackColor)
        [ aDict setObject:lookbackColor forKey:NSForegroundColorAttributeName ];
    aView = [ [ TYLookbackView alloc ] initWithFrame:TYVirtualScreenRect()
                                          controller:self
                                          attributes:aDict
                                              layout:rightMenuLayout
                                              buffer:queue
                                             ubutton:img1
                                             dbutton:img2 ];
    [ aView autorelease ];
    
    [ MainView addSubview:aView ];
    [ aView updatesTrackingRect ];
    [ [ aView window ] makeFirstResponder:aView ];
    
    [ self enterSystemMode ];

}

- (IBAction)resetGame:(id)sender
{
    if(sender){
        if(![ sender respondsToSelector:@selector(tag:) ]) {
            if(NSRunAlertPanel(NSLocalizedString(@"Reset Game",nil),
                                  NSLocalizedString(@"May I reset?",nil),
                                  @"OK",
                                  nil,
                                 @"Cancel") == NSAlertOtherReturn){
                return ;
            }
        } else if(![ self isPossibleEnterSystemMode ]){
            // メニューから呼ばれた場合で、遷移できない時。
            NSBeep();
            return;
        } else {
            [ self ty_systemcall:[ NSArray arrayWithObjects:@"systemcall",@"reset",nil ] ];
            return ;
        }            
    }
    
    if([ self status ] == TYScriptRunningStatus){
        [ ScriptEngine setBreakRun:YES ];
    }
    
    [ self setStatus:TYScriptRunningStatus ];
    [ ScriptEngine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];

    
    [ BGMView stop:self ];
    [ bgmPath release ];
    bgmPath=nil;

    [ waveController release ];
    waveController=nil;

    autoclickTimer=0;

    [ StageManager resetGame ];
    [ ScriptEngine resetGame ];
}

- (IBAction)forceResume:(id)sender
{
    [ ScriptEngine runScript ];
}

-(void)select_action
{
    if(trapSelectAction && TYStatus != TYPrinting)
        [ ScriptEngine fireOfTrap ];
    
    switch (TYStatus) {
    case TYClickWaitStatus:
        [ NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(select_action) object:nil ];
        [ ScriptEngine runScript ];
        break;
    case TYPrinting:
        [ StageManager select_action ];
        break;
    case TYButtonWaitStatus:
        //[ MainWindow setAcceptsMouseMovedEvents:NO ];
            
        [ StageManager select_action ];
        break;
    case TYSelectStatus:
        [ StageManager select_action ];
    default : break;
    }
}

-(IBAction)cancel_action:(id)sender
{
    switch (TYStatus) {
        case TYButtonWaitStatus:
            [ StageManager cancel_action:sender ];
            break;
        case TYSelectStatus:
            [ self openRightclickMenu:nil ];
            break;
        case TYScriptRunningStatus:
        case TYClickWaitStatus:
            break;
        default:
            [ StageManager cancel_action:sender ];
    }
}

-(void)up_action
{
    switch (TYStatus) {
    case TYButtonWaitStatus:
    case TYSelectStatus:
    case TYPrinting:
        [ StageManager up_action ];
        break;
    default : break;
    }
}
 
-(void)down_action
{
    switch (TYStatus) {
    case TYButtonWaitStatus:
    case TYSelectStatus:
        [ StageManager down_action ];
        break;
    default : break;
    }
}

-(void)move_mouse:(NSPoint)point
{
    switch (TYStatus){
    case TYButtonWaitStatus:
    case TYSelectStatus:
        [ StageManager move_mouse:point ];
        break;
    default:break;
    }
}

-(void)other_action:(id)code
{
    switch (TYStatus){
    case TYButtonWaitStatus:
        [ StageManager other_action:code ];
        break;
    default:break;
    }
}

-(void)enterSystemMode
{        
    isSystemMode = YES;
        
    switch (TYStatus){
    case TYPrinting:
        [ StageManager enterSystemMode ];
        break;
    case TYScriptRunningStatus:
        [ ScriptEngine setBreakRun:YES ];
        [ NSObject cancelPreviousPerformRequestsWithTarget:ScriptEngine
                                                  selector:@selector(runScript)
                                                    object:nil ];
        break;
    default:break;
    }
    [ MainView setImage:[ StageManager visualLayer ] ];
}

-(void)exitSystemModeResumeStatus:(BOOL)aBool
{
    switch (TYStatus & (TYRmodeStatusMask -1)){
    case TYPrinting:
    case TYSelectStatus:
        [ StageManager exitSystemModeResumeStatus:aBool ];
        break;
    case TYScriptRunningStatus:
        [ ScriptEngine performSelector:@selector(runScript)
                            withObject:nil
                            afterDelay:0.1 ];
        break;
    default:break;
    }

    isSystemMode = NO;
}

-(BOOL)isPossibleEnterSystemMode
{
    // ユーザからの要請によってシステムモードへ移行可能な状態か返す。
    if(isSystemMode)
        return NO;
    
    switch (TYStatus){
    case TYPrinting:
        return [ StageManager isPossibleEnterSystemMode ];
    case TYSelectStatus:
        return YES;
    default:
        return NO;
    }
}


@end
