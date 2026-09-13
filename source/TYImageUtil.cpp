//
//  TYImageUtil.m
//  Tukuyomi
//
//  Created by toveta on Mon Aug 27 2001.
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

#import "TYImageUtil.h"
#import "TYResourceServer.h"

void getRGBWithHTMLFormat(NSString* formatstr,int* r,int* g,int* b)
{
    char formptr[8];

    if( [ formatstr cStringLength ] != 7 ){
        return;
    }
    
    [ formatstr getCString:formptr ] ;
    
    *b = strtol(&formptr[5],NULL,16);
    formptr[5] = '\0';
    *g = strtol(&formptr[3],NULL,16);
    formptr[3] = '\0';
    *r = strtol(&formptr[1],NULL,16);
    
}

NSColor *getColorWithHTMLFormat(formatstr)
NSString *formatstr;
{
    NSColor *color;
    int r,g,b;

    getRGBWithHTMLFormat(formatstr,&r,&b,&g);
    
    color = [ NSColor colorWithCalibratedRed: r / 255.0
                        green: g / 255.0
                        blue: b / 255.0
                        alpha: 1 ];
    
    return color;
}

NSColor* getTextWindowColorWithHTMLFormat(formatstr)
NSString *formatstr;
{
    NSColor *color;
    int r,g,b,maxp,minp;
    
    getRGBWithHTMLFormat(formatstr,&r,&b,&g);

    if( r > g ){
        maxp = ( r > b ) ? r : b;
        minp = ( g < b ) ? g : b;
    } else {
        maxp = ( g > b ) ? g : b;
        minp = ( r < b ) ? r : b;
    }
    
    color = [ NSColor colorWithCalibratedRed: ( r -minp) / 255.0
                                    green: ( g -minp) / 255.0
                                    blue:( b-minp) /255.0
                                    alpha:(255 -maxp ) /255.0 ];
    
    return color;
}

// この関数はResourceServerに内蔵した
/*
NSImage* getTransrateImage(bitmap,transmode)
NSBitmapImageRep *bitmap;
NSString *transmode;
{
    NSImage *image;
    NSImageRep *rep;
    NSData *maskData;

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
        // 関数化してあるのにTYResouceServerに依存しちゃった。あはは。
        maskData = [ [ TYResourceServer sharedServer ] getImage:[ transmode substringFromIndex:2 ] ];
        image = ConvertImageWithMaskImage([ [ [ NSImage alloc ] initWithData:aData ] autorelease ],
                                            [ [ [ NSImage alloc ] initWithData:maskData ] autorelease ]);
    } else {
        image = [ [ [ NSImage alloc ] initWithData:aData ] autorelease ];
    }
    
    // dpi情報が入っていると、pixelサイズにならないので補正
    rep = [ image bestRepresentationForDevice:nil ];
    [ rep setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];
    [ image setSize:NSMakeSize([ rep pixelsWide ],[ rep pixelsHigh ]) ];
    
     return image; 
}
*/

