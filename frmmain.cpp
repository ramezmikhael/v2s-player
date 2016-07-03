#include "frmmain.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    tabifyDockWidget(ui->dockPlaylist, ui->dockSearch);

    ui->twPlaylist->hideColumn(1);

    setupSettings();
    setupIcons();
    setupVars();    // Global Variable initial values
    setupTimer();   // Timer handling

    connect(ui->videoArea, SIGNAL(doubleClicked()), this, SLOT(videoDoubleClicked()));
    connect(ui->videoArea, SIGNAL(showHideControlBar(bool)), this, SLOT(showHideControlBar(bool)));
    connect(ui->videoArea, SIGNAL(fileDropped(QString)), this, SLOT(fileDropped(QString)));
    connect(ui->videoArea, SIGNAL(spacePressed()), this, SLOT(on_btnPlay_clicked()));
    connect(ui->hsSeek, SIGNAL(goToValue(int)), this, SLOT(goToPoint(int)));
    connect(ui->twPlaylist, SIGNAL(fileDropped(QList<QUrl>)), this, SLOT(addToPlaylist(QList<QUrl>)));


    resetPlaybackParams();

    // Restore Settings
    ui->hsVolume->setValue(_settings.value(SETT_VOL, 50).toInt());

    // If we have a file to play we run it immediatly
    if(qApp->arguments().count() > 1)
        playFile(qApp->arguments().at(1));
}

frmMain::~frmMain()
{
    //Save window settings before closing
    _settings.setValue(SETT_IS_MAXIMIZED, isMaximized());
    _settings.setValue(SETT_SHOW_PLAYLIST, ui->actionPlaylist->isChecked());
    _settings.setValue(SETT_SHOW_SEARCH, ui->actionSearch->isChecked());

    delete ui;
}

void frmMain::setupIcons()
{
    ui->btnVolume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->btnPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->btnPrevious->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->btnStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
}

void frmMain::setupVars()
{
    _pos = 0;
    _fileDbId = -1;
    _playingFileIndex = -1;
    _isFullScreen = false;
    _markIsEdit = false;
}

void frmMain::setupTimer()
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

void frmMain::setupSettings()
{
    if(_settings.value(SETT_IS_MAXIMIZED).toBool())
        setWindowState(Qt::WindowMaximized);
    if(!_settings.value(SETT_SHOW_PLAYLIST, true).toBool())
        ui->actionPlaylist->trigger();
    if(!_settings.value(SETT_SHOW_SEARCH, true).toBool())
        ui->actionSearch->trigger();
}

void frmMain::createThumbBar()
{
    thumbbar = new QWinThumbnailToolBar(this);
    thumbbar->setWindow(windowHandle());

    tbPrevious = new QWinThumbnailToolButton;
    tbPrevious->setToolTip("Previous");
    tbPrevious->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    tbPlayPause = new QWinThumbnailToolButton;
    tbPlayPause->setToolTip("Play / Pause");
    tbPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    tbNext = new QWinThumbnailToolButton;
    tbNext->setToolTip("Next");
    tbNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    connect(tbPrevious, SIGNAL(clicked()), this, SLOT(on_btnPrevious_clicked()));
    connect(tbPlayPause, SIGNAL(clicked()), this, SLOT(on_btnPlay_clicked()));
    connect(tbNext, SIGNAL(clicked()), this, SLOT(on_btnNext_clicked()));

    thumbbar->addButton(tbPrevious);
    thumbbar->addButton(tbPlayPause);
    thumbbar->addButton(tbNext);
}

void frmMain::fileDropped(QString filename)
{
    playFile(filename);
}

void frmMain::openMark(QString filename, qint64 seekTo)
{
    playFile(filename, true);
    gstman.seek(seekTo);
}

void frmMain::addToPlaylist(QString filename)
{
    QString fname = filename; //QUrl::fromLocalFile(filename).toString();

    if(_playList.contains(fname))
        return;

    _playList.append(fname);
    addToPlaylistTree(fname);
}

void frmMain::addToPlaylist(QList<QUrl> filenames)
{
    for(int i = 0; i < filenames.count(); ++i)
        addToPlaylist(filenames.at(i).toString().remove("file:///"));
}

void frmMain::addToPlaylistTree(QString filename)
{   
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, QFileInfo(filename).fileName());
    itm->setText(1, filename);
    ui->twPlaylist->insertTopLevelItem(ui->twPlaylist->topLevelItemCount(), itm);
}

void frmMain::computeFileHash(QString filename)
{
    //Compute file hash
    ui->dockControlBar->setWindowTitle(tr("Analyzing file... be kind and give me few seconds :)"));
    FileHash *fileHash = new FileHash;
    fileHash->setFilename(filename);
    connect(fileHash, &FileHash::finished, fileHash, &QObject::deleteLater);
    connect(fileHash, SIGNAL(resultReady(QString)), this, SLOT(hashResultReady(QString)));
    fileHash->start();
}

void frmMain::hashResultReady(QString res)
{
    ui->dockControlBar->setWindowTitle(tr("Now playing: ") + _filename);

    _filehash = res;
    _fileDbId = marker.queryFileDbId(res, _playList.at(_playingFileIndex), _duration);
}

