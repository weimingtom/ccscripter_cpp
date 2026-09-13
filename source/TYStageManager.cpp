//
//  TYStageManager.m
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

#import "TYStageManager.h"
#import "TYResourceServer.h"
//#import "TYMainController.h"
#import "TYImageUtil.h"
#import "TYMiscUtil.h"
#import "TYNScrSprite.h"
#import "TYCellImage.h"
#import "TYStringImage.h"
#import "TYSelectionManager.h"
#import "TYEffectGenerater.h"
#import "TYQuakeEffectGenerater.h"
#import "TYNew_QuakeEffectGenerater.h"
#import "TYNovelLayer.h"
#import "TYStandingChar.h"
#import "TYEnviroment.h"
#import "TYWaitCursor.h"
#import "StopNSLog.h"
#import "TYExtraButtonManager.h"
#import "TYAutoModeController.h"
#import "TYArgment.h"
#import "TYConstStringArgment.h"
#import "TYBar.h"
#import "TYSoundController.h"

@implementation TYStageManager

int VSCREEN_WIDTH=640;
int VSCREEN_HEIGHT=480;

NSString* const CSEL_LABEL=@"*customsel";

static NSCharacterSet *charcterOfReferenceVarSet;

static NSRect TYFontRect(const NSPoint*,NSRect*,BOOL);

NSRect TYVirtualScreenRect() { return NSMakeRect(0,0,VSCREEN_WIDTH,VSCREEN_HEIGHT); }
NSSize TYVirtualScreenSize() { return NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT); }

NSRect TYFromRect(NSPoint* drawPoint,NSSize* imageSize,NSSize* bgSize)
{
    NSRect imageRect;
    NSRect bgRect;
    
    imageRect = NSMakeRect(0,0,imageSize->width,imageSize->height);
    bgRect = NSMakeRect(-drawPoint->x,-drawPoint->y,bgSize->width,bgSize->height);
    
    return NSIntersectionRect(imageRect,bgRect);
}

NSRect TYFontRect(const NSPoint* drawPoint,NSRect* fontRect,BOOL shadowed)
{
        return NSMakeRect(drawPoint->x +fontRect->origin.x,
                    drawPoint->y +fontRect->origin.y -shadowed *SHADOW_TICKNESS,
                    fontRect->size.width  +shadowed *SHADOW_TICKNESS,
                    fontRect->size.height +shadowed *SHADOW_TICKNESS);
}

+(void)initialize
{
    charcterOfReferenceVarSet = [ [ NSCharacterSet characterSetWithCharactersInString:@"$%" ] retain ];
}

static id sharedManager;

+(id)sharedManager
{
    return sharedManager;
}

+(void)setModeSVGA
{
    VSCREEN_WIDTH=800;
    VSCREEN_HEIGHT=600;
}

- (id)init
{
    if(!sharedManager){
        self = [super init];
        sharedManager = self;
    } else {
        return sharedManager;
    }
        
    // エフェクト定義辞書作成
    //effecter = [ [ TYEffecter alloc ] init ]; 
    effectDefDict = [ [ NSMutableDictionary alloc ] initWithObjectsAndKeys:
                        [ TYEffectDefinitionValue  valueWithEffectDefinition:
                            TYMakeEffectDefinition(TYEffectInstant,0,nil) ],
                        [ NSNumber numberWithInt:-2 ],
                        [ TYEffectDefinitionValue  valueWithEffectDefinition:
                            TYMakeEffectDefinition(TYEffectCached,0,nil) ],
                        [ NSNumber numberWithInt:0 ],
                        [ TYEffectDefinitionValue  valueWithEffectDefinition:
                            TYMakeEffectDefinition(TYEffectInstant,0,nil) ],
                        [ NSNumber numberWithInt:1 ],
                        nil ];

    visualLayer = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];
    /*
    [ visualLayer lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill(TYVirtualScreenRect());
    [ visualLayer unlockFocus ];
    */
    compositeLayer = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];
    /*
    [ compositeLayer lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill(TYVirtualScreenRect());
    [ compositeLayer unlockFocus ];
    */
    bgImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];
    [ bgImage lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill(TYVirtualScreenRect());
    [ bgImage unlockFocus ];
    
    
    bgRect = NSMakeRect(0,0,VSCREEN_WIDTH,VSCREEN_HEIGHT);
    
    underline = 0;
    
    eraseTextWindow=YES;
    
    textLeftOffset=8;
    textTopOffset=16;
    textColumn=23;
    textRow=16;
    textFontWidth=26;
    textFontHeight=26;
    textPitchx=0;
    textPitchy=2;
    defaultSpeed=[ [ NSArray alloc ] initWithObjects:[ NSNumber numberWithInt:40 ],
                                                        [ NSNumber numberWithInt:20 ],
                                                        [ NSNumber numberWithInt:10 ],nil ];
    userTextSpeed=20;
    textSpeed=&userTextSpeed;
    textBold=YES;
    textShadowed=YES;
    if(textFontName)
        [ textFontName autorelease ];
    textFontName = [ [ TYEnviroment objectForKey:TYFontNameEnviroment ] retain ];
    /*
    textFontName = [ [ [ NSBundle mainBundle ] localizedStringForKey:@"DEFAULT_FONT_NAME"
                                        value:@"Osaka" table:nil ] retain ];
     */
    textFont = [ NSFont fontWithName:textFontName size:(textFontWidth <= textFontHeight) ? textFontWidth : textFontHeight ];
    textFontColor = [ NSColor whiteColor ];
    [ self makeAttributeDict ];
    textWindowRect = TYVirtualScreenRect();
    //textWindowPoint = NSZeroPoint;
    //textWindowSize = NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT);

    {
        NSString *str;
        
        str = [ [ NSBundle mainBundle ] localizedStringForKey:@"JAPANESE_HYPHENATION_CHARSET"
                                                        value:@""
                                                        table:nil ];
        if([ str length ]){
            hyphenationSet = [ [ NSCharacterSet characterSetWithCharactersInString:str ] retain ];
        }
    }
    // テキストウインドウとノベルレイヤーの初期化
    [ self loadTextWindow:DEFAULT_TEXTWINDOW ];
    novelLayer = [ [ TYNovelLayer alloc ] initNovelLayer ];
    [ (TYNovelLayer*)novelLayer setManager:self ];
    
    layoutManager = [ [ NSLayoutManager alloc ] init ];
    [ layoutManager addTextContainer:[ [ NSTextContainer alloc ] initWithContainerSize:NSZeroSize ] ];
    [ [ [ NSTextStorage alloc ] initWithString:@"" ] addLayoutManager:layoutManager ]; // dummy

    // 空のボタンマネージャを用意
    buttonManager = [ [ TYExtraButtonManager alloc ] initWithStageManager:self sourceImage:nil ];

    // スプライト配列の初期化
    spriteArray = [ [ NSMutableArray alloc ] initWithCapacity:1 ];
    humanz = 25;

    selectColor = [ [ NSColor colorWithCalibratedRed:1 green:1 blue:1 alpha:1 ] retain ];
    unSelectColor = [ [ NSColor colorWithCalibratedRed:(255 -99) / 255.0 green:(255 -99) / 255.0 blue:(255 -99) / 255.0 alpha:1 ] retain ];

    // カーソル初期設定
    clickWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:@":l/3,160,2;cursor0.bmp"
                                                            offset:NSMakePoint(0,0)
                                                        absolute:NO ];
    [ clickWaitCursor setDelegate:self ];
    pageWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:@":l/3,160,2;cursor1.bmp"
                                                        offset:NSMakePoint(0,0)
                                                        absolute:NO ];
    [ pageWaitCursor setDelegate:self ];
    
    
    [ [ NSNotificationCenter defaultCenter ] addObserver:self
                                                selector:@selector(updateSprites:)
                                                    name:TYSpriteAnimationNotification
                                                  object:nil ];
    
        // MEMO:CCScripterが二つ以上同時に起動してるとその通知も受け取ってしまう？
    // スレッド間でのみ受け付けたい場合には、objectにNSAppを設定してみるというのはどうか。
    [ [ NSDistributedNotificationCenter defaultCenter ] addObserver:self
                                                           selector:@selector(startWaveSound:)
                                                               name:TYSoundStartNotification
                                                             object:nil ];
    [ [ NSDistributedNotificationCenter defaultCenter ]  addObserver:self
                                                            selector:@selector(stopWaveSound:)
                                                                name:TYSoundFinishNotification
                                                              object:nil ];

    return self;
}

-(void)setController:(id)aController engine:(id)aEngine
{
    controller = aController;
    engine = aEngine;
}

-(id)encodeWithSaveData
{
    NSMutableDictionary *aDict;
    NSArray *array;
    
    aDict = [ NSMutableDictionary dictionary ];

    [ aDict setObject:novelLayer forKey:TYNovelLayerSaveData ];

    // TextWindow情報作成
    array = [ NSArray arrayWithObjects:[ NSNumber numberWithInt:textLeftOffset ],
        [ NSNumber numberWithInt:textTopOffset ],
        [ NSNumber numberWithInt:textColumn ],
        [ NSNumber numberWithInt:textRow ],
        [ NSNumber numberWithInt:textFontWidth ],
        [ NSNumber numberWithInt:textFontHeight ],
        [ NSNumber numberWithInt:textPitchx ],
        [ NSNumber numberWithInt:textPitchy ],
        [ NSNumber numberWithInt:userTextSpeed ],
        [ NSNumber numberWithBool:textBold ],
        [ NSNumber numberWithBool:textShadowed ],
        textWindowPath,
        [ NSValue valueWithPoint:textWindowRect.origin ],
        [ NSValue valueWithSize:textWindowRect.size ],nil ];
    [ aDict setObject:array forKey:TYTextWindowSaveData ];
    
    [ aDict setObject:[ NSNumber numberWithInt:((textSpeed==&userTextSpeed) ? TYUserSpeedMode : TYScriptSpeedMode) ]
            forKey:TYSpeedModeSaveData ];

    // 処理中の情報を記録
    array = [ NSArray arrayWithObjects:textQueue,
        [ NSNumber numberWithInt:nowStorageOffset ],
        [ NSNumber numberWithInt:nowRow ],
        [ NSNumber numberWithInt:nowColumn ],
        [ NSNumber numberWithBool:waitNewPage ],
        [ NSNumber numberWithBool:invalidNewLine ],
        nil ];
    [ aDict setObject:array forKey:TYPrintingInfoSaveData ];
    [ aDict setObject:[ NSNumber numberWithBool:isPage ] forKey:TYIspageSaveData ];


    [ aDict setObject:[ NSNumber numberWithBool:novelLayerOn ] forKey:TYNovelLayerVisibleSaveData ];

    [ aDict setObject:spriteArray forKey:TYSpritesSaveData ];

    if(bgPath)
        [ aDict setObject:bgPath forKey:TYBGImagePathSaveData ];
    if(leftCharImage)
        [ aDict setObject:leftCharImage forKey:TYLeftCharSaveData ];
    if(centerCharImage)
        [ aDict setObject:centerCharImage forKey:TYCenterCharSaveData ];
    if(rightCharImage)
        [ aDict setObject:rightCharImage forKey:TYRightCharSaveData ];
    [ aDict setObject:[ NSNumber numberWithInt:[ TYStandingChar underline ] ] forKey:TYUnderlineSaveData ];

    if(monocroSample)
        [ aDict setObject:monocroSample forKey:TYMonocroSampleSaveData ];

    if(nega)
        [ aDict setObject:[ NSNumber numberWithInt:nega ] forKey:TYNegaSpecifySaveData ];

    if(clickWaitCursor)
        [ aDict setObject:clickWaitCursor forKey:TYClickWaitCursorSaveData ];

    if(pageWaitCursor)
        [ aDict setObject:pageWaitCursor forKey:TYPageWaitCursorSaveData ];

    if(currentCursor && [ self status ] == TYTPClickWait){
        if(currentCursor == clickWaitCursor){
            [ aDict setObject:[ NSNumber numberWithInt:TYNormalClickWaitType ] forKey:TYClickWaitTypeSaveData ];
        } else if (currentCursor == pageWaitCursor){
            [ aDict setObject:[ NSNumber numberWithInt:TYPageClickWaitType ] forKey:TYClickWaitTypeSaveData ];
        }
    }

    if(cselInfo)
        [ aDict setObject:cselInfo forKey:TYCustomSelectInfoSaveData ];

    [ aDict setObject:[ NSNumber numberWithInt:eraseTextWindow ] 
                forKey:TYEraseTextWindowSavedata ];

    if(lookbackQueue)
        [ aDict setObject:lookbackQueue forKey:TYLookbackBufferSaveData ];
    
    return aDict;
}

