//
//  TYArrayValue.m
//  Tukuyomi
//
//  Created by toveta on Mon Jan 06 2003.
//  Copyright (c) 2003 toveta. All rights reserved.
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

#import "TYArrayValue.h"


@implementation TYArrayValue

// 配列の指定位置を取り出す再帰関数
static int get_address(int,NSEnumerator*,NSEnumerator*);

static int get_address(int offset,NSEnumerator *enuI,NSEnumerator *enuA)
{
    id tmp1,tmp2;

    tmp1 = [ enuI nextObject ];    
    tmp2 = [ enuA nextObject ];

    if(tmp2 == nil)
        return 0;
    else {
        NSCAssert(([ tmp1 intValue ] > [ tmp2 intValue ]),@"Bad Address of Array Value");
        
        if(offset == 0) 
            return [ tmp2 intValue ] + get_address([ tmp1 intValue ],enuI,enuA);
        else
            return [ tmp2 intValue ] * offset + get_address(offset * [ tmp1 intValue ],enuI,enuA);
    }
}

-(id)initWithID:(int)aInt limits:(NSArray*)aArray
{
    [ super init ];
    
    idNo = aInt;
    limitArray = [ aArray retain ];
    
    // データ格納領域確保
    {
        int size = 1;
        NSEnumerator *enu = [ limitArray objectEnumerator ];
        id tmp;
        
        while(tmp = [ enu nextObject ]){
            size *= [ tmp intValue ];
        }
        
        data = [ [ NSMutableData alloc ] initWithLength:size * sizeof(int) ];
    }
    
    dataPtr = [ data mutableBytes ];
    lastLimit = [ [ limitArray lastObject ] intValue ]; 
    
    return self;
}

-(void)setNum:(NSNumber*)num address:(NSArray*)aArray
{
    NSEnumerator *enuI,*enuA;
    int address;
    
    NSAssert(([ limitArray count ]==[ aArray count ]),@"Bad Address of Array Value");
    
    enuI = [ limitArray reverseObjectEnumerator ];
    enuA = [ aArray reverseObjectEnumerator ];
    
    address = get_address(0,enuI,enuA);
    
    *(dataPtr +address) = [ num intValue ];
}

-(void)setNumbers:(NSArray*)numbers address:(NSArray*)aArray
{
    NSEnumerator *enuI,*enuA;
    int address,prLastLimit;

    NSAssert(([ limitArray count ]==([ aArray count ]+1)),@"Bad Address of Array Value");

    prLastLimit = [ [ limitArray lastObject ] intValue ];
    
    NSAssert((lastLimit == [ numbers count ]),@"No Match Argments Number Of Elements");
    
    enuI = [ limitArray reverseObjectEnumerator ];
    enuA = [ aArray reverseObjectEnumerator ];

    address = get_address([ [ enuI nextObject ] intValue ],enuI,enuA);
    
    // データ代入処理
    {
        NSEnumerator *enuN=[ numbers objectEnumerator ];
        int *ptr;
        id tmp;
        
        ptr = dataPtr +address;
        
        while(tmp = [ enuN nextObject ]){
            *ptr = [ tmp intValue ];
            ptr++;
        }
    }
    
}

-(NSNumber*)getNumOfAddress:(NSArray*)aArray
{
    NSEnumerator *enuI,*enuA;
    int address;
    
    NSAssert(([ limitArray count ]==[ aArray count ]),@"Bad Address of Array Value");
    
    enuI = [ limitArray reverseObjectEnumerator ];
    enuA = [ aArray reverseObjectEnumerator ];
    
    address = get_address(0,enuI,enuA);
    
    return [ NSNumber numberWithInt:*(dataPtr +address) ];
}

// ----------------------------------------------------------------------------------------
// NSCoding
// ----------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)aDecoder
{
	[super init];
        
        [aDecoder decodeValueOfObjCType:@encode(int) at:&idNo ];
        limitArray = [ [aDecoder decodeObject] retain ];
        data = [ [ aDecoder decodeObject ] retain ];
        dataPtr = [ data mutableBytes ];
        lastLimit = [ [ limitArray lastObject ] intValue ];
        
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
        [aCoder encodeValueOfObjCType:@encode(int) at:&idNo ];
        [aCoder encodeObject:limitArray ];
        [aCoder encodeObject:data ];
}

- (void)dealloc
{
    [limitArray release];
    [data release];
    [super dealloc];
}

@end
