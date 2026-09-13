//
//  TYFchkStringArgment.m
//  Tukuyomi
//
//  Created by toveta on Mon Apr 14 2003.
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

#include "TYFchkStringArgment.h"
#include "TYScriptEngine.h"

TYArgment* TYFchkStringArgment::argmentWithFileName(TYArgment* fileArg,
                                                     TYArgment* trueArg,
                                                     TYArgment* falseArg)
{
    return new TYFchkStringArgment(fileArg, trueArg, falseArg);
}

TYFchkStringArgment::TYFchkStringArgment(TYArgment* fileArg,
                                         TYArgment* trueArg,
                                         TYArgment* falseArg)
    : m_fileName(fileArg)
    , m_trueValue(trueArg)
    , m_falseValue(falseArg)
{
}

TYFchkStringArgment::~TYFchkStringArgment()
{
    // Note: ownership depends on caller, not destroyed here
}

TYArgType TYFchkStringArgment::argType() const
{
    return TYFchkArgType;
}

std::string TYFchkStringArgment::stringObj() const
{
    if (!m_fileName) {
        return m_falseValue ? m_falseValue->stringObj() : "";
    }

    std::string filePath = m_fileName->stringObj();

    // Use script engine to check file
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (engine) {
        if (engine->judgefchk(filePath)) {
            return m_trueValue ? m_trueValue->stringObj() : "";
        }
    }

    return m_falseValue ? m_falseValue->stringObj() : "";
}
