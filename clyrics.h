#ifndef CLYRICS_H
#define CLYRICS_H

#include "cdatabase.h"
#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QRegExp>

class CLyrics : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *myManager;
    CDatabase *db;

public:
    explicit CLyrics(QObject *parent = 0, CDatabase *dbPointer = 0);

signals:
    void lyricsChanged(QString);
    void errSignal(QString);

public slots:
    void getLyrics(QString artist, QString title);
    void replyFinished(QNetworkReply*);

};

#endif // CLYRICS_H
