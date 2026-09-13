//
//  TYStringVarArgment.m
//  Tukuyomi
//
//  Created by toveta on Wed Apr 09 2003.
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
#include "TYStringVarArgment.h"
#include "TYScriptEngine.h"

TYArgment* TYStringVarArgment::argmentWithVarID(TYArgment* varId)
{
    return new TYStringVarArgment(varId);
}

TYStringVarArgment::TYStringVarArgment(TYArgment* varId)
    : m_varID(varId)
{
}

TYStringVarArgment::~TYStringVarArgment()
{
    // Note: m_varID ownership depends on implementation
}

TYArgment* TYStringVarArgment::varID() const
{
    if (!m_varID) {
        return nullptr;
    }
    // Return intNumber representation of varID
    return m_varID->intNumber();
}

TYVarType TYStringVarArgment::varType() const
{
    return TYStringVarType;
}

TYArgType TYStringVarArgment::argType() const
{
    return TYStringVarArgType;
}

std::string TYStringVarArgment::stringObj() const
{
    TYArgment* vid = varID();
    if (!vid) {
        return "";
    }

    // Get value from script engine
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (engine) {
        TYArgment* result = engine->stringValueWithVarID(vid);
        if (result) {
            return result->stringObj();
        }
    }

    return "";
}
