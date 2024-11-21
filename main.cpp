#include "mainframe.h"
#include "appinit.h"
#include "centralwidget.h"
// #include <QApplication>
#include <qtsingleapplication.h>
#include <QMessageBox>
#include <QFile>

int main(int argc, char *argv[])
{
    QtSingleApplication a("QMusicPlayer", argc, argv);
    // QApplication a(argc, argv);

    if(a.isRunning())
    {
        //        MainFrame::showMessageBoxInfo("该程序正在运行中！");//无法使用，原因暂时未知。
        QMessageBox::warning(NULL, "提示", "该程序正在运行中！");

        return EXIT_SUCCESS;
    }

    QFile qssFile(":/stylesheets/res/stylesheets/silvery.css");
    qssFile.open(QIODevice::ReadOnly);
    QString qss = qssFile.readAll();
    qApp->setStyleSheet(qss);

    AppInit::Instance()->start();

    MainFrame w;
    CentralWidget* centralWidget = new CentralWidget;
    w.setCentralWidget(centralWidget);
    w.show();

    return a.exec();
}
