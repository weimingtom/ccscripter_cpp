//
//  TYCellImage.h
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TYCELLIMAGE_H
#define TYCELLIMAGE_H

#include <QObject>
#include <QList>
#include <QImage>

// Cell構造を持つイメージ。複数のImageを保持し、現在指定されている番号のImageにメッセージを転送することで機能を実現する。
// imageに転送できない場合はArrayへの転送も試みる。
class TYCellImage : public QObject {
    Q_OBJECT
public:
    TYCellImage(QObject* parent = nullptr);
    ~TYCellImage();

    bool initWithImages(QList<QImage*> aArray);
//-(id)initWithAttributeString:(NSAttributeString*)aString colors:(NSArray*)colorStrings;
    void changeCell(int index);
    int currentCell() const;
    int cells() const;
    bool isCellImage() const;
    bool isAnimate() const;

    QImage* currentImage() const { return m_currentImage; }

    bool respondsToSelector(const char* selector) const;
    void forwardInvocation(void* invocation);

    QString description() const;

private:
    QList<QImage*> m_imageArray;
    QImage* m_currentImage;
    int m_currentCell;
};

class QImageTYCellImageExtension
{
public:
    static void changeCell(QImage* image, int index);
    static bool isCellImage(QImage* image);
    static bool isAnimate(QImage* image);
};

#endif // TYCELLIMAGE_H
