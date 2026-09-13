//
//  TYControllButton.m
//  Tukuyomi
//
//  Created by toveta on Fri Feb 07 2003.
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
 
#include "TYControllButton.h"
#include "TYCellImage.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>

TYControllButton::TYControllButton(const QRect& frame, TYCellImage* cellImage, QWidget* parent)
    : QWidget(parent)
    , m_cellImage(cellImage)
    , m_currentCell(0)
    , m_target(nullptr)
    , m_tag(0)
    , m_trackingTag(-1)
{
    setGeometry(frame);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

TYControllButton::~TYControllButton()
{
}

void TYControllButton::changeCell(int aIndex)
{
    m_currentCell = aIndex;
    update();
}

void TYControllButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (m_cellImage) {
        QImage* img = m_cellImage->imageAt(m_currentCell);
        if (img) {
            painter.drawImage(0, 0, *img);
        }
    }
}

void TYControllButton::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if (m_target && !m_action.isEmpty()) {
        QMetaObject::invokeMethod(m_target, m_action.toUtf8().constData(),
                                 Q_ARG(QObject*, this));
    } else if (parent()) {
        QCoreApplication::sendEvent(parent(), event);
    }
}
