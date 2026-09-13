//
//  TYLIBSNDFileDecorder.h
//  Tukuyomi
//
//  Created by toveta on Sat Sep 28 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TYWaveDecorder.h"

@interface TYLIBSNDFileDecorder : NSObject<TYWaveDecorder> {
    NSString *tempPath;
    short *samples;
}

@end
