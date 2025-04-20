#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool openDatabase();
    QSqlDatabase& getDatabase();
private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASE_H
