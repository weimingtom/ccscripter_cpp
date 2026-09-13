//
//  TYBGLZSSDecorder.m
//  Tukuyomi
//
//  Created by toveta on Sat Nov 02 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#include "TYBGLZSSDecorder.h"
#include "lzss.h"
#include <QThread>
#include <QCoreApplication>

QByteArray TYBGLZSSDecorder::m_decodeData = QByteArray();
int TYBGLZSSDecorder::m_encode_size = 0;
int TYBGLZSSDecorder::m_original_size = 0;
bool TYBGLZSSDecorder::m_is_end = false;

TYBGLZSSDecorder::TYBGLZSSDecorder(QObject* parent)
    : QObject(parent)
{
}

TYBGLZSSDecorder::~TYBGLZSSDecorder()
{
}

QByteArray TYBGLZSSDecorder::decodeWithFileHandle(int fh, int encode_size, int original_size)
{
    m_is_end = false;
    m_encode_size = encode_size;
    m_original_size = original_size;
    
    QThread* thread = QThread::create([fh]() {
        decode(fh);
    });
    thread->start();
    
    while (!m_is_end)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    
    return m_decodeData;
}

// private
void TYBGLZSSDecorder::decode(int fh)
{
    m_decodeData = TYDataWithLzssDecodeFromFile(fh, m_encode_size, m_original_size);
    m_is_end = true;
}
