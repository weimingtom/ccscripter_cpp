/* TYQTMPController */

#ifndef TYQMTPCONTROLLER_H
#define TYQMTPCONTROLLER_H

#include <QObject>
#include <QWidget>

class TYQTMusicPlayer;
class TYQTMPController : public QObject 
{
    Q_OBJECT

public:
    explicit TYQTMPController(QObject* parent = nullptr);
    ~TYQTMPController();

    void setMovieView(QWidget* movieView);

public slots:
    void onApplicationStartup();

private:
    QWidget* m_movieViewObject;
};

#endif // TYQMTPCONTROLLER_H
