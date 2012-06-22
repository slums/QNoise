#ifndef CLASTFM_H
#define CLASTFM_H

#include "cdatabase.h"
#include "cnetworkaccessmanager.h"
#include <QObject>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QRegExp>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <QDateTime>

class CLastFm : public QObject
{
    Q_OBJECT

private:
    bool isSessionOK;
    QString lastFmServer, lastFmApiKey, lastFmApiSecret, lastFmSessionKey;
    CDatabase *db;
    CNetworkAccessManager *myManager;
    QStringList methods;
    QList<QByteArray> bytearrays;

    QStringList myLastQueue[4];
    QStringList myLoveQueue[3];
    QStringList myUnloveQueue[3];
    int lastFmSent, lastFmQueued;

public:
    explicit CLastFm(QObject *parent = 0, CDatabase *dbPointer = 0);

signals:
    void errSignal(QString);
    void lastReply(QString);
    void queueChanged(int);
    void sentChanged(int);
    void ifSongIsPlayingUpdateNowPlaying();

public slots:
    bool getIsSessionOK();
    void popMyQ();
    void getSession();
    void updateNowPlaying(QString title, QString artist, QString album);
    void addSongToLoveQueue(QString title, QString artist, QString album);
    void addSongToUnloveQueue(QString title, QString artist, QString album);
    void managerReply(QNetworkReply*, QString);
    void addSongToQueue(QString title, QString artist, QString album);
    void scrobbleSongs();
    void loveSongs();
    void unloveSongs();
    void updateEverything();

};

#endif // CLASTFM_H
