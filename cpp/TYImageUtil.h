//
//  TYImageUtil.h
//  Tukuyomi
//
//  Created by toveta on Mon Aug 27 2001.
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

#ifndef TYIMAGEUTIL_H
#define TYIMAGEUTIL_H

#include <QString>
#include <QImage>
#include <QColor>
#include <QList>
#include <QMap>

// Image 操作に役立つ関数郡

// '#rrggbb'形式の文字列からRGBをint型にして取り出す
extern void getRGBWithHTMLFormat(const QString& formatstr, int* r, int* g, int* b);

// '#rrggbb'形式の文字列からColorを作成
extern QColor getColorWithHTMLFormat(const QString& formatstr);

// '#rrggbb'形式の文字列から、textWindowのColorを作成
extern QColor getTextWindowColorWithHTMLFormat(const QString& formatstr);

// タグで指定した透過形式に画像を加工して返す
extern QImage ConvertImagePackedAlpha(const QImage& srcImage);
extern QImage ConvertImageTransrateLeftTop(const QImage& srcImage);
extern QImage ConvertImageWithMaskImage(const QImage& srcImage, const QImage& maskImage);

extern QImage blankBitmap(int width, int height);

extern QString TYTransmodeSymbolFromString(const QString& key);

#define TYTRANSMODE_LEFTTOP "l"
#define TYTRANSMODE_RIGHTTOP "r"
#define TYTRANSMODE_COPY "c"
#define TYTRANSMODE_ALPHA "a"
#define TYTRANSMODE_MASK "m"

#define DEFAULT_TRANSMODE "l"

enum TYNegaPriority {
    TYNoNegaPriority=0,
    TYNegaLowPriority=1,
    TYNegaHighPriority=2
};

namespace TYImageUtil {
    void convertMonocroWithSample(QImage& image, const unsigned char* sample, TYNegaPriority priority);
    
    void monocro(QImage& image, const unsigned char* sample);
    
    void nega(QImage& image);
    
    QList<QImage> horizontalDivide(const QImage& image, int count);
    
    void resizeToPixelsSize(QImage& image);
}

#endif // TYIMAGEUTIL_H
