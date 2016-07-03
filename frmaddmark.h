#ifndef FRMADDMARK_H
#define FRMADDMARK_H

#include <QDialog>
#include "gstmanager.h"
#include "markerdb.h"

namespace Ui {
class frmAddMark;
}

class frmAddMark : public QDialog
{
    Q_OBJECT

public:
    explicit frmAddMark(int markId = 0,QWidget *parent = 0);
    ~frmAddMark();
    QString title();
    QString tags();
    void setFileInfo(int fileId, QString filename, QString duration, QString markPos, GstManager *gm, bool isEdit = false);

private slots:
    void on_btnGoBackFrame_clicked();
    void on_btnGoForwardFrame_clicked();
    void on_buttonBox_accepted();

private:
    Ui::frmAddMark *ui;
    GstManager *gstman;
    MarkerDB marker;

    int _markId;
    int _fileId;
    bool _isEdit;

    void searchMark(int markId);
    QString numToDuration(qint64 secs);
};

#endif // FRMADDMARK_H
