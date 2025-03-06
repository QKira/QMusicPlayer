#include "databasemanager.h"

DatabaseManager::DatabaseManager(const QString &connectionName, const QString &driver, const QString &dbName)
    : m_connectionName(connectionName)
{
    if (QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase(driver, connectionName);
        m_db.setDatabaseName(dbName);
    }

    if (!m_db.open()) {
        MainFrame::showMessageBoxError("Database connection failed: " + m_db.lastError().text());
    } else {
        qDebug() << "Database connected successfully.";
    }
}

DatabaseManager::DatabaseManager(DatabaseManager &&other) noexcept
    : m_db(other.m_db), m_connectionName(std::move(other.m_connectionName))
{
    other.m_db = QSqlDatabase();
}

DatabaseManager &DatabaseManager::operator=(DatabaseManager &&other) noexcept {
    if (this != &other) {
        close();
        m_db = other.m_db;
        m_connectionName = std::move(other.m_connectionName);
        other.m_db = QSqlDatabase();
    }
    return *this;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
        QSqlDatabase::removeDatabase(m_connectionName);
        qDebug() << "Database connection closed.";
    }
}
