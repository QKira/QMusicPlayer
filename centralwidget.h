#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget
{
    Q_OBJECT

private:
    enum BTN_NAVI{
        BTN_NAVI_LIBRARY = 0,
        BTN_NAVI_PLAYLISTS = 1,
        BTN_NAVI_PLAYING = 2,
        BTN_NAVI_SETTINGS = 3,
    };

public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

private:
    Ui::CentralWidget *ui;

private:
    void initForm();

private slots:
    void handleNaviButtonClick();
};

#endif // CENTRALWIDGET_H
