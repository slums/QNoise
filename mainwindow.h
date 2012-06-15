#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cdatabase.h"
#include "clastfm.h"
#include "clyrics.h"
#include "clibrary.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFont>
#include <QList>
#include <QStringList>
#include <QDesktopServices>
#include <QTime>
#include <QTextCodec>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QRegExp>
#include <QByteArray>
#include <QDateEdit>
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <QCryptographicHash>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QDebug>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(QWidget *parent = 0);
    MainWindow();
    ~MainWindow();
    void createTable();

private:
    QFileDialog myFileDialog;
    Phonon::MediaObject *myResolver;
    Phonon::MediaObject *myTimeResolver;
    Phonon::MediaObject *currentSong;
    Phonon::AudioOutput *audioOutput;

    int *currentIndex;
    int *previousIndex;

    QList<Phonon::MediaSource> songList;
    QList<qint64> timeList;
    QStringList myFavourites[3];
    QStringList myLastQueue[4]; // artist, track, album, timestamp
    QStringList myLastFavQueue[3];
    QStringList myLastUnfavQueue[3];

    bool addToLast;
    QString lastFmServer, lastFmUser, lastFmPassword, lastFmApiKey, lastFmApiSecret, lastFmSessionKey;
    int lastFmSent, lastFmQueued;
    QFont myPlayingFont, myNormalFont;

    Ui::MainWindow *ui;

    CDatabase *myDatabase;
    CLastFm *myLastFm;
    CLyrics *myLyrics;
    CLibrary *myLibrary;

public:
    bool isCurrentSongFavourite();

public slots:
    void makeConnections();
    void createPointers();
    void getConfiguration();
    void clearTable();
    void clearSongList();
    void chooseFiles();
    void addFiles(QStringList);
    void addToMusicTable(Phonon::MediaObject*);
    void myResolverStateChanged(Phonon::State, Phonon::State);
    void currentSongStateChanged(Phonon::State,Phonon::State);
    void startPlaying(int, int);
    void playButtonPressed();
    void pauseButtonPressed();
    void previousButtonPressed();
    void nextButtonPressed();
    void lovePlusButtonPressed();
    void loveMinusButtonPressed();
    void redrawTable();
    void updateTime(qint64);
    void tick(qint64 time);
    void getLyrics();
    void updateLastFm();
    void updateButtonClicked();
    void checkLastToggled(bool);
    void checkLyricsToggled(bool);
    void checkRepeatToggled(bool);
    void settingsLastEditingFinished();
    void displayWarning(QString);
    QString makeItSafe(QString);
    void lastReply(QString);
    void lastQueueChanged(int);
    void lastSentChanged(int);
    void lyricsChanged(QString);
    void ifSongIsPlayingUpdateNowPlaying();
};

#endif // MAINWINDOW_H
