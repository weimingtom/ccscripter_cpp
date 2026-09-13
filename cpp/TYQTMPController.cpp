#include "TYQMPController.h"
#include "TYQTMusicPlayer.h"
#include <QDebug>

TYQTMPController::TYQTMPController(QObject* parent)
    : QObject(parent)
    , m_movieViewObject(nullptr)
{
}

TYQTMPController::~TYQTMPController()
{
}

void TYQTMPController::setMovieView(QWidget* movieView)
{
    m_movieViewObject = movieView;
}

void TYQTMPController::onApplicationStartup()
{
    TYQTMusicPlayer* player = new TYQTMusicPlayer(QRect(0, 0, 0, 0));

    QString moviePath = "/Users/shohei/SimpleCocoaMovie/MovieFile.mov";
    player->setMovie(moviePath);
    player->setLoopMode(TYQTMusicPlayer::LoopingPlayback);
    player->start();
}
