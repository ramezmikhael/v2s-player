#ifndef FILEHASH_H
#define FILEHASH_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QCryptographicHash>

class FileHash : public QThread
{
    Q_OBJECT
public:
    explicit FileHash(QObject *parent = 0);
    void setFilename(QString);
    void run() Q_DECL_OVERRIDE;

signals:
    void resultReady(QString);

private:
    QString _filename;

};

#endif // FILEHASH_H
