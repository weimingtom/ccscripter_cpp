//
//  TYFullScreenContentView.m
//  Tukuyomi
//
//  Created by toveta on Sat Feb 16 2002.
//  Copyright (c) 2001 toveta All rights reserved.
//

#import "TYFullScreenContentView.h"


@implementation TYFullScreenContentView

- (void)drawRect:(NSRect)rect;
{
    [ [ NSColor blackColor ] set ];
    NSRectFill(rect);
}

@end
