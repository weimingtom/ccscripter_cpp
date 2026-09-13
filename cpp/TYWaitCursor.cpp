//
//  TYWaitCursor.m
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

#include "TYWaitCursor.h"
#include <QPainter>
#include <QDebug>
#include <QDataStream>

extern int VSCREEN_HEIGHT;

TYWaitCursorDelegate::~TYWaitCursorDelegate() {}

TYWaitCursor::TYWaitCursor(const QString& aPath, const QPoint& point, bool absolute, QObject* parent)
    : QObject(parent)
    , m_delegate(nullptr)
    , m_targetView(nullptr)
    , m_targetPoint()
    , m_cursorSize()
    , m_resourcePath(aPath)
    , m_cursorImage()
    , m_offset(point)
    , m_isAbsolute(absolute)
    , m_isLoad(false)
    , m_bufferImage()
{
}

void TYWaitCursor::setDelegate(TYWaitCursorDelegate* aObject)
{
    m_delegate = aObject;
}

void TYWaitCursor::loadImage()
{
    if (m_isLoad)
        return;
    m_isLoad = true;

    if (m_resourcePath.isEmpty())
        return;

    // Load image from resource server with animation support
    // TYResourceServer::getImage(resourcePath, transMode=true, animate=true)
    // This would be a call to TYResourceServer_SharedServer()->getImage()
    // For now, we just track that we attempted to load

    if (m_cursorImage.isNull())
        return;

    // TYAnimationCellImage setDelegate (if animated)
    // if (m_cursorImage.isAnimate()) {
    //     ((TYAnimationCellImage*)m_cursorImage)->setDelegate(this);
    // }

    m_cursorSize = m_cursorImage.size();
    m_bufferImage = QImage(m_cursorSize, QImage::Format_ARGB32_Premultiplied);
}

QImage TYWaitCursor::compositeLayer()
{
    if (m_delegate) {
        return m_delegate->compositeLayer();
    }
    return QImage();
}

void TYWaitCursor::drawToView(QObject* aView, const QPoint& point)
{
    m_targetView = aView;

    loadImage();
    if (m_cursorImage.isNull())
        return;

    if (m_isAbsolute) {
        m_targetPoint = QPoint(static_cast<int>(m_offset.x()),
                               VSCREEN_HEIGHT - static_cast<int>(m_offset.y()) - m_cursorSize.height());
    } else {
        m_targetPoint = point;
        m_targetPoint.rx() += static_cast<int>(m_offset.x());
        m_targetPoint.ry() += -static_cast<int>(m_offset.y());
        m_targetPoint.ry() += -m_cursorSize.height();
    }


    // TYAnimationCellImage* animImg = (TYAnimationCellImage*)m_cursorImage;
    // if (animImg->isAnimate() && !animImg->isPlaying()) {
    //     animImg->play();
    // }

    drawImage(true);
}


void TYWaitCursor::didChangeCell(QObject* sender)
{
    drawImage(true);
}

void TYWaitCursor::drawImage(bool showCursor)
{
    if (m_bufferImage.isNull()) {
        m_bufferImage = QImage(m_cursorSize, QImage::Format_ARGB32_Premultiplied);
    }

    QImage offscreen = compositeLayer();

    QPainter bufferPainter(&m_bufferImage);
    bufferPainter.setCompositionMode(QPainter::CompositionMode_Source);
    bufferPainter.fillRect(0, 0, m_cursorSize.width(), m_cursorSize.height(), Qt::transparent);
    bufferPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QRectF inRect;
    if (m_isAbsolute) {
        inRect = QRectF(m_targetPoint.x(),
                        VSCREEN_HEIGHT - static_cast<int>(m_offset.y()) - m_cursorSize.height(),
                        m_cursorSize.width(),
                        m_cursorSize.height());
    } else {
        inRect = QRectF(m_targetPoint.x(), m_targetPoint.y(),
                        m_cursorSize.width(),
                        m_cursorSize.height());
    }
    // オフスクリーンを取得

    // バッファにコピー
    if (!offscreen.isNull()) {
        // Copy from offscreen to buffer at (0,0)
        bufferPainter.drawImage(QPointF(0, 0), offscreen, inRect);
    }

	// ウインドウに描画
    if (showCursor && !m_cursorImage.isNull()) { bufferPainter.drawImage(
	                                             QPoint(0, 0), 
                                                 m_cursorImage); }
    bufferPainter.end();
    //[ targetView setImage:offscreen ];

}

void TYWaitCursor::clear()
{
    if (m_cursorImage.isNull())
        return;

    stop();
    
    // targetView->setImage(compositeLayer());
    //[ self drawImage:NO ];
}

void TYWaitCursor::stop()
{
    // if (m_cursorImage && m_cursorImage->isAnimate() && m_cursorImage->isPlaying()) {
    //     m_cursorImage->stop();
    // }
}

void TYWaitCursor::resume()
{
    // if (m_cursorImage && m_cursorImage->isAnimate()) {
    //     m_cursorImage->resume();
    // }
}

TYWaitCursor：:~TYWaitCursor() 
{




}


QString TYWaitCursor::description() const
{
    return QString("path=%1 point=%2,%3 absolute=%4").arg(m_resourcePath).arg(static_cast<int>(m_offset.x())).arg(static_cast<int>(m_offset.y())).arg(m_isAbsolute ? 1 : 0);
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------

TYWaitCursor::TYWaitCursor(QDataStream& aDecoder, QObject* parent)
    : QObject(parent)
    , m_delegate(nullptr)
    , m_targetView(nullptr)
    , m_targetPoint()
    , m_cursorSize()
    , m_resourcePath()
    , m_cursorImage()
    , m_offset()
    , m_isAbsolute(false)
    , m_isLoad(false)
    , m_bufferImage()
{
    aDecoder >> m_resourcePath;
    aDecoder >> m_offset;
    aDecoder >> m_isAbsolute;
}

void TYWaitCursor::encodeWithCoder(QDataStream& aCoder) const
{
    aCoder << m_resourcePath;
    aCoder << m_offset;
    aCoder << m_isAbsolute;

}