-(void)decodeWithSaveData:(id)aObject
{
    NSArray *array;
    NSEnumerator *enumeratar;
    id temp;

    temp = [ aObject objectForKey:TYNovelLayerSaveData ];
    if(temp){
        if(novelLayer)
            [ novelLayer release ];
    
        novelLayer = [ [ aObject objectForKey:TYNovelLayerSaveData ] retain ];
        [ (TYNovelLayer*)novelLayer setManager:self ];
    }

    array = [ aObject objectForKey:TYTextWindowSaveData ];
    if(array){
        enumeratar = [ array objectEnumerator ];
        textLeftOffset = [ [ enumeratar nextObject ] intValue ];
        textTopOffset = [ [ enumeratar nextObject ] intValue ];
        textColumn = [ [ enumeratar nextObject ] intValue ];
        textRow = [ [ enumeratar nextObject ] intValue ];
        textFontWidth = [ [ enumeratar nextObject ] intValue ];
        textFontHeight = [ [ enumeratar nextObject ] intValue ];
        textPitchx = [ [ enumeratar nextObject ] intValue ];
        textPitchy = [ [ enumeratar nextObject ] intValue ];
        userTextSpeed = [ [ enumeratar nextObject ] intValue ];
        textBold = [ [ enumeratar nextObject ] boolValue ];
        textShadowed = [ [ enumeratar nextObject ] boolValue ];
        if(textWindowPath)
            [ textWindowPath release ];
        textWindowPath = [ [ enumeratar nextObject ] retain ];
        [ [ enumeratar nextObject ] getValue:&textWindowRect.origin ];
        [ [ enumeratar nextObject ] getValue:&textWindowRect.size ];
        [ self makeAttributeDict ];
    }
    
    temp = [ aObject objectForKey:TYSpeedModeSaveData ];
    if(temp && ([ temp intValue ]==TYScriptSpeedMode)){
        textSpeed=&scriptTextSpeed;
    } else {
        textSpeed=&userTextSpeed;
    }

    temp = [ aObject objectForKey:TYIspageSaveData ];
    if(temp)
        isPage = [ temp boolValue ];
    
    array = [ aObject objectForKey:TYPrintingInfoSaveData ];
    if(array){
        enumeratar = [ array objectEnumerator ];
        textQueue = [ [ enumeratar nextObject ] retain ];
        nowStorageLength = [ textQueue length ];
        nowStorageOffset = [ [ enumeratar nextObject ] intValue ];
        nowRow = [ [ enumeratar nextObject ] intValue ];
        nowColumn = [ [ enumeratar nextObject ] intValue ];
        waitNewPage = [ [ enumeratar nextObject ] boolValue ];
        invalidNewLine = [ [ enumeratar nextObject ] boolValue ];
        waitCountOfChar = nowRow * textColumn + nowColumn;
    }

    temp = [ aObject objectForKey:TYNovelLayerVisibleSaveData ];
    if(temp)
        novelLayerOn = [ temp boolValue ];
    
    // 画面の作り直し
    // テキストウインドウ
    {
        [ self loadTextWindow:textWindowPath ];
    }

    // いくつかのフラグを強制初期化
    forceWaitNewPage=NO;
    forceClickWait=NO;
    
    // まず背景、立ち絵を全て解放
    if(bgImage){
        [ bgImage release ];
        bgImage=nil;
    }
    if(bgPath){
        [ bgPath release ];
        bgPath=nil;
    }
    temp = [ [ aObject objectForKey:TYBGImagePathSaveData ] retain ];
    if(!temp){
        temp = bgPath = @"black";
    }
    [ self loadBGImage:temp ];
    
    if(leftCharImage)
        [ leftCharImage release ];
    leftCharImage=[ [ aObject objectForKey:TYLeftCharSaveData ] retain ];
    if(leftCharImage)
        [ leftCharImage loadImageFromPath ];
    
    if(centerCharImage)
        [ centerCharImage release ];
    centerCharImage = [ [ aObject objectForKey:TYCenterCharSaveData ] retain ];
    if(centerCharImage)
        [ centerCharImage loadImageFromPath ];
    
    if(rightCharImage)
        [ rightCharImage release ];
    rightCharImage = [ [ aObject objectForKey:TYRightCharSaveData ] retain ];
    if(rightCharImage)
        [ rightCharImage loadImageFromPath ];

    temp = [ aObject objectForKey:TYUnderlineSaveData ];
    if(temp){
        [ TYStandingChar setUnderline:[ temp intValue ] ];
    } else {
        [ TYStandingChar setUnderline:0 ];
    }

    if(spriteArray)
        [ spriteArray release ];
    temp = [ aObject objectForKey:TYSpritesSaveData ];
    if(temp){
        spriteArray = [ temp retain ];
        [ spriteArray makeObjectsPerformSelector:@selector(loadImageFromPath) ];
    } else {
        spriteArray = [ [ NSMutableArray array ] retain ];
    }

    [ barDict release ];
    barDict = nil;
    
    // スプライト、および立ち絵の透過度設定に関するバグに対する対処
    if([ [ aObject objectForKey:TYVersionSaveData ] intValue ] < TYSpriteAlphaFixSaveDataVersion){
        [ leftCharImage convertAlphaOfFix ];
        [ centerCharImage convertAlphaOfFix ];
        [ rightCharImage convertAlphaOfFix ];
        [ spriteArray makeObjectsPerformSelector:@selector(convertAlphaOfFix) ];
    }

    if(monocroSample){
        [ monocroSample release ];
        monocroSample = nil;
    }
    temp = [ aObject objectForKey:TYMonocroSampleSaveData ];
    if(temp){
        monocroSample = [ temp retain ];
    }

    nega = TYNoNegaPriority;
    temp = [ aObject objectForKey:TYNegaSpecifySaveData ];
    if(temp)
        nega = [ temp intValue ];
    
    temp = [ aObject objectForKey:TYEraseTextWindowSavedata ];
    if(temp)
        eraseTextWindow = [ temp intValue ];
    
    
    // オフスクリーンの再描画
    [ self updateStage ];

    // Viewにオフスクリーンを転送
    [ controller setImage:compositeLayer ];

    [ currentCursor stop ];
    currentCursor = nil;
    // カーソル処理
    temp = [ aObject objectForKey:TYClickWaitCursorSaveData ];
    if(temp) {
        [ clickWaitCursor release ];
        clickWaitCursor = [ temp retain ];
        [ clickWaitCursor setDelegate:self ];
    }
    temp = [ aObject objectForKey:TYPageWaitCursorSaveData ];
    if(temp) {
        [ pageWaitCursor release ];
        pageWaitCursor = [ temp retain ];
        [ pageWaitCursor setDelegate:self ];
    }
    // ここで（必要なら）カーソルの表示処理
    temp = [ aObject objectForKey:TYClickWaitTypeSaveData ];
    if(temp) {
        NSPoint point;
        if([ temp intValue ] == TYNormalClickWaitType){
            currentCursor = clickWaitCursor;
        } else if([ temp intValue ] == TYPageClickWaitType){
            currentCursor = pageWaitCursor;
        }
        point = [ self drawPoint ];
        //point.y += textFontHeight;
        [ currentCursor drawToView:controller point:point ];
    }
    
    temp = [ aObject objectForKey:TYCustomSelectInfoSaveData ];
    if(temp){
        cselInfo = [ temp retain ];
        [ self makeCselImage ]; 
    } else {
        if(cselInfo){
            [ cselInfo release ];
            cselInfo = nil;
        }
        if(cselImage){
            [ cselImage release ];
            cselImage = nil;
        }
    }
    
    if(lookbackQueue)
        [ lookbackQueue release ];
    temp = [ aObject objectForKey:TYLookbackBufferSaveData ];
    if(temp)
        lookbackQueue = [ temp retain ];
    else
        lookbackQueue = [ [ NSMutableArray array ] retain ];
    
    NSLog2(@"click_cur %@ , page_cur %@",clickWaitCursor,pageWaitCursor);
}

-(void)resetGame
{
    [ novelLayer release ];
    novelLayer = [ [ TYNovelLayer alloc ] initNovelLayer ];
    [ (TYNovelLayer*)novelLayer setManager:self ];

    [ visualLayer release ];
    visualLayer = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];

    [ compositeLayer release ];
    compositeLayer = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];

    [ bgImage release ];
    bgImage = [ [ NSImage alloc ] initWithSize:NSMakeSize(VSCREEN_WIDTH,VSCREEN_HEIGHT) ];
    [ bgImage lockFocus ];
    [ [ NSColor blackColor ] set ];
    NSRectFill(TYVirtualScreenRect());
    [ bgImage unlockFocus ];
    [ bgPath release ]; bgPath=nil;

    eraseTextWindow=YES;

    textLeftOffset=8;
    textTopOffset=16;
    textColumn=23;
    textRow=16;
    textFontWidth=26;
    textFontHeight=26;
    textPitchx=0;
    textPitchy=2;
    textSpeed=&userTextSpeed;
    scriptTextSpeed=20;
    textBold=YES;
    textShadowed=YES;
    textFontColor = [ NSColor whiteColor ];
    [ self makeAttributeDict ];
    [ textWindowPath release ]; textWindowPath=nil;
    textWindowRect = TYVirtualScreenRect();
    [ self loadTextWindow:DEFAULT_TEXTWINDOW ];

    [ textQueue release ]; textQueue=nil;
    nowStorageLength = 0;
    nowStorageOffset = 0;
    nowRow = 0;
    nowColumn = 0;
    waitNewPage = NO;
    invalidNewLine = NO;

    novelLayerOn = NO;

    forceWaitNewPage=NO;
    forceClickWait=NO;    
    
    [ spriteArray release ];
    spriteArray = [ [ NSMutableArray alloc ] initWithCapacity:1 ];

    [ barDict release ];
    barDict = nil;
    
    [ leftCharImage release ]; leftCharImage=nil;
    [ centerCharImage release ]; centerCharImage=nil;
    [ rightCharImage release ]; rightCharImage=nil;

    [ monocroSample release ]; monocroSample=nil;

    lookbackQueue = [ [ NSMutableArray array ] retain ];

    nega = TYNoNegaPriority;

    [ self updateStage ];

    [ controller setImage:compositeLayer ];

    [ currentCursor stop ];
    currentCursor = nil;
    // カーソル初期設定
    [ clickWaitCursor release ];
    clickWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:@":l/3,160,2;cursor0.bmp"
                                                         offset:NSMakePoint(0,0)
                                                       absolute:NO ];
    [ clickWaitCursor setDelegate:self ];
    [ pageWaitCursor release ];
    pageWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:@":l/3,160,2;cursor1.bmp"
                                                        offset:NSMakePoint(0,0)
                                                      absolute:NO ];
    [ pageWaitCursor setDelegate:self ];    
}

-(void)ty_effect:(NSMutableArray *)argments
{
    int count;
    NSNumber *effectNo=nil,*effectType=nil,*time=nil;
    NSString *path=nil;
    
    count = [ argments count ];
    
    switch(count){
    case 5:
        path = [ engine getStringOfArgment:[ argments objectAtIndex:4 ] ];
		path = winPathToUnix([ [ path componentsSeparatedByString:@";" ] lastObject ]);
    case 4:
        time = [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ];
    case 3:
        effectType = [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ];
        effectNo = [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ];
    }

    [ effectDefDict setObject:[ TYEffectDefinitionValue valueWithEffectDefinition:
        TYMakeEffectDefinition([ effectType intValue ],[ time intValue ],path) ]
        forKey:effectNo ];
}

-(void)ty_windoweffect:(NSMutableArray *)argments
{
    [ argments insertObject:[ NSNumber numberWithInt:WINDOW_EFFECT_NO ] atIndex:1 ];
    [ self performSelector:@selector(ty_effect:) withObject:argments ];
}

