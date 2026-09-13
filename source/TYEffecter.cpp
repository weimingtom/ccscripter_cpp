//
//  TYEffecter.m
//  Tukuyomi
//
//  Created by toveta on Mon Jun 18 2001.
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

#import "TYEffecter.h"


@implementation TYEffecter
+(id)effecter
{
    return [ [ [ [ self class ] alloc ] init ] autorelease ];
}

-(id)init
{
    self = [ super init ];
    
    // 0番をキャッシュのみ、１番、-2番(windoweffect)を瞬間表示で予約しておく
    ty_effectDict = [ [ NSMutableDictionary alloc ] initWithObjectsAndKeys:
                        [ TYEffectRegister effectRegisterType:TYEffectInstant time:0 path:nil ],
                        [ NSNumber numberWithInt:-2 ],
                        [ TYEffectRegister effectRegisterType:TYEffectCached time:0 path:nil ],
                        [ NSNumber numberWithInt:0 ],
                        [ TYEffectRegister effectRegisterType:TYEffectInstant time:0 path:nil ],
                        [ NSNumber numberWithInt:1 ],
                        nil ];
        
    return self;
}

-(void)setBeforeImage:(NSImage*)aImage
{
    if(beforeImage)
        [ beforeImage autorelease ];
    beforeImage = [ aImage copy ];
}
-(NSImage*)beforeImage
{
    return beforeImage;
}

-(void)setEffectNo:(int)effectNo type:(int)effectType time:(int)time path:(NSString*)path
{
    TYEffectRegister *tmp;
    
    // 引き数チェックはめんどくさいので省略
    
    tmp = [ TYEffectRegister effectRegisterType:effectType time:time path:path ];

    if(!tmp){
        return ;
    }
    
    [ ty_effectDict setObject:tmp forKey:[ NSNumber numberWithInt:effectNo ] ];
    
    return ;
}

-(BOOL)startEffect:(NSImage*)newImage effectNo:(int)effectNo
{
    TYEffectRegister *reg;
    int type;
    BOOL endFlg;
    
    reg = [ ty_effectDict objectForKey:[ NSNumber numberWithInt:effectNo ] ];

    type = [ reg getType ];

    // オフスクリーン更新のみなら無視して終了
    if(type == TYEffectCached){
        return YES;
    }

    [ afterImage autorelease ];
    afterImage = [ newImage copyWithZone:NULL ];
    //[ afterImage retain ];

    
    switch(type){
    case TYEffectInstant:
        [ beforeImage autorelease ];
        beforeImage = afterImage;
        [ beforeImage retain ];
        if(nowImage)
            [ nowImage autorelease ];
        nowImage = beforeImage;
        [ nowImage retain ];
        endFlg = YES;
        break;
    default:
        // バッファ領域の確保
        if(nowImage)
            [ nowImage autorelease ];
        nowImage = [ beforeImage copyWithZone:[ beforeImage zone ] ];
        // 現在時刻を取得
        if(startDate)
            [ startDate autorelease ];
        startDate =  [ [ NSDate date ] retain ];
        
        // 進行状況の初期化
        beforeParsentage = 0;
        
        nowEffect = reg;
        endFlg = NO;
    }
    
    return endFlg;
}

-(BOOL)getEffectionImage:(NSImage**)drawImage
{
    NSTimeInterval parsentage;

    // 推移の割合を取得
    parsentage = [ startDate timeIntervalSinceNow ] * -1 / ( [ nowEffect getTime ] / 1000.0 );
    
    if(parsentage >= 1 ){
        *drawImage = afterImage;
        [ beforeImage autorelease ];
        beforeImage = afterImage;
        [ beforeImage retain ];

        return YES;
    }
    
    [ nowImage lockFocus ];
    
    // エフェクト別の作成処理
    switch([ nowEffect getType ]){
    case TYEffectCrossFadeWithPixel:
    default:
        //[ beforeImage compositeToPoint:NSZeroPoint operation:NSCompositeCopy ];
        [ afterImage dissolveToPoint:NSZeroPoint fraction:(parsentage -beforeParsentage)];
        break;
    }
    
    [ nowImage unlockFocus ];
    
    beforeParsentage = parsentage;
    *drawImage = nowImage;
    
    return NO;
}


-(NSString*)description
{
    return [ ty_effectDict description ];
}

-(void)dealloc
{
    [ ty_effectDict release ];
    
    [ super dealloc ];
}
@end

// 以下、エフェクトデータオブジェクトの実装
@implementation TYEffectRegister
+(id)effectRegisterType:(int)type time:(int)time path:(NSString*)path
{
    return [ [ [ [ self class ] alloc ] initWithType:type time:time path:path ] autorelease ];
}

-(id)initWithType:(int)type time:(int)time path:(NSString*)path
{
    self = [ super init ];
    
    ty_effectType = type;
    ty_time = time;
    if(path)
        ty_path = [ [ NSString stringWithString:path ] retain ];
    
    return self;
}

-(int)getType
{
    return ty_effectType;
}

-(int)getTime
{
    return ty_time; 
}

-(NSString*)getPath
{
    return ty_path;
}

-(NSString*)description
{
    return [ NSString stringWithFormat:@"type:%d time:%d path:%@",ty_effectType,ty_time,ty_path ];
}

-(void)dealloc
{
    [ ty_path release ];
    [ super dealloc ];
}
@end

@implementation TYEffectDefinitionValue
+(id)valueWithEffectDefinition:(TYEffectDefinition)aDefinition
{
    return [ [ [ [ self class ] alloc ] initWithEffectDefinition:aDefinition ] autorelease ];
}

-(id)initWithEffectDefinition:(TYEffectDefinition)aDefinition
{
    self = [ super init ];

    if(aDefinition.path)
        [ aDefinition.path retain ];
    
    value = [ [ NSValue alloc ] initWithBytes:&aDefinition objCType:@encode(TYEffectDefinition) ];

    return self;
}

- (void)getValue:(void *)buffer
{
    [ value getValue:buffer ];
}

- (void)dealloc
{
    TYEffectDefinition aDefinition;
    
    [ value getValue:&aDefinition ];
    
    if(aDefinition.path)
        [ aDefinition.path release];
        
    [ value release ];
    
    [super dealloc];
}

@end