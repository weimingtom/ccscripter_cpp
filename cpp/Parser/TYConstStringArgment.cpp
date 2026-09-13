//
//  TYConstStringArgment.m
//  Tukuyomi
//
//  Created by toveta on Wed Apr 09 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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
#include "TYConstStringArgment.h"
#include <sstream>

TYArgment* TYConstStringArgment::argmentWithString(const std::string& str)
{
    return new TYConstStringArgment(str);
}

TYConstStringArgment::TYConstStringArgment(const std::string& str)
    : m_string(str)
{
}

TYConstStringArgment::~TYConstStringArgment()
{
}

// override
TYArgType TYConstStringArgment::argType() const
{
    return TYConstStringArgType;
}

std::string TYConstStringArgment::stringObj() const
{
    return m_string;
}

TYArgment* TYConstStringArgment::intNumber() const
{
    // Try to convert string to number
    try {
        int value = std::stoi(m_string);
        // Delegate to base class factory
        return TYArgment::createIntNumber(value);
    } catch (...) {
        return nullptr;
    }
}

std::string TYConstStringArgment::rawString() const
{
    return m_string;
}