void frmMain::playFile(QString filename, bool computeMD5)
{
    QFile f(filename.remove("file:///"));
    if(!f.exists())
        return;

    if(_duration > 0)
    {
        gstman.stop();
        resetPlaybackParams();
    }

    if(gstman.playFile(QString::fromLocal8Bit(filename.toUtf8()), ui->videoArea) != 0)
    {
        QMessageBox::critical(this, tr("ERROR"), tr("Cannot open media file"));
        return;
    }

    gstman.setVolume(ui->hsVolume->value());

    addToPlaylist(filename);
    _playingFileIndex = _playList.indexOf(filename);

    ui->videoArea->setFocus();
    changeState();

    _filepath = QFileInfo(_playList.at(_playingFileIndex)).absoluteDir().absolutePath();
    _filename = QFileInfo(_playList.at(_playingFileIndex)).fileName();

    // If everything is okay this far, I look for a subtitle file
    // and if I find one I make it run
    QString subtitle = _filepath + "/" + QFileInfo(_playList.at(_playingFileIndex)).completeBaseName().append(".srt");
    if(QFile(subtitle).exists())
        gstman.setSubtitle(subtitle);
    gstman.setSubtitleState(false);

    if(computeMD5)
        computeFileHash(filename);

    // Highlight playing file
    QTreeWidgetItemIterator it(ui->twPlaylist);
    while(*it){
        if((*it)->text(1) == _playList.at(_playingFileIndex))
            (*it)->setSelected(true);
        else
            (*it)->setSelected(false);
        ++it;
    }
}

void frmMain::timerTimeout()
{
    if(_duration <= 0)
    {
        _duration = gstman.queryDuration();
        _durationStr = numToDuration(_duration / 1000);

        ui->hsSeek->setMaximum(_duration > 0 ? _duration : 1);
    }

    _pos = gstman.queryPosition();

    _posStr = numToDuration(_pos / 1000);
    ui->lblTime->setText(_posStr + " / " + _durationStr);

    if(!ui->hsSeek->isSliderDown())
        ui->hsSeek->setValue(_pos);

    //Check Messages for eos message
    if(gstman.queryState() == libvlc_Ended)
    {
        on_btnStop_clicked();
        on_btnNext_clicked();
    }
}

void frmMain::videoDoubleClicked()
{
    if(_isFullScreen)
    {

        this->setWindowState(_settings.value(SETT_IS_MAXIMIZED).toBool() == true ? Qt::WindowMaximized : Qt::WindowNoState);
    }
    else
    {
        _settings.setValue(SETT_IS_MAXIMIZED, isMaximized());
        this->setWindowState(Qt::WindowFullScreen);
    }

    _isFullScreen = !_isFullScreen;
    //    ui->dockSearch->setFloating(_isFullScreen);
    ui->dockSearch->setVisible(!_isFullScreen && ui->actionSearch->isChecked());
    ui->dockPlaylist->setVisible(!_isFullScreen && ui->actionPlaylist->isChecked());
    ui->menuBar->setVisible(!_isFullScreen);
    ui->dockControlBar->setVisible(!_isFullScreen);
}

void frmMain::showHideControlBar(bool show)
{
    if(!_isFullScreen)
        return;
    ui->dockControlBar->setVisible(show);
//    ui->dockControlBar->setWindowFlags(Qt::FramelessWindowHint);
}

void frmMain::goToPoint(int p)
{
    gstman.seek(p);
    ui->hsSeek->setSliderDown(false);
}

void frmMain::openFiles()
{
    QFileDialog dlg;
    QStringList lstFiles = dlg.getOpenFileNames(this, "Open Files...", _settings.value(SETT_DIR, QString()).toString());
    //TODO: Add supported file formats filter

    if(lstFiles.count() < 1)
        return;

    _playList.clear();
    ui->twPlaylist->clear();
    for(int i = 0; i < lstFiles.count(); ++i)
        _playList.append(QUrl::fromLocalFile(lstFiles.at(i)).toString().remove("file:///"));

    // TODO: handle playlist
    _playingFileIndex = 0;
    playFile(_playList.at(_playingFileIndex));

    //Save directory path
    _settings.setValue(SETT_DIR, QFileInfo(_playList.at(0)).absoluteDir().path());
    for(int i = 0; i < _playList.count(); ++i)
        addToPlaylistTree(_playList.at(i));
}

QString frmMain::numToDuration(qint64 secs)
{
    int s = secs % 60;
    secs /= 60;
    int m = secs % 60;
    secs /= 60;
    int h = secs % 60;

    QString min, sec;
    min.sprintf("%02d:", m);
    sec.sprintf("%02d", s);

    QString dFormated = QString::number(h) + ":" + min + sec;
    return dFormated;
}

void frmMain::resetPlaybackParams()
{
    _duration = -1;
    _isPlaying = false;
    _posStr = _durationStr = numToDuration(0);
    ui->lblTime->setText(_posStr + "/" + _durationStr);

    if(_timer->isActive())
        _timer->stop();
}

