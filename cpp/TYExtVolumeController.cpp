#include "TYExtVolumeController.h"

TYExtVolumeController::TYExtVolumeController(QObject* parent)
    : TYBGMVolumeController(parent)
    , SESlider(nullptr)
    , VoiceSlider(nullptr)
    , defaultBGMVolume(0.0f)
    , defaultVoiceVolume(0.0f)
    , defaultSEVolume(0.0f)
{
}

TYExtVolumeController::~TYExtVolumeController()
{
}

void TYExtVolumeController::setDefault(QObject* sender)
{
    Q_UNUSED(sender);
    if (BGMSlider) {  BGMSlider->setValue(static_cast<int>(defaultBGMVolume)); }
    if (VoiceSlider) { VoiceSlider->setValue(static_cast<int>(defaultVoiceVolume)); }
    if (SESlider) { SESlider->setValue(static_cast<int>(defaultSEVolume)); }
}

void TYExtVolumeController::initWithDefaultBGMVolume(float defBGMVol,
                                                      float bgmVol,
                                                      float defVoiceVol,
                                                      float voiceVol,
                                                      float defSEVol,
                                                      float seVolume)
{
    defaultBGMVolume=defBGMVol;
    defaultVoiceVolume=defVoiceVol;
    defaultSEVolume=defSEVol;


    
    if (BGMSlider) { BGMSlider->setValue(static_cast<int>(bgmVol)); }
    if (VoiceSlider) { VoiceSlider->setValue(static_cast<int>(voiceVol)); }
    if (SESlider) { SESlider->setValue(static_cast<int>(seVolume)); }
}

float TYExtVolumeController::voiceVolume()
{
    return VoiceSlider ? static_cast<float>(VoiceSlider->value()) : 0.0f;
}

float TYExtVolumeController::seVolume()
{
    return SESlider ? static_cast<float>(SESlider->value()) : 0.0f;
}
