#ifndef GSTMANAGER_H
#define GSTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QtDebug>
#include <QUrl>
//#include <gst/gst.h>
//#include <gst/video/videooverlay.h>
#include <vlc/vlc.h>

#if defined(Q_OS_WIN)|| defined(Q_OS_WIN32)
#include <windows.h>
#endif

class GstManager : public QObject
{
    Q_OBJECT

    typedef enum {
        GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
        GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
        GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
    } GstPlayFlags;

public:
    explicit GstManager(QObject *parent = 0);
    ~GstManager();
    int playFile(QString filename, QWidget* playArea);
    void playPause();
    void stop();
    void seek(qint64 pos);
    void setVolume(quint16);
    void setSubtitle(QString filename);
    void setSubtitleState(bool enable);
    void goBackFrame();
    void goForwardFrame();
    void snapshot();
    qint64 queryDuration();
    qint64 queryPosition();
    libvlc_state_t queryState();
    void checkMessages();

private:
//    GstElement *playbin2;
//    GstBus *_bus;

    libvlc_instance_t * inst;
    libvlc_media_player_t *mp;
    libvlc_media_t *m;

    void allowSleep(bool enable);
};

#endif // GSTMANAGER_H
