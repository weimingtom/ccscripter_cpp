//
//  TYAliasArgment.m
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

#include "TYAliasArgment.h"
#include "TYScriptEngine.h"
#include <algorithm>
#include <cctype>

std::string TYAliasArgument::stringToAlias(const std::string& input)
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

TYArgment* TYAliasArgument::argmentWithCString(const char* aChar)
{
    if (!aChar) {
        return nullptr;
    }
    std::string str(aChar);
    str = stringToAlias(str);
    return createFromString(str);
}

TYArgment* TYAliasArgument::createFromString(const std::string& str)
{
    return AliasUtils::createAliasArgument(str);
}

namespace AliasUtils {

TYArgment* createAliasArgument(const std::string& str)
{
    // Use ScriptEngine to resolve alias
    TYScriptEngine* engine = TYScriptEngine::sharedEngine();
    if (!engine) {
        return nullptr;
    }

    // Check if it's an alias that needs resolution
    TYArgment* resolved = engine->resolveAlias(str);
    if (resolved) {
        return resolved;
    }

    // Return alias argument that will be resolved later
    // In C++ implementation, we create a wrapper
    return engine->createAliasArgument(str);
}

std::string getAliasString(const TYArgment* arg)
{
    if (!arg) {
        return "";
    }
    return arg->stringObj();
}

} // namespace AliasUtils
