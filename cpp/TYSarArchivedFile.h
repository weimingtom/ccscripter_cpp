//
//  TYSarArchivedFile.h
//  Tukuyomi
//
//  Created by toveta on Thu Jun 14 2001.
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

#ifndef TYSarArchivedFile_h
#define TYSarArchivedFile_h

#include <QObject>

// TYSarArchiverクラスに登録される個々のファイル情報です。
class TYSarArchivedFile : public QObject
{
    Q_OBJECT

public:
    TYSarArchivedFile(QObject* parent = nullptr);
    TYSarArchivedFile(unsigned offset, unsigned length, QObject* parent = nullptr);

    static TYSarArchivedFile* archivedFileoffset(unsigned offset, unsigned length);

    void initFileoffset(unsigned offset, unsigned length);

    void getFileDataOffset(unsigned* offsetarg, unsigned* lengtharg) const;

    unsigned fileOffset() const { return m_fileOffset; }
    unsigned fileLength() const { return m_fileLength; }

private:
    unsigned m_fileOffset;
    unsigned m_fileLength;
};

#endif // TYSarArchivedFile_h
