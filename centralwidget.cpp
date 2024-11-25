#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CentralWidget)
{
    ui->setupUi(this);
    this->setProperty("form", true);

    QList<QPushButton *> naviBtns = ui->widget_Left->findChildren<QPushButton *>();
    foreach (QPushButton *naviBtn, naviBtns) {
        if(naviBtn->objectName() == "btn_Playing") {
            naviBtn->setChecked(true);
        }
        connect(naviBtn, &QPushButton::clicked, this, &CentralWidget::handleNaviButtonClick);
    }
    ui->stackedWidget->setCurrentIndex(0);
}

CentralWidget::~CentralWidget()
{
    delete ui;
}

void CentralWidget::handleNaviButtonClick()
{
    QList<QPushButton *> naviBtns = ui->widget_Left->findChildren<QPushButton *>();

    QPushButton *clickedBtn = (QPushButton *)sender();

    foreach (QPushButton *naviBtn, naviBtns) {
        if(naviBtn == clickedBtn) {
            naviBtn->setChecked(true);
        } else {
            naviBtn->setChecked(false);
        }
    }

    if(clickedBtn->objectName() == "btn_Playing")
        ui->stackedWidget->setCurrentIndex(0);
    else if(clickedBtn->objectName() == "btn_Playlists")
        ui->stackedWidget->setCurrentIndex(1);
    else if(clickedBtn->objectName() == "btn_Library")
        ui->stackedWidget->setCurrentIndex(2);
    else if(clickedBtn->objectName() == "btn_Settings")
        ui->stackedWidget->setCurrentIndex(3);
}
