//
//  TYResourceServer.m
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

#import "TYResourceServer.h"
#import "TYImageUtil.h"
#import "TY-NSStringAddition.h"
#import "TY_NSDataAddtion.h"
#import <unistd.h>
#import "TYNBZDecompresser.h"
#import "TYAnimationCellImage.h"
#import "TYStageManager.h"
#import "TYStringImage.h"
#import "TYMiscUtil.h"
#import "TYEffectPatternMap.h"
#import "TYCCSProxy.h"

static id sharedServer;

@implementation TYResourceServer

+(id)sharedServer
{
    if(!sharedServer){
        sharedServer = [ [ TYResourceServer alloc ] init ];
    }
    
    return sharedServer;
}

-(id)init
{
    [ super init ];
    
    ty_archivers = [ [ NSMutableArray arrayWithCapacity:1 ] retain ];
    effectPatternDict = [ [ NSMutableDictionary alloc ] initWithCapacity:1 ];
    
    defaultTransmode = DEFAULT_TRANSMODE;

    // みずいろDVD版用暫定処理。NBZプラグインをあらかじめロードする。
    [ self addSoundPressPlugin:nil extension:@"nbz" ];
    
    return self;
}

-(void)addArchiver:(NSString*)path
{
    TYArchiver *arc;
    NSString *arcPath;
    
    // path にDLLの指定が含まれていたら破棄する。
    arcPath = [ [ path componentsSeparatedByString:@"|" ] objectAtIndex:0 ];
    
    arc = [ [ TYArchiver alloc ] initWithContentsOfFile:
            [ getNScrRootDirectory() stringByAppendingPathComponent:arcPath ] ];
    
    if (arc) [ ty_archivers addObject:arc ];
}

-(NSString*)getFilePath:(NSString*)path
{
    NSString *convPath;

    convPath = winPathToUnix(path) ;
    convPath = [ getNScrRootDirectory() stringByAppendingPathComponent:convPath ];

    return ([ [ NSFileManager defaultManager ] fileExistsAtPath:convPath
                                                    isDirectory:NO ]) ? convPath : nil ;

}

-(NSString*)getFilePathMakeTemp:(NSString*)path
{
    NSString *diskPath;
    
    diskPath = [ self getFilePath:path ];
    if(diskPath)
        return diskPath;
    else
        return nil;
}

-(NSData*)getData:(NSString*)path
{
    NSEnumerator *enu;
    TYArchiver *arc;
    NSData *data;
    NSString *convPath;

    if((!path)||(![ path length])){
        return nil;
    }

    path = winPathToUnix(path);
    
    enu = [ ty_archivers objectEnumerator ];
    while( (arc = [ enu nextObject ]) != nil ){
        data = [ arc unArchivedFile:path ];
        if(data)
            return data;
    }
    
    // 登録されたアーカイバからファイルが取得できなかった。
    // パスをディスク内のものに変換してファイルを検索。
    convPath = [ getNScrRootDirectory() stringByAppendingPathComponent:path ];
    return [ NSData dataWithContentsOfFile:convPath ];
}

-(NSBitmapImageRep*)getBitmap:(NSString*)path
{
    NSString* extension;
    TYDecompresser<TYBitmapDecoder> *decompresser;
    NSEnumerator *enu;
    NSFileHandle *handle;
    id decodeObj;
    TYArchiver *arc;
    int length;
    NSDictionary *fileAttDict;

    path = [ path lowercaseString ];
    // プラグイン定義があればそれを使う。しかしなんですかこの汚いコードは
    if(spiPluginDict){
        extension = [ path pathExtension ];
        decompresser = [ spiPluginDict objectForKey:extension ];
        if(decompresser){
            enu = [ ty_archivers objectEnumerator ];
            while( (arc = [ enu nextObject ]) != nil ){
                handle = [ arc fileHandleWithPath:path length:&length ];
                if(handle){
                    return [ decompresser decodeBitmapWithFileHandle:handle length:length ];
                }
            }
            path = [ getNScrRootDirectory() stringByAppendingPathComponent:path ];
            if(fileAttDict = [ [ NSFileManager defaultManager ] fileAttributesAtPath:path traverseLink:NO ]){
                length = [ [ fileAttDict objectForKey:NSFileSize ] intValue ];
                handle = [ NSFileHandle fileHandleForReadingAtPath:path ];
                return [ decompresser decodeBitmapWithFileHandle:handle length:length ];
            }
            return nil;
        }
    }

    decodeObj = [ self getData:path ];
    // アーカイブ中のspb圧縮データはbitmapを返してくるので
    if([ decodeObj isKindOfClass:[ NSData class ] ]){
        return [ [ [ NSBitmapImageRep alloc ] initWithData:decodeObj ] autorelease ];
    } else {
        return decodeObj;
    }

    return nil;
}

