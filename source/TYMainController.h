//
//  TYMainController.h
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

#import <Cocoa/Cocoa.h>
#import <QuickTime/QuickTime.h>
#import "TYSaveDataCoding.h"

#define SAVEDATA_NAME @"save%d.msav"
#define REGISTRY_NAME @"REGISTRY.plist"
#define AUDIO_TEMP @".temp"
#define DIRECT_WAVE_MAX 51
#define DIRECT_WAVE_REPLAY 50

@protocol TYController 
    -(void)select_action;
    -(IBAction)cancel_action:(id)sender;
    -(void)up_action;
    -(void)down_action;
    -(void)move_mouse:(NSPoint)point;
    -(void)other_action:(id)code;
@end

@class TYScriptEngine ;
@class TYStageManager ;
@class TYSoundController ;
//@class TYSaveLoadDialogController;
@class TYSaveLoadManager;
@class TYExtVolumeController;
@class TYVisualNovelView;
@class TYAudioPathController;
@class TYMenuModeView;
@class TYMovieControllerView;

// Appのデレゲート、および各Viewに対するコントローラーの役割を果たす。
@interface TYMainController : NSObject <TYController,TYSaveDataCoding>
{
    IBOutlet id MainMenu;
    IBOutlet TYVisualNovelView *MainView;
    IBOutlet id MainWindow;
    IBOutlet id FullScrrenWindow;
    IBOutlet id SpeedMenu;
    IBOutlet id SkipMenu;
    IBOutlet id NScrMenu;
    IBOutlet id ContextMenu;

    NSWindow *targetWindow;
    NSView *defaultContentView;

    TYSaveLoadManager *saveLoadController;

    NSMovieView *BGMView;
    Movie qtMovie;
    NSTimer *gMyTimer;
    NSString *bgmPath;
    BOOL bgmForceSave;
    NSString *tmpPath;

    BOOL isFullScreenMode;
    
    TYScriptEngine *ScriptEngine;
    TYStageManager *StageManager;

    NSString *audioSearchPath;
    int autoclickTimer;
    
    // 状態定義定数
    enum {
        TYScriptRunningStatus=1, // スクリプト動作中
        //TYEffectStatus, // エフェクトの表示処理中(スクリプト動作中と同義なので統合）
        TYClickWaitStatus=2, // クリック待ち
        TYTimeWaitStatus=3, // 時間待ちイベント
        //TYClickAndTimeWaitStatus, // 時間、クリックのどちらも受け付ける(クリック待ちと統合）
        TYSelectStatus=4, // 選択肢を待つ
        TYButtonWaitStatus=5, // ボタン選択を待つ
        TYPrinting=6, // 文字列表示中
        TYRmodeStatusMask=8,
    } TYStatus;

    NSString *scriptVersionStr;

    BOOL trapSelectAction;

    TYAudioPathController *audioPathController;

    float BGMVolume;
    TYExtVolumeController *volumeController;
    float defBGMVol,defVoiceVol,defSEVol;
    TYSoundController *waveController;
    TYSoundController *directWaveController[DIRECT_WAVE_MAX];
    int voiceVol,seVol;
    NSMutableSet *usingDirectWaveCannelSet;
    
    BOOL mode_ext;
    BOOL enable_automode;
    int automode_time;

    BOOL isModal;

    NSMovieView *movieView;
    TYMovieControllerView *ctrlView;
    
