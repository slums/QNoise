#include "cnetworkaccessmanager.h"

CNetworkAccessManager::CNetworkAccessManager(QObject *parent, QString mtd) :
    QNetworkAccessManager(parent)
{
    method = mtd;
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(itFinished(QNetworkReply*)));
}

void CNetworkAccessManager::itFinished(QNetworkReply *pReply)
{
    emit finishedRemastered(pReply, method);
}

void CNetworkAccessManager::postRemastered(QNetworkRequest tmpRequest, QByteArray tmpByteArray, QString tmpMtd)
{
    method = tmpMtd;
    this->post(tmpRequest, tmpByteArray);
}
