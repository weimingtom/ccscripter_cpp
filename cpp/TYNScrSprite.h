//
//  TYNScrSprite.h
//  Tukuyomi
//
//  Created by toveta on Fri Oct 19 2001.
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

#ifndef TYNSSPRSPRITE_H
#define TYNSSPRSPRITE_H

#include "TYSprite.h"

// NScriptで定義するところのスプライト
class TYNScrSprite : public TYSprite {
    Q_OBJECT
public:
    explicit TYNScrSprite(QObject* parent = nullptr);
    ~TYNScrSprite();

//+(id)dummySprite:(int)idNo; // Array検索用のダミーを作成
    TYNScrSprite(int idNo, const QString& aPath, const QPointF& point, int alphaNum, bool aBool, QObject* parent = nullptr);
//-(id)initWithID:(int)idNo image:(NSImage*)aImage point:(NSPoint)point alpha:(NSNumber*)alphaNum visible:(BOOL)aBool;


    void moveX(int x, int y, int alphaNum);
    void absoluteMoveX(int x, int y, int alphaNum);
    void setVisible(bool aBool);
    bool isVisible() const { return m_visible; }

    int spriteID() const { return m_spriteID; }

    void draw(QPainter* painter) override; // override
    void loadImageFromPath() override;

    void didChangeCell();

signals:
    void cellChanged();

public:
    static const QString AnimationNotification;

private:
    int m_spriteID;
    bool m_visible;
    bool m_registDelegate;
};

QPointF convertToQtCoordinates(const QPointF& macPoint, int imageHeight);

#endif // TYNSSPRSPRITE_H
