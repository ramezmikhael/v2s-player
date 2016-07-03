#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QTreeWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class PlayList : public QTreeWidget
{
    Q_OBJECT
public:
    explicit PlayList(QWidget *parent = 0);
    
signals:
    void fileDropped(QList<QUrl> filenames);
    
public slots:
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dropEvent(QDropEvent *ev);
    
};

#endif // PLAYLIST_H
