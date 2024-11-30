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

    initPagePlayingUI();

    // QList<QObject *> objs = ui->page_Playing->findChildren<QObject *>();
    // foreach (QObject *obj, objs) {
    //     qDebug() << obj->objectName();
    // }
}

CentralWidget::~CentralWidget()
{
    delete ui;
}

//Page_Playing
void CentralWidget::initPagePlayingUI()
{
    QPixmap pixmap{":/images/res/images/preview.jpg"};
    pixmap.scaled(ui->label_Playing_DetailPic->size(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_Playing_DetailPic->setPixmap(pixmap);
    ui->label_Playing_DetailPic->setScaledContents(true);
    int imgW = pixmap.width();
    int imgH = pixmap.height();
    detailPicScale = imgW / imgH;
    ui->label_Playing_DetailPic->resize(imgW, imgH);
    ui->label_Playing_DetailPic->move(0, 0);
}


void CentralWidget::paintEvent(QPaintEvent *event)
{
//Page_Playing
    if (ui->widget_Playing_DetailBlock->width() > ui->widget_Playing_DetailBlock->height())
    {
        float fScaleH = ui->widget_Playing_DetailBlock->height();
        float fScaleW = fScaleH * detailPicScale;
        ui->label_Playing_DetailPic->resize(fScaleW, fScaleH);
        ui->label_Playing_DetailPic->move(0, 0);
    }
    else
    {
        float fScaleW = ui->widget_Playing_DetailBlock->width();
        float fScaleH = fScaleW / detailPicScale;
        ui->label_Playing_DetailPic->resize(fScaleW, fScaleH);
        ui->label_Playing_DetailPic->move(0, 0);
    }
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
