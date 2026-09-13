//
//  TYSarArchiver.m
//  Tukuyomi
//
//  Created by toveta on Thu Jun 14 2001.
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

#import "TYSarArchiver.h"
#import <unistd.h>

/* えーと本来のCocoaプログラミング的にはこうした構造体を用意してNSValueオブジェクトを作るほうが正しい気がするのですが。動いているコードはいじるなの原則に従って手を付けない事にしてコメントアウト。
struct tySarArchivedFileInfo {
    unsigned fileOffset;
    unsigned fileLength;
};

typedef struct tySarArchivedFileInfo TYSarArchivedFileInfo;

static __inline__ TYSarArchivedFileInfo TYMakeSarArchivedFileInfo(unsigned offset,unsigned length)
{
    TYSarArchivedFileInfo info;
    info.fileOffset = offset;
    info.fileLength = length;
    return info;
}
*/

@implementation TYSarArchiver
+(id)archiverWithContentsOfFile:(NSString*)path
{
    return [ [ [ [ self class ] alloc ] initWithContentsOfFile:path ] autorelease ];
}

-(id)initWithContentsOfFile:(NSString*)path
{
    NSFileHandle *sarhandle;
    NSData *readDatabuf; // 読み込みバッファー
    unsigned char *readbufptr;
    char address[256];
    unsigned filenum;
    int i,j;
    NSAutoreleasePool *arp;
    unsigned fileOffset,fileLength;
    id *objectArray,*objectArrayTop,*keyArray,*keyArrayTop;
    int fd;
    
    // const unsigned readLength = 256;
    
    self = [ super init ];

    arp = [ [ NSAutoreleasePool alloc ] init ];

    // ファイルを読み込みモードでオープン
    sarhandle = [ NSFileHandle fileHandleForReadingAtPath:path ];
    if(!sarhandle){
        return nil;
    }
    
    // ファイル数の読み出し
    readDatabuf = [ sarhandle readDataOfLength:2 ];
    readbufptr = (unsigned char *)[ readDatabuf bytes ];
    filenum = (readbufptr[0] << 8) + readbufptr[1];

    // ファイル数分のオブジェクトへのポインタを格納する領域を確保
    objectArrayTop = objectArray = malloc(sizeof(id) *filenum);
    NSAssert((objectArray), @"malloc failed");
    keyArrayTop = keyArray = malloc(sizeof(id) *filenum);
    NSAssert((keyArray), @"malloc failed");
    
    // データ部の始点オフセットの読み出し
    readDatabuf = [ sarhandle readDataOfLength:4 ];
    readbufptr = (unsigned char *)[ readDatabuf bytes ];
    baseOffset = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];

    fd = [ sarhandle fileDescriptor ];

    // ファイルのインデックスを作成
    for( i=0; i < filenum; i++){
        
        j = 0;
        while(1){
            /*
            readDatabuf = [ sarhandle readDataOfLength:1 ]; // 改良点：まとめて256バイトぐらい読むようにしたいな。
            readbufptr = (unsigned char *)[ readDatabuf bytes ];
             */
            read(fd,readbufptr,1); 
            if(readbufptr[0]){
                // バックスラッシュはうざいのでスラッシュに変えます。
                if(readbufptr[0] == '\\') readbufptr[0] = '/';
                address[j] = readbufptr[0];
            } else {
                address[j] = '\0';
                // アドレスから文字列オブジェクトを作成し、配列に加える。
                *keyArray=[ NSString stringWithCString:address ];
                keyArray++;
                
                // 基点からのオフセットを読み出し
                readDatabuf = [ sarhandle readDataOfLength:4 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileOffset = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];

                // ファイルのバイト長を読み出し
                readDatabuf = [ sarhandle readDataOfLength:4 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileLength = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];
                
                // アーカイブ内ファイルオブジェクトを作成し、配列に加える
                *objectArray=[ TYSarArchivedFile archivedFileoffset:fileOffset length:fileLength ];
                objectArray++;
                
                break;
            }
            j++;
        }
        
    }
    
    // 辞書を登録
    filesDict = [ [ NSDictionary dictionaryWithObjects:objectArrayTop forKeys:keyArrayTop count:filenum ] retain ];
    free(objectArrayTop);
    free(keyArrayTop);

    // パスを登録
    arcPath = path;
    [ arcPath retain ];
    
    // ファイルをクローズ
    [ sarhandle closeFile ];

    [ arp release ];

    return self;
}

// アーカイブのファイルハンドルを取得し、指定データの開始位置にシークして返す。
-(NSFileHandle*)fileHandleWithPath:(NSString*)aPath length:(int*)length
{
    NSFileHandle *handle;
    TYSarArchivedFile *tmp;
    int fileOffset;

    tmp = [ filesDict objectForKey:aPath ];
    if(!tmp){
        return nil;
    }
    handle = [ NSFileHandle fileHandleForReadingAtPath:arcPath ];
    if(!handle){
        return nil;
    }
    [ tmp getFileDataOffset:&fileOffset length:length ];

    [ handle seekToFileOffset:fileOffset +baseOffset ];

    return handle;
}

// アーカイブ内からファイルを取得してきます。
-(NSData*)unArchivedFile:(NSString*)address;
{
    TYSarArchivedFile *tmp;
    unsigned fileOffset,fileLength;

    //NSLog(@"address[%@]\n@",address);
    //NSLog(@"%@\n",[ [ filesDict allKeys ] description ]);
    tmp = [ filesDict objectForKey:[ address lowercaseString ]];

    if(!tmp){
        //NSLog(@"failed search for Archive!!\narichive[%@]\npath[%@]\n",arcPath,address);
        return nil;
    }
    [ tmp getFileDataOffset:&fileOffset length:&fileLength ];

    return [ self readDataOffset:fileOffset length:fileLength ];
}

-(NSData*)readDataOffset:(unsigned)offset length:(unsigned)length
{
    NSFileHandle *handle;
    NSData *readData;

    handle = [ NSFileHandle fileHandleForReadingAtPath:arcPath ];
    
    if(!handle){
        return nil;
    }
    
    [ handle seekToFileOffset:offset +baseOffset];
    readData = [ handle readDataOfLength:length ];
    [ handle closeFile ];
    
    return readData;
}

-(void)dealloc
{
    [ filesDict release ];
    [ arcPath release ];
    
    [ super dealloc ];
}
@end
