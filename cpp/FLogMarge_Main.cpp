//
//  FLogMarge_Main.cpp
//  Tukuyomi
//
//  Created by toveta 2001.
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
#include <Foundation/Foundation.h>
#include "TYFileLog.h"

/*
    usage.. FLogMarge file1 file2 margeFile
    FLogMargeはNScrpterのファイルアクセスログを記録したファイルをマージします。
*/
int main (int argc, const char * argv[])
{
    NSString *path1,*path2,*path3;
    TYFileLog *log1,*log2;

    if(argc<4){
        NSLog(@"usage.. FLogMarge file1 file2 margeFile");
        return -1;
    }

    path1 = [ NSString stringWithCString:argv[1] ];
    path2 = [ NSString stringWithCString:argv[2] ];
    path3 = [ NSString stringWithCString:argv[3] ];
    
    log1 = [ [ [ TYFileLog alloc ] initWithContentsOfFile:path1 ] autorelease ];
    log2 = [ [ [ TYFileLog alloc ] initWithContentsOfFile:path2 ] autorelease ];
    
    [ log1 addTYFileLog:log2 ];
    
    [ log1 writeToFile:path3 ];

    return 0;
}
