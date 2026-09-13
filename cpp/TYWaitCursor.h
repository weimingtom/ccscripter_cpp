//
//  TYWaitCursor.h
//  Tukuyomi
//
//  Created by toveta on Sat Mar 09 2002.
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

#ifndef TYWaitCursor_h
#define TYWaitCursor_h

#include <QObject>
#include <QString>
#include <QImage>
#include <QPoint>
#include <QSize>
#include <QDataStream>

class QDataStream;

class TYWaitCursorDelegate {
public:
    virtual ~TYWaitCursorDelegate() {}
    virtual QImage compositeLayer() = 0;
};

class TYWaitCursor : public QObject {
    Q_OBJECT
private:
    TYWaitCursorDelegate* m_delegate;
    QObject* m_targetView;
    QPoint m_targetPoint;
    QSize m_cursorSize;

    QString m_resourcePath;
    QImage m_cursorImage;
    QPoint m_offset;
    bool m_isAbsolute;
    bool m_isLoad;

    QImage m_bufferImage;
public:
    explicit TYWaitCursor(const QString& aPath, const QPoint& point, bool absolute, QObject* parent = nullptr);
    void setDelegate(TYWaitCursorDelegate* aObject);
    void loadImage();
    void drawToView(QObject* aView, const QPoint& point);
    void clear();
    void stop();
    void resume();

// implementation of delegate
    QImage compositeLayer();

private:
    void drawImage(bool showCursor);
	
public:
    ~TYWaitCursor() {}
    QString description() const;
    TYWaitCursor(QDataStream& aDecoder, QObject* parent = nullptr);
    void encodeWithCoder(QDataStream& aCoder) const;
};

#endif // TYWaitCursor_h
