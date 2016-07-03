#include "filehash.h"

FileHash::FileHash(QObject *parent) :
    QThread(parent)
{
}

void FileHash::setFilename(QString f)
{
    _filename = f;
}

void FileHash::run()
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile file(_filename);
    file.open(QIODevice::ReadOnly);

    QString linehash;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        hash.reset();
        hash.addData(line);
        linehash += hash.result();
    }

    hash.reset();
    hash.addData(linehash.toUtf8());
    QString res = QString(hash.result().toBase64());
    emit resultReady(res);
}
