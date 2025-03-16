#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QFileDialog>
#include "databasemanager.h"
#include <QSqlQuery>
#include <QDirIterator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QVector>
#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QMenu>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRandomGenerator>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

struct SongInfo {
    qint64 song_id;
    QString song_title;
    QString song_artist;
    QString song_album;
    QString song_lyrics;
    qint64 song_duration;
    qint64 song_size;
    QString cover_path;
    QString song_path;
};

struct LyricLine {
    qint64 time;
    QString text;
};

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();
private:
    Ui::CentralWidget *ui;


//Database
private:
    DatabaseManager dbm;
    QSqlQuery qry;
    QString sql;


//widget_Left
private:
    QList<QPushButton *> naviButtonPtrs;
    void updateNaviButtonPtrs();
    void initWidgetleftUI();
private slots:
    void handleNaviButtonClick();


//Page_Playing
private:
    QMediaPlayer player;
    QList<SongInfo> currentPlaylist;
    int currentIndex = 0;
    int prevIndex = 0;
    int nextIndex = 0;
    int playMode = 1;
    bool playStatus = false;
    float detailPicScale;
    QList<LyricLine> currentLyrics;
    void initPagePlayingUI();
    void updatecurrentPlaylist();
    void playSong();
    void pauseSong();
    void playPrev();
    void playNext();
    QList<LyricLine> lyricsParser(const QString &lyrics);
    void updateLyrics(qint64 position);
    void setLyricHighlight(int index);
    void loadLyrics(const QString &lyricText);

private slots:
    void handleRadioButtonPlayModeClick();
    void handleButtonPlayingPlayClick();
    void handleButtonPlayingPlayPrevClick();
    void handleButtonPlayingPlayNextClick();
    void handleStateChanged(QMediaPlayer::PlaybackState state);
    void handleSourceChanged(const QUrl &media);
    void handleDurationChanged(qint64 duration);
    void handlePositionChanged(qint64 position);
    void handleMetaDataChanged();


//Page_Playlists
private:
    QTableView* tableView_Playlists;
    QStandardItemModel standardItemModel_Playlists;
    void initPagePlaylistsUI();
    void updatePlaylistsNaviButton();
    void updatePlaylistsModelData();
private slots:
    void handlePlaylistsNaviButtonClick();
    void playlistsNaviButtonMenu(const QPoint &pos);
    void playlistsTableViewMenu(const QPoint &pos);


//Page_Library
private:
    QTableView* tableView_Library;
    QStandardItemModel standardItemModel_Library;
    void initPageLibraryUI();
    void updateLibraryModelData();
private slots:
    void on_lineEdit_Search_textChanged(const QString &arg1);
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void handleButtonLibraryNewPLClick();
    void handleButtonLibraryAdd2PLClick();
    void handleButtonLibraryDelSongClick();
    void handleSliderVolumeChanged(int value);
    void handleSliderProgressChanged(int value);


//Page_Statistics
private:
    QTableView* tableView_StatisticsDay;
    QStandardItemModel standardItemModel_StatisticsDay;
    QTableView* tableView_StatisticsWeek;
    QStandardItemModel standardItemModel_StatisticsWeek;
    QTableView* tableView_StatisticsMonth;
    QStandardItemModel standardItemModel_StatisticsMonth;
    void initPageStatisticsUI();
    void updateStatisticsModelData();
private slots:
    void handleStatisticsButtonClick();


//Page_Settings
private:
    QTableView* tableView_PathMonitor;
    QStandardItemModel standardItemModel_PathMonitor;
    QPushButton button_PathMonitor_AddPath;
    QPushButton button_PathMonitor_DeletePath;
    void initPageSettingsUI();
    void updatePathMonitorModelData();
    void scanDirectory(QString directory);
    bool extractCover(const QString &audioFilePath, const QString &outputPath);
private slots:
    void handleSettingsPathMonitorButtonClick();
    void handleTableViewPathMonitorChanged();


protected:
    void paintEvent(QPaintEvent* event) override;

};

#endif // CENTRALWIDGET_H
