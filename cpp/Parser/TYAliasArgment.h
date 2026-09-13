//
//  TYAliasArgment.h
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

#ifndef TY_ALIAS_ARGUMENT_H
#define TY_ALIAS_ARGUMENT_H

#include <string>
#include "TYArgment.h"

// エイリアス型引数のクラス。実体はNSStringのカテゴリ。コンテキストにより、数字となるか文字列となるか決まる。
// "そのまま"使われる文字列もこれに含む。
class TYAliasArgument {
public:
    // Factory method
    static TYArgment* argmentWithCString(const char* aChar);

    // Creates an alias argument from a string
    static TYArgment* createFromString(const std::string& str);

    // Static utility to convert string to alias format
    static std::string stringToAlias(const std::string& input);
};

// Utility namespace for alias operations
namespace AliasUtils {
    TYArgment* createAliasArgument(const std::string& str);
    std::string getAliasString(const TYArgment* arg);
}

#endif // TY_ALIAS_ARGUMENT_H
