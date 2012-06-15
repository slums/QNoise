#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow()
{
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    ui->pauseButton->hide();
    ui->elapsedTimeLCD->setText("00:00");
    ui->remainingTimeLCD->setText("00:00");
    ui->elapsedTimeLCD->hide();
    ui->remainingTimeLCD->hide();
    ui->mySeekSlider->setIconVisible(false);
    ui->myVolumeSlider->setMuteVisible(false);
    ui->mySeekSlider->hide();
    ui->statusLabel->hide();
    ui->loveMinusButton->hide();
    ui->lovePlusButton->hide();
    ui->loveLabel->hide();
    createTable();
    createPointers();
    ui->mySeekSlider->setMediaObject(currentSong);
    currentSong->setTickInterval(900);

    ui->myVolumeSlider->setAudioOutput(audioOutput);

    Phonon::createPath(currentSong, audioOutput);
    makeConnections();

    myPlayingFont.setBold(true);
    *currentIndex = 0;
    *previousIndex = 0;

    // connectToDatabase();
    getConfiguration();

    if (myDatabase->getLastFmOnline())
    {
        myLastFm->getSession();
    }
}

void MainWindow::createPointers()
{
    myDatabase = new CDatabase(this);
    myLastFm = new CLastFm(this, myDatabase);
    myLyrics = new CLyrics(this, myDatabase);

    myResolver = new Phonon::MediaObject(this);
    myTimeResolver = new Phonon::MediaObject(this);
    currentSong = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    currentIndex = new int;
    previousIndex = new int;
}

void MainWindow::makeConnections()
{
    connect(ui->addFilesButton, SIGNAL(clicked()), this, SLOT(chooseFiles()));
    connect(myResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(myResolverStateChanged(Phonon::State, Phonon::State)));
    connect(myTimeResolver, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime(qint64)));
    connect(ui->musicTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(startPlaying(int, int)));
    connect(ui->stopButton, SIGNAL(clicked()), currentSong, SLOT(stop()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearTable()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearSongList()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playButtonPressed()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pauseButtonPressed()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(previousButtonPressed()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(nextButtonPressed()));
    connect(ui->loveMinusButton, SIGNAL(clicked()), this, SLOT(loveMinusButtonPressed()));
    connect(ui->lovePlusButton, SIGNAL(clicked()), this, SLOT(lovePlusButtonPressed()));
    connect(currentSong, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(currentSongStateChanged(Phonon::State,Phonon::State)));
    connect(currentSong, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(currentSong, SIGNAL(finished()), this, SLOT(nextButtonPressed()));
    connect(ui->lastFm_updateButton, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
    connect(ui->settings_repeatCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkRepeatToggled(bool)));
    connect(ui->settings_lastFmCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkLastToggled(bool)));
    connect(ui->settings_lyricsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkLyricsToggled(bool)));
    connect(ui->settings_passwordLineEdit, SIGNAL(editingFinished()), this, SLOT(settingsLastEditingFinished()));
    connect(ui->settings_userLineEdit, SIGNAL(editingFinished()), this, SLOT(settingsLastEditingFinished()));
    connect(myLastFm, SIGNAL(lastReply(QString)), this, SLOT(lastReply(QString)));
    connect(myLastFm, SIGNAL(queueChanged(int)), this, SLOT(lastQueueChanged(int)));
    connect(myLastFm, SIGNAL(sentChanged(int)), this, SLOT(lastSentChanged(int)));
    connect(myLyrics, SIGNAL(lyricsChanged(QString)), this, SLOT(lyricsChanged(QString)));
    connect(myLastFm, SIGNAL(ifSongIsPlayingUpdateNowPlaying()), this, SLOT(ifSongIsPlayingUpdateNowPlaying()));
}

