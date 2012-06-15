#include "cdatabase.h"

CDatabase::CDatabase(QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("QNoiseDB");
    if (!db.open()) {
        //MainWindow::displayWarning("Cannot open database QNoiseDB");
        qDebug() << "Cannot open database QNoiseDB";
        qDebug() << db.lastError();
        isDatabaseOK = false;
    }
    else
    {
        isDatabaseOK = true;
        qDebug() << "Opened database QNoiseDB";
        QSqlQuery query;
        if (query.exec("CREATE TABLE IF NOT EXISTS mainSettings ( id INTEGER PRIMARY KEY, last_online INTEGER, lyrics_online INTEGER, last_user VARCHAR(40), last_pass VARCHAR(40), repeat INTEGER, shuffle INTEGER )")) qDebug() << "Table mainSettings : OK";
        else
        {
            qDebug() << "Error creating table mainSettings";
            qDebug() << db.lastError();
            isDatabaseOK = false;
        }

        if (query.exec("CREATE TABLE IF NOT EXISTS myFavs ( id INTEGER PRIMARY KEY, artist VARCHAR(40), title VARCHAR(40), album VARCHAR(40) )")) qDebug() << "Table myFavs : OK";
        else
        {
            qDebug() << "Error creating table myFavs";
            qDebug() << db.lastError();
            isDatabaseOK = false;
        }

        if (query.exec("CREATE TABLE IF NOT EXISTS lastToScrobble ( id INTEGER PRIMARY KEY, artist VARCHAR(40), title VARCHAR(40), album VARCHAR(40), seconds INTEGER )")) qDebug() << "Table lastToScrobble : OK";
        else
        {
            qDebug() << "Error creating table lastToScrobble";
            qDebug() << db.lastError();
            isDatabaseOK = false;
        }

        if (query.exec("CREATE TABLE IF NOT EXISTS lastToLove ( id INTEGER PRIMARY KEY, artist VARCHAR(40), title VARCHAR(40), album VARCHAR(40) )")) qDebug() << "Table lastToLove : OK";
        else
        {
            qDebug() << "Error creating table lastToLove";
            qDebug() << db.lastError();
            isDatabaseOK = false;
        }

        if (query.exec("CREATE TABLE IF NOT EXISTS lastToUnlove ( id INTEGER PRIMARY KEY, artist VARCHAR(40), title VARCHAR(40), album VARCHAR(40) )")) qDebug() << "Table lastToUnlove : OK";
        else
        {
            qDebug() << "Error creating table lastToUnlove";
            qDebug() << db.lastError();
            isDatabaseOK = false;
        }

        addEmptyRowIfTableIsEmpty();
        getValuesFromDatabase();

    }
}

void CDatabase::getValuesFromDatabase()
{
    QSqlQuery query;
    if (!query.exec("SELECT * FROM mainSettings"))
    {
        qDebug() << "Select error : mainSettings";
        qDebug() << db.lastError();
    }
    else
    {
        if (!query.next())
        {
            qDebug() << "Some error with database.. I expected some values but got none..";
            qDebug() << db.lastError();
        }
        else
        {
            QSqlRecord r = query.record();
            int fieldNo1 = r.indexOf("last_online");
            int fieldNo2 = r.indexOf("lyrics_online");
            int fieldNo3 = r.indexOf("last_user");
            int fieldNo4 = r.indexOf("last_pass");
            int fieldNo5 = r.indexOf("repeat");
            int fieldNo6 = r.indexOf("shuffle");

            QString tmpValue;
            tmpValue = query.value(fieldNo1).toString();
            lastFmOnline = tmpValue.toInt();
            qDebug() << "Set last_online to" << tmpValue;

            tmpValue = query.value(fieldNo2).toString();
            lyricsOnline = tmpValue.toInt();
            qDebug() << "Set lyrics_online to" << tmpValue;

            tmpValue = query.value(fieldNo3).toString();
            lastFmUser = tmpValue;
            qDebug() << "Set last_user to" << tmpValue;

            tmpValue = query.value(fieldNo4).toString();
            lastFmPassword = tmpValue;
            qDebug() << "Set last_pass to ...";

            tmpValue = query.value(fieldNo5).toString();
            repeat = tmpValue.toInt();
            qDebug() << "Set repeat to" << tmpValue;

            tmpValue = query.value(fieldNo6).toString();
            shuffle = tmpValue.toInt();
            qDebug() << "Set shuffle to" << tmpValue;
        }
    }
}

void CDatabase::addEmptyRowIfTableIsEmpty()
{
    QSqlQuery query;
    if (!query.exec("SELECT * FROM mainSettings"))
    {
        qDebug() << "Select error : mainSettings";
        qDebug() << db.lastError();
    }

    if (!query.next())
    {
        qDebug() << "We have to add an empty row to mainSettings..";
        if (query.exec("INSERT INTO mainSettings (last_online, lyrics_online, last_user, last_pass, repeat, shuffle) VALUES (0, 0, \"user\", \"pass\", 0, 0)")) qDebug() << "New entry inserted successfully into mainSettings";
        else
        {
            qDebug() << "Error inserting new entry into mainSettings";
            qDebug() << db.lastError();
        }
    }

    else
    {
        qDebug() << "Query is not empty, so we don't have to add anything..";
    }
}

void CDatabase::updateOneFieldInDatabase(QString key, QString value)
{
    QSqlQuery query;
    query.prepare("UPDATE mainSettings SET " + key + " = :v1 WHERE id = 1");
    query.bindValue(":v1", value);
    if (query.exec()) {
        if (key != "last_pass") qDebug() << "Set" << key << "to" << value << "and updated database..";
        else qDebug() << "Set" << key << "to ... and updated database..";
    }
    else
    {
        qDebug() << "Error updating database";
        qDebug() << db.lastError();
    }
}

bool CDatabase::getIsDatabaseOK()
{
    return isDatabaseOK;
}

bool CDatabase::getLastFmOnline()
{
    return lastFmOnline;
}

void CDatabase::setLastFmOnline(bool value)
{
    lastFmOnline = value;
    if (value) updateOneFieldInDatabase("last_online", "1");
    else updateOneFieldInDatabase("last_online", "0");
}

bool CDatabase::getLyricsOnline()
{
    return lyricsOnline;
}

void CDatabase::setLyricsOnline(bool value)
{
    lyricsOnline = value;
    if (value) updateOneFieldInDatabase("lyrics_online", "1");
    else updateOneFieldInDatabase("lyrics_online", "0");
}

bool CDatabase::getRepeat()
{
    return repeat;
}

void CDatabase::setRepeat(bool value)
{
    repeat = value;
    if (value) updateOneFieldInDatabase("repeat", "1");
    else updateOneFieldInDatabase("repeat", "0");
}

bool CDatabase::getShuffle()
{
    return shuffle;
}

void CDatabase::setShuffle(bool value)
{
    shuffle = value;
    if (value) updateOneFieldInDatabase("shuffle", "1");
    else updateOneFieldInDatabase("shuffle", "0");
}

QString CDatabase::getLastFmUser()
{
    return lastFmUser;
}

void CDatabase::setLastFmUser(QString value)
{
    lastFmUser = value;
    updateOneFieldInDatabase("last_user", value);
}

QString CDatabase::getLastFmPassword()
{
    return lastFmPassword;
}

void CDatabase::setLastFmPassword(QString value)
{
    lastFmPassword = value;
    updateOneFieldInDatabase("last_pass", value);
}
