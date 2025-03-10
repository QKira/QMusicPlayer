#include "parsemusic.h"

ParseMusic::ParseMusic(const QString &filepath) : filepath(filepath) {}

void ParseMusic::run()
{
    QString connectionName = QString("ParseMusic_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("D:/000_QtPro/QMusicPlayer/res/db/data.db");

    if (!db.open()) {
        qDebug() << "ParseMusic::run()-> 数据库打开失败:" << db.lastError().text();
        return;
    }

    {
        QSqlQuery query(db);
        QString sql = QString("SELECT * FROM %1 WHERE song_path = '%2'").arg("songs").arg(filepath);
        if (query.exec(sql) && query.first()) {
            qDebug() << "ParseMusic::run()-> 歌曲已存在";
        } else {
            qint64 song_id = 1;
            query.prepare("SELECT MAX(song_id) FROM songs");
            if (query.exec() && query.first() && !query.value(0).isNull()) {
                song_id = query.value(0).toInt() + 1;
            }

            extractMetadata(song_id);
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
}

void ParseMusic::extractMetadata(qint64 song_id) {
    metadata["song_id"] = song_id;
    metadata["song_title"] = "unknown";
    metadata["song_artist"] = "unknown";
    metadata["song_album"] = "unknown";
    metadata["song_lyrics"] = "No lyrics";
    metadata["song_path"] = filepath;
    metadata["cover_path"] = "./res/cover_default.jpg";
    metadata["song_size"] = QFileInfo(filepath).size();
    metadata["song_duration"] = "unknown";

    QByteArray byteArray = filepath.toUtf8();
    AVFormatContext *formatContext = nullptr;
    if (avformat_open_input(&formatContext, byteArray.constData(), nullptr, nullptr) != 0) {
        qDebug() << "extractMetadata(qint64 song_id)" << "Failed to open FLAC file";
        return;
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        qDebug() << "extractMetadata(qint64 song_id)" << "Failed to read stream info";
        avformat_close_input(&formatContext);
        return;
    }

    AVDictionaryEntry *tag = nullptr;
    int64_t duration = formatContext->duration / AV_TIME_BASE;
    metadata["song_duration"] = duration;

    while ((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        QString key = QString::fromUtf8(tag->key).toLower();
        QString value = QString::fromUtf8(tag->value);
        // qDebug() << key << ":" << value;

        if (key == "title") metadata["song_name"] = value;
        else if (key == "artist") metadata["song_artist"] = value;
        else if (key == "album") metadata["song_album"] = value;
        else if (key.contains("lyrics")) metadata["song_lyrics"] = value;
    }


    avformat_close_input(&formatContext);
}

// bool ParseMusic::saveAlbumCover(AVFormatContext *formatContext, const QString &outputPath) {
//     for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
//         AVStream *stream = formatContext->streams[i];
//         if (stream->disposition & AV_DISPOSITION_ATTACHED_PIC) {
//             AVPacket *packet = &stream->attached_pic;
//             QFile file(outputPath);
//             if (file.open(QIODevice::WriteOnly)) {
//                 file.write((const char*)packet->data, packet->size);
//                 file.close();
//                 qDebug() << "saveAlbumCover(AVFormatContext *formatContext, const QString &outputPath)->" << "Album cover saved to" << outputPath;
//             } else {
//                 qDebug() << "saveAlbumCover(AVFormatContext *formatContext, const QString &outputPath)->" << "Failed to save album cover.";
//             }
//             return true;
//         }
//     }
//     qDebug() << "saveAlbumCover(AVFormatContext *formatContext, const QString &outputPath)->" << "No album cover found.";
//     return false;
// }