void MainWindow::getConfiguration()
{
    ui->settings_userLineEdit->insert(myDatabase->getLastFmUser());
    ui->settings_passwordLineEdit->insert(myDatabase->getLastFmPassword());
    ui->settings_lastFmCheckBox->setChecked(myDatabase->getLastFmOnline());
    ui->settings_lyricsCheckBox->setChecked(myDatabase->getLyricsOnline());
    ui->settings_repeatCheckBox->setChecked(myDatabase->getRepeat());
}

void MainWindow::displayWarning(QString msg)
{
    QMessageBox::warning(this, "Error", msg);
}

void MainWindow::lastReply(QString reply)
{
    ui->lastFm_textBrowser->append(reply);
}

//
void MainWindow::updateLastFm()
{
    /*if ( (myDatabase->getLastFmOnline()) && (myLastFm->getIsSessionOK()) )
    {
        if (!myLastQueue[0].isEmpty()) lastFmScrobbleSongs();
        if (!myLastFavQueue[0].isEmpty()) lastFmLoveSongs();
        if (!myLastUnfavQueue[0].isEmpty()) lastFmUnloveSongs();
        if (currentSong->state() == Phonon::PlayingState) lastFmUpdateNowPlaying();
    }*/
}
//



void MainWindow::updateButtonClicked()
{
    if (myDatabase->getLastFmOnline())
    {
        myLastFm->getSession();
        if (myLastFm->getIsSessionOK()) updateLastFm();
    }
    else displayWarning("Last.fm mode is currently offline...");
}

void MainWindow::settingsLastEditingFinished()
{
    lastFmUser = ui->settings_userLineEdit->text();
    lastFmPassword = ui->settings_passwordLineEdit->text();

    myDatabase->setLastFmUser(lastFmUser);
    myDatabase->setLastFmPassword(lastFmPassword);
}

void MainWindow::checkLastToggled(bool state)
{
    myDatabase->setLastFmOnline(state);
}

void MainWindow::checkLyricsToggled(bool state)
{
    myDatabase->setLyricsOnline(state);
}

void MainWindow::checkRepeatToggled(bool state)
{
    myDatabase->setRepeat(state);
}

void MainWindow::tick(qint64 time)
{
    QTime elapsedTime(0, (time / 60000) % 60, (time / 1000) % 60);
    QTime remainingTime(0, ((currentSong->totalTime() - time) / 60000) % 60, ((currentSong->totalTime() - time) / 1000) % 60);

    ui->elapsedTimeLCD->setText(elapsedTime.toString("mm:ss"));
    ui->remainingTimeLCD->setText(remainingTime.toString("mm:ss"));

    if ( (elapsedTime.toString("mm:ss") == "00:30") || (elapsedTime.toString("mm:ss") == "00:31") )
    {
        addToLast = true;
        // DAWID
        // dodać tej piosence jedno odtworzenie więcej w bibliotece
    }
}

void MainWindow::createTable()
{
    ui->musicTable->setColumnWidth(0, 30);
    ui->musicTable->setColumnWidth(1, 150);
    ui->musicTable->setColumnWidth(2, 150);
    ui->musicTable->setColumnWidth(3, 150);
    ui->musicTable->setColumnWidth(4, 91);
}

void MainWindow::clearTable()
{
    int tmp = ui->musicTable->rowCount();
    for (int i = 0 ; i < tmp ; i++) ui->musicTable->removeRow(0);
}

void MainWindow::clearSongList()
{
    songList.clear();
    timeList.clear();
}

void MainWindow::startPlaying(int row, int column)
{
    if (songList.size() >= row)
    {
        *previousIndex = *currentIndex;
        *currentIndex = row;
        currentSong->setCurrentSource(songList[row]);
        currentSong->play();
    }
}

void MainWindow::playButtonPressed()
{
    if (currentSong->state() == Phonon::PausedState)
    {
        currentSong->play();
    }

    else
    {
        int row = ui->musicTable->currentRow();
        if ((row >= 0) && (row < songList.size()))
        {
            *previousIndex = *currentIndex;
            *currentIndex = row;
            currentSong->setCurrentSource(songList[row]);
            currentSong->play();
        }
    }
}

