#ifndef CDATABASE_H
#define CDATABASE_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>


class CDatabase : public QObject
{
    Q_OBJECT

private:
    bool isDatabaseOK;
    bool lastFmOnline, lyricsOnline, repeat, shuffle;
    QString lastFmUser, lastFmPassword;
    QSqlDatabase db;


public:
    explicit CDatabase(QObject *parent = 0);

signals:
    void errSignal(QString);

public slots:
    void addEmptyRowIfTableIsEmpty();
    void updateOneFieldInDatabase(QString, QString);
    void getValuesFromDatabase();
    bool getIsDatabaseOK();
    bool getLastFmOnline();
    void setLastFmOnline(bool);
    bool getLyricsOnline();
    void setLyricsOnline(bool);
    bool getRepeat();
    void setRepeat(bool);
    bool getShuffle();
    void setShuffle(bool);
    QString getLastFmUser();
    void setLastFmUser(QString);
    QString getLastFmPassword();
    void setLastFmPassword(QString);

};

#endif // CDATABASE_H
