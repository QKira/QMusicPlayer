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

    initPageLibraryUI();

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
void CentralWidget::initPageLibraryUI()
{
    tableView_Library = ui->tableView_Library;
    tableView_Library->setModel(&standardItemModel_Library);
    standardItemModel_Library.setColumnCount(5);
    standardItemModel_Library.setHeaderData(0, Qt::Horizontal, "Title");
    standardItemModel_Library.setHeaderData(1, Qt::Horizontal, "Artist");
    standardItemModel_Library.setHeaderData(2, Qt::Horizontal, "Album");
    standardItemModel_Library.setHeaderData(3, Qt::Horizontal, "Size");
    standardItemModel_Library.setHeaderData(4, Qt::Horizontal, "Duration");
    tableView_Library->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_Library->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView_Library->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableView_Library->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    tableView_Library->horizontalHeader()->resizeSection(3, 100);
    tableView_Library->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    tableView_Library->horizontalHeader()->resizeSection(4, 100);
    tableView_Library->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView_Library->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_Library->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_Library->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // tableView_Library->verticalHeader()->setVisible(false);
    ui->tableView_Library->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateLibraryModelData();

    connect(tableView_Library->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CentralWidget::onSelectionChanged);
}

void CentralWidget::updateLibraryModelData()
{
    standardItemModel_Library.removeRows(0, standardItemModel_Library.rowCount());
    qry.prepare("SELECT song_id, song_title, song_artist, song_album, song_size, song_duration FROM songs");
    if (qry.exec()) {
        int cnt = 0;
        while (qry.next()) {
            qint64 song_id = qry.value(0).toLongLong();
            QString title = qry.value(1).toString();
            QString artist = qry.value(2).toString();
            QString album = qry.value(3).toString();
            QString size = QString::number(qry.value(4).toLongLong() / 1048576.0f, 'f', 1) + " MB";
            long long totalSeconds = qry.value(5).toLongLong();
            long long minutes = totalSeconds / 60;
            long long seconds = totalSeconds % 60;
            QString duration = QString("%1:%2")
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0'));
            QStandardItem* idTitleItem = new QStandardItem(title);
            idTitleItem->setData(song_id, Qt::UserRole);
            standardItemModel_Library.setItem(cnt, 0, idTitleItem);
            standardItemModel_Library.setItem(cnt, 1, new QStandardItem(artist));
            standardItemModel_Library.setItem(cnt, 2, new QStandardItem(album));
            QStandardItem* sizeItem = new QStandardItem(size);
            sizeItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Library.setItem(cnt, 3, sizeItem);
            QStandardItem* durationItem = new QStandardItem(duration);
            durationItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Library.setItem(cnt, 4, durationItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::updatePathMonitorModelData()->查询失败：" << qry.lastError().text();
    }
}

void CentralWidget::on_lineEdit_Search_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        updateLibraryModelData();
        return;
    }

    // qDebug() << arg1;

    if (standardItemModel_Library.rowCount() > 0) {
        standardItemModel_Library.removeRows(0, standardItemModel_Library.rowCount());
    }

    qry.prepare("SELECT song_id, song_title, song_artist, song_album, song_size, song_duration FROM songs WHERE song_title like ?");
    qry.addBindValue("%" + arg1 + "%");
    if (qry.exec()) {
        int cnt = 0;
        while (qry.next()) {
            qint64 song_id = qry.value(0).toLongLong();
            QString title = qry.value(1).toString();
            QString artist = qry.value(2).toString();
            QString album = qry.value(3).toString();
            QString size = QString::number(qry.value(4).toLongLong() / 1048576.0f, 'f', 1) + " MB";
            long long totalSeconds = qry.value(5).toLongLong();
            long long minutes = totalSeconds / 60;
            long long seconds = totalSeconds % 60;
            QString duration = QString("%1:%2")
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0'));
            QStandardItem* idTitleItem = new QStandardItem(title);
            idTitleItem->setData(song_id, Qt::UserRole);
            standardItemModel_Library.setItem(cnt, 0, idTitleItem);
            standardItemModel_Library.setItem(cnt, 1, new QStandardItem(artist));
            standardItemModel_Library.setItem(cnt, 2, new QStandardItem(album));
            QStandardItem* sizeItem = new QStandardItem(size);
            sizeItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Library.setItem(cnt, 3, sizeItem);
            QStandardItem* durationItem = new QStandardItem(duration);
            durationItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Library.setItem(cnt, 4, durationItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::on_lineEdit_Search_textChanged(const QString &arg1)->查询失败：" << qry.lastError().text();
    }
}

void CentralWidget::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)
    bool hasSelection = tableView_Library->selectionModel()->hasSelection();
    ui->button_Library_NewPL->setEnabled(hasSelection);
    ui->button_Library_Add2PL->setEnabled(hasSelection);
    ui->button_Library_DelSong->setEnabled(hasSelection);
}


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
    qry.prepare("SELECT * FROM path_monitor");
    if (qry.exec()) {
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
    QSqlDatabase::database().transaction();

    while (qit.hasNext()) {
        QString filePath = qit.next();

        qry.prepare("SELECT * FROM songs WHERE song_path = ?");
        qry.addBindValue(filePath);
        if (qry.exec() && qry.first()) {
            qDebug() << "CentralWidget::scanDirectory(QString directory)->" << "歌曲已存在";
            continue;
        }

        qint64 song_id = 1;
        qry.prepare("SELECT MAX(song_id) FROM songs");
        if (qry.exec() && qry.first() && !qry.value(0).isNull()) {
            song_id = qry.value(0).toInt() + 1;
        }

        QHash<QString, QVariant> metadata;
        metadata.clear();
        metadata["song_id"] = song_id;
        metadata["song_title"] = "unknown";
        metadata["song_artist"] = "unknown";
        metadata["song_album"] = "unknown";
        metadata["song_lyrics"] = "No lyrics";
        metadata["song_path"] = filePath;
        metadata["cover_path"] = QCoreApplication::applicationDirPath() + "/res/cover/cover_default.jpg";
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

        // 使用 extractCoverFromAudio 提取封面图片
        QString coverPath = QCoreApplication::applicationDirPath() +
                            QString("/res/cover/cover_%1.jpg").arg(song_id);
        QDir().mkpath(QFileInfo(coverPath).absolutePath());
        if (extractCover(filePath, coverPath) && QFile::exists(coverPath)) {
            metadata["cover_path"] = coverPath;
        }

        qry.prepare("INSERT INTO songs (song_title, song_artist, song_path, cover_path, song_lyrics, song_size, song_duration, song_album)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        qry.addBindValue(metadata.value("song_title").toString());
        qry.addBindValue(metadata.value("song_artist").toString());
        qry.addBindValue(metadata.value("song_path").toString());
        qry.addBindValue(metadata.value("cover_path").toString());
        qry.addBindValue(metadata.value("song_lyrics").toString());
        qry.addBindValue(metadata.value("song_size").toLongLong());
        qry.addBindValue(metadata.value("song_duration").toLongLong());
        qry.addBindValue(metadata.value("song_album").toString());
        if(!qry.exec()) {
            qDebug() << "CentralWidget::scanDirectory(QString directory)->" << metadata.value("song_title").toString() << ": Insert metadata error!" << qry.lastError().text();
            // qDebug() << metadata.value("song_id");
            // qDebug() << metadata.value("song_title");
            // qDebug() << metadata.value("song_artist");
            // qDebug() << metadata.value("song_album");
            // qDebug() << metadata.value("song_lyrics");
            // qDebug() << metadata.value("song_path");
            // qDebug() << metadata.value("cover_path");
            // qDebug() << metadata.value("song_size");
            // qDebug() << metadata.value("song_duration");
        }
    }

    // for (QProcess *proc : processList) {
    //     proc->deleteLater();
    // }
    // processList.clear();

    QSqlDatabase::database().commit();
}

bool CentralWidget::extractCover(const QString &audioFilePath, const QString &outputPath)
{
    // 打开输入文件
    AVFormatContext *formatContext = nullptr;
    QByteArray filePathUtf8 = audioFilePath.toUtf8();

    if (avformat_open_input(&formatContext, filePathUtf8.constData(), nullptr, nullptr) != 0) {
        qDebug() << "无法打开音频文件:" << audioFilePath;
        return false;
    }

    // 读取流信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        qDebug() << "无法获取流信息";
        avformat_close_input(&formatContext);
        return false;
    }

    // 查找封面图片流
    int streamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
            (formatContext->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
            streamIndex = i;
            break;
        }
    }

    // 如果找不到封面流，尝试直接从元数据中查找
    if (streamIndex == -1) {
        bool found = false;
        AVDictionaryEntry *tag = nullptr;

        // 查找包含图片的元数据标签
        while ((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            QString key = QString::fromUtf8(tag->key).toLower();

            // 常见的封面标签
            if (key == "cover" || key == "picture" || key == "artwork" || key == "albumart") {
                // 直接提取二进制数据
                QByteArray coverData(tag->value, tag->value ? strlen(tag->value) : 0);

                if (!coverData.isEmpty()) {
                    QImage image;
                    if (image.loadFromData(coverData)) {
                        found = image.save(outputPath, "JPEG");
                        break;
                    }
                }
            }
        }

        avformat_close_input(&formatContext);
        return found;
    }

    // 正常的封面流处理
    // AVPacket packet;
    // av_init_packet(&packet);
    // packet.data = nullptr;
    // packet.size = 0;

    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        qDebug() << "无法分配 AVPacket";
        avformat_close_input(&formatContext);
        return false;
    }

    // 获取封面数据包
    if (av_read_frame(formatContext, packet) >= 0 && packet->stream_index == streamIndex) {
        // 使用attached_pic，它直接包含了完整的图片数据
        if (formatContext->streams[streamIndex]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
            QImage image;

            // 尝试用Qt直接加载图片数据
            if (image.loadFromData(packet->data, packet->size)) {
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return image.save(outputPath, "JPEG");
            }

            // 如果直接加载失败，则使用AVCodec解码
            AVCodecContext *codecContext = nullptr;
            const AVCodec *codec = nullptr;

            // 获取解码器
            codec = avcodec_find_decoder(formatContext->streams[streamIndex]->codecpar->codec_id);
            if (!codec) {
                qDebug() << "无法找到适合的解码器";
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 分配并初始化解码器上下文
            codecContext = avcodec_alloc_context3(codec);
            if (!codecContext) {
                qDebug() << "无法分配解码器上下文";
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 复制编解码参数到上下文
            if (avcodec_parameters_to_context(codecContext, formatContext->streams[streamIndex]->codecpar) < 0) {
                qDebug() << "无法复制编解码参数";
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 打开解码器
            if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                qDebug() << "无法打开解码器";
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 解码帧
            AVFrame *frame = av_frame_alloc();
            if (!frame) {
                qDebug() << "无法分配帧";
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            int ret = avcodec_send_packet(codecContext, packet);
            if (ret < 0) {
                qDebug() << "发送数据包到解码器失败";
                av_frame_free(&frame);
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            ret = avcodec_receive_frame(codecContext, frame);
            if (ret < 0) {
                qDebug() << "无法从解码器接收帧";
                av_frame_free(&frame);
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 转换为RGB格式
            SwsContext *swsContext = sws_getContext(
                frame->width, frame->height, (AVPixelFormat)frame->format,
                frame->width, frame->height, AV_PIX_FMT_RGB24,
                SWS_BILINEAR, nullptr, nullptr, nullptr
                );

            if (!swsContext) {
                qDebug() << "无法创建图像转换上下文";
                av_frame_free(&frame);
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 为RGB数据分配空间
            uint8_t *rgbData[4] = {nullptr};
            int rgbLinesize[4] = {0};

            if (av_image_alloc(rgbData, rgbLinesize, frame->width, frame->height, AV_PIX_FMT_RGB24, 1) < 0) {
                qDebug() << "无法分配RGB图像缓冲区";
                sws_freeContext(swsContext);
                av_frame_free(&frame);
                avcodec_free_context(&codecContext);
                av_packet_unref(packet);
                avformat_close_input(&formatContext);
                return false;
            }

            // 转换为RGB
            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, rgbData, rgbLinesize);

            // 创建QImage
            QImage rgbImage(frame->width, frame->height, QImage::Format_RGB888);

            // 复制RGB数据到QImage
            for (int y = 0; y < frame->height; y++) {
                memcpy(rgbImage.scanLine(y), rgbData[0] + y * rgbLinesize[0], rgbLinesize[0]);
            }

            // 保存为JPEG文件
            bool result = rgbImage.save(outputPath, "JPEG");

            // 清理资源
            av_freep(&rgbData[0]);
            sws_freeContext(swsContext);
            av_frame_free(&frame);
            avcodec_free_context(&codecContext);
            av_packet_unref(packet);
            avformat_close_input(&formatContext);

            return result;
        }
    }

    // 清理资源
    av_packet_unref(packet);
    avformat_close_input(&formatContext);

    return false;
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
            qry.prepare("SELECT * FROM path_monitor WHERE path_value = ?");
            qry.addBindValue(pickPath);
            if(qry.exec()) {
                if (qry.first()) {
                    MainFrame::showMessageBoxError("此路径已存在！");
                } else {
                    qry.prepare("INSERT INTO path_monitor (path_value) VALUES (?)");
                    qry.addBindValue(pickPath);
                    if(qry.exec()) {
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
        updateLibraryModelData();
    } else {
        QModelIndexList indexes = tableView_PathMonitor->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty()) {
            QModelIndex index = indexes.first();
            QString path = standardItemModel_PathMonitor.data(index, Qt::DisplayRole).toString();
            // qDebug() << "CentralWidget::handleSettingsPathMonitorButtonClick()->" << path;
            qry.prepare("DELETE FROM path_monitor WHERE path_value = ?");
            qry.addBindValue(path);
            if(!qry.exec()) {
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


