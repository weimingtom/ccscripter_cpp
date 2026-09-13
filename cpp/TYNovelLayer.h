//
//  TYNovelLayer.h
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

#ifndef TYNOVELLAYER_H
#define TYNOVELLAYER_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QVector>
#include <QMetaMethod>

class TYLookbackLayer;

class TYNovelLayer : public QObject {
    Q_OBJECT
public:
    explicit TYNovelLayer(QObject* parent = nullptr);
    virtual ~TYNovelLayer();

// Making and Settings
    void initNovelLayer();
    void setManager(QObject* manager);

    void pushAttributedString(const QVariant& str);
    void pushNewLine();
    TYLookbackLayer* lookbackObject();
    QVariantList stringArray();
    QVariantList voiceArray();

    QString getText();

    void drawImage();

    bool respondsToSelector(const char* selector) const;
    void forwardInvocation(void* invocation);

signals:
    void voicePlay(const QVariant& notification);

public slots:
    void playVoice(const QVariant& notification);

protected:
    QObject* m_delegate;
    QImage* m_bufferImage; // このイメージにメッセージが転送される。
    QVariantList m_textArray;
    QString m_tempString;
    bool m_needRefresh;
    bool m_isDrawTextWindow;
    int m_nowColumn;
    int m_nowRow;
    QVariantList m_voiceArray; // encoding from version 2

    static const QString PathVoiceLookbackData;
    static const QString PointVoiceLookbackData;
};

#endif // TYNOVELLAYER_H
