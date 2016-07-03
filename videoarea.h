#ifndef VIDEOAREA_H
#define VIDEOAREA_H

#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QTimer>
#include <QtDebug>

class VideoArea : public QLabel
{
    Q_OBJECT
public:
    explicit VideoArea(QWidget *parent = 0);
    void standby();

signals:
    void doubleClicked();
    void fileDropped(QString filename);
    void spacePressed();
    void showHideControlBar(bool);

public slots:
    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent *);
//    void wheelEvent(QWheelEvent*);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent*);
    void timerTimeOut();

private:
    int _offset;
    QTimer _timer;

};

#endif // VIDEOAREA_H
