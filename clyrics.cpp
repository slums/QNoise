#include "clyrics.h"

CLyrics::CLyrics(QObject *parent, CDatabase *dbPointer) :
    QObject(parent)
{
    db = dbPointer;
    myManager = new QNetworkAccessManager(this);

    connect(myManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void CLyrics::getLyrics(QString artist, QString title)
{
    myManager->get(QNetworkRequest(QUrl("http://lyrics.wikia.com/api.php?artist="+artist+"&song="+title)));
}

void CLyrics::replyFinished(QNetworkReply *pReply)
{
    QByteArray data=pReply->readAll();
    QString content(data);

    if (pReply->url().toString().contains("api.php"))
    {
        QRegExp myRegExp(".+</strong>");
        content.remove(myRegExp);
        myRegExp = QRegExp("</li>.+");
        content.remove(myRegExp);

        if (content.contains("action=edit"))
        {
            qDebug() << "Lyrics not found...";
            emit lyricsChanged("Lyrics not found...");
        }
        else
        {
            content.replace("http://lyrics.wikia.com/", "http://lyrics.wikia.com/index.php?title=");
            content = content + "&action=edit#EditPage";
            QByteArray tmpByteArray;
            tmpByteArray.append(content);
            myManager->get(QNetworkRequest(QUrl::fromEncoded(tmpByteArray)));
        }
    }
    else if (content.contains("&lt;/lyrics&gt"))
    {
        QRegExp myRegExp("&lt;/lyrics&gt;.+");
        content.remove(myRegExp);
        myRegExp = QRegExp(".+&lt;lyrics&gt;");
        content.remove(myRegExp);
        myRegExp = QRegExp("\\s*$");
        content.remove(myRegExp);
        myRegExp = QRegExp("^\\s*");
        content.remove(myRegExp);
        content.replace("&quot;", "\"");
        qDebug() << content;
        emit lyricsChanged(content);
    }

    else
    {
        qDebug() << "There was some error with lyrics, sorry...";
        emit lyricsChanged("There was some error with lyrics, sorry...");
    }
}
