#import "TYExtVolumeController.h"

@implementation TYExtVolumeController

- (IBAction)setDefault:(id)sender
{
    [ BGMSlider setFloatValue:defaultBGMVolume ];
    [ VoiceSlider setFloatValue:defaultVoiceVolume ];
    [ SESlider setFloatValue:defaultSEVolume ];    
}

-(id)initWithDefaultBGMVolume:(float)defBGMVol
                    bgmVolume:(float)bgmVol
           defaultVoiceVolume:(float)defVoiceVol
                  voiceVolume:(float)voiceVol
              defaultSEVolume:(float)defSEVol
                     seVolume:(float)seVol
{
    self = [ super init ];
    
    defaultBGMVolume=defBGMVol;
    defaultVoiceVolume=defVoiceVol;
    defaultSEVolume=defSEVol;

    [ NSBundle loadNibNamed:@"ExtVolume" owner:self ];
    
    [ BGMSlider setFloatValue:bgmVol ];
    [ VoiceSlider setFloatValue:voiceVol ];
    [ SESlider setFloatValue:seVol ];

    return self;
}

-(float)voiceVolume
{
    return [ VoiceSlider floatValue ];
}

-(float)seVolume
{
    return [ SESlider floatValue ];
}

@end
