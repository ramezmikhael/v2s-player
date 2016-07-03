#include "seekbar.h"
#include <QStyle>
#include <QStyleOptionSlider>

SeekBar::SeekBar(QWidget *parent) :
    QSlider(parent)
{

}

void SeekBar::mouseReleaseEvent(QMouseEvent *ev)
{
    QStyleOptionSlider opt;
    this->initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    sliderLength = sr.width() / 2;
    sliderMin = gr.x();
    sliderMax = gr.right() - sliderLength + 1;

    int point = QStyle::sliderValueFromPosition(minimum(), maximum(), ev->x() - sliderMin, sliderMax - sliderMin);

    emit goToValue(point);
}
