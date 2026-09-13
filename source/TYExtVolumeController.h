/* TYExtVolumeController */

#import <Cocoa/Cocoa.h>
#import "TYBGMVolumeController.h"

@interface TYExtVolumeController : TYBGMVolumeController
{
    IBOutlet NSSlider *SESlider;
    IBOutlet NSSlider *VoiceSlider;

    float defaultBGMVolume;
    float defaultVoiceVolume;
    float defaultSEVolume;
}
- (IBAction)setDefault:(id)sender;

-(id)initWithDefaultBGMVolume:(float)defBGMVol
                    bgmVolume:(float)bgmVol
           defaultVoiceVolume:(float)defVoiceVol
                  voiceVolume:(float)voiceVol
              defaultSEVolume:(float)defSEVol
                     seVolume:(float)seVol;

-(float)voiceVolume;
-(float)seVolume;

@end
