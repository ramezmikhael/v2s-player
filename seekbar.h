#ifndef SEEKBAR_H
#define SEEKBAR_H

#include <QSlider>
#include <QMouseEvent>

class SeekBar : public QSlider
{
    Q_OBJECT
public:
    explicit SeekBar(QWidget *parent = 0);

signals:
    void goToValue(int);

public slots:
    void mouseReleaseEvent(QMouseEvent*);

};

#endif // SEEKBAR_H
