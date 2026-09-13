//
//  TYIntVarArgment.m
//  Tukuyomi
//
//  Created by toveta on Sun Apr 06 2003.
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

#include "TYIntVarArgment.h"
#include "TYScriptEngine.h"

TYArgment* TYIntVarArgment::argmentWithVarID(TYArgment* varId)
{
    return new TYIntVarArgment(varId);
}

TYIntVarArgment::TYIntVarArgment(TYArgment* varId)
    : m_varID(nullptr)
{
    if (varId) {
        // Store intNumber representation of varId
        TYArgment* intNum = varId->intNumber();
        if (intNum) {
            m_varID = intNum;
        }
    }
}

TYIntVarArgment::~TYIntVarArgment()
{
    // Note: m_varID ownership depends on implementation
}

TYArgment* TYIntVarArgment::varID() const
{
    return m_varID;
}

TYVarType TYIntVarArgment::varType() const
{
    return TYIntVarType;
}

TYArgType TYIntVarArgment::argType() const
{
    return TYIntVarArgType;
}

TYArgment* TYIntVarArgment::intNumber() const
{
    // Get value from script engine
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (engine && m_varID) {
        return engine->intNumberWithVarID(m_varID);
    }
    return nullptr;
}
