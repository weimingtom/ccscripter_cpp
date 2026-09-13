/* TYBGMVolumeController */

#ifndef TYBGMVOLUMECONTROLLER_H
#define TYBGMVOLUMECONTROLLER_H

#include <QObject>
#include <QWidget>
#include <QSlider>

class TYBGMVolumeController : public QObject 
{
    Q_OBJECT

public:
    explicit TYBGMVolumeController(QObject* parent = nullptr);
    ~TYBGMVolumeController();

    void setDelegate(QObject* delegate);
    QObject* delegate() const { return m_delegate; }

    void showDialog();
    void setBGMVolume(float volume);
    float bgmVolume() const;

    QSlider* bgmSlider() { return m_bgmSlider; }

public slots:
    void changeBGMVolume();
    void dialogOK();

signals:
    void bgmVolumeChanged(float volume);

protected:
    QSlider* m_bgmSlider;
    QWidget* m_dialogWindow;

private:
    QObject* m_delegate;
};

#endif // TYBGMVOLUMECONTROLLER_H
