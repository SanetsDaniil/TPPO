#include "database.h"

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("real_estate.db");
    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return;   // без open дальше не идём
    }
     QSqlQuery query(db);
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "property_id INTEGER NOT NULL, "
        "tenant_id INTEGER, "
        "date DATE NOT NULL, "
        "type TEXT CHECK(type IN ('Продажа','Аренда')) NOT NULL, "
        "amount REAL NOT NULL, "
        "FOREIGN KEY(property_id) REFERENCES properties(id), "
        "FOREIGN KEY(tenant_id) REFERENCES tenants(id)"
        ")"
        );
     if (!ok) {
         qDebug() << "Ошибка создания таблицы transactions:" << query.lastError().text();
     }
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::openDatabase() {
    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}
