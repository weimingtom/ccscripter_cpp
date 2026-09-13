//
//  TYSarArchiver.m
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

#include "TYSarArchiver.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

TYSarArchiver::TYSarArchiver(QObject* parent)
    : TYArchiver(parent)
    , m_baseOffset(0)
{
}

TYSarArchiver::~TYSarArchiver()
{
    qDeleteAll(m_filesDict);
}


/* えーと本来のCocoaプログラミング的にはこうした構造体を用意してNSValueオブジェクトを作るほうが正しい気がするのですが。動いているコードはいじるなの原則に従って手を付けない事にしてコメントアウト。
struct tySarArchivedFileInfo {
    unsigned fileOffset;
    unsigned fileLength;
};

typedef struct tySarArchivedFileInfo TYSarArchivedFileInfo;

static __inline__ TYSarArchivedFileInfo TYMakeSarArchivedFileInfo(unsigned offset,unsigned length)
{
    TYSarArchivedFileInfo info;
    info.fileOffset = offset;
    info.fileLength = length;
    return info;
}
*/
TYSarArchiver* TYSarArchiver::archiverWithContentsOfFile(const QString& path)
{
    TYSarArchiver* archiver = new TYSarArchiver();
    if (archiver->initWithContentsOfFile(path)) {
        return archiver;
    }
    delete archiver;
    return nullptr;
}

bool TYSarArchiver::initWithContentsOfFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // 僼傽僀儖悢偺撉傒弌偟
    unsigned char readbuf[4];
    if (file.read(reinterpret_cast<char*>(readbuf), 2) != 2) {
        return false;
    }
    unsigned filenum = (readbuf[0] << 8) | readbuf[1];

    // 僨乕僞晹偺巒揰僆僼僙僢僩偺撉傒弌偟
    if (file.read(reinterpret_cast<char*>(readbuf), 4) != 4) {
        return false;
    }
    m_baseOffset = (readbuf[0] << 24) | (readbuf[1] << 16) | (readbuf[2] << 8) | readbuf[3];

    int fd = file.handle();

    // 僼傽僀儖偺僀儞僨僢僋僗傪嵜惉
    for (unsigned i = 0; i < filenum; i++) {
        char address[256];
        int j = 0;

        while(1){
            /*
            readDatabuf = [ sarhandle readDataOfLength:1 ]; // 改良点：まとめて256バイトぐらい読むようにしたいな。
            readbufptr = (unsigned char *)[ readDatabuf bytes ];
             */
            if (read(fd, &readbuf[0], 1) != 1) {
                break;
            }
            if (readbuf[0]) {
                // バックスラッシュはうざいのでスラッシュに変えます。
                if (readbuf[0] == '\\') readbuf[0] = '/';
                address[j] = readbuf[0];
            } else {
                address[j] = '\0';
                // アドレスから文字列オブジェクトを作成し、配列に加える。
                QString key = QString::fromLatin1(address);

                // 基点からのオフセットを読み出し
                unsigned fileOffset, fileLength;
                if (file.read(reinterpret_cast<char*>(readbuf), 4) != 4) {
                    break;
                }
                fileOffset = (readbuf[0] << 24) | (readbuf[1] << 16) | (readbuf[2] << 8) | readbuf[3];

                // ファイルのバイト長を読み出し
                if (file.read(reinterpret_cast<char*>(readbuf), 4) != 4) {
                    break;
                }
                fileLength = (readbuf[0] << 24) | (readbuf[1] << 16) | (readbuf[2] << 8) | readbuf[3];

                // アーカイブ内ファイルオブジェクトを作成し、配列に加える
                TYSarArchivedFile* archivedFile = TYSarArchivedFile::archivedFileoffset(fileOffset, fileLength);
                m_filesDict.insert(key.toLower(), archivedFile);

                break;
            }
            j++;
        }
    }

    // 僷僗傪奐偺偟側偑偼搊榐偡傞偐偑偁傞偱偡偑丅
    m_arcPath = path;

    return true;
}

// アーカイブのファイルハンドルを取得し、指定データの開始位置にシークして返す。
QFile* TYSarArchiver::fileHandleWithPath(const QString& aPath, int* length)
{
    TYSarArchivedFile* tmp = m_filesDict.value(aPath.toLower(), nullptr);
    if (!tmp) {
        return nullptr;
    }

    QFile* file = new QFile(m_arcPath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
        return nullptr;
    }

    unsigned fileOffset, fileLen;
    tmp->getFileDataOffset(&fileOffset, &fileLen);

    if (!file->seek(fileOffset + m_baseOffset)) {
        delete file;
        return nullptr;
    }

    if (length) {
        *length = static_cast<int>(fileLen);
    }

    return file;
}

// アーカイブ内からファイルを取得してきます。
QByteArray TYSarArchiver::unArchivedFile(const QString& address)
{
    TYSarArchivedFile* tmp = m_filesDict.value(address.toLower(), nullptr);
    if (!tmp) {
        return QByteArray();
    }

    unsigned fileOffset, fileLength;
    tmp->getFileDataOffset(&fileOffset, &fileLength);

    return readDataOffset(fileOffset, fileLength);
}

QByteArray TYSarArchiver::readDataOffset(unsigned offset, unsigned length)
{
    QFile file(m_arcPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    if (!file.seek(offset + m_baseOffset)) {
        return QByteArray();
    }

    return file.read(length);
}
