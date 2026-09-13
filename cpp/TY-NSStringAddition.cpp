//
//  TY-NSStringAddition.m
//  Tukuyomi
//
//  Created by toveta on Tue Sep 25 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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

#include "TY-NSStringAddition.h"
#include <cstring>

#ifndef Q_OS_WIN
#include <QTextCodec>
#else
#include <windows.h>
#endif

QString TY_NSStringAddition::stringWithCSJISString(const char* cString)
{
    if (!cString) {
        return QString();
    }
    return stringWithCSJISString(cString, strlen(cString));
}

QString TY_NSStringAddition::stringWithCSJISString(const char* cString, unsigned int length)
{
    if (!cString || length == 0) {
        return QString();
    }
    
    // Try to convert Shift-JIS to Unicode
#ifndef Q_OS_WIN
    QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
    if (codec) {
        return codec->toUnicode(cString, length);
    }
#else
    // Windows-specific: use MultiByteToWideChar
    int wlen = MultiByteToWideChar(932, 0, cString, length, nullptr, 0);
    if (wlen > 0) {
        wchar_t* wbuf = new wchar_t[wlen + 1];
        MultiByteToWideChar(932, 0, cString, length, wbuf, wlen);
        wbuf[wlen] = L'\0';
        QString result = QString::fromWCharArray(wbuf);
        delete[] wbuf;
        return result;
    }
#endif
    
    return QString();
}

const char* TY_NSStringAddition::cSJISString() const
{
    static QByteArray cached;
    cached = sjisData();
    cached.append('\0');
    return cached.constData();
}

QByteArray TY_NSStringAddition::getCSJISData() const
{
    QByteArray data = sjisData();
    data.append('\0');
    return data;
}

QByteArray TY_NSStringAddition::sjisData() const
{
#ifndef Q_OS_WIN
    QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
    if (codec) {
        return codec->fromUnicode(*this);
    }
#else
    // Windows-specific: use WideCharToMultiByte
    QString us = *this;
    int len = us.length();
    int mblen = WideCharToMultiByte(932, 0, (const wchar_t*)us.utf16(), len, nullptr, 0, nullptr, nullptr);
    if (mblen > 0) {
        QByteArray result;
        result.resize(mblen);
        WideCharToMultiByte(932, 0, (const wchar_t*)us.utf16(), len, result.data(), mblen, nullptr, nullptr);
        return result;
    }
#endif
    return QByteArray();
}

unsigned TY_NSStringAddition::cSJISStringLength() const
{
    return static_cast<unsigned>(sjisData().length());
}

void TY_NSStringAddition::getCSJISString(char* buffer) const
{
    QByteArray data = sjisData();
    memcpy(buffer, data.constData(), data.size());
    buffer[data.size()] = '\0';
}

QString TY_NSStringAddition::substringSJISRange(int location, int length) const
{
    QByteArray scrData = sjisData();
    const char* ptr = scrData.constData();
    
    int strLength = scrData.length() - location;
    // Adjust length if it exceeds remaining string
    if (length > strLength)
        length = strLength;
    
    // Create temporary Shift-JIS string for the range
    QByteArray dstData(ptr + location, length);
    
    return stringWithCSJISString(dstData.constData(), dstData.length());
}

QString getBackSlashString()
{
    char b = '\\';
    
    // Return backslash as single character QString
    return TY_NSStringAddition::stringWithCSJISString(&b, 1);
}