-(void)ty_setwindow:(NSMutableArray *)argments
{
    nowRow=0;
    nowColumn=0;

    textLeftOffset = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    textTopOffset = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    textColumn = [ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    textRow = [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];
    textFontWidth = [ [ engine getValueOfArgment:[ argments objectAtIndex:5 ] ] intValue ];
    textFontHeight = [ [ engine getValueOfArgment:[ argments objectAtIndex:6 ] ] intValue ];
    textPitchx = [ [ engine getValueOfArgment:[ argments objectAtIndex:7 ] ] intValue ];
    textPitchy = [ [ engine getValueOfArgment:[ argments objectAtIndex:8 ] ] intValue ];
    scriptTextSpeed = [ [ engine getValueOfArgment:[ argments objectAtIndex:9 ] ] intValue ];
    textSpeed = &userTextSpeed;
    textBold = ([ [ engine getValueOfArgment:[ argments objectAtIndex:10 ] ] intValue ]) ? YES : NO;
    textShadowed= ([ [ engine getValueOfArgment:[ argments objectAtIndex:11 ] ] intValue ]) ? YES : NO;
    [ self makeAttributeDict ];

    // テキストウインドウの作成
    [ self loadTextWindow:[ engine getStringOfArgment:[ argments objectAtIndex:12 ] ] ];
    /*
    textWindowPath = [ engine getStringOfArgment:[ argments objectAtIndex:12 ] ];
    [ textWindowPath retain ];
     */
    if([ argments count ] == 17){
        int leftTopX=[ [ engine getValueOfArgment:[ argments objectAtIndex:13 ] ] intValue ];
        int leftTopY=[ [ engine getValueOfArgment:[ argments objectAtIndex:14 ] ] intValue ];
        int rightBottomX=[ [ engine getValueOfArgment:[ argments objectAtIndex:15 ] ] intValue ] +1;
        int rightBottomY=[ [ engine getValueOfArgment:[ argments objectAtIndex:16 ] ] intValue ] +1;
        // 色指定
        textWindowRect.origin.x = leftTopX;
        textWindowRect.origin.y = VSCREEN_HEIGHT -rightBottomY;
        textWindowRect.size.height = rightBottomY -leftTopY;
        textWindowRect.size.width = rightBottomX -leftTopX;
    } else {
        if(textWindow){
            textWindowRect.size = [ textWindow size ];
            textWindowRect.origin.x = [ [ engine getValueOfArgment:[ argments objectAtIndex:13 ] ] intValue ];
            textWindowRect.origin.y = VSCREEN_HEIGHT -[ [ engine getValueOfArgment:[ argments objectAtIndex:14 ] ] intValue ] -textWindowRect.size.height;
            
        }
    }

    [ self flushNovelLayer ];

    // 回想バッファの解放
    [ self ty_lookbackflush:nil ];
}

-(void)loadTextWindow:(NSString*)path
{    
    if(textWindowPath)
        [ textWindowPath autorelease ];
    textWindowPath = [ path retain ];

    [ textWindow release ];

    if([ [ textWindowPath substringToIndex:1 ] isEqualToString:@"#" ]){
        // 色指定
        textWindow = getTextWindowColorWithHTMLFormat(textWindowPath);
        [ textWindow retain ];
    } else {
        textWindow = [ [ TYResourceServer sharedServer ] getImage:path transMode:YES ];
        [ textWindow retain ];
    }

}

-(void)ty_erasetextwindow:(NSMutableArray *)argments
{
    eraseTextWindow = ([ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ]) ? YES : NO;
}

-(void)ty_textoff:(NSMutableArray *)argments
{
    novelLayerOn=NO;
}

-(void)ty_texton:(NSMutableArray *)argments
{
    novelLayerOn=YES;
}

-(void)ty_bg:(NSMutableArray *)argments
{
    //NSColor *color=nil;
    NSString *aPath;
    TYArgment *arg;
    int effectNo;
    
    effectNo = [ [ engine getEffectNoOfArgments:[ NSMutableArray arrayWithArray:
        [ argments subarrayWithRange:NSMakeRange(2,[ argments count]-2) ] ] ] intValue ];
    //aPath = [ engine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    arg = [ argments objectAtIndex:1 ];
    // path のタイプを調べる
    if([ arg argType ] != TYAliasArgType){
        aPath = [ engine getStringOfArgment:arg ];
    } else if((!([ (NSString*)arg compare:@"black" options:NSCaseInsensitiveSearch ] == NSOrderedSame ))&&
              (!([ (NSString*)arg compare:@"white" options:NSCaseInsensitiveSearch ] == NSOrderedSame)) ){
        aPath = [ engine getStringOfArgment:arg ];
    } else {
        aPath = arg;
    }
    [ self loadBGImage:aPath ];
    
    // ドキュメントにはない仕様ですが、立ち絵が全てクリアされます。
    [ leftCharImage release ];
    leftCharImage = nil;
    [ centerCharImage release ];
    centerCharImage = nil;
    [ rightCharImage release ];
    rightCharImage = nil;
    
    [ self changeVisualLayer:[ NSNumber numberWithInt:effectNo ] ];
}

-(void)loadBGImage:(NSString*)aPath
{
    NSColor *color=nil;

    // path のタイプを調べる
    if( [ [ aPath substringToIndex:1 ] isEqualToString:@"#" ]){
        color = getColorWithHTMLFormat(aPath);
    } else if ([ aPath compare:@"black" options:NSCaseInsensitiveSearch ] == NSOrderedSame ) {
        color = [ NSColor blackColor ];
    } else if ([ aPath compare:@"white" options:NSCaseInsensitiveSearch ] == NSOrderedSame) {
        color = [ NSColor whiteColor ];
    } else {
        //aPath = [ engine getStringOfArgment:aPath ];
        if(bgImage)
            [ bgImage autorelease ];
        bgImage = [ [ [ TYResourceServer sharedServer ] getImage:aPath transMode:NO ] retain ];
    }

    if(bgPath)
        [ bgPath release ];
    bgPath = [ aPath retain ];

    if(color){
        if(bgImage)
            [ bgImage autorelease ];
        bgImage = [ [ NSImage alloc ] initWithSize:bgRect.size ];
        [ bgImage lockFocus ];
        [ color set ];
        NSRectFill( NSMakeRect(0,0,VSCREEN_WIDTH,VSCREEN_HEIGHT) );
        [ bgImage unlockFocus ];
    }
}

-(void)ty_transmode:(NSMutableArray *)argments
{
    NSString *symbol = TYTransmodeSymbolFromString([ argments objectAtIndex:1 ]);

    [ [ TYResourceServer sharedServer ] setDefaultTransMode:symbol ];
}

-(void)ty_underline:(NSMutableArray *)argments
{
    [ TYStandingChar setUnderline:VSCREEN_HEIGHT -[ [engine getValueOfArgment:[ argments objectAtIndex:1 ]] intValue ]-1 ] ;
}

-(void)ty_ld:(NSMutableArray *)argments
{
    NSString *locate;
    id *targetImage;
    NSString *path;
    int effectNo;

    locate = [ [ argments objectAtIndex:1 ] lowercaseString ];
    path = [ engine getStringOfArgment:[ argments objectAtIndex:2 ] ];
    
    effectNo = [ [ engine getEffectNoOfArgments:[ NSMutableArray arrayWithArray:
        [ argments subarrayWithRange:NSMakeRange(3,[ argments count]-3) ] ] ] intValue ];

    //loadImage = [ [ TYResourceServer sharedServer ] getImage:path transMode:YES ];
    
    if([ locate isEqualToString:LD_LOCATE_LEFT ]){
        targetImage = &leftCharImage;
    } else if ([ locate isEqualToString:LD_LOCATE_CENTER ]){
        targetImage = &centerCharImage;
    } else if ([ locate isEqualToString:LD_LOCATE_RIGHT ]){
        targetImage = &rightCharImage;
    } else {
        // 例外をあげるべき
        return;
    }

    if(*targetImage)
        [ *targetImage autorelease ];
    *targetImage = [ [ TYStandingChar alloc ] initWithPosition:locate path:path ];

    [ self changeVisualLayer:[ NSNumber numberWithInt:effectNo ] ];
}

-(void)ty_cl:(NSMutableArray *)argments
{
    NSString *locate;
    id *targetImage;
    int effectNo;
    
    locate = [ [ argments objectAtIndex:1 ] lowercaseString ];
    effectNo = [ [ engine getEffectNoOfArgments:[ [ argments subarrayWithRange:
                    NSMakeRange(2,[argments count]-2) ] mutableCopy ] ] intValue ];
    
    if([ locate isEqualToString:LD_LOCATE_LEFT ]){
        targetImage = &leftCharImage;
    } else if ([ locate isEqualToString:LD_LOCATE_CENTER ]){
        targetImage = &centerCharImage;
    } else if ([ locate isEqualToString:LD_LOCATE_RIGHT ]){
        targetImage = &rightCharImage;
    } else if ([ locate isEqualToString:CL_LOCATE_ALL ]){
        [ leftCharImage autorelease ];
        leftCharImage = nil;
        [ centerCharImage autorelease ];
        centerCharImage = nil;
        targetImage = &rightCharImage;
    } else {
        // 例外をあげるべき
        return;
    }
    
    if(*targetImage){
        [ *targetImage autorelease ];
        *targetImage = nil;
    }


    [ self changeVisualLayer:[ NSNumber numberWithInt:effectNo ] ];
}

-(void)ty_humanz:(NSMutableArray *)argments
{
    isUseHumanz = TRUE;
    humanz = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
}

-(void)ty_lsp:(NSMutableArray *)argments
{
    [ self addSprite:argments visible:YES ];
}

-(void)ty_lsph:(NSMutableArray *)argments
{
    [ self addSprite:argments visible:NO ];
}

-(void)addSprite:(NSMutableArray *)argments visible:(BOOL)aBool;
{
    TYNScrSprite *sprite;
    int idNo;
    NSNumber *alphaNum;
    
    NSEnumerator *spriteEnumerator;
    id temp;
    int index;

    idNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    if([ argments count ] >= 6){
        // 透過度指定あり
        alphaNum = [ engine getValueOfArgment:[ argments objectAtIndex:5 ] ];
    } else {
        // 透過度指定なし
        alphaNum = [ NSNumber numberWithInt:SPRITE_ALPHA_MAX ];
    }

    sprite = [ [ TYNScrSprite alloc ] initWithID:idNo
                                            path:[ engine getStringOfArgment:[ argments objectAtIndex:2 ] ]
                                           point:NSMakePoint([ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ],[ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ])
                                           alpha:alphaNum
                                         visible:aBool ];
     [ sprite autorelease ];

    // 作成したスプライトを配列の適切な位置に挿入する。
    spriteEnumerator = [ spriteArray objectEnumerator ];
    index = 0;
    while( (temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo >= [ temp spriteID ]){
            break;
        }
        index++;
    }
    if((temp)&&(idNo == [ temp spriteID ]))
        [ spriteArray removeObjectAtIndex:index ];

    [ spriteArray insertObject:sprite atIndex:index ];
 
}

-(void)ty_csp:(NSMutableArray *)argments
{
    int idNo;
    NSEnumerator *spriteEnumerator;
    int index;
    id temp;
    
    idNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    
    if(idNo == -1){
        [ spriteArray release ];
        spriteArray = [ [ NSMutableArray alloc ] initWithCapacity:SPRITE_MAX ];
    } else {
        spriteEnumerator = [ spriteArray objectEnumerator ];
        index = 0;
        while( ( temp = [ spriteEnumerator nextObject ]) != nil){
            if(idNo == [ temp spriteID ]){
                [ spriteArray removeObjectAtIndex:index ];
                break;
            }
            index++;
        }
    }
}

-(void)ty_vsp:(NSMutableArray *)argments
{
    BOOL visible;
    NSEnumerator *spriteEnumerator;
    int index,idNo;
    TYNScrSprite *temp;

    idNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    visible = ([ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ]) ? YES : NO ;
    
    spriteEnumerator = [ spriteArray objectEnumerator ];
    index = 0;
    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            [ temp setVisible:visible ];
            [ spriteArray replaceObjectAtIndex:index withObject:temp ];
            break;
        }
        index++;
    }
}

-(void)ty_msp:(NSMutableArray *)argments
{
    NSEnumerator *spriteEnumerator;
    int index,idNo;
    TYNScrSprite *temp;
        
    idNo = [ [ argments objectAtIndex:1 ] intValue ];
    
    spriteEnumerator = [ spriteArray objectEnumerator ];
    index = 0;
    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            break;
        }
        index++;
    }
    
    if(temp == nil){
        // MEMO:本来、例外（エラー？）が発生するはず。
        return ;
    }
    
    [ temp moveX:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
                Y:[ engine getValueOfArgment:[ argments objectAtIndex:3 ] ]
                                   alpha:([ argments count ] >4) ? [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] : nil  ];
    
    [ spriteArray replaceObjectAtIndex:index withObject:temp ];
}

-(void)ty_amsp:(NSMutableArray *)argments
{
    NSEnumerator *spriteEnumerator;
    int index,idNo;
    TYNScrSprite *temp;

    idNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];

    spriteEnumerator = [ spriteArray objectEnumerator ];
    index = 0;
    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            break;
        }
        index++;
    }

    if(temp == nil){
        // MEMO:本来、例外（エラー？）が発生するはず。
        return ;
    }
    [ [ temp retain ] autorelease ];

    [ temp absoluteMoveX:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
               Y:[ engine getValueOfArgment:[ argments objectAtIndex:3 ] ]
                   alpha:([ argments count ] >4) ? [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] : nil ];

    [ spriteArray removeObjectAtIndex:index ];
    [ spriteArray insertObject:temp atIndex:index ];
}

-(void)ty_cell:(NSMutableArray *)argments
{
    [ self setSprite:[ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ]
                cell:[ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ] -1 ];
}

-(void)ty_allsphide:(NSMutableArray *)argments
{
    NSEnumerator *enu;
    TYNScrSprite *temp;
    
    if(hideSpriteArray)
        [ hideSpriteArray release ];
    
    hideSpriteArray = [ [ NSMutableArray array ] retain ];
    
    enu = [ spriteArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        if([ temp visible ]){
            [ hideSpriteArray addObject:[ NSNumber numberWithInt:[ temp spriteID ] ] ];
            [ temp setVisible:NO ];
        }
    }
}

-(void)ty_allspresume:(NSMutableArray *)argments
{
    NSEnumerator *enu;
    NSNumber *temp;
    
    enu = [ hideSpriteArray objectEnumerator ];
    while(temp = [ enu nextObject ]){
        // MEMO:最適化する余地あり
        [ self setSprite:[ temp intValue ] visible:YES ]; 
    }
    
    [ hideSpriteArray release ];
    hideSpriteArray = nil;
}

-(NSRect)rectOfSprite:(int)idNo
{
    TYNScrSprite *temp;
    NSEnumerator *spriteEnumerator=[ spriteArray objectEnumerator ];
    
    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            break;
        }
    }

    return temp ? [ temp rect ] : NSZeroRect;
}

