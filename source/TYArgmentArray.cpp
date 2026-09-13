//
//  TYArgmentArray.m
//  Tukuyomi
//
//  Created by toveta on Wed Oct 30 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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

#import "TYArgmentArray.h"
#import "TYScriptEngine.h"

// gnscripterからコピペ(ぉ
#define myisdigit(c) ((c) >= '0' && (c) <= '9')
#define myishexdigit(c) (myisdigit(c) || (c) >= 'a' && (c) <= 'f' || (c) >= 'A' && (c) <= 'F')
#define myisalpha(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z' || (c) == '_')
#define myisalnum(c) (myisdigit(c) || myisalpha(c))

static int get_item(char**,char*);
static int get_vararg(char**,char*);
static void space_skip(char**);

typedef enum { TYNoArgContext=-1,TYUndefinedArgContext,TYOperandArgContext,TYStringArgContext,TYIntergerArgContext,TYSeparaterArgContext,TYChainArgContext } TYArgContext;

static int get_item(char** ptr,char* buffer)
{
    int context=TYUndefinedArgContext;

    if( **ptr=='%' ){
        context = TYIntergerArgContext;
        get_vararg(ptr,buffer);
        return context;
    } else if( **ptr=='$') {
        context = TYStringArgContext;
        get_vararg(ptr,buffer);
        return context;
    } else if( **ptr=='"' ) {
        context = TYStringArgContext;
        *buffer=**ptr; buffer++; (*ptr)++;
        while(**ptr && **ptr != '"'){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer=**ptr; buffer++; (*ptr)++;
        *buffer='\0';
        return context;
    } else if(**ptr=='(') {
        char strbuffer[MAX_LINE_LENGTH+1];
        // MEMO (string) string string 構文に対応
        context = TYStringArgContext;

        *buffer = **ptr;
        (*ptr)++; buffer++;
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *buffer = '\0';
        strcat(buffer,strbuffer);
        buffer += strlen(strbuffer);
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }        
        *buffer = **ptr;
        (*ptr)++; buffer++;
        // second
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *(buffer) = '\0';
        strcat(strbuffer," "); // Obj-Cのパーサで楽するために必ずスペースを追加
        strcat(buffer,strbuffer);
        buffer += strlen(strbuffer);
        // third
        while(**ptr && isspace(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        get_item(ptr,strbuffer);
        *(buffer) = '\0';
        strcat(buffer,strbuffer);

        return context;
    } else if(isdigit(**ptr)||(**ptr=='-')) {
        context = TYIntergerArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;        
        while(**ptr && isdigit(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer= '\0';
        return context;
    } else if(isalpha(**ptr)) {
        context = TYUndefinedArgContext;
        while(**ptr && myisalnum(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr && strchr("!<>=",**ptr)){
        context = TYOperandArgContext;
        while(**ptr && strchr("!<>=",**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr==',') {
        context = TYSeparaterArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        *buffer='\0';
        return context;
    } else if(**ptr=='&') {
        context = TYChainArgContext;
        while(**ptr && **ptr=='&'){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    } else if(**ptr=='*' || **ptr=='#') {
        context = TYStringArgContext;
        *buffer = **ptr;
        (*ptr)++; buffer++;
        while(**ptr && myisalnum(**ptr)){
            *buffer = **ptr;
            (*ptr)++; buffer++;
        }
        *buffer='\0';
        return context;
    }
        
    return TYNoArgContext;
}

static int get_vararg(char** ptr,char* buffer)
{
    while(**ptr){
        if(isalpha(**ptr)){
            while(**ptr && (myisalnum(**ptr))){
                *buffer = **ptr;
                buffer++; (*ptr)++;
            }
            *buffer='\0';
            return 0;
        } else if(isdigit(**ptr)){
            while(**ptr && (isdigit(**ptr)) ){
                *buffer = **ptr;
                buffer++; (*ptr)++;
            }
            *buffer='\0';
            return 0;
        } else {
            *buffer = **ptr;
            buffer++; (*ptr)++;
        }
    }

    return -1;
}

static void space_skip(char** ptr)
{
    while(**ptr && isspace(**ptr))
        (*ptr)++;
}

@implementation TYArgmentArray


+(id)arrayWithCString:(char*)aCStr command:(char*)cmdCStr
{
    return [ [ [ [ self class ] alloc ] initWithCString:aCStr command:cmdCStr ] autorelease ];
}

-(id)initWithCString:(char*)aCStr command:(char*)cmdCStr
{
    [ super init ];
        
    sourceData = [ [ NSData alloc ] initWithBytes:aCStr length:strlen(aCStr) ];
    parsedArgments = [ [ NSMutableArray alloc ] initWithObjects:[ NSString stringWithCString:cmdCStr ],nil ];
    
    return self;
}

// パースモードは外から指定する
-(void)setParseMode:(TYParseMode)mode
{
    parseMode = mode;
}

-(void)parse
{
    isParsed = YES;
    
    switch(parseMode) {
    case TYParseModeNormal:
    case TYParseModeCondition:
    case TYParseModeLoop:
        break;
    }
}

-(id)objectAtIndex:(int)aIndex
{
    if((aIndex!=0)&&(!isParsed)){
        [ self parse ];        
    }
    return [ parsedArgments objectAtIndex:aIndex ];
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    SEL sel = [ anInvocation selector ];
    if([ parsedArgments respondsToSelector:sel ]) {
        // 解析が終ってなければ解析を行なう
        if(!isParsed)
            [ self parse ];
        [ anInvocation invokeWithTarget:parsedArgments ];
    } else
        [ self doesNotRecognizeSelector:sel ];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
    if([ super respondsToSelector:aSelector ])
        return [ super methodSignatureForSelector:aSelector ];
    return [ parsedArgments methodSignatureForSelector:aSelector ];
}

- (void)dealloc
{
	[parsedArgments release];
        [sourceData release];
	[super dealloc];
}


@end