void MainWindow::pauseButtonPressed()
{
    if (currentSong->state() == Phonon::PlayingState)
    {
            currentSong->pause();
    }
}

void MainWindow::previousButtonPressed()
{
    int previous = *currentIndex - 1;
    if (songList.size() != 0)
    {
        if (myDatabase->getRepeat()) previous = (previous + songList.size()) % songList.size();
    }
    if ((previous >= 0) && (previous < songList.size()))
    {
        *previousIndex = *currentIndex;
        *currentIndex = previous;
        currentSong->setCurrentSource(songList[previous]);
        currentSong->play();
    }
}

void MainWindow::nextButtonPressed()
{
    int next = *currentIndex + 1;
    if (songList.size() != 0)
    {
        if (myDatabase->getRepeat()) next = next % songList.size();
    }
    if ((next >= 0) && (next < songList.size()))
    {
        *previousIndex = *currentIndex;
        *currentIndex = next;
        currentSong->setCurrentSource(songList[next]);
        currentSong->play();
    }
}

void MainWindow::loveMinusButtonPressed()
{

    int tmpSize = myFavourites[0].size();
    if ( tmpSize > myFavourites[1].size() )
        tmpSize = myFavourites[1].size();
    if ( tmpSize > myFavourites[2].size() )
        tmpSize = myFavourites[2].size();

    for (int i = 0 ; i < tmpSize ; i++)
    {
        if ( (myFavourites[0][i] == currentSong->metaData().value("ARTIST"))
        && (myFavourites[1][i] == currentSong->metaData().value("TITLE"))
        && (myFavourites[2][i] == currentSong->metaData().value("ALBUM")) )
            {
                myFavourites[0].removeAt(i);
                myFavourites[1].removeAt(i);
                myFavourites[2].removeAt(i);
                tmpSize--;
            }
    }

    ui->loveMinusButton->hide();
    ui->lovePlusButton->show();
    ui->loveLabel->hide();

    myLastUnfavQueue[0] << currentSong->metaData().value("ARTIST");
    myLastUnfavQueue[1] << currentSong->metaData().value("TITLE");
    myLastUnfavQueue[2] << currentSong->metaData().value("ALBUM");
    if ( (myDatabase->getLastFmOnline()) && (myLastFm->getIsSessionOK()) )
    {
        // lastFmUnloveSongs();
    }

    // DAWID
    // usunąć tą piosenkę z ulubionych w bibliotece
}

void MainWindow::lovePlusButtonPressed()
{
    myFavourites[0] << currentSong->metaData().value("ARTIST");
    myFavourites[1] << currentSong->metaData().value("TITLE");
    myFavourites[2] << currentSong->metaData().value("ALBUM");
    myLastFavQueue[0] << currentSong->metaData().value("ARTIST");
    myLastFavQueue[1] << currentSong->metaData().value("TITLE");
    myLastFavQueue[2] << currentSong->metaData().value("ALBUM");
    ui->lovePlusButton->hide();
    ui->loveMinusButton->show();
    ui->loveLabel->setGeometry(((640 - ui->statusLabel->width())/2)-24, 15, 16, 16);
    ui->loveLabel->show();
    if ( (myDatabase->getLastFmOnline()) && (myLastFm->getIsSessionOK()) )
    {
        // lastFmLoveSongs();
    }

    // DAWID
    // dodać tą piosenkę do ulubionych w bibliotece
}

