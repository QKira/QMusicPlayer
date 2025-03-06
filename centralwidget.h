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


//Page_Statistics


//Page_Settings
private:
    QTableView* tableView_PathMonitor;
    QStandardItemModel standardItemModel_PathMonitor;
    QPushButton button_PathMonitor_AddPath;
    QPushButton button_PathMonitor_DeletePath;
    void initPageSettings();
private slots:
    void handleSettingsPathMonitorButtonClick();


protected:
    void paintEvent(QPaintEvent* event) override;


};

#endif // CENTRALWIDGET_H