// いじくりまわし中のコードなのでちょっと未整理です、これ。
NSImage* ConvertImagePackedAlpha(srcImage)
NSImage *srcImage;
{
    NSImage		*image;
    NSBitmapImageRep 	*srcBitmap;
    unsigned char 	*srcData,*destData;
    id 			tmp;
    int			spp,width,height,source_width;
    NSBitmapImageRep* destBitmap;
    int			sI,dI,wI,hI;
    int			*idestData;
    int			alpha;
    
    /* 
    if ( !(srcImage) || !([ srcImage isValid ])) {
        return nil;
    }

    tmp = [ [ srcImage representations ] objectAtIndex:0 ];

    if(!([ tmp isKindOfClass : [ NSBitmapImageRep class ] ]))
    {
        return nil;	// imageはnilのまま。
    } 
    */
    
    tmp = [ [ [ NSBitmapImageRep alloc ] initWithData:[ srcImage TIFFRepresentation ] ] autorelease ];    
    srcBitmap = (NSBitmapImageRep *)tmp;
    if(srcBitmap == nil) return nil;
    srcData = [ srcBitmap bitmapData ];
    
    spp = [ srcBitmap bitsPerPixel ] / 8;
    source_width = [ srcBitmap pixelsWide ];
    width = source_width / 2 ; // 右半分はマスクなので
    height = [ srcBitmap pixelsHigh ];

    image = [ [ NSImage alloc ] initWithSize:NSMakeSize(width,height) ];
    destBitmap = [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL
                                                                pixelsWide:width
                                                                pixelsHigh:height
                                                                bitsPerSample:[srcBitmap bitsPerSample]
                                                                samplesPerPixel:4
                                                                hasAlpha:YES
                                                                isPlanar:NO
                                                                colorSpaceName:NSCalibratedRGBColorSpace    
                                                                bytesPerRow:0
                                                                bitsPerPixel:0 ];

    [ image addRepresentation:destBitmap ];
    [ destBitmap autorelease ];
    
    destData = [ destBitmap bitmapData ];
    idestData = (int *)destData;

    for (sI=0,dI=0,wI=0,hI=0; ; sI+=spp,dI+=4,wI++)
    {
        if( wI >= width){
            wI = 0;
            hI++;
            if(hI >= height)
                break;
            sI = (spp *source_width *hI);
        }
        alpha = (0xFF - srcData[sI +width *spp] );
        
        destData[dI] = srcData[sI] *alpha / 0xFF;
        destData[dI +1] = srcData[sI + 1] *alpha / 0xFF;
        destData[dI +2] = srcData[sI + 2] *alpha / 0xFF;
        destData[dI +3] = alpha;
    }
    
    return [ image autorelease ];

}

NSImage* ConvertImageTransrateLeftTop(NSImage* srcImage)
{
    NSImage		*image;
    NSBitmapImageRep 	*srcBitmap;
    unsigned char 	*srcData,*destData;
    id 			tmp;
    unsigned char 	trR,trG,trB;
    int			spp,width,height;
    NSBitmapImageRep* destBitmap;
    int			bytePerPlane;
    int			sI,dI;    
    
    tmp = [ [ [ NSBitmapImageRep alloc ] initWithData:[ srcImage TIFFRepresentation ]
 ] autorelease ];
 
    srcBitmap = (NSBitmapImageRep *)tmp;
    if(srcBitmap == nil) return nil;
    srcData = [ srcBitmap bitmapData ];
    
    trR = srcData[0];
    trG = srcData[1];
    trB = srcData[2];
    spp = [ srcBitmap samplesPerPixel ];
    width = [ srcBitmap pixelsWide ];
    height = [ srcBitmap pixelsHigh ];

    destBitmap = [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL
                                                            pixelsWide:width
                                                            pixelsHigh:height
                                                         bitsPerSample:[srcBitmap bitsPerSample]
                                                       samplesPerPixel:4
                                                              hasAlpha:YES
                                                              isPlanar:NO
                                                        colorSpaceName:NSCalibratedRGBColorSpace                                                                                      								bytesPerRow:0
                                                          bitsPerPixel:0 ];
    [ destBitmap autorelease ];
    destData = [ destBitmap bitmapData ];
    
    bytePerPlane = [ srcBitmap bytesPerPlane ];
    for (sI = 0,dI =0; sI < bytePerPlane; sI += 3, dI += 4)
    {

        if(!(srcData[sI] == trR && srcData[ sI +1 ] == trG && srcData[sI + 2] == trB))
        {

            destData[dI] = srcData[sI];
            destData[dI +1] = srcData[sI + 1];
            destData[dI +2] = srcData[sI + 2];
            destData[dI +3] = 0xFF;
        } else {
            destData[dI] = 0;
            destData[dI +1] = 0;
            destData[dI +2] = 0;
            destData[dI +3] = 0;
        }
    }
    
    [ destBitmap setSize:NSMakeSize(width,height) ];

    image = [ [ NSImage alloc ] initWithSize:NSMakeSize(width,height) ];
    [ image addRepresentation:destBitmap ];
    
    return [ image autorelease ];
}


