/* TYExtVolumeController */

#ifndef TYEXTVOLUMECONTROLLER_H
#define TYEXTVOLUMECONTROLLER_H

#include "TYBGMVolumeController.h"
#include <QObject>

class TYExtVolumeController : public TYBGMVolumeController
{
    Q_OBJECT

public:
    explicit TYExtVolumeController(QObject* parent = nullptr);
    ~TYExtVolumeController();

    void setDefault(QObject* sender);

    void initWithDefaultBGMVolume(float defBGMVol,
                    float bgmVol,
                    float defVoiceVol,
                    float voiceVol,
                    float defSEVol,
                    float seVolume);

    float voiceVolume();
    float seVolume();

private:
    QSlider* SESlider;
    QSlider* VoiceSlider;

    float defaultBGMVolume;
    float defaultVoiceVolume;
    float defaultSEVolume;
};

#endif // TYEXTVOLUMECONTROLLER_H
