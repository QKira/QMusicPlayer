#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CentralWidget)
    , dbm("default", "QSQLITE", "D:/000_QtPro/QMusicPlayer/res/db/data.db")
    , qry(dbm.database())
{
    ui->setupUi(this);
    this->setProperty("form", true);

    initWidgetleftUI();

    initPagePlayingUI();

    initWidgetPlaylistsBottomLeftUI();
    // testAddButton();

    initPageSettingsUI();


    // QList<QObject *> objs = ui->page_Playing->findChildren<QObject *>();
    // foreach (QObject *obj, objs) {
    //     qDebug() << obj->objectName();
    // }
}

CentralWidget::~CentralWidget()
{
    delete ui;
}

//widget_Left
void CentralWidget::updateNaviButtonPtrs()
{
    naviButtonPtrs = ui->widget_Left->findChildren<QPushButton *>();
}

void CentralWidget::initWidgetleftUI()
{
    updateNaviButtonPtrs();
    foreach (QPushButton *naviButton, naviButtonPtrs) {
        if(naviButton->objectName() == "button_Playing") {
            naviButton->setChecked(true);
        }
        connect(naviButton, &QPushButton::clicked, this, &CentralWidget::handleNaviButtonClick);
    }
    ui->stackedWidget->setCurrentIndex(0);
}

void CentralWidget::handleNaviButtonClick()
{
    updateNaviButtonPtrs();

    QPushButton *clickedButtonPtr = (QPushButton *)sender();

    foreach (QPushButton *naviButtonPtr, naviButtonPtrs) {
        if(naviButtonPtr == clickedButtonPtr) {
            naviButtonPtr->setChecked(true);
        } else {
            naviButtonPtr->setChecked(false);
        }
    }

    if(clickedButtonPtr->objectName() == "button_Playing")
        ui->stackedWidget->setCurrentIndex(0);
    else if(clickedButtonPtr->objectName() == "button_Playlists")
        ui->stackedWidget->setCurrentIndex(1);
    else if(clickedButtonPtr->objectName() == "button_Library")
        ui->stackedWidget->setCurrentIndex(2);
    else if(clickedButtonPtr->objectName() == "button_Statistics")
        ui->stackedWidget->setCurrentIndex(3);
    else if(clickedButtonPtr->objectName() == "button_Settings")
        ui->stackedWidget->setCurrentIndex(4);
}

