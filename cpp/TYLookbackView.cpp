//
//  TYLookbackView.m
//  Tukuyomi
//
//  Created by toveta on Thu Mar 06 2003.
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

#include "TYLookbackView.h"
#include "TYLookbackLayer.h"
#include "TYMainController.h"
#include "TYStageManager.h"
#include "TYKeyBind.h"
#include "TYEnviroment.h"
#include "TYCellImage.h"
#include "TYControllButton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

TYLookbackView::TYLookbackView(QWidget* parent)
    : QWidget(parent)
    , m_controller(nullptr)
    , m_drawBuffer(nullptr)
    , m_index(0)
{
    m_button[0] = nullptr;
    m_button[1] = nullptr;
    m_rectTag[0] = -1;
    m_rectTag[1] = -1;

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

TYLookbackView::~TYLookbackView()
{
    if (m_drawBuffer) {
        delete m_drawBuffer;
    }
}

bool TYLookbackView::initWithFrame(const QRect& aFrame,
                                    QObject* aObj,
                                    const QMap<QString, QVariant>& aDict,
                                    const QMap<QString, QVariant>& layoutDict,
                                    const QVector<QImage*>& buffer,
                                    TYCellImage* uImage,
                                    TYCellImage* dImage)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(layoutDict);

    TYStageManager* manager = TYStageManager::sharedManager();

    m_controller = aObj;
    m_attDict = aDict;
    m_lookbackArray = buffer;

    // Create draw buffer
    if (manager && manager->visualLayer()) {
        m_drawBuffer = new QImage(*manager->visualLayer());
        manager->drawTextWindow();
    }

    // Create buttons
    if (manager) {
        QRect twrect = manager->textWindowRect();

        if (uImage) {
            m_btnRect[0] = QRect(
                twrect.right() - uImage->size().width(),
                twrect.bottom() - uImage->size().height(),
                uImage->size().width(),
                uImage->size().height()
            );
            m_button[0] = new TYControllButton(m_btnRect[0], uImage);
            m_button[0]->setTarget(this);
            m_button[0]->setAction("execButton");
            addChildWidget(m_button[0]);
        }

        if (dImage) {
            m_btnRect[1] = QRect(
                twrect.right() - dImage->size().width(),
                twrect.top(),
                dImage->size().width(),
                dImage->size().height()
            );
            m_button[1] = new TYControllButton(m_btnRect[1], dImage);
            m_button[1]->setTarget(this);
            m_button[1]->setAction("execButton");
            addChildWidget(m_button[1]);
        }
    }

    setPageIndex(buffer.size() - 1);

    return true;
}

void TYLookbackView::setPageIndex(int aInt)
{
    m_index = aInt;

    TYStageManager* manager = TYStageManager::sharedManager();
    if (manager && m_index >= 0 && m_index < m_lookbackArray.size()) {
        // Setup layer for this index
    }

    // Show/hide buttons based on page
    if (m_button[0]) {
        if (m_index == 0) {
            m_button[0]->hide();
        } else {
            m_button[0]->show();
        }
    }

    if (m_button[1]) {
        if (m_index == m_lookbackArray.size() - 1) {
            m_button[1]->hide();
        } else {
            m_button[1]->show();
        }
    }
}

void TYLookbackView::updatesTrackingRect()
{
    if (m_button[0]) {
        m_button[0]->installEventFilter(this);
    }
    if (m_button[1]) {
        m_button[1]->installEventFilter(this);
    }
}

void TYLookbackView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (m_drawBuffer) {
        painter.drawImage(event->rect(), *m_drawBuffer, event->rect());
    }

    if (event->rect() == QRect(0, 0, VSCREEN_WIDTH, VSCREEN_HEIGHT)) {
        if (m_index >= 0 && m_index < m_lookbackArray.size()) {
            QImage* img = m_lookbackArray[m_index];
            if (img) {
                painter.drawImage(0, 0, *img);
            }
        }
    }
}

void TYLookbackView::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void TYLookbackView::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void TYLookbackView::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0) {
        up_action();
    } else {
        down_action();
    }
}

void TYLookbackView::keyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event);
}

void TYLookbackView::down_action()
{
    if (m_index + 1 >= m_lookbackArray.size()) {
        endMode();
        return;
    }

    setPageIndex(m_index + 1);
    update();
}

void TYLookbackView::up_action()
{
    if (m_index - 1 < 0) {
        return;
    }

    setPageIndex(m_index - 1);
    update();
}

void TYLookbackView::cancel_action(QObject* sender)
{
    Q_UNUSED(sender);
    endMode();
}

void TYLookbackView::execButton(QObject* sender)
{
    if (sender == m_button[0]) {
        up_action();
    } else {
        down_action();
    }
}

void TYLookbackView::endMode()
{
    if (window()) {
        window()->activateWindow();
    }
    if (parentWidget()) {
        parentWidget()->setFocus();
    }

    if (m_controller) {
        static_cast<TYMainController*>(m_controller)->exitSystemModeResumeStatus(true);
    }

    hide();
    deleteLater();
}
