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

#include <QObject>
#include <QImage>
#include <QMap>
#include <QString>

// エフェクト情報の管理、エフェクト画像の生成管理、更新タイマイベントの実行を行うクラスです。もう使ってないけど。
class TYEffecter : public QObject {
    Q_OBJECT
public:
    explicit TYEffecter(QObject* parent = nullptr);
    ~TYEffecter();

    static TYEffecter* effecter();

    void setEffectNo(int effectNo, int effectType, int time, const QString& path);
//-(NSImage *)playEffectNo:(int)effectNo fromImage:(NSImage*)fromImage toImage:(NSImage*)toImage;
    void setBeforeImage(QImage aImage);
    QImage beforeImage() const;
//-(TYEffectRegister*)getEffect:(int)effectNo;

// エフェクト効果開始準備
    bool startEffect(QImage newImage, int effectNo);
// エフェクトのかかったイメージを取得
    bool getEffectionImage(QImage& drawImage);

private:
    QImage beforeImage_;
    QImage afterImage_;
    QImage nowImage_;
    int currentEffectNo_;
    int currentTime_;
    QString currentPath_;
};

enum TYEffectType {
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
};

// エフェクトの方向
enum TYEffectDirection {
    TYTopDirection = 1,
    TYBottomDirection = 2,
    TYLeftDirection = 4,
    TYRightDirection = 8
};

struct TYEffectDefinition {
    TYEffectType type;
    int time;
    QString path;
};

inline TYEffectDefinition TYMakeEffectDefinition(TYEffectType aType, int aTime, const QString& aPath) {
    TYEffectDefinition effdef;
    effdef.type = aType;
    effdef.time = aTime;
    effdef.path = aPath;
    return effdef;
}

// NSValueはクラスクラスタなのでオブジェクトを含む構造体を管理するにはラッパを作るしかないわけで。
#endif // TYEffecter_h