//Page_Playing
void CentralWidget::initPagePlayingUI()
{
    QPixmap pixmap{":/images/res/images/preview.jpg"};
    pixmap.scaled(ui->label_Playing_DetailPic->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_Playing_DetailPic->setPixmap(pixmap);
    ui->label_Playing_DetailPic->setScaledContents(true);
    int imgW = pixmap.width();
    int imgH = pixmap.height();
    detailPicScale = imgW / imgH;
    ui->label_Playing_DetailPic->resize(imgW, imgH);
    ui->label_Playing_DetailPic->move(0, 0);
}

//Page_Playlists
void CentralWidget::updatePlaylistsNaviButtonPtrs()
{
    playlistsNaviButtonPtrs = ui->scrollAreaWidget_Playlists->findChildren<QPushButton *>();
    foreach (QPushButton *playlistsNaviButtonPtr, playlistsNaviButtonPtrs) {
        playlistsNaviButtonPtr->setCheckable(true);
    }
}

void CentralWidget::testAddButton()
{
    int i = 0;
    foreach (QPushButton* buttonPtr, vct_ButtonPtrs) {
        QString str = "Button " + QString::number(i);
        buttonPtr = new QPushButton(str, ui->scrollAreaWidget_Playlists);
        QLayout * sa = ui->scrollAreaWidget_Playlists->layout();
        sa->addWidget(buttonPtr);
        i++;
        connect(buttonPtr, &QPushButton::clicked, this, &CentralWidget::handlePlaylistsNaviButtonClick);
    }
    updatePlaylistsNaviButtonPtrs();
}

void CentralWidget::initWidgetPlaylistsBottomLeftUI()
{
    updatePlaylistsNaviButtonPtrs();

    foreach (QPushButton *playlistsNaviButtonPtr, playlistsNaviButtonPtrs) {
        if(playlistsNaviButtonPtr->objectName() == "button_Playlists_favor") {
            playlistsNaviButtonPtr->setChecked(true);
        }
        connect(playlistsNaviButtonPtr, &QPushButton::clicked, this, &CentralWidget::handlePlaylistsNaviButtonClick);
    }
}

void CentralWidget::handlePlaylistsNaviButtonClick()
{
    updatePlaylistsNaviButtonPtrs();

    QPushButton *clickedButtonPtr = (QPushButton *)sender();

    foreach (QPushButton *playlistsNaviButtonPtr, playlistsNaviButtonPtrs) {
        if(playlistsNaviButtonPtr == clickedButtonPtr) {
            playlistsNaviButtonPtr->setChecked(true);
        } else {
            playlistsNaviButtonPtr->setChecked(false);
        }
    }
}

//Page_Library


//Page_Statistics


//Page_Settings
void CentralWidget::initPageSettingsUI()
{
    tableView_PathMonitor = ui->tableView_PathMonitor;
    tableView_PathMonitor->setModel(&standardItemModel_PathMonitor);
    standardItemModel_PathMonitor.setColumnCount(1);
    standardItemModel_PathMonitor.setHeaderData(0, Qt::Horizontal, "路径");
    tableView_PathMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView_PathMonitor->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_PathMonitor->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_PathMonitor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // tableView_PathMonitor->show();
    updatePathMonitorModelData();
    connect(ui->button_PathMonitor_AddPath, &QPushButton::clicked, this, &CentralWidget::handleSettingsPathMonitorButtonClick);
    connect(ui->button_PathMonitor_DeletePath, &QPushButton::clicked, this, &CentralWidget::handleSettingsPathMonitorButtonClick);
    connect(ui->tableView_PathMonitor->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CentralWidget::handleTableViewPathMonitorChanged);
}

void CentralWidget::updatePathMonitorModelData()
{
    standardItemModel_PathMonitor.removeRows(0, standardItemModel_PathMonitor.rowCount());
    sql =  QString("SELECT * FROM %1;").arg("path_monitor");
    if (qry.exec(sql)) {
        int cnt = 0;
        while (qry.next()) {
            // int path_id = qry.value(0).toInt();
            QString path_value = qry.value(1).toString();
            standardItemModel_PathMonitor.setItem(cnt++, 0, new QStandardItem(path_value));
        }
    } else {
        qDebug() << "CentralWidget::updatePathMonitorModelData()->查询失败：" << qry.lastError().text();
    }
}

void CentralWidget::scanDirectory(QString directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        qDebug() << "CentralWidget::scanDirectory(QString directory)->" << "目录不存在:" << directory;
        return;
    }

    QDirIterator qit(directory, QStringList() << "*.flac", QDir::Files, QDirIterator::Subdirectories);
    // int count = 0;
    QHash<QString, QVariant> metadata;
    while (qit.hasNext()) {
        QString filePath = qit.next();

        sql = QString("SELECT * FROM %1 WHERE song_path = '%2'").arg("songs").arg(filePath);
        if (qry.exec(sql) && qry.first()) {
            qDebug() << "CentralWidget::scanDirectory(QString directory)->" << "歌曲已存在";
            continue;
        }

        qint64 song_id = 1;
        qry.prepare("SELECT MAX(song_id) FROM songs");
        if (qry.exec() && qry.first() && !qry.value(0).isNull()) {
            song_id = qry.value(0).toInt() + 1;
        }

        metadata.clear();
        metadata["song_id"] = song_id;
        metadata["song_title"] = "unknown";
        metadata["song_artist"] = "unknown";
        metadata["song_album"] = "unknown";
        metadata["song_lyrics"] = "No lyrics";
        metadata["song_path"] = filePath;
        metadata["cover_path"] = "./res/cover_default.jpg";
        metadata["song_size"] = QFileInfo(filePath).size();
        metadata["song_duration"] = 0;


        QByteArray byteArray = filePath.toUtf8();
        AVFormatContext *formatContext = nullptr;
        if (avformat_open_input(&formatContext, byteArray.constData(), nullptr, nullptr) != 0) {
            qDebug() << "Failed to open FLAC file";
        }

        if (formatContext) {
            if (avformat_find_stream_info(formatContext, nullptr) >= 0) {
                metadata["song_duration"] = formatContext->duration / AV_TIME_BASE;
                AVDictionaryEntry *tag = nullptr;
                while ((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
                    QString key = QString::fromUtf8(tag->key).toLower();
                    QString value = QString::fromUtf8(tag->value);
                    if (key == "title") metadata["song_title"] = value;
                    else if (key == "artist") metadata["song_artist"] = value;
                    else if (key == "album") metadata["song_album"] = value;
                    else if (key.contains("lyrics")) metadata["song_lyrics"] = value;
                }
            }
            avformat_close_input(&formatContext);
        }

        QProcess process;
        QString program = "ffmpeg.exe";
        QStringList arguments;
        arguments << "-i" << filePath
                  << "-an" << "-vcodec" << "mjpeg"
                  << "-frames:v" << "1" << "-update" << "1"
                  << "./res/cover/cover_" + QString::number(song_id) + ".jpg";

        metadata["cover_path"] = "./res/cover/cover_" + QString::number(song_id) + ".jpg";

        process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
            args->flags |= CREATE_NO_WINDOW;
        });
        process.startDetached(program, arguments);

        sql = QString("INSERT INTO %1 (song_title, song_artist, song_path, cover_path, song_lyrics, song_size, song_duration)"
                      "VALUES ('%2', '%3', '%4', '%5', '%6', '%7', '%8')")
                    .arg("songs")
                    .arg(metadata.value("song_title").toString())
                    .arg(metadata.value("song_artist").toString())
                    .arg(metadata.value("song_path").toString())
                    .arg(metadata.value("cover_path").toString())
                    .arg(metadata.value("song_lyrics").toString())
                    .arg(metadata.value("song_size").toLongLong())
                    .arg(metadata.value("song_duration").toLongLong());

        if(!qry.exec(sql)) {
            qDebug() << "CentralWidget::scanDirectory(QString directory)->" << "Insert metadata error!";
        }
        // qDebug() << metadata.value("song_id");
        // qDebug() << metadata.value("song_title");
        // qDebug() << metadata.value("song_artist");
        // qDebug() << metadata.value("song_album");
        // qDebug() << metadata.value("song_lyrics");
        // qDebug() << metadata.value("song_path");
        // qDebug() << metadata.value("cover_path");
        // qDebug() << metadata.value("song_size");
        // qDebug() << metadata.value("song_duration");
        // count++;
    }
    // qDebug() << "CentralWidget::scanDirectory(QString directory)->" << "共 " << count << " 个音乐文件!";
}

