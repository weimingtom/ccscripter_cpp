//
//  TYArchiver.h
//  Tukuyomi
//
//  Created by toveta on Wed Aug 22 2001.
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

#ifndef TYArchiver_h
#define TYArchiver_h

#include <QObject>
#include <QString>
#include <QFile>
#include <QMap>

/* SAR,NSAのクラスクラスタ　*/
/* 要はこのクラスの初期化メソッドが適切なサブクラスのインスタンスを返してくれるわけさ　*/
class TYArchiver : public QObject
{
    Q_OBJECT

public:
    static TYArchiver* archiverWithContentsOfFile(const QString& path);

    explicit TYArchiver(QObject* parent = nullptr);
    virtual ~TYArchiver();

    virtual bool initWithContentsOfFile(const QString& path);

    virtual QFile* fileHandleWithPath(const QString& aPath, int* length);
    virtual QByteArray unArchivedFile(const QString& address);

    void setArcPath(const QString& path) { m_arcPath = path; }
    QString arcPath() const { return m_arcPath; }

    void setBaseOffset(unsigned offset) { m_baseOffset = offset; }
    unsigned baseOffset() const { return m_baseOffset; }

protected:
    QString m_path;
    QString m_arcPath;
    unsigned m_baseOffset;
};

#endif // TYArchiver_h
