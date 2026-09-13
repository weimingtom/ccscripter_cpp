#include "TYBGMVolumeController.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>

TYBGMVolumeController::TYBGMVolumeController(QObject* parent)
    : QObject(parent)
    , m_bgmSlider(nullptr)
    , m_dialogWindow(nullptr)
    , m_delegate(nullptr)
{
}

TYBGMVolumeController::~TYBGMVolumeController()
{
    if (m_dialogWindow)
        m_dialogWindow->deleteLater();
}

void TYBGMVolumeController::setDelegate(QObject* delegate)
{
    m_delegate = delegate;
}

void TYBGMVolumeController::changeBGMVolume()
{
    if (m_delegate) {
        float volume = bgmVolume();
        emit bgmVolumeChanged(volume);
        QMetaObject::invokeMethod(m_delegate, "changeBGMVolume", Qt::DirectConnection,
                                   Q_ARG(float, volume));
    }
}

void TYBGMVolumeController::dialogOK()
{
    if (m_dialogWindow) {
        m_dialogWindow->hide();
        QApplication::quit();
    }
}

void TYBGMVolumeController::showDialog()
{
    if (!m_dialogWindow) {
        m_dialogWindow = new QDialog();
        QVBoxLayout* layout = new QVBoxLayout(m_dialogWindow);

        layout->addWidget(new QLabel("BGM Volume"));

        m_bgmSlider = new QSlider(Qt::Horizontal, m_dialogWindow);
        m_bgmSlider->setRange(0, 100);
        m_bgmSlider->setValue(100);
        layout->addWidget(m_bgmSlider);

        QPushButton* okButton = new QPushButton("OK", m_dialogWindow);
        layout->addWidget(okButton);

        m_dialogWindow->setLayout(layout);

        QObject::connect(m_bgmSlider, &QSlider::valueChanged, this, &TYBGMVolumeController::changeBGMVolume);
        QObject::connect(okButton, &QPushButton::clicked, this, &TYBGMVolumeController::dialogOK);
    }

    m_dialogWindow->show();
    m_dialogWindow->exec();
}

void TYBGMVolumeController::setBGMVolume(float volume)
{
    if (m_bgmSlider) {
        m_bgmSlider->setValue(static_cast<int>(volume * 100));
    }
}

float TYBGMVolumeController::bgmVolume() const
{
    if (m_bgmSlider)
        return m_bgmSlider->value() / 100.0f;
    return 1.0f;
}
