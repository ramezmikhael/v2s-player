#include "playlist.h"

PlayList::PlayList(QWidget *parent) :
    QTreeWidget(parent)
{
    setAcceptDrops(true);
}

void PlayList::dragEnterEvent(QDragEnterEvent *ev)
{
    if(ev->mimeData()->hasFormat("text/uri-list"))
        ev->acceptProposedAction();
}

void PlayList::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
}

void PlayList::dropEvent(QDropEvent *ev)
{
    emit fileDropped(ev->mimeData()->urls());
}
