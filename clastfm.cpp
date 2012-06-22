#include "clastfm.h"

CLastFm::CLastFm(QObject *parent, CDatabase *dbPointer) :
    QObject(parent)
{
    lastFmServer = "http://ws.audioscrobbler.com/2.0/";
    lastFmApiKey = "e1b67c44035db964af06f5ab00e719ac";
    lastFmApiSecret = "39afa2e6e9513a29b7b46d722dfc0819";
    lastFmSessionKey = "";

    isSessionOK = false;

    lastFmQueued = 0;
    lastFmSent = 0;

    db = dbPointer;
    myManager = new CNetworkAccessManager(this);

    connect(myManager, SIGNAL(finishedRemastered(QNetworkReply*,QString)), this, SLOT(managerReply(QNetworkReply*,QString)));

    // if (dbPointer->getLastFmOnline()) getSession(); // we're doing it in mainwindow class
}

bool CLastFm::getIsSessionOK()
{
    return isSessionOK;
}

void CLastFm::popMyQ()
{
    QString method;
    QByteArray bytearray;
    if (!methods.isEmpty()) method = methods.first();
    if (!bytearrays.isEmpty()) bytearray = bytearrays.first();

    QNetworkRequest req = QNetworkRequest(QUrl(lastFmServer));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    if ( (!method.isEmpty()) && (!bytearray.isEmpty()) ) myManager->postRemastered(req, bytearray, method);
}

