#ifndef PARSEMUSIC_H
#define PARSEMUSIC_H

#include <QRunnable>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <qthread.h>
#include <QCoreApplication>


extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
}

class ParseMusic : public QRunnable
{
public:
    explicit ParseMusic(const QString& filepath);
    void run() override;
private:
    QString filepath;
    QMap<QString, QVariant> metadata;
    void extractMetadata(qint64 song_id);
    // bool saveAlbumCover(AVFormatContext *formatContext, const QString &outputPath);
};

#endif // PARSEMUSIC_H
