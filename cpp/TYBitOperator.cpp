//
//  TYBitOperator.m
//  Tukuyomi
//
//  Created by toveta on Mon Jun 17 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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

#include "TYBitOperator.h"
#include <unistd.h>
#include <cassert>
#include <cstdlib>

TYBitOperator::TYBitOperator(QFile* f, unsigned int s, QObject* parent)
    : QObject(parent), fh(f), size(s), mask(0x100), tmp(0)
{
    if (fh && fh->isOpen()) {
        QByteArray data = fh->read(1);
        if (!data.isEmpty()) {
            tmp = static_cast<unsigned char>(data[0]);
        }
    }
}

TYBitOperator::~TYBitOperator()
{
}

int TYBitOperator::get()
{
    mask >>= 1;
    if (mask == 0x00) {
        Q_ASSERT(size != 0);
        size--;
        QByteArray data = fh->read(1);
        if (!data.isEmpty()) {
            tmp = static_cast<unsigned char>(data[0]);
        }
        mask = 0x80;
    }
    return (tmp & mask) ? 1 : 0;
}

unsigned int TYBitOperator::getWithLength(int n)
{
    unsigned int v = 0;

    Q_ASSERT(n > 0 && n <= 32);

    for (v = 0; n > 0; n--) {
        v <<= 1;
        if (this->get())
            v |= 1;
    }
    return v;
    
}

TYCoreBitOperater TYCreateCoreBitOperater(QFile* f, unsigned int s)
{
    TYCoreBitOperater mySelf;
    int result;
    
    Q_UNUSED(s);
    
    mySelf = static_cast<TYCoreBitOperater>(malloc(sizeof(struct tyCoreBitOperater)));
    if (mySelf == nullptr)
        return nullptr;
        
    mySelf->fd = f->handle();
    //mySelf->size = s;
    mySelf->mask = 0x100;
    result = read(mySelf->fd,&mySelf->tmp,1);
    if(!result){
        TYReleaseCoreBitOperater(mySelf);
        return nullptr;
    }
    
    return mySelf;
}

inline int TYGetCoreBitOperater(TYCoreBitOperater oper)
{
    oper->mask >>= 1;
    if (oper->mask == 0x00) {
        //NSCAssert((mySelf->size != 0),@"Over Limit Size");
        //mySelf->size--;
        read(oper->fd, &oper->tmp, 1);
        oper->mask = 0x80;
    }
    return (oper->tmp & oper->mask) ? 1 : 0;
}

inline unsigned int TYGetBitsCoreBitOperater(TYCoreBitOperater oper, int n)
{
    unsigned int v = 0;

    //NSCAssert((n > 0 && n <= 32),@"invalid bit length");

    for (v = 0; n > 0; n--) {
        v <<= 1;
        
        oper->mask >>= 1;
        if (oper->mask == 0x00) {
            read(oper->fd, &oper->tmp, 1);
            oper->mask = 0x80;
        }
        if (oper->tmp & oper->mask)
        //if (TYGetCoreBitOperater(mySelf))
            v |= 1;
    }
    return v;    
}

void TYReleaseCoreBitOperater(TYCoreBitOperater oper)
{
    free(oper);
}