-(NSImage*)getImage:(NSString*)path transMode:(BOOL)aBool animate:(BOOL)isAnimate
{
    NSString *filePath;
    NSArray *pathArray,*tagArray;
    NSString *transmode;
    NSBitmapImageRep *bitmap=nil;
    //NSArray *interValArray;
    NSNumber *intervalValue;
    NSString *animeOperate;
    //id aObj;

    // 画像タグ取得
    pathArray = [ path componentsSeparatedByString:@";" ];
    tagArray = ([ pathArray count ] != 1) ? [ [ pathArray objectAtIndex:0 ] componentsSeparatedByString:@"/" ] : nil;
    if(!aBool){
        transmode = TYTRANSMODE_COPY;
    } else if([ path length ] > 2 &&
              [ [ path substringWithRange:NSMakeRange(0,2) ] isEqualToString:@":s" ]){
        // 文字列スプライト
        return [ self getImageFromString:[ path substringFromIndex:2 ] ];
    } else if([ pathArray count ] == 1){
        transmode = defaultTransmode;
    } else {
        transmode = [ tagArray objectAtIndex:0 ];
        transmode = [ transmode substringFromIndex:1 ];
    }

    if(filelog){
        [ self addlog:path ];
    }

    
    // ファイルのパスを切り出し
    filePath = winPathToUnix([ [ path componentsSeparatedByString:@";" ] lastObject ]);

    bitmap = [ self getBitmap:filePath ];

    if(!bitmap){
        NSLog(@"failed to load Image from Archives and disk [%@]",path);
        return nil;
    }
    
    // アニメーションタグ取得
    if(isAnimate){
        int cellCount;
        //NSBitmapImageRep **bmpArrayPtr;
        //NSImage **imageArrayPtr;
        NSArray *bitmapArray;
        NSMutableArray *imageArray;

        if([ tagArray count ] == 2){
            int i;
            NSNumber *loopType;
            NSArray *animeOperateArray;
            //NSImage *trImage;
            
            animeOperate = [ tagArray lastObject ];

            /*
            if([ [ animeOperate substringWithRange:NSMakeRange(0,1) ] isEqualToString:@":" ]){
                // アニメ指定なし
                return [ self transrateImageFromBitmap:bitmap transMode:transmode ];
            }
             */
            
            // とりあえず単一インターバルのループのみで。
            animeOperateArray = [ animeOperate componentsSeparatedByString:@"," ];

            cellCount = [ [ animeOperateArray objectAtIndex:0 ] intValue ];
            intervalValue = [ NSNumber numberWithInt:[ [ animeOperateArray objectAtIndex:1 ] intValue ] ];
            loopType = [ NSNumber numberWithInt:[ [ animeOperateArray objectAtIndex:2 ] intValue ] ];

            [ bitmap resizeToPixelsSize ];
            bitmapArray = [ bitmap horizontalDivide:cellCount ];

            imageArray = [ NSMutableArray array ];
            for(i=0; i < cellCount; i++){
                [ imageArray addObject:[ self transrateImageFromBitmap:[ bitmapArray objectAtIndex:i ] transMode:transmode ] ];
            }
            
            return [ [ [ TYAnimationCellImage alloc ] initWithImages:imageArray
                                                          infoDict:[ NSDictionary dictionaryWithObjectsAndKeys:loopType,TYAnimationModeInfo,intervalValue,TYAnimationIntervalInfo,nil ] ] autorelease ];

        }
    }

    // ビットマップ変換
    return [ self transrateImageFromBitmap:bitmap transMode:transmode ];

}

-(NSImage*)getImage:(NSString*)path transMode:(BOOL)aBool
{
    return [ self getImage:path transMode:aBool animate:NO ];
}

