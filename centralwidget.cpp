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

    initPagePlaylistsUI();

    initPageLibraryUI();

    initPageStatisticsUI();

    initPageSettingsUI();
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
    // QPixmap pixmap{":/images/res/images/preview.jpg"};
    // pixmap.scaled(ui->label_Playing_DetailPic->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // ui->label_Playing_DetailPic->setPixmap(pixmap);
    // ui->label_Playing_DetailPic->setScaledContents(true);
    // int imgW = pixmap.width();
    // int imgH = pixmap.height();
    // detailPicScale = imgW / imgH;
    // ui->label_Playing_DetailPic->resize(imgW, imgH);
    // ui->label_Playing_DetailPic->move(0, 0);

    playMode = 1;
    playStatus = false;
    currentIndex = 0;
    prevIndex = 0;
    nextIndex = 0;

    QAudioOutput *audio_Output = new QAudioOutput(this);
    player.setAudioOutput(audio_Output);

    connect(ui->slider_Playing_Progress, &QSlider::sliderMoved, this, &CentralWidget::handleSliderProgressChanged);

    float initialVolume = 100.0f / 100.0f;
    player.audioOutput()->setVolume(initialVolume);
    ui->hSlider_Volume->setValue(100);
    connect(ui->hSlider_Volume, &QSlider::valueChanged, this, &CentralWidget::handleSliderVolumeChanged);

    connect(ui->button_Playing_Play, &QPushButton::clicked, this, &CentralWidget::handleButtonPlayingPlayClick);
    connect(ui->button_Playing_Prev, &QPushButton::clicked, this, &CentralWidget::handleButtonPlayingPlayPrevClick);
    connect(ui->button_Playing_Next, &QPushButton::clicked, this, &CentralWidget::handleButtonPlayingPlayNextClick);
    connect(ui->radioButton_ListLoop, &QRadioButton::clicked, this, &CentralWidget::handleRadioButtonPlayModeClick);
    connect(ui->radioButton_RandomLoop, &QRadioButton::clicked, this, &CentralWidget::handleRadioButtonPlayModeClick);
    connect(ui->radioButton_SingleLoop, &QRadioButton::clicked, this, &CentralWidget::handleRadioButtonPlayModeClick);

    connect(&player, &QMediaPlayer::positionChanged, this, &CentralWidget::handlePositionChanged);
    connect(&player, &QMediaPlayer::durationChanged, this, &CentralWidget::handleDurationChanged);
    connect(&player, &QMediaPlayer::sourceChanged, this, &CentralWidget::handleSourceChanged);
    connect(&player, &QMediaPlayer::playbackStateChanged, this,  &CentralWidget::handleStateChanged);
    connect(&player, &QMediaPlayer::metaDataChanged, this,  &CentralWidget::handleMetaDataChanged);

    ui->listWidget_Lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget_Lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget_Lyrics->setSpacing(5);

    updatecurrentPlaylist();
}

