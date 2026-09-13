//
//  TYNsaArchiver.h
//  Tukuyomi
//
//  Created by toveta on Sat Oct 13 2001.
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

#ifndef TYNsaArchiver_h
#define TYNsaArchiver_h

#include <QObject>
#include <QString>
#include <QMap>
#include <QByteArray>

#include "TYSarArchiver.h"
#include "TYNsaArchivedFile.h"

enum class TYNsaFileMode {
    TYNormalNsaFileMode,
    TY2TypeNsaFileMode,
    TY3TypeNsaFileMode
};

// NSAアーカイブの管理クラス
class TYNsaArchiver : public TYSarArchiver {
    Q_OBJECT

public:
    static void setMode(TYNsaFileMode aMode);

    explicit TYNsaArchiver(const QString& path = QString(), QObject* parent = nullptr);
	
//+(id)archiverWithContentsOfFile:(NSString*)path;
    bool initWithContentsOfFile(const QString& path) override; // override

// SPB圧縮されたBMPの展開をこのメソッドで行う必要がある。
    QByteArray unArchivedFile(const QString& address) override; //override
};

#endif // TYNsaArchiver_h
