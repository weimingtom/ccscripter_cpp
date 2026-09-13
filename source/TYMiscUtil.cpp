//
//  TYMiscUtil.m
//  Tukuyomi
//
//  Created by toveta on Wed Sep 26 2001.
//  Copyright (c) 2001 toveta. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY toveta ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL toveta OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "TYMiscUtil.h"

// NSStringの先頭から数字を取得し、その長さも取得
BOOL scanIntAndLengthFromString(NSString* aSource,int* value,int* length)
{
    NSScanner *scanner;
    BOOL result;
    NSString *getString;
    
    scanner = [ NSScanner scannerWithString:aSource ];
    
    result = [ scanner scanCharactersFromSet:[ NSCharacterSet decimalDigitCharacterSet ] intoString:&getString ];

    if(result){
        *value = [ getString intValue ];
        *length = [ getString length ];
    }
    
    return result;
}

NSString* getAppDirectory(void)
{
    return [ [ [ NSBundle mainBundle ] bundlePath ] stringByDeletingLastPathComponent ];
}

static NSString* StrNScrRootDir;
void setNScrRootDirectory(NSString* aString)
{
    StrNScrRootDir = [ aString retain ];
}

NSString* getNScrRootDirectory(void)
{
    return StrNScrRootDir ? StrNScrRootDir : getAppDirectory();
}

// パスのデリミタをMac OS Xのものに変換
NSString* winPathToUnix(str)
NSString *str;
{
    NSString *convstr;
    NSArray *buffer;

    buffer = [ str componentsSeparatedByString:@"\\" ];
    convstr = [ buffer componentsJoinedByString:@"/" ];

    return convstr;
}

/*
NSMuatbleDictionary* TYDictionaryWithGlobalSaveData(NSData*)
{
}

NSData* TYGlobalSaveDataWithDictionary(NSDictionary*)
{
}
*/

/*
NSMutableDictionary* TYDictionaryWithFileLog(NSData* aData)
{
}

NSData* TYFileLogWithDictionary(NSDictionary* aDict)
{
}
*/

int TYRandom(int min,int max)
{
    return (int)((max -min +1) * (double)random() / ((double)RAND_MAX + 1.0)) +min;
}

NSString* TYWideCharDecimalString(int aInt)
{
    NSString *aStr;
    aStr = [ NSString stringWithFormat:@"%d",aInt ];
        
    return TYWideCharDecimalStringWithString(aStr);
}

NSString* TYWideCharDecimalStringWithString(NSString *decStr)
{
    NSMutableString *resultString = [ NSMutableString stringWithString:@"" ];
    int i,len;
    
    len = [ decStr length ];
    
    for(i = 0; i < len; i++){
        NSString *oneCharStr;
        
        oneCharStr = [ decStr substringWithRange:NSMakeRange(i,1) ];
        [ resultString appendString:NSLocalizedString(oneCharStr,nil) ];
    }
    
    return resultString;    
}

@implementation NSCharacterSet (TYScriptUtility)
+(id)varNameCharacterSet
{
    static NSMutableCharacterSet *set;
    
    if(set)
        return set;

    set = [ [ NSMutableCharacterSet alphanumericCharacterSet ] mutableCopy ];
    [ set addCharactersInRange:NSMakeRange(0x5F,1) ]; // アンダーライン追加
    
    return set;
}
@end

@implementation NSArray (TYSubArray)
-(id)subarrayFromIndex:(unsigned int)aInt
{
    NSRange range;
    int count  = [ self count ];
    
    NSAssert((count > aInt),@"Array Index over flow");
    
    range.location = aInt;
    range.length = count -aInt;
    
    return [ self subarrayWithRange:range ];
}
@end

@implementation NSDictionary (TYDictionarySort)
-(id)objectsSortedByKeyUsingSelector:(SEL)comparator
{
    return [ self objectsForKeys:[ [ self allKeys ] sortedArrayUsingSelector:comparator ]   notFoundMarker:[ NSNull null ] ];
}
@end
