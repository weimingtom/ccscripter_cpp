//
//  TYDigitArgment.m
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
#include "TYDigitArgment.h"
#include <sstream>

TYArgment* TYDigitArgument::argmentWithInt(int value)
{
    return new TYNumberArgument(value);
}

TYArgment* TYDigitArgument::create(int value)
{
    return argmentWithInt(value);
}

TYNumberArgument::TYNumberArgument(int value)
    : m_value(value)
{
}

TYNumberArgument::~TYNumberArgument()
{
}

TYArgType TYNumberArgument::argType() const
{
    return TYDigitArgType;
}

TYArgment* TYNumberArgument::intNumber() const
{
    // Return self as the number representation
    // In C++ we keep the value directly
    return const_cast<TYNumberArgument*>(this);
}

std::string TYNumberArgument::stringObj() const
{
    std::ostringstream oss;
    oss << m_value;
    return oss.str();
}

std::string TYNumberArgument::rawString() const
{
    return stringObj();
}

int TYArgment::intValue() const
{
    // If this is a TYNumberArgument, return its value
    const TYNumberArgument* numArg = dynamic_cast<const TYNumberArgument*>(this);
    if (numArg) {
        return numArg->getValue();
    }

    TYArgment* num = intNumber();
    if (!num) {
        return 0;
    }

    const TYNumberArgument* numResult = dynamic_cast<const TYNumberArgument*>(num);
    if (numResult) {
        return numResult->getValue();
    }

    return 0;
}

TYVarType TYNumberArgument::varType() const
{
    // Not a variable, delegate to base
    return TYNotVarType;
}

std::string TYNumberArgument::varID() const
{
    // Not a variable, delegate to base
    return TYArgment::varID();
}
