//
//  TYQTMusicPlayer.m
//  Tukuyomi
//
//  Created by toveta on Thu Aug 29 2002.
//  Copyright (c) 2002 toveta All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * &ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "TYQTMusicPlayer.h"
#include <QuickTime/QuickTime.h>

@implementation TYQTMusicPlayer

+(void)initialize
{
    static BOOL aBool;
    
    if(aBool)
        return;
    aBool = YES;

    /* we must initialize the movie toolbox before calling
        any of it's functions */
    EnterMovies();    
}

- (id)initWithFrame:(NSRect)aFrame
{
    /*
    NSArray *portArray;
    NSConnection *conn;
    NSPort *port1;
    NSPort *port2;
     */

    self = [ [ super initWithFrame:aFrame ] retain ];
    fvolume = 1.0;
    loopMode = NSQTMovieNormalPlayback;
    
    return self;
        
    /*
    port1 = [ NSPort port ];
    port2 = [ NSPort port ];
    conn = [ [ NSConnection alloc ] initWithReceivePort:port1 sendPort:port2 ];
    [ conn setRootObject:self ];
    portArray = [ NSArray arrayWithObjects:port2,port1,nil ];
    [ NSThread detachNewThreadSelector:@selector(initWithPorts:)
                              toTarget:self
                            withObject:portArray ];

    while(!selfProxy)
        [ [ NSRunLoop currentRunLoop ] runUntilDate:[ NSDate dateWithTimeIntervalSinceNow:1.0 ] ];
    
    return selfProxy;
     */
}

/*
-(void)initWithPorts:(NSArray*)portArray
{
    id server;
    NSAutoreleasePool *pool = [ [ NSAutoreleasePool alloc ] init ];
    NSConnection *conn = [ NSConnection connectionWithReceivePort:[ portArray objectAtIndex:0 ]
                                                         sendPort:[ portArray objectAtIndex:1 ] ];
                                                         
    
    server = [ conn rootProxy ];
    [ server setProtocolForProxy:@protocol(TYQTMusicPlayer) ];
    [ server registProxy:self ];

    [ [ NSRunLoop currentRunLoop ] run ];
    NSLog(@"!!caution end runLoop!!");
    [ pool release ];
    [ NSThread exit ];
}
*/

/*
-(void)registProxy:(id)proxy
{
    selfProxy = proxy;
    [ selfProxy retain ];
}
*/

- (void) setMovie:(NSMovie*)aMovie
{
    [ self stop:nil ];
    if(movie){
        [ movie release ];
    }
    movie = [ aMovie retain ];
    qtmovie = (movie) ? [ movie QTMovie ] : NULL;
    
    return;
}

- (void)start:(id)sender
{
    if(movie) {
        if(loopMode==NSQTMovieLoopingPlayback){
            [ self performSelector:@selector(start:)
                        withObject:nil 
                        afterDelay:GetMovieDuration(qtmovie) / GetMovieTimeScale(qtmovie) ];
        }
        GoToBeginningOfMovie(qtmovie);
        StartMovie(qtmovie);
        isPlaying = YES;
    }
    
    return;
}

- (void)stop:(id)sender
{
    if(movie)
        StopMovie(qtmovie);
    
    [ NSObject cancelPreviousPerformRequestsWithTarget:self
                                              selector:@selector(start:)
                                                object:nil ];

    isPlaying = NO;
    
    return;
}

- (BOOL)isPlaying
{
    isPlaying = (movie && isPlaying && !IsMovieDone(qtmovie));
    
    return isPlaying;
}

- (void)gotoBeginning:(id)sender
{
    if(movie)
        GoToBeginningOfMovie(qtmovie);
        
    return;
}

- (void)setVolume:(float)volume
{

    fvolume = volume;
    if(movie)
        SetMovieVolume(qtmovie,fvolume * 100);
        
    return;
}

- (float)volume
{
    return fvolume / 100;
}

- (void)setLoopMode:(NSQTMovieLoopMode)mode
{
    loopMode = mode;
}

- (NSQTMovieLoopMode)loopMode
{
    return loopMode;
}

- (void)showController:(BOOL)show adjustingSize:(BOOL)adjustSize
{
     // do nothing
}
- (void)setEditable:(BOOL)editable
{
    // do nothing
}

-(void)dealloc
{
    [movie release];
    [super dealloc];
}

@end


