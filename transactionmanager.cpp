#include "transactionmanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

TransactionManager::TransactionManager() {
    db = DatabaseManager::instance().getDatabase();
}

bool TransactionManager::addTransaction(const Transaction &t) {
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO transactions "
        "(property_id, tenant_id, date, type, amount) "
        "VALUES (?, ?, ?, ?, ?)"
        );
    query.addBindValue(t.propertyId);
    // Если это продажа без арендатора, можно передать NULL:
    if (t.tenantId > 0)
        query.addBindValue(t.tenantId);
    else
        query.addBindValue(QVariant(QVariant::Int));
    query.addBindValue(t.date);
    query.addBindValue(t.type);
    query.addBindValue(t.amount);

    if (!query.exec()) {
        qDebug() << "Error adding transaction:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<Transaction> TransactionManager::getAllTransactions() {
    QVector<Transaction> list;
    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM transactions")) {
        qDebug() << "Error loading transactions:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        Transaction t;
        t.id         = query.value("id").toInt();
        t.propertyId = query.value("property_id").toInt();
        t.tenantId   = query.value("tenant_id").toInt();
        t.date       = query.value("date").toDate();
        t.type       = query.value("type").toString();
        t.amount     = query.value("amount").toDouble();
        list.append(t);
    }
    return list;
}
