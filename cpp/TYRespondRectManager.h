//
//  TYRespondRectManager.h
//  Tukuyomi
//
//  Created by toveta on Tue Nov 06 2001.
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

#ifndef TYRespondRectManager_h
#define TYRespondRectManager_h

#include <QObject>
#include <QRect>
#include <QImage>
#include <QList>
#include <QByteArray>
#include <QPainter>

#ifndef VSCREEN_WIDTH
#define VSCREEN_WIDTH 640
#endif
#ifndef VSCREEN_HEIGHT
#define VSCREEN_HEIGHT 480
#endif

class TYStageManager;
class TYCellImage;

// 選択肢モード、メニューモード、ボタンモードなどの時に画面を制御する抽象クラス
// MEMO:ボタンもそのうちこちらのサブクラスに移行させるか？
class TYRespondRectManager : public QObject
{
    Q_OBJECT

public:
    explicit TYRespondRectManager(QObject* parent = nullptr);
    TYRespondRectManager(TYStageManager* manager, QObject* parent = nullptr);
    ~TYRespondRectManager();

    void setStageManager(TYStageManager* manager);
    TYStageManager* stageManager() const { return m_stageManager; }

    void addSelection(TYCellImage* aButton, const QRect& rect);

    void initialButtonImage(); // モード開始処理

    int selectedButtonID();
    int selectedIndex();
    int incToIndex(); // インデックスをひとつ増やす
    int decToIndex(); // インデックスをひとつ減らす

    QByteArray rectMap() const;
    bool changeSelection(int index); // 変更があったかどうかを返り値で通知。

    void draw(QPainter* painter);
    void clear();

private:
    void updateRectMap();

private:
    TYStageManager* m_stageManager;
    QList<TYCellImage*> m_btnArray;
    QImage m_drawBuffer;
    int m_beforeSelection;
    QByteArray m_rectMap;

public:
    QImage getDrawBuffer() const { return m_drawBuffer; }
    void setBeforeSelection(int selection) { m_beforeSelection = selection; }
    void setDrawBuffer(const QImage& buffer) { m_drawBuffer = buffer; }
};

#endif // TYRespondRectManager_h