void CentralWidget::updatecurrentPlaylist()
{
    currentPlaylist.clear();
    sql = QString("SELECT s.song_id, s.song_title, s.song_artist, s.song_album, s.song_lyrics, s.song_size, s.song_duration, s.cover_path, s.song_path FROM songs s JOIN playlist_current pc ON s.song_id = pc.song_id");
    if (qry.exec(sql)) {
        while (qry.next()) {
            SongInfo item;
            item.song_id = qry.value(0).toLongLong();
            item.song_title = qry.value(1).toString();
            item.song_artist = qry.value(2).toString();
            item.song_album = qry.value(3).toString();
            item.song_lyrics = qry.value(4).toString();
            item.song_size = qry.value(5).toLongLong();
            item.song_duration = qry.value(6).toLongLong();
            item.cover_path = qry.value(7).toString();
            item.song_path = qry.value(8).toString();
            currentPlaylist.append(item);
        }
    }
    currentIndex = 0;

    if (!currentPlaylist.isEmpty()) {
        SongInfo song = currentPlaylist[currentIndex];

        player.setSource(QUrl::fromLocalFile(song.song_path));

        loadLyrics(song.song_lyrics);

        ui->label_Playing_Title->setText(song.song_title);
        ui->label_Playing_Artist->setText(song.song_artist);

        QPixmap cover(song.cover_path);
        if (!cover.isNull()) {
            QPixmap pixmap(cover);
            pixmap.scaled(ui->label_Playing_DetailPic->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui->label_Playing_DetailPic->setPixmap(cover);
            ui->label_Playing_DetailPic->setScaledContents(true);
            int imgW = pixmap.width();
            int imgH = pixmap.height();
            detailPicScale = imgW / imgH;
            ui->label_Playing_DetailPic->resize(imgW, imgH);
            ui->label_Playing_DetailPic->move(0, 0);
        } else {
            ui->label_Playing_DetailPic->setPixmap(QPixmap(":/images/res/images/preview.jpg"));
        }

        if (currentPlaylist.size() > 1) {
            prevIndex = (currentIndex - 1 + currentPlaylist.size()) % currentPlaylist.size();
            nextIndex = (currentIndex + 1) % currentPlaylist.size();
        }
    }
}

void CentralWidget::playSong()
{
    if (currentPlaylist.isEmpty()) {
        MainFrame::showMessageBoxInfo("当前列表暂无歌曲");
        return;
    }

    // 获取当前播放的歌曲信息
    SongInfo song = currentPlaylist[currentIndex];

    qint64 song_id = song.song_id;
    qry.prepare("INSERT INTO songs_statistics (song_id, play_time) VALUES (?, ?)");
    qry.addBindValue(song_id);
    qry.addBindValue(QDateTime::currentSecsSinceEpoch());
    if (!qry.exec()) {
        qDebug() << "CentralWidget::playSong()" << qry.lastError().text();
    }


    // 设置播放器的音频源
    player.setSource(QUrl::fromLocalFile(song.song_path));

    loadLyrics(song.song_lyrics);

    // 开始播放
    player.play();
    playStatus = true;

    // 更新播放按钮图标
    ui->button_Playing_Play->setIcon(QIcon(":/icons/res/icons/playing_pause.png"));

    // 更新 UI 显示
    ui->label_Playing_Title->setText(song.song_title);
    ui->label_Playing_Artist->setText(song.song_artist);

    // 更新封面图片
    QPixmap cover(song.cover_path);
    if (!cover.isNull()) {
        QPixmap pixmap(cover);
        pixmap.scaled(ui->label_Playing_DetailPic->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->label_Playing_DetailPic->setPixmap(cover);
        ui->label_Playing_DetailPic->setScaledContents(true);
        int imgW = pixmap.width();
        int imgH = pixmap.height();
        detailPicScale = imgW / imgH;
        ui->label_Playing_DetailPic->resize(imgW, imgH);
        ui->label_Playing_DetailPic->move(0, 0);
    } else {
        ui->label_Playing_DetailPic->setPixmap(QPixmap(":/images/res/images/preview.jpg"));
    }

    // 计算 prevIndex 和 nextIndex
    if (currentPlaylist.size() > 1) {
        prevIndex = (currentIndex - 1 + currentPlaylist.size()) % currentPlaylist.size();
        nextIndex = (currentIndex + 1) % currentPlaylist.size();
    }
}

QList<LyricLine> CentralWidget::lyricsParser(const QString &lyrics)
{
    QList<LyricLine> lyricList;
    QRegularExpression regex(R"(\[(\d+):(\d+\.\d+)\](.*))"); // 解析 [分钟:秒.毫秒] 格式
    QRegularExpressionMatch match;

    QStringList lines = lyrics.split("\n");  // 按行拆分歌词
    for (const QString &line : lines) {
        match = regex.match(line);
        if (match.hasMatch()) {
            int minutes = match.captured(1).toInt();
            float seconds = match.captured(2).toFloat();
            qint64 timeMs = (minutes * 60 + seconds) * 1000; // 转换为毫秒
            QString text = match.captured(3).trimmed(); // 获取歌词内容

            lyricList.append({timeMs, text});
        }
    }
    return lyricList;
}

void CentralWidget::updateLyrics(qint64 position)
{
    if (currentLyrics.isEmpty()) return;

    for (int i = 0; i < currentLyrics.size(); ++i) {
        if (i < currentLyrics.size() - 1) {
            if (position >= currentLyrics[i].time && position < currentLyrics[i + 1].time) {
                setLyricHighlight(i);
                break;
            }
        } else { // 最后一行
            if (position >= currentLyrics[i].time) {
                setLyricHighlight(i);
                break;
            }
        }
    }
}

void CentralWidget::setLyricHighlight(int index)
{
    for (int i = 0; i < ui->listWidget_Lyrics->count(); ++i) {
        QListWidgetItem *item = ui->listWidget_Lyrics->item(i);
        if (i == index) {
            item->setForeground(QBrush(Qt::black)); // 当前行高亮
            item->setFont(QFont("Microsoft YaHei", 12, QFont::Bold)); // 加粗字体
        } else {
            item->setForeground(QBrush(Qt::black));
            item->setFont(QFont("Microsoft YaHei", 10)); // 恢复正常字体
        }
        // 让歌词文本居中
        item->setTextAlignment(Qt::AlignCenter);
    }

    ui->listWidget_Lyrics->scrollToItem(ui->listWidget_Lyrics->item(index), QAbstractItemView::PositionAtCenter);
}

void CentralWidget::loadLyrics(const QString &lyricText)
{
    currentLyrics = lyricsParser(lyricText);
    ui->listWidget_Lyrics->clear();

    for (const auto &line : currentLyrics) {
        QListWidgetItem *item = new QListWidgetItem(line.text);
        item->setTextAlignment(Qt::AlignCenter);
        item->setForeground(QBrush(Qt::black));
        item->setFont(QFont("Microsoft YaHei", 10));
        ui->listWidget_Lyrics->addItem(item);
    }
}

void CentralWidget::handleRadioButtonPlayModeClick()
{
    if (ui->radioButton_SingleLoop->isChecked()) {
        playMode = 0;
    } else if (ui->radioButton_ListLoop->isChecked()) {
        playMode = 1;
    } else if (ui->radioButton_RandomLoop->isChecked()) {
        playMode = 2;
    }
}

void CentralWidget::handleButtonPlayingPlayClick()
{
    if (currentPlaylist.isEmpty()) {
        MainFrame::showMessageBoxInfo("当前列表暂无歌曲");
        return;
    }

    if (player.playbackState() == QMediaPlayer::PlayingState) {
        player.pause();
        playStatus = false;
        ui->button_Playing_Play->setIcon(QIcon(":/icons/res/icons/playing_play.png"));
    } else {
        player.play();
        playStatus = true;
        ui->button_Playing_Play->setIcon(QIcon(":/icons/res/icons/playing_pause.png"));
    }
}

void CentralWidget::handleButtonPlayingPlayPrevClick()
{
    if (currentPlaylist.isEmpty()) {
        MainFrame::showMessageBoxInfo("当前列表暂无歌曲");
        return;
    }

    currentIndex = prevIndex;
    playSong();
}

void CentralWidget::handleButtonPlayingPlayNextClick()
{
    if (currentPlaylist.isEmpty()) {
        MainFrame::showMessageBoxInfo("当前列表暂无歌曲");
        return;
    }

    currentIndex = nextIndex;
    playSong();
}

void CentralWidget::handleStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState) {
        if (playMode == 0) { // 单曲循环
            playSong();
        } else if (playMode == 1) { // 列表循环
            handleButtonPlayingPlayNextClick();
        } else if (playMode == 2) { // 随机播放
            currentIndex = QRandomGenerator::global()->bounded(currentPlaylist.size());
            playSong();
        }
    }
}

