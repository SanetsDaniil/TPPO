#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>

struct Transaction {
    int id;
    int propertyId;
    int tenantId;       // для продажи можно оставить 0 или -1
    QDate date;
    QString type;       // "Продажа" или "Аренда"
    double amount;
    QString propertyName;
    QString tenantName;

    QDate leaseStart;
    QDate leaseEnd;
};

#endif // TRANSACTION_H