-(NSImage*)transrateImageFromBitmap:(NSBitmapImageRep*)bitmap transMode:(NSString*)transmode // private
{
    NSImage *image;
    NSImageRep *rep;
    NSBitmapImageRep *maskBitmap;
    NSImage *maskImage;

    image = [ [ [ NSImage alloc ] initWithSize:NSZeroSize ] autorelease ];
    [ image addRepresentation:bitmap ];
    if([ transmode isEqualToString:TYTRANSMODE_ALPHA ]){
        // マスク付き画像ファイル
        image = ConvertImagePackedAlpha(image);
    } else if ([ transmode isEqualToString:TYTRANSMODE_COPY ]){
        //image = [ [ [ NSImage alloc ] initWithData:aData ] autorelease ];
        ;
    } else if ([ transmode isEqualToString:TYTRANSMODE_LEFTTOP ]){
        image = ConvertImageTransrateLeftTop(image);
    } else if ([ transmode isEqualToString:TYTRANSMODE_MASK ]){
        maskBitmap = [ self getBitmap:[ transmode substringFromIndex:2 ] ];
        maskImage = [ [ [ NSImage alloc ] initWithSize:NSZeroSize ] autorelease ];
        image = ConvertImageWithMaskImage(image,maskImage);
    } else {
        //image = [ [ [ NSImage alloc ] initWithData:aData ] autorelease ];
        ;
    }

    // pixelサイズに補正
    rep = [ image bestRepresentationForDevice:nil ];
    [ rep setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];
    [ image setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];

    return image;
}

