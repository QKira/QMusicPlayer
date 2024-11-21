#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QMutex>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractButton>
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QEvent>
#include <QTimer>
#include <QFontDatabase>
#include <QtSvg/QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <dwmapi.h>
#include <windowsx.h>
#include <qt_windows.h>

class MainFrame : public QWidget
{
    Q_OBJECT
public:
    enum Widget {
        Label_Icon = 0,
        Btn_MenuMin = 1,
        Btn_MenuMax = 2,
        Btn_MenuClose = 3
    };

public:
    MainFrame(QWidget *parent = nullptr);
    ~MainFrame();

    static void setFormInCenter(QWidget *frm);

    static void getQssColor(const QString &qss,
                            QString &textColor,
                            QString &panelColor,
                            QString &borderColor,
                            QString &normalColorStart,
                            QString &normalColorEnd,
                            QString &darkColorStart,
                            QString &darkColorEnd,
                            QString &highColor);

    static void showMessageBoxInfo(const QString &info, int closeSec = 0);

    static void showMessageBoxError(const QString &info, int closeSec = 0);

    static int showMessageBoxQuestion(const QString &info);

    static QString showInputBox(bool &ok, const QString &title, int type = 0, int closeSec = 0,
                                QString defaultValue = QString(), bool pwd = false);

protected:
    bool eventFilter(QObject *obj, QEvent *evt) override;

private:
    QVBoxLayout *vbL_MainFrame;
        QWidget *widget_Main;
        QVBoxLayout *vbL_Main;
            QWidget *widget_Title;
            QHBoxLayout *hbL_Title;
                QLabel *label_Icon;
                QLabel *label_Title;
                QWidget *widget_Menu;
                QHBoxLayout *hbL_Menu;
                    QPushButton *btn_MenuMin;
                    QPushButton *btn_MenuMax;
                    QPushButton *btn_MenuClose;
            QWidget *widget_Central;
            QVBoxLayout *vbL_Central;

private:
    bool isMaximized;
    QRect location;
    QWidget *centralWidget;

    QString title;
    Qt::Alignment alignment;

public:
    QLabel *getLabIco()             const;
    QLabel *getLabTitle()           const;
    QPushButton *getBtnMenuMin()    const;
    QPushButton *getBtnMenuMax()    const;
    QPushButton *getBtnMenuMClose() const;

    QString getTitle()              const;
    Qt::Alignment getAlignment()    const;

    QSize sizeHint()       const override;
    QSize minimumSizeHint()const override;

private:
    void initControl();
    void initForm();

private Q_SLOTS:
    void handleBtnMenuMinClick();
    void handleBtnMenuMaxClick();
    void handleBtnMenuCloseClick();

public:
    //设置部件图标
    void setIcon(MainFrame::Widget widget, QChar str, quint32 size = 9);
    //设置部件图片
    void setPixmap(MainFrame::Widget widget, const QString &file, const QSize &size = QSize(16, 16));
    //设置部件是否可见
    void setVisible(MainFrame::Widget widget, bool visible = true);
    //设置只有关闭按钮
    void setOnlyCloseBtn();

    //设置标题栏高度
    void setTitleHeight(int height);
    //设置按钮统一宽度
    void setBtnWidth(int width);

    //设置标题及文本样式
    void setTitle(const QString &title);
    void setAlignment(Qt::Alignment alignment);

    //设置主窗体
    void setCentralWidget(QWidget *widget);
};

class QMFMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit QMFMessageBox(QWidget *parent = 0);
    ~QMFMessageBox();

    static QMFMessageBox *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new QMFMessageBox;
            }
        }

        return self;
    }

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static QMFMessageBox *self;

    QVBoxLayout *vbL_QMFMessageBox;
        QWidget *widget_Title;
        QHBoxLayout *hbL_Title;
            QLabel *label_Icon;
            QLabel *label_Title;
            QLabel *label_Time;
            QWidget *widget_Menu;
            QHBoxLayout *hbL_Menu;
                QPushButton *btn_MenuClose;
        QWidget *widget_Main;
        QVBoxLayout *vbL_Main;
            QFrame *frame;
            QVBoxLayout *vbl_Frame;
                QHBoxLayout *hbL_Inner1;
                    QLabel *label_IconMain;
                    QSpacerItem *hSpacer_1;
                    QLabel *label_Info;
                QHBoxLayout *hbL_Inner2;
                    QSpacerItem *hSpacer_2;
                    QPushButton *btn_Ok;
                    QPushButton *btn_Cancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void handleBtnOkClick();
    void handleBtnMenuCloseClick();
    void handleBtnCancelClick();//

public:
    void setMessage(const QString &msg, int type, int closeSec = 0);
};



class QMFInputBox : public QDialog
{
    Q_OBJECT

public:
    explicit QMFInputBox(QWidget *parent = 0);
    ~QMFInputBox();

    static QMFInputBox *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new QMFInputBox;
            }
        }

        return self;
    }

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static QMFInputBox *self;

    QVBoxLayout *vbL_QMFInputBox;
    QWidget *widget_Title;
    QHBoxLayout *hbL_Title;
    QLabel *label_Icon;
    QLabel *label_Title;
    QLabel *label_Time;
    QWidget *widget_Menu;
    QHBoxLayout *hbL_Menu;
    QPushButton *btn_MenuClose;
    QWidget *widget_Main;
    QVBoxLayout *vbL_Main;
    QFrame *frame;
    QVBoxLayout *vbL_Frame;
    QLabel *label_Info;
    QLineEdit *txtValue;
    QComboBox *cboxValue;
    QHBoxLayout *hbL_Inner;
    QSpacerItem *hSpacer;
    QPushButton *btn_Ok;
    QPushButton *btn_Cancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间
    QString value;                  //当前值

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void handleBtnOkClick();
    void handleBtnCancelClick();

public:
    void setParameter(const QString &title, int type = 0, int closeSec = 0,
                      QString defaultValue = QString(), bool pwd = false);

    QString getValue()const;

};



class IconHelper : public QObject
{
public:
    explicit IconHelper(QObject *parent = 0);
    static IconHelper *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new IconHelper;
            }
        }

        return self;
    }

    void setIcon(QLabel *lab, QChar c, quint32 size);
    void setIcon(QAbstractButton *btn, QChar c, quint32 size);

private:
    static IconHelper *self;
    QFont iconFont;
};
#endif // MAINFRAME_H
