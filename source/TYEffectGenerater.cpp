//
//  TYEffectGenerater.m
//  Tukuyomi
//
//  Created by toveta on Sun Nov 11 2001.
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

#import "TYEffectGenerater.h"
#import "TYInstantGenerater.h"
#import "TYCrossFadeGenerater.h"
#import "TYScrollEffectGenerater.h"
#import "TYShutterEffectGenerater.h"
#import "TYCartainEffectGenerater.h"
#import "TYQuakeEffectGenerater.h"
#import "TYPatternFadeGenerater.h"
#import "TYPatternCrossFadeGenerater.h"
#import "TYMosaicEffectGenerater.h"

@implementation TYEffectGenerater
-(id)initWithBeforeImage:(NSImage*)befImage afterImage:(NSImage*)atImage effect:(TYEffectDefinitionValue*)effect
{
    TYEffectGenerater *generater;
    TYEffectDefinition def;
        
    self = [ super init ];

    [ effect getValue:&def ];    

    if(def.type==TYEffectCached){
        [ self performSelector:@selector(drawEffect) withObject:nil afterDelay:0 ];
        return self;
    }
    [ self autorelease ];
    
    switch(def.type){
    case TYEffectInstant:
        generater = [ TYInstantGenerater alloc ];
        break;
    case TYEffectLeftShutter:
    case TYEffectRightShutter:
    case TYEffectTopShutter:
    case TYEffectBottomShutter:
        generater = [ TYShutterEffectGenerater alloc ];
        break;
    case TYEffectLeftCartain:
    case TYEffectRightCartain:
    case TYEffectTopCartain:
    case TYEffectBottomCartain:
        generater = [ TYCartainEffectGenerater alloc ];
        break;
    case TYEffectRightScroll:
    case TYEffectLeftScroll:
    case TYEffectBottomScroll:
    case TYEffectTopScroll:
        generater = [ TYScrollEffectGenerater alloc ];
        break;
    case TYEffectMosaicOut:
    case TYEffectMosaicIn:
        generater = [ TYMosaicEffectGenerater alloc ];
        break;
    case TYEffectFadeWithMask:
        generater = [ [ TYPatternFadeGenerater alloc ] initWithBeforeImage:befImage afterImage:atImage effect:effect ];
        if(generater)
            return generater;
        else
            return [ [ TYCrossFadeGenerater alloc ] initWithBeforeImage:befImage afterImage:atImage effect:effect ];
        break;
    case TYEffectCrossFadeWithMask:
        generater = [ [ TYPatternCrossFadeGenerater alloc ] initWithBeforeImage:befImage afterImage:atImage effect:effect ];
        if(generater)
            return generater;
        else
            return [ [ TYCrossFadeGenerater alloc ] initWithBeforeImage:befImage afterImage:atImage effect:effect ];
        break;     
    default :
        generater = [ TYCrossFadeGenerater alloc ];
    }

    return [ generater initWithBeforeImage:befImage afterImage:atImage effect:effect ];
}

-(id)initWithImage:(NSImage*)image quakeType:(int)type times:(int)times time:(int)second
{
    [ super init ];
    [ self autorelease ];
    
    return [ [ TYQuakeEffectGenerater alloc ] initWithImage:image quakeType:type times:times time:second ];
}

-(void)setDelegate:(id)aDelegate
{
    delegate = aDelegate;
}

-(void)changeEffectionImage:(NSImage*)drawImage
{
    if(delegate){
        if([ delegate respondsToSelector:@selector(changeEffectionImage:) ]){
            [ delegate changeEffectionImage:drawImage ];
        }
    }
}

-(void)effectFinished
{
    if(delegate){
        if([ delegate respondsToSelector:@selector(effectFinished) ]){
            [ delegate effectFinished ];
        }
    }
}

-(void)drawEffect
{
    [ self effectFinished ];
}

- (void)dealloc
{
    [ beforeImage release ];
    [ afterImage release ];
    [ nowImage release ];
    [ super dealloc ];
}

@end
