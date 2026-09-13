//
//  TYSprite.h
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

#ifndef TYSPRITE_H
#define TYSPRITE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QPointF>
#include <QRectF>

class TYResourceServer;
class TYCellImage;

// 立ち絵とスプライトを作るための抽象クラス
class TYSprite : public QObject, public QSharedData {
    Q_OBJECT
protected:
    QImage* m_sourceImage;
    QString m_imagePath;
    QPointF m_drawPoint;
    int m_alpha;
public:
    void loadImageFromPath();
    QImage* sourceImage();
    virtual void draw(QPainter* painter);
    QRectF rect();
    void setCell(int cellNo);
    void convertAlphaOfFix();

    void encodeWithCoder(void* coder);
    static TYSprite* initWithCoder(void* decoder);
private:
    void cleanup();
public:
    TYSprite(QObject* parent = nullptr) {}
    virtual ~TYSprite();
};

#define SPRITE_ALPHA_MAX 255

#endif // TYSPRITE_H
