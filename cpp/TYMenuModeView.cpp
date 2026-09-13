//
//  TYMenuModeView.m
//  Tukuyomi
//
//  Created by toveta on Thu Feb 06 2003.
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

#include "TYMenuModeView.h"
#include "TYControllButton.h"
#include "TYKeyBind.h"
#include "TYMainController.h"
#include "TYVisualNovelView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QHoverEvent>
#include <QApplication>

TYMenuModeView::TYMenuModeView(QWidget* parent)
    : QWidget(parent)
    , m_controller(nullptr)
    , m_selectedButton(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
}

TYMenuModeView::~TYMenuModeView()
{
    removeAllTrackingRect();
}

void TYMenuModeView::setController(QObject* aObj,
                                    const QVector<TYControllButton*>& bArray,
                                    const QVector<QString>& aArray,
                                    const QColor& aColor)
{
    m_controller = static_cast<TYController*>(aObj);
    m_selectedButton = nullptr;
    m_btnArray = bArray;
    m_actArray = aArray;
    m_menuWindowColor = aColor;
}

void TYMenuModeView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), m_menuWindowColor);
}

void TYMenuModeView::updatesTrackingRect()
{
    removeAllTrackingRect();

    for (TYControllButton* temp : m_btnArray) {
        m_trackingButtons[temp] = false;
    }
}

void TYMenuModeView::removeAllTrackingRect()
{
    if (m_selectedButton != nullptr) {
        m_selectedButton->changeCell(0);
        m_selectedButton->update();
    }

    m_selectedButton = nullptr;
    m_trackingButtons.clear();
}

void TYMenuModeView::setButtonArray(const QVector<TYControllButton*>& aArray)
{
    m_btnArray = aArray;
}

void TYMenuModeView::setActionArray(const QVector<QString>& aArray)
{
    m_actArray = aArray;
}

void TYMenuModeView::execButton(QObject* sender)
{
    Q_UNUSED(sender);
    select_action();
}

void TYMenuModeView::mouseEntered(TYControllButton* btn)
{
    if (!btn) return;

    btn->changeCell(1);
    btn->update();

    if (m_selectedButton && (btn != m_selectedButton)) {
        m_selectedButton->changeCell(0);
    }

    m_selectedButton = btn;
}

void TYMenuModeView::mouseExited(TYControllButton* btn)
{
    if (!btn) return;

    btn->changeCell(0);
    btn->update();

    if (m_selectedButton == btn) {
        m_selectedButton = nullptr;
    }
}

void TYMenuModeView::mouseMoveEvent(QMouseEvent* event)
{
    // Determine which button the mouse is over (tracking rect equivalent)
    QPoint pos = event->pos();
    TYControllButton* hoveredBtn = nullptr;

    for (TYControllButton* btn : m_btnArray) {
        QWidget* btnWidget = qobject_cast<QWidget*>(btn);
        if (btnWidget && btnWidget->isVisible()) {
            QRect btnRect = btnWidget->geometry();
            if (btnRect.contains(pos)) {
                hoveredBtn = btn;
                break;
            }
        }
    }

    // Update tracking state
    for (auto it = m_trackingButtons.begin(); it != m_trackingButtons.end(); ++it) {
        TYControllButton* btn = it.key();
        bool wasInside = it.value();
        bool isInside = (btn == hoveredBtn);

        if (!wasInside && isInside) {
            it.value() = true;
            mouseEntered(btn);
        } else if (wasInside && !isInside) {
            it.value() = false;
            mouseExited(btn);
        }
    }
}

void TYMenuModeView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        cancel_action(nullptr);
    }
}

void TYMenuModeView::keyPressEvent(QKeyEvent* event)
{
    QString selector = TYSelectorForKeycode(event->key());
    if (selector == "select_action") {
        select_action();
    } else if (selector == "cancel_action") {
        cancel_action(nullptr);
    } else if (selector == "up_action") {
        up_action();
    } else if (selector == "down_action") {
        down_action();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void TYMenuModeView::wheelEvent(QWheelEvent* event)
{
    QPoint angleDelta = event->angleDelta();
    if (angleDelta.y() > 0) {
        up_action();
    } else if (angleDelta.y() < 0) {
        down_action();
    }
}

bool TYMenuModeView::event(QEvent* event)
{
    if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) {
        // Handled via mouseMoveEvent tracking
        return true;
    }
    return QWidget::event(event);
}

void TYMenuModeView::down_action()
{
    TYControllButton* btn;
    int idx;

    if (m_selectedButton == nullptr) {
        idx = -1;
    } else {
        idx = m_btnArray.indexOf(m_selectedButton);
    }

    if (idx == m_btnArray.size() - 1 || idx == -1) {
        btn = m_btnArray.first();
    } else {
        btn = m_btnArray[idx + 1];
    }

    if (m_selectedButton != btn) {
        if (m_selectedButton) {
            m_selectedButton->changeCell(0);
            m_selectedButton->update();
        }
        btn->changeCell(1);
        btn->update();
        m_selectedButton = btn;
    }
}

void TYMenuModeView::select_action()
{
    if (!m_selectedButton) {
        return;
    }

    int idx = m_btnArray.indexOf(m_selectedButton);
    endMode();

    if (m_controller) {
        // Execute system call
        QVector<QVariant> args;
        args << "systemcall" << m_actArray[idx];
        static_cast<TYMainController*>(m_controller)->ty_systemcall(args);
    }
}

void TYMenuModeView::cancel_action(QObject* sender)
{
    Q_UNUSED(sender);
    /*
    [ controller exitSystemModeResumeStatus:YES ];
    [ self removeFromSuperview ];    
     */
    endMode();
}

void TYMenuModeView::up_action()
{
    TYControllButton* btn;
    int idx;

    if (m_selectedButton == nullptr) {
        idx = -1;
    } else {
        idx = m_btnArray.indexOf(m_selectedButton);
    }

    if(idx == 0 || idx == -1){
        btn = m_btnArray.last();
    } else {
        btn = m_btnArray[idx - 1];
    }

    if (m_selectedButton != btn) {
        if (m_selectedButton) {
            m_selectedButton->changeCell(0);
            m_selectedButton->update();
        }
        btn->changeCell(1);
        btn->update();
        m_selectedButton = btn;
    }
}

void TYMenuModeView::endMode()
{
    // Make parent view the first responder
    if (parentWidget()) {
        parentWidget()->setFocus();
    }

    removeAllTrackingRect();

    if (m_controller) {
        static_cast<TYMainController*>(m_controller)->exitSystemModeResumeStatus(true);
    }

    // Remove from superview (retain/autorelease replaced by deleteLater)
    hide();
    deleteLater();
}
