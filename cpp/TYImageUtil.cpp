//
//  TYImageUtil.m
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

#include "TYImageUtil.h"
#include "TYResourceServer.h"
#include <QColor>
#include <QImage>
#include <QList>
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <cstring>
#include <cstdlib>
#include <cassert>

void getRGBWithHTMLFormat(const QString& formatstr, int* r, int* g, int* b)
{
    if (formatstr.length() != 7) {
        return;
    }
    
    QString temp = formatstr.mid(5, 2);
    *b = temp.toInt(nullptr, 16);
    temp = formatstr.mid(3, 2);
    *g = temp.toInt(nullptr, 16);
    temp = formatstr.mid(1, 2);
    *r = temp.toInt(nullptr, 16);
}

QColor getColorWithHTMLFormat(const QString& formatstr)
{
    QColor color;
    int r, g, b;

    getRGBWithHTMLFormat(formatstr, &r, &b, &g);
    
    color.setRgb(r, g, b);
    
    return color;
}

QColor getTextWindowColorWithHTMLFormat(const QString& formatstr)
{
    QColor color;
    int r, g, b, maxp, minp;
    
    getRGBWithHTMLFormat(formatstr, &r, &g, &b);

    if( r > g ){
        maxp = ( r > b ) ? r : b;
        minp = ( g < b ) ? g : b;
    } else {
        maxp = ( g > b ) ? g : b;
        minp = ( r < b ) ? r : b;
    }
    
    color.setRgb((r - minp), (g - minp), (b - minp), (255 - maxp));
    
    return color;
}

// この関数はResourceServerに内蔵した
/*
NSImage* getTransrateImage(bitmap,transmode)
NSBitmapImageRep *bitmap;
NSString *transmode;
{
    NSImage *image;
    NSImageRep *rep;
    NSData *maskData;

    image = [ [ [ NSImage alloc ] initWithSize:NSZeroSize ] autorelease ];
    [ image addRepresentation:bitmap ];
    if([ transmode isEqualToString:TYTRANSMODE_ALPHA ]){
        // マスク付き画像ファイル
        image = ConvertImagePackedAlpha(image);
    } else if ([ transmode isEqualToString:TYTRANSMODE_COPY ]){
        //image = [ [ [ NSImage alloc ] initWithData:aData ] autorelease ];
        ;
    } else if ([ transmode isEqualToString:TYTRANSMODE_LEFTTOP ]){
        image = ConvertImageTransrateLeftTop(image);
    } else if ([ transmode isEqualToString:TYTRANSMODE_MASK ]){
        // 関数化してあるのにTYResouceServerに依存しちゃった。あはは。
        maskData = [ [ TYResourceServer sharedServer ] getImage:[ transmode substringFromIndex:2 ] ];
        image = ConvertImageWithMaskImage([ [ [ NSImage alloc ] initWithData:aData ] autorelease ],
                                            [ [ [ NSImage alloc ] initWithData:maskData ] autorelease ]);
    } else {
        image = [ [ [ NSImage alloc ] initWithData:aData ] autorelease ];
    }
    
    // dpi情報が入っていると、pixelサイズにならないので補正
    rep = [ image bestRepresentationForDevice:nil ];
    [ rep setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];
    [ image setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];
    
     return image; 
}
*/

// いじくりまわし中のコードなのでちょっと未整理です、これ。
NSImage* ConvertImagePackedAlpha(srcImage)
NSImage *srcImage;
{
    if (srcImage.isNull())
        return QImage();
    
    int source_width = srcImage.width();
    int width = source_width / 2;  // 图像数据占一半，alpha通道数据占另一半
    int height = srcImage.height();
    
    QImage destImage(width, height, QImage::Format_ARGB32);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // 图像像素数据
            QRgb srcPixel = srcImage.pixel(x, y);
            // alpha通道数据：从x+width位置读取
            QRgb alphaPixel = srcImage.pixel(x + width, y);
            
            int alpha = 0xFF - qRed(alphaPixel);  // 使用红色通道作为alpha值
            
            destImage.setPixel(x, y, qRgba(
                qRed(srcPixel) * alpha / 0xFF,
                qGreen(srcPixel) * alpha / 0xFF,
                qBlue(srcPixel) * alpha / 0xFF,
                alpha
            ));
        }
    }
    
    return destImage;
}

QImage ConvertImageTransrateLeftTop(const QImage& srcImage)
{
    if (srcImage.isNull())
        return QImage();
        
    int width = srcImage.width();
    int height = srcImage.height();
    
    QImage converted = srcImage;
    if (converted.format() != QImage::Format_ARGB32 && 
        converted.format() != QImage::Format_RGB32) {
        converted = converted.convertToFormat(QImage::Format_ARGB32);
    }
    
    // 获取左上角像素作为透明色
    QRgb trPixel = converted.pixel(0, 0);
    unsigned char trR = qRed(trPixel);
    unsigned char trG = qGreen(trPixel);
    unsigned char trB = qBlue(trPixel);
    
    QImage destImage(width, height, QImage::Format_ARGB32);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = converted.pixel(x, y);
            
            if (qRed(pixel) != trR || qGreen(pixel) != trG || qBlue(pixel) != trB) {
                destImage.setPixel(x, y, qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), 0xFF));
            } else {
                destImage.setPixel(x, y, qRgba(0, 0, 0, 0));
            }
        }
    }
    
    return destImage;
}

