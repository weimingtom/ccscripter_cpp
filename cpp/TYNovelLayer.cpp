//
//  TYNovelLayer.m
//  Tukuyomi
//
//  Created by toveta on Mon Dec 31 2001.
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

#include "TYNovelLayer.h"
#include "TYStageManager.h"
#include "TYLookbackLayer.h"

const QString TYNovelLayer::PathVoiceLookbackData = "TYPathVoiceLookbackData";
const QString TYNovelLayer::PointVoiceLookbackData = "TYPointVoiceLookbackData";

TYNovelLayer::TYNovelLayer(QObject* parent)
    : QObject(parent)
    , m_delegate(nullptr)
    , m_bufferImage(nullptr)
    , m_needRefresh(true)
    , m_isDrawTextWindow(false)
    , m_nowColumn(0)
    , m_nowRow(0)
{
}

TYNovelLayer::~TYNovelLayer()
{
    if (m_bufferImage) {
        delete m_bufferImage;
    }
}

void TYNovelLayer::initNovelLayer()
{
    m_textArray.clear();
    m_bufferImage = new QImage(TYVirtualScreenSize(), QImage::Format_ARGB32);
    m_bufferImage->fill(Qt::transparent);
    m_tempString = "";
    m_voiceArray.clear();
    m_needRefresh = true;

    // Connect voice play notification
    // In Qt, we use signals/slots instead of NSNotificationCenter
}

void TYNovelLayer::recache()
{
    if (!m_needRefresh) {
        return;
    }
    m_needRefresh = false;

    // Draw to buffer image
    QPainter painter(m_bufferImage);
    drawImage();
}

void TYNovelLayer::setManager(QObject* manager)
{
    m_delegate = manager;
}

void TYNovelLayer::pushAttributedString(const QVariant& str)
{
    m_needRefresh = true;
    m_tempString.append(str.toString());
}

void TYNovelLayer::pushNewLine()
{
    m_needRefresh = true;

    m_textArray.append(m_tempString);
    m_tempString = "";
    m_nowColumn = 0;
}

TYLookbackLayer* TYNovelLayer::lookbackObject()
{
    return new TYLookbackLayer(m_textArray, m_voiceArray);
}

QVariantList TYNovelLayer::stringArray()
{
    QVariantList array;
    for (const QVariant& temp : m_textArray) {
        array.append(temp);
    }
    if (!m_tempString.isEmpty()) {
        array.append(m_tempString);
    }
    return array;
}

QVariantList TYNovelLayer::voiceArray()
{
    return m_voiceArray;
}

void TYNovelLayer::drawImage()
{
    int length, column, row;

    if (!m_isDrawTextWindow) {
        // Call delegate to draw text window
        QMetaObject::invokeMethod(m_delegate, "drawTextWindow");
        m_isDrawTextWindow = true;
    }

    column = 0;
    row = 0;
    for (const QVariant& temp : m_textArray) {
        if (m_nowRow <= row) {
            length = temp.toString().length();
            for (int i = 0; i < length; i++, column++) {
                QChar ch = temp.toString().at(i);
                QMetaObject::invokeMethod(m_delegate, "drawAttStr",
                    Q_ARG(QChar, ch),
                    Q_ARG(int, column),
                    Q_ARG(int, row));
            }
        }
        column = 0;
        row++;
    }

    length = m_tempString.length();
    column = m_nowColumn;
    for (int i = m_nowColumn; i < length; i++, column++) {
        QChar ch = m_tempString.at(i);
        QMetaObject::invokeMethod(m_delegate, "drawAttStr",
            Q_ARG(QChar, ch),
            Q_ARG(int, column),
            Q_ARG(int, row));
    }

    m_nowRow = row;
    m_nowColumn = column;
}

void TYNovelLayer::playVoice(const QVariant& notification)
{
    QVariantMap dict;
    dict.insert(PathVoiceLookbackData, notification);
    dict.insert(PointVoiceLookbackData, QPoint(m_tempString.length(), m_textArray.size()));
    m_voiceArray.append(dict);
}

QString TYNovelLayer::getText()
{
    QString result;
    for (const QVariant& str : m_textArray) {
        result.append(str.toString());
    }
    return result;
}

// implementation of NSCoding
void TYNovelLayer::save(QDataStream& out) const
{
    out << m_textArray;
    out << m_tempString;
    out << m_voiceArray;
}

void TYNovelLayer::load(QDataStream& in)
{
    in >> m_textArray;
    in >> m_tempString;
    in >> m_voiceArray;
    m_bufferImage = new QImage(TYVirtualScreenSize(), QImage::Format_ARGB32);
    m_needRefresh = true;
}