void MainWindow::addToMusicTable(Phonon::MediaObject *newObject)
{
    int i = ui->musicTable->rowCount(); // i = currentRow
    ui->musicTable->insertRow(i);
    QString tmpId;
    tmpId.setNum(i+1);
    QTableWidgetItem *numberItem = new QTableWidgetItem(tmpId);
    ui->musicTable->setItem(i, 0, numberItem);
    QTableWidgetItem *artistItem = new QTableWidgetItem(newObject->metaData().value("ARTIST"));
    ui->musicTable->setItem(i, 1, artistItem);
    QTableWidgetItem *titleItem = new QTableWidgetItem(newObject->metaData().value("TITLE"));
    ui->musicTable->setItem(i, 2, titleItem);
    QTableWidgetItem *albumItem = new QTableWidgetItem(newObject->metaData().value("ALBUM"));
    ui->musicTable->setItem(i, 3, albumItem);

    qDebug() << "total time is " << newObject->totalTime();

    qint64 totalTime = newObject->totalTime();;
    QTime lengthTime(0, (totalTime / 60000) % 60, (totalTime / 1000) % 60);
    QTableWidgetItem *lengthItem = new QTableWidgetItem(lengthTime.toString("mm:ss"));
    ui->musicTable->setItem(i, 4, lengthItem);

    // DAWID
    // dodać tą piosenkę do naszej biblioteki, jeśli jeszcze jej tam nie ma
    // (najpierw artystę do tabeli artystów, później album do tabeli albumów)
    // (i na końcu piosenkę do tabeli piosenek)
    // a ze zdjęciem albumu coś pokombinuj :D w ostateczności daj jakieś "default.jpg" albo coś :D

}

void MainWindow::myResolverStateChanged(Phonon::State newState, Phonon::State oldState)
{
    if (newState == Phonon::ErrorState) {
        qDebug() << "Error opening file: " << myResolver->errorString();
        // while (!songList.isEmpty() && !(songList.takeLast() == myResolver->currentSource())) {}  /* loop */;
        return;
    }

    if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
        return;

    if (myResolver->currentSource().type() == Phonon::MediaSource::Invalid)
        return;

    qDebug() << "STATECHANGED111";

    addToMusicTable(myResolver);
    qDebug() << "dodano do tabeli : " << myResolver->currentSource().fileName();

    // now we must add next song in songList
    int index = ui->musicTable->rowCount();
    // int index = songList.indexOf(myResolver->currentSource()) + 1;
    qDebug() << "new index is " << index;
    qDebug() << "songlist size is " << songList.size();
    if (songList.size() > index) {
        myResolver->setCurrentSource(songList.at(index));
    }

}

