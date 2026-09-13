//
//  TYVisualNovelView.m
//  Tukuyomi
//
//  Created by toveta on Tue Sep 25 2001.
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

#include "TYVisualNovelView.h"
#include "TYMainController.h"
#include "TYKeyBind.h"

#include <QPainter>
#include <QApplication>

inline void pointDiv(QPoint& p, float s) { p.setX(static_cast<int>(p.x() / s)); p.setY(static_cast<int>(p.y() / s)); }
inline void rectDiv(QRect& r, float s) { r.setX(static_cast<int>(r.x() / s)); r.setY(static_cast<int>(r.y() / s)); r.setWidth(static_cast<int>(r.width() / s)); r.setHeight(static_cast<int>(r.height() / s)); }
inline void rectMul(QRect& r, float s) { r.setX(static_cast<int>(r.x() * s)); r.setY(static_cast<int>(r.y() * s)); r.setWidth(static_cast<int>(r.width() * s)); r.setHeight(static_cast<int>(r.height() * s)); }



/*
-(void)setMouseDownSelector:(SEL)aSelector
{
    TYMouseDownSelector = aSelector;
}
*/

/*
- (id)initWithFrame:(NSRect)frameRect
{    
    [ super initWithFrame:frameRect ];
    return self;
}
*/

void TYVisualNovelView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect rect = event->rect();

    if (m_directDraw) {
        if (rect == m_directInRect) {
            painter.drawImage(m_directInRect, *m_directDrawImage, m_directFromRect);
            m_directDraw = false;
            delete m_directDrawImage;
            m_directDrawImage = nullptr;
            return;
        }
    }

    if (m_isScale) {
        if (m_sourceImage) {
            QRect fRect = rect;
            rectDiv(fRect, m_scale);
            painter.drawImage(rect, *m_sourceImage, fRect);
            //[ sourceImage compositeToPoint:rect.origin fromRect:rect operation:NSCompositeCopy ];
        } else {
            painter.fillRect(rect, Qt::black);
        }
    } else {
        if (m_sourceImage) {
            QRect sourceRect(rect.x(), rect.y(), rect.width(), rect.height());
            painter.drawImage(rect.topLeft(), *m_sourceImage, sourceRect);
        } else {
            painter.fillRect(rect, Qt::black);
        }
    }

    if (m_directDraw) {
        painter.drawImage(m_directInRect, *m_directDrawImage, m_directFromRect);
        m_directDraw = false;
        delete m_directDrawImage;
        m_directDrawImage = nullptr;
    }
}


/*
- (BOOL)isFlipped
{
    return YES;
}
*/

QImage* TYVisualNovelView::image()
{
    return m_sourceImage;
}

void TYVisualNovelView::setImage(QImage* image)
{
    m_sourceImage = image;
}

void TYVisualNovelView::directDrawImage(QImage* image, const QRect& inRect, const QRect& fromRect)
{
    /*
    [ self lockFocus ];
    [ image drawInRect:inRect fromRect:fromRect operation:NSCompositeCopy fraction:1.0 ];
    [ self unlockFocus ];

    [ [ NSGraphicsContext graphicsContextWithWindow:[ self window ] ] flushGraphics ];
    return ;
     */
    
    m_directDraw = true;
    m_directDrawImage = new QImage(*image);
    QRect scaledInRect = inRect;
    if (m_isScale) {
        rectMul(scaledInRect, m_scale);
    }
    m_directInRect = scaledInRect;
    m_directFromRect = fromRect;
    update(m_directInRect);
}

void TYVisualNovelView::setTarget(TYController* aTarget)
{
    m_target = aTarget;
}

void TYVisualNovelView::setScaling(float s)
{
    m_isScale = (s != 1.0f);
    m_scale = s;
}
bool TYVisualNovelView::isScaling() const { return m_isScale; }
float TYVisualNovelView::scale() const { return m_scale; }

void TYVisualNovelView::setNeedsDisplayScalingRect(const QRect& sRect)
{
    QRect scaledRect = sRect;
    if (m_isScale) {
        rectMul(scaledRect, m_scale);
    }
    update(scaledRect);
}

void TYVisualNovelView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
            if (m_target) m_target->cancel_action(this);
        } else {
            if (m_target) m_target->select_action();
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_target) m_target->cancel_action(this);
    }
}
void TYVisualNovelView::keyPressEvent(QKeyEvent* event)
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QString selector = TYSelectorForKeycode(event->key());
    if (!selector.isEmpty() && m_target) {
        if (selector == "select_action") m_target->select_action();
        else if (selector == "up_action") m_target->up_action();
        else if (selector == "down_action") m_target->down_action();
        else m_target->other_action(nullptr);
    }
}
void TYVisualNovelView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isScale) {
        QPoint point = event->pos();
        pointDiv(point, m_scale);
        if (m_target) m_target->move_mouse(point);
    } else {
        if (m_target) m_target->move_mouse(event->pos());
    }
}

void TYVisualNovelView::wheelEvent(QWheelEvent* event)
{
    QPoint delta = event->angleDelta();
    if (delta.y() > 0) {
        if (m_target) m_target->up_action();
    } else {
        if (m_target) m_target->down_action();
    }
}