void frmMain::changeState()
{
    _isPlaying = !_isPlaying;
    _isPlaying ? _timer->start(100) : _timer->stop();
    ui->btnPlay->setIcon(_isPlaying ? style()->standardIcon(QStyle::SP_MediaPause) : style()->standardIcon(QStyle::SP_MediaPlay));
}

void frmMain::on_actionOpen_triggered()
{
    openFiles();
}

void frmMain::on_actionPlay_triggered()
{
    on_btnPlay_clicked();
}

void frmMain::on_actionSearch_triggered(bool checked)
{
    ui->dockSearch->setVisible(checked);
    ui->btnSearch->setChecked(checked);
}

void frmMain::on_actionPlaylist_triggered(bool checked)
{
    ui->dockPlaylist->setVisible(checked);
}

void frmMain::on_actionExit_triggered()
{
    qApp->exit();
}

void frmMain::on_actionPrevious_triggered()
{
    on_btnPrevious_clicked();
}

void frmMain::on_actionNext_triggered()
{
    on_btnNext_clicked();
}

void frmMain::on_actionStop_triggered()
{
    on_btnStop_clicked();
}

void frmMain::on_actionVol_Up_triggered()
{
    ui->hsVolume->setValue(ui->hsVolume->value() + 5);
}

void frmMain::on_actionVol_Down_triggered()
{
    ui->hsVolume->setValue(ui->hsVolume->value() - 5);
}

void frmMain::on_btnPlay_clicked()
{
    if(_playingFileIndex < 0)
    {
        openFiles();
        return;
    }
    gstman.playPause();
    changeState();
}

void frmMain::on_hsVolume_valueChanged(int value)
{
    gstman.setVolume(value);

    //Save volume
    _settings.setValue(SETT_VOL, value);
}

void frmMain::on_btnMark_clicked()
{
    if(_playingFileIndex == -1)
        return;

    bool wasPlaying = _isPlaying;

    if(_isPlaying)
        on_btnPlay_clicked();

    //Open take mark window
    frmAddMark am;
    am.setFileInfo(_fileDbId, _filename, _durationStr, _posStr, &gstman);
    am.exec();

    if(wasPlaying)
        on_btnPlay_clicked();
}

void frmMain::on_btnStop_clicked()
{
    gstman.stop();
    resetPlaybackParams();
    ui->hsSeek->setValue(0);
    ui->videoArea->standby();
    _isPlaying = true;
    changeState();
    ui->dockControlBar->setWindowTitle(tr("Ready!"));

}

void frmMain::on_btnSearch_clicked()
{
    ui->actionSearch->trigger();
}

void frmMain::on_btnNext_clicked()
{
    if(_playingFileIndex < _playList.count() - 1)
    {
        on_btnStop_clicked();
        playFile(_playList.at(++_playingFileIndex));
    }
}

void frmMain::on_btnPrevious_clicked()
{
    if(_playingFileIndex > 0)
    {
        on_btnStop_clicked();
        playFile(_playList.at(--_playingFileIndex));
    }
}

void frmMain::on_btnVolume_clicked(bool checked)
{
    ui->btnVolume->setIcon(style()->standardIcon(checked ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
    ui->hsVolume->setValue(checked ? 0 : 50);
}

void frmMain::on_txtSearch_textChanged(const QString &text)
{
    QList<WSearchResultItem*> items = this->findChildren<WSearchResultItem*>();
    foreach(WSearchResultItem *l, items)
    {
        ui->vLayoutItems->removeWidget(l);
        delete l;
    }

    if(text.isEmpty())
        return;

    QSqlQuery* qry = marker.search(text);
    while(qry->next())
    {
        WSearchResultItem *item = new WSearchResultItem(this);
        item->setMarkId(qry->value("MarkID").toInt());
        item->setTitle(qry->value("Caption").toString());
        item->setNotes(qry->value("Comment").toString());
        item->setLocation(qry->value("Filename").toString());
        item->setMSec(qry->value("Millisecond").toDouble());
        item->setVisible(true);
        item->setAttribute(Qt::WA_DeleteOnClose);
        connect(item, SIGNAL(clicked(QString,qint64)), this, SLOT(resultItemClicked(QString,qint64)));
        connect(item, SIGNAL(edit(int)), this, SLOT(editMark(int)));
        connect(item, SIGNAL(remove(int)), this, SLOT(removeMark(int)));
        ui->vLayoutItems->insertWidget(ui->vLayoutItems->count() - 1, item);
    }
}

void frmMain::resultItemClicked(QString filename, qint64 msec)
{
    openMark(filename, msec);
}

void frmMain::editMark(int markId)
{
    _markIsEdit = true;
    frmAddMark am(markId);
    am.exec();
}

void frmMain::removeMark(int markId)
{
    if(QMessageBox::question(this, tr("V2S Player"), tr("Are you sure you want to delete this mark?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        return;
    marker.removeMark(markId);
}

void frmMain::on_twPlaylist_itemDoubleClicked(QTreeWidgetItem *item, int)
{
    playFile(item->data(1, 0).toString());
}

void frmMain::on_actionSnapshot_triggered()
{
    gstman.snapshot();
}
