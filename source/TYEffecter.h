//
//  TYEffecter.h
//  Tukuyomi
//
//  Created by toveta on Mon Jun 18 2001.
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

@class TYEffectRegister;
// エフェクト情報の管理、エフェクト画像の生成管理、更新タイマイベントの実行を行うクラスです。もう使ってないけど。
@interface TYEffecter : NSObject {
    NSMutableDictionary *ty_effectDict; // ユーザー定義エフェクト番号格納辞書
    NSDate *startDate; // エフェクト開始時刻
    NSTimeInterval beforeParsentage;
    TYEffectRegister *nowEffect;
    NSImage *beforeImage;
    NSImage *afterImage;
    NSImage *nowImage;
}

+(id)effecter;
-(id)init;
-(void)setEffectNo:(int)effectNo type:(int)effectType time:(int)time path:(NSString*)path;
//-(NSImage *)playEffectNo:(int)effectNo fromImage:(NSImage*)fromImage toImage:(NSImage*)toImage;
-(void)setBeforeImage:(NSImage*)aImage;
-(NSImage*)beforeImage;
//-(TYEffectRegister*)getEffect:(int)effectNo;

// エフェクト効果開始準備
-(BOOL)startEffect:(NSImage*)newImage effectNo:(int)effectNo;
// エフェクトのかかったイメージを取得
-(BOOL)getEffectionImage:(NSImage**)drawImage;


typedef enum  {
TYEffectCached=0,// 変更しない。キャッシングのみ
TYEffectInstant, // 一瞬で表示
TYEffectLeftShutter,//　２　左シャッター
TYEffectRightShutter,//　３　右シャッター
TYEffectTopShutter,//　４　上シャッター
TYEffectBottomShutter,//　５　下シャッター
TYEffectLeftCartain,//　６　左カーテン
TYEffectRightCartain,//　７　右カーテン
TYEffectTopCartain,//　８　上カーテン
TYEffectBottomCartain,//　９　下カーテン
TYEffectCrossFadeWithPixel,//１０　ピクセル単位のクロスフェード
TYEffectLeftScroll,//１１　左からスクロール
TYEffectRightScroll,//１２　右からスクロール
TYEffectTopScroll,//１３　上からスクロール
TYEffectBottomScroll,//１４　下からスクロール
TYEffectFadeWithMask,//１５　マスクパターンによるフェード。
TYEffectMosaicOut,//16 モザイクアウト
TYEffectMosaicIn,//17 モザイクイン
TYEffectCrossFadeWithMask,//18 マスクパターンによるクロスフェード
} TYEffectType ;
@end

// エフェクトの方向
typedef enum {
    TYTopDirection=1,
    TYBottomDirection=2,
    TYLeftDirection=4,
    TYRightDirection=8
} TYEffectDirection;

// 特殊なエフェクト番号
enum {
TYEffectNoWindow=-2,
TYEffectNoLocal=-1,
TYEffectNoChached=0,
TYEffectNoInstant=1
};

// 辞書に登録されるエフェクトオブジェクト
@interface TYEffectRegister : NSObject {
    int ty_effectType;
    int ty_time; 
    NSString* ty_path;
}
+(id)effectRegisterType:(int)type time:(int)time path:(NSString*)path;
-(id)initWithType:(int)type time:(int)time path:(NSString*)path;
-(int)getType;
-(int)getTime;
-(NSString*)getPath;

@end

typedef struct tyEffectDefinition {
    TYEffectType type;
    int time;
    NSString *path;
} TYEffectDefinition;

static __inline__ TYEffectDefinition TYMakeEffectDefinition(TYEffectType aType,int aTime,NSString* aPath) {
    TYEffectDefinition effdef;
    effdef.type = aType;
    effdef.time = aTime;
    effdef.path = aPath;
    return effdef;
}

// NSValueはクラスクラスタなのでオブジェクトを含む構造体を管理するにはラッパを作るしかないわけで。
@interface TYEffectDefinitionValue : NSObject {
    NSValue *value;
}
+(id)valueWithEffectDefinition:(TYEffectDefinition)aDefinition;
-(id)initWithEffectDefinition:(TYEffectDefinition)aDefinition;
-(void)getValue:(void *)buffer;
@end