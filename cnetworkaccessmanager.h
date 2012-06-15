#ifndef CNETWORKACCESSMANAGER_H
#define CNETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QByteArray>
#include <QString>

class CNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
private:
    QString method;

public:
    explicit CNetworkAccessManager(QObject *parent = 0, QString mtd = "");

signals:
    void finishedRemastered(QNetworkReply*, QString);

public slots:
    void itFinished(QNetworkReply*);
    void postRemastered(QNetworkRequest, QByteArray, QString);

};

#endif // CNETWORKACCESSMANAGER_H
