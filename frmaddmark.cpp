#include "frmaddmark.h"
#include "ui_frmaddmark.h"

frmAddMark::frmAddMark(int markId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmAddMark)
{
    ui->setupUi(this);

    ui->btnGoForwardFrame->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->btnGoBackFrame->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));

    _markId = markId;
    if(markId > 0)
        searchMark(markId);
}

frmAddMark::~frmAddMark()
{
    delete ui;
}

QString frmAddMark::title()
{
    return ui->txtTitle->text();
}

QString frmAddMark::tags()
{
    return ui->txtTags->toPlainText();
}

void frmAddMark::setFileInfo(int fileId, QString filename, QString duration, QString markPos, GstManager *gm, bool isEdit)
{
    _fileId = fileId;
    _isEdit = isEdit;
    gstman = gm;
    ui->lblFilename->setText(filename);
    ui->lblDuration->setText(QString("<b>").append(ui->lblDuration->text()).append("</b>").append(" ").append(duration));
    ui->lblMarkPos->setText(QString("<b>").append(ui->lblMarkPos->text()).append("</b>").append(" ").append(markPos));
}

void frmAddMark::on_btnGoBackFrame_clicked()
{
    gstman->goBackFrame();
}

void frmAddMark::on_btnGoForwardFrame_clicked()
{
    gstman->goForwardFrame();
}

void frmAddMark::searchMark(int markId)
{
    qDebug() << "mark: " << markId;
    MarkerDB marker;
    QSqlQuery *qry = marker.searchMarkById(markId);
    if(qry->next())
    {
        ui->lblFilename->setText(qry->value("Filename").toString());
        QString duration = numToDuration(qry->value("Duration").toDouble() / 1000);
        QString markPos = numToDuration(qry->value("Sec").toDouble());
        ui->txtTitle->setText(qry->value("Caption").toString());
        ui->txtTags->setText(qry->value("Comment").toString());

        ui->lblDuration->setText(QString("<b>").append(ui->lblDuration->text()).append("</b>").append(" ").append(duration));
        ui->lblMarkPos->setText(QString("<b>").append(ui->lblMarkPos->text()).append("</b>").append(" ").append(markPos));
    }
}

QString frmAddMark::numToDuration(qint64 secs)
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

void frmAddMark::on_buttonBox_accepted()
{
    if(!_isEdit)
        marker.addMark(_fileId, (qint64)gstman->queryPosition(), title() , tags());
    else
        marker.editMark(_markId, title() , tags());
}
