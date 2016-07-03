#ifndef FRMMAIN_H
#define FRMMAIN_H

#define SETT_VOL            "vol"
#define SETT_DIR            "od"
#define SETT_IS_MAXIMIZED   "ws"
#define SETT_SHOW_PLAYLIST  "plst"
#define SETT_SHOW_SEARCH    "srch"

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QtWinExtras>
#include "gstmanager.h"
#include "markerdb.h"
#include "frmaddmark.h"
#include "wsearchresultitem.h"
#include "filehash.h"

namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    void createThumbBar();

public slots:
    void playFile(QString filename, bool computeMD5 = true);

private slots:
    void fileDropped(QString);
    void addToPlaylist(QString filename);
    void addToPlaylist(QList<QUrl> filenames);
    void addToPlaylistTree(QString filename);
    void timerTimeout();
    void videoDoubleClicked();
    void showHideControlBar(bool);
    void goToPoint(int);
    void hashResultReady(QString);
    void resultItemClicked(QString filename, qint64 msec);
    void editMark(int);
    void removeMark(int);
    void on_actionOpen_triggered();
    void on_actionPlay_triggered();
    void on_actionSearch_triggered(bool checked);
    void on_actionPlaylist_triggered(bool checked);
    void on_actionExit_triggered();
    void on_actionPrevious_triggered();
    void on_actionNext_triggered();
    void on_actionStop_triggered();
    void on_actionVol_Up_triggered();
    void on_actionVol_Down_triggered();
    void on_btnPlay_clicked();
    void on_hsVolume_valueChanged(int value);
    void on_btnMark_clicked();
    void on_btnStop_clicked();
    void on_btnSearch_clicked();
    void on_btnNext_clicked();
    void on_btnPrevious_clicked();
    void on_btnVolume_clicked(bool checked);
    void on_txtSearch_textChanged(const QString &arg1);
    void on_twPlaylist_itemDoubleClicked(QTreeWidgetItem *item, int);
    void on_actionSnapshot_triggered();

private:
    Ui::frmMain *ui;
    QTimer *_timer;
    QSettings _settings;

    QWinThumbnailToolBar *thumbbar;
    QWinThumbnailToolButton *tbPrevious;
    QWinThumbnailToolButton *tbPlayPause;
    QWinThumbnailToolButton *tbNext;

    bool _isPlaying;
    bool _updateSeekbar;
    bool _isFullScreen;
    int _fileDbId;
    int _markDbId;
    bool _markIsEdit;
    QString _filehash;
    qint16 _playingFileIndex;
    qint64 _duration;
    qint64 _pos;
    QString _durationStr;
    QString _posStr;
    QString _filepath;
    QString _filename;
    QStringList _playList;

    GstManager gstman;
    MarkerDB marker;

    void setupIcons();
    void setupVars();
    void setupTimer();
    void setupSettings();
    void resetPlaybackParams();
    void changeState();
    void openFiles();
    void openMark(QString filename, qint64 seekTo);
    void computeFileHash(QString filename);
    QString numToDuration(qint64);
};

#endif // FRMMAIN_H
