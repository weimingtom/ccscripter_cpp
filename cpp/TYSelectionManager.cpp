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

#include "TYSelectionManager.h"
#include "TYCellImage.h"
#include "TYStageManager.h"

#include <QPainter>

TYSelectionManager::TYSelectionManager(QObject* parent)
    : TYRespondRectManager(parent)
{
}

TYSelectionManager::TYSelectionManager(TYStageManager* manager, QObject* parent)
    : TYRespondRectManager(manager, parent)
{
}

TYSelectionManager::~TYSelectionManager()
{
}

void TYSelectionManager::initialButtonImage()
{
    // 全てのボタンを描く
    QPainter painter(&m_drawBuffer);
    painter.setCompositionMode(QPainter::CompositionMode_Source);

    for (TYCellImage* temp : m_btnArray) {
        if (temp && temp->currentImage()) {
            painter.drawImage(0, 0, *temp->currentImage());
        }
    }

    if (m_stageManager) {
        QImage* compositeLayer = m_stageManager->compositeLayer();
        if (compositeLayer) {
            QPainter layerPainter(compositeLayer);
            layerPainter.drawImage(0, 0, m_drawBuffer);
        }
    }

    m_beforeSelection = 0;
}

bool TYSelectionManager::changeSelection(int index)
{
    if (m_beforeSelection == index)
        return false;

    QPainter painter(&m_drawBuffer);
    painter.fillRect(QRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT), Qt::transparent);

    int count = m_btnArray.size();
    for (int i = 0; i < count; i++) {
        TYCellImage* temp = m_btnArray.at(i);
        
        if(i == index-1){
            if (temp) { temp->changeCell(1); }
        } else {
            if (temp) { temp->changeCell(0); }
        }
        
            if (temp->currentImage()) { painter.drawImage(0, 0, *temp->currentImage()); }
        }
    }

    if (m_stageManager) {
        QImage* compositeLayer = m_stageManager->compositeLayer();
        if (compositeLayer) {
            QPainter layerPainter(compositeLayer);
            layerPainter.drawImage(0, 0, m_drawBuffer);
        }
    }

    m_beforeSelection = index;
    
    return true;
}