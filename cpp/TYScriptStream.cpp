//
//  TYScriptStream.m
//  Tukuyomi
//
//  Created by toveta on Wed Oct 17 2001.
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

#include "TYScriptStream.h"

static TYScriptStream* sharedStreamInstance = nullptr;

TYScriptStream::TYScriptStream(QByteArray aData, QObject* parent)
    : QObject(parent)
    , m_data(aData)
    , m_line(0)
{
    // lex にかけて初期化さす。
}

TYScriptStream::~TYScriptStream()
{
}

TYScriptStream* TYScriptStream::sharedStream()
{
    if (!sharedStreamInstance) {
        sharedStreamInstance = new TYScriptStream(QByteArray());
    }
    return sharedStreamInstance;
}

void TYScriptStream::addNewLine(char* aChar)
{
    m_linePtrArray.append(QByteArray(aChar));
}

void TYScriptStream::addLabel(QString label)
{
    m_labelDict[label.toLower()] = m_linePtrArray.size();
}

QVariant TYScriptStream::objectAtIndex(int aIndex)
{
    // 文字列を返す。
    if (aIndex >= 0 && aIndex < m_linePtrArray.size()) {
        return QVariant(QString::fromLocal8Bit(m_linePtrArray[aIndex]));
    }
    return QVariant();
}

char* TYScriptStream::cStringAtIndex(int aIndex)
{
    // ポインタを返す。
    if (aIndex >= 0 && aIndex < m_linePtrArray.size()) {
        QByteArray arr = m_linePtrArray[aIndex];
        char* result = new char[arr.size() + 1];
        qstrcpy(result, arr.constData());
        return result;
    }
    return nullptr;
}
