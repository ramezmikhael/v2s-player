#ifndef WSEARCHRESULTITEM_H
#define WSEARCHRESULTITEM_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class WSearchResultItem;
}

class WSearchResultItem : public QWidget
{
    Q_OBJECT

public:
    explicit WSearchResultItem(QWidget *parent = 0);
    ~WSearchResultItem();
    void setTitle(QString);
    void setNotes(QString);
    void setLocation(QString);
    void setMSec(qint64);
    void setMarkId(int);

signals:
    void clicked(QString filename, qint64 msec);
    void edit(int markId);
    void remove(int markId);

private slots:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void on_btnEdit_clicked();
    void on_btnRemove_clicked();

private:
    Ui::WSearchResultItem *ui;
    int _markId;
    int _fileId;
    qint64 _msec;
    QString _location;
};

#endif // WSEARCHRESULTITEM_H
