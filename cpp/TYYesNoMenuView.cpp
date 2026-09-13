//
//  TYYesNoMenuView.m
//  Tukuyomi
//
//  Created by toveta on Sun Mar 02 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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
 
#include "TYYesNoMenuView.h"
#include "TYStringImage.h"
#include "TYCellImage.h"
#include "TYStageManager.h"
#include "TYControllButton.h"


bool TYYesNoMenuView::initWithFrame(
                                     const QRect& aFrame,
                                     QObject* aObj,
                                     const QMap<QString, QVariant>& aDict,
                                     const QStringList& aArray,
                                     const QMap<QString, QVariant>& layoutDict)
{
    int fontWidth;
    int fontHeight;
    int pitchX;
    int pitchY;
    bool shadow;

    QVector<TYControllButton*> buttonArray;
    QVector<QString> actionArray;
    QColor menuColor;
	// 初期化

    // メニューレイアウトの読み込み
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

    int cnt = 2;
    
    // 1行目の位置取得。
    int y = VSCREEN_HEIGHT - (VSCREEN_HEIGHT + (-fontHeight - pitchY) * (cnt - 1)) / 2;

    int tag = 0;
    QStringList::const_iterator it = aArray.constBegin();
    while (it != aArray.constEnd()) {
        QString aStr;

        switch (tag) {
        case 0:
            aStr = *it;
            break;
        case 1:
            aStr = "YES";
            break;
        case 2:
            aStr = "NO";
            break;
        default:
            aStr = *it;
            break;
        }
        
        // イメージ生成
        TYCellImage* image = TYStringImage::imagesWithString(
            aStr, aDict,
            (tag == 0) ? QVector<QColor>() << layoutDict[TYRMenuSelectColor].value<QColor>() : buttonColor,
            fontHeight, fontWidth, pitchX, shadow
        );

        // button生成。
        QSize size = image->size();
        QRect frame;

        if (tag == 0) {
            frame = QRect(
                (VSCREEN_WIDTH - size.width()) / 2,
                y,
                size.width(),
                size.height()
            );
        } else {
            int x = VSCREEN_WIDTH / 2 + size.width() * ((tag == 1) ? -1.3f : 0.3f);
            frame = QRect(x, y, size.width(), size.height());
        }

        TYControllButton* button = new TYControllButton(frame, image);

        if(tag != 0){
            button->setTarget(this);
            button->setAction("execButton");
            button->setTag(tag - 1);
            buttonArray.append(button);
            actionArray.append(*(it + 1));
            addChildWidget(button);
        }
        
        if(tag == 0)
            y -= fontHeight +pitchY;
            
        tag++;
        ++it;
    }
    
    // 初期化完了
    setController(aObj, 
                  buttonArray, 
                  actionArray, 
                  menuColor);
	
    return true;
}

/*
-(void)mouseEntered:(NSEvent*)theEvent
{
    [ super mouseEntered:theEvent ];
}
*/

