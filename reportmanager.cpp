#include "reportmanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ReportManager::ReportManager() {}

QVector<Transaction> ReportManager::getRentalTransactions() {
    QVector<Transaction> rentals;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (!query.exec("SELECT * FROM transactions WHERE type = 'Аренда'")) {
        qDebug() << "Ошибка выборки аренды:" << query.lastError().text();
        return rentals;
    }
    while (query.next()) {
        Transaction t;
        t.id           = query.value("id").toInt();
        t.propertyId     = query.value("property_id").toInt();
        t.tenantId       = query.value("tenant_id").toInt();
        t.date           = query.value("date").toDate();
        t.type           = query.value("type").toString();
        t.amount         = query.value("amount").toDouble();
        t.leaseStart     = query.value("lease_start").toDate();
        t.leaseEnd       = query.value("lease_end").toDate();
        t.propertyName   = query.value("property_name").toString();
        t.tenantName     = query.value("tenant_name").toString();
        rentals.append(t);
    }
    return rentals;
}

QVector<Transaction> ReportManager::getSalesTransactions() {
    QVector<Transaction> sales;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (!query.exec("SELECT * FROM transactions WHERE type = 'Продажа'")) {
        qDebug() << "Ошибка выборки продаж:" << query.lastError().text();
        return sales;
    }
    while (query.next()) {
        Transaction t;
        t.id           = query.value("id").toInt();
        t.propertyId     = query.value("property_id").toInt();
        t.tenantId       = query.value("tenant_id").toInt();
        t.date           = query.value("date").toDate();
        t.type           = query.value("type").toString();
        t.amount         = query.value("amount").toDouble();
        // Для продаж leaseStart/leaseEnd могут быть пустыми, но читаем:
        t.leaseStart     = query.value("lease_start").toDate();
        t.leaseEnd       = query.value("lease_end").toDate();
        t.propertyName   = query.value("property_name").toString();
        t.tenantName     = query.value("tenant_name").toString();
        sales.append(t);
    }
    return sales;
}
