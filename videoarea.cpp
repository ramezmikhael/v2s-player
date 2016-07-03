#include "videoarea.h"

VideoArea::VideoArea(QWidget *parent) :
    QLabel(parent)
{
    setAcceptDrops(true);
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    standby();

    _timer.setInterval(3000);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));
    _timer.start();
}

void VideoArea::standby()
{
    setPixmap(QPixmap(":/pics/pics/logo01.png"));
}

void VideoArea::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Space)
        emit spacePressed();
}

void VideoArea::mousePressEvent(QMouseEvent *)
{
    setFocus();
}

void VideoArea::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked();
}

void VideoArea::mouseMoveEvent(QMouseEvent *ev)
{
    setCursor(Qt::ArrowCursor);
    if(!_timer.isActive())
        _timer.start();
    if(ev->pos().y() > this->height() - 80)
        emit showHideControlBar(true);
    else
        emit showHideControlBar(false);
}

void VideoArea::dragEnterEvent(QDragEnterEvent *ev)
{
    if(ev->mimeData()->hasFormat("text/uri-list"))
        ev->acceptProposedAction();
}

void VideoArea::dropEvent(QDropEvent *ev)
{
    emit fileDropped(ev->mimeData()->urls().at(0).toString());
}

void VideoArea::timerTimeOut()
{
    setCursor(Qt::BlankCursor);
    _timer.stop();
}
