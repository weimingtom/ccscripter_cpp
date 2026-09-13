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
#import <Carbon/Carbon.h>
#import "TYScreenManager.h"
#import "StopNSLog.h"

static CFDictionaryRef originalMode;
static NSWindow *blackWindow;

static SInt32 TYCFGetDictionarySInt32Value(CFDictionaryRef,const void *);

void TYGetDisplayInfo()
{
    NSLog(@"%@",CGDisplayAvailableModes(kCGDirectMainDisplay));
}


NSSize TYStartFullScreen(int minWidth,int minHeight)
{
    CFDictionaryRef bestMode = NULL;
    CFDictionaryRef mode;
    CFArrayRef modeList;
    unsigned int bestSize=UINT_MAX;
    int i,cnt,width,height,bpp;
    SInt32 org_bpp;
    
    
    originalMode = CGDisplayCurrentMode( kCGDirectMainDisplay );
    org_bpp = TYCFGetDictionarySInt32Value(originalMode,kCGDisplayBitsPerPixel);
    
    modeList = CGDisplayAvailableModes( kCGDirectMainDisplay );
    cnt = CFArrayGetCount( modeList );
        
    for ( i = 0; i < cnt; i++ )
    {
        //  Pull the mode dictionary out of the CFArray
        mode = CFArrayGetValueAtIndex( modeList, i );

        /* ここでモードを検査する。 */
        width = TYCFGetDictionarySInt32Value(mode,kCGDisplayWidth);
        height = TYCFGetDictionarySInt32Value(mode,kCGDisplayHeight);
        bpp = TYCFGetDictionarySInt32Value(mode,kCGDisplayBitsPerPixel);
        if(bpp == org_bpp &&
           width >= minWidth && height >= minHeight &&
           width * height < bestSize )
        {
            bestSize = width * height;
            if(bestMode != mode) {
                if(bestMode)
                    CFRelease(bestMode);
                bestMode = CFRetain(mode);
            }
        }
    }

    NSLog2(@"%@",bestMode);
    
    if ( NULL != bestMode ) {
        int bestWidth,bestHeight;
        
        bestWidth = TYCFGetDictionarySInt32Value(bestMode,kCGDisplayWidth);
        bestHeight = TYCFGetDictionarySInt32Value(bestMode,kCGDisplayHeight);
        
        CGDisplayCapture( kCGDirectMainDisplay );
        CGDisplaySwitchToMode( kCGDirectMainDisplay, bestMode );
        CGDisplayRelease( kCGDirectMainDisplay );
        
        return NSMakeSize(bestWidth,bestHeight);
    }
    
    return NSZeroSize;
}

static SInt32 TYCFGetDictionarySInt32Value(CFDictionaryRef aDict,const void * aKey)
{
    CFNumberRef number;
    SInt32 buf;
        
    number = CFDictionaryGetValue(aDict,aKey);
    if(number && CFNumberGetType(number) == kCFNumberSInt32Type)
        CFNumberGetValue(number,kCFNumberSInt32Type,&buf);
    else
        buf = LONG_MAX;
    
    return buf;
}


void TYEndFullScreen()
{
    if(originalMode != NULL) {
        CGDisplayCapture( kCGDirectMainDisplay );
        CGDisplaySwitchToMode( kCGDirectMainDisplay, originalMode );
        CGDisplayRelease( kCGDirectMainDisplay );
        CFRelease(originalMode);
    }
    
    originalMode = NULL;
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