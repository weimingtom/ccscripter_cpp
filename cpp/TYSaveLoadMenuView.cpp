//
//  TYSaveLoadMenuView.m
//  Tukuyomi
//
//  Created by toveta on Sun Mar 02 2003.
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

#include "TYSaveLoadMenuView.h"
#include "TYStringImage.h"
#include "TYCellImage.h"
#include "TYStageManager.h"
#include "TYControllButton.h"

TYSaveLoadMenuView::TYSaveLoadMenuView(QWidget* parent)
    : TYMenuModeView(parent)
{
}

TYSaveLoadMenuView::~TYSaveLoadMenuView()
{
}

bool TYSaveLoadMenuView::initWithFrame(const QRect& aFrame,
                                        QObject* aObj,
                                        const QMap<QString, QVariant>& layoutDict,
                                        const QMap<QString, QVariant>& aDict,
                                        const QStringList& dArray,
                                        const QStringList& eArray,
                                        bool doLoad)
{
    Q_UNUSED(aFrame);

    int fontWidth;
    int fontHeight;
    int pitchX;
    int pitchY;
    bool shadow;

    QVector<TYControllButton*> buttonArray;
    QVector<QString> actionArray;
    QColor menuColor;

    // Read menu layout
    if (layoutDict.contains(TYRMenuFontSize)) {
        QSize size = layoutDict[TYRMenuFontSize].toSize();
        fontWidth = size.width();
        fontHeight = size.height();
    } else {
        fontWidth = 20;
        fontHeight = 24;
    }

    if (layoutDict.contains(TYRMenuFontPitchSize)) {
        QSize size = layoutDict[TYRMenuFontPitchSize].toSize();
        pitchX = size.width();
        pitchY = size.height();
    } else {
        pitchX = 0;
        pitchY = 0;
    }

    shadow = layoutDict[TYRMenuShadowed].toBool();
    menuColor = layoutDict[TYRMenuBGColor].value<QColor>();

    QVector<QColor> buttonColor;
    buttonColor.append(layoutDict[TYRMenuNoSelectColor].value<QColor>());
    buttonColor.append(layoutDict[TYRMenuSelectColor].value<QColor>());

    int cnt = dArray.size();

    // 1行目の位置取得。
    int y = VSCREEN_HEIGHT - (VSCREEN_HEIGHT + (-fontHeight - pitchY) * (cnt - 1)) / 2;

    int tag = 0;
    int idx = 0;

    QStringList::const_iterator dit = dArray.constBegin();
    QStringList::const_iterator eit = eArray.constBegin();

    while (dit != dArray.constEnd()) {
        QString temp = *dit;

        bool exists = true;
        if (idx != 0 && eit != eArray.constEnd()) {
            exists = (*eit).toLower() == "true";
            ++eit;
        }

        QVector<QColor> color;
        if (idx == 0) {
            color.append(layoutDict[TYRMenuSelectColor].value<QColor>());
        } else if (doLoad && !exists) {
            color.append(layoutDict[TYRMenuNoDataColor].value<QColor>());
        } else {
            color = buttonColor;
        }

        // イメージ生成
        TYCellImage* image = TYStringImage::imagesWithString(
            temp, aDict, color,
            fontHeight, fontWidth, pitchX, shadow
        );

        // button生成。
        QSize size = image->size();
        QRect frame(
            (VSCREEN_WIDTH - size.width()) / 2,
            y,
            size.width(),
            size.height()
        );

        TYControllButton* button = new TYControllButton(frame, image);

        if (idx != 0 && (!doLoad || exists)) {
            button->setTarget(this);
            button->setAction("execButton");
            button->setTag(tag++);
            buttonArray.append(button);
            QString actionStr = doLoad ? "_qload%1" : "_qsave%1";
            actionArray.append(actionStr.arg(idx));
        }

        addChildWidget(button);

        y -= fontHeight + pitchY;
        ++idx;
        ++dit;
    }
    
    // 初期化完了
    setController(aObj, 
                  buttonArray, 
                  actionArray, 
                  menuColor);
	
    return true;
}
