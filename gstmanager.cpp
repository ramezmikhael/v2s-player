#include "gstmanager.h"

GstManager::GstManager(QObject *parent) :
    QObject(parent)
{
    /* Load the VLC engine */
    inst = libvlc_new (0, NULL);

    /* Create a new item */
    m = libvlc_media_new_location (inst, "");

    /* Create a media player playing environement */
    mp = libvlc_media_player_new_from_media (m);

    /* No need to keep the media now */
    libvlc_media_release (m);
}

GstManager::~GstManager()
{
    /* Free the media_player */
    libvlc_media_player_release (mp);
    libvlc_release (inst);
}

int GstManager::playFile(QString filename, QWidget *playArea)
{

    // Use the next line to replace the windows \ directory separator with /
    filename = QUrl::fromLocalFile(filename).toString();

    /* Create a new item */
    m = libvlc_media_new_location (inst, filename.toLocal8Bit());

    /* Create a media player playing environement */
    mp = libvlc_media_player_new_from_media (m);

    /* No need to keep the media now */
    libvlc_media_release (m);

    /* Force libvlc to stop handling keyboard events  */
    libvlc_video_set_key_input(mp, false);

    /* Force libvlc to stop handling mouse events  */
    libvlc_video_set_mouse_input(mp, false);

    /* if we have a window around */
    //libvlc_media_player_set_xwindow (mp, xid);
    /* or on windows */
    libvlc_media_player_set_hwnd (mp, (HWND)playArea->winId());
    /* or on mac os */
    //libvlc_media_player_set_nsobject (mp, view);

    /* play the media_player */
    libvlc_media_player_play (mp);

    //Stop screen saver and power off
    allowSleep(false);

    return 0;
}

void GstManager::playPause()
{
    libvlc_state_t state = libvlc_media_player_get_state(mp);

    if(state == libvlc_Playing)
    {
        libvlc_media_player_set_pause(mp, 1);
    }
    else
    {
        libvlc_media_player_set_pause(mp, 0);
        allowSleep(false);
    }
}

void GstManager::stop()
{
    libvlc_state_t state = libvlc_media_player_get_state(mp);

    if(state == libvlc_Playing || state == libvlc_Paused)
    {
        //seek(0);
        libvlc_media_player_stop(mp);
        allowSleep(true);
    }
}

libvlc_state_t GstManager::queryState()
{
    return libvlc_media_player_get_state(mp);;
}

void GstManager::checkMessages()
{
    // TODO
    return;
}

void GstManager::allowSleep(bool enable)
{
#if defined(Q_OS_WIN)|| defined(Q_OS_WIN32)
    if(enable)
    {   //Restore everything to normal
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, 0, 0);
        SetThreadExecutionState(ES_CONTINUOUS);
    }
    else
    {   //Disable screen saver and power idle
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, 0, 0);
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    }
#endif
}

void GstManager::seek(qint64 pos)
{
    libvlc_media_player_set_time(mp, pos);
}

void GstManager::setVolume(quint16 val)
{
    libvlc_audio_set_volume(mp, val);
}

void GstManager::setSubtitle(QString filename)
{
    if(!filename.contains("file:///"))
        filename = "file:///" + filename;

    qDebug() << filename.toLocal8Bit().data();
    libvlc_video_set_subtitle_file(mp, filename.toUtf8());
}

void GstManager::setSubtitleState(bool enable)
{
    //TODO
}

void GstManager::goBackFrame()
{
    //Assuming that the video played is 24 fps subtract 40 ms position
    seek(queryPosition() - 80);
}

void GstManager::goForwardFrame()
{
    //Assuming that the video played is 24 fps add 40 ms position
    seek(queryPosition() + 80);

}

void GstManager::snapshot()
{
    libvlc_video_take_snapshot(mp, 0, "c:\\snap.png", 0, 0);
}

qint64 GstManager::queryDuration()
{
    return libvlc_media_player_get_length(mp);
}

qint64 GstManager::queryPosition()
{
    return libvlc_media_player_get_time(mp);
}