NSImage* ConvertImageWithMaskImage(NSImage* srcImage,NSImage* maskImage)
{
    NSImage *convImage;
    NSBitmapImageRep *convBitmap,*selfBitmap,*maskBitmap;
    unsigned char *convByte,*selfByte,*maskByte;
    NSSize selfSize,maskSize;
    int selfBPP,maskBPP;
    unsigned convBitmapBytesLength;
    unsigned i;
    int alpha;
    
    // ソースとマスクのビットマップを取得
    selfBitmap = [ [ [ NSBitmapImageRep alloc ] initWithData:[ srcImage TIFFRepresentation ] ] autorelease ];
    selfByte = [ selfBitmap bitmapData ];
    selfBPP = [ selfBitmap bitsPerPixel ] / 8;
    
    maskBitmap = [ [ [ NSBitmapImageRep alloc ] initWithData:[ srcImage TIFFRepresentation ] ] autorelease ];
    maskByte = [ maskBitmap bitmapData ];
    maskBPP = [ maskBitmap bitsPerPixel ] / 8;

    // ソースとマスクの画像サイズは同一であること。
    selfSize = NSMakeSize([ selfBitmap pixelsWide ],[ selfBitmap pixelsHigh ]);
    maskSize = NSMakeSize([ maskBitmap pixelsWide ],[ maskBitmap pixelsHigh ]);
    if(!NSEqualSizes(selfSize,maskSize)){
        NSLog(@"Not Equal size of maskImage to sourceImage!!\n");
        return nil;
    }
    
    // 新規NSBitmapImageRepを作成し、そのビットマップを取得
    convBitmap = [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL
                                                                pixelsWide:[ selfBitmap pixelsWide ]
                                                                pixelsHigh:[ selfBitmap pixelsHigh ]
                                                                bitsPerSample:[selfBitmap bitsPerSample]
                                                                samplesPerPixel:4
                                                                hasAlpha:YES
                                                                isPlanar:NO
                                                                colorSpaceName:NSCalibratedRGBColorSpace                                                                                      								bytesPerRow:0
                                                                bitsPerPixel:0 ];
    [ convBitmap autorelease ];
    convByte = [ convBitmap bitmapData ];
    convBitmapBytesLength = [ convBitmap bytesPerPlane ];
    // ソースとマスクから新規イメージのビットマップに値をコピー。
    for(i=0; i < convBitmapBytesLength; i+=4,selfByte+=selfBPP,maskByte+=maskBPP){
        alpha = 0xFF -(maskByte[0] +maskByte[1] +maskByte[2] ) / 3;
        *(convByte +i) = *selfByte *alpha / 0xFF;
        *(convByte +i +1) = *(selfByte +1) *alpha / 0xFF;
        *(convByte +i +2) = *(selfByte +2) *alpha / 0xFF;
        *(convByte +i +3) = alpha;
    }
    
    // 新規イメージを作成し、ビットマップを加える
    convImage = [ [ [ NSImage alloc ] initWithSize:NSMakeSize([ convBitmap pixelsWide ],[convBitmap pixelsHigh ]) ] autorelease ];
    [ convImage addRepresentation:convBitmap ];
    
    return convImage;

}

// 24bit Bitmapを作って返す。
NSBitmapImageRep* blankBitmap(int width,int height)
{
    return [ [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:3 hasAlpha:NO isPlanar:NO colorSpaceName:NSCalibratedRGBColorSpace bytesPerRow:0 bitsPerPixel:0 ] autorelease ]; 
 
}

NSString* TYTransmodeSymbolFromString(NSString* key)
{
    static NSDictionary *dict;

    if(!dict){
        dict = [ [ NSDictionary dictionaryWithObjectsAndKeys:TYTRANSMODE_LEFTTOP,@"leftup",
            TYTRANSMODE_RIGHTTOP,@"rightup",
            TYTRANSMODE_COPY,@"copy",
            TYTRANSMODE_ALPHA,@"alpha",nil ] retain ];
    }

    return [ dict objectForKey:[ key lowercaseString ] ];
}

