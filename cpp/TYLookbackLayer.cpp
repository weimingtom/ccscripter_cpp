//
//  TYLookbackLayer.m
//  Tukuyomi
//
//  Created by toveta on Thu Mar 06 2003.
//  Copyright (c) 2003 toveta All rights reserved.
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

#include "TYLookbackLayer.h"
#include "TYStageManager.h"
#include <QPainter>
#include <QMetaMethod>

QImage TYLookbackLayer::lookBackBuffer;

TYLookbackLayer::TYLookbackLayer(QObject* parent)
    : TYNovelLayer(parent)
    , attributed(false)
{
}

TYLookbackLayer::~TYLookbackLayer()
{
}

void TYLookbackLayer::initWithStrings(const QVariantList& aArray, const QVariantList& vArray)
{
    m_textArray = aArray;
    m_voiceArray = vArray;
    m_tempString = QString();
    m_needRefresh = true;
}

void TYLookbackLayer::recache()
{
    m_nowColumn = 0;
    m_nowRow = 0;

    lookBackBuffer = QImage(800, 600, QImage::Format_ARGB32);

    QPainter painter(&lookBackBuffer);
    painter.setRenderHint(QPainter::Antialiasing);
    drawImage();
}

void TYLookbackLayer::drawImage()
{
    if (!m_delegate) {
        return;
    }

    // Use Qt's meta-object system to call delegate's drawAttStr method
    // Signature: void drawAttStr(const QString& str, int column, int row)
    const QMetaObject* meta = m_delegate->metaObject();
    int methodIndex = meta->indexOfMethod("drawAttStr");
    if (methodIndex < 0) {
        return;
    }
    QMetaMethod drawMethod = meta->method(methodIndex);

    int column = 0;
    int row = 0;
    for (int i = 0; i < m_textArray.size(); ++i) {
        if (m_nowRow <= row) {
            QVariant tempVar = m_textArray[i];
            QString temp;
            if (tempVar.canConvert<QString>()) {
                temp = tempVar.toString();
            }
            for (int loc = 0; loc < temp.length(); ++loc, ++column) {
                QString substring = temp.mid(loc, 1);
                drawMethod.invoke(m_delegate,
                                  Qt::DirectConnection,
                                  Q_ARG(QString, substring),
                                  Q_ARG(int, column),
                                  Q_ARG(int, row));
            }
        }
        column = 0;
        ++row;
    }
}

void TYLookbackLayer::setAttribute(const QMap<QString, QVariant>& aDict)
{
    Q_UNUSED(aDict);
    if (attributed) {
        return;
    }
    attributed = true;
}

QVariantList TYLookbackLayer::stringArray() const
{
    if (attributed) {
        return TYNovelLayer::stringArray();
    } else {
        return m_textArray;
    }
}

// implementation of NSCoding
void TYLookbackLayer::encodeWithCoder(QDataStream& aCoder)
{
    TYNovelLayer::save(aCoder);
}

void TYLookbackLayer::initWithCoder(QDataStream& aDecoder)
{
    TYNovelLayer::load(aDecoder);
    m_needRefresh = true;
}