void CentralWidget::handleSettingsPathMonitorButtonClick()
{
    QPushButton *clickedButtonPtr = (QPushButton *)sender();
    if(clickedButtonPtr == ui->button_PathMonitor_AddPath)
    {
        ui->button_PathMonitor_DeletePath->setEnabled(false);
        tableView_PathMonitor->clearSelection();
        QString pickPath = QFileDialog::getExistingDirectory(nullptr, "选择监控路径", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!pickPath.isEmpty()) {
            sql =  QString("SELECT * FROM %1 WHERE path_value = '%2';").arg("path_monitor").arg(pickPath);
            if(qry.exec(sql)) {
                if (qry.first()) {
                    MainFrame::showMessageBoxError("此路径已存在！");
                } else {
                    sql = QString("INSERT INTO %1 (path_value)"
                                  "VALUES ('%2')").arg("path_monitor").arg(pickPath);
                    if(qry.exec(sql)) {
                        scanDirectory(pickPath);
                    } else {
                        qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->" << qry.lastError().text();
                    }
                }
            } else {
                qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->" << qry.lastError().text();
            }
        } else {
            qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->未选择路径或路径为空！";
        }
        updatePathMonitorModelData();
    } else {
        QModelIndexList indexes = tableView_PathMonitor->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty()) {
            QModelIndex index = indexes.first();
            QString path = standardItemModel_PathMonitor.data(index, Qt::DisplayRole).toString();
            // qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->" << path;
            sql = QString("DELETE FROM %1 WHERE path_value = '%2'").arg("path_monitor").arg(path);
            if(!qry.exec(sql)) {
                qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->" << qry.lastError().text();
            }
        }
        ui->button_PathMonitor_DeletePath->setEnabled(false);
        tableView_PathMonitor->clearSelection();
        updatePathMonitorModelData();
    }
}

void CentralWidget::handleTableViewPathMonitorChanged()
{
    QModelIndexList indexes = tableView_PathMonitor->selectionModel()->selectedIndexes();
    if (!indexes.isEmpty()) {
        ui->button_PathMonitor_DeletePath->setEnabled(true);
        // QModelIndex index = indexes.first(); // 取第一个选中的单元格
        // QString text = standardItemModel_PathMonitor.data(index, Qt::DisplayRole).toString();
        // qDebug() << CentralWidget::handleTableViewPathMonitorChanged()-> << "选中内容：" << text;
    }
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
