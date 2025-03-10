#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>
#include <array>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <QStandardItemModel>
#include <QFileDialog>
#include "databasemanager.h"
#include <QSqlQuery>
#include <QDirIterator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QProcess>
#include <QVector>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
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
    QList<QPushButton *> playlistsNaviButtonPtrs;
    void updatePlaylistsNaviButtonPtrs();
    std::array<QPushButton *, 100> vct_ButtonPtrs;
    void testAddButton();
    void initWidgetPlaylistsBottomLeftUI();
private slots:
    void handlePlaylistsNaviButtonClick();

//Page_Library
private:
    QTableView* tableView_Library;
    QStandardItemModel standardItemModel_Library;
    // QVector<QProcess*> processList;
    void initPageLibraryUI();
    void updateLibraryModelData();

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
private slots:
    void handleSettingsPathMonitorButtonClick();
    void handleTableViewPathMonitorChanged();

protected:
    void paintEvent(QPaintEvent* event) override;

};

#endif // CENTRALWIDGET_H