-(void)ty_spstr:(NSMutableArray *)argments
{
    NSScanner* scanner;
    NSCharacterSet *set = [ NSCharacterSet characterSetWithCharactersInString:@"PCS" ];
    NSCharacterSet *digitSet = [ NSCharacterSet decimalDigitCharacterSet ];
    NSString *strOp,*strId,*strCell;
    
    scanner = [ NSScanner scannerWithString:[ engine getStringOfArgment:[ argments objectAtIndex:1 ] ] ];

    while([ scanner scanCharactersFromSet:set intoString:&strOp ]){
        [ scanner scanCharactersFromSet:digitSet intoString:&strId ];
        if([ strOp isEqualToString:@"C" ]){
            [ self setSprite:[ strId intValue ] visible:NO ];
        } else if([ strOp isEqualToString:@"P" ]){
            if([ scanner scanString:@"," intoString:NULL ]){
                [ scanner scanCharactersFromSet:digitSet intoString:&strCell ];
                [ self setSprite:[ strId intValue ] cell:[ strCell intValue ] ];
            } else {
                [ self setSprite:[ strId intValue ] visible:YES ];                
            }
        } else if([ strOp isEqualToString:@"S" ]) {
            NSString *fileName;
            [ scanner scanString:@",(" intoString:NULL ];
            [ scanner scanUpToString:@")" intoString:&fileName ];
            
            [ controller ty_dwave:
                [ NSMutableArray arrayWithObjects:
                    @"dwave",[ NSNumber numberWithInt:[ strId intValue ] ],
                    [ TYConstStringArgment argmentWithString:fileName ],nil ] ];
            
            [ scanner scanString:@")" intoString:NULL ];
        }
    }
    
}

-(void)setSprite:(int)idNo visible:(BOOL)aBool
{
    TYNScrSprite *temp;
    NSEnumerator *spriteEnumerator=[ spriteArray objectEnumerator ];

    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            break;
        }
    }

    return [ temp setVisible:aBool ];
}

-(void)setSprite:(int)idNo cell:(int)cellNo
{
    TYNScrSprite *temp;
    NSEnumerator *spriteEnumerator=[ spriteArray objectEnumerator ];

    while( ( temp = [ spriteEnumerator nextObject ]) != nil){
        if(idNo == [ temp spriteID ]){
            break;
        }
    }

    return [ temp setCell:cellNo ];
}

-(void)ty_bar:(NSMutableArray *)argments
{
    TYBar *bar;
    NSNumber *barNumber;
    int now,x,y,width,height,max;
    NSColor *color;
    
    if(!barDict)
        barDict = [ [ NSMutableDictionary dictionary ] retain ];

    barNumber = [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ];
    now = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    x = [ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    y = [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];
    width = [ [ engine getValueOfArgment:[ argments objectAtIndex:5 ] ] intValue ];
    height = [ [ engine getValueOfArgment:[ argments objectAtIndex:6 ] ] intValue ];
    max = [ [ engine getValueOfArgment:[ argments objectAtIndex:7 ] ] intValue ];
    color = getColorWithHTMLFormat([ engine getStringOfArgment:[ argments objectAtIndex:8 ] ]);
    
    y = VSCREEN_HEIGHT -height -y;
    width *= max / now;
    
    bar = [ [ TYBar alloc ] initWithRect:NSMakeRect(x,y,width,height) color:color ];
    [ bar autorelease ];
    
    [ barDict setObject:bar forKey:barNumber ];
}

-(void)ty_barclear:(NSMutableArray *)argments
{
    [ barDict release ];
    barDict = nil;
}

-(void)ty_monocro:(NSMutableArray *)argments
{
    unsigned char sample[4];
    NSString *arg;
    int r,g,b;
    
    arg = [ argments objectAtIndex:1 ];
    
    if([ arg isEqualToString:@"off" ]){
        if(monocroSample) {
            [ monocroSample release ];
            monocroSample = nil;
        }
    } else {
        getRGBWithHTMLFormat([ argments objectAtIndex:1 ],&r,&g,&b);
        sample[0] = r;
        sample[1] = g;
        sample[2] = b;
        if(monocroSample)
            [ monocroSample release ];
        monocroSample = [ [ NSData dataWithBytes:sample length:3 ] retain ];
    }
}

-(void)ty_nega:(NSMutableArray *)argments
{
    nega = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
}

-(void)ty_print:(NSMutableArray *)argments
{
    [ self changeVisualLayer:[ engine getEffectNoOfArgments:[ NSMutableArray arrayWithArray:
        [ argments subarrayWithRange:NSMakeRange(1,[ argments count]-1) ] ] ] ];
}

-(void)ty_repaint:(NSMutableArray *)argments
{
    [ engine eval:[ NSMutableArray arrayWithObjects:@"print",[ NSNumber numberWithInt:1 ],nil ] ];
}

-(void)ty_btndef:(NSMutableArray *)argments
{
    NSImage *image;
    
    [ btnTime release ];
    btnTime = nil;
    [ btnStartDate release ];
    btnStartDate = nil;
    
    if([ [ argments objectAtIndex: 1] argType ] == TYAliasArgType && 
       [ [ argments objectAtIndex:1 ] isEqualToString:@"clear" ]){
        [ buttonManager clear ];
    } else {
        [ buttonManager release ];
        
        image = [ [ TYResourceServer sharedServer ] getImage:[ engine getStringOfArgment:[ argments objectAtIndex:1 ] ] transMode:NO ];

        buttonManager = [ [ TYExtraButtonManager alloc ] initWithStageManager:self sourceImage:image ];
    }
}

-(void)ty_btntime:(NSMutableArray *)argments
{
    [ btnTime release ];
    btnTime = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] retain ];
}

-(void)ty_btntime2:(NSMutableArray *)argments
{
    if (playingSoundFlg == YES) {
        waitFinishSoundOnBtnTime2 = YES;
        [ btnTime release ];
        btnTime = nil;
    } else {
        [ self ty_btntime:argments ];
    }
}

-(void)ty_btn:(NSMutableArray *)argments
{
    [ buttonManager addButton:[ engine getValueOfArgment:[ argments objectAtIndex:1 ] ]
                        x:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] 
                        y:[ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] 
                        width:[ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] 
                        height:[ engine getValueOfArgment:[ argments objectAtIndex:5 ] ] 
                        selx:[ engine getValueOfArgment:[ argments objectAtIndex:6 ] ] 
                        sely:[ engine getValueOfArgment:[ argments objectAtIndex:7 ] ] ];

}

-(void)ty_spbtn:(NSMutableArray *)argments
{
    [ buttonManager addSpriteButton:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
                           spriteNo:[ engine getValueOfArgment:[ argments objectAtIndex:1 ] ]
                       appendAction:nil ];
}

-(void)ty_cellcheckspbtn:(NSMutableArray *)argments
{
    NSEnumerator *enu;
    id temp;
    BOOL isCell=NO;
    int idNo;
    
    enu = [ spriteArray objectEnumerator ];
    
    idNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    
    while(temp = [ enu nextObject ]){
        if([ temp spriteID ] == idNo) {
            isCell = [ [ temp sourceImage ] isCellImage ];
            break;
        }
    }

    if(!isCell)
        return ;
        
    [ buttonManager addSpriteButton:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
                           spriteNo:[ engine getValueOfArgment:[ argments objectAtIndex:1 ] ]
                       appendAction:nil ];
}

-(void)ty_exbtn:(NSMutableArray *)argments
{
    [ buttonManager addSpriteButton:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
                           spriteNo:[ engine getValueOfArgment:[ argments objectAtIndex:1 ] ]
                       appendAction:[ TYConstStringArgment argmentWithString:[ engine getStringOfArgment:[ argments objectAtIndex:3 ] ] ] ];
}

-(void)ty_exbtn_d:(NSMutableArray *)argments
{
    [ buttonManager setNoSelectAction:[ TYConstStringArgment argmentWithString:[ engine getStringOfArgment:[ argments objectAtIndex:1 ] ] ] ];
}

-(void)ty_btnwait:(NSMutableArray *)argments
{
    [ self startButtonWait:argments releaseAfterSelect:YES ];
}

-(void)ty_btnwait2:(NSMutableArray*)argments
{
    [ self startButtonWait:argments releaseAfterSelect:NO ];
}

-(void)ty_textbtnwait:(NSMutableArray*)argments
{
    novelLayerOn = YES;
    [ self startButtonWait:argments releaseAfterSelect:YES ];
}

-(void)ty_selectbtnwait:(NSMutableArray*)argments
{
    novelLayerOn = YES;
    [ self startButtonWait:argments releaseAfterSelect:YES ];
}

-(void)startButtonWait:(NSMutableArray*)argments releaseAfterSelect:(BOOL)aBool
{
    releaseButtonAfterSelect = aBool;
    isDrawButton = YES;

    [ buttonTargetValue release ];
    buttonTargetValue = [ argments objectAtIndex:1 ];
    [ buttonTargetValue retain ];

    [ buttonManager initialButtonImage ];
    [ controller setImage:compositeLayer ];

    beforeSelectButtonID = 0;
    [ controller setStatus:TYButtonWaitStatus ];
    [ engine setBreakRun:YES ];
    
    if(btnTime) {
        [ btnStartDate release ];
        btnStartDate = [ [ NSDate date ] retain ];
        [ self performSelector:@selector(other_action:) 
                    withObject:[ NSNumber numberWithInt:-5 ]
                    afterDelay:[ btnTime intValue ] / 1000.0 ];
    }
}

-(void)ty_getbtntimer:(NSMutableArray*)argments
{
    int time;
    
    time = [ btnStartDate timeIntervalSinceNow ];
    
    [ argments addObject:[ NSNumber numberWithInt:time * 1000 ] ];
    
    [ engine ty_mov:argments ];
}

-(void)ty_gettext:(NSMutableArray*)argments
{
    [ argments addObject:[ (TYNovelLayer*)novelLayer getText ] ];
    [ engine ty_mov:argments ];
}

-(void)ty_blt:(NSMutableArray*)argments
{
    NSImage *aImage;
    int dx,dy,dw,dh;
    int bx,by,bw,bh;

    aImage = [ buttonManager sourceImage ];
    dx = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    dy = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
    dw = [ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    dh = [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];
    bx = [ [ engine getValueOfArgment:[ argments objectAtIndex:5 ] ] intValue ];
    by = [ [ engine getValueOfArgment:[ argments objectAtIndex:6 ] ] intValue ];
    bw = [ [ engine getValueOfArgment:[ argments objectAtIndex:7 ] ] intValue ];
    bh = [ [ engine getValueOfArgment:[ argments objectAtIndex:8 ] ] intValue ];

    // y座標変換
    dy = VSCREEN_HEIGHT -dy -dh;
    by = [ aImage size ].height -by -bh;

    [ controller directDrawImage:aImage inRect:NSMakeRect(dx,dy,dw,dh) fromRect:NSMakeRect(bx,by,bw,bh) ];
    [ engine setBreakRun:YES ];
    [ engine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
}

-(void)ty_ofscpy:(NSMutableArray *)argments
{
    NSImage *image;

    image = [ controller makeImageFromMainView ];

    if(!image){
        NSLog(@"can't make image from MainView ");
        return;
    }

    [ controller setImage:image ];
}


-(void)ty_selectcolor:(NSMutableArray *)argments
{
    if(selectColor)
        [ selectColor release ];
    selectColor = getColorWithHTMLFormat([ argments objectAtIndex:1 ]);
    [ selectColor retain ];
    
    if(unSelectColor)
        [ unSelectColor release ];
    unSelectColor = getColorWithHTMLFormat([ argments objectAtIndex:2 ]);
    [ unSelectColor retain ];
}

-(void)ty_csel:(NSMutableArray *)argments
{
    NSEnumerator *enm;
    id temp;
    NSString *selStr;
    
    if(cselInfo)
        [ cselInfo release ];
    cselInfo = [ [ NSMutableArray arrayWithCapacity:4 ] retain ];

    enm = [ argments objectEnumerator ];
    [ enm nextObject ];
    while( (temp = [ enm nextObject ]) != nil){
        selStr = [ engine getStringOfArgment:temp ];
        if((!selStr)||(![ selStr length ])){
            // 空文字列ならスキップ
            [ enm nextObject ];
        } else {
            [ cselInfo addObject:selStr ];
            [ cselInfo addObject:[ NSMutableArray arrayWithObjects:@"goto",[ enm nextObject ],nil ] ];
        }
    }

    [ self makeCselImage ];
    
    [ engine ty_goto:[ NSMutableArray arrayWithObjects:@"goto",CSEL_LABEL,nil ] ];
}

-(void)makeCselImage
{
    NSArray *strImage;
    NSString *selStr;
    NSEnumerator *enu;

    if(cselImage)
        [ cselImage release ];
    cselImage = [ [ NSMutableArray arrayWithCapacity:[  cselInfo count ] /2 ] retain ];

    enu = [ cselInfo objectEnumerator ];
    while(selStr = [ enu nextObject ]){
        strImage = [ TYStringImage imagesWithString:selStr
                                         attributes:textAttDict
                                             colors:[ NSArray arrayWithObjects:unSelectColor,selectColor,nil ]
                                         fontHeight:textFontHeight
                                          fontWidth:textFontWidth
                                           interval:textPitchx
                                             shadow:textShadowed ];
        [ cselImage addObject:[ [ [ TYCellImage alloc ] initWithImages:strImage ] autorelease ] ];
        [ enu nextObject ];
    }
}

-(void)ty_getcselnum:(NSMutableArray *)argments
{
    [ engine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",[ argments objectAtIndex:1 ],
        [ NSNumber numberWithInt:[ cselInfo count ] /2 ],nil ] ];
}

-(void)ty_cselbtn:(NSMutableArray *)argments
{
    NSNumber *sNo = [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ];
    NSPoint point = NSMakePoint([ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ],
                                [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ]);
    
    [ buttonManager addCselButton:[ engine getValueOfArgment:[ argments objectAtIndex:2 ] ]
                      selectionNo:sNo
                            image:[ cselImage objectAtIndex:[ sNo intValue ] ]
                            point:point ];
}

-(void)ty_cselgoto:(NSMutableArray *)argments
{
    int sNo = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];

    [ engine eval:[ cselInfo objectAtIndex:sNo *2 +1 ] ];
    [ self ty_textclear:nil ];
    
    [ cselInfo release ];
    cselInfo = nil;
    [ cselImage release ];
    cselImage = nil;
}

