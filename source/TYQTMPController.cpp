#import "TYQTMPController.h"
#import "TYQTMusicPlayer.h"

@implementation TYQTMPController

- (void) applicationDidFinishLaunching  : (NSNotification *) aNote
{
    NSMovie *aMovie;
    TYQTMusicPlayer *player;

    player = [ [ TYQTMusicPlayer alloc ] initWithFrame:NSZeroRect ];
    aMovie = [ [ NSMovie alloc ] initWithURL:[ NSURL fileURLWithPath:@"/Users/shohei/SimpleCocoaMovie/MovieFile.mov" ] byReference:YES ];
    //[ MovieViewObject setMovie:aMovie ];
    [ player setMovie:aMovie ];
    [ player setLoopMode:NSQTMovieLoopingPlayback ];
    [ player start:nil ];
    
}

@end
