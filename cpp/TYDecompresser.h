//
//  TYDecompresser.h
//  Tukuyomi
//
//  Created by toveta on Sun Feb 10 2002.
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

#ifndef TYDECOMPRESSER_H
#define TYDECOMPRESSER_H

#include <QObject>

class TYDecompresser : public QObject {
    Q_OBJECT
public:
    explicit TYDecompresser(QObject* parent = nullptr);
    virtual ~TYDecompresser();

    static TYDecompresser* sharedObject(); // must override from subclass

private:
    static TYDecompresser* m_sharedObject;
};

class TYDataDecoder {
public:
    virtual ~TYDataDecoder() {}
    virtual QByteArray decodeDataWithFileHandle(int handle, int encode_size) = 0;
};

class TYBitmapDecoder {
public:
    virtual ~TYBitmapDecoder() {}
    virtual void* decodeBitmapWithFileHandle(int handle, int encode_size) = 0;
};

#endif // TYDECOMPRESSER_H