-(void)ty_select:(NSMutableArray *)argments
{
    NSMutableArray *respondArgments;
    NSEnumerator *enm;
    id temp;
    NSString *selStr;
    NSMutableArray *selStrArray;
    
    selStrArray = [ NSMutableArray arrayWithCapacity:1 ];
    respondArgments = [ NSMutableArray arrayWithCapacity:1 ];

    enm = [ argments objectEnumerator ];
    [ enm nextObject ];
    while( (temp = [ enm nextObject ]) != nil){
        selStr = [ engine getStringOfArgment:temp ];
        if((!selStr)||(![ selStr length ])){
            // 空文字列ならスキップ
            [ enm nextObject ];
        } else {
            [ selStrArray addObject:selStr ];
            [ respondArgments addObject:[ NSMutableArray arrayWithObjects:@"goto",[ enm nextObject ],nil ] ];
        }
    }
    
    [ self startSelect:selStrArray responds:respondArgments ];
}

-(void)ty_selgosub:(NSMutableArray *)argments
{
    NSMutableArray *respondArgments;
    NSEnumerator *enm;
    id temp;
    NSString *selStr;
    NSMutableArray *selStrArray;
    
    selStrArray = [ NSMutableArray arrayWithCapacity:1 ];
    respondArgments = [ NSMutableArray arrayWithCapacity:1 ];

    enm = [ argments objectEnumerator ];
    [ enm nextObject ];
    while( (temp = [ enm nextObject ]) != nil){
        selStr = [ engine getStringOfArgment:temp ];
        if((!selStr)||(![ selStr length ])){
            // 空文字列ならスキップ
            [ enm nextObject ];
        } else {
            [ selStrArray addObject:selStr ];
            [ respondArgments addObject:[ NSMutableArray arrayWithObjects:@"gosub",[ enm nextObject ],nil ] ];
        }
    }
    
    //[ engine eval:[ NSMutableArray arrayWithObjects:@"skip",@"-1",nil ] ]; // われながらこれはあんまりだと思う。
    [ self startSelect:selStrArray responds:respondArgments ];
}

-(void)ty_selnum:(NSMutableArray *)argments
{
    NSMutableArray *respondArgments;
    NSEnumerator *enm;
    id temp;
    NSString *targetVar;
    int value;
    NSString *selStr;
    NSMutableArray *selStrArray;
    
    selStrArray = [ NSMutableArray arrayWithCapacity:1 ];
    respondArgments = [ NSMutableArray arrayWithCapacity:1 ];

    enm = [ argments objectEnumerator ];
    [ enm nextObject ];
    targetVar = [ enm nextObject ];
    value = 0;
    while( (temp = [ enm nextObject ]) != nil){
        selStr = [ engine getStringOfArgment:temp ];
        if((selStr)&&([ selStr length ])) {
            [ selStrArray addObject:selStr ];

            [ respondArgments addObject:[ NSMutableArray arrayWithObjects:@"mov",targetVar,[ NSNumber numberWithInt:value ],nil ] ];
        }
        value++;
    }

    [ self startSelect:selStrArray responds:respondArgments ];
}

-(void)startSelect:(NSArray*)messages responds:(NSArray*)respondArray
{
    NSEnumerator *msgEnumerator;
    NSString *tempStr;
    NSImage *unselImage,*selImage;
    NSArray *imageArray;
    TYCellImage *cellImage;
    int len;
    int startRow=nowRow;

    // テキストウインドウが表示されていなければ再描画
    if(!novelLayerOn){
        novelLayerOn = YES ;
        [ self updateStage ];
    }
    
    selectAction = [ respondArray retain ];

    selectionManager = [ [ TYSelectionManager alloc ] initWithStageManager:self ];

    msgEnumerator = [ messages objectEnumerator ];
    while( (tempStr = [ msgEnumerator nextObject ]) != nil){
        int row;
        int column;
        NSRange strRange;
        NSRect selRect;
        
        // 非選択状態を作成
        unselImage = [ [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ] autorelease ];
        strRange = NSMakeRange(0,1);
        [ unselImage lockFocus ];
        row=startRow;
        for(len = [ tempStr length ],column=locateX; strRange.location < len ; strRange.location++,column++){
            if(column >= textColumn){
                row++;
                column=locateX;
            }
            [ self drawChar:[ tempStr substringWithRange:strRange ] color:unSelectColor atColumn:column atRow:row ];
        }
        [ unselImage unlockFocus ];
        
        // 選択状態を作成
        selImage = [ [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ] autorelease ];
        row = startRow;
        strRange = NSMakeRange(0,1);
        [ selImage lockFocus ];
        for(len = [ tempStr length ],column=locateX; strRange.location < len ; strRange.location++,column++){
            if(column >= textColumn){
                row++;
                column=locateX;
            }
            [ self drawChar:[ tempStr substringWithRange:strRange ] color:selectColor atColumn:column atRow:row ];
        }
        [ selImage unlockFocus ];
        
        // CellImageを作成
        imageArray = [ NSArray arrayWithObjects:unselImage,selImage,nil ];
        cellImage = [ [ [ TYCellImage alloc ] initWithImages:imageArray ] autorelease ];
        
        // Rectを求める
        selRect = NSMakeRect(textLeftOffset,
                    VSCREEN_HEIGHT -textTopOffset -(textFontHeight +textPitchy) *row -textFontHeight,
                    (row -startRow) ? textColumn *(textFontWidth +textPitchx) : column *(textFontWidth +textPitchx),
                    (row -startRow +1) *(textFontHeight +textPitchy) -textPitchy);
        
        // マネージャに追加
        [ selectionManager addSelection:cellImage rect:selRect ];
        startRow = row;
        startRow++;
    }

    [ selectionManager initialButtonImage ];
    [ controller setImage:compositeLayer ];
    [ controller setStatus:TYSelectStatus ];
    [ engine setBreakRun:YES ];
    [ self setSkipStatus:TYNoSkip ];
}

-(void)ty_br:(NSMutableArray *)argments
{
    nowRow++;
    [ (TYNovelLayer*)novelLayer pushNewLine ];
}

-(void)ty_locate:(NSMutableArray *)argments
{
    locateX=[ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    nowColumn=locateX;
    nowRow=[ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
}

-(void)ty_textclear:(NSMutableArray *)argments
{
    nowRow=0;
    nowColumn=0;
    locateX=0;
    [ self flushNovelLayer ];

    [ engine setBreakRun:YES ];
    [ engine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
    
    return ;
}

-(void)ty_puttext:(NSMutableArray *)argments
{
    [ self textPrint:[ engine getStringOfArgment:[ argments objectAtIndex:1 ] ] ];
}

-(void)ty_clickstr:(NSMutableArray *)argments
{
    NSString *str;
    
    str = [ engine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    clickStrSet = [ [ NSCharacterSet characterSetWithCharactersInString:str ] retain ];
    clickStrUnderLimit = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];
}

-(void)ty_linepage:(NSMutableArray *)argments
{
    linepage = YES;
}

-(void)ty_defaultspeed:(NSMutableArray *)argments
{
    if(defaultSpeed)
        [ defaultSpeed release ];

    defaultSpeed=[ [ NSArray alloc ] initWithObjects:
        [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ],
        [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ],
        [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ],nil ];

    userTextSpeed=[ [ defaultSpeed objectAtIndex:[ controller selectedSpeedTag ] ] intValue ];
}

-(void)ty_textspeed:(NSMutableArray*)argments
{
    scriptTextSpeed = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    
    textSpeed = &scriptTextSpeed;
}

-(void)ty_skipoff:(NSMutableArray*)argments
{
    skipStatus = TYNoSkip;
}

-(void)ty_quakex:(NSMutableArray *)argments
{
    NSImage *image;
    int times,second;

    image = [ controller image ];
    times = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    second = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];

    if(effectGenerater)
        [ effectGenerater release ];

    effectGenerater = [ [ TYQuakeEffectGenerater alloc ] initWithImage:image quakeType:TYQuakeTypeHorizontal times:times time:second ];
    [ effectGenerater setDelegate:self ];
    
    [ engine setBreakRun:YES ];
}

-(void)ty_quakey:(NSMutableArray *)argments
{
    NSImage *image;
    int times,second;

    image = [ controller image ];
    times = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    second = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];

    if(effectGenerater)
        [ effectGenerater release ];

    effectGenerater = [ [ TYQuakeEffectGenerater alloc ] initWithImage:image quakeType:TYQuakeTypeVertical times:times time:second ];
    [ effectGenerater setDelegate:self ];

    [ engine setBreakRun:YES ];
}

-(void)ty_quake:(NSMutableArray *)argments
{
    NSImage *image;
    int amplitude,time;

    image = [ controller image ];
    amplitude = [ [ engine getValueOfArgment:[ argments objectAtIndex:1 ] ] intValue ];
    time = [ [ engine getValueOfArgment:[ argments objectAtIndex:2 ] ] intValue ];

    if(effectGenerater)
        [ effectGenerater release ];

    effectGenerater = [ [ TYNew_QuakeEffectGenerater alloc ] initWithImage:image
                                                                 amplitude:amplitude
                                                                      time:time ];

    [ effectGenerater setDelegate:self ];

    [ engine setBreakRun:YES ];    
}

-(void)ty_setcursor:(NSMutableArray*)argments
{
    int cursorNo;
    NSString *path;
    int x,y;

    cursorNo = [ [ argments objectAtIndex:1 ] intValue ];
    path = [ engine getStringOfArgment:[ argments objectAtIndex:2 ] ];
    x = [ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    y = [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];

    if(cursorNo==0){
        [ clickWaitCursor release ];
        clickWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:path
                                                             offset:NSMakePoint(x,y)
                                                           absolute:NO ];
        [ clickWaitCursor setDelegate:self ];
    } else {
        [ pageWaitCursor release ];
        pageWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:path
                                                             offset:NSMakePoint(x,y)
                                                           absolute:NO ];
        [ pageWaitCursor setDelegate:self ];
    }
}

-(void)ty_abssetcursor:(NSMutableArray*)argments
{
    int cursorNo;
    NSString *path;
    int x,y;

    cursorNo = [ [ argments objectAtIndex:1 ] intValue ];
    path = [ engine getStringOfArgment:[ argments objectAtIndex:2 ] ];
    x = [ [ engine getValueOfArgment:[ argments objectAtIndex:3 ] ] intValue ];
    y = [ [ engine getValueOfArgment:[ argments objectAtIndex:4 ] ] intValue ];

    if(cursorNo==0){
        [ clickWaitCursor release ];
        clickWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:path
                                                             offset:NSMakePoint(x,y)
                                                           absolute:YES ];
        [ clickWaitCursor setDelegate:self ];
    } else {
        [ pageWaitCursor release ];
        pageWaitCursor = [ [ TYWaitCursor alloc ] initWithResource:path
                                                            offset:NSMakePoint(x,y)
                                                          absolute:YES ];
        [ pageWaitCursor setDelegate:self ];
    }
}

-(void)ty_textgosub:(NSMutableArray*)argments
{
    textgosubLabel = [ engine getStringOfArgment:[ argments objectAtIndex:1 ] ];
    [ controller disableContextMenu ];
    [ textgosubLabel retain ];
}

-(void)ty_ispage:(NSMutableArray*)argments
{
    NSString *var;
    
    var = [ argments objectAtIndex:1 ];
    
    [ engine ty_mov:[ NSMutableArray arrayWithObjects:@"mov",var,[ NSNumber numberWithInt:isPage ],nil ] ];
}

/*
-(void)ty_texec:(NSMutableArray*)argments
{
    if(isPage)
        [ self flushNovelLayer ];
}
*/

-(void)ty_windowback:(NSMutableArray*)argments
{
    isWindowback = YES;
    overNovelLayer = [ [ NSImage alloc ] initWithSize:TYVirtualScreenSize() ];
}

