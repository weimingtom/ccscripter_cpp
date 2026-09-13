//
//  TYArgment.h
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

#ifndef TY_ARGUMENT_H
#define TY_ARGUMENT_H

#include <string>
#include <vector>
#include <memory>

// Variable types
enum TYVarType {
    TYNotVarType = 0,
    TYStringVarType = 1,
    TYIntVarType = 2,
    TYArrayVarType = 3,
    TYDigitVarTypeMask = 2
};

// Argument types
enum TYArgType {
    TYDigitArgType,
    TYConstStringArgType,
    TYIntVarArgType,
    TYStringVarArgType,
    TYAliasArgType,
    TYFchkArgType
};

// Comparison operation types
enum TYOpType {
    TYEqualOpType = 2,
    TYNotEqualOpType = 1 | 4,
    TYGreatThanOpType = 4,
    TYGreatOrEqualOpType = 2 | 4,
    TYLessThanOpType = 1,
    TYLessOrEqualOpType = 1 | 2
};

// Base class for all argument types in N스크 parser
class TYArgment {
public:
    virtual ~TYArgment() = default;

    // Virtual methods to be overridden
    virtual std::string varID() const;
    virtual TYVarType varType() const;
    virtual TYArgType argType() const = 0;
    virtual TYArgment* intNumber() const;
    virtual std::string stringObj() const;
    virtual std::string rawString() const;

    // Utility methods
    virtual int intValue() const;
    virtual bool canAddObject() const;
    virtual std::vector<TYArgment*>* toArray();

    // Static factory methods
    static TYArgment* createIntNumber(int value);
    static TYArgment* createConstString(const std::string& str);

protected:
    TYArgment() = default;
};

// Comparison function declaration
bool TYCompareArgment(TYArgment* left, TYArgment* right, TYOpType op);

#endif // TY_ARGUMENT_H