QImage ConvertImageWithMaskImage(const QImage& srcImage, const QImage& maskImage)
{
    if (srcImage.isNull() || maskImage.isNull())
        return QImage();
        
    if (srcImage.size() != maskImage.size()) {
        qWarning() << "Not Equal size of maskImage to sourceImage!!\n";
        return QImage();
    }
    
    QImage convertedSrc = srcImage;
    QImage convertedMask = maskImage;
    if (convertedSrc.format() != QImage::Format_ARGB32 && convertedSrc.format() != QImage::Format_RGB32) {
        convertedSrc = convertedSrc.convertToFormat(QImage::Format_ARGB32);
    }
    if (convertedMask.format() != QImage::Format_ARGB32 && convertedMask.format() != QImage::Format_RGB32) {
        convertedMask = convertedMask.convertToFormat(QImage::Format_ARGB32);
    }
    
    int width = convertedSrc.width();
    int height = convertedSrc.height();
    QImage destImage(width, height, QImage::Format_ARGB32);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb srcPixel = convertedSrc.pixel(x, y);
            QRgb maskPixel = convertedMask.pixel(x, y);
            
            int alpha = 0xFF - (qRed(maskPixel) + qGreen(maskPixel) + qBlue(maskPixel)) / 3;
            
            destImage.setPixel(x, y, qRgba(
                qRed(srcPixel) * alpha / 0xFF,
                qGreen(srcPixel) * alpha / 0xFF,
                qBlue(srcPixel) * alpha / 0xFF,
                alpha
            ));
        }
    }
    
    return destImage;
}

QImage blankBitmap(int width, int height)
{
    return QImage(width, height, QImage::Format_RGB32);
}

QString TYTransmodeSymbolFromString(const QString& key)
{
    static QMap<QString, QString> dict;

    if (dict.isEmpty()) {
        dict.insert("leftup", TYTRANSMODE_LEFTTOP);
        dict.insert("rightup", TYTRANSMODE_RIGHTTOP);
        dict.insert("copy", TYTRANSMODE_COPY);
        dict.insert("alpha", TYTRANSMODE_ALPHA);
    }

    return dict.value(key.toLower(), QString());
}

namespace TYImageUtil {

void convertMonocroWithSample(QImage& image, const unsigned char* sample, TYNegaPriority priority)
{
    if (priority || sample) {
        if (image.format() != QImage::Format_ARGB32 &&
            image.format() != QImage::Format_RGB32) {
            image = image.convertToFormat(QImage::Format_ARGB32);
        }
    }
    
    if (image.isNull())
        return;

    if (priority == TYNegaLowPriority) {
        nega(image);
    }
        
    if (sample) {
        monocro(image, sample);
    }
        
    if (priority == TYNegaHighPriority) {
        nega(image);
    }
}

void monocro(QImage& image, const unsigned char* sample)
{
    if (image.isNull())
        return;
        
    int width = image.width();
    int height = image.height();
    
    QImage converted = image;
    if (converted.format() != QImage::Format_ARGB32 && converted.format() != QImage::Format_RGB32) {
        converted = converted.convertToFormat(QImage::Format_ARGB32);
    }
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = converted.pixel(x, y);
            unsigned char current = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
            image.setPixel(x, y, qRgb(
                sample[0] * current / 0xFF,
                sample[1] * current / 0xFF,
                sample[2] * current / 0xFF
            ));
        }
    }
}

void nega(QImage& image)
{
    if (image.isNull())
        return;
        
    int width = image.width();
    int height = image.height();
    
    QImage converted = image;
    if (converted.format() != QImage::Format_ARGB32 && converted.format() != QImage::Format_RGB32) {
        converted = converted.convertToFormat(QImage::Format_ARGB32);
    }
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = converted.pixel(x, y);
            image.setPixel(x, y, qRgb(
                0xFF - qRed(pixel),
                0xFF - qGreen(pixel),
                0xFF - qBlue(pixel)
            ));
        }
    }
}

QList<QImage> horizontalDivide(const QImage& image, int count)
{
    QList<QImage> array;
    
    if (image.isNull() || count <= 0)
        return array;
    
    int width = image.width();
    int piece_width = width / count;
    
    if (piece_width == 0)
        piece_width = 1;
        
    int height = image.height();
    
    QImage converted = image;
    if (converted.format() != QImage::Format_ARGB32 && converted.format() != QImage::Format_RGB32) {
        converted = converted.convertToFormat(QImage::Format_RGB32);
    }
    
    for (int i = 0; i < count; i++) {
        QImage destBitmap(piece_width, height, QImage::Format_RGB888);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < piece_width; x++) {
                int srcX = piece_width * i + x;
                if (srcX < width) {
                    QRgb pixel = converted.pixel(srcX, y);
                    destBitmap.setPixel(x, y, qRgb(qRed(pixel), qGreen(pixel), qBlue(pixel)));
                }
            }
        }
        
        array.append(destBitmap);
    }
    
    return array;
}

void resizeToPixelsSize(QImage& image)
{
    // 将图像大小调整为像素尺寸（用于DPI设置后恢复原始像素大小）
    int pixelWidth = image.width();
    int pixelHeight = image.height();
    
    // 如果图像被缩放过，则重新调整回原始像素大小
    if (image.width() != pixelWidth || image.height() != pixelHeight) {
        image = image.scaled(pixelWidth, pixelHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

} // namespace TYImageUtil
