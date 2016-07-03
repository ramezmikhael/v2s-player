#include "markerdb.h"

MarkerDB::MarkerDB(QObject *parent) :
    QObject(parent)
{
    dbFilename = "/markers.db";

    //Create database object
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + dbFilename);
    if(!db.open())
        qDebug() << db.lastError().text();

    //Check if database exists, or else create one
    QSqlQuery qry;
    qry.prepare("SELECT TValue FROM AppVersion where TKey = 'version'");
    if(!qry.exec())
        createDatabase();
}

int MarkerDB::addFile(QString filename, qint64 duration, QString MD5Hash)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery qry;
    qry.prepare("INSERT INTO Files (Filename, Duration, MD5Hash) "
                "Values (:Filename, :Duration, :MD5Hash)");
    qry.bindValue(":Filename", QString::fromUtf8(filename.toUtf8()));
    qry.bindValue(":Duration", duration);
    qry.bindValue(":MD5Hash", MD5Hash);
    if(!qry.exec())
        return -1;

    //Get last inserted ID
    qry.exec("SELECT MAX(FileID) From Files");
    qry.next();
    return qry.value(0).toInt();
}

int MarkerDB::addMark(int fileDbId, qint64 msec, QString caption, QString comment)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery qry;

    //Insert mark in place
    qry.prepare("INSERT INTO FileMarks (FileID, Caption, Comment, Millisecond) "
                "Values (:FileID, :Caption, :Comment, :Millisecond)");
    qry.bindValue(":FileID", fileDbId);
    qry.bindValue(":Caption", QString::fromUtf8(caption.toUtf8()));
    qry.bindValue(":Comment", QString::fromUtf8(comment.toUtf8()));
    qry.bindValue(":Millisecond", msec);

    if(!qry.exec())
        return -1;

    return fileDbId;

}

bool MarkerDB::editMark(int markId, QString caption, QString comment)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery qry;

    //Insert mark in place
    qry.prepare("UPDATE FileMarks SET Caption = :Caption, Comment = :Comment "
                "WHERE MarkID = :MarkID");

    qry.bindValue(":Caption", QString::fromUtf8(caption.toUtf8()));
    qry.bindValue(":Comment", QString::fromUtf8(comment.toUtf8()));
    qry.bindValue(":MarkID", markId);

    if(!qry.exec())
        return false;

    return true;
}

bool MarkerDB::removeMark(int markId)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery qry;

    //Insert mark in place
    qry.prepare("DELETE FROM FileMarks WHERE MarkID = :MarkID");
    qry.bindValue(":MarkID", markId);

    if(!qry.exec())
        return false;

    return true;
}

int MarkerDB::queryFileDbId(QString MD5Hash, QString filename, qint64 duration)
{
    if(db.isOpen())
        db.open();

    QSqlQuery qry;
    qry.prepare("SELECT FileID, Filename, Duration FROM Files WHERE MD5Hash = :MD5Hash");
    qry.bindValue(":MD5Hash", MD5Hash);
    qry.exec();
    if(qry.next())
    {
        int fileID = qry.value(0).toInt();
        QString oldFilename = qry.value(1).toString();

        if(oldFilename != filename)
            updateFilename(fileID, filename);

        return fileID;
    }
    else
    {
        return addFile(filename, duration, MD5Hash);
    }
}

void MarkerDB::updateFilename(int fileID, QString filename)
{
    if(db.isOpen())
        db.open();

    QSqlQuery qry;
    qry.prepare("UPDATE Files SET Filename = :Filename WHERE FileID = :FileID");
    qry.bindValue(":Filename", filename);
    qry.bindValue(":FileID", fileID);
    if(!qry.exec())
        qCritical() << "Cannot execute update query";
}

QSqlQuery* MarkerDB::search(QString criteria)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery *qry = new QSqlQuery("SELECT MarkID, IFNULL(Caption, 'Undefined') AS Caption, "
                                   "IFNULL(Comment, 'NO COMMENT') AS Comment, "
                                   "Millisecond, Millisecond / 1000 AS Sec, Filename "
                                   "FROM Files "
                                   "LEFT JOIN FileMarks ON Files.FileID = FileMarks.FileID "
                                   "WHERE Caption LIKE '%" + criteria +"%' OR Comment LIKE '%" + criteria + "%' "
                                   "OR Filename like '%" + criteria + "%'");
    if(!qry->exec())
        qCritical() << qry->lastError().text();

    return qry;
}

QSqlQuery* MarkerDB::searchMarkById(int id)
{
    if(!db.isOpen())
        db.open();

    QSqlQuery *qry = new QSqlQuery("SELECT Filename, Duration, IFNULL(Caption, 'Undefined') AS Caption, "
                                   "IFNULL(Comment, 'NO COMMENT') AS Comment, "
                                   "Millisecond, Millisecond / 1000 AS Sec "
                                   "FROM Files "
                                   "LEFT JOIN FileMarks ON Files.FileID = FileMarks.FileID "
                                   "WHERE MarkID = " + QString::number(id));
    if(!qry->exec())
        qCritical() << qry->lastError().text();

    return qry;
}

void MarkerDB::createDatabase()
{
    if(db.isOpen())
        db.open();

    QSqlQuery qry;
    qry.exec("CREATE TABLE AppVersion (ID INTEGER PRIMARY KEY AUTOINCREMENT, TKey NVARCHAR, TValue NVARCHAR) ");
    qry.exec("CREATE TABLE Files (FileID INTEGER PRIMARY KEY AUTOINCREMENT, Filename NVARCHAR, Duration TIMESTAMP, MD5Hash NVARCHAR)");
    qry.exec("CREATE TABLE FileMarks (MarkID INTEGER PRIMARY KEY AUTOINCREMENT, FileID INTEGER, Caption NVARCHAR, Comment NVARCHAR, Millisecond TIMESTAMP)");
    qry.exec("INSERT INTO AppVersion (TKey, TValue) values ('version', '1')");
}
