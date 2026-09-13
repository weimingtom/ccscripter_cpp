/*
 *  TYScreenManager.m
 *  Tukuyomi
 *
 *  Created by toveta on Wed Mar 12 2003.
 *  Copyright (c) 2003 toveta All rights reserved.
 *
 */
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
#include "TYScreenManager.h"
#include "StopNSLog.h"

#include <QDesktopWidget>
#include <QDebug>

static QWidget* blackWindow = nullptr;
static QSize originalScreenSize;

void TYGetDisplayInfo()
{
    qDebug() << "Available display modes:" << QDesktopWidget().availableGeometry();
}

static SInt32 TYCFGetDictionarySInt32Value(void* aDict, const void* aKey)
{
    Q_UNUSED(aDict);
    Q_UNUSED(aKey);
    return 0;
}

QSize TYStartFullScreen(int minWidth, int minHeight)
{
    // Store original screen size
    QDesktopWidget desktop;
    QRect screenGeometry = desktop.screenGeometry(&desktop.primaryScreen());
    originalScreenSize = screenGeometry.size();

    // Find best matching mode
    int bestWidth = -1;
    int bestHeight = -1;
    int bestSize = INT_MAX;

    // Check available geometries for best fit
    for (int i = 0; i < desktop.screenCount(); i++) {
        QRect rect = desktop.screenGeometry(i);
        int width = rect.width();
        int height = rect.height();

        if (width >= minWidth && height >= minHeight) {
            int size = width * height;
            if (size < bestSize) {
                bestSize = size;
                bestWidth = width;
                bestHeight = height;
            }
        }
    }

    if (bestWidth > 0 && bestHeight > 0) {
        qDebug() << "Switching to fullscreen mode:" << bestWidth << "x" << bestHeight;
        return QSize(bestWidth, bestHeight);
    }

    return QSize(0, 0);
}

void TYEndFullScreen()
{
    qDebug() << "Restoring original screen mode";
    blackWindow = nullptr;
}

/*
void TYFullScreenBlackOut()
{
    NSWindow aWindow;
    
    aWindow = [ [ NSWindow alloc ] initWithContentRect:[ [ NSScreen mainScreen ] frame ]
                                             styleMask:NSBorderlessWindowMask 						 	       backing:NSBackingStoreRetained
                                                 defer:NO ];

    [ aWindow setLevel:NSScreenSaverWindowLevel ];
    
}

void TYReadyFullScreenBlackIn()
{
}

void TYFullScreenBlackIn()
{
}
 */