#ifndef MARKER_H
#define MARKER_H

#include <QObject>
#include <QtDebug>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>

class MarkerDB : public QObject
{
    Q_OBJECT
public:
    explicit MarkerDB(QObject *parent = 0);
    int addFile(QString filename, qint64 duration, QString MD5Hash);
    int addMark(int fileDbId, qint64 msec, QString caption = QString(), QString notes = QString());
    bool editMark(int markId, QString caption = QString(), QString comment = QString());
    bool removeMark(int markId);
    int queryFileDbId(QString MD5Hash, QString filename, qint64 duration);
    void updateFilename(int fileID, QString filename);
    QSqlQuery *search(QString criteria);    
    QSqlQuery *searchMarkById(int id);

signals:

public slots:

private:
    QString dbFilename;
    QSqlDatabase db;

    void createDatabase();
};

#endif // MARKER_H