void CLastFm::getSession()
{
    QString tmpMd5Password, tmpMd5Sig;
    QByteArray tmpByteArray;
    tmpByteArray.append(db->getLastFmPassword());
    tmpMd5Password = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    tmpByteArray.clear();
    tmpByteArray.append(db->getLastFmUser()+tmpMd5Password);
    tmpMd5Password = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();

    QString tmpSig;
    tmpSig = "api_key" + lastFmApiKey + "authToken" + tmpMd5Password + "methodauth.getMobileSession" + "username" + db->getLastFmUser() + lastFmApiSecret;
    tmpByteArray.clear();
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();

    QByteArray postData;
    postData.append("method=auth.getMobileSession&");
    postData.append("username="+db->getLastFmUser()+"&");
    postData.append("authToken="+tmpMd5Password+"&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("api_sig="+tmpMd5Sig);

    methods.prepend("auth.getMobileSession");
    bytearrays.prepend(postData);
    popMyQ();
}

void CLastFm::updateNowPlaying(QString title, QString artist, QString album)
{
    if (getIsSessionOK())
    {
        QString tmpSig, tmpMd5Sig;
        QByteArray tmpByteArray;
        tmpSig = "album" + album + "api_key" + lastFmApiKey + "artist" + artist + "methodtrack.updateNowPlaying" + "sk" + lastFmSessionKey + "track" + title + lastFmApiSecret;
        tmpByteArray.append(tmpSig);
        tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();

        QByteArray postData;

        postData.append("method=track.updateNowPlaying&");
        postData.append("track="+title+"&");
        postData.append("artist="+artist+"&");
        postData.append("album="+album+"&");
        postData.append("api_key="+lastFmApiKey+"&");
        postData.append("sk="+lastFmSessionKey+"&");
        postData.append("api_sig="+tmpMd5Sig);

        methods.append("track.updateNowPlaying");
        bytearrays.append(postData);
        popMyQ();
    }
}

void CLastFm::addSongToLoveQueue(QString title, QString artist, QString album)
{
    myLoveQueue[0] << artist;
    myLoveQueue[1] << title;
    myLoveQueue[2] << album;

    if ( (db->getLastFmOnline()) && (getIsSessionOK()) ) {
        loveSongs();
    }
}

void CLastFm::addSongToUnloveQueue(QString title, QString artist, QString album)
{
    myUnloveQueue[0] << artist;
    myUnloveQueue[1] << title;
    myUnloveQueue[2] << album;

    if ( (db->getLastFmOnline()) && (getIsSessionOK()) ) {
        unloveSongs();
    }
}

void CLastFm::addSongToQueue(QString title, QString artist, QString album)
{
    myLastQueue[0] << artist;
    myLastQueue[1] << title;
    myLastQueue[2] << album;
    QDateTime tmpTime = QDateTime::currentDateTime();
    QString tmpS;
    qint64 tmpI;
    tmpI = tmpTime.toTime_t();
    tmpS.setNum(tmpI);
    myLastQueue[3] << tmpS;
    lastFmQueued = myLastQueue[0].size();
    tmpS.setNum(lastFmQueued);
    emit queueChanged(lastFmQueued);

    if ( (db->getLastFmOnline()) && (getIsSessionOK()) ) {
        scrobbleSongs();
    }
}

void CLastFm::managerReply(QNetworkReply *pReply, QString mtd)
{
    int size1, size2;
    size1 = methods.size();
    size2 = bytearrays.size();
    if (size1 != size2) qDebug() << "WARNING ! Queues have different sizes !";
    else qDebug() << "BEGIN: Queue size is" << size1;
    if (size1 <= 0) return;

    QByteArray data=pReply->readAll();
    QString content(data);

    qDebug() << "lastFm manager reply :" << content;
    emit lastReply(content);
    qDebug() << "URL was :" << pReply->url().toString();


    // JEŚLI RESPONSE JEST OK, TO USUWAMY JUŻ TEN REQUEST
    if (content.contains("status=\"ok\""))
    {
        if (!methods.isEmpty()) methods.removeFirst();
        if (!bytearrays.isEmpty()) bytearrays.removeFirst();
    }



    // ===================== //
    // auth.getMobileSession //
    // ===================== //

    if (mtd == "auth.getMobileSession")
    {
        qDebug() << "I got auth.getMobileSession response...";
        if (content.contains("status=\"ok\""))
        {
            QRegExp myRegExp(".+<key>");
            content.remove(myRegExp);
            myRegExp = QRegExp("</key>.+");
            content.remove(myRegExp);
            lastFmSessionKey = content;
            isSessionOK = true;
        }
        else
        {
            isSessionOK = false;
            if (content.contains("error code=\"4\""))
            {
                qDebug() << "Check your last.fm user/pass";
                emit errSignal("Check your last.fm user/pass");
            }
            else if (content.contains("error code=\"11\""))
            {
                qDebug() << "Last.fm is temporarily offline";
                emit errSignal("Last.fm is temporarily offline");
            }
            else
            {
                qDebug() << "Some strange error occured and I couldn't get lastFm session";
                emit errSignal("I couldn't get last.fm session");
            }
        }
    }

    // ====================== //
    // track.updateNowPlaying //
    // ====================== //

    else if (mtd == "track.updateNowPlaying")
    {
        qDebug() << "I got track.updateNowPlaying response...";
        if (content.contains("status=\"ok\""))
        {
            //
        }
        else
        {
            if (content.contains("error code=\"9\""))
            {
                qDebug() << "Invalid session key, I'm re-authenticating...";
                isSessionOK = false;
                getSession();
            }
            else if (content.contains("error code=\"11\""))
            {
                qDebug() << "Last.fm is temporarily offline";
                emit errSignal("Last.fm is temporarily offline");
            }
            else if (content.contains("error code=\"16\""))
            {
                qDebug() << "Last.fm is temporarily unavailable";
                emit errSignal("Last.fm is temporarily unavailable");
            }
            else
            {
                qDebug() << "Some strange error occured and I couldn't updateNowPlaying";
                emit errSignal("I couldn't update \"Now Playing\"");
            }
        }
    }

    // ========== //
    // track.love //
    // ========== //

    else if (mtd == "track.love")
    {
        qDebug() << "I got track.love response...";

        if (content.contains("status=\"ok\""))
        {
            if (!myLoveQueue[0].isEmpty()) {
                myLoveQueue[0].removeAt(0);
                myLoveQueue[1].removeAt(0);
                myLoveQueue[2].removeAt(0);
            }
            if (!myLoveQueue[0].isEmpty()) {
                loveSongs();
            }
        }
    }

    // ============ //
    // track.unlove //
    // ============ //

    else if (mtd == "track.unlove")
    {
        qDebug() << "I got track.unlove response...";

        if (content.contains("status=\"ok\""))
        {
            if (!myUnloveQueue[0].isEmpty()) {
                myUnloveQueue[0].removeAt(0);
                myUnloveQueue[1].removeAt(0);
                myUnloveQueue[2].removeAt(0);
            }
            if (!myUnloveQueue[0].isEmpty()) {
                unloveSongs();
            }
        }
    }

    // ============== //
    // track.scrobble //
    // ============== //

    else if (mtd == "track.scrobble")
    {
        qDebug() << "I got track.scrobble response...";

        int oldSize = myLastQueue[0].size();
        int newSize;

        if (content.contains("status=\"ok\""))
        {
                if (myLastQueue[0].size() > 10)
                {
                    for (int i = 0 ; i <= 9 ; i++)
                    {
                        myLastQueue[0].removeAt(0);
                        myLastQueue[1].removeAt(0);
                        myLastQueue[2].removeAt(0);
                        myLastQueue[3].removeAt(0);
                    }
                    newSize = myLastQueue[0].size();
                    lastFmSent = lastFmSent + (oldSize - newSize);
                    emit sentChanged(lastFmSent);
                    lastFmQueued = lastFmQueued - (oldSize - newSize);
                    emit queueChanged(lastFmQueued);

                    scrobbleSongs();
                }
                else
                {
                    int tmpSize = myLastQueue[0].size();
                    for (int i = 0 ; i < tmpSize ; i++)
                    {
                        myLastQueue[0].removeAt(0);
                        myLastQueue[1].removeAt(0);
                        myLastQueue[2].removeAt(0);
                        myLastQueue[3].removeAt(0);
                    }
                    emit ifSongIsPlayingUpdateNowPlaying();
                    newSize = myLastQueue[0].size();
                    lastFmSent = lastFmSent + (oldSize - newSize);
                    emit sentChanged(lastFmSent);
                    lastFmQueued = lastFmQueued - (oldSize - newSize);
                    emit queueChanged(lastFmQueued);
                }

        }
        else qDebug() << "There was an error sending track(s) to Last.fm";

    }

    // ==================== //
    // other Method name... //
    // ==================== //

    else
    {
        qDebug() << "Strange response, ignoring...";
    }

    size1 = methods.size();
    size2 = bytearrays.size();
    if (size1 != size2) qDebug() << "WARNING ! Queues have different sizes !";
    else qDebug() << "END: Queue size is" << size1;

    // if (size1 > 0) popMyQ();
}

void CLastFm::loveSongs()
{
    QString tmpSig, tmpMd5Sig;
    QByteArray tmpByteArray;
    QByteArray postData;

    postData.append("method=track.love&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("sk="+lastFmSessionKey+"&");

    QString artist, title;
    artist = myLoveQueue[0][0];
    title = myLoveQueue[1][0];
    postData.append("artist="+artist+"&");
    postData.append("track="+title+"&");

    tmpSig = "api_key" + lastFmApiKey + "artist" + artist + "methodtrack.love" + "sk" + lastFmSessionKey + "track" + title + lastFmApiSecret;
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    postData.append("api_sig="+tmpMd5Sig);

    methods.append("track.love");
    bytearrays.append(postData);
    popMyQ();
}

void CLastFm::unloveSongs()
{
    QString tmpSig, tmpMd5Sig;
    QByteArray tmpByteArray;
    QByteArray postData;

    postData.append("method=track.unlove&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("sk="+lastFmSessionKey+"&");

    QString artist, title;
    artist = myUnloveQueue[0][0];
    title = myUnloveQueue[1][0];
    postData.append("artist="+artist+"&");
    postData.append("track="+title+"&");

    tmpSig = "api_key" + lastFmApiKey + "artist" + artist + "methodtrack.unlove" + "sk" + lastFmSessionKey + "track" + title + lastFmApiSecret;
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    postData.append("api_sig="+tmpMd5Sig);

    methods.append("track.unlove");
    bytearrays.append(postData);
    popMyQ();
}

void CLastFm::scrobbleSongs()
{
    QString tmpSig, tmpMd5Sig;
    QByteArray tmpByteArray;
    QByteArray postData;

    postData.append("method=track.scrobble&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("sk="+lastFmSessionKey+"&");

    QString tmp1, tmp2, tmp3, tmp4;
    for (int i = 0 ; i <= 9 ; i++)
    {
        if (i < myLastQueue[0].size())
        {
            QString artist = myLastQueue[0][i];
            QString title = myLastQueue[1][i];
            QString album = myLastQueue[2][i];
            QString timestamp = myLastQueue[3][i];
            QString tmpIndex;
            tmpIndex.setNum(i);

            postData.append("artist["+tmpIndex+"]="+artist+"&");
            postData.append("track["+tmpIndex+"]="+title+"&");
            postData.append("album["+tmpIndex+"]="+album+"&");
            postData.append("timestamp["+tmpIndex+"]="+timestamp+"&");
            tmp1 = tmp1 + "album["+tmpIndex+"]" + album;
            tmp2 = tmp2 + "artist["+tmpIndex+"]" + artist;
            tmp3 = tmp3 + "timestamp["+tmpIndex+"]" + timestamp;
            tmp4 = tmp4 + "track["+tmpIndex+"]" + title;
        }
    }

    tmpSig = tmp1 + "api_key" + lastFmApiKey + tmp2 + "methodtrack.scrobble" + "sk" + lastFmSessionKey + tmp3 + tmp4 + lastFmApiSecret;
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    postData.append("api_sig="+tmpMd5Sig);

    methods.append("track.scrobble");
    bytearrays.append(postData);
    popMyQ();
}

void CLastFm::updateEverything()
{
    if (!myLoveQueue[0].isEmpty()) loveSongs();
    if (!myUnloveQueue[0].isEmpty()) unloveSongs();
    if (!myLastQueue[0].isEmpty()) scrobbleSongs();
}
