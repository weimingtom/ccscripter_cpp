//
//  TYStageManager.h
//  Tukuyomi
//
//  Created by toveta on Sun Aug 19 2001.
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
#import "TYEffecter.h"
#import "TYScriptEngine.h"
#import "TYMainController.h"
#import "TYSaveDataCoding.h"

@class TYSelectionManager;
@class TYEffectGenerater;
@class TYStandingChar;
@class TYWaitCursor;
@class TYExtraButtonManager;

typedef enum tySkipStatusMask {
    TYNoSkip = 0,
    TYOnetimeSkipMask = 1,
    TYSelectionSkipMask = 1 << 1,
    TYKidokuSkipMask = 1 << 2,
    TYAutoModeSkipMask = 1 << 3,
    TYEffectCancelSkipMask = TYSelectionSkipMask | TYKidokuSkipMask ,
    TYNoTimeWaitSkipMask = TYOnetimeSkipMask | TYSelectionSkipMask | TYKidokuSkipMask,
} TYSkipStatusMask;

#define BAR_NUMBER_MAX 99

// オフスクリーンバッファの変更を行うクラス。テキスト表示中の制御も兼ねてる
@interface TYStageManager : NSObject < TYController,TYSaveDataCoding > {
    TYMainController *controller; // イメージを引き渡すコントローラー。
    id engine; // スクリプトエンジン
    NSImage *compositeLayer; // 画像レイヤーとテキストレイヤーを統合したレイヤー
    NSMutableDictionary *effectDefDict; // エフェクト定義登録辞書
    TYEffectGenerater *effectGenerater; // エフェクト効果作成クラス
    id afterEffectTarget; // エフェクト終了時に叩かれるオブジェクト
    SEL afterEffectSelector; // 同セレクタ
    id afterEffectObject; // 同引数オブジェクト
    //TYEffecter *effecter; // エフェクトを実現する下請けオブジェクト。
    
    NSImage *visualLayer; // 背景とか立ちキャラとかスプライトとか。
    
    // 背景情報
    NSImage *bgImage;
    NSString *bgPath;
    NSRect bgRect; // 背景の描画領域
    
    // 立ちキャラ情報
    float underline;
    TYStandingChar *leftCharImage;
    TYStandingChar *centerCharImage;
    TYStandingChar *rightCharImage;
    
    // スプライト
    NSMutableArray *spriteArray;
    int humanz; // 立ち絵がこの番号までのスプライトの上に表示される。
    BOOL isUseHumanz; // スクリプト中からhumanz命令が使われたことを示すフラグ。
    BOOL isWindowback; // スプライトをテキストウインドウの上に描画する。
    NSMutableArray *hideSpriteArray; // allsphide 命令で一時的にvisible:NO にしたスプライト
    
    // 特殊フィルタ
    NSData *monocroSample;
    int nega;
    
    //NSMutableArray *textQueue;
    NSImage *novelLayer; // 文字とテキストウインドウのレイヤー
    NSLayoutManager *layoutManager;
    NSString *textQueue;
    
    NSImage *overNovelLayer;
    
    NSMutableDictionary *textAttDict;
    NSMutableDictionary *textShadowAttDict;
    NSString *textFontName;
    NSFont *textFont;
    NSSize displayFontSize; // 実際に表示されるフォントのサイズ
    
    BOOL novelLayerOn; // テキストウインドウ表示フラグ
    BOOL eraseTextWindow;
    
    int textLeftOffset;
    int textTopOffset;
    int textColumn;
    int textRow;
    int textFontWidth;
    int textFontHeight;
    NSColor *textFontColor;
    int textPitchx;
    int textPitchy;
    int *textSpeed;
    int userTextSpeed;
    int scriptTextSpeed;
    NSArray *defaultSpeed;
    int selectedSpeedTag;
    BOOL textBold;
    BOOL textShadowed;
    
    int locateX;
    
    id textWindow;
    NSString *textWindowPath;
    NSRect textWindowRect;
    //NSPoint textWindowPoint;
    //NSSize textWindowSize;
    
    enum TYTPStatus { // テキスト表示状態時のステータス
        TYTPOff,
        TYTPPrinting,
        TYTPClickWait,
        TYTPTimeWait,
    } tpStatus;
    BOOL waitNewPage; // 改頁待ちフラグ
    BOOL forceClickWait; // 強制クリック待ちフラグ
    BOOL forceWaitNewPage; // 強制改ページ待ちフラグ
    BOOL invalidNewLine; // 次改行無効フラグ
    NSCharacterSet *clickStrSet;
    int clickStrUnderLimit;
    NSCharacterSet *hyphenationSet; // 禁則処理用文字セット

    int nowColumn;
    int nowRow;
    int nowStorageLength;
    int nowStorageOffset;
    
    //NSString *textQueue;

    // エフェクト終了後に起動するメソッドとレシーバー
    /*
    id nextTarget;
    id nextObject;
    SEL nextSelector;
    */
    
    // ボタン処理関連
    TYExtraButtonManager *buttonManager;
    int beforeSelectButtonID;
    NSString *buttonTargetValue;
    BOOL releaseButtonAfterSelect;
    BOOL isDrawButton;
    NSNumber *btnTime;
    NSDate *btnStartDate;
    
    // 選択肢処理
    TYSelectionManager *selectionManager;
    int beforeSelectIndex;
    NSArray *selectAction;
    NSColor *selectColor;
    NSColor *unSelectColor;
    
