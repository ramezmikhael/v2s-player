#include "wsearchresultitem.h"
#include "ui_wsearchresultitem.h"

WSearchResultItem::WSearchResultItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WSearchResultItem)
{
    ui->setupUi(this);

    setMouseTracking(true);
    ui->btnEdit->setVisible(false);
    ui->btnRemove->setVisible(false);
}

WSearchResultItem::~WSearchResultItem()
{
    delete ui;
}

void WSearchResultItem::setTitle(QString title)
{
    ui->lblTitle->setText(title);
}

void WSearchResultItem::setNotes(QString notes)
{
    ui->lblNotes->setText(notes);
}

void WSearchResultItem::setLocation(QString location)
{
    _location = location;
}

void WSearchResultItem::setMSec(qint64 msec)
{
    _msec = msec;
}

void WSearchResultItem::setMarkId(int markId)
{
    _markId = markId;
}

void WSearchResultItem::mousePressEvent(QMouseEvent *)
{
    emit clicked(_location, _msec);
}

void WSearchResultItem::enterEvent(QEvent *)
{
    ui->btnEdit->setVisible(true);
    ui->btnRemove->setVisible(true);
}

void WSearchResultItem::leaveEvent(QEvent *)
{
    ui->btnEdit->setVisible(false);
    ui->btnRemove->setVisible(false);
}


void WSearchResultItem::on_btnEdit_clicked()
{
    emit(edit(_markId));
}

void WSearchResultItem::on_btnRemove_clicked()
{
    emit(remove(_markId));
}