void MainWindow::currentSongStateChanged(Phonon::State newState, Phonon::State oldState)
{
    if (oldState == Phonon::LoadingState) addToLast = false;

    if (newState == Phonon::PlayingState)
    {
        ui->playButton->hide();
        ui->pauseButton->show();
        if (ui->musicTable->rowCount() > *currentIndex) for (int i = 0 ; i <= 4 ; i++)
        {
            ui->musicTable->item(*currentIndex, i)->setFont(myPlayingFont);
        }

        if (ui->musicTable->rowCount() > *previousIndex) for (int i = 0 ; i <= 4 ; i++)
        {
            ui->musicTable->item(*previousIndex, i)->setFont(myNormalFont);
        }

        ui->welcomeLabel->hide();
        ui->elapsedTimeLCD->show();
        ui->remainingTimeLCD->show();
        ui->mySeekSlider->show();
        QString tmpStatus = currentSong->metaData().value("ARTIST") + " - " + currentSong->metaData().value("TITLE");
        ui->statusLabel->setText(tmpStatus);
        ui->statusLabel->adjustSize();
        ui->statusLabel->setGeometry((640 - ui->statusLabel->width())/2, 15, ui->statusLabel->width(), ui->statusLabel->height());

        ui->statusLabel->show();

        if (isCurrentSongFavourite())
        {
            ui->lovePlusButton->hide();
            ui->loveMinusButton->show();
            ui->loveLabel->setGeometry(((640 - ui->statusLabel->width())/2)-24, 15, 16, 16);
            ui->loveLabel->show();
        }
        else
        {
            ui->loveLabel->hide();
            ui->loveMinusButton->hide();
            ui->lovePlusButton->show();
        }

        if (myDatabase->getLyricsOnline())
        {
            getLyrics();
        }
        else
        {
            // DAWID
            // jeśli ta piosenka ma jakieś lyrics zapisane w bibliotece, to wyświetl to
            // a jeśli nie ma, to wyświetl:
            ui->lyrics_textBrowser->setText("Lyrics online mode is currently disabled...\n\nGo to settings to change it...");
        }

        if ( (myDatabase->getLastFmOnline()) && (myLastFm->getIsSessionOK()) )
        {
            // lastFmUpdateNowPlaying();
            myLastFm->updateNowPlaying(currentSong->metaData().value("TITLE"), currentSong->metaData().value("ARTIST"), currentSong->metaData().value("ALBUM"));
        }
    }

    if ( (newState == Phonon::StoppedState) || (newState == Phonon::PausedState) )
    {
        if (newState == Phonon::StoppedState) ui->loveLabel->hide();
        ui->pauseButton->hide();
        ui->playButton->show();
        if ( (newState == Phonon::StoppedState) && (ui->musicTable->rowCount() > *currentIndex) ) for (int i = 0 ; i <= 4 ; i++)
        {
            ui->musicTable->item(*currentIndex, i)->setFont(myNormalFont);
        }

    }

    if (newState == Phonon::StoppedState)
    {
        ui->elapsedTimeLCD->setText("00:00");
        ui->remainingTimeLCD->setText("00:00");
        ui->elapsedTimeLCD->hide();
        ui->remainingTimeLCD->hide();
        ui->mySeekSlider->hide();
        ui->statusLabel->hide();
        ui->welcomeLabel->show();
        ui->loveMinusButton->hide();
        ui->lovePlusButton->hide();
        ui->lyrics_textBrowser->clear();

        // if (addToLast) addCurrentSongToLastFmQueue();
        if (addToLast) myLastFm->addSongToQueue(currentSong->metaData().value("TITLE"), currentSong->metaData().value("ARTIST"), currentSong->metaData().value("ALBUM"));
    }
}

void MainWindow::chooseFiles()
{
    QStringList myFileList = myFileDialog.getOpenFileNames(this, "Wybierz pliki muzyczne", QDesktopServices::storageLocation(QDesktopServices::MusicLocation), "Audio Files (*.mp3 *.ogg *.wma)");
    addFiles(myFileList);
}

void MainWindow::addFiles(QStringList myFileList)
{
    for (int i = 0 ; i < myFileList.size() ; i++)
    {
        Phonon::MediaSource tmp(myFileList[i]);
        qDebug() << "Adding song: " << myFileList[i];
        songList.append(tmp);
    }
    redrawTable();
}

void MainWindow::redrawTable()
{
    clearTable();

    // ustawiamy metaResolver na pierwszy utwór
    if (!songList.isEmpty()) {
        myResolver->setCurrentSource(songList[0]);
    }

    /*timeList.clear();
    for (int i = 0 ; i < songList.size() ; i++)
    {
        myTimeResolver->setCurrentSource(songList[i]);
    }*/

}

void MainWindow::updateTime(qint64 newTotalTime)
{
    timeList << newTotalTime;
    if (timeList.size() == songList.size())
    {
        for (int i = 0 ; i < timeList.size() ; i++)
        {
            qint64 totalTime = timeList[i];
            QTime lengthTime(0, (totalTime / 60000) % 60, (totalTime / 1000) % 60);
            QTableWidgetItem *lengthItem = new QTableWidgetItem(lengthTime.toString("mm:ss"));
            ui->musicTable->setItem(i, 4, lengthItem);
        }
    }
}

bool MainWindow::isCurrentSongFavourite()
{
    int tmpSize = myFavourites[0].size();
    if ( tmpSize > myFavourites[1].size() )
        tmpSize = myFavourites[1].size();
    if ( tmpSize > myFavourites[2].size() )
        tmpSize = myFavourites[2].size();

    for (int i = 0 ; i < tmpSize ; i++)
    {
        if ( (myFavourites[0][i] == currentSong->metaData().value("ARTIST"))
        && (myFavourites[1][i] == currentSong->metaData().value("TITLE"))
        && (myFavourites[2][i] == currentSong->metaData().value("ALBUM")) )
            return true;
    }

    return false;
}

