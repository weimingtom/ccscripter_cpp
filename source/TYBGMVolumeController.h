/* TYBGMVolumeController */

#import <Cocoa/Cocoa.h>

@interface TYBGMVolumeController : NSObject
{
    IBOutlet NSSlider *BGMSlider;

    id delegate;
}
- (IBAction)changeBGMVolume:(id)sender;
- (IBAction)dialogOK:(id)sender;

-(id)initWithBGMVolume:(float)volume;
-(void)setDelegate:(id)aObj;
-(void)showDialog;
-(void)setBGMVolume:(float)volume;
@end
