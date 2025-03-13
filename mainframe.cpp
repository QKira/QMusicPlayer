#include "mainframe.h"

MainFrame::MainFrame(QWidget *parent)
    : QWidget(parent)
{
    this->initControl();
    this->initForm();
}

MainFrame::~MainFrame()
{
    delete widget_Main;
}

void MainFrame::setFormInCenter(QWidget *frm)
{
    int frmX = frm->width();
    int frmY = frm->height();
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    int deskWidth = screenRect.width();
    int deskHeight = screenRect.height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    frm->move(movePoint);
}

void MainFrame::getQssColor(const QString &qss, QString &textColor, QString &panelColor, QString &borderColor, QString &normalColorStart, QString &normalColorEnd, QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QString str = qss;

    QString flagTextColor = "TextColor:";
    int indexTextColor = str.indexOf(flagTextColor);
    if(indexTextColor >= 0)
        textColor = str.mid(indexTextColor + flagTextColor.length(), 7);

    QString flagPanelColor = "PanelColor:";
    int indexPanelColor = str.indexOf(flagPanelColor);
    if(indexPanelColor >= 0)
        panelColor = str.mid(indexPanelColor + flagPanelColor.length(), 7);

    QString flagBorderColor = "BorderColor:";
    int indexBorderColor = str.indexOf(flagBorderColor);
    if(indexBorderColor >= 0)
        borderColor = str.mid(indexBorderColor + flagBorderColor.length(), 7);

    QString flagNormalColorStart = "NormalColorStart:";
    int indexNormalColorStart = str.indexOf(flagNormalColorStart);
    if(indexNormalColorStart >= 0)
        normalColorStart = str.mid(indexNormalColorStart + flagNormalColorStart.length(), 7);

    QString flagNormalColorEnd = "NormalColorEnd:";
    int indexNormalColorEnd = str.indexOf(flagNormalColorEnd);
    if(indexNormalColorEnd >= 0)
        normalColorEnd = str.mid(indexNormalColorEnd + flagNormalColorEnd.length(), 7);

    QString flagDarkColorStart = "DarkColorStart:";
    int indexDarkColorStart = str.indexOf(flagDarkColorStart);
    if(indexDarkColorStart >= 0)
        darkColorStart = str.mid(indexDarkColorStart + flagDarkColorStart.length(), 7);

    QString flagDarkColorEnd = "DarkColorEnd:";
    int indexDarkColorEnd = str.indexOf(flagDarkColorEnd);
    if(indexDarkColorEnd >= 0)
        darkColorEnd = str.mid(indexDarkColorEnd + flagDarkColorEnd.length(), 7);

    QString flagHighColor = "HighColor:";
    int indexHighColor = str.indexOf(flagHighColor);
    if(indexHighColor >= 0)
        highColor = str.mid(indexHighColor + flagHighColor.length(), 7);
}

void MainFrame::showMessageBoxInfo(const QString &info)
{
    QMFMessageBox::Instance()->setMessage(info, 0);
    QMFMessageBox::Instance()->show();
}

void MainFrame::showMessageBoxError(const QString &info)
{
    QMFMessageBox::Instance()->setMessage(info, 2);
    QMFMessageBox::Instance()->show();
}

int MainFrame::showMessageBoxQuestion(const QString &info)
{
    QMFMessageBox msg;
    msg.setMessage(info, 1);
    return msg.exec();
}

QString MainFrame::showInputBox(bool &ok, const QString &title, int type, QString defaultValue, bool pwd)
{
    QMFInputBox input;
    input.setParameter(title, type, defaultValue, pwd);
    ok = input.exec();
    return input.getValue();
}

bool MainFrame::eventFilter(QObject *obj, QEvent *evt)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    if (evt->type() == QEvent::WindowStateChange)
    {
        //打印窗口状态
        // qDebug()<< this->windowState();
    }

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (event->button() == Qt::LeftButton)
        {
            mousePressed = true;
            mousePoint = event->globalPosition().toPoint() - this->pos();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mousePressed = false;
        return true;
    }
    else if (event->type() == QEvent::MouseMove && obj == this->widget_Title)
    {
        if (!isMaximized && mousePressed && (event->buttons() & Qt::LeftButton))
        {
            this->move(event->globalPosition().toPoint() - mousePoint);
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        //以下写法可以将双击识别限定在标题栏
        if (this->btn_MenuMax->isVisible() && obj == this->widget_Title)
        {
            //        此写法双击识别在整个框架内都有效
            //        if (this->btn_MenuMax->isVisible())
            //        {
            this->btn_MenuMax->click();
            return true;
        }
    }
    else if (event->type() == QEvent::Hide)
    {
        if (obj == widget_Main)
        {
            this->hide();
        }
    }

    return QWidget::eventFilter(obj, evt);
}

