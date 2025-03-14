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

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

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

// //Mask
// private:
//     QWidget* widget_Mask;
//     void initMask();
//     void openMask();
//     void closeMask();


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
    float detailPicScale;
    void initPagePlayingUI();


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


//Page_Statistics


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
