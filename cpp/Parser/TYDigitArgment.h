//
//  TYDigitArgment.h
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


#ifndef TY_DIGIT_ARGUMENT_H
#define TY_DIGIT_ARGUMENT_H

#include "TYArgment.h"
#include <string>

// 数値即値のクラス。実体はNSNumberのカテゴリ。

class TYDigitArgument {
public:
    // Factory method
    static TYArgment* argmentWithInt(int value);

    // Creates digit argument from integer
    static TYArgment* create(int value);
};

// Number argument wrapper
class TYNumberArgument : public TYArgment {
public:
    explicit TYNumberArgument(int value);
    virtual ~TYNumberArgument();

    // Override
    virtual TYArgType argType() const override;
    virtual TYArgment* intNumber() const override;
    virtual std::string stringObj() const override;
    virtual std::string rawString() const override;
    virtual TYVarType varType() const override;
    virtual std::string varID() const override;

    // Access
    int getValue() const { return m_value; }

private:
    int m_value;
};

#endif // TY_DIGIT_ARGUMENT_H