QLabel *MainFrame::getLabIco() const
{
    return this->label_Icon;
}

QLabel *MainFrame::getLabTitle() const
{
    return this->label_Title;
}

QPushButton *MainFrame::getBtnMenuMin() const
{
    return this->btn_MenuMin;
}

QPushButton *MainFrame::getBtnMenuMax() const
{
    return this->btn_MenuMax;
}

QPushButton *MainFrame::getBtnMenuClose() const
{
    return this->btn_MenuClose;
}

QString MainFrame::getTitle() const
{
    return this->title;
}

Qt::Alignment MainFrame::getAlignment() const
{
    return this->alignment;
}

QSize MainFrame::sizeHint() const
{
    return QSize(600, 450);
}

QSize MainFrame::minimumSizeHint() const
{
    return QSize(200, 150);
}

void MainFrame::initControl()
{
    this->setObjectName(QString::fromUtf8("MainFrame"));
    this->resize(900, 750);
    vbL_MainFrame = new QVBoxLayout(this);
    vbL_MainFrame->setSpacing(0);
    vbL_MainFrame->setObjectName(QString::fromUtf8("vbL_MainFrame"));
    vbL_MainFrame->setContentsMargins(1, 1, 1, 1);
    widget_Main = new QWidget(this);
    widget_Main->setObjectName(QString::fromUtf8("widget_Main"));
    widget_Main->setStyleSheet(QString::fromUtf8(""));
    vbL_Main = new QVBoxLayout(widget_Main);
    vbL_Main->setSpacing(0);
    vbL_Main->setObjectName(QString::fromUtf8("vbL_Main"));
    vbL_Main->setContentsMargins(0, 0, 0, 0);
    widget_Title = new QWidget(widget_Main);
    widget_Title->setObjectName(QString::fromUtf8("widget_Title"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widget_Title->sizePolicy().hasHeightForWidth());
    widget_Title->setSizePolicy(sizePolicy);
    widget_Title->setMinimumSize(QSize(0, 30));
    hbL_Title = new QHBoxLayout(widget_Title);
    hbL_Title->setSpacing(0);
    hbL_Title->setObjectName(QString::fromUtf8("hbL_Title"));
    hbL_Title->setContentsMargins(0, 0, 0, 0);
    label_Icon = new QLabel(widget_Title);
    label_Icon->setObjectName(QString::fromUtf8("label_Icon"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_Icon->sizePolicy().hasHeightForWidth());
    label_Icon->setSizePolicy(sizePolicy1);
    label_Icon->setMinimumSize(QSize(30, 0));
    label_Icon->setAlignment(Qt::AlignCenter);

    hbL_Title->addWidget(label_Icon);

    label_Title = new QLabel(widget_Title);
    label_Title->setObjectName(QString::fromUtf8("label_Title"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label_Title->sizePolicy().hasHeightForWidth());
    label_Title->setSizePolicy(sizePolicy2);
    label_Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    hbL_Title->addWidget(label_Title);

    widget_Menu = new QWidget(widget_Title);
    widget_Menu->setObjectName(QString::fromUtf8("widget_Menu"));
    sizePolicy1.setHeightForWidth(widget_Menu->sizePolicy().hasHeightForWidth());
    widget_Menu->setSizePolicy(sizePolicy1);
    hbL_Menu = new QHBoxLayout(widget_Menu);
    hbL_Menu->setSpacing(0);
    hbL_Menu->setObjectName(QString::fromUtf8("hbL_Menu"));
    hbL_Menu->setContentsMargins(0, 0, 0, 0);

    QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);

    btn_MenuMin = new QPushButton(widget_Menu);
    btn_MenuMin->setObjectName(QString::fromUtf8("btn_MenuMin"));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(btn_MenuMin->sizePolicy().hasHeightForWidth());
    btn_MenuMin->setSizePolicy(sizePolicy4);
    btn_MenuMin->setMinimumSize(QSize(30, 0));
    btn_MenuMin->setMaximumSize(QSize(30, 16777215));
    btn_MenuMin->setCursor(QCursor(Qt::ArrowCursor));
    btn_MenuMin->setFocusPolicy(Qt::NoFocus);

    hbL_Menu->addWidget(btn_MenuMin);

    btn_MenuMax = new QPushButton(widget_Menu);
    btn_MenuMax->setObjectName(QString::fromUtf8("btn_MenuMax"));
    sizePolicy3.setHeightForWidth(btn_MenuMax->sizePolicy().hasHeightForWidth());
    btn_MenuMax->setSizePolicy(sizePolicy3);
    btn_MenuMax->setMinimumSize(QSize(30, 0));
    btn_MenuMax->setMaximumSize(QSize(30, 16777215));
    btn_MenuMax->setCursor(QCursor(Qt::ArrowCursor));
    btn_MenuMax->setFocusPolicy(Qt::NoFocus);

    hbL_Menu->addWidget(btn_MenuMax);

    btn_MenuClose = new QPushButton(widget_Menu);
    btn_MenuClose->setObjectName(QString::fromUtf8("btn_MenuClose"));
    sizePolicy3.setHeightForWidth(btn_MenuClose->sizePolicy().hasHeightForWidth());
    btn_MenuClose->setSizePolicy(sizePolicy3);
    btn_MenuClose->setMinimumSize(QSize(30, 0));
    btn_MenuClose->setMaximumSize(QSize(30, 16777215));
    btn_MenuClose->setCursor(QCursor(Qt::ArrowCursor));
    btn_MenuClose->setFocusPolicy(Qt::NoFocus);

    hbL_Menu->addWidget(btn_MenuClose);
    hbL_Title->addWidget(widget_Menu);
    vbL_Main->addWidget(widget_Title);

    widget_Central = new QWidget(widget_Main);
    widget_Central->setObjectName(QString::fromUtf8("widget_Central"));
    vbL_Central = new QVBoxLayout(widget_Central);
    vbL_Central->setSpacing(0);
    vbL_Central->setObjectName(QString::fromUtf8("vbL_Central"));
    vbL_Central->setContentsMargins(0, 0, 0, 0);

    vbL_Main->addWidget(widget_Central);
    vbL_MainFrame->addWidget(widget_Main);

    connect(this->btn_MenuMin, SIGNAL(clicked(bool)), this, SLOT(handleBtnMenuMinClick()));
    connect(this->btn_MenuMax, SIGNAL(clicked(bool)), this, SLOT(handleBtnMenuMaxClick()));
    connect(this->btn_MenuClose, SIGNAL(clicked(bool)), this, SLOT(handleBtnMenuCloseClick()));
}

void MainFrame::initForm()
{
    setIcon(MainFrame::Label_Icon, (QChar)0xF001, 11);
    setIcon(MainFrame::Btn_MenuMin, (QChar)0xf066);
    setIcon(MainFrame::Btn_MenuMax, (QChar)0xf065);
    setIcon(MainFrame::Btn_MenuClose, (QChar)0xf00d);

    setTitle("QMusicPlayer");
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    centralWidget = 0;
    isMaximized = false;
    location = this->geometry();
    this->setProperty("form", true);
    //    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setWindowFlags(Qt::FramelessWindowHint);//

    this->installEventFilter(this);
    this->widget_Title->installEventFilter(this);
}

void MainFrame::handleBtnMenuMinClick()
{
    showMinimized();
}

void MainFrame::handleBtnMenuMaxClick()
{
    if (isMaximized)
    {
        showNormal();
        this->setGeometry(location);
    }
    else
    {
        location = this->geometry();
        showMaximized();
    }

    isMaximized = !isMaximized;
}

void MainFrame::handleBtnMenuCloseClick()
{
    close();
}

void MainFrame::setIcon(Widget widget, QChar str, quint32 size)
{
    if (widget == MainFrame::Label_Icon)
    {
        IconHelper::Instance()->setIcon(this->label_Icon, str, size);
    }
    else if (widget == MainFrame::Btn_MenuMin)
    {
        IconHelper::Instance()->setIcon(this->btn_MenuMin, str, size);
    }
    else if (widget == MainFrame::Btn_MenuMax)
    {
        IconHelper::Instance()->setIcon(this->btn_MenuMax, str, size);
    }
    else if (widget == MainFrame::Btn_MenuClose)
    {
        IconHelper::Instance()->setIcon(this->btn_MenuClose, str, size);
    }
}

void MainFrame::setPixmap(Widget widget, const QString &file, const QSize &size)
{
    QPixmap pix = QPixmap(file);
    pix = pix.scaled(size, Qt::KeepAspectRatio);

    if (widget == MainFrame::Label_Icon)
    {
        this->label_Icon->setPixmap(pix);
    }
    else if (widget == MainFrame::Btn_MenuMin)
    {
        this->btn_MenuMin->setIcon(QIcon(file));
    }
    else if (widget == MainFrame::Btn_MenuMax)
    {
        this->btn_MenuMax->setIcon(QIcon(file));
    }
    else if (widget == MainFrame::Btn_MenuClose)
    {
        this->btn_MenuClose->setIcon(QIcon(file));
    }
}

void MainFrame::setVisible(Widget widget, bool visible)
{
    if (widget == MainFrame::Label_Icon)
    {
        this->label_Icon->setVisible(visible);
    }
    else if (widget == MainFrame::Btn_MenuMin)
    {
        this->btn_MenuMin->setVisible(visible);
    }
    else if (widget == MainFrame::Btn_MenuMax)
    {
        this->btn_MenuMax->setVisible(visible);
    }
    else if (widget == MainFrame::Btn_MenuClose)
    {
        this->btn_MenuClose->setVisible(visible);
    }
}

void MainFrame::setOnlyCloseBtn()
{
    this->btn_MenuMin->setVisible(false);
    this->btn_MenuMax->setVisible(false);
}

void MainFrame::setTitleHeight(int height)
{
    this->widget_Title->setFixedHeight(height);
}

void MainFrame::setBtnWidth(int width)
{
    this->label_Icon->setFixedWidth(width);
    this->btn_MenuMin->setFixedWidth(width);
    this->btn_MenuMax->setFixedWidth(width);
    this->btn_MenuClose->setFixedWidth(width);
}

void MainFrame::setTitle(const QString &title)
{
    if (this->title != title)
    {
        this->title = title;
        this->label_Title->setText(title);
        QFont font = this->label_Title->font();
        font.setBold(true);
        this->label_Title->setFont(font);
        this->setWindowTitle(this->label_Title->text());
    }
}

void MainFrame::setAlignment(Qt::Alignment alignment)
{
    if (this->alignment != alignment)
    {
        this->alignment = alignment;
        this->label_Title->setAlignment(alignment);
    }
}

void MainFrame::setCentralWidget(QWidget *widget)
{
    if (this->centralWidget == 0) {
        this->widget_Central->layout()->addWidget(widget);

        resize(widget->width(), widget->height() + this->widget_Title->height());

        this->centralWidget = widget;
        this->centralWidget->installEventFilter(this);
    }
}

QMFMessageBox *QMFMessageBox::self = 0;
QMFMessageBox::QMFMessageBox(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
}

QMFMessageBox::~QMFMessageBox()
{
    delete widget_Main;
}

void QMFMessageBox::initControl()
{
    this->setObjectName(QString::fromUtf8("QMFMessageBox"));
    this->resize(280, 150);
    vbL_QMFMessageBox = new QVBoxLayout(this);
    vbL_QMFMessageBox->setSpacing(0);
    vbL_QMFMessageBox->setObjectName(QString::fromUtf8("vbL_QMFMessageBox"));
    vbL_QMFMessageBox->setContentsMargins(1, 1, 1, 1);

    widget_Title = new QWidget(this);
    widget_Title->setObjectName(QString::fromUtf8("widget_Title"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widget_Title->sizePolicy().hasHeightForWidth());
    widget_Title->setSizePolicy(sizePolicy);
    widget_Title->setMinimumSize(QSize(0, 30));
    hbL_Title = new QHBoxLayout(widget_Title);
    hbL_Title->setSpacing(0);
    hbL_Title->setObjectName(QString::fromUtf8("hbL_Title"));
    hbL_Title->setContentsMargins(0, 0, 0, 0);

    label_Icon = new QLabel(widget_Title);
    label_Icon->setObjectName(QString::fromUtf8("label_Icon"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_Icon->sizePolicy().hasHeightForWidth());
    label_Icon->setSizePolicy(sizePolicy1);
    label_Icon->setMinimumSize(QSize(31, 0));
    label_Icon->setAlignment(Qt::AlignCenter);
    hbL_Title->addWidget(label_Icon);

    label_Title = new QLabel(widget_Title);
    label_Title->setObjectName(QString::fromUtf8("label_Title"));
    label_Title->setStyleSheet(QString::fromUtf8(""));
    label_Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    hbL_Title->addWidget(label_Title);

    label_Time = new QLabel(widget_Title);
    label_Time->setObjectName(QString::fromUtf8("label_Time"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label_Time->sizePolicy().hasHeightForWidth());
    label_Time->setSizePolicy(sizePolicy2);
    label_Time->setAlignment(Qt::AlignCenter);
    hbL_Title->addWidget(label_Time);

    widget_Menu = new QWidget(widget_Title);
    widget_Menu->setObjectName(QString::fromUtf8("widget_Menu"));
    sizePolicy1.setHeightForWidth(widget_Menu->sizePolicy().hasHeightForWidth());
    widget_Menu->setSizePolicy(sizePolicy1);
    hbL_Menu = new QHBoxLayout(widget_Menu);
    hbL_Menu->setSpacing(0);
    hbL_Menu->setObjectName(QString::fromUtf8("hbL_Menu"));
    hbL_Menu->setContentsMargins(0, 0, 0, 0);

    btn_MenuClose = new QPushButton(widget_Menu);
    btn_MenuClose->setObjectName(QString::fromUtf8("btn_MenuClose"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btn_MenuClose->sizePolicy().hasHeightForWidth());
    btn_MenuClose->setSizePolicy(sizePolicy3);
    btn_MenuClose->setMinimumSize(QSize(30, 0));
    btn_MenuClose->setMaximumSize(QSize(30, 16777215));
    btn_MenuClose->setCursor(QCursor(Qt::ArrowCursor));
    btn_MenuClose->setFocusPolicy(Qt::NoFocus);
    btn_MenuClose->setFlat(true);
    hbL_Menu->addWidget(btn_MenuClose);

    hbL_Title->addWidget(widget_Menu);

    vbL_QMFMessageBox->addWidget(widget_Title);

    widget_Main = new QWidget(this);
    widget_Main->setObjectName(QString::fromUtf8("widget_Main"));
    widget_Main->setStyleSheet(QString::fromUtf8(""));
    vbL_Main = new QVBoxLayout(widget_Main);
    vbL_Main->setSpacing(5);
    vbL_Main->setObjectName(QString::fromUtf8("vbL_Main"));
    vbL_Main->setContentsMargins(5, 5, 5, 5);

    frame = new QFrame(widget_Main);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);
    vbl_Frame = new QVBoxLayout(frame);
    vbl_Frame->setObjectName(QString::fromUtf8("vbl_Frame"));
    vbl_Frame->setContentsMargins(-1, 9, -1, -1);

    hbL_Inner1 = new QHBoxLayout();
    hbL_Inner1->setObjectName(QString::fromUtf8("hbL_Inner1"));
    label_IconMain = new QLabel(frame);
    label_IconMain->setObjectName(QString::fromUtf8("label_IconMain"));
    label_IconMain->setMinimumSize(QSize(30, 30));
    label_IconMain->setMaximumSize(QSize(30, 30));
    label_IconMain->setStyleSheet(QString::fromUtf8("border-image: url(:/images/img/msg_info.png);"));
    hbL_Inner1->addWidget(label_IconMain);

    hSpacer_1 = new QSpacerItem(5, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hbL_Inner1->addItem(hSpacer_1);

    label_Info = new QLabel(frame);
    label_Info->setObjectName(QString::fromUtf8("label_Info"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(label_Info->sizePolicy().hasHeightForWidth());
    label_Info->setSizePolicy(sizePolicy4);
    label_Info->setMinimumSize(QSize(0, 33));
    label_Info->setScaledContents(false);
    label_Info->setWordWrap(true);
    hbL_Inner1->addWidget(label_Info);

    vbl_Frame->addLayout(hbL_Inner1);

    hbL_Inner2 = new QHBoxLayout();
    hbL_Inner2->setObjectName(QString::fromUtf8("hbL_Inner2"));

    hSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hbL_Inner2->addItem(hSpacer_2);

    btn_Ok = new QPushButton(frame);
    btn_Ok->setObjectName(QString::fromUtf8("btn_Ok"));
    btn_Ok->setMinimumSize(QSize(80, 0));
    btn_Ok->setFocusPolicy(Qt::StrongFocus);
    btn_Ok->setStyleSheet(QString::fromUtf8(""));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/img/btn_ok.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_Ok->setIcon(icon);
    hbL_Inner2->addWidget(btn_Ok);

    btn_Cancel = new QPushButton(frame);
    btn_Cancel->setObjectName(QString::fromUtf8("btn_Cancel"));
    btn_Cancel->setMinimumSize(QSize(80, 0));
    btn_Cancel->setFocusPolicy(Qt::StrongFocus);
    btn_Cancel->setStyleSheet(QString::fromUtf8(""));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/img/btn_close.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_Cancel->setIcon(icon1);
    hbL_Inner2->addWidget(btn_Cancel);

    vbl_Frame->addLayout(hbL_Inner2);

    vbL_Main->addWidget(frame);

    vbL_QMFMessageBox->addWidget(widget_Main);

    widget_Title->raise();
    widget_Main->raise();
    frame->raise();

    this->btn_Ok->setText("确定");
    this->btn_Cancel->setText("取消");

    connect(this->btn_Ok, SIGNAL(clicked(bool)), this, SLOT(handleBtnOkClick()));
    connect(this->btn_MenuClose, SIGNAL(clicked(bool)), this, SLOT(handleBtnMenuCloseClick()));
    connect(this->btn_Cancel, SIGNAL(clicked(bool)), this, SLOT(handleBtnCancelClick()));//
}

void QMFMessageBox::initForm()
{
    IconHelper::Instance()->setIcon(this->label_Icon, QChar(0xF001), 11);
    IconHelper::Instance()->setIcon(this->btn_MenuClose, QChar(0xf00d), 9);

    this->setProperty("form", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
    this->setFixedSize(280, 150);
    this->setWindowTitle(this->label_Title->text());

    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;
    this->label_IconMain->setFixedSize(30, 30);

    QList<QPushButton *> btns1 = this->frame->findChildren<QPushButton *>();

    foreach (QPushButton *btn, btns1)
    {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }

    this->installEventFilter(this);
}

void QMFMessageBox::setMessage(const QString &msg, int type)
{
    if (type == 0)
    {
        this->label_IconMain->setStyleSheet("border-image: url(:/images/img/msg_info.png);");
        this->btn_Cancel->setVisible(false);
        this->label_Title->setText("提示");
    }
    else if (type == 1)
    {
        this->label_IconMain->setStyleSheet("border-image: url(:/images/img/msg_question.png);");
        this->label_Title->setText("询问");
    }
    else if (type == 2)
    {
        this->label_IconMain->setStyleSheet("border-image: url(:/images/img/msg_error.png);");
        this->btn_Cancel->setVisible(false);
        this->label_Title->setText("错误");
    }

    this->label_Info->setText(msg);
    this->setWindowTitle(this->label_Title->text());
}

bool QMFMessageBox::eventFilter(QObject *obj, QEvent *evt)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (event->button() == Qt::LeftButton)
        {
            mousePressed = true;
            mousePoint = event->globalPosition().toPoint() - this->pos();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mousePressed = false;
        return true;
    }
    //    else if (event->type() == QEvent::MouseMove)//限制拖动
    //    {
    //        if (mousePressed && (event->buttons() & Qt::LeftButton))
    //        {
    //            this->move(event->globalPosition().toPoint() - mousePoint);
    //            return true;
    //        }
    //    }

    return QWidget::eventFilter(obj, evt);
}

void QMFMessageBox::handleBtnOkClick()
{
    done(Accepted);
    close();
}

void QMFMessageBox::handleBtnMenuCloseClick()
{
    done(Rejected);
    close();
}

void QMFMessageBox::handleBtnCancelClick()
{
    done(Rejected);
    close();
}


QMFInputBox *QMFInputBox::self = 0;
QMFInputBox::QMFInputBox(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
}

QMFInputBox::~QMFInputBox()
{
    delete widget_Main;
}

void QMFInputBox::initControl()
{
    this->setObjectName(QString::fromUtf8("QMFInputBox"));
    this->resize(260, 166);
    vbL_QMFInputBox = new QVBoxLayout(this);
    vbL_QMFInputBox->setSpacing(0);
    vbL_QMFInputBox->setObjectName(QString::fromUtf8("vbL_QMFInputBox"));
    vbL_QMFInputBox->setContentsMargins(1, 1, 1, 1);

    widget_Title = new QWidget(this);
    widget_Title->setObjectName(QString::fromUtf8("widget_Title"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widget_Title->sizePolicy().hasHeightForWidth());
    widget_Title->setSizePolicy(sizePolicy);
    widget_Title->setMinimumSize(QSize(0, 30));
    hbL_Title = new QHBoxLayout(widget_Title);
    hbL_Title->setSpacing(0);
    hbL_Title->setObjectName(QString::fromUtf8("hbL_Title"));
    hbL_Title->setContentsMargins(0, 0, 0, 0);

    label_Icon = new QLabel(widget_Title);
    label_Icon->setObjectName(QString::fromUtf8("label_Icon"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_Icon->sizePolicy().hasHeightForWidth());
    label_Icon->setSizePolicy(sizePolicy1);
    label_Icon->setMinimumSize(QSize(31, 0));
    label_Icon->setAlignment(Qt::AlignCenter);
    hbL_Title->addWidget(label_Icon);

    label_Title = new QLabel(widget_Title);
    label_Title->setObjectName(QString::fromUtf8("label_Title"));
    label_Title->setStyleSheet(QString::fromUtf8(""));
    label_Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    hbL_Title->addWidget(label_Title);

    label_Time = new QLabel(widget_Title);
    label_Time->setObjectName(QString::fromUtf8("label_Time"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label_Time->sizePolicy().hasHeightForWidth());
    label_Time->setSizePolicy(sizePolicy2);
    label_Time->setAlignment(Qt::AlignCenter);
    hbL_Title->addWidget(label_Time);

    widget_Menu = new QWidget(widget_Title);
    widget_Menu->setObjectName(QString::fromUtf8("widget_Menu"));
    sizePolicy1.setHeightForWidth(widget_Menu->sizePolicy().hasHeightForWidth());
    widget_Menu->setSizePolicy(sizePolicy1);
    hbL_Menu = new QHBoxLayout(widget_Menu);
    hbL_Menu->setSpacing(0);
    hbL_Menu->setObjectName(QString::fromUtf8("hbL_Menu"));
    hbL_Menu->setContentsMargins(0, 0, 0, 0);

    btn_MenuClose = new QPushButton(widget_Menu);
    btn_MenuClose->setObjectName(QString::fromUtf8("btn_MenuClose"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btn_MenuClose->sizePolicy().hasHeightForWidth());
    btn_MenuClose->setSizePolicy(sizePolicy3);
    btn_MenuClose->setMinimumSize(QSize(30, 0));
    btn_MenuClose->setMaximumSize(QSize(30, 16777215));
    btn_MenuClose->setCursor(QCursor(Qt::ArrowCursor));
    btn_MenuClose->setFocusPolicy(Qt::NoFocus);
    btn_MenuClose->setFlat(true);
    hbL_Menu->addWidget(btn_MenuClose);

    hbL_Title->addWidget(widget_Menu);

    vbL_QMFInputBox->addWidget(widget_Title);

    widget_Main = new QWidget(this);
    widget_Main->setObjectName(QString::fromUtf8("widget_Main"));
    widget_Main->setStyleSheet(QString::fromUtf8(""));
    vbL_Main = new QVBoxLayout(widget_Main);
    vbL_Main->setSpacing(5);
    vbL_Main->setObjectName(QString::fromUtf8("vbL_Main"));
    vbL_Main->setContentsMargins(5, 5, 5, 5);

    frame = new QFrame(widget_Main);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);
    vbL_Frame = new QVBoxLayout(frame);
    vbL_Frame->setObjectName(QString::fromUtf8("vbL_Frame"));

    label_Info = new QLabel(frame);
    label_Info->setObjectName(QString::fromUtf8("label_Info"));
    label_Info->setScaledContents(false);
    label_Info->setWordWrap(true);
    vbL_Frame->addWidget(label_Info);

    txtValue = new QLineEdit(frame);
    txtValue->setObjectName(QString::fromUtf8("txtValue"));
    vbL_Frame->addWidget(txtValue);

    cboxValue = new QComboBox(frame);
    cboxValue->setObjectName(QString::fromUtf8("cboxValue"));
    vbL_Frame->addWidget(cboxValue);

    hbL_Inner = new QHBoxLayout();
    hbL_Inner->setObjectName(QString::fromUtf8("hbL_Inner"));

    hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hbL_Inner->addItem(hSpacer);

    btn_Ok = new QPushButton(frame);
    btn_Ok->setObjectName(QString::fromUtf8("btn_Ok"));
    btn_Ok->setMinimumSize(QSize(80, 0));
    btn_Ok->setStyleSheet(QString::fromUtf8(""));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/img/btn_ok.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_Ok->setIcon(icon);
    hbL_Inner->addWidget(btn_Ok);

    btn_Cancel = new QPushButton(frame);
    btn_Cancel->setObjectName(QString::fromUtf8("btn_Cancel"));
    btn_Cancel->setMinimumSize(QSize(80, 0));
    btn_Cancel->setStyleSheet(QString::fromUtf8(""));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/img/btn_close.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_Cancel->setIcon(icon1);
    hbL_Inner->addWidget(btn_Cancel);

    vbL_Frame->addLayout(hbL_Inner);

    vbL_Main->addWidget(frame);

    vbL_QMFInputBox->addWidget(widget_Main);

    setTabOrder(txtValue, btn_Ok);
    setTabOrder(btn_Ok, btn_Cancel);

    this->label_Title->setText("输入框");
    this->btn_Ok->setText("确定");
    this->btn_Cancel->setText("取消");

    connect(this->btn_Ok, SIGNAL(clicked(bool)), this, SLOT(handleBtnOkClick()));
    connect(this->btn_MenuClose, SIGNAL(clicked(bool)), this, SLOT(handleBtnCancelClick()));
    connect(this->btn_Cancel, SIGNAL(clicked(bool)), this, SLOT(handleBtnCancelClick()));
}

void QMFInputBox::initForm()
{
    IconHelper::Instance()->setIcon(this->label_Icon, QChar(0xF001), 11);
    IconHelper::Instance()->setIcon(this->btn_MenuClose, QChar(0xf00d), 9);

    this->setProperty("form", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
    this->setFixedSize(280, 150);
    this->setWindowTitle(this->label_Title->text());

    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;

    QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();

    foreach (QPushButton *btn, btns)
    {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }

    this->installEventFilter(this);
}

void QMFInputBox::setParameter(const QString &title, int type, QString defaultValue, bool pwd)
{
    this->label_Info->setText(title);

    if (type == 0)
    {
        this->cboxValue->setVisible(false);
        this->txtValue->setPlaceholderText(defaultValue);

        if (pwd)
        {
            this->txtValue->setEchoMode(QLineEdit::Password);
        }
    }
    else if (type == 1)
    {
        this->txtValue->setVisible(false);
        this->cboxValue->addItems(defaultValue.split("|"));
        if (defaultValue == "无可添加歌单") {
            this->btn_Ok->setEnabled(false);
        }
    }
}

QString QMFInputBox::getValue() const
{
    return this->value;
}

bool QMFInputBox::eventFilter(QObject *obj, QEvent *evt)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (event->button() == Qt::LeftButton)
        {
            mousePressed = true;
            mousePoint = event->globalPosition().toPoint() - this->pos();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mousePressed = false;
        return true;
    }
    //    else if (event->type() == QEvent::MouseMove)//限制拖动
    //    {
    //        if (mousePressed && (event->buttons() & Qt::LeftButton))
    //        {
    //            this->move(event->globalPosition().toPoint() - mousePoint);
    //            return true;
    //        }
    //    }

    return QWidget::eventFilter(obj, evt);
}

void QMFInputBox::handleBtnOkClick()
{
    if (this->txtValue->isVisible())
    {
        value = this->txtValue->text();
    }
    else if (this->cboxValue->isVisible())
    {
        value = this->cboxValue->currentText();
    }

    int ret =  MainFrame::showMessageBoxQuestion("请确定输入是否正确：" + value);//*
    if(ret == QDialog::Accepted)
    {
        done(Accepted);
        close();
    }
    else if(ret ==QDialog::Rejected)
    {
        return;
    }

}

void QMFInputBox::handleBtnCancelClick()
{
    close();
}


IconHelper *IconHelper::self = 0;
IconHelper::IconHelper(QObject *) : QObject(qApp)
{
    int fontId = QFontDatabase::addApplicationFont(":/fonts/res/fonts/fontawesome-webfont.ttf");
    QStringList fontName = QFontDatabase::applicationFontFamilies(fontId);

    if (fontName.count() > 0)
    {
        iconFont = QFont(fontName.at(0));
    }
    else
    {
        qDebug() << "load fontawesome-webfont.ttf error";
    }
}

void IconHelper::setIcon(QLabel *lab, QChar c, quint32 size)
{
    iconFont.setPointSize(size);
    lab->setFont(iconFont);
    lab->setText(c);
}

void IconHelper::setIcon(QAbstractButton *btn, QChar c, quint32 size)
{
    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}
