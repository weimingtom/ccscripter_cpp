//
//  TYVisualNovelView.h
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

#ifndef TYVISUALNOVELVIEW_H
#define TYVISUALNOVELVIEW_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPoint>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class TYController;

class TYVisualNovelView : public QWidget
{
    Q_OBJECT
private:
    TYController* m_target;
    QImage* m_sourceImage;
    QImage* m_directDrawImage;
    QRect m_directInRect;
    QRect m_directFromRect;
    bool m_directDraw;

    float m_scale;
    bool m_isScale;
	
public:

//-(void)setMouseDownSelector:(SEL)aSelector;
    void setTarget(TYController* aTarget);

//-(void)mouseDownSelector:(SEL)

// Override NSView
//- (BOOL)isFlipped;

// emulation NSImageView 注意：このViewはimageを参照で保持する。
    QImage* image();
    void setImage(QImage* image);

// Direct Drawing(and force Display)
    void directDrawImage(QImage* image, const QRect& inRect, const QRect& fromRect);

// set scaling(not use affinTransform)
    void setScaling(float s);
    bool isScaling() const;
    float scale() const;
    void setNeedsDisplayScalingRect(const QRect& sRect);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public:
    explicit TYVisualNovelView(QWidget* parent = nullptr) : QWidget(parent)
    , m_target(nullptr)
    , m_sourceImage(nullptr)
    , m_directDrawImage(nullptr)
    , m_directInRect()
    , m_directFromRect()
    , m_directDraw(false)
    , m_scale(1.0f)
    , m_isScale(false)
    {
         setMouseTracking(true);
         setFocusPolicy(Qt::StrongFocus);
    }
    ~TYVisualNovelView() {}
};

#endif // TYVISUALNOVELVIEW_H