void MainWindow::getLyrics()
{
    QString artist = currentSong->metaData().value("ARTIST");
    QString title = currentSong->metaData().value("TITLE");
    ui->lyrics_textBrowser->setText("Getting lyrics...");
    myLyrics->getLyrics(artist, title);
}


MainWindow::~MainWindow()
{
    delete ui;
}


/*

void MainWindow::lastFmAddReplyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString content(data);

    ui->lastFm_textBrowser->setText(content);

    if (content.contains("status=\"ok\""))
    {
        // QRegExp myRegExp(".+<key>");
        // content.remove(myRegExp);
        // myRegExp = QRegExp("</key>.+");
        // content.remove(myRegExp);

    }
    else displayWarning("There was an error sending \"Now Playing\" to Last.fm");
}

void MainWindow::lastFmScrobbleReplyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString content(data);
    int oldSize = myLastQueue[0].size();
    int newSize;

    ui->lastFm_textBrowser->setText(content);

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
                QString tmpS;
                tmpS.setNum(lastFmSent);
                ui->lastFm_sentLabel->setText("Sent : "+tmpS);
                lastFmQueued = lastFmQueued - (oldSize - newSize);
                tmpS.setNum(lastFmQueued);
                ui->lastFm_queuedLabel->setText("Queued : "+tmpS);

                lastFmScrobbleSongs();
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
                if ( (currentSong->state() == Phonon::PlayingState) || (currentSong->state() == Phonon::PausedState) )
                    lastFmUpdateNowPlaying();
                newSize = myLastQueue[0].size();
                lastFmSent = lastFmSent + (oldSize - newSize);
                QString tmpS;
                tmpS.setNum(lastFmSent);
                ui->lastFm_sentLabel->setText("Sent : "+tmpS);
                lastFmQueued = lastFmQueued - (oldSize - newSize);
                tmpS.setNum(lastFmQueued);
                ui->lastFm_queuedLabel->setText("Queued : "+tmpS);
            }

    }
    else displayWarning("There was an error sending track(s) to Last.fm");
}

void MainWindow::lastFmScrobbleSongs()
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
    lastFmScrobbleManager->post(QNetworkRequest(QUrl(lastFmServer)), postData);

}

void MainWindow::addCurrentSongToLastFmQueue()
{
    // myLastQueue[0] << currentSong->metaData().value("ARTIST");
    // myLastQueue[1] << currentSong->metaData().value("TITLE");
    // myLastQueue[2] << currentSong->metaData().value("ALBUM");
    // QDateTime tmpTime = QDateTime::currentDateTime();
    // QString tmpS;
    // qint64 tmpI;
    // tmpI = tmpTime.toTime_t();
    // tmpS.setNum(tmpI);
    // myLastQueue[3] << tmpS;
    // lastFmQueued = myLastQueue[0].size();
    // tmpS.setNum(lastFmQueued);
    // ui->lastFm_queuedLabel->setText("Queued : "+tmpS);
    // if ( (myDatabase->getLastFmOnline()) && (myLastFm->getIsSessionOK()) ) lastFmScrobbleSongs();

    myLastFm->addSongToQueue(currentSong->metaData().value("TITLE"), currentSong->metaData().value("ARTIST"), currentSong->metaData().value("ALBUM"));
}

void MainWindow::lastFmLoveSongs()
{
    QString tmpSig, tmpMd5Sig;
    QByteArray tmpByteArray;
    QByteArray postData;

    postData.append("method=track.love&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("sk="+lastFmSessionKey+"&");

    QString artist, title;
    artist = myLastFavQueue[0][0];
    title = myLastFavQueue[1][0];
    postData.append("artist="+artist+"&");
    postData.append("track="+title+"&");

    tmpSig = "api_key" + lastFmApiKey + "artist" + artist + "methodtrack.love" + "sk" + lastFmSessionKey + "track" + title + lastFmApiSecret;
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    postData.append("api_sig="+tmpMd5Sig);
    lastFmLoveManager->post(QNetworkRequest(QUrl(lastFmServer)), postData);

}

void MainWindow::lastFmLoveReplyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString content(data);

    ui->lastFm_textBrowser->setText(content);

    if (content.contains("status=\"ok\""))
    {
        myLastFavQueue[0].removeAt(0);
        myLastFavQueue[1].removeAt(0);
        myLastFavQueue[2].removeAt(0);

        if (!myLastFavQueue[0].isEmpty())
        {
            lastFmLoveSongs();
        }
    }
    else displayWarning("There was an error sending \"Love Song\" to Last.fm");
}

void MainWindow::lastFmUnloveSongs()
{
    QString tmpSig, tmpMd5Sig;
    QByteArray tmpByteArray;
    QByteArray postData;

    postData.append("method=track.unlove&");
    postData.append("api_key="+lastFmApiKey+"&");
    postData.append("sk="+lastFmSessionKey+"&");

    QString artist, title;
    artist = myLastUnfavQueue[0][0];
    title = myLastUnfavQueue[1][0];

    tmpSig = "api_key" + lastFmApiKey + "artist" + artist + "methodtrack.unlove" + "sk" + lastFmSessionKey + "track" + title + lastFmApiSecret;
    tmpByteArray.append(tmpSig);
    tmpMd5Sig = QCryptographicHash::hash(tmpByteArray, QCryptographicHash::Md5).toHex();
    postData.append("api_sig="+tmpMd5Sig);
    lastFmUnloveManager->post(QNetworkRequest(QUrl(lastFmServer)), postData);

}

void MainWindow::lastFmUnloveReplyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString content(data);

    ui->lastFm_textBrowser->setText(content);

    if (content.contains("status=\"ok\""))
    {
        myLastUnfavQueue[0].removeAt(0);
        myLastUnfavQueue[1].removeAt(0);
        myLastUnfavQueue[2].removeAt(0);

        if (!myLastUnfavQueue[0].isEmpty())
        {
            lastFmUnloveSongs();
        }
    }
    else displayWarning("There was an error sending \"Unlove Song\" to Last.fm");
}
*/
QString MainWindow::makeItSafe(QString oldString)
{
    oldString.replace("&", "and");
    return oldString;
}

