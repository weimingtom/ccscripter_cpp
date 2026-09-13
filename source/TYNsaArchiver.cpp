//
//  TYNsaArchiver.m
//  Tukuyomi
//
//  Created by toveta on Sat Oct 13 2001.
//  Copyright (c) 2001 toveta All rights reserved.
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

#import "TYNsaArchiver.h"
#import <unistd.h>
#import "lzss.h"
#import "spb.h"
//#import "TYBGLZSSDecorder.h"

static TYNsaFileMode NsaArchiverMode = TYNormalNsaFileMode;

@implementation TYNsaArchiver

+(void)setMode:(TYNsaFileMode)aMode
{
    NsaArchiverMode = aMode;
}

-(id)initWithContentsOfFile:(NSString*)path
{
    NSFileHandle *handle;
    NSData *readDatabuf; // 読み込みバッファー
    unsigned char *readbufptr;
    char address[256];
    unsigned filenum;
    int i,j;
    NSAutoreleasePool *arp;
    unsigned fileCompress,fileOffset,fileLength,fileExLength;
    id *objectArray,*objectArrayTop,*keyArray,*keyArrayTop;
    int fd;
    
    // const unsigned readLength = 256;
    
    self = [ super init ];

    arp = [ [ NSAutoreleasePool alloc ] init ];

    // ファイルを読み込みモードでオープン
    handle = [ NSFileHandle fileHandleForReadingAtPath:path ];
    if(!handle){
        return nil;
    }
    
    // ファイル数の読み出し
    switch (NsaArchiverMode) {
    case TY2TypeNsaFileMode:
        [ handle readDataOfLength:1 ];
        break;
    case TY3TypeNsaFileMode:
        [ handle readDataOfLength:2 ];
        break;
    default:
        break;
    }
    readDatabuf = [ handle readDataOfLength:2 ];
    readbufptr = (unsigned char *)[ readDatabuf bytes ];
    filenum = (readbufptr[0] << 8) + readbufptr[1];

    // ファイル数分のオブジェクトへのポインタを格納する領域を確保
    objectArrayTop = objectArray = malloc(sizeof(id) *filenum);
    NSAssert((objectArray), @"malloc failed");
    keyArrayTop = keyArray = malloc(sizeof(id) *filenum);
    NSAssert((keyArray), @"malloc failed");
    
    // データ部の始点オフセットの読み出し
    readDatabuf = [ handle readDataOfLength:4 ];
    readbufptr = (unsigned char *)[ readDatabuf bytes ];
    baseOffset = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];
    switch (NsaArchiverMode) {
    case TY2TypeNsaFileMode:
        baseOffset++;
        break;
    case TY3TypeNsaFileMode:
        baseOffset += 2;
        break;
    default:
        break;
    }
    fd = [ handle fileDescriptor ];
    
    // ファイルのインデックスを作成
    for( i=0; i < filenum; i++){
        
        j = 0;
        while(1){
            /*
            readDatabuf = [ handle readDataOfLength:1 ]; // MEMO:つーかディスクリプタを取得してfgetc()する方がコスト低いような。
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
                // MEMO:ここで日本語の使われたアーカイブで死ぬ可能性がある。NSDataで持つべき。
                *keyArray=[ NSString stringWithCString:address ];
                keyArray++;
                
                // 圧縮タイプの読み出し
                readDatabuf = [ handle readDataOfLength:1 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileCompress = *readbufptr;
                
                // 基点からのオフセットを読み出し
                readDatabuf = [ handle readDataOfLength:4 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileOffset = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];

                // ファイルのバイト長を読み出し
                readDatabuf = [ handle readDataOfLength:4 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileLength = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];
                
                // ファイルの展開後バイト長を読み出し
                readDatabuf = [ handle readDataOfLength:4 ];
                readbufptr = (unsigned char *)[ readDatabuf bytes ];
                fileExLength = (readbufptr[0] << 24) +(readbufptr[1] << 16)  +(readbufptr[2] << 8) +readbufptr[3];
                
                
                // アーカイブ内ファイルオブジェクトを作成し、配列に加える
                *objectArray=[ TYNsaArchivedFile archivedFileoffset:fileOffset length:fileLength type:fileCompress exLength:fileExLength ];
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
    [ handle closeFile ];

    [ arp release ];

    return self;
}

// アーカイブ内からファイルを取得してきます。
-(id)unArchivedFile:(NSString*)address;
{
    TYNsaArchivedFile *tmp;
    unsigned fileOffset,fileLength;

    //NSLog(@"address[%@]\n@",address);
    //NSLog(@"%@\n",[ [ filesDict allKeys ] description ]);
    tmp = [ filesDict objectForKey:[ address lowercaseString ] ];

    if(!tmp){
        //NSLog(@"failed search for Archive!!\narichive[%@]\npath[%@]\n",arcPath,address);
        return nil;
    }
    [ tmp getFileDataOffset:&fileOffset length:&fileLength ];

    switch ([ tmp compressType ]){
        NSFileHandle *fh;
        case 0:
            // 圧縮されていないデータならそのまま返す
            return [ self readDataOffset:fileOffset length:fileLength ];
        case 1:
            return TYGetSpbWithData([ self readDataOffset:fileOffset length:fileLength ],
                                    fileLength,
                                    [ tmp expandedLength ]);
        case 2:
            fh=[ NSFileHandle fileHandleForReadingAtPath:arcPath ];
            [ fh seekToFileOffset:fileOffset +baseOffset];
            return TYDataWithLzssDecodeFromFile(fh,fileLength,[ tmp expandedLength ]);
            //return [ TYBGLZSSDecorder decodeWithFileHandle:fh encode_size:fileLength original_size:[ tmp expandedLength ] ];
    }

    /*
    if([ tmp compressType ] == 0){
        return [ self readDataOffset:fileOffset length:fileLength ]; // 圧縮されていないデータならそのまま返す
    }
    
    // compressType==2 lzss形式
    if([ tmp compressType ] == 2){
        
    }
     */
    

    
    return nil;
}

@end
