//
//  TYButtonManager.m
//  Tukuyomi
//
//  Created by toveta on Thu Oct 11 2001.
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

#include "TYButtonManager.h"
#include "TYStageManager.h"
#include <QPainter>

TYButton::TYButton(QObject* parent)
    : QObject(parent)
    , m_unselectedImage(nullptr)
    , m_selectedImage(nullptr)
{
}

TYButton::~TYButton()
{
}

void TYButton::initWithID(const QNumber& no, QImage* unselImage, QImage* selImage, const QRect& aRect)
{
    m_idno = no;
    m_unselectedImage = unselImage;
    m_selectedImage = selImage;
    m_drawRect = aRect;
}

void TYButton::drawWithSelect(bool aBool)
{
    QPainter painter;

    // Clear rect with black
    QImage img(m_drawRect.size(), QImage::Format_ARGB32);
    img.fill(Qt::black);

    QImage* drawImage = aBool ? m_selectedImage : m_unselectedImage;
    if (drawImage) {
        painter.begin(&img);
        painter.drawImage(0, 0, *drawImage);
        painter.end();
    }
}

QString TYButton::description() const
{
    return QString("ID=%1, Rect=%2").arg(m_idno.toString()).arg(m_drawRect.toString());
}

// TYButtonManager implementation
TYButtonManager::TYButtonManager(QObject* parent)
    : QObject(parent)
    , m_stageManager(nullptr)
    , m_sourceImage(nullptr)
    , m_drawBuffer(nullptr)
    , m_beforeSelection(TYButtonUnInitialize)
{
}

TYButtonManager::~TYButtonManager()
{
    if (m_drawBuffer) {
        delete m_drawBuffer;
    }
}

void TYButtonManager::initWithImage(QImage* srcImage)
{
    m_sourceImage = srcImage;
    m_btnArray.clear();
    m_drawBuffer = new QImage(TYVirtualScreenSize(), QImage::Format_ARGB32);
    m_drawBuffer->fill(Qt::transparent);
    m_beforeSelection = TYButtonUnInitialize;

    int bufferSize = VSCREEN_WIDTH * VSCREEN_HEIGHT;
    m_rectMap.resize(bufferSize);
    m_rectMap.fill(0);
}

void TYButtonManager::addButton(const QNumber& no, int x, int y, int width, int height, int selx, int sely)
{
    QImage* unselImage = new QImage(width, height, QImage::Format_ARGB32);
    unselImage->fill(Qt::transparent);

    QImage* selImage = new QImage(width, height, QImage::Format_ARGB32);
    QRect rect(x, VSCREEN_HEIGHT - y - height, width, height);

    // Copy selected region from source
    if (m_sourceImage) {
        QPainter painter(selImage);
        QRect srcRect(selx, m_sourceImage->height() - height - sely, width, height);
        painter.drawImage(QPoint(0, 0), *m_sourceImage, srcRect);
        painter.end();
    }

    TYButton* btn = new TYButton(this);
    btn->initWithID(no, unselImage, selImage, rect);
    m_btnArray.append(btn);

    // Update rect map
    int index = m_btnArray.size();
    uchar* map = (uchar*)m_rectMap.data();

    for (int i = 0; i < rect.height(); i++) {
        for (int j = 0; j < rect.width(); j++) {
            int px = rect.x() + j;
            int py = rect.y() + i;
            if (px >= 0 && px < VSCREEN_WIDTH && py >= 0 && py < VSCREEN_HEIGHT) {
                map[px + py * VSCREEN_WIDTH] = index;
            }
        }
    }
}

int TYButtonManager::selectedButtonID()
{
    if (m_beforeSelection) {
        return m_btnArray[m_beforeSelection - 1]->idNo().toInt();
    }
    return 0;
}

int TYButtonManager::incToIndex()
{
    int count = m_btnArray.size();
    if (!count) {
        return 0;
    }

    int nowSelection = m_beforeSelection + 1;
    if (nowSelection > count) {
        return 1;
    }
    return nowSelection;
}

int TYButtonManager::decToIndex()
{
    int count = m_btnArray.size();
    if (!count) {
        return 0;
    }

    int nowSelection = m_beforeSelection - 1;
    if (nowSelection <= 0) {
        return count;
    }
    return nowSelection;
}

void TYButtonManager::initialButtonImage()
{
    if (m_beforeSelection == TYButtonUnInitialize) {
        for (TYButton* temp : m_btnArray) {
            temp->drawWithSelect(false);
        }
        m_beforeSelection = TYButtonNoSelected;
    }
}

void TYButtonManager::drawWithSelection(int status)
{
    QPainter painter;
    if (!m_drawBuffer) {
        return;
    }

    // Clear previous selection
    if (m_beforeSelection) {
        m_btnArray[m_beforeSelection - 1]->drawWithSelect(false);
    }

    // Draw current selection
    if (status) {
        m_btnArray[status - 1]->drawWithSelect(true);
    }

    m_beforeSelection = status;
}

QString TYButtonManager::description() const
{
    QString desc;
    for (const TYButton* btn : m_btnArray) {
        desc += btn->description() + "\n";
    }
    return desc;
}