@implementation NSImage (TYBitmapConvertExtension)
-(void)convertMonocroWithSample:(const unsigned char *)sample priority:(TYNegaPriority)priority;
{
    NSBitmapImageRep *bmprep=nil;
    const unsigned char *ptr;
    int width,height;
    
    if( priority || sample){
        bmprep = [ [ [ NSBitmapImageRep alloc ] initWithData:[ self TIFFRepresentation ] ] autorelease ];
    }
    
    if(!bmprep){
            return;
    }
    
    if(priority==TYNegaLowPriority){
        [ bmprep nega ];
    }
        
    if(sample){
        [ bmprep monocro:sample ];
    }
        
    if(priority==TYNegaHighPriority){
        [ bmprep nega ];
    }
    
    ptr = [ bmprep bitmapData ];
    width = [ bmprep pixelsWide ];
    height = [ bmprep pixelsHigh ];
    [ self lockFocus ];
    NSDrawBitmap(NSMakeRect(0,0,width,height),width,height,8,3,24,width *3,NO,NO,NSCalibratedRGBColorSpace,&ptr);
    [ self unlockFocus ];
}
@end

@implementation NSBitmapImageRep (TYBitmapConvertExtension)
-(void)monocro:(const unsigned char *)sample
{
    int pixels,spp;
    unsigned char *ptr;
    unsigned char current;
    int i;
    
    pixels = [ self pixelsWide ] *[ self pixelsHigh ];
    spp = [ self samplesPerPixel ];
    ptr = [ self bitmapData ];
    for(i=0; i < pixels; i++,ptr+=spp){
        current = (ptr[0] + ptr[1] + ptr[2]) / 3;
        ptr[0] = sample[0] * current / 0xFF;
        ptr[1] = sample[1] * current / 0xFF;
        ptr[2] = sample[2] * current / 0xFF;
    }
}

-(void)nega
{
    int pixels,spp;
    unsigned char *ptr;
    int i;
    
    pixels = [ self pixelsWide ] *[ self pixelsHigh ];
    spp = [ self samplesPerPixel ];
    ptr = [ self bitmapData ];
    for(i=0; i < pixels; i++,ptr+=spp){
        ptr[0] = 0xFF -ptr[0];
        ptr[1] = 0xFF -ptr[1];
        ptr[2] = 0xFF -ptr[2];
    }
}
@end

@implementation NSBitmapImageRep (TYBitmapDivideExtension)
-(NSArray*)horizontalDivide:(int)count
{
    int i,j,k;
    int width,height,bps,spp,pBits,piece_width;
    unsigned char *source,*dest;
    int sI,dI;
    int sbpr,dbpp;
    NSMutableArray *array = [ NSMutableArray array ];
    NSBitmapImageRep *destBitmap;

    width = [ self pixelsWide ];
    piece_width = width / count;
    // 分割数がセルのピクセル幅より大きい場合の対処。
    if(piece_width == 0)
        piece_width = 1;
    height = [ self pixelsHigh ];

    bps = [ self bitsPerSample ];
    spp = [ self samplesPerPixel ];
    pBits = [ self bitsPerPixel ] /8;
    source = [ self bitmapData ];
    sbpr = [ self bytesPerRow ];

    for(i=0; i < count; i++){
        
        /* SPP=3でないと後でEXEC_BAD_ACCESSに。原因は謎。
         */
        destBitmap = [ [ NSBitmapImageRep alloc ] initWithBitmapDataPlanes:NULL
                                                   pixelsWide:piece_width
                                                   pixelsHigh:height
                                                bitsPerSample:8
                                              samplesPerPixel:3
                                                     hasAlpha:NO
                                                     isPlanar:NO
                                               colorSpaceName:NSCalibratedRGBColorSpace
                                                  bytesPerRow:0
                                                 bitsPerPixel:0 ];
        [ destBitmap autorelease ];

        dest = [ destBitmap bitmapData ];
        dbpp = [ destBitmap bytesPerPlane ];

        dI=0;
        for(j=0; j < height; j++){
            // sourceのbytesPerRowを元に、一列ごとにsIを設定しなおすこと。
            // 列の後ろにゴミが付いている場合がある。
            sI = piece_width *pBits *i +  sbpr *j;
            for(k=0; k < piece_width; k++,sI+=pBits,dI+=3){
                dest[dI] = source[sI];
                dest[dI+1] = source[sI+1];
                dest[dI+2] = source[sI+2];
            }
        }

        [ array addObject:destBitmap ];        
    }

    return array;
}

@end

@implementation NSImageRep (TYImageRepUtil)
-(void)resizeToPixelsSize
{
    [ self setSize:NSMakeSize([ self pixelsWide ],[ self pixelsHigh ]) ];
}
@end