    // 右クリックメニュー
    NSMutableArray *rightMenuItemArray;
    NSMutableDictionary *rightMenuLayout;
    TYMenuModeView *menuModeView;
    BOOL isSystemMode;
    NSColor *lookbackColor;
    NSArray *lookbackBtn;
}
- (IBAction)changeTextSpeed:(id)sender;
- (IBAction)skipToSelection:(id)sender;
- (IBAction)skipWhileKidoku:(id)sender;
- (IBAction)openAutoMode:(id)sender;
- (IBAction)versionDisp:(id)sender;
- (IBAction)openSaveDialog:(id)sender;
- (IBAction)openLoadDialog:(id)sender;
- (IBAction)openAudioPathWindow:(id)sender;
- (IBAction)showDebugInfo:(id)sender;
- (IBAction)windowMode:(id)sender;
- (IBAction)fullScreenMode:(id)sender;
- (IBAction)openBGMVolumeDialog:(id)sender;
- (IBAction)eraseTextWindowMode:(id)sender;
- (IBAction)preference:(id)sender;
- (IBAction)resetGame:(id)sender;
- (IBAction)forceResume:(id)sender;
- (IBAction)openRightclickMenu:(id)sender;
- (IBAction)enterLookback:(id)sender;

-(void)enterSystemMode;
-(void)exitSystemModeResumeStatus:(BOOL)aBool;
-(void)enterYesNoModeTitle:(NSString*)title 
                 yesAction:(NSString*)yAction 
                  noAction:(NSString*)nAction;
-(BOOL)isPossibleEnterSystemMode;

+(void)alertAndTerminate:(NSString*)title msg:(NSString*)msg;
+(id)controller;

//- (void)openSaveLoadDialog:(int)loadmode;
- (void)enterSaveLoadMenu:(int)loadmode;

- (void)closeBGMVolumeDialog:(id)sender;
- (void)changeBGMVolume:(id)sender;

-(void)setStatus:(int)aStatus;
-(int)status;
-(void)saveLocalData:(NSNumber*)number;
-(BOOL)loadLocalData:(NSNumber*)number;

-(int)autoclickTimer;
-(void)setTrap:(BOOL)aBool;
//-(void)endButton;

-(void)setImage:(NSImage*)aImage;
-(void)setImage:(NSImage*)aImage inRect:(NSRect)inRect;
-(NSImage*)image;
-(void)directDrawImage:(NSImage*)sourceImage inRect:(NSRect)inRect fromRect:(NSRect)fromRect;
-(NSImage*)makeImageFromMainView;
-(void)setNeedsDisplayMainView:(BOOL)needs;
-(void)setAcceptsMouseMovedEvents:(BOOL)flag;

-(void)disableContextMenu;

-(void)playSound:(NSString*)argment loop:(BOOL)aBool forceSave:(BOOL)save; // private
-(void)playWaveSound:(NSString*)argment loop:(BOOL)aBool; // private
-(void)playWaveChannel:(int)ch path:(NSString*)aPath loop:(BOOL)aBool; // private
-(void)playMovie:(NSString*)aPath cancel:(BOOL)aBool;
-(void)endCheckMovie;
-(void)endPlayMovie;

-(void)lockFocusMainView;
-(void)unlockFocusMainView;
-(NSWindow*)mainWindow;

- (void)changeFont:(id)fontManager;

- (void)setAudioPath:(NSString*)aPath;

-(int)selectedSpeedTag;
// recept Notification
-(void)didChangeKeyWindow:(NSNotification*)notification;

-(void)setEnableAutomode:(BOOL)aBool;

// 公開ハンドラ
-(void)ty_playstop:(NSMutableArray *)argments;
-(void)ty_defvoicevol:(NSMutableArray *)argments;
-(void)ty_defsevol:(NSMutableArray *)argments;
-(void)ty_systemcall:(NSMutableArray *)argments;
-(void)ty_menusetwindow:(NSMutableArray *)argments;
-(void)ty_dwave:(NSMutableArray *)argments;

extern NSString* const TYRMenuFontSize;
extern NSString* const TYRMenuFontPitchSize;
extern NSString* const TYRMenuShadowed;
extern NSString* const TYRMenuBold;
extern NSString* const TYRMenuBGColor;
extern NSString* const TYRMenuSelectColor;
extern NSString* const TYRMenuNoSelectColor;
extern NSString* const TYRMenuNoDataColor;

extern NSString* const TYVoicePlayNotification;
@end