-(NSImage*)getImageFromString:(NSString*)str
{
    //NSCharacterSet *cset = [ NSCharacterSet alphanumericCharacterSet ];
    NSString *buffer;
    NSMutableArray *colors = [ NSMutableArray array ];
    int i,len;
    TYStageManager *manager = [ TYStageManager sharedManager ];
    NSArray *images;
    BOOL isOrgSize=NO; // サイズ指定フラグ
    int fontWidth,fontHeight,fontInterval;
    NSMutableDictionary *textAttDict;

    // 文字列の変数展開
    str = [ [ TYScriptEngine sharedEngine ] stringOfReplaceVars:str ];
    
    if([ [ str substringWithRange:NSMakeRange(0,1) ] isEqualToString:@"/" ]){
        // サイズ指定付きスプライト
        NSString *substr;
        NSScanner *scanner;
        
        isOrgSize = YES;
        scanner = [ NSScanner scannerWithString:[ str substringFromIndex:1 ] ];
        
        [ scanner scanUpToString:@"," intoString:&substr ];
        fontWidth = [ substr intValue ];
        [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
        [ scanner scanUpToString:@"," intoString:&substr ];
        fontHeight = [ substr intValue ];
        [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
        [ scanner scanUpToString:@";" intoString:&substr ];
        fontInterval = [ substr intValue ];
        [ scanner setScanLocation:[ scanner scanLocation ] +1 ];
        // フォントサイズは縦、横の小さい方に合わせる
        if(fontWidth < fontHeight)
            fontHeight = fontWidth;
        else if(fontWidth > fontHeight)
            fontWidth = fontHeight;
        
        textAttDict = [ [ manager textAttDict ] mutableCopy ];
        {
            NSFont *aFont = [ textAttDict objectForKey:NSFontAttributeName ];
            [ textAttDict setObject:[ NSFont fontWithName:[ aFont fontName ] size:fontHeight ] 
                            forKey:NSFontAttributeName ];
        }

        str = [ [ scanner string ] substringFromIndex:[ scanner scanLocation ] ];
    }
    
    i=0;
    len = [ str length ];
    while((i < len) && [ [ str substringWithRange:NSMakeRange(i,1)] isEqualToString:@"#" ]){
        buffer = [ str substringWithRange:NSMakeRange(i,7) ];
        [ colors addObject:getColorWithHTMLFormat(buffer) ];
        i+=7;
    }

    images = [ TYStringImage imagesWithString:[ str substringFromIndex:i ]
                                   attributes:(!isOrgSize) ? [ manager textAttDict ] : textAttDict 
                                       colors:colors
                                   fontHeight:(!isOrgSize) ? [ manager textFontHeight ] : fontHeight
                                     fontWidth:(!isOrgSize) ? [ manager textFontWidth ] : fontWidth
                                     interval:[ manager textPitchx ]
                                       shadow:[ manager isShadow ] ];

    return [ [ [ TYCellImage alloc ] initWithImages:images ] autorelease ];
}

-(void)addSpi:(NSString*)pluginName extension:(NSString*)extension
{
    // 画像データ解凍プラグインのロード

    if(!spiPluginDict){
        spiPluginDict = [ [ NSMutableDictionary dictionary ] retain ];
    }

    // MEMO:いずれ外部バンドルの読み込みを可能にする。
    extension = [ extension lowercaseString ];
    if([ extension isEqualToString:@"nbz" ]){
        [ spiPluginDict setObject:[ [ [ TYNBZDecompresser alloc ] init ] autorelease ] forKey:extension ];
    }

}

-(void)addSoundPressPlugin:(NSString*)pluginName extension:(NSString*)extension;
{
    // 音声データ解凍プラグインのロード

    if(!soundPressPluginDict){
        soundPressPluginDict = [ [ NSMutableDictionary dictionary ] retain ];
    }

    // MEMO:いずれ外部バンドルの読み込みを可能にする。
    extension = [ extension lowercaseString ];
    if([ extension isEqualToString:@"nbz" ]){
        [ soundPressPluginDict setObject:[ [ [ TYNBZDecompresser alloc ] init ] autorelease ] forKey:extension ];
    }

}

-(NSData*)getSoundData:(NSString*)path
{
    NSEnumerator *enu;
    TYArchiver *arc;
    NSData *data;
    NSString *convPath;
    NSString *extension;
    TYDecompresser<TYDataDecoder> *decompresser;
    NSFileHandle *handle;
    int length;
    NSDictionary *fileAttDict;

    if((!path)||(![ path length])){
        return nil;
    }

    // パスのデリミタを変換
    convPath = winPathToUnix(path);
    convPath = [ convPath lowercaseString ];

    // プラグイン定義があればそれを使う。しかしなんですかこの汚いコードは
    if(soundPressPluginDict){
        extension = [ convPath pathExtension ];
        decompresser = [ soundPressPluginDict objectForKey:extension ];
        if(decompresser){
            enu = [ ty_archivers objectEnumerator ];
            while( (arc = [ enu nextObject ]) != nil ){
                handle = [ arc fileHandleWithPath:convPath length:&length ];
                if(handle){
                    data = [ decompresser decodeDataWithFileHandle:handle length:length ];
                    return data;
                }
            }
            convPath = [ getNScrRootDirectory() stringByAppendingPathComponent:convPath ];
            if(fileAttDict = [ [ NSFileManager defaultManager ] fileAttributesAtPath:convPath traverseLink:NO ]){
                length = [ [ fileAttDict objectForKey:NSFileSize ] intValue ];
                handle = [ NSFileHandle fileHandleForReadingAtPath:convPath ];
                data = [ decompresser decodeDataWithFileHandle:handle length:length ];
                return data;
            }
            return nil;
        }
    }

    return [ self getData:convPath ];
}

-(void)addlog:(NSString*)filename
{
    [ filelogDict setObject:[ NSNumber numberWithBool:YES ] forKey:[ filename uppercaseString ] ];
}

-(void)setDefaultTransMode:(NSString*)transmode
{
    defaultTransmode = [ transmode retain ];
}

-(BOOL)fchk:(NSString*)filename
{
    return ([ filelogDict objectForKey:[ filename uppercaseString ] ]) ? YES : NO;
}

-(BOOL)filelog:(NSString*)path;
{
    NSFileHandle *handle;
    NSMutableData *bufData;
    int fileNum;
    int descripter;
    unsigned char buf;
    int i,ret;

    filelogDict = [ [ NSMutableDictionary alloc ] initWithCapacity:1 ];

    // MEMO:ここでまずログを書き出すディレクトリの書き込み権限チェックを入れる。

    filelog=YES;

    handle = [ NSFileHandle fileHandleForReadingAtPath:path ];
    if(!handle){
        return YES;
    }
    descripter = [ handle fileDescriptor ];
    
    /*
    filelogDict = [ [ NSMutableDictionary alloc ] initWithContentsOfFile:
        [ getAppDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] ];
    */


    bufData = [ NSMutableData dataWithCapacity:1 ];
    while(read(descripter,&buf,1)){
        if(buf == 0x0a)
            break;
        else
            [ bufData appendBytes:&buf length:1 ];
    }
    [ bufData appendBytes:"" length:1 ];
    fileNum = atoi([ bufData bytes ]);

    for(i=0; i < fileNum; i++){
        ret = read(descripter,&buf,1);
        if((!ret)||(buf != '"'))
            break;
            
        bufData = [ NSMutableData dataWithCapacity:1 ];
        
        while(read(descripter,&buf,1)&&(buf!='"')){
            [ bufData appendBytes:&buf length:1 ];
        }
        [ bufData XORMaskToAllBytes:132 ]; // XORマスクをかけられているので戻す。
        [ bufData appendBytes:"" length:1 ];

        [ filelogDict setObject:[ NSNumber numberWithBool:YES ]
                        forKey:[ NSString stringWithCSJISString:[ bufData bytes ] ] ];
    }
    
    [ handle closeFile ];
    
    return YES;
}

-(BOOL)saveLog:(NSString*)path
{
    char numbuf[12];
    NSMutableData *writeData;
    NSMutableData *data;
    int count;
    NSEnumerator *enm;
    id temp;
    
    if(filelog){
        //[ filelogDict writeToFile:[ getAppDirectory() stringByAppendingPathComponent:FILELOG_FILENAME ] atomically:YES ];
        writeData = [ NSMutableData dataWithCapacity:2 ];
        count = [ filelogDict count ];
        sprintf(numbuf,"%d\n",count);
        [ writeData appendBytes:numbuf length:strlen(numbuf) ];
        
        enm = [ filelogDict keyEnumerator ];
        while((temp = [ enm nextObject ]) != nil){
            [ writeData appendBytes:"\"" length:1 ];
            data = [ [ [ temp sjisData ] mutableCopy ] autorelease ];
            [ data XORMaskToAllBytes:132 ];
            [ writeData appendData:data ];
            [ writeData appendBytes:"\"" length:1 ];
        }
        [ writeData writeToFile:path atomically:YES ];
    }
    
    return YES;
}

-(void)addEffectPattern:(NSString*)path
{
    TYEffectPatternMap *pattern;
    NSBitmapImageRep *bmp;
    
    bmp = [ self getBitmap:path ];
    if(bmp){
        pattern = [ [ TYEffectPatternMap alloc ] initWithBitmap:bmp ];
        if(pattern){
            [ effectPatternDict setObject:pattern forKey:path ];
            [ pattern release ];
        }
    }
}

-(TYEffectPatternMap*)getEffectPattern:(NSString*)path
{
    id tmp;
    if(tmp = [ effectPatternDict objectForKey:path ])
        return tmp;
    
    [ self addEffectPattern:path ];
    return [ effectPatternDict objectForKey:path ];
}

-(void)executeBundle:(NSString*)pathAndArg
{
    NSString *bundlePath,*argStr = nil;
    NSBundle *bundle;
    id<TYExecutablePlugin> plugin;

    // DLL名と引数のパース
    {
        NSScanner *scanner = [ NSScanner scannerWithString:pathAndArg ];
        NSString *str;
        NSString *dllPath;
        
        if(![ scanner scanUpToString:@"/" intoString:&str ]) {
            dllPath = pathAndArg;
        } else {
            dllPath = str;
            argStr = [ pathAndArg substringFromIndex:[ scanner scanLocation ] +1 ];
        }
        
        bundlePath = [ [ dllPath stringByDeletingPathExtension ] stringByAppendingPathExtension:@"bundle" ];
    }
    
    plugin = [ executableBundles objectForKey:bundlePath ];
    if(plugin == nil) {
        bundle = [ NSBundle bundleWithPath:[ getAppDirectory() stringByAppendingPathComponent:bundlePath ] ];
        [ bundle load ];
        plugin = [ [ [ [ bundle principalClass ] alloc ] init ] autorelease ];

        if(plugin == nil) {
            NSLog(@"can't load bundle %@",bundlePath);
            return;
        }
        
        [ executableBundles setObject:plugin forKey:bundlePath ];
    }
    
    [ plugin execPlugin:[ TYCCSProxy proxy ] argment:argStr];
}

- (void)dealloc
{
	[ ty_archivers release];
	[super dealloc];
}

@end