void CentralWidget::handleSourceChanged(const QUrl &media)
{

}

void CentralWidget::handleDurationChanged(qint64 duration)
{
    if (duration > 0) {
        ui->slider_Playing_Progress->setMinimum(0);  // 进度条最小值
        ui->slider_Playing_Progress->setMaximum(static_cast<int>(duration));  // 进度条最大值
    }

    // 显示总时长
    QTime time(0, 0, 0);
    time = time.addMSecs(duration);
    ui->label_Playing_TotalTime->setText(time.toString("mm:ss"));
}

void CentralWidget::handlePositionChanged(qint64 position)
{
    ui->slider_Playing_Progress->setValue(static_cast<int>(position));

    // 转换时间格式（00:00）
    QTime time(0, 0, 0);
    time = time.addMSecs(position);
    ui->label_Playing_CurTime->setText(time.toString("mm:ss"));

    updateLyrics(position);
}

void CentralWidget::handleMetaDataChanged()
{

}

void CentralWidget::handleSliderVolumeChanged(int value)
{
    if (player.audioOutput()) {
        float volume = static_cast<float>(value) / 100.0f;  // 映射到 0.0 ~ 1.0
        player.audioOutput()->setVolume(volume);
    }
}

void CentralWidget::handleSliderProgressChanged(int value)
{
    player.setPosition(static_cast<qint64>(value));
}


//Page_Playlists
void CentralWidget::initPagePlaylistsUI()
{
    tableView_Playlists = ui->tableView_Playlists;
    tableView_Playlists->setModel(&standardItemModel_Playlists);
    standardItemModel_Playlists.setColumnCount(5);
    standardItemModel_Playlists.setHeaderData(0, Qt::Horizontal, "Title");
    standardItemModel_Playlists.setHeaderData(1, Qt::Horizontal, "Artist");
    standardItemModel_Playlists.setHeaderData(2, Qt::Horizontal, "Album");
    standardItemModel_Playlists.setHeaderData(3, Qt::Horizontal, "Size");
    standardItemModel_Playlists.setHeaderData(4, Qt::Horizontal, "Duration");
    tableView_Playlists->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_Playlists->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView_Playlists->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableView_Playlists->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    tableView_Playlists->horizontalHeader()->resizeSection(3, 100);
    tableView_Playlists->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    tableView_Playlists->horizontalHeader()->resizeSection(4, 100);
    tableView_Playlists->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView_Playlists->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_Playlists->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_Playlists->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // tableView_Library->verticalHeader()->setVisible(false);
    tableView_Playlists->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableView_Playlists->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView_Playlists, &QTableView::customContextMenuRequested, this, &CentralWidget::playlistsTableViewMenu);

    updatePlaylistsNaviButton();

    updatePlaylistsModelData();
}