    // スキップ制御
    int skipStatus;
    int automodeWait;
    int automodeWaitType;
    int waitCountOfChar;
    BOOL playingSoundFlg;
    BOOL waitFinishSound;
    BOOL waitFinishSoundOnBtnTime2;

    // クリック待ちカーソル
    TYWaitCursor *clickWaitCursor;
    TYWaitCursor *pageWaitCursor;
    TYWaitCursor *currentCursor;
    
    BOOL linepage,linepageWait;
    
    // システムカスタマイズ
    NSString *textgosubLabel;
    BOOL isPage;
    NSMutableArray *cselInfo; // 文字列、コマンドの順に格納。
    NSMutableArray *cselImage;
    
    // 回想バッファ
    NSMutableArray *lookbackQueue;
    
    // バー
    NSMutableDictionary *barDict;
}
+(id)sharedManager;

+(void)setModeSVGA;

-(void)setController:(id)aController engine:(id)aEngine;

-(void)enterSystemMode;
-(void)exitSystemModeResumeStatus:(BOOL)aBool;
-(BOOL)isPossibleEnterSystemMode;

-(void)loadTextWindow:(NSString*)path;
-(void)loadBGImage:(NSString*)aPath;
-(void)addSprite:(NSMutableArray *)argments visible:(BOOL)aBool; // スプライトを追加

-(void)startSelect:(NSArray*)messages responds:(NSArray*)respondArray;
 
-(void)updateStage; // 画像の再構築
-(void)updateStageOnlyButton;
-(NSImage*)visualLayer;
-(NSImage*)compositeLayer;
-(void)changeVisualLayer:(NSNumber*)effectNum; // 背景等に変更があったときに呼ばれる
-(void)flushNovelLayer; // novelLayerの初期化とcompositeLayerの再描画。
-(void)startEraseTextWindow:(NSNumber*)effectNum;
//-(void)setNextPerformSelector:(SEL)aSelector target:(id)target withObject:(id)object;
-(NSArray*)lookbackQueue;

//-(void)addTextQueue:(NSString*)aString; // テキストをキューに追加
-(void)textPrint:(NSString*)aString; // テキストの表示開始
-(void)drawTextWindow;
-(NSRect)textWindowRect;
-(void)drawChar:(NSString*)charStr color:(NSColor*)color atColumn:(int)column atRow:(int)row;
-(void)drawAttStr:(NSAttributedString*)str atColumn:(int)column atRow:(int)row;
-(NSPoint)drawPoint;
-(void)setSkipStatus:(int)aStatus;
-(int)skipStatus;
-(void)setClickWait:(BOOL)pageWait;
//-(void)rectOfColumnRange:(NSRange)columnRange rowRange:(NSRange)rowRange;
//-(void)ty_bg:(NSString*)path effectNo:(int)effectNo; // 背景画像変更

-(void)resetGame;

-(int)status;
-(void)setStatus:(int)status;

-(void)printing;
-(void)resumePrinting;
-(void)automode_select_action; // Automodeフラグを保持したままになる。

// 公開ハンドラ
-(void)ty_select:(NSMutableArray *)argments;
-(void)ty_spstr:(NSMutableArray *)argments;
-(void)ty_textclear:(NSMutableArray*)argments;
-(void)ty_lookbackflush:(NSMutableArray*)argments;

//-(void)setTextWindow:(NSArray)argments;
-(void)makeAttributeDict; // フォントの修飾情報の再構築
-(NSDictionary*)textAttDict;
-(int)textFontWidth;
-(int)textFontHeight;
-(int)textPitchx;
-(BOOL)isShadow;

- (void)changeFont:(id)fontManager;
- (void)changeTextSpeed:(int)index;
- (void)setAutoMode:(int)speed
           waitType:(int)type
               wait:(int)wait;
- (int)userTextSpeed;

-(void)startButtonWait:(NSMutableArray*)argments releaseAfterSelect:(BOOL)aBool;
-(void)changeSelectedButton:(unsigned int)btnIndex;
-(void)responseButton:(int)result;
-(void)changeSelection:(int)index;
-(void)responseSelection;
-(void)makeCselImage;

// sprite operation
-(NSRect)rectOfSprite:(int)idNo;
-(void)setSprite:(int)idNo visible:(BOOL)aBool;
-(void)setSprite:(int)idNo cell:(int)cellNo;

/* delegate methods of EffectGeneraters*/
-(void)changeEffectionImage:(NSImage*)drawImage;
-(void)effectFinished;

// notification
-(void)updateSprites:(NSNotification*)aNotification; // from TYNScrSprite
-(void)startWaveSound:(NSNotification*)aNotification; // from TYWaveController
-(void)stopWaveSound:(NSNotification*)aNotification;

//#define VSCREEN_WIDTH 640
//#define VSCREEN_HEIGHT 480

extern int VSCREEN_WIDTH;
extern int VSCREEN_HEIGHT;
#define LD_LOCATE_LEFT @"l"
#define LD_LOCATE_CENTER @"c"
#define LD_LOCATE_RIGHT @"r"
#define CL_LOCATE_ALL @"a"
#define WINDOW_EFFECT_NO -2
#define SHADOW_TICKNESS 1
#define SPRITE_MAX 255
#define DEFAULT_TEXTWINDOW @"#999999"
extern NSString* const CSEL_LABEL;
#define CSEL_SPRITE_NO 500

extern NSSize TYVirtualScreenSize();
extern NSRect TYVirtualScreenRect();
extern NSRect TYFromRect(NSPoint*,NSSize*,NSSize*);

@end
