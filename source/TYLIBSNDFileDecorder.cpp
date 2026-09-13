//
//  TYLIBSNDFileDecorder.m
//  Tukuyomi
//
//  Created by toveta on Sat Sep 28 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#import "TYLIBSNDFileDecorder.h"
#import <sndfile.h>

@implementation TYLIBSNDFileDecorder

-(id)initWithData:(NSData*)sourceData
{
    [ super init ];
    
    tempPath = [ [ [ NSBundle bundleForClass:[ self class ] ] resourcePath ] stringByAppendingPathComponent:@".TEMP" ];
    [ tempPath retain ];
    [ sourceData writeToFile:tempPath atomically:NO ];
    
    return self;
}

-(BOOL)getSamples:(short**)samples_ptr
           frames:(int*)frames
         channels:(int*)channels
             rate:(int*)rate
{
    SNDFILE *infile;
    SF_INFO in_info;

    /*
    if((in_info = malloc(sizeof(SF_INFO))) == NULL){
        NSLog(@"can't allocate SF_INFO");
        return NO;
    }
    */
    
    if((infile = sf_open([ tempPath fileSystemRepresentation ],
                 SFM_READ,
                 &in_info)) == NULL){
        sf_perror(infile);
        return NO;
    }
    
    if((samples = malloc(sizeof(short) * in_info.frames * in_info.channels)) == NULL){
        sf_perror(infile);
        return NO;
    }
    
    if(sf_readf_short(infile,samples,in_info.frames) != in_info.frames){
        sf_perror(infile);
        return NO;
    }

    *samples_ptr = samples;
    *frames = in_info.frames;
    *channels = in_info.channels;
    *rate = in_info.samplerate;
    
    sf_close(infile);
    
    return YES;
}

- (void)dealloc
{
    [ [ NSFileManager defaultManager ] removeFileAtPath:tempPath handler:nil ];
    [ tempPath release ];
    free(samples);
    [super dealloc];
}

@end