void CentralWidget::updatePlaylistsNaviButton()
{
    foreach (QPushButton *playlistsNaviButtonPtr, ui->scrollAreaWidget_Playlists->findChildren<QPushButton *>()) {
        QLayout * layout = ui->scrollAreaWidget_Playlists->layout();
        layout->removeWidget(playlistsNaviButtonPtr);
        delete playlistsNaviButtonPtr;
    }

    QPushButton* buttonPLCurrentPtr = new QPushButton("当前列表", ui->scrollAreaWidget_Playlists);
    buttonPLCurrentPtr->setObjectName("button_Playlist_current");
    buttonPLCurrentPtr->setCheckable(true);
    buttonPLCurrentPtr->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(buttonPLCurrentPtr, &QPushButton::customContextMenuRequested, this, &CentralWidget::playlistsNaviButtonMenu);
    QLayout * layout = ui->scrollAreaWidget_Playlists->layout();
    layout->removeItem(ui->vSpacer_Playlists);
    layout->addWidget(buttonPLCurrentPtr);
    layout->addItem(ui->vSpacer_Playlists);
    connect(buttonPLCurrentPtr, &QPushButton::clicked, this, &CentralWidget::handlePlaylistsNaviButtonClick);
    buttonPLCurrentPtr->setChecked(true);

    QMap<qint64, QString> pl_IDNAME_Map;
    qry.prepare("SELECT * FROM playlists");
    if (qry.exec()) {
        while (qry.next()) {
            pl_IDNAME_Map[qry.value(0).toLongLong()] = qry.value(1).toString();
        }
    }

    QMapIterator<qint64, QString> it(pl_IDNAME_Map);
    while (it.hasNext()) {
        it.next();
        QString str = it.value();
        QPushButton* newButtonPtr = new QPushButton(str, ui->scrollAreaWidget_Playlists);
        newButtonPtr->setObjectName("button_Playlist_" + QString::number(it.key()));
        newButtonPtr->setCheckable(true);
        newButtonPtr->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(newButtonPtr, &QPushButton::customContextMenuRequested, this, &CentralWidget::playlistsNaviButtonMenu);
        layout->removeItem(ui->vSpacer_Playlists);
        layout->addWidget(newButtonPtr);
        layout->addItem(ui->vSpacer_Playlists);
        connect(newButtonPtr, &QPushButton::clicked, this, &CentralWidget::handlePlaylistsNaviButtonClick);
    }
}

