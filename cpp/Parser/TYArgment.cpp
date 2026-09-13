//
//  TYArgment.m
//  YLLinkTest
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

#include "TYArgment.h"
#include "TYScriptEngine.h"
#include <stdexcept>
#include <sstream>

std::string TYArgment::varID() const
{
    throw std::runtime_error("TYArgment: not var");
}

TYVarType TYArgment::varType() const
{
    throw std::runtime_error("TYArgment: not var");
}

TYArgment* TYArgment::intNumber() const
{
    throw std::runtime_error("TYArgment: not int");
}

std::string TYArgment::stringObj() const
{
    throw std::runtime_error("TYArgment: not string");
}

std::string TYArgment::rawString() const
{
    throw std::runtime_error("TYArgment: not rawString");
}

int TYArgment::intValue() const
{
    TYArgment* num = intNumber();
    if (!num) {
        return 0;
    }
    return num->intValue();
}

bool TYArgment::canAddObject() const
{
    return false;
}

std::vector<TYArgment*>* TYArgment::toArray()
{
    return nullptr;
}

TYArgment* TYArgment::createIntNumber(int value)
{
    // Delegate to concrete implementation
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (engine) {
        return engine->createNumberArgument(value);
    }
    return nullptr;
}

TYArgment* TYArgment::createConstString(const std::string& str)
{
    // Delegate to concrete implementation
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (engine) {
        return engine->createConstStringArgument(str);
    }
    return nullptr;
}

// Comparison implementation
bool TYCompareArgment(TYArgment* left, TYArgment* right, TYOpType op)
{
    if (!left || !right) {
        return false;
    }

    int result = 0;

    // MEMO:引数の型チェックが必要
    // 左辺の文字列エイリアス、型の異る比較がエラーとなる。
    switch (left->argType()) {
    case TYConstStringArgType:
    case TYStringVarArgType:
    case TYFchkArgType:
        result = left->stringObj().compare(right->stringObj());
        break;
    case TYDigitArgType:
    case TYIntVarArgType:
    case TYAliasArgType:
    default:
        result = left->intValue() - right->intValue();
        break;
    }

    int flag = 0;
    if (result == 0)
        flag = 2;
    else if (result < 0)
        flag = 1;
    else
        flag = 4;
    
    return (flag & op) ? true : false;
}
