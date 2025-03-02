#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>
#include <array>
#include <qlabel.h>
#include <qpushbutton.h>

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
    void refreshNaviButtonPtrs();
    void initWidgetleftUI();

//Page_Playing
private:
    float detailPicScale;
    void initPagePlayingUI();

//Page_Playlists
private:
    std::array<QPushButton *, 100> vct_ButtonPtrs;
    void testAddButton();
    QList<QPushButton *> playlistsNaviButtonPtrs;
    void refreshPlaylistsNaviButtonPtrs();
    void initWidgetPlaylistsBottomLeftUI();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void handleNaviButtonClick();
    void handlePlaylistsNaviButtonClick();
};

#endif // CENTRALWIDGET_H
