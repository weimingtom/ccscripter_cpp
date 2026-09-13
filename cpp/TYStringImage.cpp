//
//  TYStringImage.m
//  Tukuyomi
//
//  Created by toveta on Wed May 08 2002.
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

#include "TYStringImage.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QTextLayout>
#include <QTextOption>

static QFont tempFont;
static QColor tempShadowColor = Qt::black;
static QPoint tempShadowOffset(SHADOW_TICKNESS, -SHADOW_TICKNESS);

TYStringImage::TYStringImage(QObject* parent) : QObject(parent), m_image(nullptr)
{
}

TYStringImage::TYStringImage(QImage* aImage, const QSize& aSize, const QPoint& point, QObject* parent)
    : QObject(parent)
    , m_image(aImage ? new QImage(*aImage) : nullptr)
    , m_size(aSize)
    , m_origin(-point)
{
}

QList<TYStringImage*> TYStringImage::imagesWithString(const QString& aStr,
    const QMap<QString, QVariant>& attributes,
    const QList<QColor>& colorsArray,
    int fontHeight,
    int fontWidth,
    int interval,
    bool shadow)
{
    QList<TYStringImage*> array;

    if (aStr.isNull() || aStr.isEmpty()) {
        // Return empty placeholder image
        QImage* placeholder = new QImage(1, 1, QImage::Format_ARGB32);
        placeholder->fill(0);
        array.append(new TYStringImage(placeholder, QSize(1, 1), QPoint(0, 0)));
        return array;
    }

    // Get font from attributes
    QFont font;
    if (attributes.contains("font")) {
        font = attributes["font"].value<QFont>();
    }
    int fontSize = (fontWidth > fontHeight) ? fontWidth : fontHeight;
    font.setPointSize(fontSize);

    // Calculate image size based on text
    QFontMetrics fm(font);
    int textWidth = fm.width(aStr);
    int textHeight = fm.height();

    int imageWidth = (fontWidth + interval) * (aStr.length() - 1) + textWidth;
    int imageHeight = textHeight;

    if (imageWidth <= 0) 
         imageWidth = 1;
    if (imageHeight <= 0) 
         imageHeight = 1;

    // Create images for each color
    for (const QColor& color : colorsArray) {
        QImage* newImage = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
        newImage->fill(0);

        QPainter painter(newImage);
        painter.setFont(font);
        painter.setPen(color);

        int offsetX = 0;
        int offsetY = 0;

        for (int i = 0; i < aStr.length(); i++) {
            QString charStr = aStr.mid(i, 1);
            int charWidth = fm.width(charStr);

            QPoint drawPoint(offsetX, fm.ascent());

            if (shadow) {
                // Draw shadow
                QPainter shadowPainter(newImage);
                shadowPainter.setFont(font);
                shadowPainter.setPen(tempShadowColor);
                shadowPainter.drawText(drawPoint.x() + SHADOW_TICKNESS,
                                       drawPoint.y() - SHADOW_TICKNESS,
                                       charStr);
            }

            // Draw main text
            painter.drawText(drawPoint, charStr);

            offsetX += fontWidth + interval;
        }

        painter.end();

        QPoint offset(0, -fm.descent());
        TYStringImage* stringImage = new TYStringImage(newImage, QSize(imageWidth, imageHeight), offset);
        array.append(stringImage);

        delete newImage; // stringImage makes its own copy
    }

    return array;
}



void TYStringImage::setOriginByValue(const QVariant& pointValue)
{
    if (pointValue.canConvert<QPoint>()) {
        m_origin = pointValue.value<QPoint>();
    }
}

void TYStringImage::dissolveToPoint(const QPoint& point, float fraction)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setOpacity(fraction);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image);
    painter.end();
}

void TYStringImage::dissolveToPoint(const QPoint& point, const QRect& rect, float fraction)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setOpacity(fraction);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image, rect);
    painter.end();
}

void TYStringImage::compositeToPoint(const QPoint& point, QPainter::CompositionMode op)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setCompositionMode(op);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image);
    painter.end();
}

void TYStringImage::compositeToPoint(const QPoint& point, const QRect& rect, QPainter::CompositionMode op)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setCompositionMode(op);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image, rect);
    painter.end();
}

void TYStringImage::compositeToPoint(const QPoint& point, QPainter::CompositionMode op, float delta)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setOpacity(delta);
    painter.setCompositionMode(op);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image);
    painter.end();
}

void TYStringImage::compositeToPoint(const QPoint& point, const QRect& rect, QPainter::CompositionMode op, float delta)
{
    if (!m_image) return;

    QPoint adjustedPoint = point + m_origin;
    QPainter painter;
    painter.setOpacity(delta);
    painter.setCompositionMode(op);
    painter.begin(nullptr);
    painter.drawImage(adjustedPoint, *m_image, rect);
    painter.end();
}

/* めんどいので保留（えー）
- (void)drawAtPoint:(NSPoint)point fromRect:(NSRect)fromRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)drawInRect:(NSRect)rect fromRect:(NSRect)fromRect operation:(NSCompositingOperation)op fraction:(float)delta;
*/

TYStringImage::~TYStringImage()
{
    if (m_image) {
        delete m_image;
        m_image = nullptr;
    }
}