void CentralWidget::updatePlaylistsModelData()
{
    standardItemModel_Playlists.removeRows(0, standardItemModel_Playlists.rowCount());

    QString pl_id;
    foreach (QPushButton *playlistsNaviButtonPtr, ui->scrollAreaWidget_Playlists->findChildren<QPushButton *>()) {
        if (playlistsNaviButtonPtr->isChecked()) {
            pl_id = playlistsNaviButtonPtr->objectName().remove("button_Playlist_");
        }
    }

    sql = QString("SELECT songs.song_id, songs.song_title, songs.song_artist, songs.song_album, songs.song_size, songs.song_duration FROM songs INNER JOIN %1 ON songs.song_id = %2")
              .arg("playlist_" + pl_id, "playlist_" + pl_id + ".song_id");

    if (qry.exec(sql)) {
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
            standardItemModel_Playlists.setItem(cnt, 0, idTitleItem);
            standardItemModel_Playlists.setItem(cnt, 1, new QStandardItem(artist));
            standardItemModel_Playlists.setItem(cnt, 2, new QStandardItem(album));
            QStandardItem* sizeItem = new QStandardItem(size);
            sizeItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Playlists.setItem(cnt, 3, sizeItem);
            QStandardItem* durationItem = new QStandardItem(duration);
            durationItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_Playlists.setItem(cnt, 4, durationItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::updatePlaylistsModelData()->查询失败：" << qry.lastError().text();
    }
}

void CentralWidget::handlePlaylistsNaviButtonClick()
{
    QPushButton *clickedButtonPtr = (QPushButton *)sender();

    foreach (QPushButton *playlistsNaviButtonPtr, ui->scrollAreaWidget_Playlists->findChildren<QPushButton *>()) {
        if (playlistsNaviButtonPtr == clickedButtonPtr) {
            playlistsNaviButtonPtr->setChecked(true);
        } else {
            playlistsNaviButtonPtr->setChecked(false);
        }
    }

    updatePlaylistsModelData();
}

void CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    // qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << clickedButton->objectName();

    QMenu contextMenu;
    contextMenu.addAction("清空当前列表");

    if (clickedButton->objectName() != "button_Playlist_current") {
        contextMenu.addAction("播放此列表");
        contextMenu.addAction("删除此列表");
    }

    QAction *selectedAction = contextMenu.exec(clickedButton->mapToGlobal(pos));
    if (!selectedAction) return;

    if (selectedAction->text() == "清空当前列表") {
        QSqlDatabase::database().transaction();
        sql = QString("DELETE FROM %1").arg("playlist_" + clickedButton->objectName().remove("button_Playlist_"));
        if (!qry.exec(sql)) {
            qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
        QSqlDatabase::database().commit();
        updatePlaylistsModelData();
        if (clickedButton->objectName() != "button_Playlist_current") {
            updatecurrentPlaylist();
        }
    } else if (selectedAction->text() == "播放此列表") {
        QSqlDatabase::database().transaction();
        qry.prepare("DELETE FROM playlist_current");
        if (!qry.exec()) {
            qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
        sql = QString("INSERT INTO playlist_current (song_id) SELECT song_id FROM %1").arg("playlist_" + clickedButton->objectName().remove("button_Playlist_"));
        if (!qry.exec(sql)) {
            qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
        QSqlDatabase::database().commit();
        updatePlaylistsModelData();
        updatecurrentPlaylist();
    } else if (selectedAction->text() == "删除此列表") {
        QSqlDatabase::database().transaction();
        qry.prepare("DELETE FROM playlists WHERE pl_id = ?");
        qry.addBindValue(clickedButton->objectName().remove("button_Playlist_"));
        if (!qry.exec()) {
            qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
        sql = QString("DROP TABLE %1").arg("playlist_" + clickedButton->objectName().remove("button_Playlist_"));
        if (!qry.exec(sql)) {
            qDebug() << "CentralWidget::playlistsNaviButtonMenu(const QPoint &pos)->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
        QSqlDatabase::database().commit();
        updatePlaylistsNaviButton();
        updatePlaylistsModelData();
    }
}

void CentralWidget::playlistsTableViewMenu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("从列表中删除所选歌曲");

    QAction *selectedAction = contextMenu.exec(tableView_Playlists->mapToGlobal(pos));
    if (!selectedAction) return;

    if (selectedAction->text() == "从列表中删除所选歌曲") {
        QString pl_id;
        foreach (QPushButton *playlistsNaviButtonPtr, ui->scrollAreaWidget_Playlists->findChildren<QPushButton *>()) {
            if (playlistsNaviButtonPtr->isChecked()) {
                pl_id = "playlist_" + playlistsNaviButtonPtr->objectName().remove("button_Playlist_");
            }
        }

        QItemSelectionModel *selectionModel = tableView_Playlists->selectionModel();
        QModelIndexList selectedIndexes = selectionModel->selectedRows();

        for (const QModelIndex &index : selectedIndexes) {
            int row = index.row();
            QVariant songIdData = standardItemModel_Playlists.item(row, 0)->data(Qt::UserRole);
            if (songIdData.isValid()) {
                qint64 song_id = songIdData.toLongLong();
                sql = QString("DELETE FROM %1 WHERE song_id = %2").arg(pl_id).arg(song_id);
                if (!qry.exec(sql)) {
                    qDebug() << "CentralWidget::playlistsTableViewMenu(const QPoint &pos)" << qry.lastError().text();
                }
            }
        }
        updatePlaylistsModelData();
        updatecurrentPlaylist();
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
    connect(ui->button_Library_NewPL, &QPushButton::clicked, this, &CentralWidget::handleButtonLibraryNewPLClick);
    connect(ui->button_Library_Add2PL, &QPushButton::clicked, this, &CentralWidget::handleButtonLibraryAdd2PLClick);
    connect(ui->button_Library_DelSong, &QPushButton::clicked, this, &CentralWidget::handleButtonLibraryDelSongClick);
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

void CentralWidget::handleButtonLibraryNewPLClick()
{
    bool ok = false;
    QString newPLName =  MainFrame::showInputBox(ok, "请输入歌单名称");
    if(ok && !newPLName.isEmpty())
    {
        QSqlDatabase::database().transaction();
        qry.prepare("SELECT * FROM playlists WHERE pl_name = ?");
        qry.addBindValue(newPLName);
        if (qry.exec() && qry.first()) {
            MainFrame::showMessageBoxError("歌单已存在");
            // qDebug() << "CentralWidget::handleButtonLibraryNewPLClick()->" << "歌单已存在";
            return;
        }

        qint64 pl_id = 1;
        qry.prepare("SELECT seq FROM sqlite_sequence WHERE name = 'playlists'");
        if (qry.exec() && qry.first() && !qry.value(0).isNull()) {
            pl_id = qry.value(0).toLongLong() + 1;
            // qDebug() << pl_id;
        }

        qry.prepare("INSERT INTO playlists (pl_name) VALUES (?)");
        qry.addBindValue(newPLName);
        if(!qry.exec()) {
            qDebug() << "CentralWidget::handleButtonLibraryNewPLClick()->" << "Insert " << newPLName << " to playlists error!" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        QString tableName = QString("playlist_%1").arg(pl_id);
        QString createTableSQL = QString(
                                     "CREATE TABLE IF NOT EXISTS [%1] ("
                                     "[song_index] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "
                                     "[song_id] INTEGER NOT NULL UNIQUE);"
                                     ).arg(tableName);
        if(!qry.exec(createTableSQL)) {
            qDebug() << "CentralWidget::handleButtonLibraryNewPLClick()->" << "Create table" << tableName << " error!" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        QItemSelectionModel *selectionModel = tableView_Library->selectionModel();
        QModelIndexList selectedIndexes = selectionModel->selectedRows();

        for (const QModelIndex &index : selectedIndexes) {
            int row = index.row();
            QVariant songIdData = standardItemModel_Library.item(row, 0)->data(Qt::UserRole);
            if (songIdData.isValid()) {
                qint64 song_id = songIdData.toLongLong();
                // qDebug() << "选中的行:" << row << " song_id:" << song_id;
                sql = QString("SELECT * FROM %1 WHERE song_id = %2").arg(tableName).arg(song_id);
                if (qry.exec(sql) && qry.first()) {
                    qDebug() << "CentralWidget::handleButtonLibraryNewPLClick()->" << song_id << "已存在" << qry.lastError().text();
                    continue;
                }
                sql = QString("INSERT INTO %1 (song_id) VALUES (%2)").arg(tableName).arg(song_id);
                if (!qry.exec(sql)) {
                    qDebug() << "CentralWidget::handleButtonLibraryNewPLClick()->" << song_id << "插入失败" << qry.lastError().text();
                }
            }
        }

        QSqlDatabase::database().commit();
    }
    updatePlaylistsNaviButton();
    updatePlaylistsModelData();
}

void CentralWidget::handleButtonLibraryAdd2PLClick()
{
    bool ok = false;

    QStringList pl_lists;
    qry.prepare("SELECT pl_name FROM playlists");
    if (qry.exec() && qry.first()) {
        pl_lists << qry.value(0).toString();
        while (qry.next()) {
            pl_lists << qry.value(0).toString();
        }

        QString PL = MainFrame::showInputBox(ok, "添加至歌单", 1, pl_lists.join("|"));
        // qDebug() << PL;

        qint64 pl_id = 0;
        qry.prepare("SELECT pl_id FROM playlists WHERE pl_name = ?");
        qry.addBindValue(PL);
        if (qry.exec() && qry.first()) {
            pl_id = qry.value(0).toLongLong();
        } else {
            qDebug() << "CentralWidget::handleButtonLibraryAdd2PLClick()->" << qry.lastError().text();
            return;
        }

        QItemSelectionModel *selectionModel = tableView_Library->selectionModel();
        QModelIndexList selectedIndexes = selectionModel->selectedRows();

        for (const QModelIndex &index : selectedIndexes) {
            int row = index.row();
            QVariant songIdData = standardItemModel_Library.item(row, 0)->data(Qt::UserRole);
            if (songIdData.isValid()) {
                qint64 song_id = songIdData.toLongLong();
                // qDebug() << "选中的行:" << row << " song_id:" << song_id;
                sql = QString("SELECT * FROM %1 WHERE song_id = %2").arg("playlist_" + QString::number(pl_id)).arg(song_id);
                if (qry.exec(sql) && qry.first()) {
                    // qDebug() << "CentralWidget::handleButtonLibraryAdd2PLClick()->" << song_id << ": 已存在" << qry.lastError().text();
                    continue;
                }
                sql = QString("INSERT INTO %1 (song_id) VALUES (%2)").arg("playlist_" + QString::number(pl_id)).arg(song_id);
                if (!qry.exec(sql)) {
                    qDebug() << "CentralWidget::handleButtonLibraryAdd2PLClick()->" << song_id << ": 插入失败" << qry.lastError().text();
                }
            }
        }
    } else {
        MainFrame::showInputBox(ok, "添加至歌单", 1, "无可添加歌单");
    }
    updatePlaylistsNaviButton();
    updatePlaylistsModelData();
}

void CentralWidget::handleButtonLibraryDelSongClick()
{
    QString msg = "是否从曲库和所有歌单中删除所选歌曲？";
    int ret = MainFrame::showMessageBoxQuestion(msg);
    if (ret) {
        QVector<qint64> plIDVector;
        qry.prepare("SELECT pl_id FROM playlists");
        if (qry.exec()) {
            while (qry.next()) {
                plIDVector.push_back(qry.value(0).toLongLong());
            }
        } else {
            qDebug() << "CentralWidget::handleButtonLibraryDelSongClick()->" << qry.lastError().text();
        }

        QItemSelectionModel *selectionModel = tableView_Library->selectionModel();
        QModelIndexList selectedIndexes = selectionModel->selectedRows();

        for (const QModelIndex &index : selectedIndexes) {
            int row = index.row();
            QVariant songIdData = standardItemModel_Library.item(row, 0)->data(Qt::UserRole);
            if (songIdData.isValid()) {
                qint64 song_id = songIdData.toLongLong();

                QSqlDatabase::database().transaction();

                qry.prepare("DELETE FROM playlist_current WHERE song_id = ?");
                qry.addBindValue(song_id);
                if (!qry.exec()) {
                    qDebug() << "CentralWidget::handleButtonLibraryDelSongClick()->" << qry.lastError().text();
                    QSqlDatabase::database().rollback();
                    continue;
                }

                for (qint64 plID : plIDVector) {
                    sql = QString("DELETE FROM %1 WHERE song_id = %2").arg("playlist_" + QString::number(plID)).arg(song_id);
                    if (!qry.exec(sql)) {
                        QSqlDatabase::database().rollback();
                        qDebug() << "CentralWidget::handleButtonLibraryDelSongClick()->" << qry.lastError().text();
                        continue;
                    }
                }

                QString coverPath;
                qry.prepare("SELECT cover_path FROM songs WHERE song_id = ?");
                qry.addBindValue(song_id);
                if (!qry.exec()) {
                    qDebug() << "CentralWidget::handleButtonLibraryDelSongClick()->" << qry.lastError().text();
                    continue;
                }
                qry.first();
                coverPath = qry.value(0).toString();

                sql = QString("DELETE FROM songs WHERE song_id = %1").arg(song_id);
                if (!qry.exec(sql)) {
                    QSqlDatabase::database().rollback();
                    qDebug() << "CentralWidget::handleButtonLibraryDelSongClick()->" << qry.lastError().text();
                    continue;
                }

                QString defaultPath = QCoreApplication::applicationDirPath() + "/res/cover/cover_default.jpg";
                if (coverPath != defaultPath) {
                    // qDebug() << coverPath;
                    QFile::remove(coverPath);
                }

                QSqlDatabase::database().commit();
            }
        }
    }
    updateLibraryModelData();
    updatePlaylistsNaviButton();
    updatePlaylistsModelData();
}


//Page_Statistics
void CentralWidget::initPageStatisticsUI()
{
    tableView_StatisticsDay = ui->tableView_Statistics_Day;
    tableView_StatisticsDay->setModel(&standardItemModel_StatisticsDay);
    standardItemModel_StatisticsDay.setColumnCount(2);
    standardItemModel_StatisticsDay.setHeaderData(0, Qt::Horizontal, "Title");
    standardItemModel_StatisticsDay.setHeaderData(1, Qt::Horizontal, "Count");
    tableView_StatisticsDay->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_StatisticsDay->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView_StatisticsDay->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_StatisticsDay->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_StatisticsDay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_StatisticsDay->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // tableView_StatisticsDay->verticalHeader()->setVisible(false);
    tableView_StatisticsDay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableView_StatisticsWeek = ui->tableView_Statistics_Week;
    tableView_StatisticsWeek->setModel(&standardItemModel_StatisticsWeek);
    standardItemModel_StatisticsWeek.setColumnCount(2);
    standardItemModel_StatisticsWeek.setHeaderData(0, Qt::Horizontal, "Title");
    standardItemModel_StatisticsWeek.setHeaderData(1, Qt::Horizontal, "Count");
    tableView_StatisticsWeek->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_StatisticsWeek->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView_StatisticsWeek->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_StatisticsWeek->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_StatisticsWeek->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_StatisticsWeek->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // tableView_StatisticsWeek->verticalHeader()->setVisible(false);
    tableView_StatisticsWeek->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableView_StatisticsMonth = ui->tableView_Statistics_Month;
    tableView_StatisticsMonth->setModel(&standardItemModel_StatisticsMonth);
    standardItemModel_StatisticsMonth.setColumnCount(2);
    standardItemModel_StatisticsMonth.setHeaderData(0, Qt::Horizontal, "Title");
    standardItemModel_StatisticsMonth.setHeaderData(1, Qt::Horizontal, "Count");
    tableView_StatisticsMonth->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_StatisticsMonth->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView_StatisticsMonth->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_StatisticsMonth->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_StatisticsMonth->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_StatisticsMonth->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // tableView_StatisticsMonth->verticalHeader()->setVisible(false);
    tableView_StatisticsMonth->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(ui->button_Statistics_Day, &QPushButton::clicked, this, &CentralWidget::handleStatisticsButtonClick);
    connect(ui->button_Statistics_Week, &QPushButton::clicked, this, &CentralWidget::handleStatisticsButtonClick);
    connect(ui->button_Statistics_Month, &QPushButton::clicked, this, &CentralWidget::handleStatisticsButtonClick);

    updateStatisticsModelData();
}

void CentralWidget::updateStatisticsModelData()
{
    standardItemModel_StatisticsDay.removeRows(0, standardItemModel_StatisticsDay.rowCount());
    sql = QString(
        "SELECT s.song_id, s.song_title, COUNT(ss.song_id) AS play_count "
        "FROM songs_statistics AS ss "
        "JOIN songs AS s ON ss.song_id = s.song_id "
        "WHERE ss.play_time >= CAST(STRFTIME('%s', 'now', '-1 day') AS INTEGER) "
        "GROUP BY s.song_id, s.song_title "
        "ORDER BY play_count DESC "
        "LIMIT 20;"
        );
    if (qry.exec(sql)) {
        int cnt = 0;
        while (qry.next()) {
            QString song_title = qry.value(1).toString();
            QStandardItem* titleItem = new QStandardItem(song_title);
            titleItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsDay.setItem(cnt, 0, titleItem);
            QString count = qry.value(2).toString();
            QStandardItem* countItem = new QStandardItem(count);
            countItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsDay.setItem(cnt, 1, countItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::updatePlaylistsModelData()->查询失败：" << qry.lastError().text();
    }

    standardItemModel_StatisticsWeek.removeRows(0, standardItemModel_StatisticsWeek.rowCount());
    sql = QString(
        "SELECT s.song_id, s.song_title, COUNT(ss.song_id) AS play_count "
        "FROM songs_statistics AS ss "
        "JOIN songs AS s ON ss.song_id = s.song_id "
        "WHERE ss.play_time >= CAST(STRFTIME('%s', 'now', '-7 days') AS INTEGER) "
        "GROUP BY s.song_id, s.song_title "
        "ORDER BY play_count DESC "
        "LIMIT 20;"
        );
    if (qry.exec(sql)) {
        int cnt = 0;
        while (qry.next()) {
            QString song_title = qry.value(1).toString();
            QStandardItem* titleItem = new QStandardItem(song_title);
            titleItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsWeek.setItem(cnt, 0, titleItem);
            QString count = qry.value(2).toString();
            QStandardItem* countItem = new QStandardItem(count);
            countItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsWeek.setItem(cnt, 1, countItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::updatePlaylistsModelData()->查询失败：" << qry.lastError().text();
    }

    standardItemModel_StatisticsMonth.removeRows(0, standardItemModel_StatisticsMonth.rowCount());
    sql = QString(
        "SELECT s.song_id, s.song_title, COUNT(ss.song_id) AS play_count "
        "FROM songs_statistics AS ss "
        "JOIN songs AS s ON ss.song_id = s.song_id "
        "WHERE ss.play_time >= CAST(STRFTIME('%s', 'now', '-30 days') AS INTEGER) "
        "GROUP BY s.song_id, s.song_title "
        "ORDER BY play_count DESC "
        "LIMIT 20;"
        );
    if (qry.exec(sql)) {
        int cnt = 0;
        while (qry.next()) {
            QString song_title = qry.value(1).toString();
            QStandardItem* titleItem = new QStandardItem(song_title);
            titleItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsMonth.setItem(cnt, 0, titleItem);
            QString count = qry.value(2).toString();
            QStandardItem* countItem = new QStandardItem(count);
            countItem->setTextAlignment(Qt::AlignCenter);
            standardItemModel_StatisticsMonth.setItem(cnt, 1, countItem);
            cnt++;
        }
    } else {
        qDebug() << "CentralWidget::updatePlaylistsModelData()->查询失败：" << qry.lastError().text();
    }
}

void CentralWidget::handleStatisticsButtonClick()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    if (clickedButton == ui->button_Statistics_Day) {
        QSqlDatabase::database().transaction();

        qry.prepare("DELETE FROM playlist_current");
        if (!qry.exec()) {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        sql = QString(
            "SELECT song_id FROM songs_statistics "
            "WHERE play_time >= CAST(STRFTIME('%s', 'now', '-1 day') AS INTEGER) "
            "GROUP BY song_id "
            "ORDER BY COUNT(song_id) DESC "
            "LIMIT 20;"
            );
        QList<qint64> songIds;
        if (qry.exec(sql)) {
            while (qry.next()) {
                songIds.append(qry.value(0).toLongLong());
            }
        } else {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        qry.prepare("INSERT INTO playlist_current (song_id) VALUES (?)");
        for (qint64 song_id : songIds) {
            qry.addBindValue(song_id);
            if (!qry.exec()) {
                qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
                QSqlDatabase::database().rollback();
                return;
            }
        }

        QSqlDatabase::database().commit();
        updatePlaylistsModelData();
        updatecurrentPlaylist();
    } else if (clickedButton == ui->button_Statistics_Week) {
        QSqlDatabase::database().transaction();

        qry.prepare("DELETE FROM playlist_current");
        if (!qry.exec()) {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        sql = QString(
            "SELECT song_id FROM songs_statistics "
            "WHERE play_time >= CAST(STRFTIME('%s', 'now', '-7 days') AS INTEGER) "
            "GROUP BY song_id "
            "ORDER BY COUNT(song_id) DESC "
            "LIMIT 20;"
            );
        QList<qint64> songIds;
        if (qry.exec(sql)) {
            while (qry.next()) {
                songIds.append(qry.value(0).toLongLong());
            }
        } else {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        qry.prepare("INSERT INTO playlist_current (song_id) VALUES (?)");
        for (qint64 song_id : songIds) {
            qry.addBindValue(song_id);
            if (!qry.exec()) {
                qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
                QSqlDatabase::database().rollback();
                return;
            }
        }

        QSqlDatabase::database().commit();
        updatePlaylistsModelData();
        updatecurrentPlaylist();
    } else if (clickedButton == ui->button_Statistics_Month) {
        QSqlDatabase::database().transaction();

        qry.prepare("DELETE FROM playlist_current");
        if (!qry.exec()) {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        sql = QString(
            "SELECT song_id FROM songs_statistics "
            "WHERE play_time >= CAST(STRFTIME('%s', 'now', '-30 days') AS INTEGER) "
            "GROUP BY song_id "
            "ORDER BY COUNT(song_id) DESC "
            "LIMIT 20;"
            );
        QList<qint64> songIds;
        if (qry.exec(sql)) {
            while (qry.next()) {
                songIds.append(qry.value(0).toLongLong());
            }
        } else {
            qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }

        qry.prepare("INSERT INTO playlist_current (song_id) VALUES (?)");
        for (qint64 song_id : songIds) {
            qry.addBindValue(song_id);
            if (!qry.exec()) {
                qDebug() << "CentralWidget::handleStatisticsButtonClick()->" << qry.lastError().text();
                QSqlDatabase::database().rollback();
                return;
            }
        }

        QSqlDatabase::database().commit();
        updatePlaylistsModelData();
        updatecurrentPlaylist();
    }
}


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
        qry.prepare("SELECT seq FROM sqlite_sequence WHERE name = 'songs'");
        if (qry.exec() && qry.first() && !qry.value(0).isNull()) {
            song_id = qry.value(0).toLongLong() + 1;
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

