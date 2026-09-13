//
//  TYBGLZSSDecorder.m
//  Tukuyomi
//
//  Created by toveta on Sat Nov 02 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#import "TYBGLZSSDecorder.h"
#import "lzss.h"

@implementation TYBGLZSSDecorder

static NSData *decodeData;
static int encode_size,original_size;
static BOOL is_end;

+(NSData*)decodeWithFileHandle:(NSFileHandle*)fh encode_size:(int)e original_size:(int)o
{

    is_end = NO;
    encode_size = e;
    original_size = o;
    
    [ NSThread detachNewThreadSelector:@selector(decode:)
                              toTarget:self
                            withObject:fh ];
    
    while(!is_end)
        [ [ NSRunLoop currentRunLoop ] runUntilDate:[ NSDate dateWithTimeIntervalSinceNow:0.01 ] ];
        
    [ decodeData autorelease ];
    return decodeData;
}

// private
+(void)decode:(NSFileHandle*)fh
{
    NSAutoreleasePool *ap = [ [ NSAutoreleasePool alloc ] init ];
    decodeData = TYDataWithLzssDecodeFromFile(fh,encode_size,original_size);
    [ decodeData retain ];
    is_end = YES;
    [ ap release ];
}
@end
