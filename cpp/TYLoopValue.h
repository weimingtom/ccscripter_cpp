//
//  TYLoopValue.h
//  Tukuyomi
//
//  Created by toveta on Thu Jan 09 2003.
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

#ifndef TYLoopValue_h
#define TYLoopValue_h

#include <QObject>
#include <QVariant>

struct TYScriptPoint { // スクリプトにおける”座標”
    unsigned line;
    unsigned column;
};

struct TYLoopStruct {
    TYScriptPoint point;
    QVariant varNo;  // 配列型も許すためにはid型である必要がある
    int limit;
    int step;
};

static inline TYLoopStruct TYMakeLoopStruct(TYScriptPoint p, QVariant n, int l, int s)
{
    TYLoopStruct ls;

    ls.point = p;
    ls.varNo = n;
    ls.limit = l;
    ls.step = s;

    return ls;
}

// スクリプト中のループ情報オブジェクト。
class TYLoopValue : public QObject {
    Q_OBJECT

public:
    explicit TYLoopValue(TYLoopStruct loop, QObject* parent = nullptr);
    ~TYLoopValue();

    TYLoopStruct value() const { return m_loop; }

private:
    TYLoopStruct m_loop; // foward
};

#endif // TYLoopValue_h
