#import "TYBGMVolumeController.h"

@implementation TYBGMVolumeController

- (IBAction)changeBGMVolume:(id)sender
{
    [ delegate changeBGMVolume:sender ];
}

- (IBAction)dialogOK:(id)sender
{
    [ [ sender window ] orderOut:self ];
    [[NSApplication sharedApplication] stopModalWithCode:NSOKButton ];
}

-(id)initWithBGMVolume:(float)volume
{
    [ super init ];
    [ NSBundle loadNibNamed:@"BGMVolume" owner:self ];
    [ BGMSlider setFloatValue:volume ];
    return self;
}

-(void)setDelegate:(id)aObj
{
    delegate = aObj;
}

-(void)showDialog
{
    [[NSApplication sharedApplication] runModalForWindow:[ BGMSlider window ] ];
}

-(void)setBGMVolume:(float)volume
{
    [ BGMSlider setFloatValue:volume ];
}

@end
