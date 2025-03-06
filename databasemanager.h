#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include "mainframe.h"

class DatabaseManager {
public:
    explicit DatabaseManager(const QString& connectionName = "default_connection",
                             const QString& driver = "QSQLITE",
                             const QString& dbName = "./res/db/data.db");

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    DatabaseManager(DatabaseManager&& other) noexcept;

    DatabaseManager& operator=(DatabaseManager&& other) noexcept;

    QSqlDatabase& database() { return m_db; }

    void close();

    ~DatabaseManager() {
        close();
    }

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

#endif // DATABASEMANAGER_H
