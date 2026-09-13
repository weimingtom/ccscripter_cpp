//
//  TYRespondRectManager.m
//  Tukuyomi
//
//  Created by toveta on Tue Nov 06 2001.
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

#include "TYRespondRectManager.h"
#include "TYCellImage.h"

#include <QPainter>

TYRespondRectManager::TYRespondRectManager(QObject* parent)
    : QObject(parent)
    , m_stageManager(nullptr)
    , m_beforeSelection(-1)
{
    m_drawBuffer = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);
    m_rectMap = QByteArray(VSCREEN_WIDTH * VSCREEN_HEIGHT, 0);
}

TYRespondRectManager::TYRespondRectManager(TYStageManager* manager, QObject* parent)
    : QObject(parent)
    , m_stageManager(manager)
    , m_beforeSelection(-1)
{
    m_drawBuffer = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);
    m_rectMap = QByteArray(VSCREEN_WIDTH * VSCREEN_HEIGHT, 0);
}

TYRespondRectManager::~TYRespondRectManager()
{
}

void TYRespondRectManager::setStageManager(TYStageManager* manager)
{
    m_stageManager = manager;
}

void TYRespondRectManager::addSelection(TYCellImage* aButton, const QRect& rect)
{
    unsigned char* map = reinterpret_cast<unsigned char*>(m_rectMap.data());
    int index = m_btnArray.size() + 1; // Index is 1-based

    // cellImageを配列に突っ込む。
    m_btnArray.append(aButton);

    // rectMapに登録
    QRect clippedRect = rect.intersected(QRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT));
    for (int i = 0; i < clippedRect.height(); i++) {
        unsigned char* ptr = map + clippedRect.left() + (clippedRect.top() + i) * VSCREEN_WIDTH;
        for (int j = 0; j < clippedRect.width(); j++, ptr++) {
            *ptr = index;
        }
    }
}

void TYRespondRectManager::initialButtonImage()
{
    return;
}

int TYRespondRectManager::selectedButtonID()
{
    return m_beforeSelection; // デフォルトではindex=idNo;
}

int TYRespondRectManager::selectedIndex()
{
    return m_beforeSelection; // 常にindexを返す。
}

int TYRespondRectManager::incToIndex()
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

int TYRespondRectManager::decToIndex()
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

QByteArray TYRespondRectManager::rectMap() const
{
    return m_rectMap;
}

bool TYRespondRectManager::changeSelection(int index)
{
    if (m_beforeSelection == index) {
        return false;
    }

    m_beforeSelection = index;
    return true;
}

void TYRespondRectManager::draw(QPainter* painter)
{
    if (!painter) return;
    painter->drawImage(0, 0, m_drawBuffer);
}

void TYRespondRectManager::clear()
{
    // 清除按钮池的引用
    m_btnArray.clear();

    // 重置绘制缓冲区
    m_drawBuffer = QImage(VSCREEN_WIDTH, VSCREEN_HEIGHT, QImage::Format_ARGB32);

    m_beforeSelection = -1;

    // 清除矩形映射
    memset(m_rectMap.data(), 0, static_cast<size_t>(VSCREEN_WIDTH) * VSCREEN_HEIGHT);
}