void MainWindow::lastQueueChanged(int lastFmQueued)
{
    QString tmpS;
    tmpS.setNum(lastFmQueued);
    ui->lastFm_queuedLabel->setText("Queued : "+tmpS);
}

void MainWindow::lastSentChanged(int lastFmSent)
{
    QString tmpS;
    tmpS.setNum(lastFmSent);
    ui->lastFm_sentLabel->setText("Sent : "+tmpS);
}

void MainWindow::ifSongIsPlayingUpdateNowPlaying()
{
    if ( (currentSong->state() == Phonon::PlayingState) || (currentSong->state() == Phonon::PausedState) )
        myLastFm->updateNowPlaying(currentSong->metaData().value("TITLE"), currentSong->metaData().value("ARTIST"), currentSong->metaData().value("ALBUM"));
}

void MainWindow::lyricsChanged(QString newLyrics)
{
    // ui->lyrics_textBrowser->clear();
    ui->lyrics_textBrowser->setText(newLyrics);

    // DAWID
    // jeśli newLyrics != "There was some error with lyrics, sorry..."
    // oraz jeśli newLyrics != "Lyrics not found..."
    // to dodaj (albo update'uj) lyrics dla tej piosenki w naszej bibliotece
    // nawet jeśli już tam jakiś lyrics jest, to update'uj
    // ...
    // a jeśli newLyrics jest równe któremuś z tych powyższych tekstów, to zobacz, może
    // w bibliotece jest jakiś tekst dla tego utworu
    // i jak jest, to go wyświetl
    // pozdro 600
}