-(void)ty_lookbackflush:(NSMutableArray*)argments
{
    if(lookbackQueue)
        [ lookbackQueue release ];
    lookbackQueue = [ [ NSMutableArray arrayWithCapacity:
                        [ [ TYEnviroment objectForKey:TYLookbackBufferPageEnviroment ] intValue ] ] retain ];
}

-(NSArray*)lookbackQueue
{
    return lookbackQueue;
}

#pragma mark ***implementation of make Image***

- (void)updateStage
{
    NSSize vscreenSize;
    NSEnumerator *spriteEnumerator;
    TYNScrSprite *temp;

    vscreenSize = TYVirtualScreenSize();

    [ visualLayer lockFocus ];
    
    if(bgImage){
        [ bgImage compositeToPoint:bgRect.origin fromRect:NSMakeRect(0,0,bgRect.size.width,bgRect.size.height) 
            operation:NSCompositeSourceOver ];
    }

    // 立ち絵より下のスプライトを描画
    spriteEnumerator = [ spriteArray objectEnumerator ];
    while( (temp = [ spriteEnumerator nextObject ]) != nil){
        if([ temp spriteID ] < humanz){
            break;
        }
        [ temp draw ];
    }
    
    if(leftCharImage){
        [ leftCharImage draw ];
    }
    if(centerCharImage){
        [ centerCharImage draw ];
    }
    
    if(rightCharImage){
        [ rightCharImage draw ];
    }

    // windowbackが指定されている時は、これらはtextwindowの上に描画される
    if(!isWindowback){
        // 立ち絵より上のスプライトを描画
        if(temp){
            [ temp draw ];
            while( (temp = [ spriteEnumerator nextObject ]) != nil){
                [ temp draw ];
            }        
        }
        // バーの描画
        if(barDict) {
            [ [ barDict objectsSortedByKeyUsingSelector:@selector(compare:) ] makeObjectsPerformSelector:@selector(draw) ];
        }
    }
    [ visualLayer unlockFocus ];
    
    // モノクロ・ネガ変換
    if(monocroSample || nega){
        unsigned const char *sample = NULL;
        if(monocroSample)
            sample = [ monocroSample bytes ];
        [ visualLayer convertMonocroWithSample:sample priority:nega ];
    }
    
    [ compositeLayer lockFocus ];
    [ visualLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    // テキスト表示部
    if(novelLayerOn){
        [ novelLayer recache ];
        [ novelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    } 
    // windowbackが指定されている場合にはテキストウインドウの上にスプライトを描画。
    if(isWindowback) {
        [ overNovelLayer lockFocus ];
        NSRectFillUsingOperation(TYVirtualScreenRect(),NSCompositeClear);
        if(temp){
            [ temp draw ];
            while( (temp = [ spriteEnumerator nextObject ]) != nil){
                [ temp draw ];
            }        
        }
        // バーの描画
        if(barDict) {
            [ [ barDict objectsSortedByKeyUsingSelector:@selector(compare:) ] makeObjectsPerformSelector:@selector(draw) ];
        }
        [ overNovelLayer unlockFocus ];
        [ overNovelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    }
    // ボタンの描画
    if(isDrawButton){
        [ buttonManager draw ];
    }

    [ compositeLayer unlockFocus ];
}

- (void)updateStageOnlyButton
{
    [ compositeLayer lockFocus ];
    [ visualLayer compositeToPoint:NSZeroPoint operation:NSCompositeCopy ];
    if(novelLayerOn){
        [ novelLayer recache ];
        [ novelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    }
    // windowbackが指定されている場合にはテキストウインドウの上にスプライトを描画。
    if(isWindowback) {
        [ overNovelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
        /*
        NSEnumerator *spriteEnumerator;
        TYNScrSprite *temp;
        
        spriteEnumerator = [ spriteArray objectEnumerator ];
        while( (temp = [ spriteEnumerator nextObject ]) != nil){
            if([ temp spriteID ] <= humanz){
                break;
            }
        }
        
        if(temp){
            while( (temp = [ spriteEnumerator nextObject ]) != nil){
                [ temp draw ];
            }
        }
         */
    }
    [ buttonManager draw ];
    [ compositeLayer unlockFocus ];
}

-(NSImage*)visualLayer
{
    return visualLayer;
}

-(NSImage*)compositeLayer
{
    return compositeLayer;
}

-(void)changeVisualLayer:(NSNumber*)effectNum
{
    int effectNo;
    
    effectNo = [ effectNum intValue ];
    
    // erasetextwindow設定が真のとき、テキストウインドウが表示中ならまずはそれを消す
    if(eraseTextWindow && novelLayerOn){
        [ self startEraseTextWindow:effectNum ];
        return ;
    }

    [ self updateStage ];

    // エフェクトジェネレータ作成
    if(effectGenerater)
        [ effectGenerater release ];
    effectGenerater = [ [ TYEffectGenerater alloc ] initWithBeforeImage:[ controller image ] afterImage:compositeLayer effect:[ effectDefDict objectForKey:[ NSNumber numberWithInt:(skipStatus & TYEffectCancelSkipMask) ? 1 : effectNo ] ] ];
    [ effectGenerater setDelegate:self ];

    [ engine setBreakRun:YES ];
    
    // エフェクト終了後の指示を作成
    if(afterEffectObject)
        [ afterEffectObject release ];

    if([ (TYMainController*)controller status ] == TYPrinting ){
        afterEffectTarget = self;
        afterEffectSelector = @selector(printing);
        afterEffectObject = nil;
    } else {
        afterEffectTarget = engine;
        afterEffectSelector = @selector(runScript);
        afterEffectObject = nil;
    }    
}

-(void)flushNovelLayer
{
    int page = [ [ TYEnviroment objectForKey:TYLookbackBufferPageEnviroment ] intValue ];
    
    // 回想バッファに突っこむ
    if(novelLayer && page) {
        int cnt;
        [ lookbackQueue addObject:[ (TYNovelLayer*)novelLayer lookbackObject ] ];
        cnt = [ lookbackQueue count ];
        if(cnt > page)
            [ lookbackQueue removeObjectsInRange:NSMakeRange(0,cnt -page) ];
    }
    
    [ novelLayer release ];
    novelLayer = [ [ TYNovelLayer alloc ] initNovelLayer ];
    [ (TYNovelLayer*)novelLayer setManager:self ];
    [ novelLayer recache ];

    [ compositeLayer lockFocus ];
    [ visualLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    [ novelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
    // windowbackが指定されている場合にはテキストウインドウの上にスプライトを描画。
    if(isWindowback) {
        [ overNovelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
        /*
        NSEnumerator *spriteEnumerator;
        TYNScrSprite *temp;
        
        spriteEnumerator = [ spriteArray objectEnumerator ];
        while( (temp = [ spriteEnumerator nextObject ]) != nil){
            if([ temp spriteID ] <= humanz){
                break;
            }
        }
        
        if(temp){
            while( (temp = [ spriteEnumerator nextObject ]) != nil){
                [ temp draw ];
            }
        }
         */
    }
    [ compositeLayer unlockFocus ];
    [ controller setImage:compositeLayer ];
    
}

// このへんは後でちゃんと整理します。
-(void)startAppearTextWindow:(NSNumber*)effectNum
{
    [ self updateStage ];
    
    // エフェクトジェネレータ生成
    if(effectGenerater)
        [ effectGenerater release ];
    effectGenerater = [ [ TYEffectGenerater alloc ] initWithBeforeImage:[ controller image ] afterImage:compositeLayer effect:[ effectDefDict objectForKey:[ NSNumber numberWithInt:(skipStatus & TYEffectCancelSkipMask) ? 1 : WINDOW_EFFECT_NO ] ] ];
    [ effectGenerater setDelegate:self ];

    [ engine setBreakRun:YES ];
    
    // エフェクト終了後の指示を作成
    afterEffectTarget = self;
    afterEffectSelector = @selector(resumePrinting);
    if(afterEffectObject)
        [ afterEffectObject release ];
    afterEffectObject = nil;

}

-(void)resumePrinting
{    
    [ controller setStatus:TYPrinting ];
    [ self setStatus:TYTPPrinting ];
    [ self printing ];
}

-(void)startEraseTextWindow:(NSNumber*)effectNum
{
    novelLayerOn = NO;

    // -updatestageは行わない。この時点では変更が反映されないから。

    // windowbackが指定されている場合は、visualLayerにテキストウインドウ上のスプライトを描画する
    // FIXME:ていうかこのルーチンコピペしすぎなのでリファクタリングしる。
    [ visualLayer lockFocus ];
    if(isWindowback) {
        [ overNovelLayer compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver ];
        /*
        NSEnumerator *spriteEnumerator;
        TYNScrSprite *temp;
        
        spriteEnumerator = [ spriteArray objectEnumerator ];
        while( (temp = [ spriteEnumerator nextObject ]) != nil){
            if([ temp spriteID ] <= humanz){
                break;
            }
        }
        
        if(temp){
            while( (temp = [ spriteEnumerator nextObject ]) != nil){
                [ temp draw ];
            }
        }
         */
    }
    [ visualLayer unlockFocus ];
    
    // エフェクトジェネレータ生成
    if(effectGenerater)
        [ effectGenerater release ];
    effectGenerater = [ [ TYEffectGenerater alloc ] initWithBeforeImage:[ controller image ] afterImage:visualLayer effect:[ effectDefDict objectForKey:[ NSNumber numberWithInt:(skipStatus & TYEffectCancelSkipMask) ? 1 : WINDOW_EFFECT_NO ] ] ];
    [ effectGenerater setDelegate:self ];

    [ engine setBreakRun:YES ];
    
    // エフェクト終了後の指示を作成
    afterEffectTarget = self;
    afterEffectSelector = @selector(changeVisualLayer:);
    if(afterEffectObject)
        [ afterEffectObject release ];
    afterEffectObject = effectNum;
    [ afterEffectObject retain ];
}

-(void)changeEffectionImage:(NSImage*)drawImage
{
   [ controller setImage:drawImage ];
}

-(void)effectFinished
{
    // MEMO:effectblankで設定された秒数後に叩くように
    // afterEffect～で設定された内容を叩く
    [ afterEffectTarget performSelector:afterEffectSelector withObject:afterEffectObject afterDelay:0 ];
}

-(void)quakeFinished
{
    if(effectGenerater){
        [ effectGenerater release ];
        effectGenerater=nil;
    }
    [ engine runScript ];
}
-(int)status
{
    return tpStatus;
}

-(void)setStatus:(int)status
{
    tpStatus = status;
    if(status == TYTPClickWait){
        if([ controller autoclickTimer]){
            [ self performSelector:@selector(select_action)
                        withObject:nil
                        afterDelay:[ controller autoclickTimer ] / 1000.0 ];
        }
    }
}

-(void)textPrint:(NSString*)aString
{
    NSScanner *scanner;
    NSString *str=nil;
    NSString *symbol;
    NSMutableArray *array;

    [ controller setStatus:TYPrinting ];
    [ self setStatus:TYTPPrinting ];

    [ engine setBreakRun:YES ];
    
    if(textQueue)
        [ textQueue autorelease ];
    // 必要なら文字列中の変数展開を行う
    scanner = [ NSScanner scannerWithString:aString ];
    [ scanner scanUpToCharactersFromSet:charcterOfReferenceVarSet intoString:&str ];
    if([ scanner isAtEnd ]==NO){
        array = [ NSMutableArray arrayWithCapacity:1 ];
        while(1){
            if(str)
                [ array addObject:str ];
            symbol = [ [ scanner string ] substringWithRange:NSMakeRange([ scanner scanLocation ],1) ];
            [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
            [ scanner scanCharactersFromSet:[ NSCharacterSet varNameCharacterSet ] intoString:&str ];
            if([ symbol isEqualToString:@"$" ]){
                [ array addObject:[ engine getStringOfArgment:[ NSString stringWithFormat:@"$%@",str ] ] ];
            } else if([ symbol isEqualToString:@"%" ]){
                int value;
                value = [ [ engine getValueOfArgment:[ symbol stringByAppendingString:str ] ] intValue ];
                [ array addObject:[ NSString stringWithFormat:@"%d",value ] ];
            }

            str = nil;
            [ scanner scanUpToCharactersFromSet:charcterOfReferenceVarSet intoString:&str ];
            if([ scanner isAtEnd ])
                break;
        }
        if(str)
            [ array addObject:str ];

        aString = [ array componentsJoinedByString:@"" ];
    }

    
    //textQueue = [ [ NSMutableAttributedString alloc ] initWithString:aString attributes:textAttDict ];
    textQueue = aString;
    [ textQueue retain ];
    nowStorageOffset = 0;
    nowStorageLength = [ textQueue length ];
    //NSLog(@"%@",[ textQueue attributesAtIndex:0 effectiveRange:nil ] );

    // すでに表示してあれば文字描画処理開始
    [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
}

-(void)drawTextWindow
{
    if([ textWindow isKindOfClass:[ NSColor class ] ]){
        [ textWindow set ];
        NSRectFillUsingOperation(textWindowRect,NSCompositeSourceOver);
    } else if(textWindow) {
        [ textWindow compositeToPoint:textWindowRect.origin operation:NSCompositeSourceOver ];
    }    
}

-(NSRect)textWindowRect
{
    return textWindowRect;
}

-(void)drawChar:(NSString*)charStr color:(NSColor*)color atColumn:(int)column atRow:(int)row
{
    NSTextStorage *storage;
    NSPoint drawPoint;
    NSRange actRange;
    
    drawPoint.x = textLeftOffset +(textFontWidth +textPitchx) *column;
    drawPoint.y = VSCREEN_HEIGHT -textTopOffset -(textFontHeight +textPitchy) *row -textFontHeight;
    if(textShadowed && (floor(NSAppKitVersionNumber) < MAC_OS_X_VERSION_10_3)){
        storage = [ [ [ NSTextStorage alloc ] initWithString:charStr attributes:textShadowAttDict ] autorelease ];
        [ layoutManager replaceTextStorage:storage ];
        [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ];
        [ layoutManager drawGlyphsForGlyphRange:actRange atPoint:NSMakePoint(drawPoint.x+SHADOW_TICKNESS,drawPoint.y-SHADOW_TICKNESS) ];
        [ storage addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,1) ];
    } else {
        storage = [ [ [ NSTextStorage alloc ] initWithString:charStr attributes:textShadowAttDict ] autorelease ];
        [ storage addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,1) ];
        [ layoutManager replaceTextStorage:storage ];
        [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ];
    }
    
    [ layoutManager replaceTextStorage:storage ];
    [ layoutManager drawGlyphsForGlyphRange:actRange atPoint:drawPoint ];
}

-(void)drawAttStr:(NSAttributedString*)str atColumn:(int)column atRow:(int)row
{
    NSColor *color;
    NSTextStorage *storage;
    NSPoint drawPoint;
    NSRange actRange;

    drawPoint.x = textLeftOffset +(textFontWidth +textPitchx) *column;
    drawPoint.y = VSCREEN_HEIGHT -textTopOffset -(textFontHeight +textPitchy) *row -textFontHeight;
    if(textShadowed && (floor(NSAppKitVersionNumber) < NSAppKitVersionNumber10_2)){
        color = [ [ str fontAttributesInRange:NSMakeRange(0,1) ] objectForKey:NSForegroundColorAttributeName ];
        storage = [ [ [ NSTextStorage alloc ] initWithAttributedString:str ] autorelease ];
        [ storage addAttribute:NSForegroundColorAttributeName value:[ NSColor blackColor ] range:NSMakeRange(0,1) ];
        [ layoutManager replaceTextStorage:storage ];
        [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ];
        [ layoutManager drawGlyphsForGlyphRange:actRange atPoint:NSMakePoint(drawPoint.x+SHADOW_TICKNESS,drawPoint.y-SHADOW_TICKNESS) ];
        [ storage addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,1) ];
    } else {
        storage = [ [ [ NSTextStorage alloc ] initWithAttributedString:str ] autorelease ];
        [ layoutManager replaceTextStorage:storage ];
        [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ];
    }

    [ layoutManager replaceTextStorage:storage ];
    [ layoutManager drawGlyphsForGlyphRange:actRange atPoint:drawPoint ];
}

-(NSPoint)drawPoint
{
    NSPoint drawPoint;

    drawPoint.x = textLeftOffset +(textFontWidth +textPitchx) *nowColumn;
    drawPoint.y = VSCREEN_HEIGHT -textTopOffset -(textFontHeight +textPitchy) *nowRow;

    return drawPoint;
}

// 本家が逐次処理ではなく、先に解析をすませて必要なら表示、という流れっぽいので大幅にこれとtextPrint:なおす必要があるかも。
-(void)printing
{
    NSDate *date = [ NSDate date ];
    //NSTextStorage *shadowStrage;
    NSTextStorage *charStorage;

    // 改ページ待ちだったのなら、novelLayer,compositeLayerをフラッシュ
    if(waitNewPage){
        waitNewPage = NO;
        nowRow=0;
        nowColumn=0;
        locateX=0;
        waitCountOfChar = 0;
        [ self flushNovelLayer ];
        [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
        return;
    }

    // 残り文字を調べ、あれば処理。なければスクリプト再生に戻る
    if( nowStorageOffset >= nowStorageLength ){
        if(forceWaitNewPage){
            forceWaitNewPage = NO;
            waitNewPage = YES;
            invalidNewLine = YES;
            [ self setClickWait:YES ];
            return;
        } else if(forceClickWait){
            forceClickWait = NO;
            [ self setClickWait:NO ];
            return;
        }
        if(linepage && !linepageWait){
            linepageWait = YES;
            waitNewPage = YES;
            [ self setClickWait:YES ];
            return;
        }
        linepageWait = NO;
        
        if((!invalidNewLine)&&(nowColumn)){
            nowRow++;
            [ (TYNovelLayer*)novelLayer pushNewLine ];
            nowColumn = locateX;
        }
        [ controller setStatus:TYScriptRunningStatus ];
        [ engine lineEnd ];// 行の終了を通知。
        [ engine performSelector:@selector(runScript) withObject:nil afterDelay:0 ];
    } else {
        NSPoint drawPoint;
        NSRange actRange;
        NSString *currentStr;

        currentStr = [ textQueue substringWithRange:NSMakeRange(nowStorageOffset,1) ];
        
        invalidNewLine = NO;
        
        // 表示文中の特殊命令かどうかのチェック
        if([ currentStr isEqualToString:@"@" ]){
            nowStorageOffset++;
            forceClickWait = NO;
            [ self setClickWait:NO ];
            return;
        } else if([ currentStr isEqualToString:@"\\" ]){
            nowStorageOffset++;
            forceWaitNewPage = NO;
            forceClickWait = NO;
            waitNewPage = YES;
            invalidNewLine = YES;
            [ self setClickWait:YES ];
            return;
        } else if([ currentStr isEqualToString:@"/" ]){ // 直後の改行を無視
            invalidNewLine = YES;
            nowStorageOffset++;
            [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
            return;
        } else if([ currentStr isEqualToString:@"#" ]){ // 色指定
            textFontColor = getColorWithHTMLFormat([ textQueue substringWithRange:NSMakeRange(nowStorageOffset,7) ]);
            [ self makeAttributeDict ];
            nowStorageOffset += 7;
            [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
            return ;
        /* 変数ダンプ
        } else if([ currentStr isEqualToString:@"%" ]){
        } else if([ currentStr isEqualToString:@"$" ]){
        */
        } else if([ currentStr isEqualToString:@"!" ]){
            int value;
            int length;
            
            // !のみで終わっている文への対処
            if([ textQueue length ] < nowStorageOffset+2){
                nowStorageOffset++;
                return;
            }
                
            currentStr = [ textQueue substringWithRange:NSMakeRange(nowStorageOffset+1,1) ];
            if([ currentStr isEqualToString:@"w" ]){ // ウェイト(キャンセル不可)
                if(!scanIntAndLengthFromString([ textQueue substringFromIndex:nowStorageOffset+2 ],&value,&length)){
                    // 例外をあげるべき
                    ;
                }
                [ self performSelector:@selector(printing) withObject:nil afterDelay:value / 1000.0 ];
                nowStorageOffset += length +2;
                return;
            } else if([ currentStr isEqualToString:@"d" ]){ // ディレイ(キャンセル可)
                if(!scanIntAndLengthFromString([ textQueue substringFromIndex:nowStorageOffset+2 ],&value,&length)){
                    // 例外をあげるべき
                    ;
                }
                [ self setStatus:TYTPClickWait ];
                [ self performSelector:@selector(select_action) withObject:nil afterDelay:value / 1000.0 ];
                nowStorageOffset += length +2;
                return;
            } else if([ currentStr isEqualToString:@"s" ]) { // テキストスピード調整
                if([ [ textQueue substringWithRange:NSMakeRange(nowStorageOffset +2,1) ] isEqualToString:@"d" ]){
                    // メニューから選ばれていたスピードに設定
                    textSpeed = &userTextSpeed;
                    nowStorageOffset += 3;
                } else {
                    if(!scanIntAndLengthFromString([ textQueue substringFromIndex:nowStorageOffset +2 ],&value,&length)){
                        ;
                    }
                    scriptTextSpeed = value;
                    textSpeed = &scriptTextSpeed;
                    nowStorageOffset += length +2;
                }
                [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
                return ;
            }
            // 例外を上げれ。
            ;
        }

        // テキストウインドウが表示されていなかったらまずは表示する。
        if(!novelLayerOn){
            novelLayerOn = YES;
            //[ self setNextPerformSelector:@selector(printing) ];
            [ self startAppearTextWindow:[ NSNumber numberWithInt:WINDOW_EFFECT_NO ] ];
            return ;
        }
        
        // 文字送り
        if(nowColumn>=textColumn){
            nowColumn=locateX;
            nowRow++;
            [ (TYNovelLayer*)novelLayer pushNewLine ];
        } else if(nowColumn+1==textColumn) {
            if((nowStorageOffset+1 < nowStorageLength) &&
              [ [ NSScanner scannerWithString:
[ textQueue substringWithRange:NSMakeRange(nowStorageOffset+1,1) ] ] scanCharactersFromSet:hyphenationSet intoString:nil ]){
                nowColumn=locateX;
                nowRow++;
                [ (TYNovelLayer*)novelLayer pushNewLine ];
            }
        }
        
        
        // 強制クリック待ち文字（設定されていれば）のチェック
        if([ currentStr isEqualToString:@"_" ]){
            nowStorageOffset++;
            if( nowStorageOffset >= nowStorageLength ){
                [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
                return;
            }
            currentStr = [ textQueue substringWithRange:NSMakeRange(nowStorageOffset,1) ];
        } else {
            // 直前が強制クリック待ちを無視以外の時だけチェックされる。連続する場合は最後のみ有効とする。
            if((clickStrSet)&&([ [ NSScanner scannerWithString:currentStr ] scanCharactersFromSet:clickStrSet intoString:nil ])){
                if(textRow -nowRow -1 <= clickStrUnderLimit ){
                    forceWaitNewPage = YES;
                } else {
                    forceClickWait = YES;
                }
            } else if(forceClickWait){
                forceClickWait = NO;
                [ self setClickWait:NO ];
                return;
            } else if(forceWaitNewPage){
                forceWaitNewPage = NO;
                waitNewPage = YES;
                invalidNewLine = YES;
                [ self setClickWait:YES ];
                return;
            }
        }

        drawPoint.x = textLeftOffset +(textFontWidth +textPitchx) *nowColumn;
        drawPoint.y = VSCREEN_HEIGHT -textTopOffset -(textFontHeight +textPitchy) *nowRow -textFontHeight;
        //[ layoutManager replaceTextStorage:textQueue ];
        [ compositeLayer lockFocus ];
        if(textShadowed && (floor(NSAppKitVersionNumber) < NSAppKitVersionNumber10_2)){
            charStorage = [ [ NSTextStorage alloc ] initWithString:currentStr attributes:textShadowAttDict ];
            [ charStorage autorelease ];
            
            [ layoutManager replaceTextStorage:charStorage ];

            [ layoutManager drawGlyphsForGlyphRange:
                [ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ]
                 atPoint:NSMakePoint(drawPoint.x +SHADOW_TICKNESS,drawPoint.y -SHADOW_TICKNESS) ];
            [ charStorage addAttribute:NSForegroundColorAttributeName value:textFontColor range:NSMakeRange(0,1) ];
            [ layoutManager replaceTextStorage:charStorage ];
            [ layoutManager drawGlyphsForGlyphRange:actRange atPoint:drawPoint ];
        } else {
            charStorage = [ [ NSTextStorage alloc ] initWithString:[ textQueue substringWithRange:NSMakeRange(nowStorageOffset,1) ] attributes:textAttDict ];
            [ charStorage autorelease ];
            [ layoutManager replaceTextStorage:charStorage ];
            [ layoutManager drawGlyphsForGlyphRange:[ layoutManager glyphRangeForCharacterRange:NSMakeRange(0,1) actualCharacterRange:&actRange ] 
 atPoint:drawPoint ];
        }

        /* 表示が乱れるため、コメントアウト
        if(isWindowback) {
            NSRect aRect;
            NSGlyph glyph;
            NSRect bRect;
            glyph = [ layoutManager glyphAtIndex:0 ];
            bRect = [ textFont boundingRectForGlyph:glyph ];
            aRect = TYFontRect(&drawPoint,&bRect,textShadowed);
            [ overNovelLayer compositeToPoint:aRect.origin fromRect:aRect operation:NSCompositeSourceOver ];
        }
         */
        
        [ compositeLayer unlockFocus ];

        [ (TYNovelLayer*)novelLayer pushAttributedString:charStorage ];
        
        {
            NSGlyph glyph;
            NSRect bRect;
            glyph = [ layoutManager glyphAtIndex:0 ];
            bRect = [ textFont boundingRectForGlyph:glyph ];
            [ controller setImage:compositeLayer inRect:TYFontRect(&drawPoint,&bRect,textShadowed) ];
        }

        nowColumn++;
        nowStorageOffset++;
        [ self performSelector:@selector(printing) withObject:nil afterDelay:
            ((skipStatus & TYNoTimeWaitSkipMask) ? 0 : *textSpeed / 1000.0) +[ date timeIntervalSinceNow ] ];
    }    
}

-(void)setClickWait:(BOOL)pageWait
{
    NSPoint point;
    int count;

    
    if(skipStatus==TYNoSkip || skipStatus==TYOnetimeSkipMask ||(skipStatus & TYAutoModeSkipMask)){
        int columnbuf;

        count = nowRow * textColumn + nowColumn;
        if(count < waitCountOfChar)
            waitCountOfChar = 0;

        if(textgosubLabel){
            isPage = pageWait;
            [ engine fireOfTextgosub:textgosubLabel ];
        } else {                
            currentCursor = (pageWait) ? pageWaitCursor : clickWaitCursor;
            if(nowColumn>=textColumn){
                columnbuf=nowColumn;
                nowColumn=locateX;
                nowRow++;
                point=[ self drawPoint ];
                [ currentCursor drawToView:controller point:point ];
                nowColumn=columnbuf;
                nowRow--;
            } else {
                point=[ self drawPoint ];
                [ currentCursor drawToView:controller point:point ];
            }
            if(skipStatus & TYAutoModeSkipMask){
                if(playingSoundFlg)
                    waitFinishSound=YES;
                else {
                    if(automodeWaitType == TYNumOfCharsAutomodeWait)
                        [ self performSelector:@selector(automode_select_action) withObject:nil afterDelay:(count -waitCountOfChar) *automodeWait / 1000.0 ];
                    else
                        [ self performSelector:@selector(automode_select_action) withObject:nil afterDelay:automodeWait / 1000.0 ];
                }
            }
        }
        [ self setStatus:TYTPClickWait ];
        
        if(skipStatus == TYOnetimeSkipMask){
            [ self setSkipStatus:TYNoSkip ];
        }

        waitCountOfChar = count;
        
        return;
    } else {
        [ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
        return;
    }
}

-(void)setSkipStatus:(int)aStatus
{
    skipStatus = aStatus;
    if(aStatus && ([ controller status ] == TYPrinting)&&([ self status ] == TYTPClickWait)){
        if(aStatus != TYAutoModeSkipMask)
            [ self select_action ];
        else
            [ self automode_select_action ];
    }
}

-(int)skipStatus
{
    return skipStatus;
}

-(NSImage*)textWindow
{
    return textWindow;
}

-(void)makeAttributeDict
{
    int fontSize;
    
    // MEMO:fontSizeは暫定処理。横長、縦長にも対応する必要性あり。
    fontSize = (textFontWidth <= textFontHeight) ? textFontWidth : textFontHeight;
    textFont = [ NSFont fontWithName:textFontName size:fontSize *1.0 ]; // NScriptの描画と感覚が違うので調整が必要?
    // Bold処理
    if(textBold && [ TYEnviroment boolForKey:TYEnableBoldFontEnviroment ])
        textFont = [ [ NSFontManager sharedFontManager ] convertFont:textFont toHaveTrait:NSBoldFontMask ];
    
    displayFontSize = [ textFont boundingRectForFont ].size;

    /*
    if([ [ layoutManager textContainers ] count ]){
        [ layoutManager removeTextContainerAtIndex:0 ];
        [ layoutManager addTextContainer:[ [ NSTextContainer alloc ] initWithContainerSize:NSZeroSize ] ];
    }
    */

    if(textAttDict)
        [ textAttDict autorelease ];
    textAttDict = [ [ NSMutableDictionary alloc ] initWithObjectsAndKeys:
                    textFont,NSFontAttributeName,nil ];
    if(textShadowAttDict)
        [ textShadowAttDict autorelease ];
    textShadowAttDict = [ textAttDict mutableCopyWithZone:[ textShadowAttDict zone ] ];
    
    // APIによるドロップシャドウ(10.3以降)
    if ((floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_2) && 
        (textShadowed == YES)) {
        NSShadow *shadowObj = [ [ [ NSShadow alloc ] init ] autorelease ]; 
        [ shadowObj setShadowOffset:NSMakeSize(2,-2) ];
        [ shadowObj setShadowBlurRadius:1.0 ];
        [ shadowObj setShadowColor:
            [ NSColor colorWithCalibratedRed:0
                                    green:0
                                        blue:0
                                    alpha:1 ] ];

        [ textAttDict setObject:shadowObj forKey:NSShadowAttributeName ];
    } else {
        
        if(textShadowAttDict)
            [ textShadowAttDict autorelease ];
        textShadowAttDict = [ textAttDict mutableCopyWithZone:[ textShadowAttDict zone ] ];
    }

    [ textAttDict setObject:textFontColor forKey:NSForegroundColorAttributeName ];
                    
    //[ textShadowAttDict setObject:[ NSColor blackColor ] forKey:NSForegroundColorAttributeName ];
}

-(NSDictionary*)textAttDict{ return textAttDict; }

-(int)textFontWidth{ return textFontWidth; }

-(int)textFontHeight{ return textFontHeight; }

-(int)textPitchx{ return textPitchx; }

-(BOOL)isShadow{ return textShadowed; }

-(void)updateSprites:(NSNotification*)notification
{
    // MEMO:スプライトのRectのみの更新に最適化すべき。
    [ self updateStage ];
    [ controller setImage:compositeLayer ];    
}

-(void)startWaveSound:(NSNotification*)aNotification
{
    playingSoundFlg=YES;
}

-(void)stopWaveSound:(NSNotification*)aNotification
{    
    playingSoundFlg=NO;
    if(waitFinishSound){
        waitFinishSound=NO;
        [ self performSelector:@selector(automode_select_action) withObject:nil afterDelay:0 ];
    } else if (waitFinishSoundOnBtnTime2 == YES) {
        waitFinishSoundOnBtnTime2 = NO;
        [ self performSelector:@selector(other_action:) withObject:[ NSNumber numberWithInt:-5 ] afterDelay:0 ];
    }
}

- (void)changeFont:(id)fontManager
{
    NSFont*	dummyFont = [NSFont fontWithName:@"Times" size:14];
    NSFont*	newFont;
    
    newFont = [fontManager convertFont:dummyFont];
    
    NSLog(@"font Name = %@",[ newFont fontName ]);
    textFontName = [ newFont fontName ];
    [ TYEnviroment setObject:textFontName forKey:TYFontNameEnviroment ];
    
    [ self makeAttributeDict ];
}

- (void)changeTextSpeed:(int)index
{
    userTextSpeed = [ [ defaultSpeed objectAtIndex:index ] intValue ];
}

- (void)setAutoMode:(int)speed
           waitType:(int)type
               wait:(int)wait
{
    [ self setSkipStatus:TYAutoModeSkipMask ];
    userTextSpeed = speed;
    textSpeed=&userTextSpeed;
    automodeWait = wait;
    automodeWaitType = type;
}

- (int)userTextSpeed
{
    return userTextSpeed;
}

-(void)responseButton:(int)result
{
    NSMutableArray *argments;

    isDrawButton = NO;
    
    // タイマ発生のアクションを解除。
    [ NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(other_action:) object:nil ];
    
    // 変数に結果を書き込み
    argments = [ NSMutableArray arrayWithCapacity:3 ];
    [ argments addObject:[ NSNull null ] ];
    [ argments addObject:buttonTargetValue ];
    [ argments addObject:[ NSNumber numberWithInt:(result < 0) ? result : [ buttonManager selectedButtonID ] ] ]; 
    [ engine performSelector:@selector(ty_mov:) withObject:argments ];

    // 選択した結果が0より大きい場合は即座に解放する。
    if(result >  0 && releaseButtonAfterSelect){
        [ buttonManager release ];
        buttonManager = [ [ TYExtraButtonManager alloc ] initWithStageManager:self
                                                                  sourceImage:nil ];
    }

    // Viewに現在のオフスクリーンのコピーを保存させる
    [ controller setImage:[ [ compositeLayer copyWithZone:[ compositeLayer zone ] ] autorelease ] ];
    
    // スクリプトの処理を再開
    [ engine runScript ];
}

-(void)changeSelectedButton:(unsigned int)btnIndex
{
    if(btnIndex != beforeSelectButtonID){
        [ buttonManager changeSelection:btnIndex ];
        [ controller setImage:compositeLayer ];
        beforeSelectButtonID = btnIndex;
    }
}

-(void)responseSelection
{
    int result;
    
    result = [ selectionManager selectedButtonID ];
    if(!result){
        return ;
    }

    [ self flushNovelLayer ];
    nowRow=0;
    nowColumn=0;
    locateX=0;
    
    [ engine eval:[ selectAction objectAtIndex:result -1 ] ];
    [ selectAction release ];
    [ selectionManager release ];
    [ engine runScript ];

}

-(void)changeSelection:(int)index
{
    if([ selectionManager changeSelection:index ]){
        [ controller setImage:compositeLayer ];
    }
}

- (void)dealloc
{
    if(effectGenerater)
        [ effectGenerater release ];
    [ super dealloc ];
}

-(void)automode_select_action
{
    if(tpStatus == TYTPClickWait) {
        [ self setStatus:TYTPPrinting ];
        //[ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
        [ currentCursor clear ];
        currentCursor = nil;
        [ self resumePrinting ];        
    }
}

// protocol implementations
-(void)select_action
{
    
    switch ([ controller status ]){
    case TYPrinting:
        switch (tpStatus) {
        case TYTPClickWait:
            if(skipStatus == TYAutoModeSkipMask) {
                [ self setSkipStatus:TYNoSkip ];
                return;
            }
            [ NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(automode_select_action) object:nil ];
            [ self setStatus:TYTPPrinting ];
            //[ self performSelector:@selector(printing) withObject:nil afterDelay:0 ];
            [ currentCursor clear ];
            currentCursor = nil;
            [ self resumePrinting ];
            break;
        case TYTPPrinting:
            if(skipStatus==TYNoSkip){
                [ self setSkipStatus:TYOnetimeSkipMask ];
            } else {
                if(skipStatus==TYKidokuSkipMask){
                    [ engine setWatchKidoku:NO ];
                }
                [ self setSkipStatus:TYNoSkip ];
            }
            break;
        default:
            ;
        }
        break;
    case TYButtonWaitStatus:
        [ self responseButton:beforeSelectButtonID ];
        return;
    case TYSelectStatus:
        [ self responseSelection ];
    default:
        ;
    }

}

-(IBAction)cancel_action:(id)sender
{
    switch ([ controller status ]) {
    case TYButtonWaitStatus:
        [ self responseButton:-1 ];
        return;
    default :
        if([ self status ] == TYTPClickWait){
            [ controller openRightclickMenu:nil ];
        }
    }
}

-(void)up_action
{
    switch ([ controller status ]){
    case TYButtonWaitStatus:
        [ self changeSelectedButton:[ buttonManager decToIndex ] ];
        return;
    case TYSelectStatus:
        if([ selectionManager selectedIndex ] <= 1)
            [ controller enterLookback:nil ];
        else
            [ self changeSelection:[ selectionManager decToIndex ] ];
        break;
    default:
        if([ self status ]==TYTPClickWait){
            [ controller enterLookback:nil ];
        }
    }
}

-(void)down_action
{
    switch ([ controller status ]){
    case TYButtonWaitStatus:
        [ self changeSelectedButton:[ buttonManager incToIndex ] ];
        return;
    case TYSelectStatus:
        [ self changeSelection:[ selectionManager incToIndex ] ];
    default:
        ;
    }
}

-(void)move_mouse:(NSPoint)point
{
    const unsigned char *ptr;
    int btnIndex;
    
    //　Viewの範囲内かいちおうチェック

    switch([ controller status ]){
    case TYButtonWaitStatus:
        if(NSPointInRect(point,TYVirtualScreenRect())){
            ptr = (const unsigned char*)[ [ buttonManager rectMap ] bytes ];
            ptr += (int)point.x +(int)point.y *VSCREEN_WIDTH;
            btnIndex = *ptr;
        } else {
            btnIndex = 0;
        }
        [ self changeSelectedButton:btnIndex ];
        break;
    case TYSelectStatus:
        if(NSPointInRect(point,TYVirtualScreenRect())){
            ptr = (const unsigned char*)[ [ selectionManager rectMap ] bytes ];
            ptr += (int)point.x +(int)point.y *VSCREEN_WIDTH;
            btnIndex = *ptr;
        } else {
            btnIndex = 0;
        }
        [ self changeSelection:btnIndex ];
        break;
    default:
        ;
    }
}

-(void)other_action:(id)code
{
    switch([ controller status ]){
    case TYButtonWaitStatus:
        [ self responseButton:[ code intValue ] ];
        break;
    default:
        ;
    }
}

-(void)enterSystemMode
{
    [ currentCursor stop ];
}

-(void)exitSystemModeResumeStatus:(BOOL)aBool
{
    if(aBool){
        switch ([ controller status ]){
        case TYPrinting:
            [ currentCursor resume ];
        case TYSelectStatus:
        default:
            [ controller setImage:compositeLayer ];
            break;
        }
    }
}

-(BOOL)isPossibleEnterSystemMode
{
    switch([ self status ]){
    case TYTPClickWait:
        return YES;
    default :
        return NO;
    }
}

@end
